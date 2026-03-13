#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <algorithm>

/**
 * @brief Простой явный метод Эйлера с очень маленьким шагом
 * Для тестирования на коротком интервале
 */
void euler_step(double t, double &x1, double &x2, double h, double mu) {
    double dx1_dt = x2;
    double dx2_dt = mu * (1.0 - x2 * x2) * x2 - x1;

    x1 += h * dx1_dt;
    x2 += h * dx2_dt;
}

/**
 * @brief Метод Рунге-Кутты 4-го порядка
 */
void rk4_step(double t, double &x1, double &x2, double h, double mu) {
    double k1_x1 = h * x2;
    double k1_x2 = h * (mu * (1.0 - x2 * x2) * x2 - x1);

    double k2_x1 = h * (x2 + k1_x2 / 2.0);
    double k2_x2 = h * (mu * (1.0 - (x2 + k1_x2 / 2.0) * (x2 + k1_x2 / 2.0)) * (x2 + k1_x2 / 2.0) - (x1 + k1_x1 / 2.0));

    double k3_x1 = h * (x2 + k2_x2 / 2.0);
    double k3_x2 = h * (mu * (1.0 - (x2 + k2_x2 / 2.0) * (x2 + k2_x2 / 2.0)) * (x2 + k2_x2 / 2.0) - (x1 + k2_x1 / 2.0));

    double k4_x1 = h * (x2 + k3_x2);
    double k4_x2 = h * (mu * (1.0 - (x2 + k3_x2) * (x2 + k3_x2)) * (x2 + k3_x2) - (x1 + k3_x1));

    x1 += (k1_x1 + 2.0 * k2_x1 + 2.0 * k3_x1 + k4_x1) / 6.0;
    x2 += (k1_x2 + 2.0 * k2_x2 + 2.0 * k3_x2 + k4_x2) / 6.0;
}

int main() {
    double mu = 1000.0;
    double t0 = 0.0;
    double t_end = 1000.0;
    double x1 = 0.0; // x(0)
    double x2 = 0.001; // x'(0)

    double h = 0.00001;

    std::cout << "===========================================\n";
    std::cout << "РЕШЕНИЕ УРАВНЕНИЯ РЕЛЕЯ (μ = " << mu << ")\n";
    std::cout << "===========================================\n";
    std::cout << "x(0) = " << x1 << "\n";
    std::cout << "x'(0) = " << x2 << "\n";
    std::cout << "Время: [" << t0 << ", " << t_end << "]\n";
    std::cout << "Шаг: " << h << " (очень маленький из-за μ=" << mu << ")\n";
    std::cout << "Метод: Рунге-Кутты 4-го порядка\n";
    std::cout << "===========================================\n\n";

    std::vector<double> t_values;
    std::vector<double> x1_values;
    std::vector<double> x2_values;

    double t = t0;
    std::cout << "Тестовое интегрирование на [0, 0.1]...\n";

    double x1_test = x1;
    double x2_test = x2;
    double t_test = t0;

    int test_steps = 0;
    while (t_test < 0.1) {
        rk4_step(t_test, x1_test, x2_test, h, mu);
        t_test += h;
        test_steps++;

        if (test_steps % 10000 == 0) {
            std::cout << "t = " << t_test << ", x = " << x1_test
                    << ", x' = " << x2_test << "\n";
        }
    }

    std::cout << "\nПосле 0.1 секунд:\n";
    std::cout << "x(0.1) = " << x1_test << "\n";
    std::cout << "x'(0.1) = " << x2_test << "\n";
    std::cout << "Количество шагов: " << test_steps << "\n";

    if (std::abs(x1_test) > 1e-6) {
        std::cout << "Начинаем полное интегрирование до t = " << t_end << "...\n";

        t_values.push_back(t);
        x1_values.push_back(x1);
        x2_values.push_back(x2);

        long long total_steps = 0;
        long long save_interval = 10000;

        while (t < t_end) {
            if (t + h > t_end) {
                h = t_end - t;
            }

            rk4_step(t, x1, x2, h, mu);
            t += h;
            total_steps++;

            if (total_steps % save_interval == 0) {
                t_values.push_back(t);
                x1_values.push_back(x1);
                x2_values.push_back(x2);
            }

            if (total_steps % 1000000 == 0) {
                std::cout << "t = " << t << ", x = " << x1
                        << ", x' = " << x2
                        << ", шагов: " << total_steps << "\n";
            }
        }

        t_values.push_back(t);
        x1_values.push_back(x1);
        x2_values.push_back(x2);

        std::cout << "\nИнтегрирование завершено!\n";
        std::cout << "Всего шагов: " << total_steps << "\n";
        std::cout << "Сохранено точек: " << t_values.size() << "\n";
        std::cout << "x(" << t << ") = " << x1 << "\n";
        std::cout << "x'(" << t << ") = " << x2 << "\n";
    }

    std::cout << "\n===========================================\n";
    std::cout << "АНАЛИЗ РЕШЕНИЯ:\n";
    std::cout << "===========================================\n";

    if (!x1_values.empty()) {
        double x_max = *std::max_element(x1_values.begin(), x1_values.end());
        double x_min = *std::min_element(x1_values.begin(), x1_values.end());
        double dx_max = *std::max_element(x2_values.begin(), x2_values.end());
        double dx_min = *std::min_element(x2_values.begin(), x2_values.end());

        std::cout << "Количество точек: " << x1_values.size() << "\n";
        std::cout << "Максимальное x: " << std::scientific << x_max << "\n";
        std::cout << "Минимальное x: " << x_min << "\n";
        std::cout << "Амплитуда колебаний: " << (x_max - x_min) << "\n";
        std::cout << "Максимальная скорость: " << dx_max << "\n";
        std::cout << "Минимальная скорость: " << dx_min << "\n";
    }

    std::ofstream file("data/results.csv");
    file << "t,x(t),dx/dt\n";
    file << std::scientific << std::setprecision(10);

    for (size_t i = 0; i < t_values.size(); ++i) {
        file << t_values[i] << "," << x1_values[i] << "," << x2_values[i] << "\n";
    }

    file.close();

    std::cout << "\n===========================================\n";
    std::cout << "Результаты сохранены в results.csv\n";
    std::cout << "Количество точек: " << t_values.size() << "\n";

    std::cout << "\nКонтрольные точки:\n";
    std::cout << "t\t\tx(t)\t\t\tx'(t)\n";
    std::cout << "------------------------------------------------\n";

    std::vector<double> check_times = {0.0, 1.0, 5.0, 10.0, 50.0, 100.0, 500.0, 1000.0};

    for (double tc: check_times) {
        double x_val = 0, dx_val = 0;

        for (size_t i = 1; i < t_values.size(); ++i) {
            if (t_values[i] >= tc) {
                double alpha = (tc - t_values[i - 1]) / (t_values[i] - t_values[i - 1]);
                x_val = x1_values[i - 1] + alpha * (x1_values[i] - x1_values[i - 1]);
                dx_val = x2_values[i - 1] + alpha * (x2_values[i] - x2_values[i - 1]);
                break;
            }
        }

        std::cout << std::fixed << std::setprecision(1) << tc << "\t\t"
                << std::scientific << std::setprecision(6)
                << x_val << "\t\t" << dx_val << "\n";
    }

    return 0;
}
