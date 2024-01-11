#include <libpynq.h>
#include <stdlib.h>
#include <libui.h>
#include <libcom.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdatomic.h>

#include <decision.h>

static Data data;
atomic_uint_fast16_t freq_atom;
atomic_uint_fast16_t ampl_atom;
atomic_int stress_atom;
atomic_int output_kalive;
atomic_int cry_atom;
atomic_int bpm_atom;

void* fnc_ui_thread (void* arg)
{
    ui_t* ui = arg;
    //outputs
    struct timespec t_sleep = {0, 10000};
    uint16_t freq, ampl, cry, bpm, stress;
    freq = ampl = cry = bpm = stress = -1;

    while(atomic_load(&output_kalive))
    {
      if( atomic_load(&freq_atom) == freq && atomic_load(&ampl_atom) == ampl
          && atomic_load(&cry_atom) == cry && atomic_load(&bpm_atom) == bpm
          && atomic_load(&stress_atom) == stress)
      {
        nanosleep(&t_sleep, NULL); //sleep in microseconds
        continue;
      }
      freq = atomic_load(&freq_atom);
      ampl = atomic_load(&ampl_atom);
      cry = atomic_load(&cry_atom);
      bpm = atomic_load(&bpm_atom);

      ui_rprintf(ui, 3, "%qBPM[60-240]: %q%d", RGB_BLUE, RGB_RED, bpm);
      ui_rprintf(ui, 4, "%qCrying[0-100]: %q%d", RGB_BLUE, RGB_RED, cry);
      ui_rprintf(ui, 5, "%qStress[0-100]: %q%d", RGB_BLUE, RGB_RED, stress);
      ui_rprintf(ui, 7, "%qFrequency[1-5]: %q%d", RGB_PURPLE, RGB_GREEN, freq);
      ui_rprintf(ui, 8, "%qAmplitude[1-5]: %q%d", RGB_PURPLE, RGB_GREEN, ampl);

      ui_draw(ui);
    }
    return NULL;
}
void* fnc_com_thread (void* arg)
{
    com_t* com = arg;
    //outputs
    uint32_t bpm, cry;

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
        if (cry < 150 )
        {
          bpm = MAX(cry, 0);
          bpm = MIN(cry, 100);

          atomic_store(&cry_atom, cry);
        }
      }  
    }
    return NULL;
}

void move_to(Data *data, Sector *sector)
{
  fprintf(stderr, "\33[35mMoving into f: %d--A: %d\n", sector->column+1, sector->row+1);
  fprintf(stderr, "\33[37m");

  //Update Position in data
  data->prv_sect = data->pos_sect;
  data->pos_sect = sector;
  //Push to the I/O threads
  atomic_store(&freq_atom, sector->column+1);
  atomic_store(&ampl_atom, sector->row+1);
  //Signal that we should start waiting
  data->sys_state = MOVED;
}

void move_revert(Data *data)
{
  fprintf(stderr, "\33[33mMoving back to f: %d--A: %d\n", data->prv_sect->column+1, data->prv_sect->row+1);
  fprintf(stderr, "\33[37m");
  
  //Push to the I/O threads
  atomic_store(&freq_atom, data->prv_sect->column+1);
  atomic_store(&ampl_atom, data->prv_sect->row+1);

  //Update Position in data
  data->pos_sect = data->prv_sect;
  data->prv_sect = NULL;
  
  //Signal that we should start waiting
  data->sys_state = REVERTED;
}

