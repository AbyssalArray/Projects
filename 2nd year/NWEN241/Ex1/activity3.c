#include <stdio.h>

int main(void) {
    printf("Enter a string: ");
    char input[30];
    scanf("%[^\n]", input);
    printf("%s", input);
    return 0;
}
