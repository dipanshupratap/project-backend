#ifndef BACKEND_H
#define BACKEND_H

#define MAX_LEN 100

typedef struct {
    char arr[7][MAX_LEN];
    int front, rear, size;
} Queue;

typedef struct {
    char passwords[100][MAX_LEN];
    char answers[100][7][MAX_LEN];
    int count;
} Map;

void initQueue(Queue* q);
void enqueue(Queue* q, char* str);
void generatePasswords(Queue* q, char pwds[3][MAX_LEN]);
const char* checkStrength(const char* pwd);
void addToMap(Map* m, const char* password, Queue* q);
int findInMap(Map* m, const char* password);
int decodePassword(Map* m, const char* password, char answers[7][MAX_LEN]);
void savePasswordToFile(const char* password, Queue* q);

#endif
