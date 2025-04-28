#include <stdio.h>
#include <string.h>

int is_prime(int num) {
    for (int i = num-1; i>1; i--){
        if (num%i==0){
            int j = num%i;
            //printf("%d\n",j);
            return 0;
        }
    }
    return 1;
}

int main(){
    printf("%d",is_prime(12));

    return 0;
}