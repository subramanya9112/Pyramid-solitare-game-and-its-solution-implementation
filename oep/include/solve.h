#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

static atomic_bool dataLoaded = ATOMIC_VAR_INIT(false);
static int dataProgress = 0, timecounter = 0, solved = 0;
FILE *fp1 = NULL;

typedef struct tree{
    char suits;
    int rank;
    Texture2D card_photo;
    Vector2 pos;
}tree1;
tree1 dealt1[28] = {0};

typedef struct doubly_linked_list1{
    char suits;
    int rank;
    Texture2D card_photo;
    struct doubly_linked_list1 *next, *prev;
}double_linked_list1;
double_linked_list1 stock1[24] = {0}, *first = &stock1[0], *upper_stock = NULL;

typedef struct tree_a1{
    int number;
    int rank;
    struct tree_a1 *parent1;
    struct tree_a1 *parent2;
    struct tree_a1 *child1;
    struct tree_a1 *child2;
}tree_a2;
tree_a2 t[28], *free_cards[7], *parent1, *parent2, *child, *child1[2];

typedef struct linked_list{
    tree_a2 *free_cards;
    struct linked_list *next;
}free_card;

typedef struct double_a1{
    int rank;
    struct double_a1 *next;
    struct double_a1 *prev;
}double_a2;
double_a2 stock[24], *first_stock, *last_stock, *stock_upper, *stock_previous;

struct standarddeck{
    int count;
    struct deck_standard{
        char suit;
        int rank;
    }standard[52];
}standard;

int n = 7, done2 = 0, click = 0, x = 0;
Vector2 cursor_pos = {0};
Vector2 end_pos = {0};
double l;

free_card * insert_top(int number, free_card *head){
    free_card *new_free_cards;
    new_free_cards = (free_card *) malloc(sizeof(free_card));
    new_free_cards->free_cards = &t[number];
    new_free_cards->next= head;
    head = new_free_cards;
    return head;
}

free_card * insert_to_free_cards(int number, free_card *head){
    free_card *current_free_cards = head;
    free_card *new_free_cards;
    n++;
    if(n == 1){
        new_free_cards = (free_card *) malloc(sizeof(free_card));
        new_free_cards->free_cards = &t[number];
        new_free_cards->next = NULL;
        head = new_free_cards;
        return head;
    }
    while(current_free_cards->next != NULL){
        if(current_free_cards->next->free_cards->number > number){
            new_free_cards = (free_card *) malloc(sizeof(free_card));
            new_free_cards->free_cards = &t[number];
            new_free_cards->next = current_free_cards->next;
            current_free_cards->next = new_free_cards;
            return head;
        }
        current_free_cards = current_free_cards->next;
    }
    if(current_free_cards->next == NULL){
        new_free_cards = (free_card *) malloc(sizeof(free_card));
        new_free_cards->free_cards = &t[number];
        new_free_cards->next = NULL;
        current_free_cards->next = new_free_cards;
        return head;
    }
    return NULL;
}

tree_a2 * is_king_in_freecards(free_card *head){
    free_card *current_node = head;
    while ( current_node != NULL){
        if(current_node->free_cards->rank == 13)
            return current_node->free_cards;
        current_node = current_node->next;
    }
    return NULL;
}

free_card * remove_in_freecards(int number, free_card *head){
    n--;
    free_card *current_free_cards = head;
    free_card *new_free_cards;
    if(n == 0)
        return NULL;
    if (current_free_cards != NULL && current_free_cards->free_cards->number == number) {
        head = current_free_cards->next;
        return head;
    }
    while (current_free_cards != NULL && current_free_cards->free_cards->number != number){
        new_free_cards = current_free_cards;
        current_free_cards = current_free_cards->next;
    }
    if (current_free_cards == NULL) return head;
    new_free_cards->next = current_free_cards->next;
    current_free_cards = current_free_cards->next;
    return head;
}

