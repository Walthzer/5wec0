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

//void displayValues(display_t display, struct person_t persons[], int nrPersons, int from, int to)

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
void bubbleSort(person_t persons[], int nrPersons)
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
      //displayValues -> IMPLEMENT DISPLAY
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
void mrgeSort(person_t persons[], int nrPersons, int left, int right)
{

  int mid = (left+right) / 2;
  if (left < right) {
    printf("need to sort      ");
    printValues(persons, left, right);

    mrgeSort (persons, nrPersons, left,mid);
    printf("after sorting lhs ");
    printValues(persons, left, mid);

    mrgeSort (persons, nrPersons, mid+1,right);
    printf("after sorting rhs ");
    printValues(persons, mid+1, right);

    merge (persons, left,mid,right);
    printf("after merging     ");
    printValues(persons, left, right);
  }
}


void do_sorting(void)
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
      break;

      case 'b':
        bubbleSort(persons, persons_count);
      break;

      case 'm':
        //Merge sorts condition -> check outside of recursive function!
        if (persons_count < 2)
          break;
        mrgeSort(persons, persons_count, 0, persons_count-1);
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
  //Initialize
  do_sorting();

  printf("Bye!\n");

  return EXIT_SUCCESS;
}
