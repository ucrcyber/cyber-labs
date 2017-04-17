#include <stdio.h>
#include <string.h>

void vulnfunc(char * s) {
    char buf[100];

    strcpy(buf, s);
    printf("\nEntered Text: %s\n", buf);
}

int main(int argc, char ** argv) {
    vulnfunc(argv[1]);

    return 0;
}
