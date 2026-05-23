#include <stdio.h>
int n;
long int iterative(int n) {
long int a,b,c,i;
a=0;
b=1;
if (n == 0) {
return a;}
else {
for (i=2; i<=n; ++i) {
c=b;
b+=a;
a=c; }
return b;}
}

long int recursive(int n) {
if (n <=1) { 
return n;}
return recursive(n-1) + recursive(n-2);
}

int main() {
printf("enter n\n");
scanf("%d", &n);
printf("iterative started\n");
printf("iterative answer - %ld\n", iterative(n));
printf("recursive started\n");
printf("recursive answer - %ld\n", recursive(n));
}
