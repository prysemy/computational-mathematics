# Вычисление интеграла быстроосциллирующей функции

## Условие задачи

Вычислить интеграл:
$I = \int_{0}^{3} \sin(100x) \cdot e^{-x^2} \cdot \cos(2x)  dx$

с точностью $10^{-6}$.

## Методы численного интегрирования

### 1. Составные формулы

#### Метод средних прямоугольников
\[ \int_a^b f(x) dx \approx \sum_{i=1}^N f\left(\frac{x_{i-1} + x_i}{2}\right) \Delta x_i \]

#### Метод трапеций
\[ \int_a^b f(x) dx \approx \sum_{i=1}^N \frac{f(x_{i-1}) + f(x_i)}{2} \Delta x_i \]

#### Метод Симпсона
\[ \int_a^b f(x) dx \approx \sum_{i=1}^N \frac{f(x_{i-1}) + 4f\left(\frac{x_{i-1}+x_i}{2}\right) + f(x_i)}{6} \Delta x_i \]

#### Правило 3/8
\[ \int_a^b f(x) dx \approx \sum_{i=1}^N \frac{f(x_{i-1}) + 3f\left(\frac{2x_{i-1}+x_i}{3}\right) + 3f\left(\frac{x_{i-1}+2x_i}{3}\right) + f(x_i)}{8} \Delta x_i \]

### 2. Формулы Гаусса

#### Общая формула Гаусса
\[ \int_a^b f(x) dx \approx \frac{b-a}{2} \sum_{i=1}^n w_i f\left(\frac{a+b}{2} + \frac{b-a}{2} \xi_i\right) \]

#### Гаусс 2 узла
\[ \int_a^b f(x) dx \approx \frac{b-a}{2} \left[ f\left(\frac{a+b}{2} - \frac{b-a}{2\sqrt{3}}\right) + f\left(\frac{a+b}{2} + \frac{b-a}{2\sqrt{3}}\right) \right] \]

#### Гаусс 3 узла
\[ \int_a^b f(x) dx \approx \frac{b-a}{2} \left[ \frac{5}{9}f\left(\frac{a+b}{2} - \frac{b-a}{2}\sqrt{\frac{3}{5}}\right) + \frac{8}{9}f\left(\frac{a+b}{2}\right) + \frac{5}{9}f\left(\frac{a+b}{2} + \frac{b-a}{2}\sqrt{\frac{3}{5}}\right) \right] \]

#### Гаусс 4 узла
\[ \int_a^b f(x) dx \approx \frac{b-a}{2} \sum_{i=1}^4 w_i f\left(\frac{a+b}{2} + \frac{b-a}{2} \xi_i\right) \]
где:
- \(\xi_{1,4} = \pm \sqrt{\frac{3}{7} + \frac{2}{7}\sqrt{\frac{6}{5}}}\)
- \(\xi_{2,3} = \pm \sqrt{\frac{3}{7} - \frac{2}{7}\sqrt{\frac{6}{5}}}\)
- \(w_{1,4} = \frac{18-\sqrt{30}}{36}\)
- \(w_{2,3} = \frac{18+\sqrt{30}}{36}\)

### 3. Адаптивный метод

#### Адаптивный Симпсон
Рекурсивное применение:
\[ I \approx I_{\text{left}} + I_{\text{right}} \]
с контролем погрешности \(|S_1 - S_2| < 15\cdot\text{tol}\)

## Запуск

```bash
# Компиляция C++ программы
g++ -o integrator main.cpp -lm
./integrator

# Визуализация результатов
dotnet run
