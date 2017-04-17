#include <stdio.h>
#include <string.h>

void vulnfunc(char * s) {
    char buf[20];

    strcpy(buf, s);
    printf("\nEntered Text: %s\n", buf);
}

void hiddenfunc(void) {
    printf("This should never happen!\n");
}

int main(int argc, char ** argv) {
    vulnfunc(argv[1]);

    return 0;
}
