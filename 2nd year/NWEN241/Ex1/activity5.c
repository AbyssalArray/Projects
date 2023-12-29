#include <stdio.h>

float calculate_area(float len, float wid) {
    return len * wid;
}

int main(void)
{
    float l, w, a;
    scanf("%f %f", &l, &w);
    a = calculate_area(l,w);
    printf("%f", a);
}
