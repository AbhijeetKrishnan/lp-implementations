#include <stdio.h>

#define ADD(x,y) (x+y)
#define MULT(x,y) (x*y)

int main() {
    int num;
    num=ADD(MULT(3,4),ADD(MULT(5,2),1));
    printf("Number=%d\n", num);
    return 0;
}
