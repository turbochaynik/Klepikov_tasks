#include <stdio.h>
double str2double(char str[]) {
double result = 0;  //результат
double fraction = 1;  //для дробной части
int sign = 1; // знак
int i = 0;
while (str[i]==' ') {
	i+=1;
}
if (str[i] == '-') {
	sign = -1;
        i+=1; }
else if (str[i] == '+') {
	i+=1;
}
while ((str[i]<='9') && (str[i]>='0')) {
        result = result * 10 + (str[i] - '0');
	i+=1;
    }
    if (str[i] == '.') {
        i+=1;
        while ((str[i]<='9') && (str[i]>='0')) {
            fraction /= 10;
            result += (str[i] - '0') * fraction;
            i+=1;
        }
    }
    if (str[i] == 'e' || str[i] == 'E') {
        i+=1;
        int exp_sign = 1; //знак после  экспоненты
        if (str[i] == '-') {
            exp_sign = -1;
            i+=1;
        } else if (str[i] == '+') {
            i+=1;
        }

        int exponent = 0;  //флаг, который показывает, на 10 в какой степени мы будем умножать/делить число
        while ((str[i]<='9') && (str[i]>='0')) {
            exponent = exponent * 10 + (str[i] - '0');
            i++;
        }

        double power = 1; //само умножение 
        for (int j = 0; j < exponent; j++) {
            power *= 10;
        }

        if (exp_sign == -1) {
            result /= power;
        } else {
            result *= power;
        }
    }
return sign * result;
}

int main() {
	char input[100];
	while (scanf("%s", input) != EOF) {
        double value = str2double(input);
        printf("%.10g\n", value);
    }

    return 0;
}

