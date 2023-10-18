 /*
 *  TU/e 5WEC0::WEEK 7 Assigment
 *
 *  Written by: Walthzer
 * 
 */
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define MAXROWS 10
typedef struct {
int rows, columns;
float *elements[MAXROWS];
} matrix_t;

void free_matrix_elements (matrix_t *ptr_m)
{
  for (int i = 0; i < MAXROWS; i++)
  {
    if(ptr_m->elements[i] != NULL)
    {
      free(ptr_m->elements[i]);
      ptr_m->elements[i] = NULL;
    }
  }
}

matrix_t new_matrix(int rows, int columns, char zeroOrRead)
{
  matrix_t m = { -1, -1, { NULL, } };
  if (rows > MAXROWS || rows < 1 || columns < 1)
  {
    printf("Rows must be 1 to 10, and columns must be at least 1\n");
    return m;
  }
  m.rows = rows;
  m.columns = columns;

  for (int idx_row = 0; idx_row < rows; idx_row++)
  {
    //Malloc float array of column size per row
    m.elements[idx_row] = (float*) malloc (columns*sizeof(float));
    
    if(zeroOrRead != '0')
      printf("Row %d: ", idx_row);

    for (int idx_column = 0; idx_column < columns; idx_column++)
    {
      float input_f = 0.0f;
      if(zeroOrRead != '0')
        scanf(" %f", &input_f);
      m.elements[idx_row][idx_column] = input_f;
    }
    
  }
  return m;
}

void print_maze(matrix_t m, char name)
{
  if(m.rows < 1)
  {
    printf("Matrix %c is empty\n", name);
    return;
  }

  printf("Matrix %c (%d X %d):\n", name, m.rows, m.columns);
  for (int idx_row = 0; idx_row < m.rows; idx_row++)
  {
    for (int idx_column = 0; idx_column < m.columns; idx_column++)
    {
      printf("%8.2f", m.elements[idx_row][idx_column]);
    }
    printf("\n");
  }
  
}

matrix_t copy_matrix(matrix_t m)
{
  //Initialize copy to size of m -> allocated the needed memory
  matrix_t copy = new_matrix(m.rows, m.columns, '0');

  //Copy float values into copy
  for (int idx_row = 0; idx_row < copy.rows; idx_row++)
  {

    for (int idx_column = 0; idx_column < copy.columns; idx_column++)
    {
      copy.elements[idx_row][idx_column] = m.elements[idx_row][idx_column];
    }
    
  }
  return copy;
}

//MATH operations
//Addition
void add_matrices(matrix_t ma, matrix_t mb)
{
  //Cannot add empty matrices
  if(ma.rows < 1 || mb.rows < 1)
  {
    printf("Matrices cannot be empty\n");
    return;
  }
  //Cannot add non-matched size matrices
  if(ma.rows != mb.rows || ma.columns != mb.columns)
  {
    printf("Matrix dimensions don't agree\n");
    return;
  }

  for (int idx_row = 0; idx_row < ma.rows; idx_row++)
  {
    for (int idx_column = 0; idx_column < ma.columns; idx_column++)
    {
      mb.elements[idx_row][idx_column] += ma.elements[idx_row][idx_column]; 
    }
  }
}

//Transpose
void transpose_matrix(matrix_t *ptr_m)
{
  if(ptr_m->rows < 1)
  {
    printf("Matrix cannot be empty\n");
    return;
  }

  matrix_t transpose = new_matrix(ptr_m->columns, ptr_m->rows, '0');

  for (int idx_row = 0; idx_row < ptr_m->rows; idx_row++)
  {
    for (int idx_column = 0; idx_column < ptr_m->columns; idx_column++)
    {
      transpose.elements[idx_column][idx_row] = ptr_m->elements[idx_row][idx_column];
    }
  }
  //Free memory and assign transposed matrix
  free_matrix_elements(ptr_m);
  *ptr_m = transpose;
}

void multiply_matrices(matrix_t ma, matrix_t mb, matrix_t *ptr_mc)
{
  //Cannot multiply empty matrices
  if(ma.rows < 1 || mb.rows < 1)
  {
    printf("Matrices cannot be empty\n");
    return;
  }
  //Cannot multiply non-matched size matrices
  if(ma.rows != mb.columns || ma.rows != mb.columns)
  {
    printf("Matrix dimensions don't agree\n");
    return;
  }
  //Make sure mc doesn't leak memory
  free_matrix_elements(ptr_mc);
  *ptr_mc = new_matrix(ma.rows, mb.columns, '0');

  for (int idx_row = 0; idx_row < ptr_mc->rows; idx_row++)
  {
    for (int idx_column = 0; idx_column < ptr_mc->columns; idx_column++)
    {
      //at coordinate [D, E] in result matrix
      //Multiply every A[D, i] with B[i, E] and sum them together
      //Clamp to the last element if we have mismatched matrices -> NOT NEEDED

      for (int i = 0; i < ma.columns; i++)
      {
        ptr_mc->elements[idx_row][idx_column] += ma.elements[idx_row][i] * mb.elements[i][idx_column];
      }
    }
  }
  
}

