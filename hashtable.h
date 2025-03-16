#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct Node {
    char *key;
    int value;
    struct Node *next;
} Node;

typedef struct HashTable {
    int minSize;
    int maxSize;
    Node **array;
    int length;
} HashTable;

HashTable init(int N) {
    HashTable table;
    table.minSize = N;
    table.maxSize = N;
    table.array = (Node **)malloc(N*sizeof(Node *));
    for (int i = 0; i < N; i++) {
        table.array[i] = NULL;
    }
    table.length = 0;
    return table;
}

HashTable copy(HashTable table){
    HashTable new;
    new.minSize = table.minSize;
    new.maxSize = table.maxSize;
    new.array = (Node **)malloc(table.maxSize*sizeof(Node *));
    for (int i = 0; i < table.maxSize; i++) {
        new.array[i] = table.array[i];
    }
    new.length = table.length;
    return new;
}

void clear(HashTable *table) {
    Node *prev, *curr;
    for (int i = 0; i < table->maxSize; i++) {
        prev=NULL;
        curr = table->array[i];
        while(curr!=NULL){
            prev=curr;
            curr=curr->next;
            free(prev->key);
            free(prev);
        }
        table->array[i]=NULL;
    }
    table->length=0;
}

void delete(HashTable *table) {
    Node *prev, *curr;
    for (int i = 0; i < table->maxSize; i++) {
        prev=NULL;
        curr = table->array[i];
        while(curr!=NULL){
            prev=curr;
            curr=curr->next;
            free(prev->key);
            free(prev);
        }
        table->array[i]=NULL;
    }
    table->maxSize=0;
    table->minSize=0;
    free(table->array);
    table->array=NULL;
    table->length=0;
}

int hash(char *key, int maxSize) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash*31) + (*key++);
    }
    float k=(pow(5,0.5)-1)/2;
    return (int)floor(maxSize*fmod(k*hash,1));
}

float loadFactor(HashTable table){
    return (float)table.length/table.maxSize;
}

void rehash(HashTable *table){
    float currentLoadFactor=loadFactor(*table);
    int newSize;
    if(currentLoadFactor>=0.75)
        newSize=table->maxSize*2;
    else if(currentLoadFactor<=0.25 && table->minSize<table->maxSize)
        newSize=table->minSize>table->maxSize/2?table->minSize:table->maxSize/2;
    else return;
    
    HashTable newTable=init(newSize);
    newTable.minSize=table->minSize;
    for (int i = 0; i < table->maxSize; i++) {
        Node *curr = table->array[i];
        while(curr!=NULL){
            int index=hash(curr->key,newSize);
            Node *newNode = (Node *)malloc(sizeof(Node));
            newNode->key=strdup(curr->key);
            newNode->value=curr->value;
            newNode->next=newTable.array[index];
            newTable.array[index]=newNode;
            newTable.length++;
            curr=curr->next;
        }
    }
    delete(table);
    *table=newTable;
}

bool isin(HashTable table, char *key) {
    int index = hash(key,table.maxSize);
    Node* curr = table.array[index];

    while(curr!=NULL){
        if(strcmp(curr->key,key)==0){
            return true;
        }
        curr=curr->next;
    }
    return false;
}

int get(HashTable table, char *key) {
    int index = hash(key,table.maxSize);
    Node* curr = table.array[index];

    while(curr!=NULL){
        if(strcmp(curr->key,key)==0){
            return curr->value;
        }
        curr=curr->next;
    }
    return -1;
}

void put(HashTable *table, char *key, int value) {
    int index = hash(key,table->maxSize);
    Node *curr = table->array[index];

    while(curr!=NULL){
        if(strcmp(curr->key,key)==0){
            curr->value=value;
            return;
        }
        curr=curr->next;
    }

    Node *new = (Node *)malloc(sizeof(Node));
    new->key=strdup(key);
    new->value=value;
    new->next=table->array[index];
    table->array[index]=new;
    table->length++;
    
    rehash(table);
}

void pop(HashTable *table, char *key) {
    int index = hash(key,table->maxSize);
    Node* prev = NULL;
    Node* curr = table->array[index];

    while(curr!=NULL){
        if(strcmp(curr->key,key)==0){
            if(prev==NULL){
                table->array[index]=curr->next;
            }else{
                prev->next=curr->next;
            }
            free(curr->key);
            free(curr);
            table->length--;
            break;
        }
        prev=curr;
        curr=curr->next;
    }
    
    rehash(table);
}

char **keys(HashTable table){
    char **keyList=(char **)malloc(table.length*sizeof(char *));
    int count=0;
    Node *curr;
    for (int i = 0; i < table.maxSize; i++) {
        curr = table.array[i];
        while(curr!=NULL){
            keyList[count++]=strdup(curr->key);
            curr=curr->next;
        }
    }
    return keyList;
}

int *values(HashTable table){
    int *valueList=(int *)malloc(table.length*sizeof(int));
    int count=0;
    Node *curr;
    for (int i = 0; i < table.maxSize; i++) {
        curr = table.array[i];
        while(curr!=NULL){
            valueList[count++]=curr->value;
            curr=curr->next;
        }
    }
    return valueList;
}

Node *items(HashTable table){
    Node *itemList=(Node *)malloc(table.length*sizeof(Node));
    int count=0;
    Node *curr;
    for (int i = 0; i < table.maxSize; i++) {
        curr = table.array[i];
        while(curr!=NULL){
            Node new;
            new.key=strdup(curr->key);
            new.value=curr->value;
            new.next=NULL;
            itemList[count++]=new;
            curr=curr->next;
        }
    }
    return itemList;
}

void traverse(HashTable table){
    printf("{ ");
    for (int i = 0; i < table.maxSize; i++){
        Node *curr=table.array[i];
        while (curr!=NULL){
            printf("%s:%d ",curr->key,curr->value);
            curr=curr->next;
        }
    }
    printf("} : %.2f\n",loadFactor(table));
}

void describe(HashTable table) {
    Node *curr;
    for (int i = 0; i < table.maxSize; i++) {
        printf("%2d | ",i);
        curr = table.array[i];
        while(curr!=NULL){
            printf("{%s,%d}",curr->key,curr->value);
            curr=curr->next;
            if(curr!=NULL) printf(" -> ");
        }
        printf("\n");
    }
}

int main() {
    HashTable table = init(10);

    put(&table,"Alice",25);
    put(&table,"Bob",10);
    put(&table,"Charlie",50);
    put(&table,"Dennis",45);
    put(&table,"Eve",30);
    put(&table,"Fiona",40);
    put(&table,"Ginny",20);
    put(&table,"Henry",35);

    // char **list1=keys(table);
    // for (int i = 0; i < table.length; i++)
    // {
    //     printf("%s, ",list1[i]);
    // }
    // printf("\n");

    // int *list2=values(table);
    // for (int i = 0; i < table.length; i++)
    // {
    //     printf("%d, ",list2[i]);
    // }
    // printf("\n");

    // Node *list3=items(table);
    // for (int i = 0; i < table.length; i++)
    // {
    //     printf("%s:%d, ",list3[i].key,list3[i].value);
    // }
    // printf("\n");

    // HashTable table2=copy(table);
    // describe(table2);
    // traverse(table2);

    traverse(table);
    int i=17;
    while(i-->0) put(&table,"Bob",get(table,"Bob")+1);
    traverse(table);

    return 0;
}