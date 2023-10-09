 /*
 *  TU/e 5WEC0::WEEK 6 Assigment
 *
 *  Written by: Walthzer
 * 
 */
#include <stdio.h>

//Makefile modifications handle this
#ifndef MANJAROO
  #include <libpynq.h>
#else
  #include <display.h>
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
  char grid[DISPLAY_HEIGHT][DISPLAY_WIDTH];
  coordinate_t size;
} maze_t;
//Get X from Size in Passed Pointer to maze_t struct
#define SIZE_X(maze) maze->size.x
//Get Y from Size in Passed Pointer to maze_t struct
#define SIZE_Y(maze) maze->size.y
//Get the section of the maze at X and Y
#define SEC(maze,x,y) maze->grid[y][x]

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

  PROMPT_INT("Number of rows? ", &ptr_maze_s->y);
  PROMPT_INT("Number of colums? ", &ptr_maze_s->x);

  //Prevent incorrect maze size
  if(ptr_maze_s->x < 2 || ptr_maze_s->y < 2)
  {
    printf("The number of rows and columns must be at least two\n");
    *ptr_maze_s = makeCoord(-1, -1);
    return;
  }

  int nrStarts = 0;

  //Rows are presented by prompts:
  for (int idx_y = 0; idx_y < ptr_maze_s->y; idx_y++)
  {
    char* maze_row_str = ptr_maze->grid[idx_y];

    //Prompt for and store row data:
    printf("Input row %2d: ", idx_y);
    scanf(" %s", maze_row_str);

    //Remove extra starting points from the row
    for (int idx_x = 0; idx_x < ptr_maze_s->x; idx_x++)
    {
      if (maze_row_str[idx_x] == START)
      {
        if (nrStarts > 0)
        {
          maze_row_str[idx_x] = WALL;
          printf("Dropping extra starting point\n");
        }
        nrStarts++;
      }
    }
  }
}
//Output the maze to stdout
void print_maze(maze_t* maze)
{
  //Do nothing if maze is empty
  if (maze->size.x == -1)
    return;

  for (int idx_y = 0; idx_y < maze->size.y; idx_y++)
  {
    printf("%s\n", maze->grid[idx_y]);
  } 
}
//Give the colour a maze section should be on the display
int resolve_colour(char piece)
{
  switch (piece) {
    case WALL: return RGB_RED;
    case PATH: return RGB_YELLOW;
    case START: return RGB_BLUE;
    case DESTINATION: return RGB_GREEN;
    case VISITED: return 100; //rgb_conv(100,100,100);
  }
  return RGB_BLACK;
}
//Primary draw call to write to display -> reports its operation to stdout (Making the HW less fun by the Week Prof. Goossens...)
void draw_section(display_t* display, coordinate_t upper_l, coordinate_t lower_r, int colour)
{
  #ifdef PYNQLIB_H
  displayDrawFillRect(display, upper_l.x, upper_l.y, lower_r.x, lower_r.y, colour);
  #endif
  printf("dfr %d %d %d %d %d\n", upper_l.x, upper_l.y, lower_r.x, lower_r.y, colour);
}
//Display the maze onto the PYNQ display
void display_maze(display_t* display, maze_t* ptr_maze)
{
  //Do nothing if maze is empty
  if (SIZE_X(ptr_maze) == -1)
    return;

  //Setup variables for the draw operations
  int colour = 0;
  //Scaling factors
  int scale_x = DISPLAY_WIDTH / SIZE_X(ptr_maze);
  int scale_y = DISPLAY_HEIGHT / SIZE_Y(ptr_maze);

  //Clear Display
  #ifdef PYNQLIB_H
  displayFillScreen(display, RGB_BLACK);
  #endif

  //Row by Row drawing of the maze
  for (int idx_y = 0; idx_y < SIZE_Y(ptr_maze); idx_y++)
  {
    for (int idx_x = 0; idx_x < SIZE_X(ptr_maze); idx_x++)
    {
      colour = resolve_colour(SEC(ptr_maze,idx_x,idx_y));

      //Create the drawing coordinates 
      coordinate_t upper_l = makeCoord( idx_x*scale_x, 
                                        idx_y*scale_y);
                                        
      coordinate_t lower_r = makeCoord( (idx_x + 1)*scale_x - (1),
                                        (idx_y + 1)*scale_y - (1));
      draw_section(display, upper_l, lower_r, colour);
    } 
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

      case 'p':
        print_maze(&maze);
      break;

      case 'd':
        display_maze(display, &maze);
      break;

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
    display_set_flip(&display, true, true); //Display is mounted flipped on y-axis
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