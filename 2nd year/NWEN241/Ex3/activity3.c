#include <stdio.h>


int main(int argc, char* argv[])
{
    int sum = 0;
    int in;
    for (int i = 1; i < argc; i++) {
        sscanf(argv[i], "%d", &in);
        sum += in;
    }
    printf("%d\n", sum);
    return 0;
}
