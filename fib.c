#include<stdio.h>
#include<stdlib.h>
int fib(int n){
    if(n==0 || n==1){
        return n;
    }
    return (fib(n-1)+fib(n-2));
}
int main(int argc,char **argv){
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }
    int n=atoi(argv[1]);
    if (n < 0) {
        printf("Please provide a non-negative integer.\n");
        return 1;
    }
    printf("%d\n",fib(n));
    return 0;
}