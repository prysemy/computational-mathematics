# Вычисление интеграла быстроосциллирующей функции

## Условие задачи

Вычислить интеграл:
$I = \int_{0}^{3} \sin(100x) \cdot e^{-x^2} \cdot \cos(2x)  dx$

с точностью $10^{-6}$.

## Методы численного интегрирования

### 1. Составные формулы

#### Метод средних прямоугольников

$\int_a^b f(x) dx \approx \sum_{i=1}^N f\left(\frac{x_{i-1} + x_i}{2}\right) \Delta x_i$

#### Метод трапеций

$\int_a^b f(x) dx \approx \sum_{i=1}^N \frac{f(x_{i-1}) + f(x_i)}{2} \Delta x_i$

#### Метод Симпсона

$\int_a^b f(x) dx \approx \sum_{i=1}^N \frac{f(x_{i-1}) + 4f\left(\frac{x_{i-1}+x_i}{2}\right) + f(x_i)}{6} \Delta x_i$

#### Правило 3/8

$\int_a^b f(x) dx \approx \sum_{i=1}^N \frac{f(x_{i-1}) + 3f\left(\frac{2x_{i-1}+x_i}{3}\right) + 3f\left(\frac{x_{i-1}+2x_i}{3}\right) + f(x_i)}{8} \Delta x_i$

### 2. Формулы Гаусса

#### Общая формула Гаусса

$\int_a^b f(x) dx \approx \frac{b-a}{2} \sum_{i=1}^n w_i f\left(\frac{a+b}{2} + \frac{b-a}{2} \xi_i\right)$

#### Гаусс 2 узла

Узлы: $\xi_{1,2} = \pm \frac{1}{\sqrt{3}}$

Веса: $w_{1,2} = 1$

#### Гаусс 3 узла

Узлы: $\xi_{1,3} = \pm \sqrt{\frac{3}{5}}, \xi_2 = 0$

Веса: $w_{1,3} = \frac{5}{9}, w_2 = \frac{8}{9}$

#### Гаусс 4 узла

Узлы: $\xi_{1,4} = \pm 0.861136, \xi_{2,3} = \pm 0.339981$

Веса: $w_{1,4} = 0.347855, w_{2,3} = 0.652145$
