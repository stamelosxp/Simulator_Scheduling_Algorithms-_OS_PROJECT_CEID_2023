#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

enum Status {
    READY,
    RUNNING,
    STOPPED,
    EXITED
};
struct process{
    char processName[100];
    double burstTime;
    int priority;
    int pID;
    enum Status status;
};
struct node {
    struct node* next;
    struct node* prev;
    struct node* childHead;
    struct node* childTail;
    struct process process;
};
struct node* head = NULL;
struct node* tail = NULL;

struct node* create_node(struct process process) {
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->process = process;
    new_node->childTail = NULL;
    new_node->childHead = NULL;
    return new_node;
}

void insertWithPriority(struct process process) {
    struct node* new_node = create_node(process);

    if (tail == NULL) {
        head = new_node;
        tail = new_node;
    } else {
        struct node* current = head;
        struct node* prev = NULL;
        int inserted = 0;

        while (current != NULL) {
            if (process.priority == current->process.priority) {
                if (current->childHead == NULL) {
                    current->childHead = new_node;
                    current->childTail = new_node;
                } else {
                    current->childTail->next = new_node;
                    new_node->prev = current->childTail;
                    current->childTail = new_node;
                }
                inserted = 1;
                break;
            } else if (process.priority < current->process.priority) {
                new_node->prev = current->prev;
                new_node->next = current;

                if (current == head) {
                    head = new_node;
                } else {
                    prev->next = new_node;
                }

                current->prev = new_node;
                inserted = 1;
                break;
            }

            prev = current;
            current = current->next;
        }

        if (!inserted) {
            new_node->prev = tail;
            tail->next = new_node;
            tail = new_node;
        }
    }
}

void insert(struct process process){
    struct node* new_node = create_node(process);

    if (tail == NULL) {
        head = new_node;
        tail = new_node;
    }
    else{
        new_node->prev = tail;
        tail->next = new_node;
        tail = new_node;
    }
}

void display_forward() {
    struct node* current = head;
    while (current != NULL) {
        printf("\nName: %s Pid: %d Status: %d Burst Time: %f", current->process.processName, current->process.pID,current->process.status, current->process.burstTime);

        struct node* child = current->childHead;
        while (child != NULL) {
            printf("\nName: %s Pid: %d Status: %d Burst Time: %f", child->process.processName, child->process.pID,child->process.status,current->process.burstTime);
            child = child->next;
        }
        current = current->next;
    }
}

void runFCFSSJF(struct node* enteredHead) {
    clock_t t;
    t = clock();
    while (enteredHead != NULL) {
        if (enteredHead->process.status == READY) {
            int pid = fork();
            enteredHead->process.status = RUNNING;

            if (pid == 0) {
                // Child process
                execl(enteredHead->process.processName, enteredHead->process.processName, NULL);

                exit(EXIT_SUCCESS);
            } else if (pid > 0) {
                enteredHead->process.pID=pid;

                waitpid(pid, NULL, 0);
                t = clock() - t;
                double time_taken = ((double)t)/CLOCKS_PER_SEC;
                enteredHead->process.burstTime =time_taken;

                printf("The process \"%s\" has been done\n\n", enteredHead->process.processName);
                enteredHead->process.status = EXITED;

            } else {
                perror("Fork error");
                exit(EXIT_FAILURE);
            }
        }
        struct node* currentChild = enteredHead->childHead;
        if(currentChild != NULL){
            runFCFSSJF(currentChild);
        }
        enteredHead = enteredHead->next;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <policy> <input_filename>\n", argv[0]);
        return 1;
    }

    if(argv[3]==NULL){
        FILE* inputFile = fopen(argv[2], "r");
        if (inputFile == NULL) {
            perror("File open error");
            return 1;
        }

        if (strcmp(argv[1], "BATCH") == 0) {
            printf("FCFS\n");

            char executable[100];
            int priority;
            while (fscanf(inputFile, "%s %d", executable, &priority) != EOF) {
                struct process process;
                strcpy(process.processName, executable);
                process.priority = priority;
                process.status = READY;
                insert(process);
            }
            fclose(inputFile);
            clock_t t;
            t = clock();
            runFCFSSJF(head);
            t = clock() - t;
            double time_taken = ((double)t)/CLOCKS_PER_SEC;
            printf("FCFS ran for %f seconds\n", time_taken);
        }
        else if(strcmp(argv[1], "SJF") == 0){
            printf("SJF\n");

            char executable[100];
            int priority;
            while (fscanf(inputFile, "%s %d", executable, &priority) != EOF) {
                struct process process;
                strcpy(process.processName, executable);
                process.priority = priority;
                process.status = READY;
                insertWithPriority(process);
            }
            fclose(inputFile);
            clock_t t;
            t = clock();
            runFCFSSJF(head);
            t = clock() - t;
            double time_taken = ((double)t)/CLOCKS_PER_SEC;
            printf("SJF ran for %f seconds\n", time_taken);

        }
    } else if(argv[3]!=NULL){
        FILE* inputFile = fopen(argv[3], "r");
        if (inputFile == NULL) {
            perror("File open error");
            return 1;
        }

        if (strcmp(argv[1], "RR") == 0) {

            printf("\nERROR! Τhere is no code for RR\n");
        }
        else if (strcmp(argv[1], "PRIO") == 0) {
            printf("\nERROR! Τhere is no code for PRIO\n");
        }
    }
    else {
        fprintf(stderr, "Invalid policy: %s\n", argv[1]);
        return 1;
    }
    display_forward();
    printf("\n");

    return 0;
}
