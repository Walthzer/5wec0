#include <libpynq.h>
#include <stdio.h>

#ifndef PYNQLIB_H
  #define DISPLAY_WIDTH 240
  typedef void* display_t;
#endif

#define TRUE 1
#define FALSE 0

#define PRNT_CMD printf("Command? ")
#define PROMPT_INT(prompt, ptr) printf(prompt); scanf(" %d", ptr)
#define PROMPT_STR(prompt, ptr) printf(prompt); scanf(" %s", ptr)

//GIVEN DEFINES
#define START       'S' // starting position
#define DESTINATION 'D' // destination
#define WALL        '#' // wall
#define NOT_VISITED '.' // corridor
#define PATH        '+' // part of the current path
#define VISITED     '~' // corridor; part of a past path

//TYPES
//Coordinate Type -> Tuple
typedef struct
{
  int x, y;
} coordinate_t;

//Primary Maze type with the grid and size.
typedef struct
{
  char grid[DISPLAY_WIDTH][DISPLAY_HEIGHT];
  coordinate_t size;
} maze_t;

//TYPE FUNCTIONS
//Coordinate creation function
coordinate_t makeCoord(int i_x, int i_y)
{
  coordinate_t coord;
  coord.x = i_x;
  coord.y = i_y;
  return coord;
}

//LOGIC FUNCTIONS

//Build the maze from prompts
void input_maze(maze_t* ptr_maze)
{
  coordinate_t* ptr_maze_s = &ptr_maze->size;

  PROMPT_INT("Number of rows? ", &ptr_maze_s->x);
  PROMPT_INT("Number of colums? ", &ptr_maze_s->y);

  for (int i = 0; i < ptr_maze_s->y; i++)
  {
    printf("Input row %d")
  }
  
}

void do_maze(display_t *display)
{
//Initialize
  char cmd = '\0';
  maze_t maze;
  maze.size = makeCoord(-1, -1);
  
  PRNT_CMD;
  while (TRUE)
  {

    scanf(" %c",&cmd);
    switch (cmd) 
    {
      case 'q':
        return;

      case 'i':
        input_maze(&maze);
      break;
      default:
        printf("Unknown command \'%c\'\n", cmd);
    }
    PRNT_CMD;
  }
}

int main(void) {
  display_t display;
  #ifdef PYNQLIB_H
    pynq_init();
    display_init(&display);
    display_set_flip(&display, true, false); //Display is mounted flipped on y-axis
    displayFillScreen(&display, RGB_BLACK);
  #endif

  //Initialize
  do_maze(&display);

  printf("Bye!\n");

  #ifdef PYNQLIB_H
    display_destroy(&display);
    pynq_destroy();
  #endif

  return 0;
}