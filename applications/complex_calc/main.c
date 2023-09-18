#include <stdio.h>
#include <math.h>

//complex type
typedef struct Complex
{
  float real;
  float imaginary;
}
complex;

//main function
int main(void) 
{
  printf("** Complex Calculator **\n");
  complex current = {0.0,0.0};
  complex newer = {0.0,0.0};
  complex result = {0.0,0.0};
  float exponent = 0;
  
  char operation = '0';
  do 
  {
    //"Operation [0+-*crpq]?"
    printf("Operation [0+-*crpq]? ");
    scanf(" %c",&operation);
  
    //Switch statement for using operations.
    switch(operation)
    {
      //quit
      case 'q':
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }  
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}
        printf("Bye!\n");
        break;
      
      //add
      case '+':
        printf("Complex operand? ");
        scanf("%f %f", &newer.real, &newer.imaginary);
        result.real = current.real + newer.real;
        result.imaginary= current.imaginary+ newer.imaginary;
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }  
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}  
        current = result;
        break;
      
      //subtract
      case '-':
        printf("Complex operand? ");
        scanf("%f %f", &newer.real, &newer.imaginary);
        result.real = current.real - newer.real;
        result.imaginary= current.imaginary - newer.imaginary;
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}  
        current = result;
        break;
      
      //default to 0
      case '0':
        result.real = 0.0;
        result.imaginary = 0.0;
        printf("Current value is %f+%fi\n", result.real,result.imaginary);
        current = result;
        break;
      
      //conjugate
      case 'c':
        result.real = current.real;
        result.imaginary = 0 - current.imaginary;
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}  
        current = result;   
        break;

      //multiply
      case '*':
        printf("Complex operand? ");
        scanf("%f %f", &newer.real, &newer.imaginary);
        //The real component; isquared = -1
        result.real = ((current.real*newer.real) - (current.imaginary*newer.imaginary));
        //The Imaginary component solved and factored
        result.imaginary = ((current.real*newer.imaginary) + (current.imaginary*newer.real)); 
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}  
        current = result;        
        break;

      //raise to power
      case 'r':
        printf("Natural operand? ");
        scanf("%f", &exponent);
        if(exponent==0)
        {
          result.real = 1;
          result.imaginary = 0;
          current = result;
        }  

        complex power = {1.0f, 0.0f};

        for(int i = 0; i < exponent; i++)
        {
          //The real component; isquared = -1
          result.real = ((power.real*current.real) - (power.imaginary*current.imaginary));
          //The Imaginary component solved and factored
          result.imaginary = ((power.real*current.imaginary) + (power.imaginary*current.real)); 
          power = result;            
        }
        current = result;
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}  
        break;

      //Printing into a complex plane.
      case 'p':
        for (int outer=-10; outer <= 10; outer++) 
        {          
          for (int inner=-10; inner <= 10; inner++) 
          {
            if(outer==0&&inner!=0)
            {
              printf("-");
            }          
            if(inner==0&&outer!=0)
            {
              printf("|");
            }
            if(outer==0 && inner==0)
            {  
              if(current.real==0&&current.imaginary==0)
              {
                printf("*");
              }
              else{printf("+");}
            }
            int x = (-current.imaginary);
            int y = (current.real);
            if(outer== x&& inner==y)
            {
              if(x!=0&&y!=0){printf("*");}
            } else {
              if(outer!=0&&inner!=0){printf(".");}   
            }
            
          }
          printf("\n");
        }
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}  

        break;
      
      default:
        printf("Invalid command '%c'\n",operation);
        if(result.imaginary<0)
        {
          printf("Current value is %f%fi\n", result.real,result.imaginary); 
        }
        else{printf("Current value is %f+%fi\n", result.real,result.imaginary);}
    }  
  }
  while(operation!='q');
  return 0;
}

