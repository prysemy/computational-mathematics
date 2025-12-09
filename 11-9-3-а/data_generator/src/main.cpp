#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>

/**
 * @brief Функция для функции f
 * f(t, y, y') для уравнения: y'' = -f(t, y, y')
 * В нашей задаче: y'' - x√y = 0 => y'' = x√y
 * Поэтому: f(t, y, y') = -x√y
 */
double f(double x, double y, double dy) {
    if (y < 0) return 0.0;
    return -x * std::sqrt(y);
}

/**
 * @brief Функция для производной ∂f/∂y
 * f = -x√y => ∂f/∂y = -x/(2√y)
 */
double df_dy(double x, double y, double dy) {
    if (y <= 0) return 0.0;
    return -x / (2.0 * std::sqrt(y));
}

/**
 * @brief Функция для производной ∂f/∂y'
 * f = -x√y не зависит от y', поэтому ∂f/∂y' = 0
 */
double df_ddy(double x, double y, double dy) {
    return 0.0;
}

/**
 * @brief Функция метода Рунге-Кутты 4 порядка для системы 4-х ОДУ
 * Система:
 * 1) y' = v
 * 2) v' = -f(x, y, v)
 * 3) p' = q
 * 4) q' = -df_ddy*q - df_dy*p
 *
 * где p = ∂y/∂α, q = ∂v/∂α
 */
void runge_kutta_4_system(double x, double h, double &y, double &v, double &p, double &q) {
    double k1_y = h * v;
    double k1_v = h * (-f(x, y, v));
    double k1_p = h * q;
    double k1_q = h * (-df_ddy(x, y, v) * q - df_dy(x, y, v) * p);
    
    double k2_y = h * (v + k1_v/2);
    double k2_v = h * (-f(x + h / 2, y + k1_y / 2, v + k1_v / 2));
    double k2_p = h * (q + k1_q/2);
    double k2_q = h * (-df_ddy(x + h / 2, y + k1_y / 2, v + k1_v / 2) * (q + k1_q / 2)
                       - df_dy(x + h / 2, y + k1_y / 2, v + k1_v / 2) * (p + k1_p / 2));

    double k3_y = h * (v + k2_v/2);
    double k3_v = h * (-f(x + h / 2, y + k2_y / 2, v + k2_v / 2));
    double k3_p = h * (q + k2_q/2);
    double k3_q = h * (-df_ddy(x + h / 2, y + k2_y / 2, v + k2_v / 2) * (q + k2_q / 2)
                       - df_dy(x + h / 2, y + k2_y / 2, v + k2_v / 2) * (p + k2_p / 2));
    
    double k4_y = h * (v + k3_v);
    double k4_v = h * (-f(x + h, y + k3_y, v + k3_v));
    double k4_p = h * (q + k3_q);
    double k4_q = h * (-df_ddy(x + h, y + k3_y, v + k3_v) * (q + k3_q)
                       - df_dy(x + h, y + k3_y, v + k3_v) * (p + k3_p));
    
    y = y + (k1_y + 2 * k2_y + 2 * k3_y + k4_y) / 6.0;
    v = v + (k1_v + 2 * k2_v + 2 * k3_v + k4_v) / 6.0;
    p = p + (k1_p + 2 * k2_p + 2 * k3_p + k4_p) / 6.0;
    q = q + (k1_q + 2 * k2_q + 2 * k3_q + k4_q) / 6.0;
}

/**
 * @brief Функция решения краевой задачи методом пристрелки.
 * @return Возвращает y(1) и ∂y(1)/∂α.
 */
std::pair<double, double> solve_shooting_iteration(double alpha, int n) {
    double h = 1.0 / n;

    double x = 0.0;
    double y = 0.0;
    double v = alpha;
    double p = 0.0;
    double q = 1.0;

    for (int i = 0; i < n; ++i) {
        runge_kutta_4_system(x, h, y, v, p, q);
        x += h;
    }

    return {y, p};
}

double newton_method(double alpha0, int n, double tol = 1e-8, int max_iter = 50) {
    double alpha = alpha0;

    std::cout << "Метод Ньютона для метода пристрелки:\n";
    std::cout << "Начальное α = " << alpha << "\n";

    for (int iter = 0; iter < max_iter; ++iter) {
        auto [y1, dy1_dalpha] = solve_shooting_iteration(alpha, n);

        double residual = y1 - 2.0;

        std::cout << "Итерация " << iter + 1
                << ": α = " << alpha
                << ", y(1) = " << y1
                << ", r(α) = " << residual
                << ", ∂y/∂α = " << dy1_dalpha << "\n";

        if (std::abs(residual) < tol) {
            std::cout << "Сходимость достигнута на итерации " << iter + 1 << "\n";
            return alpha;
        }

        if (std::abs(dy1_dalpha) > 1e-12) {
            alpha = alpha - residual / dy1_dalpha;
        } else {
            std::cout << "Предупреждение: малая производная, метод может расходиться\n";
            alpha = alpha + 0.1;
        }
    }

    std::cout << "Достигнут максимум итераций\n";
    return alpha;
}

int main() {
    int n = 1000;
    double alpha0 = 2.0;
    double tol = 1e-8;

    std::cout << "===========================================\n";
    std::cout << "МЕТОД ПРИСТРЕЛКИ (по лекции)\n";
    std::cout << "Уравнение: y'' - x√y = 0\n";
    std::cout << "Граничные условия: y(0) = 0, y(1) = 2\n";
    std::cout << "===========================================\n";

    double alpha_final = newton_method(alpha0, n, tol);

    std::cout << "\n===========================================\n";
    std::cout << "РЕЗУЛЬТАТЫ:\n";
    std::cout << "Найденное y'(0) = " << alpha_final << "\n";

    std::vector<double> x_vals(n + 1);
    std::vector<double> y_vals(n + 1);
    std::vector<double> dy_vals(n + 1);

    double h = 1.0 / n;
    double x = 0.0;
    double y = 0.0;
    double v = alpha_final;
    double p = 0.0;
    double q = 1.0;

    x_vals[0] = x;
    y_vals[0] = y;
    dy_vals[0] = v;

    for (int i = 0; i < n; ++i) {
        runge_kutta_4_system(x, h, y, v, p, q);
        x += h;

        x_vals[i + 1] = x;
        y_vals[i + 1] = y;
        dy_vals[i + 1] = v;
    }

    std::cout << "y(1) = " << y_vals.back() << " (должно быть 2.0)\n";
    std::cout << "Ошибка: " << std::abs(y_vals.back() - 2.0) << "\n";

    std::ofstream file("data/results.csv");
    file << "x,y(x),y'(x)\n";
    file << std::scientific << std::setprecision(10);

    for (int i = 0; i <= n; ++i) {
        file << x_vals[i] << "," << y_vals[i] << "," << dy_vals[i] << "\n";
    }
    file.close();

    std::cout << "\nЗначения в контрольных точках:\n";
    std::cout << "x\ty(x)\n";
    std::cout << "----------------\n";

    std::vector<double> check_points = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

    for (double xc: check_points) {
        int idx = static_cast<int>(xc * n);
        if (idx >= n) idx = n - 1;

        double x1 = x_vals[idx];
        double x2 = x_vals[idx + 1];
        double y1 = y_vals[idx];
        double y2 = y_vals[idx + 1];

        double yc = y1 + (xc - x1) * (y2 - y1) / (x2 - x1);

        std::cout << std::fixed << std::setprecision(4) << xc
                << "\t" << std::setprecision(6) << yc << "\n";
    }

    return 0;
}
