#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include <time.h>  

#include "raylib.h"
#include "pthread.h"

#include "include/solve.h"

#define EasyMax 5

#define COLLEGE_NAME_FRAME      1
#define OEP_FRAME               2
#define PYRAMID_SOLITARE_FRAME  3
#define SELECTION_PLAY_SOLVE    4
#define PLAY_CHOICE             5
#define GAMEPLAY_EASY           6
#define GAMEPLAY_HARD           7
#define LOSE_SCREEN             8
#define WIN_SCREEN              9
#define SOLVE_CHOICE            10
#define KEYBOARD_RULES          11
#define KEYBOARD_ENTER          12
#define FILE_RULES              13
#define FILE_ENTER              14
#define CAN_NOT_SOLVE           15
#define CAN_SOLVE               16
#define FILE_WRITE              17
#define SOLVE_WITH_ANIMATION    18
#define CLOSE                   19
#define RATING                  20
#define EXIT                    21

const int WIDTH = 1860;
const int HEIGHT = 1000;


char ch[52][4]={"C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "C10", "C11", "C12", "C13",
                "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12", "D13",
                "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9", "H10", "H11", "H12", "H13",
                "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9", "S10", "S11", "S12", "S13"};

char numbers[][50]={"first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth", "tenth",
                    "eleventh", "twelfth", "thirteenth", "fourteenth", "fifteenth", "sixteenth", "seventeenth", "eighteenth", "nineteenth", "twentieth",
                    "twenty first", "twenty second", "twenty third", "twenty fourth"};

typedef struct tree2{
    int number;
    char suits;
    int rank;
    int flag;
    struct tree2 *child1, *child2, *parent1, *parent2;
    Texture2D cardphoto;
    Vector2 pos;
}tree3;
tree3 dealt3[28] = {0};

typedef struct doubly_linked_list{
    int number;
    char suits;
    int rank;
    struct doubly_linked_list *prev, *next;
    Texture2D cardphoto;
}double_linked_list3;
double_linked_list3 stock3[24] = {0}, *first1 = &stock3[0], *up = NULL;

typedef struct{
    int no;
    union{
        tree3 *dealt3;
        double_linked_list3 *stock3;
    }c;
}Card;
Card selcard[2];

int free_cards2[7], card_i = 0, restock3ed = 0;
Rectangle btnBounds_free_cards2[7], stock3_press[2] = {0}, stock3_change = {1400, 425, 120, 200};
int pf[7] = {0}, points = 0, p[] = {0, 1, 3, 6, 10, 15, 21};

void draw_cards(){
    int number = 0;
    for(int i = 0; i<7; i++)
        for(int j = 0; j<i+1; j++, number++)
            DrawTexture(dealt3[number].cardphoto, dealt3[number].pos.x, dealt3[number].pos.y, WHITE);
    if(up == NULL)
        DrawTexture(first1->cardphoto, 1400, 600, WHITE);
    else if(up->next == NULL)
        DrawTexture(up->cardphoto, 1400, 200, WHITE);
    else{
        DrawTexture(up->cardphoto, 1400, 200, WHITE);
        DrawTexture(up->next->cardphoto, 1400, 600, WHITE);        
    }
    Image temp = LoadImage(TextFormat("images/nextstock3.png"));
    ImageResize(&temp, 120, 200);
    Texture2D next = LoadTextureFromImage(temp);
    DrawTexture(next, 1400, 425, WHITE);
}

void assign(){
    int x = WIDTH/2 - 300, y = 30;
    for(int i = 0, number = 0, pos2 = -2, pos1 = -1; i<7; i++){
        for(int j = 0; j<i+1; j++, number++){
            if(number == 0){
                dealt3[0].parent1 = NULL;
                dealt3[0].parent2 = NULL;
            }
            else if(number-i-1 == pos2){
                dealt3[number].parent1 = NULL;
                dealt3[number].parent2 = &dealt3[pos2+1];
                dealt3[pos2+1].child1 = &dealt3[number];
            }
            else if(number-i-1 == pos1){
                dealt3[number].parent1 = &dealt3[pos1];
                dealt3[number].parent2 = NULL;
                dealt3[pos1].child2 = &dealt3[number];
            }
            else{
                dealt3[number].parent1 = &dealt3[number-i-1];
                dealt3[number].parent2 = &dealt3[number-i];
                dealt3[number-i].child1 = &dealt3[number];
                dealt3[number-i-1].child2 = &dealt3[number];
            }
            dealt3[number].pos.x = x - (i*125)/2.0 + (j*125);
            dealt3[number].pos.y = y + i*90;
        }
        pos2=(pos2>-1)?pos2+i:pos2+1;
        pos1=(pos1>-1)?pos1+i+1:pos1+1;
    }
    for(int i = 0; i<24; i++){
        if(i==0){
            stock3[0].prev = NULL;
            stock3[0].next = &stock3[1];
        }
        else if(i==23){
            stock3[23].prev = &stock3[22];
            stock3[23].next = NULL;
        }
        else{
            stock3[i].prev = &stock3[i-1];
            stock3[i].next = &stock3[i+1];
        }
    }
}

void loadEasy(void){
    struct card{
        char suit;
        int number;
    }ca[52];
    srand(time(0));
    int i, j, f = 1;
    FILE *fp = fopen(TextFormat("files/easy/%d.txt",rand()%EasyMax), "r");
    for(i = 0; i<52; i++)
        fscanf(fp,"%c\t%d\n",&ca[i].suit,&ca[i].number);
    fclose(fp);
    fp = fopen("cards.txt", "w");
    Image temp;
    for(i = 0; i < 52; i++){
        j = i-28;
        fprintf(fp, "%c\t%d\n", ca[i].suit, ca[i].number);
        if(i > 27)
            f = 0;
        if(f){
            dealt3[i].number = i;
            dealt3[i].suits = ca[i].suit;
            dealt3[i].rank = ca[i].number;
            temp = LoadImage(TextFormat("images/cards/%c%d.png", dealt3[i].suits, dealt3[i].rank));
            ImageResize(&temp, 120, 200);
            dealt3[i].cardphoto = LoadTextureFromImage(temp);
        }
        else{
            stock3[j].number = j;
            stock3[j].suits = ca[i].suit;
            stock3[j].rank = ca[i].number;
            temp = LoadImage(TextFormat("images/cards/%c%d.png", stock3[j].suits, stock3[j].rank));
            ImageResize(&temp, 120, 200);
            stock3[j].cardphoto = LoadTextureFromImage(temp);
        }
    }
    assign();
    fclose(fp);
}

void shuffle(void){
    struct card{
        char suit;
        int number;
    }ca[52], temp;
    srand(time(0));
    int i, j, f = 1;
    FILE *fp = fopen("files/deck.txt", "r");

    for(i = 0; i<52; i++)
        fscanf(fp, "%c\t%d\n", &ca[i].suit, &ca[i].number);

    //FISHER YATES shuffle algorithm
    for(i = 51; i > 0; i--){
        j = rand()%(i+1);
        temp = ca[i];
        ca[i] = ca[j];
        ca[j] = temp;
    }
    fclose(fp);
    
    fp = fopen("cards.txt","w");
    for(i = 0; i < 52; i++){
        fprintf(fp,"%c\t%d\n", ca[i].suit, ca[i].number);
        if(i > 27){
            f = 0;
            j = i-28;
        }
        if(f){
            dealt3[i].number = i;
            dealt3[i].suits = ca[i].suit;
            dealt3[i].rank = ca[i].number;
            Image temp = LoadImage(TextFormat("images/cards/%c%d.png", dealt3[i].suits, dealt3[i].rank));
            ImageResize(&temp, 120, 200);
            dealt3[i].cardphoto = LoadTextureFromImage(temp);
        }
        else{
            stock3[j].number = j;
            stock3[j].suits = ca[i].suit;
            stock3[j].rank = ca[i].number;
            Image temp = LoadImage(TextFormat("images/cards/%c%d.png", stock3[j].suits, stock3[j].rank));
            ImageResize(&temp, 120, 200);
            stock3[j].cardphoto = LoadTextureFromImage(temp);
        }
    }
    assign();
    fclose(fp);
}

