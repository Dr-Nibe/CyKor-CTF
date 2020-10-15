#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define PW_MAX_LEN 100

char* password = "zjavbxj rjsemflaus ghssksek";

void read_line(char* buf, int len) {
    __fpurge(stdin);
    fgets(buf, len, stdin);
    if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;
}

void intro() {
    sleep(3);
    puts("You're locked up in a dark room.");
    sleep(3);
    puts("You see some stuffs around you.");
    sleep(3);
    puts("Find the clues and escape from here.");
}

void print_menu() {
    sleep(3);
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

    sleep(3);
    puts("\nYou found a small safe.");
    sleep(3);
    puts("Password is four-digit number. (It can start with 0)");
    sleep(3);

input:
    puts("\nInput password. (0000 ~ 9999)");
    printf("> ");
    srand(time(0));
    pw = rand() % 10000;
    read_line(input_char, 8);

    if (strlen(input_char) < 4) {
        puts("Too short!");
        sleep(3);
        goto input;
    }

    input = atoi(input_char);
    if (input == pw) puts("Correct.");
    else if (input >= 0 && input <= 9999) {
        puts("Wrong.");
        sleep(3);
        goto input;
    }
    else {
        sleep(3);
        puts("\nSomething's wrong...");
        sleep(3);
        puts("Maybe you triggered some kind of bug.");
        sleep(3);
        puts("\nsrand(time(0));");
        puts("int pw = rand() % 10000;");
        sleep(3);
        goto input;
    }
    
    sleep(3);
    puts("\nThere is a photo in the safe. > https://bit.ly/3iXDynM");
}

void computer() {
    char* pw = malloc(PW_MAX_LEN + 1);

    sleep(3);
    puts("\nThere's an old computer in the corner.");
    sleep(3);
    puts("You turned it on.");
    sleep(3);
    puts("There's a password on it.");
    sleep(3);

input:
    printf("\nInput password. (maximum %d bytes) > ", PW_MAX_LEN);
    read_line(pw, PW_MAX_LEN);

    if (strcmp(pw, password) != 0) {
        puts("Wrong.");
        sleep(3);
        goto input;
    }

    puts("Correct.");
    sleep(3);
    puts("\nUnfortunately, the Internet is not working.");
    sleep(3);
    puts("...");
    sleep(3);
    puts("You found a suspicious zip file. > https://bit.ly/3nOe0wY");
}

void post_it() {
    sleep(3);
    puts("\nYou see a post-it on the table.");
    sleep(3);
    puts("There's something written all over it.");
    sleep(3);
    puts("It looks like some kind of signal.");
    sleep(3);
    puts("_-_-_---_--_--___--___--_----____--__-____--__-__-___--__-__-____-_-_-_-_--_---__-_-__-__-_-_-___--____-_--_--___-__-_-__--_----_-_--__-_-_-_--__-_-_--__-__-_--_-__---__-_-_--__--_--___-_--____-__---__-_-_-_-__---__-_--_-__-_-_-__-___--_____---_____-_--__-_-_-_--___--__-___--_-_-_-__----_-_-_---_--_--___--__-___-___---_-_-_---_--_-_-__-_--_-__-_-_-___-_-_--__-_-_---_-_-__-__---__-__-_--__-_--_-_-__-__-_-__-__---__--__-_-_--_--___-_-__-__---_-_-_-_-__--_--_--_-_---_____-__---__--___-__-___--__---______--_-_-");
}

int main() {
    int op = 0;;

    intro();

    while (1) {
        print_menu();
        printf("> ");
        scanf("%d", &op);

        switch (op) {
            case 0:
                puts("You failed to escape :(");
                sleep(3);
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
        sleep(3);
    }
}