#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    int a, b;
    FILE *in; // use for handling input file
    FILE *out; // use for handling output file

    // Open raw.txt for reading
    in = fopen("raw.txt", "r");
    if (in == NULL) {
        printf("input file failed to open\n");
    }
    // Open processed.txt for writing
    out = fopen("processed.txt", "w");
    // Go thru raw.txt file and generate processed.txt file accordingly
    while (fscanf(in, "%d %d", &a, &b) == 2) {
        fprintf(out, "%d %d %d\n", a, b, a+b);
    }

    fclose(in);
    fclose(out);
    return 0;
}
