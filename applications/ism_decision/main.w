#include <libpynq.h>
#include <stdlib.h>
#include <libui.h>
#include <libcom.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_uint_fast16_t freq_atom;
atomic_uint_fast16_t ampl_atom;
atomic_int stress_atom;
atomic_int output_kalive;
atomic_int cry_atom;
atomic_int bpm_atom;

//determine the period in ms
float period_ms(clock_t a, clock_t b)
{
  return ((a > b ? a - b : b - a) / CLOCKS_PER_SEC) * 1000;
}

//determine the stress level of the babys
int calculate_stress(int Heartbeat, int Crying)
{
  int bpm_stress = 100-(((240-Heartbeat)/20)*10);
  int curr_stress = ((Crying/25)+1)*10;
  if(curr_stress < 50 && curr_stress <= bpm_stress)
  {
    return curr_stress;
  }
  return bpm_stress;
}

//determine wich direction to move
void move(int*a, int*f, char direction){
  switch (direction)
  {
  case 'd':
    (*a) = (*a)-1;
    break;
  case 'l':
    (*f) = (*f)-1;
    break;
  case 'r':
    (*f) = (*f)+1;
    break;
  case 'u':
    (*a) = (*a)+1;
    break;
  default:
    (*a) = 5;
    (*f)= 5;
    break;
  }
}

void move_back(int*a, int*f, char direction){
  switch (direction)
  {
  case 'd':
    (*a) = (*a)+1;
    break;
  case 'l':
    (*f) = (*f)+1;
    break;
  case 'r':
    (*f) = (*f)-1;
    break;
  case 'u':
    (*a) = (*a)-1;
    break;
  default:
    (*a) = 5;
    (*f)= 5;
    break;
  }
}

//send instructions where to move
void send(int ampl, int freq){
  atomic_store(&freq_atom, freq);
  atomic_store(&ampl_atom, ampl);
}

//move back to the position that you were before you activated a panic block
int r_prev(char prev_path[], com_t com){
  
  //move to the start
  int a, f, curr_stress , p_stress;
  a = f = 5;
  curr_stress  = p_stress = 100;
  move(&a,&f, '0');
  send(a,f);
  for(int i = 0; prev_path[i] != '\0'; i++){
    while(curr_stress  == p_stress){
      uint32_t data = 0;
      com_get(&com, HEARTBEAT, &data);
      int heartbeat = data;
      com_get(&com, CRYING, &data);
      int crying = data;
      curr_stress  = calculate_stress(heartbeat, crying);
    }
    move(&a,&f, prev_path[i]);
    send(a,f);
  }
  return 1;
}

int path(int p_stress, char direction, char prev_path[], com_t com, ui_t ui, int heartbeat, int crying, int a, int f){
  int curr_stress  = p_stress;
  clock_t start = clock();

  //move in the given direction
  move(&a, &f, direction);
  send(a, f);

  // wait thirty seconds or until you see a curr_stress 
  clock_t now = clock();
  while(curr_stress  == p_stress && period_ms(now, start) < 30000){
    curr_stress = calculate_stress(atomic_load(&bpm_atom), atomic_load(&cry_atom)); 
  }
  
  //if the stress level is below 10 we cleared the maze
  if(curr_stress  < 10) { return 1; }
  
  //if the stress level went down add the move into the patharray to save this move
  if(curr_stress  < p_stress){
    if(prev_path[0] == '\0'){
      prev_path[0] = direction;
    }
    for(int i = 0; prev_path[i] != '\0'; i++){
      if(prev_path[i] == '\0'){
        prev_path[i] = direction;
      }
    }
  }

  //if the stress level is 100 we need to go back to the start and follow the previous path until we are back in the space before this.
  if(curr_stress  == 100){
    int completed = r_prev(prev_path, com);
    if(completed != 1){
      printf("error");
    }
    return -1;
  }

  //if the stress level stays the same  or is higher we need to go back to the previous place
  if(curr_stress  >= p_stress){
    move_back(&a,&f, direction);
    send(a,f);
    return -1;
  }

  // set valid to 0
  int valid = 0;

  // if the stress level decreases and you did not go a up in your previous move go a place down
  if(direction == 'u' || a == 0){valid = -1;}
  if(curr_stress  < p_stress && direction != 'u' && a != 0){
    valid = path(curr_stress , 'd', prev_path, com, ui, heartbeat, crying, a, f);
  }

  // if down was not the solution and you did not go to the right on your previous move go to the left
  if(curr_stress  < p_stress && valid == -1 && direction != 'r' && f != 0){
    valid = path(curr_stress , 'l', prev_path, com, ui, heartbeat, crying, a, f);
  }

  //if left was also not the solution try up or right depending on wich move you previously made
  if(curr_stress  < p_stress && valid == -1){
    if(direction == 'd' && f != 5){
      valid = path(curr_stress , 'u', prev_path, com, ui, heartbeat, crying, a, f);
    }
    if(direction == 'l' && a != 5){
      valid = path(curr_stress , 'r', prev_path, com, ui, heartbeat, crying, a, f);
    }
  }
  
  //if we found the solution return to the start;
  if(valid == 1){
    return 1;
  }

  //if this was the solution return 1 as a way to say that this is the right move
  return valid;
}

