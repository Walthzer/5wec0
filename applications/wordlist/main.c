 /*
 *  TU/e 5WEC0::WEEK 8::WordList assignment
 *
 *  Written by: Walthzer
 * 
 */
#include <stdio.h>
#include <libpynq.h>

#define MAXWORDS 12
#define MAXLEN 20

void print_word_list(char *wordList[])
{
    for(int i = 0; i < MAXWORDS; i++)
    {
        if(wordList[i] == NULL)
            continue;
        
        char *curr_p = wordList[i];
        printf("\"%s\" is referenced from entries", wordList[i]);
        for(int w = 0; w < MAXWORDS; w++)
        {
            if(wordList[w] != curr_p)
                continue;
            printf(" %d", w);
        }
        printf("\n");
    }
}

void insert_word(char *wordList[MAXWORDS], char *word)
{
    //Find an index first
    //Before equal words else alfabetically
    
    //Dirty check if the array if full:
    if(wordList[MAXWORDS - 1] != NULL)
    {
        printf("List is full\n");
        return;
    }
    
    int index = -1;
    for(int i = 0; i < MAXWORDS; i++)
    {
        if(wordList[i] == NULL)
        {
            index = i;
            break;
        }
        
        if(strcmp(wordList[i], word) < 0)
            continue;
        
        //Word is equal or comes before current index
        index = i;
        break;
    }
    
    //Backup check for full list
    if(index < 0)
    {
        printf("List is full\n");
        return;
    }
    
    char *word_p = (char*) malloc ((strlen(word)+1)*sizeof(char));
    strcpy(word_p, word);
    
    //The position is free
    if(wordList[index] == NULL)
    {
        wordList[index] = word_p;
        return;
    }
    
    //The position is taken -> shifting time
    //Start at the second last index and shift
    //Back until we free our desired index
    for(int i = MAXWORDS - 2; i >= index; i--)
    {
        if(wordList[i] == NULL)
            continue;
        
        wordList[i + 1] = wordList[i];
    }
    wordList[index] = word_p;
}

void find_word(char **wordList, char *word, int *index)
{
    *index = -1;
    for(int i = 0; i < MAXWORDS; i++)
    {
        if(wordList[i] == NULL)
            continue;
        if(strcmp(wordList[i], word) != 0)
            continue;
        
        *index = i;
    }
}

void swap_words(char *wordList[MAXWORDS], char *word1, char *word2)
{
    int a, b;
    a = b = -1;
    for(int i = 0; i < MAXWORDS; i++)
    {
        if(a != -1 && b != -1)
        {
            char *tmp = wordList[a];
            wordList[a] = wordList[b];
            wordList[b] = tmp;
            return;
        }
        if(wordList[i] == NULL)
            continue;
        
        if(strcmp(wordList[i], word1) == 0)
            a = i;
        
        if(strcmp(wordList[i], word2) == 0)
            b = i;
    }
    printf("Word(s) not in list\n");
}

void longest_word(char *wordList[MAXWORDS], char **word)
{
    *word = NULL;
    if(wordList[0] == NULL)
        return;
    
    int longest_word = 0;
    for(int i = 1; i < MAXWORDS; i++)
    {
        if(wordList[i] == NULL)
            continue;
        if(strlen(wordList[longest_word]) <= strlen(wordList[i]))
            longest_word = i;
    }
    *word = wordList[longest_word];
}

void change_vowels(char *wordList[MAXWORDS])
{
    for(int i = 0; i < MAXWORDS; i++)
    {
        if(wordList[i] == NULL)
            continue;
            
        int length = strlen(wordList[i]) + 1;
        for(int c = 0; c < length; c++)
        {
            switch(wordList[i][c])
            {
                case 'e':
                    wordList[i][c] = 'a';
                break;
                case 'a':
                    wordList[i][c] = 'e';
                break;
                case 'u':
                    wordList[i][c] = 'o';
                break;
                case 'o':
                    wordList[i][c] = 'u';
                break;
            }
        }
    }
}

void share_word(char *wordList[MAXWORDS], char *word)
{
    //Find the word
    int index = -1;
    for(int i = 0; i < MAXWORDS; i++)
    {
        if(wordList[i] == NULL)
            continue;
        
        if(strcmp(wordList[i], word) != 0)
            continue;
        
        //Word is equal to current index
        index = i;
        break;
    }
    
    //Word not found -> insert behaviour
    if(index == -1)
    {
        insert_word(wordList, word);
        return;
    }
    
    //The position is taken -> shifting time
    //Start at the second last index and shift
    //Back until we free our desired index
    for(int i = MAXWORDS - 2; i >= index; i--)
    {
        if(wordList[i] == NULL)
            continue;
        
        wordList[i + 1] = wordList[i];
    }
    //Word found -> share behaviour
    wordList[index] = wordList[index + 1];
}

