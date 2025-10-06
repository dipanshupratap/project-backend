#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "backend.h"

// Queue Functions
void initQueue(Queue* q) { q->front = 0; q->rear = -1; q->size = 0; }
void enqueue(Queue* q, char* str) {
    if (q->size < 7) {
        q->rear = (q->rear + 1) % 7;
        strcpy(q->arr[q->rear], str);
        q->size++;
    }
}

// Password Strength Checker
const char* checkStrength(const char* pwd) {
    int uc = 0, lc = 0, dg = 0, sp = 0, len = 0;
    for (const char* p = pwd; *p; p++) {
        if (isupper(*p)) uc++;
        else if (islower(*p)) lc++;
        else if (isdigit(*p)) dg++;
        else sp++;
        len++;
    }
    if ((uc + lc == len) && len < 6)
        return "Weak";
    else if ((uc + lc + dg == len) && dg > 0 && len >= 6)
        return "Medium";
    else if ((uc + lc + dg + sp == len) && dg > 0 && sp > 0 && len >= 8)
        return "Strong";
    else
        return "Weak";
}

// Password Generator
void generatePasswords(Queue* q, char pwds[3][MAX_LEN]) {
    snprintf(pwds[0], MAX_LEN, "%c%c@%s", q->arr[0][0], q->arr[1][0], q->arr[3]);
    char reverseDOB[MAX_LEN] = "";
    int len = strlen(q->arr[3]);
    for (int i = len - 1; i >= 0; i--) {
        char temp[2] = {q->arr[3][i], '\0'};
        strcat(reverseDOB, temp);
    }
    snprintf(pwds[1], MAX_LEN, "%s_%s!", q->arr[1], reverseDOB);
    char initials[8] = "";
    for (int i = 0; i < 5 && i < 7; i++) {
        strncat(initials, q->arr[i], 1);
    }
    snprintf(pwds[2], MAX_LEN, "%s%s", initials, q->arr[3]);
}

// Map functions
void addToMap(Map* m, const char* password, Queue* q) {
    strcpy(m->passwords[m->count], password);
    for (int i = 0; i < 7; i++) strcpy(m->answers[m->count][i], q->arr[(q->front + i) % 7]);
    m->count++;
}

int findInMap(Map* m, const char* password) {
    for (int i = 0; i < m->count; i++)
        if (strcmp(m->passwords[i], password) == 0)
            return i;
    return -1;
}

int decodePassword(Map* m, const char* password, char answers[7][MAX_LEN]) {
    int idx = findInMap(m, password);
    if (idx == -1) return 0;
    for (int i = 0; i < 7; i++)
        strcpy(answers[i], m->answers[idx][i]);
    return 1;
}

void savePasswordToFile(const char* password, Queue* q) {
    FILE* fp = fopen("passwords.txt", "a");
    if (!fp) return;
    fprintf(fp, "Password: %s\n", password);
    for (int i = 0; i < 7; i++)
        fprintf(fp, "%d: %s\n", i + 1, q->arr[(q->front + i) % 7]);
    fprintf(fp, "---\n");
    fclose(fp);
}