void* fnc_ui_thread (void* arg)
{
    ui_t* ui = arg;

    //outputs
    struct timespec t_sleep = {0, 10000};
    uint16_t freq, ampl, cry, bpm, stress;

    while(atomic_load(&output_kalive))
    {
      uint32_t new_bpm = atomic_load(&bpm_atom);
      if( atomic_load(&freq_atom) == freq && atomic_load(&ampl_atom) == ampl
          && atomic_load(&cry_atom) == cry && atomic_load(&bpm_atom) == bpm
          && atomic_load(&stress_atom) == stress
        )
      {
        nanosleep(&t_sleep, NULL); //sleep in microseconds
        continue;
      }
      freq = atomic_load(&freq_atom);
      ampl = atomic_load(&ampl_atom);
      cry = atomic_load(&cry_atom);
      bpm = atomic_load(&bpm_atom);

      ui_rprintf(&ui, 3, "%qBPM[60-240]: %q%d", RGB_BLUE, RGB_RED, bpm);
      ui_rprintf(&ui, 4, "%qCrying[0-100]: %q%d", RGB_BLUE, RGB_RED, cry);
      ui_rprintf(&ui, 5, "%qStress[0-100]: %q%d", RGB_BLUE, RGB_RED, stress);
      ui_rprintf(&ui, 7, "%qFrequency[1-5]: %q%d", RGB_PURPLE, RGB_GREEN, freq);
      ui_rprintf(&ui, 8, "%qAmplitude[1-5]: %q%d", RGB_PURPLE, RGB_GREEN, ampl);

      ui_draw(ui);
    }
    return NULL;
}
void* fnc_com_thread (void* arg)
{
    com_t* com = arg;

    //outputs
    int bpm, cry;

    while(atomic_load(&output_kalive))
    {
      com_putm(com, MOTOR, (uint16_t)atomic_load(&freq_atom), (uint16_t)atomic_load(&ampl_atom));

      com_run(com);

      if(com_get(com, HEARTBEAT, &bpm))
      {
        if (bpm < 300 || bpm > 0)
        {
          bpm = MAX(bpm, 60);
          bpm = MIN(bpm, 240);

          atomic_store(&bpm_atom, bpm);
        }
      }
      if(com_get(com, CRYING, &cry))
      {
        if (cry < 150 || cry >= 0)
        {
          bpm = MAX(cry, 0);
          bpm = MIN(cry, 100);

          atomic_store(&cry_atom, cry);
        }
      }  
    }
    return NULL;
}

int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);

  //Set header
  ui_rcenter(&ui, 0, true);
  ui_rprintf(&ui, 0, "%q%w%tRYB 24", RGB_WHITE, RGB_ORANGE, COMIC);
  ui_rcenter(&ui, 1, true);
  ui_rprintf(&ui, 1, "%q%w%tDecision Making", RGB_WHITE, RGB_ORANGE, COMIC);
  //interface layout
  //Inputs: Heartbeat+Crying
  ui_rprintf(&ui, 2, "%q--Inputs--", RGB_CYAN);
  ui_rcenter(&ui, 2, true);
  //Outputs: Heartbeat+Crying
  ui_rprintf(&ui, 6, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 6, true);
  ui_draw(&ui);

  com_t com;
  com_init(&com, DECISION);
  com_putm(&com, MOTOR, 5, 5);

  //THREADING
  atomic_init(&freq_atom, 5);
  atomic_init(&ampl_atom, 5);
  atomic_init(&output_kalive, true);
  atomic_init(&cry_atom, -1);
  atomic_init(&bpm_atom, 60);
  atomic_init(&stress_atom, 100);
  pthread_t ui_thread;
  pthread_t com_thread;
  pthread_create(&ui_thread , NULL, &fnc_ui_thread , &ui);
  pthread_create(&com_thread , NULL, &fnc_com_thread , &com);

  //Init inputs
  switches_init();
  buttons_init();
  char prev_path[255] = {'\0'};

  int heartbeat = -1;
  int crying = -1;
  int a, f;
  a = f = 5;

  //Wait until we get data from the sensors
  while(get_switch_state(1) && /*atomic_load(&cry_atom) != -1 &&*/ atomic_load(&bpm_atom) != 60)
  {
    sleep_msec(50);
  }
  printf("Sensors active, now solving...");
 
  while (get_switch_state(1))
  {
    int valid = path(100, 'd', prev_path, com, ui, heartbeat, crying, a, f);
    if(valid == 1){
      ui_rprintf(&ui, 9, "%q--TEST SUCCES--", RGB_GREEN);
      ui_rcenter(&ui, 9, true);
    }
    else{
      ui_rprintf(&ui, 9, "%q--TEST FAIL--", RGB_RED);
      ui_rcenter(&ui, 9, true);
    }
  }
  //threading 
  //gracefull exit from thread;
  atomic_store(&output_kalive, false);
  pthread_join(ui_thread , NULL);
  pthread_join(com_thread , NULL);

  ui_rprintf(&ui, 1, "%q%w%tDEACTIVATED", RGB_RED, RGB_WHITE, NORMAL);
  ui_draw(&ui);

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};