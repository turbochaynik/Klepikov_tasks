#include <stdio.h>
int main(void) {
double e, x, x1, x2;
int i;
printf("enter e\n");
scanf("%lf", &e);
printf("enter x\n");
while(scanf("%lf", &x) == 1) {
x1 = 1;
x2 = (0.5)*(x1 + (x/x1));
while( ((x1-x2)>=e) || ((x2-x1)>=e)) {
x1=x2;
x2 = (0.5)*(x1 + (x/x1));

}
printf("%lf\n", x2);
printf("enter x\n");
}
}