int delete_free_card(free_card **head){
    free_card *current_free_cards1 = *head;
    free_card *current_free_cards2;
    for(int i = 0; i < n-1; i++){
        current_free_cards2 = current_free_cards1->next;
        for(int j = i+1; j < n; j++){
            if( ( current_free_cards1->free_cards->rank + current_free_cards2->free_cards->rank ) == 13 ){
                child1[0] = current_free_cards1->free_cards;
                child1[1] = current_free_cards2->free_cards;
                return 1;
            }
            current_free_cards2 = current_free_cards2->next;
        }
        current_free_cards1 = current_free_cards1->next;
    }
    return 0;
}

int delete_free_card_with_prev_stock(free_card **head){
    stock_previous = stock_upper->prev;
    free_card *current_free_cards1 = *head;
    if(stock_previous == NULL)
        return 0;
    for(int i = 0; i < n; i++){
        if( ( current_free_cards1->free_cards->rank + stock_previous->rank ) == 13 ){
            child1[0] = current_free_cards1->free_cards;
            if(stock_previous->prev == NULL){
                stock_upper->prev = NULL;
                first_stock = stock_upper;
            }
            else{
                double_a2 *temp;
                temp = stock_previous->prev;
                temp->next = stock_upper;
                stock_upper->prev = temp;
            }
            return 1;
        }
        current_free_cards1 = current_free_cards1->next;
    }
    return 0;
}

int delete_free_card_with_stock(free_card **head){
    free_card *current_free_cards1 = *head;
    for(int i = 0; i < n; i++){
        if( ( current_free_cards1->free_cards->rank + stock_upper->rank ) == 13 ){
            child1[0] = current_free_cards1->free_cards;
            if(stock_upper->next == NULL){
                stock_upper->next = NULL;
                last_stock = stock_upper;
                stock_upper = NULL;
            }
            else if(stock_upper->prev == NULL){
                stock_upper = stock_upper->next;
                stock_upper->prev = NULL;
                first_stock = stock_upper;
            }
            else{
                double_a2 *temp;
                temp = stock_upper->prev;
                stock_upper = stock_upper->next;
                temp->next = stock_upper;
                stock_upper->prev = temp;
            }
            return 1;
        }
        current_free_cards1 = current_free_cards1->next;
    }
    return 0;
}

