 /*
 *  TU/e 5WEC0::Q2 WEEK 1 Assigment
 *
 *  Written by: Walthzer
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define CTM0(var) var==-0 ? 0 : var

typedef struct _node_t {
    float re, im;
    struct _node_t *next;
} node_t;

//////////////////////////////////
////////// HELPER ////////////////
//////////////////////////////////

//Recursive freeing of the list
void free_node_list(node_t *node)
{
    if(node == NULL)
        return;
    if(node->next != NULL)
        free_node_list(node->next);
    free(node);
}
//find the last node or the last full node -> has a next node
node_t *find_last(node_t *head, int full)
{
    node_t *node = head;
    while(node != NULL)
    {
        //Last node
        if(node->next == NULL)
            return node;
        //Second last node
        if(node->next->next == NULL && full == 1)
            return node;
        node = node->next;
    }
    return node;
}

float euclidian_dist(float a_re, float a_im, float b_re, float b_im)
{
    return sqrt(pow(a_re - b_re, 2) + pow(a_im - b_im, 2));
}

//////////////////////////////////
////////// HOMEWORK //////////////
//////////////////////////////////

//Print the list
void print_list(node_t *node)
{
    printf("[");
    int counter = 0;
    while(node != NULL)
    {
        if(counter > 0)
            printf(",");
        printf("%.2f%+.2fi", CTM0(node->re), CTM0(node->im));
        node = node->next;
        counter++;
    }

    printf("]\n");
}
//Create the node on the heap and assign it
node_t* alloc_node(float re, float im, node_t *next)
{
    node_t *node = malloc(sizeof(node_t));
    node->re = re;
    node->im = im;
    node->next = next;
    return node;
}
//Insert at the "front" head of the list
node_t *insert_head(node_t *list, float re, float im)
{
     //Node is at the front so next node is the list
    return alloc_node(re, im, list);
}
//Insert at the "rear" tail of the list
node_t *insert_tail(node_t *list, float re, float im)
{
    //List is empty -> just return node
    if(list == NULL)
        return alloc_node(re, im, NULL);
    
    //Find the last node in the list and set tail
    find_last(list, 0)->next = alloc_node(re, im, NULL);
    return list;
}
//Remove the first node of the list
node_t *remove_first(node_t *head)
{
    if(head == NULL)
        return NULL;
    node_t *list = head->next;
    free(head);
    return list;
}
//Remove the last node of the list
node_t *remove_last(node_t *head)
{
    node_t *last_full = find_last(head, 1);
    if(last_full != NULL)
    {
        if(last_full->next != NULL)
        {
            free(last_full->next);
            last_full->next = NULL;
        } else
        {
            free(last_full);
            return NULL;
        }
    }
    return head;
}
//rFind the closest number using euclidian distance
node_t *find_closest(node_t *list, float re, float im)
{
    if(list == NULL)
        return NULL;
    
    node_t *node = list->next;
    node_t *closest = list;
    float close_dist = euclidian_dist(re, im, closest->re, closest->im);
    
    while(node != NULL)
    {
        float node_dist = euclidian_dist(re, im, node->re, node->im);
        if(close_dist > node_dist)
        {
            closest = node;
            close_dist = node_dist;
        }
        node = node->next;
    }
    return closest;
}
//Insert node into list after closest
node_t *insert_after_closest (node_t *head, float re, float im)
{
    node_t *closest = find_closest(head, re, im);
    if(closest == NULL)
        return alloc_node(re, im, NULL);
    if(closest->next == NULL)
        closest->next = alloc_node(re, im, NULL);
    else
        closest->next = alloc_node(re, im, closest->next);
    return head;
}
//Insert node into list before closest
node_t *insert_before_closest (node_t *head, float re, float im)
{
    node_t *closest = find_closest(head, re, im);
    if(closest == NULL)
        return alloc_node(re, im, NULL);

    node_t *leading = head;
    while(leading->next != NULL)
    {
        if(leading->next == closest)
        {
            leading->next = alloc_node(re, im, closest); 
            return head;
        }
        leading = leading->next;
    }
    return alloc_node(re, im, closest);
}
//Remove the closest number
node_t *remove_closest(node_t *head, float re, float im)
{
    node_t *closest = find_closest(head, re, im);
    if(closest == NULL)
        return head;
    
    if(closest == head)
    {
        node_t *next = closest->next;
        free(closest);
        return next;
    }
    
    node_t *leading = head;
    while(leading->next != NULL)
    {
        if(leading->next == closest)
        {
            leading->next = closest->next; 
            free(closest);
            return head;
        }
        leading = leading->next;
    }
    //Fail state
    free(leading);
    return NULL;
}
//print reversed list
void print_list_reverse(node_t *head)
{
    if(head == NULL)
        return;
    
    print_list_reverse(head->next);
    if(head->next != NULL)
        printf(",");
    printf("%.2f%+.2fi", CTM0(head->re), CTM0(head->im));
}
int main (void)
{
    node_t *list = NULL;
    node_t *node = NULL;
    float inpt_re, inpt_im;
    char cmd;
    do
    {
        printf("Command? ");
        scanf(" %c", &cmd);
          
        switch (cmd)
        {
            case 'q':
            break;
            
            case 'p':
            print_list(list);
            break;
            
            case 'v':
            printf("[");
            print_list_reverse(list);
            printf("]\n");
            break;
            
            case 'h':
            printf("re, im? ");
            scanf(" %f %f", &inpt_re, &inpt_im);
            list = insert_head(list, inpt_re, inpt_im);
            break;
            
            case 't':
            printf("re, im? ");
            scanf(" %f %f", &inpt_re, &inpt_im);
            list = insert_tail(list, inpt_re, inpt_im);
            break;
            
            case 'a':
            printf("re, im? ");
            scanf(" %f %f", &inpt_re, &inpt_im);
            list = insert_after_closest(list, inpt_re, inpt_im);
            break;
            
            case 'b':
            printf("re, im? ");
            scanf(" %f %f", &inpt_re, &inpt_im);
            list = insert_before_closest(list, inpt_re, inpt_im);
            break;
            
            case 'f':
            list = remove_first(list);
            break;
            
            case 'l':
            list = remove_last(list);
            break;
            
            case 'r':
            printf("re, im? ");
            scanf(" %f %f", &inpt_re, &inpt_im);
            list = remove_closest(list, inpt_re, inpt_im);
            break;
            
            case 'c':
            printf("re, im? ");
            scanf(" %f %f", &inpt_re, &inpt_im);
            node = find_closest(list, inpt_re, inpt_im);
            
            if(node == NULL)
                printf("No closest number found\n");
            else
                printf("Closest number is %.2f%+.2fi\n", CTM0(node->re), CTM0(node->im));
            break;
              
            default:
                printf("Unknown command \'%c\'\n", cmd);
            break;
        }
      
    } while (cmd != 'q');
    free_node_list(list);
    
    printf("Bye!\n");
}