void points1(){
    if(pf[6] == 7){
        points += 25;
        pf[6]++;
    }
    if(pf[5] == 6){
        points += 50;
        pf[5]++;
    }
    if(pf[4] == 5){
        points += 75;
        pf[4]++;
    }
    if(pf[3] == 4){
        points += 100;
        pf[3]++;
    }
    if(pf[2] == 3){
        points += 150;
        pf[2]++;
    }    
    if(pf[1] == 2){
        points += 200;
        pf[1]++;
    }
    if(pf[0] == 1){
        points += 500;
        pf[0]++;
    }
    points += 5;
}

void check_for_free_cards2(){
    int j = 0;
    for(int i = 0; i<28; i++)
        if(dealt3[i].flag == 0)
            if(dealt3[i].child1 == NULL && dealt3[i].child2 == NULL){
                Rectangle temp = {dealt3[i].pos.x, dealt3[i].pos.y, 120, 200};
                btnBounds_free_cards2[j] = temp;
                free_cards2[j++] = i;
            }
    while(j<7){
        free_cards2[j] = -1;
        Rectangle temp = {0};
        btnBounds_free_cards2[j] = temp;
        j++;
    }
    if(up == NULL){
        Rectangle temp1 = {0}, temp2 = {1400, 600, 120, 200};
        stock3_press[0] = temp1;
        stock3_press[1] = temp2;
    }
    else if(up->next == NULL){
        Rectangle temp1 = {1400, 200, 120, 200}, temp2 = {0};
        stock3_press[0] = temp1;
        stock3_press[1] = temp2;
    }
    else{
        Rectangle temp1 = {1400, 200, 120, 200}, temp2 = {1400, 600, 120, 200};
        stock3_press[0] = temp1;
        stock3_press[1] = temp2;        
    }
}

void add_to_free_cards2(int m1, int m2){
    selcard[card_i].no = m1;
    if(m1 == 1)
        selcard[card_i].c.dealt3 = &dealt3[m2];
    else if(m1 == 2){
        if(m2 == 0)
            selcard[card_i].c.stock3 = up;
        else
            if(up == NULL)
                selcard[card_i].c.stock3 = first1;
            else
                selcard[card_i].c.stock3 = up->next;
    }
    card_i++;
}

void can_remove(){
    Card s = {0};
    if(card_i == 1){
        if(selcard[0].no == 1){
            if(selcard[0].c.dealt3->rank == 13){
                Texture2D temp= {0};
                selcard[0].c.dealt3->cardphoto = temp;
                if(selcard[0].c.dealt3->number>=p[6])
                    pf[6]++;
                else if(selcard[0].c.dealt3->number>=p[5])
                    pf[5]++;
                else if(selcard[0].c.dealt3->number>=p[4])
                    pf[4]++;
                else if(selcard[0].c.dealt3->number>=p[3])
                    pf[3]++;
                else if(selcard[0].c.dealt3->number>=p[2])
                    pf[2]++;
                else if(selcard[0].c.dealt3->number>=p[1])
                    pf[1]++;
                else
                    pf[0]++;
                if(selcard[0].c.dealt3->parent1 != NULL)
                    selcard[0].c.dealt3->parent1->child2 = NULL;
                if(selcard[0].c.dealt3->parent2 != NULL)
                    selcard[0].c.dealt3->parent2->child1 = NULL;
                selcard[0].c.dealt3->flag = 1;
                card_i = 0;
                selcard[0] = s;
                points1();
            }
        }
        else{
            if(selcard[0].c.stock3->rank == 13){
                if(selcard[0].c.stock3->prev != NULL)
                    selcard[0].c.stock3->prev->next = selcard[0].c.stock3->next;
                if(first1 == selcard[0].c.stock3)
                    first1 = selcard[0].c.stock3->next;
                if(up == selcard[0].c.stock3 && up->prev == NULL){
                    first1 = selcard[0].c.stock3->next;
                    up = NULL;
                }
                if(selcard[0].c.stock3->next != NULL)
                    selcard[0].c.stock3->next->prev = selcard[0].c.stock3->prev;
                if(up == selcard[0].c.stock3)
                    up = selcard[0].c.stock3->prev;
                card_i = 0;
                points1();
                selcard[0] = s;
            }            
        }
    }
    else if(card_i == 2){
        if(selcard[0].no == 1 && selcard[1].no == 1){
            if(selcard[0].c.dealt3->rank + selcard[1].c.dealt3->rank == 13){
                Texture2D temp= {0};
                selcard[0].c.dealt3->cardphoto = temp;
                selcard[0].c.dealt3->flag = 1;
                if(selcard[0].c.dealt3->number>=p[6])
                    pf[6]++;
                else if(selcard[0].c.dealt3->number>=p[5])
                    pf[5]++;
                else if(selcard[0].c.dealt3->number>=p[4])
                    pf[4]++;
                else if(selcard[0].c.dealt3->number>=p[3])
                    pf[3]++;
                else if(selcard[0].c.dealt3->number>=p[2])
                    pf[2]++;
                else if(selcard[0].c.dealt3->number>=p[1])
                    pf[1]++;
                else
                    pf[0]++;
                if(selcard[0].c.dealt3->parent1 != NULL)
                    selcard[0].c.dealt3->parent1->child2 = NULL;
                if(selcard[0].c.dealt3->parent2 != NULL)
                    selcard[0].c.dealt3->parent2->child1 = NULL;
                
                selcard[1].c.dealt3->cardphoto = temp;
                selcard[1].c.dealt3->flag = 1;
                if(selcard[1].c.dealt3->number>=p[6])
                    pf[6]++;
                else if(selcard[1].c.dealt3->number>=p[5])
                    pf[5]++;
                else if(selcard[1].c.dealt3->number>=p[4])
                    pf[4]++;
                else if(selcard[1].c.dealt3->number>=p[3])
                    pf[3]++;
                else if(selcard[1].c.dealt3->number>=p[2])
                    pf[2]++;
                else if(selcard[1].c.dealt3->number>=p[1])
                    pf[1]++;
                else
                    pf[0]++;
                if(selcard[1].c.dealt3->parent1 != NULL)
                    selcard[1].c.dealt3->parent1->child2 = NULL;
                if(selcard[1].c.dealt3->parent2 != NULL)
                    selcard[1].c.dealt3->parent2->child1 = NULL;
                points1();
            }
        }            
        else if(selcard[0].no == 1 && selcard[1].no == 2){
            if(selcard[0].c.dealt3->rank + selcard[1].c.stock3->rank == 13){
                Texture2D temp= {0};
                selcard[0].c.dealt3->cardphoto = temp;
                selcard[0].c.dealt3->flag = 1;
                if(selcard[0].c.dealt3->number>=p[6])
                    pf[6]++;
                else if(selcard[0].c.dealt3->number>=p[5])
                    pf[5]++;
                else if(selcard[0].c.dealt3->number>=p[4])
                    pf[4]++;
                else if(selcard[0].c.dealt3->number>=p[3])
                    pf[3]++;
                else if(selcard[0].c.dealt3->number>=p[2])
                    pf[2]++;
                else if(selcard[0].c.dealt3->number>=p[1])
                    pf[1]++;
                else
                    pf[0]++;
                if(selcard[0].c.dealt3->parent1 != NULL)
                    selcard[0].c.dealt3->parent1->child2 = NULL;
                if(selcard[0].c.dealt3->parent2 != NULL)
                    selcard[0].c.dealt3->parent2->child1 = NULL;
                if(first1 == selcard[1].c.stock3)
                    first1 = selcard[1].c.stock3->next;
                if(up == selcard[1].c.stock3 && up->prev == NULL){
                    first1 = selcard[1].c.stock3->next;
                    up = NULL;
                }
                if(selcard[1].c.stock3->prev != NULL)
                    selcard[1].c.stock3->prev->next = selcard[1].c.stock3->next;
                if(selcard[1].c.stock3->next != NULL)
                    selcard[1].c.stock3->next->prev = selcard[1].c.stock3->prev;
                if(up == selcard[1].c.stock3){
                    up = selcard[1].c.stock3->prev;
                }
                points1();
            }
        }
        else if(selcard[0].no == 2 && selcard[1].no == 1){
            if(selcard[1].c.dealt3->rank + selcard[0].c.stock3->rank == 13){
                Texture2D temp= {0};
                selcard[1].c.dealt3->cardphoto = temp;
                selcard[1].c.dealt3->flag = 1;
                if(selcard[1].c.dealt3->number>=p[6])
                    pf[6]++;
                else if(selcard[1].c.dealt3->number>=p[5])
                    pf[5]++;
                else if(selcard[1].c.dealt3->number>=p[4])
                    pf[4]++;
                else if(selcard[1].c.dealt3->number>=p[3])
                    pf[3]++;
                else if(selcard[1].c.dealt3->number>=p[2])
                    pf[2]++;
                else if(selcard[1].c.dealt3->number>=p[1])
                    pf[1]++;
                else
                    pf[0]++;
                if(selcard[1].c.dealt3->parent1 != NULL)
                    selcard[1].c.dealt3->parent1->child2 = NULL;
                if(selcard[1].c.dealt3->parent2 != NULL)
                    selcard[1].c.dealt3->parent2->child1 = NULL;                
                
                if(first1 == selcard[0].c.stock3)
                    first1 = selcard[0].c.stock3->next;
                if(up == selcard[0].c.stock3 && up->prev == NULL){
                    first1 = selcard[0].c.stock3->next;
                    up = NULL;
                }
                if(selcard[0].c.stock3->prev != NULL)
                    selcard[0].c.stock3->prev->next = selcard[0].c.stock3->next;
                if(selcard[0].c.stock3->next != NULL)
                    selcard[0].c.stock3->next->prev = selcard[0].c.stock3->prev;
                if(up == selcard[0].c.stock3){
                    up = selcard[0].c.stock3->prev;
                }
                points1();
            }
        }
        else{
            if(selcard[0].c.stock3->rank + selcard[1].c.stock3->rank == 13){
                if(selcard[0].c.stock3->next == selcard[1].c.stock3){
                    up = selcard[0].c.stock3->prev;
                    if(first1 == selcard[0].c.stock3)
                        first1 = selcard[0].c.stock3->next;
                    if(up == selcard[0].c.stock3 && up->prev == NULL){
                        first1 = selcard[0].c.stock3->next;
                        up = NULL;
                    }

                    if(first1 == selcard[1].c.stock3)
                        first1 = selcard[1].c.stock3->next;
                    if(up == selcard[1].c.stock3 && up->prev == NULL){
                        first1 = selcard[1].c.stock3->next;
                        up = NULL;
                    }
                    if(up != NULL)
                        up->next = selcard[1].c.stock3->next;
                    if(up->next != NULL)
                        selcard[1].c.stock3->next->prev = up;
                }
                else{
                    up = selcard[1].c.stock3->prev;
                    if(up != NULL)
                        up->next = selcard[0].c.stock3->next;
                    if(up->next != NULL)
                        selcard[0].c.stock3->next->prev = up;
                }
                points1();
            }                
        }
        selcard[0] = s;
        selcard[1] = s;
        
        card_i = 0;
    }
}

