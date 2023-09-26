#include <libpynq.h>
#include <stdio.h>
#include <string.h>

#ifndef PYNQLIB_H
  #define DISPLAY_WIDTH 240
  typedef void* display_t;
#endif


#define EXIT_SUCCESS 0

#define TRUE 1
#define FALSE 0
#define MAX_NAME_LENGTH 100
#define MAX_PERSONS DISPLAY_WIDTH

typedef struct _person_t {
  char name[MAX_NAME_LENGTH];
  float height;
} person_t;

void print_command()
{
  printf("Command? ");
}
void printValues(person_t persons[], int from, int to)
{
  if (to > MAX_PERSONS)
    to = MAX_PERSONS;
  
  printf("[");
  for (int i = from; i <= to; i++)
  {
    printf("(\"%s\",%.2f)", persons[i].name, persons[i].height);
    if (i < to)
      printf(",");
  }
  printf("]\n");
}

int initialise(person_t persons[], int nrPersons)
{
  if (nrPersons < 1 || nrPersons > MAX_PERSONS)
  {
    printf("Number of persons must be from 1 up to %d\n", MAX_PERSONS);
    return 0;
  }

  for (int i = 0; i < nrPersons; i++)
  {
    //Count down from nrPersons to 1.
    //Last element needs height 0 so add -1 offset
    persons[i].height = nrPersons - i - 1;
    sprintf(persons[i].name, "person-%d", i);
  }
  return nrPersons;
}

float maxValue(person_t persons[], int nrPersons)
{
  if (nrPersons < 1)
    return 0.0f;

  float* max_height = &persons[0].height;
  for (int i = 1; i < nrPersons; i++)
  {
    if (*max_height < persons[i].height)
      max_height = &persons[i].height;
  }
  return *max_height;
}
float minValue(person_t persons[], int nrPersons)
{
  if (nrPersons < 1)
    return 0.0f;

  float* min_height = &persons[0].height;
  for (int i = 1; i < nrPersons; i++)
  {
    if (*min_height > persons[i].height)
      min_height = &persons[i].height;
  }
  return *min_height;
}

void replacePerson(person_t persons[], int nrPersons)
{
  int index;
  printf("Index? ");
  scanf(" %d", &index);
  if (index < 0 || index > nrPersons - 1)
  {
    printf("Index must be from 0 up to %d\n", nrPersons);
    return;
  }
  //Directly read into the structs -> memory is already present
  printf("Name? ");
  scanf(" %s", persons[index].name); 
  printf("Height? ");
  scanf(" %f", &persons[index].height);
}

void displayValues(display_t display, person_t persons[], int nrPersons, int from, int to)
{
  sleep_msec(1000);
  float min, max;
  min = minValue (persons,nrPersons);
  max = maxValue (persons,nrPersons);

  float range = max-min;

  //Setup display
  displayFillScreen(&display, RGB_BLUE);
  for (int i = 0; i < nrPersons; i++)
  {
    //Skip the bar if its height is the min value
    if (persons[i].height == min && nrPersons > 1)
      continue;

    int bar_width = DISPLAY_WIDTH/nrPersons;

    //Select colour for the current bar
    uint16_t colour = RGB_GREEN;
    if (i == to || i == from)
      colour = RGB_RED;

    //Display is rotated on pcb, display_flip is set in main()
    int x1 = i * bar_width;
    int y1 = ((persons[i].height - min)/range * DISPLAY_HEIGHT - 1);
    int x2 = (i + 1) * bar_width - 1;
    int y2 = 0;

    fflush(NULL);

    displayDrawFillRect(&display, 
        x1, y1,
        x2, y2,
        colour);
  }
}

//Compare A to B:
//Return TRUE if A > B
int person_cmp(person_t a, person_t b)
{
  //if a has smaller height return FALSE:
  if (a.height < b.height)
    return FALSE;

  //Elements with equal height -> sort based on name:
  //  if a.name < b.name -> return FALSE
  if (a.height == b.height && (strcmp(a.name, b.name) <= 0))
    return FALSE;
  
  //a > b
  return TRUE;
}

