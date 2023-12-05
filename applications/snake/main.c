 /*
 *  TU/e 5WEC0::Q2 WEEK 2 Assigment
 *
 *  Written by: Walthzer
 * 
 */

#include <stdio.h>
#include <libpynq.h>

#define BS 10 /* block size */
#define WIDTH (DISPLAY_WIDTH/BS)
#define HEIGHT (DISPLAY_HEIGHT/BS)

void set_block(display_t *display, int x, int y, int c) {
    displayDrawFillRect(display, x*BS, y*BS, (x+1)*BS-1,(y+1)*BS-1, c);
}

void play(display_t *display)
{
    displayFillScreen(display, RGB_BLACK);
    
    int delay = 300;
    int dirx, diry;
    int x, y;
    x = 0;
    y = 0;

    do {
    // get input
    int button_states[NUM_BUTTONS] = { 0 };
    sleep_msec_buttons_pushed (button_states, delay);
    if (button_states[0]) { dirx = +1; diry = 0; }
    else if (button_states[1]) { dirx = 0; diry = -1; }
    else if (button_states[2]) { dirx = 0; diry = +1; }
    else if (button_states[3]) { dirx = -1; diry = 0; }
    // erase at old position
    set_block(display, x, y, RGB_BLACK);
    x = (WIDTH + x + dirx) % WIDTH;
    y = (HEIGHT + x + diry) % HEIGHT;
    // draw at new position
    set_block(display,x,y,RGB_BLUE);
    } while (true);
}

typedef struct pos_t
{
    int x, y;
    struct pos_t* next;
} pos_t;

typedef struct _ttl_t {
    int x, y;
    int startTTL;
    int ttl;
    struct _ttl_t *next;
} ttl_t;

//---------------------------------------------------------------------
//---------------------------SNAKE Functions---------------------------
//---------------------------------------------------------------------

void print_pos(pos_t *list)
{
    printf("[");
    pos_t *item = list;
    while (item != NULL)
    {
        printf("(%d,%d)", item->x, item->y);
        item = item->next;
        
        if(item != NULL)
            printf(",");
    }
    
    printf("]\n");
}

pos_t* create_pos(int x, int y, pos_t *next)
{
    pos_t *pos = malloc(sizeof(pos_t));
    *pos = (pos_t){x, y, next};
    return pos;
}

void insert_pos(pos_t **list, int x, int y)
{
    //List is empty -> simply put in an item
    if(*list == NULL)
    {
        *list = create_pos(x, y, NULL);
        return;
    }
    
    //List has items -> check positions and insert
    pos_t *item = *list;
    
    //Loop condition is safety -> interior should handel control;
    while (item != NULL) {
        if(item->x == x && item->y == y)
        {
            printf("Position (%d,%d) is already in the list\n", x, y);
            return;
        }
        
        if(item->next == NULL)
        {
            item->next = create_pos(x, y, NULL);
            break;
        }
        item = item->next;
    }
}

pos_t remove_first_pos(pos_t **list)
{
    if(*list == NULL)
        return (pos_t){-1, -1, NULL};
    
    pos_t *head = *list;
    *list = head->next;
    
    pos_t removed = {head->x, head->y, NULL};
    free(head);
    
    return removed;
}

pos_t* look_up_pos(pos_t *list, int x, int y)
{
    //List is empty -> simply put in an item
    if(list == NULL)
        return NULL;
    
    //List has items -> check positions and insert
    pos_t *item = list;
    
    //Loop condition is safety -> interior should handel control;
    while (item != NULL) {
        if(item->x == x && item->y == y)
            return item;
        item = item->next;
    }
    return NULL;
}

//---------------------------------------------------------------------
//---------------------------TTL Functions-----------------------------
//---------------------------------------------------------------------
void print_ttl(ttl_t *list)
{
    printf("[");
    ttl_t *item = list;
    while (item != NULL)
    {
        printf("(%d,%d,%d,%d)", item->x, item->y, item->startTTL, item->ttl);
        item = item->next;
        
        if(item != NULL)
            printf(",");
    }
    
    printf("]\n");
}
ttl_t* create_ttl(int x, int y, int startTTL, ttl_t *next)
{
    ttl_t *ttl = malloc(sizeof(ttl_t));
    *ttl = (ttl_t){x, y, startTTL, startTTL, next};
    return ttl;
}

ttl_t* look_up_ttl(ttl_t *list, int x, int y)
{   
    //List is empty
    if(list == NULL)
        return NULL;
        
    //List has items -> check positions and insert
    ttl_t *item = list;
    
    while (item != NULL) {
        //Found item
        if(item->x == x && item->y == y)
            return item;

        item = item->next;
    }
    //No item found
    return NULL;
    
}

