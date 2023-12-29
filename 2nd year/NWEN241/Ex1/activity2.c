#include <stdio.h>

int main(void) {
    printf("Enter a string without whitespaces: ");
    char input[30];
    scanf("%s", input);
    printf("%s", input);
    return 0;
}
