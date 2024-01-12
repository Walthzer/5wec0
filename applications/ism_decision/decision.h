 /*
 *  TU/e 5WEC0::PYNQ RYB System Module Integration. Decision module header
 * 
 *  Written by: Walthzer
 * 
 */

#include <libpynq.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdatomic.h>
#include <libui.h>
#include <libcom.h>

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

#define MAX_SIZE 5
#define SIZE_ADJ 4

enum SYS_STATE {IDLE, MOVED, REVERTED, SKIP_CROSS, MANUAL};

//Inverse Model
typedef struct _sector_
{
  int row, column;
  int trg_stress;
} Sector;

typedef struct _data_
{
  //Freqentie is x;
  //Amplitude is y;
  Sector model[5][5];
  Sector *pos_sect, *prv_sect;
  int sys_strs;
  int sys_state;
  int idle_counter;
} Data;

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
      stress = atomic_load(&stress_atom);

      ui_rprintf(ui, 3, "%qBPM[60-240]: %q%d", RGB_BLUE, RGB_RED, bpm);
      ui_rprintf(ui, 4, "%qCrying[0-100]: %q%d", RGB_BLUE, RGB_RED, cry);
      ui_rprintf(ui, 5, "%qStress[0-100]: %q%d", RGB_BLUE, RGB_RED, stress);
      ui_rprintf(ui, 7, "%qFrequency[1-5]: %q%d", RGB_PURPLE, RGB_GREEN, freq);
      ui_rprintf(ui, 8, "%qAmplitude[1-5]: %q%d", RGB_PURPLE, RGB_GREEN, ampl);

      ui_rprintf(ui, 9, "%qMODE: %q%s", RGB_ORANGE, get_switch_state(0) ? RGB_RED : RGB_GREEN, get_switch_state(0) ? "Manual" : "Automatic");

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

//determine the stress level of the babys
int calculate_stress(int Heartbeat, int Crying)
{
  int stress;
  int bpm_stress = 100-(((240-Heartbeat)/20)*10);
  int cry_stress = ((Crying/25)+1)*10;
  if(cry_stress < 50 && cry_stress <= bpm_stress && Crying > -1)
  {
    stress = cry_stress;
  } else {stress = bpm_stress; };
  
  //fprintf(stderr, "Calculated Stress: %d -- Heart: %d -- Cry: %d\n", stress, Heartbeat, Crying);
  return stress;
}

void init_system(Data* data)
{
    //---Init Model---
  for (int idx_r = 0; idx_r < MAX_SIZE; idx_r++)
  {
    for(int idx_c = 0; idx_c < MAX_SIZE; idx_c++)
    {
      data->model[idx_r][idx_c] = (Sector){idx_r, idx_c, -1};
    }
  }
  //Top Right
  data->model[4][4].trg_stress = 200;
  data->pos_sect = &data->model[4][4];
  data->prv_sect = NULL;
  data->sys_strs = 100;
  data->sys_state = IDLE;
  data->idle_counter = 0;
  //---------

  atomic_store(&freq_atom, 5);
  atomic_store(&ampl_atom, 5);
  atomic_store(&cry_atom, 100);
  atomic_store(&bpm_atom, 60);
  atomic_store(&stress_atom, 100);

  sleep_msec(5000);
}

void setup(Data* data)
{
  //Wait until we get data from the sensors
  if(get_switch_state(0))
  {
    fprintf(stderr, "Not using Sensors...\n");
    int bpm;
    printf("Enter new heartrate: ");
    scanf(" %d", &bpm);
    data->sys_strs = calculate_stress(bpm, -1);
  } else 
  {
    fprintf(stderr, "Waiting on sensors...\n");
    while(get_switch_state(1) && atomic_load(&bpm_atom) < 200)
    {
      sleep_msec(50);
    }
    data->sys_strs = calculate_stress(atomic_load(&bpm_atom), atomic_load(&cry_atom));
    fprintf(stderr, "Sensors active, tn solving...\n");
  }
}


int get_adjacent(Data* data, Sector** adj_sectors, int row, int col)
{
  int found = 0;
  //Down
  if(row -1 > -1) {found++; adj_sectors[0] = &data->model[row-1][col]; } else {adj_sectors[0] = NULL; }
  //Left
  if(col - 1 > -1) {found++; adj_sectors[1] = &data->model[row][col-1]; }       else {adj_sectors[1] = NULL; }
  //Up -> Never go UP
  adj_sectors[2] = NULL;
  //if(row + 1 < MAX_SIZE) {found++; adj_sectors[2] = &data->model[row+1][col]; }        else {adj_sectors[2] = NULL; }
  //Right -> Never move right
  adj_sectors[3] = NULL;
  //if(col + 1 < MAX_SIZE) {found++; adj_sectors[3] = &data->model[row][col+1]; } else {adj_sectors[3] = NULL; }

  return found;
}

//Calculate microseconds period from rise and fall stamps
//Results is always positive
int signed_time_diff_micro(struct timeval *a, struct timeval *b)
{
  return (((a->tv_sec - b->tv_sec) * 1e+6) + (a->tv_usec - b->tv_usec));
}
int time_diff_micro(struct timeval *a, struct timeval *b) {

  int delta = signed_time_diff_micro(a, b);
  if (delta < 0)
    return -delta;
  return delta;
}

int has_elapsed_sec(struct timeval *a, struct timeval *b, int seconds)
{
  int delay_micro = seconds * 1000000;
  return (time_diff_micro(a, b) > delay_micro);
}

void wait_until_enter(void)
{
  /*printf("Press Enter To Continue");
  fflush(NULL);
  getchar();
  getchar();
  fprintf(stderr, "\033[F\033[2K.");*/
}

void wait_sec(int seconds)
{
  struct timeval ts, tn;
  gettimeofday(&ts, NULL);
  tn = ts;
  while(!has_elapsed_sec(&ts, &tn, seconds)) {gettimeofday(&tn, NULL); sched_yield(); };
}