static void *solve(void *arg){
    tree1 *dealt12 = &dealt1[0];
    double_linked_list1 *stock12 = &stock1[0];
    FILE *fp = fopen("files_for_exchange/solve.txt", "w");

    for(int i = 0, number = 0, pos2 = -2, pos1 = -1; i<7; i++){
        for(int j = 0; j<i+1; j++, number++){
            t[number].rank = (dealt12)->rank;
            dealt12++;
            t[number].number = number;
            if(number == 0){
                t[0].parent1 = NULL;
                t[0].parent2 = NULL;
            }
            else if(number-i-1 == pos2){
                t[number].parent1 = NULL;
                t[number].parent2 = &t[pos2+1];
                t[pos2+1].child1 = &t[number];
            }
            else if(number-i-1 == pos1){
                t[number].parent1 = &t[pos1];
                t[number].parent2 = NULL;
                t[pos1].child2 = &t[number];
            }
            else{
                t[number].parent1 = &t[number-i-1];
                t[number].parent2 = &t[number-i];
                t[number-i].child1 = &t[number];
                t[number-i-1].child2 = &t[number];
            }
        }
        pos2=(pos2>-1)?pos2+i:pos2+1;
        pos1=(pos1>-1)?pos1+i+1:pos1+1;
    }

    for(int i = 0; i<24; i++){
        stock[i].rank = (stock12)->rank;
        stock12++;
        if(i==0){
            stock[0].prev = NULL;
            stock[0].next = &stock[1];
        }
        else if(i==23){
            stock[23].prev = &stock[22];
            stock[23].next = NULL;
        }
        else{
            stock[i].prev = &stock[i-1];
            stock[i].next = &stock[i+1];
        }
    }

    free_card *head = NULL;
    for(int i = 7; i > 0; i--)
        head = insert_top(i + 20, head);
    first_stock = &stock[0];
    last_stock = &stock[23];
    stock_upper = first_stock;
    int remaining_stock = 3;
    for(;;){
        if(head == NULL){
            solved = 1;
            fprintf(fp, "999");
            fclose(fp);
            return NULL;
        }
        if(stock_upper == NULL){
            if(first_stock == NULL && last_stock == NULL){
                fprintf(fp, "0\n");
                fclose(fp);
                solved = 2;
                return NULL;
            }
            else{
                if(remaining_stock == 0){
                    fprintf(fp, "1\n");
                    fclose(fp);
                    solved = 2;
                    return NULL;
                }
                else{
                    fprintf(fp, "2\n");
                    stock_upper = first_stock;
                    remaining_stock--;
                }
            }
            continue;
        }
        if(stock_upper->rank == 13){
            if(stock_upper == first_stock && first_stock == last_stock){
                fprintf(fp, "3\n");
                stock_upper = NULL;
                first_stock = NULL;
                last_stock = NULL;
            }
            else if(stock_upper == first_stock){
                fprintf(fp, "3\n");
                stock_upper = stock_upper->next;
                stock_upper->prev = NULL;
                first_stock = first_stock->next;
            }
            else if(stock_upper == last_stock){
                fprintf(fp, "3\n");
                last_stock = last_stock->prev;
                stock_upper = NULL;
            }
            else{
                double_a2 *temp;
                fprintf(fp, "3\n");
                temp = stock_upper->prev;
                stock_upper = stock_upper->next;
                temp->next = stock_upper;
                stock_upper->prev = temp;
            }
            continue;
        }
        if((child = is_king_in_freecards(head))){
            fprintf(fp, "4 %d\n", child->number);
            parent1 = child->parent1;
            parent2 = child->parent2;
            if(parent1 != NULL){
                parent1->child2 = NULL;
                if(parent1->child1 == NULL )
                    head = insert_to_free_cards(parent1->number, head);
            }
            if(parent2 != NULL){
                parent2->child1 = NULL;
                if(parent2->child2 == NULL )
                    head = insert_to_free_cards(parent2->number, head);
            }
            head = remove_in_freecards(child->number, head);
            continue;
        }
        if(delete_free_card(&head)){
            fprintf(fp, "5 %d %d\n", child1[0]->number, child1[1]->number);
            head = remove_in_freecards(child1[0]->number, head);
            head = remove_in_freecards(child1[1]->number, head);
            parent1 = child1[0]->parent1;
            parent2 = child1[0]->parent2;
            if(parent1 != NULL){
                parent1->child2 = NULL;
                if(parent1->child1 == NULL )
                    head = insert_to_free_cards(parent1->number, head);
            }
            if(parent2 != NULL){
                parent2->child1 = NULL;
                if(parent2->child2 == NULL )
                    head = insert_to_free_cards(parent2->number, head);
            }
            parent1 = child1[1]->parent1;
            parent2 = child1[1]->parent2;
            if(parent1 != NULL){
                parent1->child2 = NULL;
                if(parent1->child1 == NULL )
                    head = insert_to_free_cards(parent1->number, head);
            }
            if(parent2 != NULL){
                parent2->child1 = NULL;
                if(parent2->child2 == NULL )
                    head = insert_to_free_cards(parent2->number, head);
            }
            continue;
        }
        if(delete_free_card_with_prev_stock(&head)){
            fprintf(fp, "6 %d\n", child1[0]->number);
            head = remove_in_freecards(child1[0]->number, head);
            parent1 = child1[0]->parent1;
            parent2 = child1[0]->parent2;
            if(parent1 != NULL){
                parent1->child2 = NULL;
                if(parent1->child1 == NULL )
                    head = insert_to_free_cards(parent1->number, head);
            }
            if(parent2 != NULL){
                parent2->child1 = NULL;
                if(parent2->child2 == NULL )
                    head = insert_to_free_cards(parent2->number, head);
            }
            continue;
        }
        if(delete_free_card_with_stock(&head)){
            fprintf(fp, "7 %d\n", child1[0]->number);
            head = remove_in_freecards(child1[0]->number, head);
            parent1 = child1[0]->parent1;
            parent2 = child1[0]->parent2;
            if(parent1 != NULL){
                parent1->child2 = NULL;
                if(parent1->child1 == NULL )
                    head = insert_to_free_cards(parent1->number, head);
            }
            if(parent2 != NULL){
                parent2->child1 = NULL;
                if(parent2->child2 == NULL )
                    head = insert_to_free_cards(parent2->number, head);
            }
            continue;
        }
        stock_upper = stock_upper->next;
        fprintf(fp, "8\n");
    }
}

