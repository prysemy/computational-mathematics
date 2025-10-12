# Решение системы уравнений

Найти все корни системы уравнений:

$$
\begin{cases} 
x^2 + y^2 = 1, \\ 
y = \tan{x}.
\end{cases}
$$

С точностью $10^{-6}$.

*Примечание:* корни отделить графическим методом.

## Решение методом Ньютона с матрицей Якоби

### Математическая формулировка

Перепишем систему в виде:

$$
\begin{cases}
f_1(x, y) = x^2 + y^2 - 1 = 0 \\
f_2(x, y) = y - \tan{x} = 0
\end{cases}
$$

### Матрица Якоби


$$
\begin{equation}
J(x, y) = \begin{bmatrix}
\frac{\partial f_1}{\partial x} & \frac{\partial f_1}{\partial y} \\
\frac{\partial f_2}{\partial x} & \frac{\partial f_2}{\partial y}
\end{bmatrix}
= \begin{bmatrix}
2x & 2y \\ -\frac{1}{\cos^2 x} & 1
\end{bmatrix}
\end{equation}
$$

### Итерационная формула метода Ньютона


$$
[x_{n+1}; y_{n+1}] = [x_n; y_n] - J^{-1}(x_n, y_n) · [f_1(x_n, y_n); f_2(x_n, y_n)]
$$

### Обратная матрица Якоби


$$
J^{-1}(x, y) = \frac{1}{\det J(x, y)} \begin{bmatrix}
1 & -2y \\
\frac{1}{\cos^2 x} & 2x
\end{bmatrix}
$$

где определитель:

$$
\det J(x, y) = 2x \cdot 1 - 2y \cdot \left(-\frac{1}{\cos^2 x}\right) = 2x + \frac{2y}{\cos^2 x}
$$

### Итерационный процесс

На каждой итерации:

1. Вычисляем вектор функции:


   $$
   F(x_n, y_n) =
   \begin{bmatrix}
   x_n^2 + y_n^2 - 1 \\
   y_n - \tg x_n
   \end{bmatrix}
   $$

2. Вычисляем матрицу Якоби и её определитель:
   
   $\det J = 2x_n + \frac{2y_n}{\cos^2 x_n}$

3. Вычисляем поправку:

   $\Delta = J^{-1} \cdot F = \frac{1}{\det J} \begin{bmatrix} 1 & -2y_n \\ \frac{1}{\cos^2 x_n} & 2x_n \end{bmatrix} \cdot \begin{bmatrix} x_n^2 + y_n^2 - 1 \\ y_n - \tg x_n \end{bmatrix}$

4. Обновляем решение:

   
   $$
   \begin{bmatrix}
   x_{n+1} \\
   y_{n+1}
   \end{bmatrix}
   = \begin{bmatrix}
   x_n \\
   y_n
   \end{bmatrix}
   - \Delta
   $$

### Итерации продолжаются до выполнения условия:


$$
\max(|x_{n+1} - x_n|, |y_{n+1} - y_n|) < 10^{-6}
$$
