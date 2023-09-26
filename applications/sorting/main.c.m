#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPERSONS 240
#define MAXNAME 100

struct person_t {
    char name[MAXNAME];
    float height;
};

int initialise(struct person_t persons[], int nrPersons){
    int X = nrPersons - 1;
        printf("Number of persons? ");
        scanf("%d",&nrPersons);
        if(nrPersons < 1 || nrPersons > MAXPERSONS)
            printf("Number of persons must be from 1 up to 240\n");
        else{
             for(int i = 0; i <= X; i++)
                 sprintf(persons[i].name, "person-%d",i); }
    return nrPersons;
}

void printValues(struct person_t persons[], int from, int to){
    printf("[");
    int aux=to;
    for(int i = from; i <= to; i++){
        printf("(\"person-%d\",%.2f)",i-1,(float)aux-1);
        if(i!=to)
            printf(",");
        aux--;
    }
    printf("]\n");
}

float maxValue(struct person_t persons[], int nrPersons){
    int max_height=persons[0].height;
    if(nrPersons == 0)
        max_height = 0;
    for(int i = 1; i <= nrPersons ; i++){
        if(persons[i].height >= max_height)
            max_height=persons[i].height;
    }
    return max_height;
}

float minValue(struct person_t persons[], int nrPersons){
    int min_height=persons[0].height;
    if(nrPersons == 0)
        min_height = 0;
    for(int i = 1; i <= nrPersons ; i++){
        if(persons[i].height <= min_height)
            min_height=persons[i].height;
    }
    return min_height;
}

void replacePerson(struct person_t persons[], int nrPersons){
    int i;
    char new_name[MAXNAME];
    float new_height;
    printf("Index? ");
    scanf("%d",&i);
    if(i < 0 || i > nrPersons)
        printf("Index must be from 0 up to %d",nrPersons);
    else{
        printf("Name? ");
        scanf("%s",new_name);
        printf("Height? ");
        scanf("%f",&new_height);
        strcpy(persons[i].name,new_name);
        persons[i].height=new_height;
    }
}

//void displayValues(display_t display, struct person_t persons[], int nrPersons, int from, int to){
    


int main(void)
{
    char cmd;
    struct person_t persons[MAXPERSONS];
    int nrPersons=0;
    
    do{
        printf("Command? ");
        scanf(" %c",&cmd);
        switch(cmd){
            
        case 'i':
        nrPersons=initialise(persons, nrPersons);
        break;
           
        case 'p':
        printValues(persons, 1, nrPersons);
        break;
        
        case 'h':
        printf("Min: %.3f\n",minValue(persons, nrPersons));
        printf("Max: %.3f\n",maxValue(persons, nrPersons));
        printf("Range: %.3f\n",maxValue(persons, nrPersons)-minValue(persons, nrPersons));
        break;
        
        case 'r':
        replacePerson(persons, nrPersons);
        break;
        
        case 'q':
        printf("Bye!\n");
        break;
        
        default:
        printf("Unknown command '%c'\n",cmd);
        break;
    }
    } while (cmd != 'q');
}