//#include <libpynq.h>
#include <stdio.h>

#ifndef PYNQLIB_H
  #define DISPLAY_WIDTH 240
  typedef void* display_t;
#endif

#define LENGTH 120
#define TRUE 1
#define FALSE 0

void print_command()
{
  printf("Command? ");
}
void print_array(char str[], int length)
{
  for (int i = 0; i < length; i++)
  {
    switch (str[i])
    {
    case '\0':
      printf("\\0");
      break;

    case '\n':
      printf("\\n");
      break;

    default:
      printf("%c", str[i]);
      break;
    }
  }
  printf("\n");
}
//Find lengthe of string
int str_len(char str[])
{
  int i = 0;
  for (;str[i] != '\0' && i < LENGTH; i++) {}

  return i;
}
void print_names(char names[], char* startPos[], int nrNames)
{
  for (int i = 0; i < nrNames; i++)
  {
    char* ptr_pos = startPos[i];
    printf("startPos[%d]=%03d #chars=%03d string=\"%s\"\n", i, (int)(ptr_pos - names), (int)(str_len(ptr_pos)) + 1, ptr_pos);
  }
}
//Return 1 if strings are equal else 0;
int str_cmp(char a[], char b[])
{
  for (int i = 0; i < LENGTH; i++)
  {
    if (a[i] != b[i])
      return 0;
    if (a[i] == '\0')
      break;
  }
  return 1;
}
int space_used(char* startPos[], int nrNames)
{
  return startPos[nrNames] - *startPos; 
}

int lookup_name(char* startPos[], int nrNames, char name[])
{
  //Filter array backwards -> return greater index in multiplicity cases
  for (int i = nrNames - 1; i >= 0; i--)
  { 
    char* ptr_curr_pos = startPos[i];
    if (str_cmp(name, ptr_curr_pos))
      return startPos[i] - *startPos;
  }
  return -1;
}
void insert_name(char* starPos[], int* ptr_nrNames, char newName[])
{
  //Abort if there is not enough space
  if (str_len(newName) >= LENGTH - space_used(starPos, *ptr_nrNames))
  {
    printf("Not enough space left\n");
    return;
  }

  char* ptr_pos = starPos[*ptr_nrNames];
  //crude strcpy function
  for (int i = 0; newName[i] != '\0' && i < LENGTH; i++)
  {
    *ptr_pos = newName[i];
    ptr_pos++; //Increment the pointer -> automatic 
  }
  ptr_pos++; //Skip 1 null char.
  (*ptr_nrNames)++; 
  starPos[*ptr_nrNames] = ptr_pos;
}
void remove_name(char *startPos[], int *nrNames, char name[])
{
  int index = -1;
  //Find the first occurence of a name
  for (int i = 0; i < *nrNames; i++)
  { 
    char* ptr_curr_pos = startPos[i];
    if (str_cmp(name, ptr_curr_pos))
    {
      index = i;
      break;
    }
  }
  //Did not find the name
  if (index < 0)
  {
    printf("Did not find \"%s\"\n", name);
    return;
  }
  //Replace the removed names index
  startPos[index] = startPos[*nrNames - 1];
  (*nrNames)--;
  //Stabilize last ptr in startPos
  startPos[*nrNames] = startPos[*nrNames + 1];
  
}

void swap_names(char* startPos[], int a, int b)
{
  char* tmp = startPos[a];
  startPos[a] = startPos[b];
  startPos[b] = tmp;
}
void merge_name(char* startPos[], int* nrNames, int index)
{
  char* ptr_start = startPos[index];
  char* ptr_center = ptr_start + str_len(ptr_start); //get terminating null char
  //Link names:
  *ptr_center = '+';

  //Is the found next removed -> we are done
  for (int i = 0; i < *nrNames; i++)
  {
    if ((ptr_center + 1) == startPos[i])
    {
      //Swap the removed names index with the last index
      startPos[i] = startPos[*nrNames - 1];
      (*nrNames)--;
      //Ensure the startPos[nrNames] points to free space
      startPos[*nrNames] = startPos[*nrNames + 1];
      return;
    }
  }
}

void do_sisters(display_t* display_t) {
  //Initialize
  char cmd = '\0';
  int nrNames = 0;
  char* startPos[10] = {NULL};
  char names[LENGTH] = {'\0'};
  char newName[LENGTH] = {'\0'};
  startPos[0] = names;
  
  print_command();
  while (TRUE)
  {

    scanf(" %c",&cmd);
    switch (cmd) 
    {
      case 'q':
        return;

      case 'i':
        //Insert a new name into the names array.
        printf("Name? ");
        scanf("%s", newName);
        insert_name(startPos, &nrNames, newName);
      break;

      case 'l':
        //Find index of a string in the names array.
        printf("Name? ");
        scanf("%s", newName);
        printf("\"%s\" has index %d\n",newName, lookup_name(startPos, nrNames, newName));
      break;

      case 'r':
        //Remove a name pointer from the startPos array -> No deletion of data.
        printf("Name? ");
        scanf("%s", newName);
        remove_name(startPos, &nrNames, newName);
      break;

      case 'p':
        //Print persons array
        print_array(names, LENGTH);
      break;

      case 'u':
        //Print used space
        printf("Space used is %d\n", space_used(startPos, nrNames));
      break;

      case 'n':
        //Print names in array
        print_names(names, startPos, nrNames);
      break;

      case 's':
        //Swap two ptrs in the startPos array -> a name swap
        printf("Indices? ");
        int i_first, i_second;
        scanf(" %d %d", &i_first, &i_second);
        swap_names(startPos, i_first, i_second);
      break;

      case 'm':
        //merges a name with its following name in the names array.
        printf("StartPos index? ");
        int index;
        scanf(" %d", &index);
        merge_name(startPos, &nrNames, index);
      break;

      case 'd':
        //NOT IMPLEMENTED
      break;

      default:
        printf("Unknown command \'%c\'\n", cmd);
    }
    print_command();
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
  do_sisters(&display);

  printf("Bye!\n");

  #ifdef PYNQLIB_H
    display_destroy(&display);
    pynq_destroy();
  #endif

  return 0;
}