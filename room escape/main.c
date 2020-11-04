#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define PW_MAX_LEN 100
#define SLEEP_TIME 1

char* password = "zjavbxj rjsemflaus ghssksek";

void read_line(char* buf, int len) {
    __fpurge(stdin);
    fgets(buf, len, stdin);
    if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;
}

void intro() {
    sleep(SLEEP_TIME);
    puts("You're locked up in a dark room.");
    sleep(SLEEP_TIME);
    puts("You see some stuffs around you.");
    sleep(SLEEP_TIME);
    puts("Find the clues and escape from here.");
}

void print_menu() {
    sleep(SLEEP_TIME);
    puts("");
    puts("0. give up");
    puts("1. safe");
    puts("2. computer");
    puts("3. post-it");
}

void safe() {
    int pw;
    char input_char[8] = { 0, };
    int input;

    sleep(SLEEP_TIME);
    puts("\nYou found a small safe.");
    sleep(SLEEP_TIME);
    puts("Password is four-digit number. (It can start with 0)");
    sleep(SLEEP_TIME);

input:
    puts("\nInput password. (0000 ~ 9999. 'q' to give up)");
    printf("> ");
    srand(time(0));
    pw = rand() % 10000;
    read_line(input_char, 8);

    if (!strcmp(input_char, "q") || !strcmp(input_char, "q\n")) {
        return;
    }
    else if (strlen(input_char) < 4) {
        puts("Too short!");
        sleep(SLEEP_TIME);
        goto input;
    }

    input = atoi(input_char);
    if (input == pw) puts("Correct.");
    else if (input >= 0 && input <= 9999) {
        puts("Wrong.");
        sleep(SLEEP_TIME);
        goto input;
    }
    else {
        sleep(SLEEP_TIME);
        puts("\nSomething's wrong...");
        sleep(SLEEP_TIME);
        puts("Maybe you triggered some kind of bug.");
        sleep(SLEEP_TIME);
        puts("\nsrand(time(0));");
        puts("int pw = rand() % 10000;");
        sleep(SLEEP_TIME);
        goto input;
    }
    
    sleep(SLEEP_TIME);
    puts("\nThere is a photo in the safe. > https://bit.ly/3iXDynM");
}

void computer() {
    char* pw = malloc(PW_MAX_LEN + 1);

    sleep(SLEEP_TIME);
    puts("\nThere's an old computer in the corner.");
    sleep(SLEEP_TIME);
    puts("You turned it on.");
    sleep(SLEEP_TIME);
    puts("There's a password on it.");
    sleep(SLEEP_TIME);

input:
    printf("\nInput password. (maximum %d bytes. 'q' to give up) > ", PW_MAX_LEN);
    read_line(pw, PW_MAX_LEN);

    if (!strcmp(pw, "q") || !strcmp(pw, "q\n")) {
        return;
    }
    else if (strcmp(pw, password)) {
        puts("Wrong.");
        sleep(SLEEP_TIME);
        goto input;
    }

    puts("Correct.");
    sleep(SLEEP_TIME);
    puts("\nUnfortunately, the Internet is not working.");
    sleep(SLEEP_TIME);
    puts("...");
    sleep(SLEEP_TIME);
    puts("You found a suspicious zip file. > https://bit.ly/34WbT2P");
}

void post_it() {
    sleep(SLEEP_TIME);
    puts("\nYou see a post-it on the table.");
    sleep(SLEEP_TIME);
    puts("There's something written all over it.");
    sleep(SLEEP_TIME);
    puts("It looks like some kind of signal.");
    sleep(SLEEP_TIME);
    puts("_-_-_---_--_--___--___--_----____--__-____--__-__-___--__-__-____-_-_-_-_--_---__-_-__-__-_-_-___--____-_--_--___-__-_-__--_----_-_--__-_-_-_--__-_-_--__-__-_--_-__---__-_-_--__--_--___-_--____-__---__-_-_-_-__---__-_--_-__-_-_-__-___--_____---_____-_--__-_-_-_--___--__-___--_-_-_-__----_-_-_---_--_--___--__-___-___---_-_-_---_--_-_-__-_--_-__-_-_-___-_-_--__-_-_---_-_-__-__---__-__-_--__-_--_-_-__-__-_-__-__---__--__-_-_--_--___-_-__-__---_-_-_-_-__--_--_--_-_---_____-__---__--___-__-___--__---______--_-_-");
}

int main() {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);

    int op = 0;;

    intro();

    while (1) {
        print_menu();
        printf("> ");
        scanf("%d", &op);

        switch (op) {
            case 0:
                puts("You failed to escape :(");
                sleep(SLEEP_TIME);
                exit(0);
                break;
            case 1:
                safe();
                break;
            case 2:
                computer();
                break;
            case 3:
                post_it();
                break;
            default:
                puts("Invalid.");
                break;
        }
        sleep(SLEEP_TIME);
    }
}