matrix_t minor_matrix(matrix_t *ptr_m, int row, int column)
{
  matrix_t minor = {-1, -1, {NULL}};
  //Matrix not atleast 2*2
  if(ptr_m->rows < 2 || ptr_m->columns < 2)
  {
    printf("Matrix must have at least two rows & columns\n");
    return minor;
  }
  //row or column out-of-bound
  if(ptr_m->rows <= row || ptr_m->columns <= column)
  {
    printf("Row & column must be 0 up to %d & %d, respectively\n", ptr_m->rows, ptr_m->columns);
    return minor;
  }

  minor = new_matrix((ptr_m->rows)-1, (ptr_m->columns)-1, '0');

  int m_row = 0;
  for (int idx_row = 0; idx_row < ptr_m->rows; idx_row++)
  {
    if(idx_row == row)
      continue;

    int m_column = 0; //Dumbass
    for (int idx_column = 0; idx_column < ptr_m->columns; idx_column++)
    {
      if(idx_column == column)
        continue;
      
      //printf("\nMinor row: %d  Minor column: %d", m_row, m_column);
      //printf("\nrow: %d  column: %d", idx_row, idx_column);
      minor.elements[m_row][m_column] = ptr_m->elements[idx_row][idx_column];
      m_column++;
    }
    m_row++;
  }
  return minor;
}

float determinant (matrix_t *ptr_m)
{
    if(ptr_m->rows < 1)
    {
        printf("Matrix cannot be empty\n");
        return 0.0f;
    }
    
    //Matrix muse be square
    if(ptr_m->rows != ptr_m->columns)
    {
        printf("Matrix must be square\n");
        return 0.0f;
    }
    
    float det = 0.0f;
    
    if (ptr_m->rows == 1) //1*1 Matrix
        det = ptr_m->elements[0][0];
    else if (ptr_m->rows == 2) //2*2 Matrix
        det = ptr_m->elements[0][0]*ptr_m->elements[1][1] - ptr_m->elements[0][1]*ptr_m->elements[1][0];
    else
    {
      //N*N matrix
      const int r = 0;
      for (int c=0; c < ptr_m->columns; c++) {
        matrix_t minor = minor_matrix(ptr_m, r, c);
        int sign = ((r+c) % 2 == 0 ? 1 : -1); // (-1)^(r+c)
        det += sign * ptr_m->elements[r][c]*determinant(&minor);
        free_matrix_elements(&minor);
      }
    }
    if(det == -0.0f)
        printf ("%1.2f:", 0.0f);
    else
        printf ("%1.2f:", det);
    return det;
}

int main(void) {

  //Initialize
  char cmd = '\0';
  matrix_t ma, bc, mc;
  ma = bc = mc = (matrix_t){ -1, -1, { NULL, } };

  int inputRows, inputColumns = -1;
  do
  {
    printf("Command? ");
    scanf(" %c",&cmd);
    switch (cmd) 
    {
      case 'q':
        break;

      case 'A':
      case '0':
        printf("Size of matrix A (rows columns)? ");
        scanf(" %d %d", &inputRows, &inputColumns);
        free_matrix_elements(&ma);
        ma = new_matrix(inputRows, inputColumns, cmd);
      break;

      case 'B':
        free_matrix_elements(&bc);
        bc = copy_matrix(ma);
      break;

      case 'c':
        print_maze(mc, 'C');
      break;
      case 'b':
        print_maze(bc, 'B');
      break;
      case 'a':
        print_maze(ma, 'A');
      break;

      //MATH operations
      //Addition
      case '+':
        add_matrices(ma, bc);
      break;
      //Transpose
      case 't':
        transpose_matrix(&ma);
      break;
      //Multiply
      case '*':
        multiply_matrices(ma, bc, &mc);
      break;
      //Minor
      case 'm':
        printf("Remove which row & column? ");
        scanf(" %d %d", &inputRows, &inputColumns);
        free_matrix_elements(&mc);
        mc = minor_matrix(&ma, inputRows, inputColumns);
      break;
      //Determinat
      case 'd':
        float det = determinant(&ma);
        if (det != 0.0f)
          printf("\nThe determinant is %f\n", det);
      break;

      default:
        printf("Unknown command \'%c\'\n", cmd);
    }
  
  }while (cmd != 'q');

  free_matrix_elements(&ma);
  free_matrix_elements(&bc);
  free_matrix_elements(&mc);

  printf("Bye!\n");

  return 0;
}