//bubbleSort function:
void bubbleSort(display_t display, person_t persons[], int nrPersons)
{
  int swaps = 1;
  person_t buffer;

  //If we preformend any swaps -> run sort again
  //Sorting stops when we perform no swaps anymore. 
  while (swaps > 0)  
  {
    swaps = 0;
    for (int i = 0; i < nrPersons - 1; i++)
    {

      if(!person_cmp(persons[i], persons[i + 1]))
        continue;

      //Copy element A into storage
      //Set element A to equal B
      //Restore copy of A into B
      buffer = persons[i];
      persons[i] = persons[i + 1];
      persons[i + 1] = buffer;

      swaps++;
      displayValues(display, persons, nrPersons, i, i+1);
      printf("after swapping: ");
      printValues(persons, 0, nrPersons - 1);
    }

  }
}

//Merge function from kees
void merge (person_t a[], int left, int mid, int right)
{
  person_t tmp[right-left+1];
  int pos = 0;
  int lpos = left;
  int rpos = mid + 1;
  while (lpos <= mid && rpos <= right) {
  if (person_cmp(a[rpos], a[lpos]))
  tmp[pos++] = a[lpos++];
  else
  tmp[pos++] = a[rpos++];
  }
  while (lpos <= mid) tmp[pos++] = a[lpos++];
  while (rpos <= right) tmp[pos++] = a[rpos++];
  /* copy back */
  for(int i = 0; i < pos; i++)
    a[i+left] = tmp[i];
}

//Recursive merge sort function:
void mrgeSort(display_t display, person_t persons[], int nrPersons, int left, int right)
{

  int mid = (left+right) / 2;
  if (left < right) {
    printf("need to sort      ");
    printValues(persons, left, right);

    mrgeSort (display, persons, nrPersons, left,mid);
    printf("after sorting lhs ");
    printValues(persons, left, mid);

    mrgeSort (display, persons, nrPersons, mid+1,right);
    printf("after sorting rhs ");
    printValues(persons, mid+1, right);

    merge (persons, left,mid,right);
    printf("after merging     ");
    printValues(persons, left, right);
    displayValues(display, persons, nrPersons, right, left);
  }
}


void do_sorting(display_t* display)
{
  //Initialize
  char cmd = '\0';
  int persons_count = 0;
  person_t persons[MAX_PERSONS];

  print_command();
  while (TRUE)
  {

    scanf(" %c",&cmd);
    switch (cmd) 
    {
      case 'q':
        return;

      case 'i':
        //Initialise array with persons.
        int persons_to_int;
        printf("Number of persons? ");
        scanf(" %d", &persons_to_int);
        int int_persons = initialise(persons, persons_to_int);
        if (int_persons > 0)
          persons_count = int_persons;
      break;

      case 'p':
        //Print persons array
        printValues(persons, 0, persons_count - 1);
      break;
      
      case 'h':
        //Print the Max-Min height and the range.
        float min, max;
        min = minValue(persons, persons_count);
        max = maxValue(persons, persons_count);
        printf("Min: %.3f\nMax: %.3f\nRange: %.3f\n", min, max, max-min);
      break;

      case 'r':
        //replace a person
        replacePerson(persons, persons_count);
      break;

      case 'd':
        displayValues(*display, persons, persons_count, -1, -1);
      break;

      case 'b':
        bubbleSort(*display, persons, persons_count);
      break;

      case 'm':
        //Merge sorts condition -> check outside of recursive function!
        if (persons_count < 2)
          break;
        mrgeSort(*display, persons, persons_count, 0, persons_count-1);
      break;
      case 's':
        printf("Not Implemented");
        return;
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
  do_sorting(&display);

  printf("Bye!\n");

  #ifdef PYNQLIB_H
    display_destroy(&display);
    pynq_destroy();
  #endif

  return EXIT_SUCCESS;
}
