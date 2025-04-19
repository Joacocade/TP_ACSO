#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(){
    // a, b signed ints donde a + b - 0x20 debe tener 11 bits prendidos

    int a = 0x20;
    signed int b = 0x800003ff; // Binary: 1000 0000 0000 0000 0000 0011 1111 1111 (11 bits prendidos 1)

    printf("a = 0x%x\n", a);
    printf("b = 0x%x\n", b);
    printf("a = %d\n", a);
    printf("b = %d\n", b);


    int result1 = a + b - 0x20;
    bool result2 = (a ^ b) < 0;
    printf("a + b - 0x20 = %d\n", result1);
    printf("a + b - 0x20 (binary) = ");
    for(int i = 31; i >= 0; i--) {
        printf("%d", (result1 >> i) & 1);
        if(i % 4 == 0) printf(" ");
    }
    printf("\n");
    printf("a ^ b < 0 : %s\n", result2 ? "True" : "False");
}