#include <stdio.h>

int main(void) {
    // gathering data
    printf("Enter temperature: ");
    float ctemp,ftemp;
    scanf("%f", &ctemp);

    //returning data
    ftemp = ctemp * 9/5 + 32;
    printf("%.3f", ftemp);
    return 0;
}