static void *LoadDataThread(void *arg){
    int timeCounter = 0;
    clock_t prevTime = clock();

    while (timeCounter < 5000){
        clock_t currentTime = clock() - prevTime;
        timeCounter = currentTime*1000/CLOCKS_PER_SEC;
        dataProgress = timeCounter/10;
    }
    timecounter = 5001;
    atomic_store(&dataLoaded, true);
    return NULL;
}

static void *sleeep(void *arg){
    int timeCounter = 0;
    clock_t prevTime = clock();

    while (timeCounter < x){
        clock_t currentTime = clock() - prevTime;
        timeCounter = currentTime*1000/CLOCKS_PER_SEC;
    }
    return NULL;
}

static void *write_solution(void *arg){
    FILE *fp = fopen("files_for_exchange/solve.txt", "r"); 
    int choice, n1, n2;
    while(fscanf(fp, "%d", &choice) != EOF){
        switch(choice){
            case 0:{
                fprintf(fp1, "Stock Exhausted\n");
                break;
            }
            case 1:{
                fprintf(fp1, "Stock is empty\n");
                break;
            }
            case 2:{
                fprintf(fp1, "Stock restocked\n");
                break;
            }
            case 3:{
                fprintf(fp1, "Stock king is removed\n");
                break;
            }
            case 4:{
                fscanf(fp, "%d", &n1);
                fprintf(fp1, "King at place %d is removed\n", n1+1);
                break;
            }
            case 5:{
                fscanf(fp, "%d", &n1);
                fscanf(fp, "%d", &n2);
                fprintf(fp1, "%d card is matched with %d card\n", n1+1, n2+1);
                break;
            }
            case 6:{
                fscanf(fp, "%d", &n1);
                fprintf(fp1, "%d card is matched with previous stock\n", n1+1);
                break;
            }
            case 7:{
                fscanf(fp, "%d", &n1);
                fprintf(fp1, "%d card is matched with stock card\n", n1+1);
                break;
            }
            case 8:{
                fprintf(fp1, "Stock changed\n");
                break;
            }
            case 999:{
                break;
            }
        }
    }
    fclose(fp);
    fclose(fp1);
    return NULL;
}

Vector2 distance(){
    double x = pow((cursor_pos.x - end_pos.x), 2);
    double y = pow((cursor_pos.y - end_pos.y), 2);
    l = pow(x+y, 0.5)/5;
    Vector2 fixed_move;
    fixed_move.x = (end_pos.x - cursor_pos.x)/((l+1)*20);
    fixed_move.y = (end_pos.y - cursor_pos.y)/((l+1)*20);
    return fixed_move;
}

static void *move_cur(void *arg){
    int timeCounter = 0, prev = -1;
    clock_t prevTime = clock();
    Vector2 fixed_move = distance();
    
    while (timeCounter < l*20){
        clock_t currentTime = clock() - prevTime;
        timeCounter = currentTime*1000/CLOCKS_PER_SEC;
        if(prev != timeCounter){
            cursor_pos.x += fixed_move.x;
            cursor_pos.y += fixed_move.y;
            prev = timeCounter;
        }
    }
    return NULL;
}

