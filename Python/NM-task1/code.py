import math
import numpy
def f(x):
    return 1 / ((25 * x**2 + 1) * ((3 * x - x**2)**0.5))

# Метод трапеций

def trapezoid(a, b, N):
    h = (b - a) / N
    if a == 0 and b == 3:
        a += h
        b -= h
        return 2*numpy.arcsin(math.sqrt(1/N)) + (1/226)*(math.pi - 2*numpy.arcsin(math.sqrt(1-1/N))) + trapezoid(a, b, N - 2)
    result = 0.5 * (f(a) + f(b))
    for i in range(1, N):
        x = a + i * h
        result += f(x)
    result *= h
    return result

# Метод Симпсона

def sympson(a, b, N):
    h = (b - a) / N
    if a == 0 and b == 3:
        a += h
        b -= h
        return 2*numpy.arcsin(math.sqrt(1/N)) + (1/226)*(math.pi - 2*numpy.arcsin(math.sqrt(1-1/N))) + sympson(a, b, N-2)
    result = f(a) + f(b)
    for i in range(1, N):
        x = a + i * h
        if i % 2 != 0:
            result += 4 * f(x)
        else:
            result += 2 * f(x)
    return result * h / 3

intervals = [(1, 2), (0, 3)]
n_values = [16, 32, 64]
for a, b in intervals:
    print(f"Интеграл на отрезке [{a}, {b}]:")
    for n in n_values:
        trap_result = trapezoid(a, b, n)
        simp_result = sympson(a, b, n)
        print(f"n = {n}, Метод трапеций: {trap_result:.6f}, Метод Симпсона: {simp_result:.6f}")
        if a == 0:
            print("Погрешность трапеций -", (90*math.sqrt(3)/8)*numpy.arcsin(1/math.sqrt(n))/n + ((150 - 150/n)/(226 - 450/n + 225/(n**2)))*3/n * numpy.arcsin(math.sqrt(1 - 1/n)) + abs(trapezoid(a, b, n-2)-trapezoid(a, b, (n-2)//2))/3)
            print("Погрешность Симпсона -", (90*math.sqrt(3)/8)*numpy.arcsin(1/math.sqrt(n))/n + ((150 - 150/n)/(226 - 450/n + 225/(n**2)))*3/n * numpy.arcsin(math.sqrt(1 - 1/n)) + abs(sympson(a, b, n-2)-sympson(a, b, (n-2)//2))/15)
        else:
            print("Погрешность трапеций -", (abs(trapezoid(a, b, n)-trapezoid(a, b, n//2))/3))
            print("Погрешность Симпсона -", (abs(sympson(a, b, n)-sympson(a, b, n//2))/15))
        
    print()    


