#include <stdlib.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

typedef struct
{
  float real;
  float im;
} complex;

void print_complex(complex *data)
{
  if (data->im >= 0)
    printf("Current value is %f+%fi\n", data->real, data->im);
  else
    printf("Current value is %f%fi\n", data->real, data->im);
}

void print_operation(void)
{
  printf("Operation [0+-*crpq]? ");
}

void print_plane(complex *data)
{
  for (int i = 10; i >= -10 ; i--)
  {
    for (int y = -10; y <= 10; y++)
    {
      //Put Complex Number onto the plane
      if (y == (int)data->real && i == (int)data->im)
      {
        printf("*");
        continue;
      }

      if (y == 0 && i == 0)
        printf("+");
      else if ( y == 0)
        printf("|");
      else if ( i == 0)
        printf("-");
      else
        printf(".");  
    }
    printf("\n");
  }
  
  
}

void prmt_c_operhand(complex *data)
{
  printf("Complex operand? ");
  scanf(" %f %f", &data->real, &data->im);
}

int prmt_n_operhand()
{
  int n;
  printf("Natural operand? ");
  scanf(" %d", &n);
  return n;
}

complex multiply_complex(complex *a, complex *b)
{
  complex product = {
      ( a->real * b->real ) - ( a->im * b->im ), //Real Component ac-bd
      ( a->real * b->im ) + ( a->im * b->real )  //imaginary Component ad+bc
    };

  return product;
}

void sum_complex(int operator, complex *data)
{
  complex input;
  prmt_c_operhand(&input);
  data->real = data->real + ( operator * input.real);
  data->im = data->im     + ( operator * input.im);
}

void do_calculator(void)
{
  //Initialize
  char cmd = 'q';
  complex current_c = {0.0f, 0.0f};
  complex complex_0 = {1.0f, 0.0f};

  print_operation();
  while (TRUE)
  {
    scanf(" %c", &cmd);
    switch (cmd) 
    {
      case 'q': //Exit the program
          print_complex(&current_c);
        return;
      
      case '0': //Zero out the complex
        current_c.real = 0;
        current_c.im = 0;
        break;

      case '+': //Add to the complex
          sum_complex(1, &current_c);
        break;

      case '-': //Subtract for the complex
          sum_complex(-1, &current_c);
        break;

      case '*': //Multiply with the complex
          complex input;
          prmt_c_operhand(&input);
          current_c = multiply_complex(&current_c, &input);
        break;

      case 'c': //Conjugate the complex (C FUNNY WITH -0..)
          if (current_c.im == 0) 
            break;
          current_c.im = -1 * current_c.im;
        break;

      case 'r': // Raise the complex to a Natural power
          int n = prmt_n_operhand();
          complex intermediate = current_c;
          current_c = complex_0;
          for (int i = 0; i < n; i++)
          {
            current_c = multiply_complex(&current_c, &intermediate);
          }
        break;

      case 'p': //Visualize on the complex plane
          print_plane(&current_c);
        break;

      default:
        printf("Invalid command '%c'\n", cmd);
    }
    print_complex(&current_c);
    print_operation();
  }
}

int main(void) {

  //Initialize
  printf("** Complex Calculator **\n");

  do_calculator();

  printf("Bye!\n");
  return EXIT_SUCCESS;
}