int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);

  //Set header
  ui_rcenter(&ui, 0, true);
  ui_rprintf( &ui, 0, "%q%w%tRYB 24", RGB_WHITE, RGB_ORANGE, COMIC);
  ui_rcenter(&ui, 1, true);
  ui_rprintf( &ui, 1, "%q%w%tDecision Making", RGB_WHITE, RGB_ORANGE, COMIC);
  //interface layout
  //Inputs: Heartbeat+Crying
  ui_rprintf( &ui, 2, "%q--Inputs--", RGB_CYAN);
  ui_rcenter(&ui, 2, true);
  //Outputs: Heartbeat+Crying
  ui_rprintf( &ui, 6, "%q--Outputs--", RGB_CYAN);
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

  //---Init Model---
  for (int idx_r = 0; idx_r < MAX_SIZE; idx_r++)
  {
    for(int idx_c = 0; idx_c < MAX_SIZE; idx_c++)
    {
      data.model[idx_r][idx_c] = (Sector){idx_r, idx_c, -1};
    }
  }
  //Top Right
  data.model[4][4].trg_stress = 200;
  data.pos_sect = &data.model[4][4];
  data.prv_sect = NULL;
  data.int_bpm = data.int_cry = -1;
  data.sys_strs = 100;
  data.sys_state = IDLE;
  //---------

  //Init inputs
  switches_init();
  buttons_init();

  //Wait until we get data from the sensors
  while(!get_switch_state(0) || (get_switch_state(1) && /*atomic_load(&cry_atom) != -1 &&*/ atomic_load(&bpm_atom) != 60) )
  {
    sleep_msec(50);
  }
  if(get_switch_state(0))
  {
    fprintf(stderr, "Not using Sensors...\n");
    int bpm;
    printf("Enter new heartrate: ");
    scanf(" %d", &bpm);
    data.sys_strs = calculate_stress(bpm, atomic_load(&cry_atom));
  } else 
  {
    fprintf(stderr, "Sensors active, now solving...\n");
  }
 
  //We start at model 5-5
  /*
    We have a preference of movement
    -Down
    -Left
    -Up
    -Right

    Every Sector we have visited is recorded.
    Skip an entry in movement preference if
      -> its target stress is more then 1 step removed from current stress
  */

  /*
    After moving we observe the stress of the baby
    and store this into the sectors

    -> If the stress rises -> return to previous sector
    -> if it lowers, continue the pathing
    -> panic jump => Store a target stress of two block lower then current stress into sector
       Reset the path to 5/5

  */

  Sector* adj_sectors[SIZE_ADJ] = {NULL};
  struct timeval start;
  struct timeval now;

  while (get_switch_state(1))
  { 
    //Wait for results if we just moved:
    int stress = data.sys_strs;
    gettimeofday(&start, NULL);
    now = start;
    //Continue if
    // Stress has changed
    // 30 seconds have elapsed
    fprintf(stderr, "waiting...\n");
    while(data.sys_state != IDLE && data.sys_strs == stress && !has_elapsed_sec(&start, &now, 30))
    {
      int bpm;
      printf("Enter new heartrate: ");
      scanf(" %d", &bpm);
      stress = calculate_stress(bpm, atomic_load(&cry_atom));
      break;
      gettimeofday(&now, NULL);
      sleep_msec(50); //Prevent Us from blocking the I/O threads
    }
    //Debug text
    fprintf(stderr, "Complete: ");
    if(data.sys_state == IDLE)                 {fprintf(stderr, "Sys hasn't moved!\n"); }
    else if(data.sys_strs != stress)           {fprintf(stderr, "Stress change: %d => %d\n", data.sys_strs, stress); }
    else if(has_elapsed_sec(&start, &now, 30)) {fprintf(stderr, "Timeout after 30 seconds!\n"); }
    else {fprintf(stderr, "NO IDEA????\n"); }

    //Clear the move flag
    data.sys_state = IDLE;

    ///////////////////////////
    //Observe the new stress and act
    ///////////////////////////
    
    //If panic, restart
    if(stress >= 100)
    {
      //Stress is now 100
      data.sys_strs = 100;
      fprintf(stderr, "\33[31mPanic has occured, restarting => ");
      fprintf(stderr, "\33[37m");
      //Set 2 levels lower of previous sys_stress into sector as target
      //To prevent a panic loop
      data.pos_sect->trg_stress = data.sys_strs-20;
      move_to(&data, &data.model[4][4]);
      continue; //we have to go back to wait for the result of the move
    }

    //Set this stress into the sector as its target stress
    data.pos_sect->trg_stress = stress;

    //If stress went up or is equal -> go back
    if(data.sys_strs <= stress && data.prv_sect != NULL)
    {
      fprintf(stderr, "\33[93mStress went up, %d => %d;", data.sys_strs, stress);
      fprintf(stderr, "\33[37m");
      move_revert(&data);
      continue; //we have to go back to wait for the result of the move
    }
    fprintf(stderr, "\33[32mStress went down %d => %d;\n", data.sys_strs, stress);
    fprintf(stderr, "\33[37m");

    data.sys_strs = stress;
    ///////////////////////////
    //Action 1 -> Find a known sector adjacent to our current position.
    //and try to move into it
    ///////////////////////////

    wait_until_enter();

    //Get our adjacent sectors
    get_adjacent(&data, adj_sectors, data.pos_sect->row, data.pos_sect->column);
    //fprintf(stderr, "Sectors found: %d\n", scs_fnd);

    //Look at all adjacent sectors
    Sector* prf_knw_sector = NULL;
    Sector* prf_new_sector = NULL;
    for (int i = 0; i < SIZE_ADJ; i++)
    {
      //Null means sector is OOB 
      if(adj_sectors[i] == NULL) 
      {
        printf("NULL\n");
        continue;
      }
      printf("X: %d; Y:%d\n", adj_sectors[i]->row, adj_sectors[i]->column);

      //Is this an unknown sector?
      //Only last resort
      if(adj_sectors[i]->trg_stress == -1)
      {
        if(prf_new_sector == NULL)
        {
          prf_new_sector = adj_sectors[i];
        } else
        {
          continue;
        }
      }

      //Are we allowed to move in?
      //sec trgt stress is less then or equal to sys stress
      //sec trgt stress is not more then 1 step removed from sys stress
      if(adj_sectors[i]->trg_stress < data.sys_strs && (data.sys_strs-20) < adj_sectors[i]->trg_stress)
      {
        prf_knw_sector = adj_sectors[i];
        break;
      }
    }

    if(prf_knw_sector != NULL)
    {
      move_to(&data, prf_knw_sector);
      continue; //we have to go back to wait for the result of the move
    } else if (prf_new_sector != NULL)
    {
      move_to(&data, prf_new_sector);
      continue; //we have to go back to wait for the result of the move
    }
    
    {
      fprintf(stderr, "No move command?\n");
    }
  }
  //threading 
  //gracefull exit from thread;
  atomic_store(&output_kalive, false);
  pthread_join(ui_thread , NULL);
  pthread_join(com_thread , NULL);

  ui_rprintf( &ui, 1, "%q%w%tDEACTIVATED", RGB_RED, RGB_WHITE, NORMAL);
  ui_draw(&ui);

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};