ttl_t* insert_ttl(ttl_t *list, int x, int y, int startTTL)
{
    //List is empty -> simply put in an item
    if(list == NULL)
        return create_ttl(x, y, startTTL, NULL);
        
    //List has items -> check TTL and insert
    ttl_t *item = list;
    ttl_t *previous = NULL;
    
    //Prevent duplicate
    if(look_up_ttl(list, x, y) != NULL)
    {
        printf("Position (%d,%d) is already in the list\n", x, y);
        return list;
    }
    
    //Leading and Middle insertion
    while (item != NULL) {
        //Insert before same TTL or smaller TTL -> Decreasing list;
        if(item->ttl >= startTTL)
        {
            //insert front
            if(previous == NULL)
                return create_ttl(x, y, startTTL, item);
            
            //Insert middle
            previous->next = create_ttl(x, y, startTTL, item);
            return list;
        }
        previous = item;
        item = item->next;
    }
    
    //End insertion
    previous->next = create_ttl(x, y, startTTL, NULL);
    return list;
}
void update_ttl(ttl_t *list)
{
    ttl_t *item = list;
    //decrement TTL values
    while(item != NULL)
    {
        item->ttl--;
        item = item->next;
    }
}

int remove_ttl(ttl_t **list)
{
    ttl_t *head = *list;
    ttl_t *item = *list;
    //decrement TTL values
    int removed_count = 0;
    while(item != NULL && item->ttl < 1)
    {
        removed_count++;
        head = item->next;
        free(item);
        item = head;
    }
    *list = head;
    return removed_count;
}

void remove_middle_ttl(ttl_t **list, int x, int y)
{
    //List is empty
    if(list == NULL)
        return;
        
    //List has items -> check positions and insert
    ttl_t *previous = NULL;
    ttl_t *item = *list;
    
    while (item != NULL) {
        if(item->x == x && item->y == y)
        {
            //Delete front
            if(previous == NULL)
                *list = item->next;
            else
                previous->next = item->next;

            free(item);
            return;
        }
        previous = item;
        item = item->next;
    }
    //Item not in list
    printf("Position (%d,%d) not in list\n", x, y);
}

int main(void)
{

    pynq_init();
    display_t display;
    display_init(&display);
    buttons_init();


    pos_t *snake = NULL;
    ttl_t *bonus = NULL;
    pos_t *aux_p;
    pos_t aux;
    char cmd;
    int x, y, ttl;
    
    do {
        printf("Command? ");
        scanf(" %c", &cmd);
        
        switch (cmd)
        {
            case 'q':
            break;
            
            case 'p':
                play(&display);
            break;

            case 'u':
                update_ttl(bonus);
            break;
            
            case 'r':
                int removed = remove_ttl(&bonus);
                printf("Removed %d nodes\n", removed);
            break;
            
            case 'm':
                printf("Position (x y)? ");
                scanf(" %d %d", &x, &y);
                remove_middle_ttl(&bonus, x, y);
            break;
            
            case 's':
                print_pos(snake);
            break;
            
            case 'b':
                print_ttl(bonus);
            break;
            
            case 't':
            while(bonus != NULL) {remove_middle_ttl(&bonus, bonus->x, bonus->y);};
            break;
            
            case 'e':
                printf("Position (x y)? ");
                scanf(" %d %d", &x, &y);
                insert_pos(&snake, x, y);
            break;
            
            case 'i':
                printf("Position (x y ttl)? ");
                scanf(" %d %d %d", &x, &y, &ttl);
                bonus = insert_ttl(bonus, x, y, ttl);
            break;
            
            case 'f':
                aux = remove_first_pos(&snake);
                printf("Removed (%d,%d)\n", aux.x, aux.y);
            break;
            
            case 'l':
                printf("Position (x y)? ");
                scanf(" %d %d", &x, &y);
                aux_p = look_up_pos(snake, x, y);
                if(aux_p != NULL)
                    printf("Position (%d,%d) is in the list\n", x, y);
                else
                    printf("Position (%d,%d) is not in the list\n", x, y);
            break;
            
            default:
                printf("Unknown command \'%c\'\n", cmd);
            break;
        }
        
    } while (cmd != 'q');
    
    while(snake != NULL) {remove_first_pos(&snake);};
    while(bonus != NULL) {update_ttl(bonus); remove_ttl(&bonus);};
    
    printf("Bye!\n");

    buttons_destroy();
    display_destroy(&display);
    pynq_destroy();
}
