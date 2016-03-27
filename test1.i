#include <stdio.h>


int main() {
    int radius;
    float area;
    printf("Enter the radius: ");
    scanf("%d", &radius);
    area=(3.1415*(radius*radius));
    printf("Area=%.2f", area);
    return 0;
}