void redo(){
    int i, j = 0 ,f = 1;
    struct card{
        char suit;
        int number;
    }ca[52];
    FILE *fp = fopen("cards.txt","r");
    for(i = 0; i < 52; i++){
        fscanf(fp, "%c\t%d\n", &ca[i].suit, &ca[i].number);
        if(i>27){
            f = 0;
            j = i-28;
        }
        if(f){
            dealt3[i].suits = ca[i].suit;
            dealt3[i].rank = ca[i].number;
            dealt3[i].flag = 0;
            Image temp = LoadImage(TextFormat("images/cards/%c%d.png", dealt3[i].suits, dealt3[i].rank));
            ImageResize(&temp, 120, 200);
            dealt3[i].cardphoto = LoadTextureFromImage(temp);
        }
        else{
            stock3[j].suits = ca[i].suit;
            stock3[j].rank = ca[i].number;
            Image temp = LoadImage(TextFormat("images/cards/%c%d.png", stock3[j].suits, stock3[j].rank));
            ImageResize(&temp, 120, 200);
            stock3[j].cardphoto = LoadTextureFromImage(temp);
        }
    }
    fclose(fp);
    assign();
    first1 = &stock3[0];
    up = NULL;
    card_i = 0, restock3ed = 0;
}

int standard_deck(char suit, int rank){
    int count = 0;
    if(standard.count != 0){
        while(count != standard.count){
            if(standard.standard[count].suit == suit && standard.standard[count].rank == rank)
                return 1;
        count++;
        }
    }
    standard.count++;
    standard.standard[count].suit = suit;
    standard.standard[count].rank = rank;
    return 0;
}

bool IsAnyKeyPressed(){
    bool keyPressed = false;
    int key = GetKeyPressed();
    if ((key >= 32) && (key <= 126)) keyPressed = true;
    return keyPressed;
}

int submit_to_tree(int number, char input[]){
    switch(input[0]){
        case 'c':
        case 'C':   dealt1[number].suits = 'C';
                    break;
        case 'd':
        case 'D':   dealt1[number].suits = 'D';
                    break;
        case 'h':
        case 'H':   dealt1[number].suits = 'H';
                    break;
        case 's':
        case 'S':   dealt1[number].suits = 'S';
                    break;
        default: return 0;
    }
    switch(input[1]){
        case 'a':
        case 'A':   dealt1[number].rank = 1;
                    break;
        case '2':   dealt1[number].rank = 2;
                    break;
        case '3':   dealt1[number].rank = 3;
                    break;
        case '4':   dealt1[number].rank = 4;
                    break;
        case '5':   dealt1[number].rank = 5;
                    break;
        case '6':   dealt1[number].rank = 6;
                    break;
        case '7':   dealt1[number].rank = 7;
                    break;
        case '8':   dealt1[number].rank = 8;
                    break;
        case '9':   dealt1[number].rank = 9;
                    break;
        case 't':
        case 'T':   dealt1[number].rank = 10;
                    break;
        case 'j':
        case 'J':   dealt1[number].rank = 11;
                    break;
        case 'q':
        case 'Q':   dealt1[number].rank = 12;
                    break;
        case 'k':
        case 'K':   dealt1[number].rank = 13;
                    break;
        default:    return 0;
    }
    int no = standard_deck(dealt1[number].suits, dealt1[number].rank);
    if(no == 1){
        dealt1[number].suits = 0, dealt1[number].rank = 0;
        return 1;
    }
    Image temp = LoadImage(TextFormat("images/cards/%c%d.png", dealt1[number].suits, dealt1[number].rank));
    ImageResize(&temp, 120, 200);
    dealt1[number].card_photo = LoadTextureFromImage(temp);
    return 2;
}

int submit_to_stock(int number, char input[]){
    switch(input[0]){
        case 'c':
        case 'C':   stock1[number].suits = 'C';
                    break;
        case 'd':
        case 'D':   stock1[number].suits = 'D';
                    break;
        case 'h':
        case 'H':   stock1[number].suits = 'H';
                    break;
        case 's':
        case 'S':   stock1[number].suits = 'S';
                    break;
        default: return 0;
    }
    switch(input[1]){
        case 'a':
        case 'A':   stock1[number].rank = 1;
                    break;
        case '2':   stock1[number].rank = 2;
                    break;
        case '3':   stock1[number].rank = 3;
                    break;
        case '4':   stock1[number].rank = 4;
                    break;
        case '5':   stock1[number].rank = 5;
                    break;
        case '6':   stock1[number].rank = 6;
                    break;
        case '7':   stock1[number].rank = 7;
                    break;
        case '8':   stock1[number].rank = 8;
                    break;
        case '9':   stock1[number].rank = 9;
                    break;
        case 't':
        case 'T':   stock1[number].rank = 10;
                    break;
        case 'j':
        case 'J':   stock1[number].rank = 11;
                    break;
        case 'q':
        case 'Q':   stock1[number].rank = 12;
                    break;
        case 'k':
        case 'K':   stock1[number].rank = 13;
                    break;
        default:    return 0;
    }
    int no = standard_deck(stock1[number].suits, stock1[number].rank);
    if(no == 1){
        stock1[number].suits = 0, stock1[number].rank = 0;
        return 1;
    }
    Image temp = LoadImage(TextFormat("images/cards/%c%d.png", stock1[number].suits, stock1[number].rank));
    ImageResize(&temp, 130, 200);
    stock1[number].card_photo = LoadTextureFromImage(temp);
    return 2;
}

