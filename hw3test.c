#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"

#define FILENAME "database.json"

// 定義雙向鏈結串列節點結構
typedef struct Node {
    char *name;
    char *phone;
    char *email;
    struct Node *llink;
    struct Node *rlink;
} Node;

typedef Node *list_pointer;

list_pointer head,current,previous,forward;



// 函數聲明

Node *create_node(const char *name, const char *phone, const char *email);
void lpush(list_pointer *head, const char *name, const char *phone, const char *email);
void rpush(list_pointer *head, const char *name, const char *phone, const char *email);
Node lpop();
Node Rpop();
int llen();
void lrange(list_pointer *head, int start, int end);
void free_list(list_pointer *head);
void init_json();
cJSON *load_json();
void save_json(cJSON *root);
void display_entries(cJSON *root);


// 創建新列表
Node create_list() {
    head = (list_pointer)malloc(sizeof(Node));
    head->llink = NULL;
    head->rlink = NULL;
}

// 創建新節點
Node *create_node(const char *name, const char *phone, const char *email){
    Node *node=(Node *)malloc(sizeof(Node));
    node->name=strdup(name);
    node->phone=strdup(phone);
    node->email=strdup(email);
    node->llink=NULL;
    node->rlink=NULL;
    return node;
}

// LPUSH：在列表頭部插入
void lpush(list_pointer *head, const char *name, const char *phone, const char *email){
    Node *ptr=create_node(name,phone,email);
    current=head;
    forward=current->llink;
    ptr->llink=forward->llink;
    ptr->rlink=current->llink;
    current->rlink=ptr;
    forward->llink=ptr;
}



// RPUSH：在列表尾部插入
void rpush(list_pointer *head, const char *name, const char *phone, const char *email){
    Node *ptr=create_node(name,phone,email);
    current=head;
    previous=current->rlink;
    ptr->llink=current->llink;
    ptr->rlink=previous->rlink;
    current->llink=ptr;
    previous->rlink=ptr;
}


// LPOP：從頭部移除並返回節點
Node lpop() {
    current=head->rlink;
    forward=current->rlink;
    forward->llink=head;
    head->rlink=forward;
    free(current);
}


// RPOP：從尾部移除並返回節點
Node Rpop() {
    current=head->llink;
    previous=current->llink;
    previous->rlink=head;
    head->llink=forward;
    free(current);
}

int llen() {
    int length=0;
    current=head->rlink;
    if(current==NULL){
        return 0;
    }
    while(current!=NULL){
        length++;
        current=current->rlink;
    }
    return length;
}



int main(){
    create_list();
    
}




















