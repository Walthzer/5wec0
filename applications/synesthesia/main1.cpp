#include <stdio.h>

#define EXIT_SUCCESS 0

#define TRUE 1
#define FALSE 0
#define LENGTH 30
#define MORSE_UNIT_MSC 1000

void readLine(char str[], int size)
{
  int i = 0;
  while (TRUE)
  { 
    //Read one char
    char c = getchar();

    //Stop reading at a newline
    if (c == '\n')
    {
      //If we aren't at the end of the string -> terminate it
      if(i < size - 1)
        str[i] = '\0';
      break;      
    } else if (i < size - 1)
    {
      str[i] = c;
    }

    i++;
  }  
}

void print_c(char c)
{
  putchar(c);
  fflush(NULL);
}
void print_str(char str[])
{
  print_c('\"');
  for (int i = 0; str[i] != '\0'; i++)
  {
    print_c(str[i]);
  }
  print_c('\"');
  print_c('\n');
}
void print_array(char str[])
{
  //Prints a string array as an array -> display special chars
  print_c('[');
  for (int i = 0; i < LENGTH; i++)
  {
    switch (str[i])
    {
      //Display null char
    case '\0':
      print_c('\\');
      print_c('0');
      break;
      //Display newline char
    case '\n':
      print_c('\\');
      print_c('n');
      break;

    default:
      print_c(str[i]);
      break;
    }
    if (i + 1 < LENGTH)
      print_c(',');
  }
  print_c(']');
  print_c('\n');
}

int is0to9(char c)
{
  return c >= 48 && c <= 57;
}

void print_command()
{
  printf("Command? ");
}
int findFirstOccurrence(char str[], char aChar)
{
  for (int i = 0; str[i] != '\0'; i++)
  {
    if (str[i] == aChar)
      return i;
  }
  return -1;
}
int readInt(char str[], int length)
{
  //If first char isn't a digit -> return -1
  if (!is0to9(str[0]))
    return -1;
  
  int value = 0;
  for (int i = 0; is0to9(str[i]) && i < length; i++)
  {
    //Turn the Char value into a true int -> Ansi table ofset 48
    //Multiply by 10 to shift value.
    value = (value * 10) + (int)(str[i] - 48);
  }
  return value;
}
void insertChar(char str[], char aChar, int index)
{
  //Insert a char at a certain index -> Append at position
  //bad index or a possible overflow -> do nothing

  //Prevent bad index:
  if (index < 0 || index > LENGTH - 1)
    return;

  //Exit if insert would mean overflow:
  //if second to last char is NOT null-char -> String is full.
  if (str[LENGTH - 2] != '\0')
    return;

  char buffer[] = {'\0', aChar};
  for (int i = 0; i < LENGTH; i++)
  {
    //Skip until we reach the target position
    if (i < index)
      continue;

    buffer[0] = str[i];
    str[i] = buffer[1];

    buffer[1] = buffer[0];
  }
}
int replaceChars(char str[], char from_str[], char toChar)
{
  int replacement_cntr = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    for (int y = 0; from_str[y] != '\0'; y++)
    {
      if (from_str[y] == str[i])
      {
        str[i] = toChar;
        replacement_cntr++;
      } 
    }
  }
  return replacement_cntr; 
}

void do_prints(void)
{
  //Initialize
  char input_str[LENGTH] = {'\0'};
  char curr_str[LENGTH] = {'\0'};
  char d_char[1] = {'\0'};


  print_command();
  while (TRUE)
  {
    readLine(input_str, LENGTH);
    switch (input_str[0]) 
    {
      case 'q':
        return;

      case 's':
        //Read in new values for the char array -> doesn't clear
        printf("Please enter a string? ");
        readLine(curr_str, LENGTH);
      break;

      case 'p':
          //Print Char array as a string
          printf("The current string is: ");
          print_str(curr_str);
      break;
      
      case 'a':
          //Print Char array as an array
          printf("The current array is: ");
          print_array(curr_str);
        break;

      case 'o':
        //Find first occurrence of a char
        printf("Find first occurrence of which character? ");
        readLine(d_char, 2);
        int o_pos = findFirstOccurrence(curr_str, d_char[0]);
        printf("The first occurrence of \'%c\' is at index %d\n", d_char[0], o_pos);
      break;

      case 'i':
        //Insert a char at a certain index -> Append at position
        //bad index or a possible overflow -> do nothing
        //Read a char to insert
        printf("Insert which character? ");
        readLine(d_char, 2);

        //Read a integer-str for the insert index
        printf("At what index? ");
        char index_char[LENGTH];
        readLine(index_char, LENGTH);

        insertChar(curr_str, d_char[0], readInt(index_char, LENGTH));
      break;

      case 'r':
        //Read array of char's that need to be replaced
        printf("Replace which characters? ");
        char from_str[LENGTH];
        readLine(from_str, LENGTH);

        //Read replacment char
        printf("with which character? ");
        readLine(d_char, 2);

        replaceChars(curr_str, from_str, d_char[0]);
      break;

      case 'm':
        printf("NOT IMPLEMENTED");
      break;

      default:
        printf("Unknown command '%c'\n", input_str[0]);
    }
    print_command();
  }
}

int main(void) {
  //Initialize
  do_prints();

  printf("Bye!\n");

  return EXIT_SUCCESS;
}
