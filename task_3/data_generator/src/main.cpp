#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <algorithm>

/**
 * @brief Функция коэффициента при y' (p(x) = x^2 - 3)
 */
double p(double x) {
    return x * x - 3.0;
}

/**
 * @brief Функция коэффициента при y (q(x) = (x^2 - 3)*cos(x))
 */
double q(double x) {
    return (x * x - 3.0) * std::cos(x);
}

/**
 * @brief Функция правой части уравнения f(x)
 */
double f(double x) {
    return 2.0 - 6.0 * x + 2.0 * std::pow(x, 3)
           + (std::pow(x, 2) - 3.0) * std::exp(x) * std::sin(x) * (1.0 + std::cos(x))
           + std::cos(x) * (std::exp(x) + (std::pow(x, 2) - 1.0) + std::pow(x, 4) - 3.0 * std::pow(x, 2));
}

/**
 * @brief Метод прогонки (Томаса) для трехдиагональной системы
 */
std::vector<double> thomasAlgorithm(const std::vector<double> &a,
                                    const std::vector<double> &b,
                                    const std::vector<double> &c,
                                    const std::vector<double> &d) {
    int n = d.size();
    std::vector<double> alpha(n, 0.0), beta(n, 0.0), x(n, 0.0);

    alpha[0] = -c[0] / b[0];
    beta[0] = d[0] / b[0];

    for (int i = 1; i < n - 1; ++i) {
        double denominator = b[i] + a[i] * alpha[i - 1];
        alpha[i] = -c[i] / denominator;
        beta[i] = (d[i] - a[i] * beta[i - 1]) / denominator;
    }

    x[n - 1] = (d[n - 1] - a[n - 1] * beta[n - 2]) /
               (b[n - 1] + a[n - 1] * alpha[n - 2]);

    for (int i = n - 2; i >= 0; --i) {
        x[i] = alpha[i] * x[i + 1] + beta[i];
    }

    return x;
}