void delete_word(char *wordList[MAXWORDS], char *word)
{
    int index;
    find_word(wordList, word, &index);
    if(index < 0)
    {
        printf("Word not found\n");
        return;
    }
    
    //Not shared
    if(index == 0 || wordList[index] != wordList[index - 1])
    {
        if(wordList[index] != NULL)
            free(wordList[index]);
    }
    
    ///Shared
    wordList[index] = NULL;
    
    //Shift the list
    //Start at the following index and shift
    //forward until we reach the end of the list
    for(int i = index + 1; i < MAXWORDS; i++)
    {
        if(wordList[i] == NULL)
            continue;
        
        wordList[i - 1] = wordList[i];
        wordList[i] = NULL;
    }
}

void append_words(char *wordList[MAXWORDS], char *word1, char *word2)
{
    int idx_a, idx_b;
    find_word(wordList, word1, &idx_a);
    find_word(wordList, word2, &idx_b);
    
    //Missing one or both of the words
    if(idx_a < 0 || idx_b < 0)
    {
        printf("Word(s) not in list\n");
        return;
    }
    
    int length_a = strlen(word1);
    char *appended_p = (char *) malloc ((length_a + strlen(word2) + 2)*sizeof(char));
    //Combine the words in to the new space
    strcpy(appended_p, word1);
    appended_p[length_a] = '+';
    strcpy(appended_p + (length_a + 1), word2);
    
    free(wordList[idx_a]);
    wordList[idx_a] = appended_p;
    
    delete_word(wordList, word2);
}

void display_word_list (display_t *display, char *wordList[MAXWORDS])
{
    displayFillScreen(display, RGB_BLACK);
    int s = DISPLAY_HEIGHT/MAXWORDS;
    // longer words are allowed but later (incl. null) characters won’t be shown
    int maxlen = DISPLAY_WIDTH/s -3;
    for (int i = 0; i < MAXWORDS; i++) {
        if (wordList[i] == NULL) {
            displayDrawRect(display, 0, i*s, s-1, (i+1)*s-1, RGB_RED); // needs to be modified!
        } else {
            displayDrawRect(display, 0, i*s, s-1, (i+1)*s-1, RGB_WHITE); // needs to be modified!
            int shared = 0;
            for (int j = 0; j <= i; j++) {
                if (wordList[i] == wordList[j]) {
                    displayDrawLine(display, s/2, i*s+s/2, (3+0)*s+s/2, j*s+s/2, RGB_WHITE);
                    shared = (i != j);
                    break;
                }
            }
            if (!shared) {
                int len = strlen(wordList[i]) +1;
                if (len > maxlen) len = maxlen;
                for (int l = 0; l < len; l++) {
                    uint16_t colour = RGB_RED;
                    if(wordList[i][l] == '\0')
                        colour = RGB_YELLOW;

                    displayDrawRect(display, (3+l)*s, i*s, (4+l)*s-1, (i+1)*s-1, colour); // needs to be m
                }
            }
        }
    }

}

int main(void)
{
    pynq_init();
    display_t display;
    display_init(&display);
    
    char *wordList[MAXWORDS] = {NULL};
    char word[MAXLEN] = {'\0'};
    char aux_word[MAXLEN] = {'\0'};
    int index;
    
    char *longest = NULL;
    
    char cmd;
    do
    {
        printf("Command? ");
        scanf(" %c", &cmd);
        
        switch(cmd)
        {
            case 'q':
            break;
            
            case 'p':
                print_word_list(wordList);
            break;
            
            case 'i':
                printf("Word? ");
                scanf(" %s", word);
                insert_word(wordList, word);
            break;
            
            case 'f':
                printf("Word? ");
                scanf(" %s", word);
                find_word((char **)&wordList, word, &index);
                printf("The index is %d\n", index);
            break;
            
            case 'w':
                printf("Words? ");
                scanf(" %s %s", word, aux_word);
                swap_words(wordList, word, aux_word);
            break;
            
            case 'l':
                longest_word((char**)&wordList, &longest);
                if(longest != NULL)
                    printf("The longest word is %s\n", longest);
                else
                    printf("No words\n");
            break;
            
            case 'v':
                change_vowels(wordList);
            break;
            
            case 's':
                printf("Word? ");
                scanf(" %s", word);
                share_word(wordList, word);
            break;
            
            case 'a':
                printf("Words? ");
                scanf(" %s %s", word, aux_word);
                append_words(wordList, word, aux_word);
            break;
            
            case 'd':
                printf("Word? ");
                scanf(" %s", word);
                delete_word(wordList, word);
            break;
            
            //DISPLAY
            case 'D':
                display_word_list(&display, wordList);
            break;
            
            default:
                printf("Unknown command \'%c\'\n", cmd);
            break;
        }
        
    }while(cmd != 'q');
    
    //Clean up
    while(wordList[0] != NULL) {delete_word(wordList, wordList[0]);};
    
    display_destroy(&display);
    pynq_destroy();
    printf("Bye!\n");
}