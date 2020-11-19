#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define MAX_NUMBEROFSTUDENTS 100
#define MAX_STUDENT_NAME_LEN 0x20
#define MAX_SUBJECT_NAME_LEN 0x20

typedef struct {
    unsigned int score;
    char credit[4];
    char subject_name[MAX_SUBJECT_NAME_LEN];
} grade;

typedef struct {
    char name[MAX_STUDENT_NAME_LEN + 4];
    unsigned int age;
} student;

int cnt;
student *students[MAX_NUMBEROFSTUDENTS];

void intro() {
    puts("========== Yonsei University Grade Management System ==========\n");
}

void menu() {
    puts("[a]dd");
    puts("[d]elete");
    puts("[p]rint");
    puts("[m]odify");
    puts("[e]xit");
}

void calc_credit(char* credit, unsigned int score) {
    if (score >= 95) strcpy(credit, "A+");
    else if (score >= 90) strcpy(credit, "A");
    else if (score >= 85) strcpy(credit, "B+");
    else if (score >= 80) strcpy(credit, "B");
    else if (score >= 75) strcpy(credit, "C+");
    else if (score >= 70) strcpy(credit, "C");
    else if (score >= 65) strcpy(credit, "D+");
    else if (score >= 60) strcpy(credit, "D");
    else strcpy(credit, "F");
}

void read_line(char* buf, int len) {
    __fpurge(stdin);
    fgets(buf, len, stdin);
    buf[strlen(buf) + 1] = 0; // poison NULL byte
}

void input_student_info(char *name, unsigned int *age) {
    puts("\n[+] input name.");
    read_line(name, MAX_STUDENT_NAME_LEN);
    int len = strlen(name);
    if (name[len - 1] == '\n') name[len - 1] = 0;

    puts("[+] input age.");
    scanf("%d", age);
}

void print_student_info(student *s) {
    printf("[+] name: %s\n", s->name);
    printf("[+] age: %d\n", s->age);
}

void input_grade(char *subject_name, unsigned int *score) {
    puts("[+] input subject name.");
    read_line(subject_name, MAX_SUBJECT_NAME_LEN);
    int len = strlen(subject_name);
    if (subject_name[len - 1] == '\n') subject_name[len - 1] = 0;

    puts("[+] input score.");
    scanf("%d", score);
}

void print_grade(grade *g) {
    printf("[+] subject name: %s\n", g->subject_name);
    printf("[+] score: %d\n", g->score);
    printf("[+] credit: %s\n", g->credit);
}

void add() {
    char name[MAX_STUDENT_NAME_LEN + 1] = { 0. };
    unsigned int age = 0;
    input_student_info(name, &age);

    unsigned int num_subjects = 0;
    puts("\n[+] input number of subjects.");
    scanf("%d", &num_subjects);

    int alloc_size = sizeof(student) + sizeof(grade) * num_subjects;
    student *s = (student *)malloc(alloc_size);
    if (s == NULL) {
        puts("[-] fail to add student info.\n");
        return;
    }
    students[cnt++] = s;

    strcpy(s->name, name);
    s->age = age;
    
    puts("");
    for (int i = 0; i < num_subjects; i++) {
        grade *g = (void *)s + sizeof(student) + sizeof(grade) * i;

        printf("[+] subject %d\n", i + 1);
        input_grade(g->subject_name, &g->score);
        calc_credit(g->credit, g->score);
        puts("");
    }

    puts("[+] added.\n");
}

void delete() {
    unsigned int index = 0;

    puts("\n[+] input student index.");
    scanf("%d", &index);

    if (students[index] == NULL) {
        puts("[-] student doesn't exist.\n");
        return;
    }

    free(students[index]);
    cnt--;

    puts("[+] deleted.\n");
}

void print() {
    unsigned int index = 0;
    unsigned int num_subjects = 0;
    
    puts("\n[+] input student index.");
    scanf("%d", &index);

    if (students[index] == NULL) {
        puts("[-] student doesn't exist.\n");
        return;
    }

    puts("");
    print_student_info(students[index]);
    puts("");

    num_subjects = (*(int *)((void *)students[index] - 8) - sizeof(student)) / sizeof(grade);

    for (int i = 0; i < num_subjects; i++) {
        print_grade((void *)students[index] + sizeof(student) + sizeof(grade) * i);
        puts("");
    }
}

void modify() {
    unsigned int index = 0;
    unsigned int num_subjects = 0;

    puts("\n[+] input student index.");
    scanf("%d", &index);

    if (students[index] == NULL) {
        puts("[-] student doesn't exist.\n");
        return;
    }

    num_subjects = (*(int *)((void *)students[index] - 8) - sizeof(student)) / sizeof(grade);

    puts("");
    for (int i = 0; i < num_subjects; i++) {
        grade *g = (void *)students[index] + sizeof(student) + sizeof(grade) * i;

        printf("[+] subject %d\n", i + 1);
        input_grade(g->subject_name, &g->score);
        calc_credit(g->credit, g->score);
        puts("");
    }

    puts("[+] modified.\n");
}

int main() {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);

    char op[8] = { 0, };

    intro();
    
    while (1) {
        menu();
        puts("\n[+] input menu.");
        read_line(op, 7);
        
        switch (op[0]) {
        case 'a':
        case 'A':
            add();
            break;
        case 'd':
        case 'D':
            delete();
            break;
        case 'm':
        case 'M':
            modify();
            break;
        case 'p':
        case 'P':
            print();
            break;
        case 'e':
        case 'E':
            puts("[+] Exit program.");
            return 0;
        default:
            puts("[-] Invalid.\n");
            break;
        }
    }

    return 0;
}