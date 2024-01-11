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

enum SYS_STATE {IDLE, MOVED, REVERTED};

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
  int int_bpm, int_cry;
  int sys_strs;
  int sys_state;
} Data;

int get_adjacent(Data* data, Sector** adj_sectors, int row, int col)
{
  int found = 0;
  //Down
  if(row -1 > -1) {found++; adj_sectors[0] = &data->model[row-1][col]; } else {adj_sectors[0] = NULL; }
  //Left
  if(col - 1 > -1) {found++; adj_sectors[1] = &data->model[row][col-1]; }       else {adj_sectors[1] = NULL; }
  //Up
  if(row + 1 < MAX_SIZE) {found++; adj_sectors[2] = &data->model[row+1][col]; }        else {adj_sectors[2] = NULL; }
  //Right -> For now, never move right
  adj_sectors[3] = NULL;
  //if(col + 1 < MAX_SIZE) {found++; adj_sectors[3] = &data->model[row][col+1]; } else {adj_sectors[3] = NULL; }

  return found;
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
  
  fprintf(stderr, "Calculated Stress: %d -- Heart: %d -- Cry: %d\n", stress, Heartbeat, Crying);
  return stress;
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