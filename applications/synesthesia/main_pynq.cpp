#define RUN_ON_MANJ 1
#ifdef RUN_ON_MANJ
  #include <stdlib.h>
  #include <stdio.h>
  #include <unistd.h>
#else
  #include <libpynq.h>
#endif

#define TRUE 1
#define FALSE 0
#define LENGTH 30
#define MORSE_UNIT_MSC 500

void readLine(char str[], int size)
{
  int i = 0;
  while (TRUE)
  { 
    char c = getchar();
    if (c == '\n')
    {
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
  print_c('[');
  for (int i = 0; i < LENGTH; i++)
  {
    switch (str[i])
    {
    case '\0':
      print_c('\\');
      print_c('0');
      break;

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

//int is_vowel(char c)
//int is_constant(char c)
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

int char_to_morse_index(char c)
{
  //Char to int value -> use '0' == 48 offset
  //Jump over letters with the index -> offset 25
  if (is0to9(c))
    return (c - 48) + 25;

  //If char is a lowercase letter -> upper it
  if (c >= 97 && c <= 122)
    c = c - 32;

  //Char to int value -> use A == 65 offset
  if (c >= 65 && c <= 90)
    return (c - 65);

  //Not digit or letter
  return -1;

}

#ifdef RUN_ON_MANJ
void sleep_msec(int delay)
{
  sleep(delay * 0);
}
#endif

void blink_led(int delay)
{
  #ifndef RUN_ON_MANJ
    color_led_on();
    printf("ON--");
    fflush(NULL);
    sleep_msec(delay);
    color_led_off();
    printf("OFF--");
    fflush(NULL);
  #else
    sleep_msec(delay);
  #endif
}

void print_morse(char str[])
{
  int symbol_space = 1;
  int letter_space = 3;
  int word_space = 7;
  int dot = 1;
  int dash = 3;
  int morse[26+10][5] =
  {
    // letters
    { dot, dash },
    { dash, dot, dot, dot },
    { dash, dot, dash, dot },
    { dash, dot, dot },
    { dot },
    { dot, dot, dash, dot },
    { dash, dash, dot },
    { dot, dot, dot, dot },
    { dot, dot },
    { dot, dash, dash, dash },
    { dash, dot, dash },
    { dot, dash, dot, dot },
    { dash, dash },
    { dash, dot },
    { dash, dash, dash },
    { dot, dash, dash, dot },
    { dash, dash, dot, dash },
    { dot, dash, dot },
    { dot, dot, dot },
    { dash },
    { dot, dot, dash },
    { dot, dot, dot, dash },
    { dot, dash, dash },
    { dash, dot, dot, dash },
    { dash, dot, dash, dash },
    { dash, dash, dot, dot },
    // digits
    { dot, dash, dash, dash, dash },
    { dot, dot, dash, dash, dash },
    { dot, dot, dot, dash, dash },
    { dot, dot, dot, dot, dash },
    { dot, dot, dot, dot, dot },
    { dash, dot, dot, dot, dot },
    { dash, dash, dot, dot, dot },
    { dash, dash, dash, dot, dot },
    { dash, dash, dash, dash, dot },
    { dash, dash, dash, dash, dash }
  };

  for (int i = 0; str[i] != '\0'; i++)
  {
    int i_value = char_to_morse_index(str[i]);

    //End of word delay
    if (i_value < 0)
    {
      printf("   ");
        fflush(NULL);
      sleep_msec(word_space * MORSE_UNIT_MSC);
      continue;
    }

    for (int y = 0; y < 5; y++)
    {
      if (morse[i_value][y] == 0)
        break;
      
      if (y > 0)
        {
          sleep_msec(symbol_space * MORSE_UNIT_MSC);
        }

      blink_led(morse[i_value][y] * MORSE_UNIT_MSC);
    }
    printf("/");
      fflush(NULL);
    sleep_msec(letter_space * MORSE_UNIT_MSC);
  }
  printf("\n");
  
}

void do_prints(void)
{
  //Initialize
  char input_str[LENGTH] = {'\0'};
  char curr_str[LENGTH] = {'\0'};
  char d_char[1];


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
        print_morse(curr_str);
      break;

      default:
        printf("Unknown command '%c'\n", input_str[0]);
    }
    //print_value_str("string", curr_str);
    print_command();
  }
}

int main(void) {
  #ifndef RUN_ON_MANJ
  pynq_init();
  color_leds_init_pwm();
  #endif


  //Initialize
  do_prints();

  printf("Bye!\n");

  #ifndef RUN_ON_MANJ
  pynq_destroy();
  #endif

  return EXIT_SUCCESS;
}
