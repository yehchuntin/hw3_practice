#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"

#define FILENAME "database.json"

// å®?ç¾©é????????çµ?ä¸²å??ç¯?é»?çµ?æ§?
typedef struct Node {
    char *name;
    char *phone;
    char *email;
    struct Node *llink;
    struct Node *rlink;
} Node;

typedef Node *list_pointer;

list_pointer head, current;
cJSON *root; // JSON ??¹ç??é»?

// ??½æ?¸è?²æ??
void create_list();
Node *create_node(const char *name, const char *phone, const char *email);
void lpush(list_pointer head, const char *name, const char *phone, const char *email);
void rpush(list_pointer head, const char *name, const char *phone, const char *email);
Node *lpop(list_pointer head);
Node *rpop(list_pointer head);
int llen();
void lrange(list_pointer head, int start, int end, int a);
void free_list(list_pointer head);
void init_json();
cJSON *load_json();
void save_json();
void sync_json_with_list(list_pointer head);
void sync_list_with_json(list_pointer head);

// ??µå»º??°å??è¡?
void create_list() {
    head = (list_pointer)malloc(sizeof(Node));
    head->llink = head;
    head->rlink = head;
    head->name = NULL;
    head->phone = NULL;
    head->email = NULL;
}

// ??µå»º??°ç??é»?
Node *create_node(const char *name, const char *phone, const char *email) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = strdup(name);
    node->phone = strdup(phone);
    node->email = strdup(email);
    node->llink = NULL;
    node->rlink = NULL;
    return node;
}

// LPUSHï¼???¨å??è¡¨é?­é?¨æ????¥ä¸¦???æ­¥å?? JSON
void lpush(list_pointer head, const char *name, const char *phone, const char *email) {
    Node *ptr = create_node(name, phone, email);
    ptr->rlink = head->rlink;
    ptr->llink = head;
    head->rlink->llink = ptr;
    head->rlink = ptr;

    // ???æ­¥å?? JSON
    sync_json_with_list(head);
}

// RPUSHï¼???¨å??è¡¨å°¾??¨æ????¥ä¸¦???æ­¥å?? JSON
void rpush(list_pointer head, const char *name, const char *phone, const char *email) {
    Node *ptr = create_node(name, phone, email);
    ptr->llink = head->llink;
    ptr->rlink = head;
    head->llink->rlink = ptr;
    head->llink = ptr;

    // ???æ­¥å?? JSON
    sync_json_with_list(head);
}

// LPOPï¼?å¾???­é?¨ç§»??¤ä¸¦???æ­¥å?ªé?? JSON
Node *lpop(list_pointer head) {
    if (head->rlink == head) {
        return NULL;
    }
    Node *ptr = head->rlink;
    head->rlink = ptr->rlink;
    ptr->rlink->llink = head;

    // ???æ­¥å?? JSON
    sync_json_with_list(head);
    return ptr;
}

// RPOPï¼?å¾?å°¾é?¨ç§»??¤ä¸¦???æ­¥å?ªé?? JSON
Node *rpop(list_pointer head) {
    if (head->llink == head) {
        return NULL;
    }
    Node *ptr = head->llink;
    head->llink = ptr->llink;
    ptr->llink->rlink = head;

    // ???æ­¥å?? JSON
    sync_json_with_list(head);
    return ptr;
}

// LLENï¼?è¨?ç®????è¡¨é?·åº¦
int llen() {
    int count = 0;
    Node *current = head->rlink;
    while (current != head) {
        count++;
        current = current->rlink;
    }
    return count;
}

// DISPLAY???LRANGE???LRANGEï¼?é¡¯ç¤º??¶å?????è¡¨å?§å®¹
void lrange(list_pointer head, int start, int end,int a) {
    if (head->rlink == head) {
        printf("List is empty.\n");
        return;
    }

    if(a==1){   //å¤???³ä??????????¸æ±ºå®???¯å?¦å?°é?????(DISPLAY???LRANGE???LRANGE)
        printf("Current entries in list:\n");
    }

    Node *current = head->rlink;
    int index = 0;
    while (current != head) {
        if (index >= start && index <= end) {
            printf("{name: %s, phone: %s, email: %s}\n", current->name, current->phone, current->email);
        }
        current = current->rlink;
        index++;
    }
}

// DELETE ENTRYï¼?å¾????è¡¨å?? JSON ä¸­å?ªé?¤æ??å®????ç¨?
void delete_entry(list_pointer head, const char *name) {
    Node *current = head->rlink;
    while (current != head) {
        if (strcmp(current->name, name) == 0) {
            current->llink->rlink = current->rlink;
            current->rlink->llink = current->llink;

            cJSON_DeleteItemFromObject(root, name); // å¾? JSON ??ªé??
            sync_json_with_list(head); // ???æ­¥æ?´æ?? JSON

            free(current->name);
            free(current->phone);
            free(current->email);
            free(current);

            printf("Entry '%s' deleted successfully.\n", name);
            return;
        }
        current = current->rlink;
    }
    printf("Entry '%s' not found.\n", name);
}

