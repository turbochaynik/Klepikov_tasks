#include <stdio.h>
double x, a, s1, s2, n;
int main(void) {
s1=0;
s2 = 0;
n=1;
scanf("%lf\n", &x);
while (scanf("%lf\n", &a) == 1) {
        s1 = s1*x + a;
	s2 = (s1 - a)/x + s2*x;


}
printf(" function in point = %lf\n", s1);
printf(" derivative in point = %lf\n", s2);
}