int main() {
    double a = 0.0; // левая граница
    double b = M_PI; // правая граница (π)
    double ya = 0.0; // y(0) = 0
    double yb = M_PI * M_PI; // y(π) = π^2

    int N = 1000;
    double h = (b - a) / N;

    std::cout << "===========================================\n";
    std::cout << "РЕШЕНИЕ КРАЕВОЙ ЗАДАЧИ\n";
    std::cout << "===========================================\n";
    std::cout << "Уравнение: y'' + (x^2-3)y' + (x^2-3)cos(x)*y = f(x)\n";
    std::cout << "Граничные условия: y(0) = 0, y(π) = π²\n";
    std::cout << "Интервал: [" << a << ", " << b << "]\n";
    std::cout << "Число узлов: " << N << "\n";
    std::cout << "Шаг сетки: " << h << "\n";
    std::cout << "===========================================\n\n";

    // Создаем векторы для трехдиагональной матрицы
    // Уравнение: y_{i-1} * A_i + y_i * B_i + y_{i+1} * C_i = D_i
    std::vector<double> A(N - 1, 0.0); // нижняя диагональ (i=1..N-1)
    std::vector<double> B(N - 1, 0.0); // главная диагональ (i=1..N-1)
    std::vector<double> C(N - 1, 0.0); // верхняя диагональ (i=1..N-1)
    std::vector<double> D(N - 1, 0.0); // правая часть (i=1..N-1)

    // Заполняем коэффициенты для внутренних узлов (i = 1..N-1)
    for (int i = 1; i < N; ++i) {
        double x = a + i * h; // координата узла

        // Конечные разности второго порядка:
        // y'' ≈ (y_{i-1} - 2y_i + y_{i+1})/h²
        // y' ≈ (y_{i+1} - y_{i-1})/(2h)

        // Коэффициент при y_{i-1}
        A[i - 1] = 1.0 / (h * h) - p(x) / (2.0 * h);

        // Коэффициент при y_i
        B[i - 1] = -2.0 / (h * h) + q(x);

        // Коэффициент при y_{i+1}
        C[i - 1] = 1.0 / (h * h) + p(x) / (2.0 * h);

        D[i - 1] = f(x);
    }

    // y(0) = 0: для i=1 уравнение включает y_0 = ya
    D[0] -= A[0] * ya;

    // y(π) = π²: для i=N-1 уравнение включает y_N = yb
    D[N - 2] -= C[N - 2] * yb;

    std::vector<double> y_internal = thomasAlgorithm(A, B, C, D);

    std::vector<double> x_full(N + 1, 0.0);
    std::vector<double> y_full(N + 1, 0.0);

    for (int i = 0; i <= N; ++i) {
        x_full[i] = a + i * h;
        if (i == 0) {
            y_full[i] = ya;
        } else if (i == N) {
            y_full[i] = yb;
        } else {
            y_full[i] = y_internal[i - 1];
        }
    }

    std::cout << "===========================================\n";
    std::cout << "ЗНАЧЕНИЯ РЕШЕНИЯ В ЗАДАННЫХ ТОЧКАХ\n";
    std::cout << "===========================================\n";
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "x\t\ty(x)\n";
    std::cout << "-------------------------------------------\n";

    std::vector<double> target_points = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0};

    for (double x_target: target_points) {
        double y_val = 0.0;

        int idx = static_cast<int>((x_target - a) / h);

        if (idx < 0) idx = 0;
        if (idx >= N) idx = N - 1;

        double x1 = x_full[idx];
        double x2 = x_full[idx + 1];
        double y1 = y_full[idx];
        double y2 = y_full[idx + 1];

        double t = (x_target - x1) / (x2 - x1);
        y_val = y1 + t * (y2 - y1);

        std::cout << x_target << "\t\t" << y_val << "\n";
    }

    std::ofstream file("data/results.csv");
    file << "x,y(x)\n";
    file << std::scientific << std::setprecision(10);

    for (int i = 0; i <= N; i += 10) {
        file << x_full[i] << "," << y_full[i] << "\n";
    }

    file.close();

    std::cout << "\n===========================================\n";
    std::cout << "АНАЛИЗ РЕШЕНИЯ:\n";
    std::cout << "===========================================\n";

    double y_min = *std::min_element(y_full.begin(), y_full.end());
    double y_max = *std::max_element(y_full.begin(), y_full.end());

    std::cout << "Количество узлов: " << N + 1 << "\n";
    std::cout << "Минимальное значение y: " << y_min << "\n";
    std::cout << "Максимальное значение y: " << y_max << "\n";
    std::cout << "Амплитуда: " << y_max - y_min << "\n";

    std::cout << "\nПроверка граничных условий:\n";
    std::cout << "y(0) = " << y_full[0] << " (должно быть 0)\n";
    std::cout << "y(π) = " << y_full[N] << " (должно быть " << M_PI * M_PI << ")\n";

    std::cout << "\n===========================================\n";
    std::cout << "ДОПОЛНИТЕЛЬНЫЕ КОНТРОЛЬНЫЕ ТОЧКИ:\n";
    std::cout << "===========================================\n";
    std::cout << "x\t\ty(x)\n";
    std::cout << "-------------------------------------------\n";

    std::vector<double> check_points = {
        0.0, M_PI / 6, M_PI / 4, M_PI / 3, M_PI / 2,
        2 * M_PI / 3, 3 * M_PI / 4, 5 * M_PI / 6, M_PI
    };

    for (double x_check: check_points) {
        int idx = static_cast<int>((x_check - a) / h);

        if (idx < 0) idx = 0;
        if (idx >= N) idx = N - 1;

        double x1 = x_full[idx];
        double x2 = x_full[idx + 1];
        double y1 = y_full[idx];
        double y2 = y_full[idx + 1];

        double t = (x_check - x1) / (x2 - x1);
        double y_val = y1 + t * (y2 - y1);

        std::cout << x_check << "\t\t" << y_val << "\n";
    }

    std::cout << "\n===========================================\n";
    std::cout << "Результаты сохранены в data/results.csv\n";
    std::cout << "Для визуализации запустите Program.cs\n";
    std::cout << "===========================================\n";

    return 0;
}