// ???æ­¥é??è¡¨å?? JSON
void sync_json_with_list(list_pointer head) {
    root = cJSON_CreateObject();
    Node *current = head->rlink;
    while (current != head) {
        cJSON *entry = cJSON_CreateObject();
        cJSON_AddStringToObject(entry, "phone", current->phone);
        cJSON_AddStringToObject(entry, "email", current->email);
        cJSON_AddItemToObject(root, current->name, entry);
        current = current->rlink;
    }

    // ä¿?å­? JSON ???ä»?
    save_json();
}

// ???æ­? JSON ??°é??è¡?
void sync_list_with_json(list_pointer head) {
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, root) {
        const char *name = item->string;
        const char *phone = cJSON_GetObjectItem(item, "phone")->valuestring;
        const char *email = cJSON_GetObjectItem(item, "email")->valuestring;
        rpush(head, name, phone, email);
    }
}

// FREE LISTï¼??????¾é??è¡¨ç????????ç¯?é»?
void free_list(list_pointer head) {
    if (head == NULL) return;

    Node *current = head->rlink;
    while (current != head) {
        Node *temp = current;
        current = current->rlink;

        if (temp->name) free(temp->name);
        if (temp->phone) free(temp->phone);
        if (temp->email) free(temp->email);

        free(temp);
    }

    head->rlink = head;
    head->llink = head;
}

// ???å§???? JSON ???ä»?
void init_json() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        file = fopen(FILENAME, "w");
        if (!file) {
            perror("Unable to create JSON file");
            exit(1);
        }
        fprintf(file, "{}\n");
        fclose(file);
    }
}

// è®???? JSON ???ä»?
cJSON *load_json() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No existing data file found. Starting fresh.\n");
        return cJSON_CreateObject();
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON *json = cJSON_Parse(data);
    free(data);

    if (!json) {
        printf("Error parsing JSON. Starting with an empty JSON object.\n");
        return cJSON_CreateObject();
    }

    return json;
}

// ä¿?å­? JSON ???ä»?
void save_json() {
    char *data = cJSON_Print(root);
    FILE *file = fopen(FILENAME, "w");
    if (!file) {
        perror("Unable to save JSON file");
        free(data);
        return;
    }
    fprintf(file, "%s", data);
    fclose(file);
    free(data);
}

// ä¸»ç??å¼?
int main() {
    printf("Initializing JSON...\n");
    init_json();
    printf("Loading JSON...\n");
    root = load_json();
    printf("Creating list...\n");
    create_list();
    printf("Synchronizing list with JSON...\n");
    sync_list_with_json(head);
    printf("Synchronization completed. Entering main menu...\n");

    int choice;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Add Entry\n2. Delete Entry\n3. Display All Content\n4. LPUSH\n5. RPUSH\n6. LPOP\n7. RPOP\n8. LLEN\n9. LRANGE\n10. Save and Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 10) {
            printf("Exiting...\n");
            break;
        }

        char name[256], phone[256], email[256];
        Node *node;
        int start, end;

        switch (choice) {
            case 1:
                printf("Enter name: ");
                scanf("%s", name);
                printf("Enter phone: ");
                scanf("%s", phone);
                printf("Enter email: ");
                scanf("%s", email);
                rpush(head, name, phone, email);
                break;
            case 2:
                printf("Enter name to delete: ");
                scanf("%s", name);
                delete_entry(head, name);
                break;
            case 3:
                
                lrange(head, 0, llen() - 1,1);
                break;
            case 4:
                printf("Enter name: ");
                scanf("%s", name);
                printf("Enter phone: ");
                scanf("%s", phone);
                printf("Enter email: ");
                scanf("%s", email);
                lpush(head, name, phone, email);
                break;
            case 5:
                printf("Enter name: ");
                scanf("%s", name);
                printf("Enter phone: ");
                scanf("%s", phone);
                printf("Enter email: ");
                scanf("%s", email);
                rpush(head, name, phone, email);
                break;
            case 6:
                node = lpop(head);
                if (node) {
                    printf("LPOP: {name: %s, phone: %s, email: %s}\n", node->name, node->phone, node->email);
                    free(node->name);
                    free(node->phone);
                    free(node->email);
                    free(node);
                } else {
                    printf("List is empty\n");
                }
                break;
            case 7:
                node = rpop(head);
                if (node) {
                    printf("RPOP: {name: %s, phone: %s, email: %s}\n", node->name, node->phone, node->email);
                    free(node->name);
                    free(node->phone);
                    free(node->email);
                    free(node);
                } else {
                    printf("List is empty\n");
                }
                break;
            case 8:
                printf("LLEN: %d\n", llen());
                break;
            case 9:
                printf("Enter start index: ");
                scanf("%d", &start);
                printf("Enter end index: ");
                scanf("%d", &end);
                lrange(head, start, end,0);
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    save_json();
    free_list(head);
    cJSON_Delete(root);
    return 0;
}
