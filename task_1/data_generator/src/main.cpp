#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

/**
 * @brief Функция верхней полуокружности.
*/
double circle(double x) {
    return sqrt(1 - x * x);
}

/**
 * @brief Функция нижней полуокружности.
*/
double circle_neg(double x) {
    return -sqrt(1 - x * x);
}

/**
 * @brief Функция производной от тангенса.
*/
double derivative(double x) {
    return 1.0 / (cos(x) * cos(x));
}

/**
 * @brief Функция метод Ньютона.
 * @param x0 начальное приближение по x.
 * @param epsilon требуемая точность.
 * @param max_iterations максимальное число итераций.
 *
 * Решает систему методом Ньютона с матрицей Якоби.
 * Возвращает уточненное значение x.
*/
double newton_method(const double x0, const double epsilon, const int max_iterations) {
    double x = x0;

    for (int i = 0; i < max_iterations; i++) {
        double f = x * x + tan(x) * tan(x) - 1; // F(x) = x² + tg²(x) - 1
        double df = 2 * x + 2 * tan(x) / (cos(x) * cos(x)); // F'(x)

        if (fabs(df) < 1e-12) {
            break;
        }

        double x_new = x - f / df;

        if (fabs(x_new - x) < epsilon) {
            return x_new;
        }

        x = x_new;
    }

    return x;
}

int main() {
    std::ofstream outfile("data/results.txt");
    outfile << std::fixed << std::setprecision(8);

    double x_min = -2.0;
    double x_max = 2.0;
    int points = 1000;
    double step = (x_max - x_min) / points;

    std::vector<double> roots;

    outfile << "ГРАФИЧЕСКИЙ АНАЛИЗ СИСТЕМЫ УРАВНЕНИЙ\n";
    outfile << "x^2 + y^2 = 1\n";
    outfile << "y = tg(x)\n";
    outfile << "=====================================\n";

    outfile << "ДАННЫЕ ДЛЯ ГРАФИКА:\n";
    outfile << "x\tcircle_upper\tcircle_lower\ttan\n";

    for (int i = 0; i <= points; i++) {
        double x = x_min + i * step;
        double tan_val;
        if (fabs(cos(x)) < 1e-10) {
            tan_val = NAN;
        } else {
            tan_val = tan(x);
        }

        double circle_up = (fabs(x) <= 1.0) ? circle(x) : NAN;
        double circle_low = (fabs(x) <= 1.0) ? circle_neg(x) : NAN;

        outfile << x << "\t" << circle_up << "\t" << circle_low << "\t" << tan_val << std::endl;
    }

    outfile << std::endl;

    outfile << "ПРИБЛИЖЕННЫЕ КОРНИ:" << std::endl;

    std::vector<double> initial_guesses = {-1.2, -0.6, 0.0, 0.6, 1.2};

    for (double guess: initial_guesses) {
        double epsilon = 1e-6;
        double root = newton_method(guess, epsilon, 100);
        double y_root = tan(root);

        // Проверяем, что точка лежит на окружности
        if (fabs(root * root + y_root * y_root - 1) < epsilon) {
            bool is_duplicate = false;
            for (double existing_root: roots) {
                if (fabs(root - existing_root) < 0.1) {
                    is_duplicate = true;
                    break;
                }
            }

            if (!is_duplicate) {
                roots.push_back(root);
                outfile << "Корень: x = " << root << ", y = " << y_root << std::endl;
                outfile << "Проверка: x^2 + y^2 = " << root * root + y_root * y_root << std::endl;
            }
        }
    }

    outfile << std::endl;
    outfile << "ВСЕ НАЙДЕННЫЕ КОРНИ:" << std::endl;
    for (size_t i = 0; i < roots.size(); i++) {
        double y = tan(roots[i]);
        outfile << "Корень " << i + 1 << ": (" << roots[i] << ", " << y << ")" << std::endl;
    }

    outfile.close();
    return 0;
}