void draw_cards1(){
    int number = 0;
    int x = WIDTH/2 - 300, y = 30;
    for(int i = 0; i<7; i++){
        for(int j = 0; j<i+1; j++, number++){
            DrawTexture(dealt1[number].card_photo, x - (i*125)/2.0 + (j*125), y + i*90, WHITE);
        }
    }
    for(int i = 0; i<24; i++)
        DrawTexture(stock1[i].card_photo, 1400, i*32 + 30, WHITE);
}

void draw(){
    int number = 0;
    int x = WIDTH/2 - 300, y = 30;
    for(int i = 0; i<7; i++){
        for(int j = 0; j<i+1; j++, number++){
            dealt1[number].pos.x = x - (i*125)/2.0 + (j*125);
            dealt1[number].pos.y = y + i*90;
            DrawTexture(dealt1[number].card_photo, dealt1[number].pos.x, dealt1[number].pos.y, WHITE);
        }
    }
    if(upper_stock == NULL)
        DrawTexture(first->card_photo, 1400, 600, WHITE);
    else if(upper_stock->next == NULL)
        DrawTexture(upper_stock->card_photo, 1400, 200, WHITE);
    else{
        DrawTexture(upper_stock->card_photo, 1400, 200, WHITE);
        DrawTexture(upper_stock->next->card_photo, 1400, 600, WHITE);
    }    
    Image temp = LoadImage(TextFormat("images/nextstock.png"));
    ImageResize(&temp, 120, 200);
    Texture2D next = LoadTextureFromImage(temp);
    DrawTexture(next, 1400, 425, WHITE);
}

int copy(FILE *fp,int *number){
    int error;
    char s[10];
    for(int i = 0; i<28; i++){
        fscanf(fp, "%s", s);
        error = submit_to_tree(i, s);
        if(error != 2)
            return 1;
    }
    for(int i = 0; i<24; i++){
        fscanf(fp, "%s", s);
        error = submit_to_stock(i, s);
        if(error != 2)
            return 1;
    }
    *number = 52;
    return 0;
}