static void *case_2(void *arg){
    pthread_t threadId2;
    end_pos.x = 1460;
    end_pos.y = 525;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    upper_stock = NULL;
    x = 500;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    done2 = 0;
    return NULL;
}

static void *case_3(void *arg){
    pthread_t threadId2;
    end_pos.x = 1460;
    end_pos.y = 700;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    if(upper_stock == NULL){
        first = first->next;
        first->prev = NULL;
    }
    else if(upper_stock->next->next == NULL){
        upper_stock->next = NULL;
    }
    else{
        upper_stock->next = upper_stock->next->next;
        upper_stock->next->prev = upper_stock;
    }
    x = 500;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    done2 = 0;
    return NULL;
}

static void *case_4(void *arg){
    pthread_t threadId2;
    int a;
    fscanf(fp1, "%d", &a);
    end_pos.x = dealt1[a].pos.x + 60;
    end_pos.y = dealt1[a].pos.y + 100;
    Texture2D temp = {0};
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    dealt1[a].card_photo = temp;
    x = 500;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    done2 = 0;
    return NULL;
}

static void *case_5(void *arg){
    pthread_t threadId2;
    int a, b;
    Texture2D temp = {0};
    fscanf(fp1, "%d", &a);
    end_pos.x = dealt1[a].pos.x + 60;
    end_pos.y = dealt1[a].pos.y + 100;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    x = 200;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    
    fscanf(fp1, "%d", &b);
    end_pos.x = dealt1[b].pos.x + 60;
    end_pos.y = dealt1[b].pos.y + 100;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    
    dealt1[a].card_photo = temp;
    dealt1[b].card_photo = temp;
    x = 500;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    done2 = 0;
    return NULL;
}

static void *case_6(void *arg){
    pthread_t threadId2;
    Texture2D temp = {0};

    int a;
    fscanf(fp1, "%d", &a);
    end_pos.x = dealt1[a].pos.x + 60;
    end_pos.y = dealt1[a].pos.y + 100;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    x = 200;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    
    end_pos.x = 1460;
    end_pos.y = 300;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    
    dealt1[a].card_photo = temp;
    if(upper_stock->prev == NULL){
        upper_stock = NULL;
        first = first->next;
        first->prev = NULL;
    }
    else if(upper_stock->next == NULL){
        upper_stock = upper_stock->prev;
        upper_stock->next = NULL;
    }
    else{
        upper_stock->prev->next = upper_stock->next;
        upper_stock->next->prev = upper_stock->prev;
        upper_stock = upper_stock->prev;
    }
    x = 500;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    done2 = 0;
    return NULL;
}

static void *case_7(void *arg){
    pthread_t threadId2;
    Texture2D temp = {0};

    int a;
    fscanf(fp1, "%d", &a);
    end_pos.x = dealt1[a].pos.x + 60;
    end_pos.y = dealt1[a].pos.y + 100;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    x = 200;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    
    end_pos.x = 1460;
    end_pos.y = 700;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;

    dealt1[a].card_photo = temp;
    if(upper_stock == NULL){
        first = first->next;
        first->prev = NULL;
    }
    else if(upper_stock->next->next == NULL){
        upper_stock->next = NULL;
    }
    else{
        upper_stock->next = upper_stock->next->next;
        upper_stock->next->prev = upper_stock;
    }
    x = 500;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    done2 = 0;
    return NULL;
}

static void *case_8(void *arg){
    pthread_t threadId2;
    end_pos.x = 1460;
    end_pos.y = 525;
    pthread_create(&threadId2, NULL, &move_cur, NULL);
    pthread_join(threadId2, NULL);     
    click = 1;
    
    if(upper_stock == NULL)
        upper_stock = first;
    else
        upper_stock = upper_stock->next;
    x = 1000;
    pthread_create(&threadId2, NULL, &sleeep, NULL);
    pthread_join(threadId2, NULL);     
    done2 = 0;
    return NULL;
}

