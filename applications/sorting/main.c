//#include <libpynq.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define LENGTH 30

void printChar(char c)
{
  putchar(c);
  fflush(NULL);
}

void readLine(char str[], int length)
{
  int i = 0;
  char read;
  while ((read = getchar()) != '\n')
  {
    if (i < length - 1)
    {
      str[i] = read;
      i++;
    }
  }
  str[i] = '\0';
}

int isconsonant(char c)
{
  c = tolower(c);
  char cons[] = "bcdfghjklmnpqrstvwxyz";
  for (int i = 0; i < (int)strlen(cons); i++)
  {
    if (cons[i] == c)
    {
      return 1;
    }
  }
  return 0;
}

int isvowel(char c)
{
  c = tolower(c);
  char vowel[] = "aeiou";
  for (int i = 0; i < (int)strlen(vowel); i++)
  {
    if (vowel[i] == c)
    {
      return 1;
    }
  }
  return 0;
}

int is0to9(char c)
{
  c = tolower(c);
  char number[] = "0123456789";
  for (int i = 0; i < (int)strlen(number); i++)
  {
    if (number[i] == c)
    {
      return 1;
    }
  }
  return 0;
}

int readInt(char str[], int length)
{
  int i = length;
  int integer = 0;

  for (i = 0; i < (int)strlen(str); i++)
  {
    if (is0to9(str[i]) == 1)
    {
      integer = integer * 10 + (str[i] - 48);
    }
    if (str[i] == 0 || !is0to9(str[0]))
    {
      integer = -1;
    }
    if (str[i] == ' ')
    {
      integer = -1;
    }
  }
  return integer;
}

int findFirstOccurrence(char str[], char aChar)
{
  int index = strchr(str, aChar) - str;
  return index;
}

void printString(char s[])
{
  int len = strlen(s);
  printf("\"");

  for (int i = 0; i <= len; i++)
  {
    if (s[i] != '\0')
    {
      printChar(s[i]);
    }
  }
  printf("\"\n");
}

void printArray(char a[], int length)
{
  printf("[");
  for (int i = 0; i < length; i++)
  {
    if (a[i] == '\0')
    {
      printf("\\0");
      fflush(NULL);
    }
    else if (a[i] == '\n')
    {
      printf("\\n");
      fflush(NULL);
    }
    else
    {
      printChar(a[i]);
    }

    if (i < length - 1)
    {
      printf(",");
    }
  }
  printf("]\n");
}

void insertChar(char str[], char aChar, int index)
{
  int i = 0;
  int check = 0;
  char copyStr[LENGTH] = {'\0'};

  while (i <= (int)strlen(str))
  {
    if (i < index && check == 0)
    {
      copyStr[i] = str[i];
    }
    else if (i == index)
    {
      copyStr[i] = aChar;
      check++;
    }
    else if (check)
    {
      copyStr[i] = str[i - 1];
    }
    i++;
  }
  i = 0;

  while (copyStr[i] != '\0')
  {
    str[i] = copyStr[i];
    i++;
  }
}

void replaceChars(char str[], char fromString[], char toChar)
{
  int i = 0;
  int length = strlen(str);
  int length2 = strlen(fromString);

  while (i < length && str[i] != '\0')
  {
    for (int j = 0; j < length2; j++)
    {
      if (str[i] == fromString[j])
      {
        str[i] = toChar;
      }
    }
    i++;
  }
}

int main(void)
{
  //pynq_init();
  //color_leds_init_pwm();

  char inputString[LENGTH] = {'\0'};
  char currString[LENGTH] = {'\0'};
  char occurrenceString[LENGTH] = {'\0'};
  char insertString[LENGTH] = {'\0'};
  char indexString[LENGTH] = {'\0'};
  char fromString[LENGTH] = {'\0'};
  char searchfor = '\0';
  int result = -2;

  do
  {
    printf("Command? ");
    readLine(inputString, LENGTH);
    switch (inputString[0])
    {
    case 'o':
      printf("Find first occurrence of which character? ");
      readLine(occurrenceString, 2);
      searchfor = occurrenceString[0];
      result = findFirstOccurrence(currString, searchfor);
      if (result <= LENGTH && result >= 0)
      {
        result = result;
      }
      else
      {
        result = -1;
      }
      printf("The first occurrence of \'%c\' is at index %d\n", searchfor, result);
      break;
    case 'i':
      printf("Insert which character? ");
      readLine(insertString, LENGTH);
      printf("At what index? ");
      readLine(indexString, LENGTH);
      int index = readInt(indexString, LENGTH);
      insertChar(currString, insertString[0], index);
      break;
    case 'r':
      printf("Replace which characters? ");
      readLine(fromString, LENGTH);
      printf("with which character? ");
      readLine(insertString, LENGTH);
      replaceChars(currString, fromString, insertString[0]);
      break;
    case 'p':
      printf("The current string is: ");
      printString(currString);
      break;
    case 'a':
      printf("The current array is: ");
      printArray(currString, LENGTH);
      break;
    case 's':
      printf("Please enter a string? ");
      readLine(currString, LENGTH);
      break;
    case 'q':
      printf("Bye!\n");
      break;
    default:
      printf("Unknown command '%c'\n", inputString[0]);
      break;
    }
  } while (inputString[0] != 'q');

  //leds_destroy();
  //pynq_destroy();
}