int main(void){
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    InitWindow(WIDTH, HEIGHT, "Open Ended Program");
    SetWindowPosition(30, 60);
    InitAudioDevice();
    
    int choice = CAN_NOT_SOLVE, delay = 0;
    float font_size, spacings, scrollingText = WIDTH + 100, scrollingTextPos = -100;
    char buffer[100];
    Image background_i = {0}, logo_i = {0};
    Texture2D background_t = {0}, logo_t = {0};
    Font fonts = { 0 };
    Vector2 positions = { 0 }, mousePoint = { 0.0f, 0.0f };
    Color color ={ 0 };
    
    Image temp = {0};
    temp = LoadImage("images/button/play.png");
    ImageResize(&temp, 400, 150);
    Texture2D button1 = LoadTextureFromImage(temp);
    
    temp = LoadImage("images/button/solve.png");
    ImageResize(&temp, 400, 150);
    Texture2D button2 = LoadTextureFromImage(temp);
    
    Rectangle sourceRec1 = { 0, 0, button1.width, button1.height };
    Rectangle sourceRec2 = { 0, 0, button2.width, button2.height };
    
    Rectangle btnBounds1 = { 100, HEIGHT/2 - button1.height/2, button1.width, button1.height };
    Rectangle btnBounds2 = { WIDTH-button2.width-100, HEIGHT/2 - button2.height/2, button2.width, button2.height };
    
    temp = LoadImage("images/button/easy_game.png");
    ImageResize(&temp, 400, 150);
    Texture2D button3 = LoadTextureFromImage(temp);
    
    temp = LoadImage("images/button/hard_game.png");
    ImageResize(&temp, 400, 150);
    Texture2D button4 = LoadTextureFromImage(temp);
    
    temp = LoadImage("images/button/4.png");
    ImageResize(&temp, 100, 50);
    Texture2D ngt = LoadTextureFromImage(temp);
    
    temp = LoadImage("images/button/3.png");
    ImageResize(&temp, 100, 50);
    Texture2D rbt = LoadTextureFromImage(temp);
    
    Rectangle sourceRec3 = { 0, 0, button3.width, button3.height };
    Rectangle sourceRec4 = { 0, 0, button4.width, button4.height };
    Rectangle srng = { 0, 0, ngt.width, ngt.height };
    Rectangle srrb = { 0, 0, rbt.width, rbt.height };

    Rectangle btnBounds3 = { 100, HEIGHT/2 - button3.height/2, button3.width, button3.height };
    Rectangle btnBounds4 = { WIDTH-button4.width-100, HEIGHT/2 - button4.height/2, button4.width, button4.height };
    Rectangle bbng = { 300,900 , 100, 50 };
    Rectangle bbrb = { 450,900,100,50 };
    
    bool btnAction1 = false, btnAction2 = false, ng = false, rb = false;
    int m1, m2;
    
    
    char input[100] = "\0", **droppedFiles = {0};
    int letterCount = 0;

    Rectangle textBox = {WIDTH/2 + 40, HEIGHT-150, 225, 50 };
    Rectangle textBox1 = {400, HEIGHT/2 - 50, 1000, 50 };
    bool mouseOnText = false;
    int framesCounter = 0, framecounter = 0;
    int i = 0, j = 0, number = 0, error = 0, error_no = -1;
        
    Music background_music = LoadMusicStream("music/Background.mp3");
    PlayMusicStream(background_music); 
    SetMusicVolume(background_music, 1);
    Sound click_sound = LoadSound("music/card_click.wav");

    pthread_t threadId1, threadId2;
    
    temp = LoadImage("images/button/close.png");
    ImageResize(&temp, 100, 100);
    Texture2D close = LoadTextureFromImage(temp);
    Rectangle closeRec1 = { 0, 0, close.width, close.height };
    Rectangle closeBounds1 = { WIDTH - 100, 0, WIDTH, close.height };
    bool btnclose = false;
    
    int rating = 0, number5 = 0;
    Image rating1 = {0}, rating2 = {0};
    Texture2D rating1_t = {0}, rating2_t = {0};
    rating1 = LoadImage("images/rating1.png");
    ImageResize(&rating1, 60, 60);
    rating1_t = LoadTextureFromImage(rating1);
    rating2 = LoadImage("images/rating2.png");
    ImageResize(&rating2, 60, 60);
    rating2_t = LoadTextureFromImage(rating2);

    fonts = LoadFont("fonts/jupiter_crash.png");
    Image cursor = LoadImage("images/cursor.png");
    ImageResize(&cursor, 14, 22);
    Texture2D cursor_t = LoadTextureFromImage(cursor);
    Sound click_sound2 = LoadSound("music/card_click.wav");
                       
    SetTargetFPS(60);
    while (!WindowShouldClose()){
        UpdateMusicStream(background_music);
        
        if(!(choice == CLOSE || choice == RATING || choice == EXIT)){
            mousePoint = GetMousePosition();        
            if (CheckCollisionPointRec(mousePoint, closeBounds1))
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnclose = true;
            if (btnclose){
                temp = LoadImage("images/button/rate.png");
                ImageResize(&temp, 400, 150);
                button1 = LoadTextureFromImage(temp);
                
                temp = LoadImage("images/button/exit.png");
                ImageResize(&temp, 400, 150);
                button2 = LoadTextureFromImage(temp);
                
                Rectangle temp1 = { 0, 0, button1.width, button1.height };
                sourceRec1 = temp1;
                Rectangle temp2 = { 0, 0, button2.width, button2.height };
                sourceRec2 = temp2;

                Rectangle temp3 = { 100, HEIGHT/2 - button1.height/2, button1.width, button1.height };
                btnBounds1 = temp3;
                Rectangle temp4 = { WIDTH-button2.width-100, HEIGHT/2 - button2.height/2, button2.width, button2.height };
                btnBounds2 = temp4;
                
                choice = CLOSE;
                PlaySound(click_sound);
            }
        }

        switch(choice){
            case COLLEGE_NAME_FRAME:{
                break;
            }
            case OEP_FRAME:{
                break;
            }
            case PYRAMID_SOLITARE_FRAME:{
                if (scrollingText > scrollingTextPos)
                    scrollingText -= 35;
                break;
            }
            case SELECTION_PLAY_SOLVE:{
                mousePoint = GetMousePosition();
                
                if (CheckCollisionPointRec(mousePoint, btnBounds1))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction1 = true;

                if (CheckCollisionPointRec(mousePoint, btnBounds2))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction2 = true;

                if (btnAction1){
                    choice = PLAY_CHOICE;
                    PlaySound(click_sound);
                    btnAction1 = btnAction2 = false;
                }
                if (btnAction2){
                    choice = SOLVE_CHOICE;
                    temp = LoadImage("images/button/keyboard.png");
                    ImageResize(&temp, 400, 150);
                    button1 = LoadTextureFromImage(temp);
                    
                    temp = LoadImage("images/button/file.png");
                    ImageResize(&temp, 400, 150);
                    button2 = LoadTextureFromImage(temp);
                    
                    Rectangle temp1 = { 0, 0, button1.width, button1.height };
                    sourceRec1 = temp1;
                    Rectangle temp2 = { 0, 0, button2.width, button2.height };
                    sourceRec2 = temp2;

                    Rectangle temp3 = { 100, HEIGHT/2 - button1.height/2, button1.width, button1.height };
                    btnBounds1 = temp3;
                    Rectangle temp4 = { WIDTH-button2.width-100, HEIGHT/2 - button2.height/2, button2.width, button2.height };
                    btnBounds2 = temp4;
                    PlaySound(click_sound);
  
                    btnAction1 = false, btnAction2 = false;
                }
                break;
            }
            case PLAY_CHOICE:{
                mousePoint = GetMousePosition();
                
                if (CheckCollisionPointRec(mousePoint, btnBounds3))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction1 = true;

                if (CheckCollisionPointRec(mousePoint, btnBounds4))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction2 = true;

                if (btnAction1){
                    choice = GAMEPLAY_EASY;
                    loadEasy();
                    btnAction1 = false, btnAction2 = false;
                }
                if (btnAction2){
                    choice = GAMEPLAY_HARD;
                    shuffle();
                    btnAction1 = false, btnAction2 = false;
                }
                break;
            }
            case GAMEPLAY_EASY:
            case GAMEPLAY_HARD:{
                check_for_free_cards2();
                
                mousePoint = GetMousePosition();
                
                for(int i = 0; i < 7; i++ )
                    if (CheckCollisionPointRec(mousePoint, btnBounds_free_cards2[i]))
                        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
                            btnAction1 = true;
                            m1 = 1;
                            m2 = free_cards2[i];
                        }
                if (CheckCollisionPointRec(mousePoint, stock3_press[0]))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
                        btnAction1 = true;
                        m1 = 2;
                        m2 = 0;
                    }
                if (CheckCollisionPointRec(mousePoint, stock3_press[1]))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
                        btnAction1 = true;
                        m1 = 2;
                        m2 = 1;
                    }
                if(btnAction1){
                    add_to_free_cards2(m1, m2);
                    can_remove();
                    btnAction1 = false;
                }
                if (CheckCollisionPointRec(mousePoint, stock3_change))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                        btnAction2 = true;
                if(btnAction2){
                    if(up == NULL)
                        up = first1;
                    else if(up->next == NULL){
                        up = NULL;
                        restock3ed++; 
                    }
                    else
                        up = up->next;
                    btnAction2 = false;
                }
                
                if (CheckCollisionPointRec(mousePoint,bbng))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                        ng = true;
                    
                if (CheckCollisionPointRec(mousePoint,bbrb))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                        rb = true;
                                   
                if(ng){
                    choice = PLAY_CHOICE;
                    ng = false;
                    points = 0;
                    assign();
                    first1 = &stock3[0];
                    up = NULL;
                    card_i = 0, restock3ed = 0;
                    redo();
                    for(int asd = 0; asd < 7; asd++)
                        pf[asd] = 0;
                }
                
                if(rb){
                    redo();
                    rb = false;
                    points = 0;
                    for(int asd = 0; asd < 7; asd++)
                        pf[asd] = 0;
                }
                
                if(pf[0] == 2)
                    choice = WIN_SCREEN;
                if(restock3ed == 3)
                    choice = LOSE_SCREEN;
                if(first1 == NULL)
                    choice = LOSE_SCREEN;
                break;
            }
            case SOLVE_CHOICE:{
                mousePoint = GetMousePosition();
                
                if (CheckCollisionPointRec(mousePoint, btnBounds1))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction1 = true;

                if (CheckCollisionPointRec(mousePoint, btnBounds2))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction2 = true;

                if (btnAction1){
                    choice = KEYBOARD_RULES;
                    btnAction1 = false;
                    PlaySound(click_sound);
                    HideCursor();
                }
                if (btnAction2){
                    choice = FILE_RULES;
                    btnAction1 = false;
                    PlaySound(click_sound);
                    HideCursor();
                }
                break;
            }
            case KEYBOARD_RULES:{
                if(IsKeyPressed(KEY_ENTER)){
                    choice = KEYBOARD_ENTER;
                    ShowCursor();
                }
                break;
            }
            case KEYBOARD_ENTER:{
                framecounter++;
                if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;
                else mouseOnText = false;

                if (mouseOnText){
                    int key = GetKeyPressed();
                    if ((key >= 32) && (key <= 125) && (letterCount < 2)){
                        input[letterCount] = (char)key;
                        letterCount++;
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)){
                        letterCount--;
                        input[letterCount] = '\0';
                        if (letterCount < 0) letterCount = 0;
                    }
                }

                if (mouseOnText) framesCounter++;
                else framesCounter = 0;
                
                if(IsKeyPressed(KEY_ENTER) && number < 52){
                    if(number < 28){
                        if(error_no = submit_to_tree(number, input), error_no == 2){
                            input[0] = '\0';
                            input[1] = '\0';
                            letterCount = 0;
                            number++;
                            j++;
                        }else if(error_no == 0 ){
                            input[0] = '\0';
                            input[1] = '\0';
                            letterCount = 0;
                            error = 40;
                        }else{
                            input[0] = '\0';
                            input[1] = '\0';
                            letterCount = 0;
                            error = 40;                            
                        }
                        
                        if(j-1 >= i){
                            i++;
                            j = 0;
                        }
                    }
                    else if(number < 52){
                        if(error_no = submit_to_stock(number - 28, input), error_no == 2){
                            input[0] = '\0';
                            input[1] = '\0';
                            letterCount = 0;
                            number++;
                        }else if(error_no == 0){
                            input[0] = '\0';
                            input[1] = '\0';
                            letterCount = 0;
                            error = 40;
                        }else{
                            input[0] = '\0';
                            input[1] = '\0';
                            letterCount = 0;
                            error = 40;                            
                        }
                    }
                    
                    if(error == 0)
                        error_no = -1;
                }
                if(number == 52){
                    number = 53;
                    pthread_create(&threadId1, NULL, &LoadDataThread, NULL);
                    pthread_create(&threadId2, NULL, &solve, NULL);
                }
                if(solved == 1 && timecounter > 5000){
                    choice = CAN_SOLVE;
                    
                    temp = LoadImage("images/button/show_solution.png");
                    ImageResize(&temp, 400, 150);
                    button1 = LoadTextureFromImage(temp);
                    
                    temp = LoadImage("images/button/write_to_file.png");
                    ImageResize(&temp, 400, 150);
                    button2 = LoadTextureFromImage(temp);
                    
                    Rectangle temp1 = { 0, 0, button1.width, button1.height };
                    sourceRec1 = temp1;
                    Rectangle temp2 = { 0, 0, button2.width, button2.height };
                    sourceRec2 = temp2;

                    Rectangle temp3 = { 100, HEIGHT/2 - button1.height/2, button1.width, button1.height };
                    btnBounds1 = temp3;
                    Rectangle temp4 = { WIDTH-button2.width-100, HEIGHT/2 - button2.height/2, button2.width, button2.height };
                    btnBounds2 = temp4;
  
                    btnAction1 = false, btnAction2 = false;
                }
                if(solved == 2 && timecounter > 5000){
                    choice = CAN_NOT_SOLVE;
                }
                break;
            }
            case FILE_RULES:{
                if(IsKeyPressed(KEY_ENTER)){
                    choice = FILE_ENTER;
                    ShowCursor();
                }
                break;
            }  
            case FILE_ENTER:{
                if (CheckCollisionPointRec(GetMousePosition(), textBox1)) mouseOnText = true;
                else mouseOnText = false;

                if (mouseOnText){
                    int key = GetKeyPressed();
                    if ((key >= 32) && (key <= 125) && (letterCount < 40)){
                        input[letterCount] = (char)key;
                        letterCount++;
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)){
                        letterCount--;
                        input[letterCount] = '\0';
                        if (letterCount < 0) letterCount = 0;
                    }
                }
         
                if (mouseOnText) framesCounter++;
                else framesCounter = 0;
                
                if(IsKeyPressed(KEY_ENTER)){
                    input[letterCount] = '\0';
                    FILE *fp = fopen(input, "r" );
                    if(fp == NULL){
                        error = 40;
                    }
                    else{
                        error = copy(fp, &number);
                        if(error == 0){
                            choice = KEYBOARD_ENTER;
                        }
                        else
                            error = 40;
                    }
                }
                if (IsFileDropped()){
                    int x = 0 ;
                    droppedFiles = GetDroppedFiles(&x);
                    strcpy(input, droppedFiles[0]);
                    letterCount = strlen(input);
                }

                break;
            }
            case CAN_NOT_SOLVE:{                
                if(IsKeyPressed(KEY_ENTER)){
                    delay = 50;
                    choice = EXIT;
                }
                break;    
            }
            case CAN_SOLVE:{
                mousePoint = GetMousePosition();
                
                if (CheckCollisionPointRec(mousePoint, btnBounds1))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction1 = true;

                if (CheckCollisionPointRec(mousePoint, btnBounds2))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction2 = true;

                if (btnAction2){
                    choice = FILE_WRITE;
                    PlaySound(click_sound);
                }
                if (btnAction1){
                    for(int i = 0; i<24; i++){
                        if(i==0){
                            stock1[i].prev = NULL;
                            stock1[i].next = &stock1[i+1];
                        }else if(i == 23){
                            stock1[i].prev = &stock1[22];
                            stock1[i].next = NULL;
                        }else{
                            stock1[i].prev = &stock1[i-1];
                            stock1[i].next = &stock1[i+1];
                        }
                    }
                    fp1 = fopen("files_for_exchange/solve.txt", "r");     
                    HideCursor();
                    choice = SOLVE_WITH_ANIMATION;
                    delay = 0;
                }
                break;
            }
            case FILE_WRITE:{
                if (CheckCollisionPointRec(GetMousePosition(), textBox1)) mouseOnText = true;
                else mouseOnText = false;

                if (mouseOnText){
                    int key = GetKeyPressed();
                    if ((key >= 32) && (key <= 125) && (letterCount < 40)){
                        input[letterCount] = (char)key;
                        letterCount++;
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)){
                        letterCount--;
                        input[letterCount] = '\0';
                        if (letterCount < 0) letterCount = 0;
                    }
                }
         
                if (mouseOnText) framesCounter++;
                else framesCounter = 0;
                
                if(IsKeyPressed(KEY_ENTER)){
                    fp1 = fopen(input, "w" );
                    if(fp1 == NULL){
                        error = 40;
                    }
                    else{
                        pthread_create(&threadId1, NULL, &write_solution, NULL);
                        delay = 50;
                        choice = EXIT;
                    }
                }
                if (IsFileDropped()){
                    int x = 0 ;
                    droppedFiles = GetDroppedFiles(&x);
                    strcpy(input, droppedFiles[0]);
                    letterCount = strlen(input);
                }

                break;
            }
            case WIN_SCREEN:{
                if (IsKeyPressed(KEY_ENTER)){
                    choice = EXIT;
                }
            }
            case LOSE_SCREEN:{
                if (IsKeyPressed(KEY_ENTER)){
                    choice = EXIT;
                }
            }
            case SOLVE_WITH_ANIMATION:{
                if(done2 == 0){
                    fscanf(fp1, "%d", &number5);
                    if(number5 == 999){
                        ShowCursor();
                        choice = EXIT;
                        delay = 50;
                    }
                    switch(number5){
                        case 2:
                            pthread_create(&threadId1, NULL, &case_2, NULL);
                            break;
                        case 3:
                            pthread_create(&threadId1, NULL, &case_3, NULL);
                            break;
                        case 4:
                            pthread_create(&threadId1, NULL, &case_4, NULL);
                            break;
                        case 5:
                            pthread_create(&threadId1, NULL, &case_5, NULL);
                            break;
                        case 6:
                            pthread_create(&threadId1, NULL, &case_6, NULL);
                            break;
                        case 7:
                            pthread_create(&threadId1, NULL, &case_7, NULL);
                            break;
                        case 8:
                            pthread_create(&threadId1, NULL, &case_8, NULL);
                            break;
                    }
                    done2 = 1;
                }
                if(click){
                    PlaySound(click_sound2);
                    click = 0;
                }
                break;
            }
            case CLOSE:{
                mousePoint = GetMousePosition();
                
                if (CheckCollisionPointRec(mousePoint, btnBounds1))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction1 = true;

                if (CheckCollisionPointRec(mousePoint, btnBounds2))
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction2 = true;

                if (btnAction1){
                    choice = RATING;
                    PlaySound(click_sound);

                }
                if (btnAction2){
                    delay = 50;
                    choice = EXIT;
                    PlaySound(click_sound);
                }
                break;
            } 
            case RATING:{
                if(IsKeyPressed(KEY_RIGHT)){
                    if(rating < 5)
                        rating++;
                }
                if(IsKeyPressed(KEY_LEFT)){
                    if(rating > 0)
                        rating--;
                }
                if(IsKeyPressed(KEY_ENTER)){
                    choice = EXIT;
                }
                break;
            }
            case EXIT:{
                if(delay == -200)
                    exit(0);
                delay--;
                break;
            }
        }
        
        BeginDrawing();
            ClearBackground((Color){153, 0, 0, 0});
            switch(choice){
                case COLLEGE_NAME_FRAME:{
                    background_i = LoadImage("images/background/1.png");
                    ImageResize(&background_i, WIDTH, HEIGHT);
                    background_t = LoadTextureFromImage(background_i);
                    DrawTexture(background_t, 0, 0, WHITE);
                    
                    logo_i = LoadImage("images/logo.png");
                    logo_t = LoadTextureFromImage(logo_i);
                    DrawTexture(logo_t, WIDTH/2 - logo_t.width/2, 80, WHITE);
                    
                    fonts = LoadFont("fonts/setback.png");
                    color = WHITE;

                    strcpy(buffer, "SIDDAGANGA INSTITUTE OF TECHNOLOGY");
                    font_size = 75;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 280;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    strcpy(buffer, "TUMKUR - 572103");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 400;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "KARNATAKA");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 490;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    delay += 1;
                    if(delay == 20){
                        choice = OEP_FRAME;
                        delay = 0;
                    }
                    break;
                }
                case OEP_FRAME:{
                    background_i = LoadImage("images/background/1.png");
                    ImageResize(&background_i, WIDTH, HEIGHT);
                    background_t = LoadTextureFromImage(background_i);
                    DrawTexture(background_t, 0, 0, WHITE);
                    
                    logo_i = LoadImage("images/logo.png");
                    logo_t = LoadTextureFromImage(logo_i);
                    DrawTexture(logo_t, WIDTH/2 - logo_t.width/2, 80, WHITE);
                    
                    fonts = LoadFont("fonts/setback.png");
                    color = WHITE;

                    strcpy(buffer, "OPEN ENDED PROJECT ON DATA STRUCTURES");
                    font_size = 70;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 280;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    strcpy(buffer, "SUBMITTED TO:");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH - MeasureTextEx(fonts, buffer, font_size, spacings).x - 400;
                    positions.y = 680;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "1.  KAVITHA MA'AM");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH - 750;
                    positions.y = 760;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "2. GURURAJ SIR");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH - 750;
                    positions.y = 850;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    delay += 1;
                    if(delay == 20){
                        choice = PYRAMID_SOLITARE_FRAME;
                        delay = 0;
                    }
                    break;
                }
                case PYRAMID_SOLITARE_FRAME:{       
                    background_i = LoadImage("images/background/2.png");
                    ImageResize(&background_i, WIDTH, HEIGHT);
                    background_t = LoadTextureFromImage(background_i);
                    DrawTexture(background_t, 0, 0, WHITE);

                    fonts = LoadFont("fonts/jupiter_crash.png");
                    color = WHITE;

                    strcpy(buffer, "PYRAMID SOLITARE");
                    font_size = 130;
                    spacings = 7;
                    scrollingTextPos =  WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    DrawTextEx(fonts, buffer,  (Vector2){scrollingText, 400 }, font_size, spacings, color);

                    strcpy(buffer, "DEVELOPED BY:");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH - MeasureTextEx(fonts, buffer, font_size, spacings).x - 400;
                    positions.y = 680;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "SUBRAMANYA G");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH - 620;
                    positions.y = 750;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "VISHNU TEJA S");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH - 620;
                    positions.y = 820;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    strcpy(buffer, "VISHWAK V");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH - 620;
                    positions.y = 890;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    delay += 1;
                    if(delay == 50){
                        choice = SELECTION_PLAY_SOLVE;
                        delay = 0;
                    }
                    break;
                }
                case SELECTION_PLAY_SOLVE:{
                    background_i = LoadImage("images/background/3.png");
                    ImageResize(&background_i, WIDTH, HEIGHT);
                    background_t = LoadTextureFromImage(background_i);
                    DrawTexture(background_t, 0, 0, WHITE);

                    DrawTextureRec(button1, sourceRec1, (Vector2){ btnBounds1.x, btnBounds1.y }, WHITE);
                    DrawTextureRec(button2, sourceRec2, (Vector2){ btnBounds2.x, btnBounds2.y }, WHITE);
                    break;
                }
                case PLAY_CHOICE:{
                    background_i = LoadImage("images/background/3.png");
                    ImageResize(&background_i, WIDTH, HEIGHT);
                    background_t = LoadTextureFromImage(background_i);
                    DrawTexture(background_t, 0, 0, WHITE);

                    DrawTextureRec(button3, sourceRec3, (Vector2){ btnBounds3.x, btnBounds3.y }, WHITE);
                    DrawTextureRec(button4, sourceRec4, (Vector2){ btnBounds4.x, btnBounds4.y }, WHITE);
                    break;
                }
                case GAMEPLAY_EASY:
                case GAMEPLAY_HARD:{
                    draw_cards();
                    DrawTextureRec(ngt, srng, (Vector2){ bbng.x, bbng.y }, WHITE);
                    DrawTextureRec(rbt, srrb, (Vector2){ bbrb.x, bbrb.y }, WHITE);
                    DrawTextEx(fonts, TextFormat("Points:%d",points), (Vector2){1000,50}, 50, 7, WHITE);
                    break;
                }
                case WIN_SCREEN:{
                    color = WHITE;
                    font_size = 80;
                    spacings = 7;
                    
                    strcpy(buffer, "Congratulations!You have won a game.");
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 400;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, TextFormat("Points: %d", points));
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 500;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "Press Enter to exit.");
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = HEIGHT - 100;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    break;
                }
                case LOSE_SCREEN:{
                    color = WHITE;
                    font_size = 80;
                    spacings = 7;
                    
                    strcpy(buffer, "Better luck next time!");
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 400;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, TextFormat("Points: %d", points));
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 500;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "Press Enter to exit.");
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = HEIGHT - 100;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    break;
                }
                case SOLVE_CHOICE:{
                    background_i = LoadImage("images/background/3.png");
                    ImageResize(&background_i, WIDTH, HEIGHT);
                    background_t = LoadTextureFromImage(background_i);
                    DrawTexture(background_t, 0, 0, WHITE);
                    
                    fonts = LoadFont("fonts/setback.png");
                    color = WHITE;

                    strcpy(buffer, "GIVE THE CARD DETAILS");
                    font_size = 75;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 100;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    DrawTextureRec(button1, sourceRec1, (Vector2){ btnBounds1.x, btnBounds1.y }, WHITE);
                    DrawTextureRec(button2, sourceRec2, (Vector2){ btnBounds2.x, btnBounds2.y }, WHITE);
                    break;
                }
                case KEYBOARD_RULES:{
                    fonts = LoadFont("fonts/jupiter_crash.png");
                    color = LIGHTGRAY;

                    DrawTextEx(fonts, "RULES:", (Vector2){20, 20 }, 100, 7, color);
                    DrawTextEx(fonts, "1. Enter the card details in the below box of next window.", (Vector2){20, 120 }, 70, 6, color);
                    DrawTextEx(fonts, "2. Type the card details as [suits][rank](Ex:CJ,D1).", (Vector2){20, 220 }, 70, 6, color);
                    DrawTextEx(fonts, "3. Type C for Clubs, D for Diamonds, H for Hearts and S for Spades.", (Vector2){20, 320 }, 70, 6, color);
                    DrawTextEx(fonts, "4. Type A for Ace, T for Ten, J for Jack, Q for Queen and K for King.", (Vector2){20, 420 }, 70, 6, color);
                    DrawTextEx(fonts, "5. Type number itself for the other ranks.", (Vector2){20, 520 }, 70, 6, color);
                    
                    strcpy(buffer, "PRESS ENTER TO GOTO INPUT MENU");
                    font_size = 70;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 900;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    break;
                }
                case KEYBOARD_ENTER:{
                    if(number < 52){
                        if(number < 28)
                            DrawText(TextFormat("Enter the card of %s row and %s column:", numbers[i], numbers[j]), 50, HEIGHT - 140, 35, GRAY);
                        else
                            DrawText(TextFormat("Enter the card of %s stock:", numbers[number - 28]), 50, HEIGHT - 140, 35, GRAY);    
                        DrawRectangleRec(textBox, LIGHTGRAY);
                        if (mouseOnText) DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, RED);
                        else DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, DARKGRAY);

                        DrawText(input, textBox.x + 5, textBox.y + 8, 40, MAROON);

                        if (mouseOnText)
                            if (letterCount < 2)
                                if (((framesCounter/20)%2) == 0) DrawText("_", textBox.x + 8 + MeasureText(input, 40), textBox.y + 12, 40, MAROON);
                        if(error != 0 && error_no == 0){
                            error--;
                            DrawText("Wrong Input, Please enter the card correctly", 150, HEIGHT-50, 35, GRAY);
                        }
                        if(error != 0 && error_no == 1){
                            error--;
                            DrawText("The card has already been entered", 150, HEIGHT-50, 35, GRAY);
                        }
                    }
                    if(number == 53){
                        DrawRectangle(700, HEIGHT - 180, dataProgress, 60, (Color){ 64, 85,118, 255 });
                        if ((framecounter/15)%2) DrawText("SOLVING...", 850, HEIGHT - 165, 40, (Color){ 200, 40, 60, 255 });
                    }
                    draw_cards1();
                    break;
                }
                case FILE_RULES:{
                    fonts = LoadFont("fonts/jupiter_crash.png");
                    color = LIGHTGRAY;

                    DrawTextEx(fonts, "RULES:", (Vector2){20, 20 }, 100, 7, color);
                    DrawTextEx(fonts, "1. Enter the file path in the below box of next window.", (Vector2){20, 120 }, 70, 6, color);
                    DrawTextEx(fonts, "2. Type the card details as [suits][rank](Ex:CJ,D1).", (Vector2){20, 220 }, 70, 6, color);
                    DrawTextEx(fonts, "3. Type C for Clubs, D for Diamonds, H for Hearts and S for Spades.", (Vector2){20, 320 }, 70, 6, color);
                    DrawTextEx(fonts, "4. Type A for Ace, T for Ten, J for Jack, Q for Queen and K for King.", (Vector2){20, 420 }, 70, 6, color);
                    DrawTextEx(fonts, "5. Type number itself for the other ranks.", (Vector2){20, 520 }, 70, 6, color);
                    
                    strcpy(buffer, "PRESS ENTER TO GO INPUT MENU");
                    font_size = 70;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 900;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    break;
                }
                case FILE_ENTER:{
                    DrawText("Enter the file path:", 50, HEIGHT/2 - 40, 35, GRAY);
                    DrawRectangleRec(textBox1, LIGHTGRAY);
                    if (mouseOnText) DrawRectangleLines(textBox1.x, textBox1.y, textBox1.width, textBox1.height, RED);
                    else DrawRectangleLines(textBox1.x, textBox1.y, textBox1.width, textBox1.height, DARKGRAY);

                    DrawText(input, textBox1.x + 5, textBox1.y + 8, 40, MAROON);

                    if (mouseOnText)
                        if (letterCount < 40)
                            if (((framesCounter/20)%2) == 0) DrawText("_", textBox1.x + 8 + MeasureText(input, 40), textBox1.y + 12, 40, MAROON);

                    strcpy(buffer, "PRESS ENTER TO GO TO THE SOLVE MENU");
                    font_size = 70;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 900;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    if(error != 0){
                        error--;
                        DrawText("The file path or file is not correct", 390, HEIGHT-250, 60, GRAY);
                    }

                    break;
                }
                case CAN_NOT_SOLVE:{
                    fonts = LoadFont("fonts/jupiter_crash.png");
                    color = LIGHTGRAY;
                    strcpy(buffer, "Sorry, Can't solve this game");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 370;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    strcpy(buffer, "But their may be a solution");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 430;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    
                    strcpy(buffer, "PRESS ENTER TO EXIT");
                    font_size = 70;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 900;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    break;    
                }
                case CAN_SOLVE:{
                    background_i = LoadImage("images/background/3.png");
                    ImageResize(&background_i, WIDTH, HEIGHT);
                    background_t = LoadTextureFromImage(background_i);
                    DrawTexture(background_t, 0, 0, WHITE);
                    
                    fonts = LoadFont("fonts/jupiter_crash.png");
                    color = LIGHTGRAY;

                    strcpy(buffer, "Hurry! Solved the game");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 240;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    strcpy(buffer, "How to show the solution?");
                    font_size = 60;
                    spacings = 6;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 300;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    DrawTextureRec(button1, sourceRec1, (Vector2){ btnBounds1.x, btnBounds1.y }, WHITE);
                    DrawTextureRec(button2, sourceRec2, (Vector2){ btnBounds2.x, btnBounds2.y }, WHITE);
                    break;

                }
                case FILE_WRITE:{
                    DrawText("Enter the file path:", 50, HEIGHT/2 - 40, 35, GRAY);
                    DrawRectangleRec(textBox1, LIGHTGRAY);
                    if (mouseOnText) DrawRectangleLines(textBox1.x, textBox1.y, textBox1.width, textBox1.height, RED);
                    else DrawRectangleLines(textBox1.x, textBox1.y, textBox1.width, textBox1.height, DARKGRAY);

                    DrawText(input, textBox1.x + 5, textBox1.y + 8, 40, MAROON);

                    if (mouseOnText)
                        if (letterCount < 40)
                            if (((framesCounter/20)%2) == 0) DrawText("_", textBox1.x + 8 + MeasureText(input, 40), textBox1.y + 12, 40, MAROON);

                    strcpy(buffer, "PRESS ENTER TO WRITE TO THE FILE");
                    font_size = 70;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 900;
                    color = LIGHTGRAY;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    if(error != 0){
                        error--;
                        DrawText("The file path or file is not correct", 390, HEIGHT-250, 60, GRAY);
                    }
                    break;
                }
                case SOLVE_WITH_ANIMATION:{
                    draw();
                    DrawTexture(cursor_t, cursor_pos.x, cursor_pos.y, WHITE);
                    break;
                }
                case CLOSE:{
                    fonts = LoadFont("fonts/setback.png");
                    color = LIGHTGRAY;

                    strcpy(buffer, "EXIT PAGE");
                    font_size = 75;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = font_size + 100;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    DrawTextureRec(button1, sourceRec1, (Vector2){ btnBounds1.x, btnBounds1.y }, WHITE);
                    DrawTextureRec(button2, sourceRec2, (Vector2){ btnBounds2.x, btnBounds2.y }, WHITE);
                    break;
                } 
                case RATING:{
                    strcpy(buffer, "Rate our program : ");
                    font_size = 50;
                    spacings = 6;
                    positions.x = 300;
                    positions.y = HEIGHT/2 - font_size/2;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);

                    i = 0;
                    while(i != rating){
                        DrawTexture(rating2_t, 920 + i * 80, positions.y, WHITE);                    
                        i++;
                    }
                    while(i != 5){
                        DrawTexture(rating1_t, 920 + i * 80, positions.y, WHITE);
                        i++;
                    }
                    strcpy(buffer, "PRESS ENTER TO SUBMIT");
                    font_size = 70;
                    spacings = 7;
                    positions.x = WIDTH/2 - MeasureTextEx(fonts, buffer, font_size, spacings).x/2;
                    positions.y = 900;
                    DrawTextEx(fonts, buffer, positions, font_size, spacings, color);
                    break;
                }
                case EXIT:{
                    fonts = LoadFont("fonts/jupiter_crash.png");
                    color = LIGHTGRAY;

                    DrawTextEx(fonts, "Contributed by:", (Vector2){20, 20 }, 100, 7, color);
                    DrawTextEx(fonts, "1. Notepad++ (https://notepad-plus-plus.org/)", (Vector2){20, 120 }, 70, 6, color);
                    DrawTextEx(fonts, "2. Raylib library", (Vector2){20, 220 }, 70, 6, color);
                    DrawTextEx(fonts, "   (i)   https://www.raylib.com/", (Vector2){20, 320 }, 70, 6, color);
                    DrawTextEx(fonts, "   (ii)   https://raysan5.itch.io/raylib", (Vector2){20, 420 }, 70, 6, color);
                    DrawTextEx(fonts, "   (iii)  https://github.com/raysan5/raylib/wiki", (Vector2){20, 520 }, 70, 6, color);
                    
                    break;
                }
            }
            if(!(choice == CLOSE || choice == RATING || choice == EXIT))
                DrawTextureRec(close, closeRec1, (Vector2){ closeBounds1.x, closeBounds1.y }, WHITE);

        EndDrawing();
    }

    UnloadImage(background_i);
    UnloadMusicStream(background_music);
    UnloadTexture(background_t);
    UnloadFont(fonts);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
    