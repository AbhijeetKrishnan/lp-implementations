#include <stdio.h>

#define PI 3.1415
#define square(radius) (radius*radius)
#define area(radius) (PI*square(radius))

int main() {
    int radius;
    float area;
    printf("Enter the radius: ");
    scanf("%d", &radius);
    area=area(radius);
    printf("Area=%.2f", area);
    return 0;
}
