#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <fstream>
#include <iomanip>

/**
 * @brief Класс для численного решения ОДУ методом Рунге-Кутты 5-го порядка (метод Дормана-Принса).
 *
 * Реализует метод с адаптивным выбором шага для контроля точности.
 * Коэффициенты соответствуют методу Дормана-Принса 5(4).
 */
class RungeKutta5 {
private:
    // Коэффициенты метода Дормана-Принса 5-го порядка
    const double с2 = 1.0 / 5.0;
    const double с3 = 3.0 / 10.0;
    const double с4 = 4.0 / 5.0;
    const double с5 = 8.0 / 9.0;
    const double с6 = 1.0;

    // Коэффициенты a_{ij} для вычисления промежуточных значений y
    const double a21 = 1.0 / 5.0;
    const double a31 = 3.0 / 40.0, a32 = 9.0 / 40.0;
    const double a41 = 44.0 / 45.0, a42 = -56.0 / 15.0, a43 = 32.0 / 9.0;
    const double a51 = 19372.0 / 6561.0, a52 = -25360.0 / 2187.0, a53 = 64448.0 / 6561.0, a54 = -212.0 / 729.0;
    const double a61 = 9017.0 / 3168.0, a62 = -355.0 / 33.0, a63 = 46732.0 / 5247.0, a64 = 49.0 / 176.0,
            a65 = -5103.0 / 18656.0;

    // Коэффициенты для решения 5-го порядка
    const double b1 = 35.0 / 384.0;
    const double b2 = 0.;
    const double b3 = 500.0 / 1113.0;
    const double b4 = 125.0 / 192.0;
    const double b5 = -2187.0 / 6784.0;
    const double b6 = 11.0 / 84.0;

    // Коэффициенты для решения 4-го порядка (b*_i в таблице Бутчера)
    // Используются для оценки ошибки
    const double b_1 = 5179.0 / 57600.0;
    const double b_2 = 0.0;
    const double b_3 = 7571.0 / 16695.0;
    const double b_4 = 393.0 / 640.0;
    const double b_5 = -92097.0 / 339200.0;
    const double b_6 = 187.0 / 2100.0;
    const double b_7 = 1.0 / 40.0;

public:
    /**
     * @brief Решает ОДУ первого порядка методом Рунге-Кутты 5-го порядка с адаптивным шагом.
     *
     * @param f Функция правой части уравнения dy/dx = f(x, y).
     * @param x0 Начальное значение x.
     * @param y0 Начальное значение y(x0).
     * @param x_end Конечное значение x.
     * @param h Начальный шаг интегрирования.
     * @param tol Допустимая погрешность на шаг (по умолчанию 1e-6).
     * @param h_min Минимально допустимый шаг (по умолчанию 1e-6).
     * @param h_max Максимально допустимый шаг (по умолчанию 0.1).
     * @return Пару векторов: значения x и соответствующие значения y.
     *
     * Алгоритм:
     * - Использует метод Дормана-Принса 5(4) с встроенной оценкой ошибки.
     * - На каждом шаге вычисляет решение 5-го и 4-го порядков.
     * - Оценивает локальную погрешность как разность между этими решениями.
     * - Регулирует шаг интегрирования для поддержания заданной точности.
     * - Если погрешность превышает tol, шаг уменьшается.
     * - Если погрешность значительно меньше tol, шаг увеличивается.
     *
     * Пример использования:
     * @code
     * auto f = [](double x, double y) { return -2.0 * y; };
     * RungeKutta5 rk5;
     * auto [x_vals, y_vals] = rk5.solve(f, 0.0, 1.0, 10.0, 0.1, 1e-8);
     * @endcode
     */
    std::pair<std::vector<double>, std::vector<double> > solve(
        std::function<double(double, double)> f,
        double x0, double y0,
        double x_end, double h,
        double tol = 1e-6,
        double h_min = 1e-6,
        double h_max = 0.1) {
        std::vector<double> x_values = {x0};
        std::vector<double> y_values = {y0};

        double x = x0;
        double y = y0;
        double h_cur = h;

        int steps = 0;

        while (x < x_end) {
            steps++;

            if (x + h_cur > x_end) {
                h_cur = x_end - x;
            }

            double k1 = h_cur * f(x, y);
            double k2 = h_cur * f(x + this->с2 * h_cur, y + this->a21 * k1);
            double k3 = h_cur * f(x + this->с3 * h_cur, y + this->a31 * k1 + this->a32 * k2);
            double k4 = h_cur * f(x + this->с4 * h_cur, y + this->a41 * k1 + this->a42 * k2 + this->a43 * k3);
            double k5 = h_cur * f(x + this->с5 * h_cur,
                                  y + this->a51 * k1 + this->a52 * k2 + this->a53 * k3 + this->a54 * k4);
            double k6 = h_cur * f(x + this->с6 * h_cur,
                                  y + this->a61 * k1 + this->a62 * k2 + this->a63 * k3 + this->a64 * k4 + this->a65 *
                                  k5);

            double y_next_5 = y + this->b1 * k1 + this->b3 * k3 + this->b4 * k4 + this->b5 * k5 + this->b6 * k6;

            double k7 = h_cur * f(x + h_cur, y_next_5);
            double y_next_4 = y + this->b_1 * k1 + this->b_2 * k3 + this->b_3 * k4 + this->b_4 * k5 + this->b_5 * k6 +
                              this->b_6 * k7;

            double delta = std::abs(y_next_5 - y_next_4);

            if (delta <= tol || h_cur <= h_min) {
                x += h_cur;
                y = y_next_5;

                x_values.push_back(x);
                y_values.push_back(y);

                if (delta > 0) {
                    double scale = 0.9 * std::pow(tol / delta, 0.2);
                    scale = std::max(0.1, std::min(5.0, scale));
                    h_cur *= scale;
                    h_cur = std::max(h_min, std::min(h_max, h_cur));
                }
            } else {
                double scale = 0.9 * std::pow(tol / delta, 0.25);
                scale = std::max(0.1, std::min(0.9, scale));
                h_cur *= scale;
            }
        }

        return {x_values, y_values};
    }

    /**
     * @brief Решает ОДУ методом Рунге-Кутты 5-го порядка с фиксированным шагом.
     *
     * @param f Функция правой части уравнения dy/dx = f(x, y).
     * @param x0 Начальное значение x.
     * @param y0 Начальное значение y(x0).
     * @param x_end Конечное значение x.
     * @param h Фиксированный шаг интегрирования.
     * @return Пару векторов: значения x и соответствующие значения y.
     *
     * Алгоритм:
     * - Использует метод Дормана-Принса 5-го порядка.
     * - Шаг интегрирования постоянный.
     * - Подходит для сравнения с адаптивным методом.
     * - Менее эффективен при резких изменениях решения.
    */
    std::pair<std::vector<double>, std::vector<double> > solve_fix(
        std::function<double(double, double)> f,
        double x0, double y0,
        double x_end, double h) {
        std::vector<double> x_values = {x0};
        std::vector<double> y_values = {y0};

        double x = x0;
        double y = y0;
        int steps = 0;

        while (x < x_end) {
            steps++;

            if (x + h > x_end) {
                h = x_end - x;
            }

            double k1 = h * f(x, y);
            double k2 = h * f(x + this->с2 * h, y + this->a21 * k1);
            double k3 = h * f(x + this->с3 * h, y + this->a31 * k1 + this->a32 * k2);
            double k4 = h * f(x + this->с4 * h, y + this->a41 * k1 + this->a42 * k2 + this->a43 * k3);
            double k5 = h * f(x + this->с5 * h, y + this->a51 * k1 + this->a52 * k2 + this->a53 * k3 + this->a54 * k4);
            double k6 = h * f(x + this->с6 * h,
                              y + this->a61 * k1 + this->a62 * k2 + this->a63 * k3 + this->a64 * k4 + this->a65 * k5);

            y += this->b1 * k1 + this->b3 * k3 + this->b4 * k4 + this->b5 * k5 + this->b6 * k6;
            x += h;

            x_values.push_back(x);
            y_values.push_back(y);
        }

        return {x_values, y_values};
    }
};

/**
 * @brief Точное аналитическое решение уравнения y' = -2y.
 *
 * @param x Аргумент функции.
 * @return Значение функции в точке x: y(x) = -2 * exp(-2x).
 *
 * Уравнение: dy/dx = -2y, y(0) = -2.
 * Решение: y(x) = -2 * exp(-2x).
*/
double true_value(double x) {
    return -2.0 * exp(-2.0 * x);
}

/**
 * @brief Основная функция программы.
 *
 * Программа решает дифференциальное уравнение y' = -2y с начальным условием y(0) = -2
 * на интервале [0, 100] методом Рунге-Кутты 5-го порядка.
 *
 * Алгоритм работы:
 * 1. Создает объект RungeKutta5.
 * 2. Определяет функцию правой части уравнения.
 * 3. Решает уравнение двумя способами:
 *    - С адаптивным выбором шага.
 *    - С фиксированным шагом для сравнения.
 * 4. Выводит результаты и сравнивает с точным решением.
 * 5. Сохраняет результаты в файл solution.csv.
 */
int main() {
    RungeKutta5 rk5;

    auto f = [](double x, double y) -> double {
        return -2.0 * y;
    };

    double x0 = 0.;
    double y0 = -2.;
    double x_end = 100.;

    std::cout << "===========================================\n";
    std::cout << "Решение уравнения: y' = -2y\n";
    std::cout << "Начальное условие: y(0) = " << y0 << "\n";
    std::cout << "Интервал: [0, " << x_end << "]\n";
    std::cout << "Точное решение: y(x) = " << y0 << " * exp(-2x)\n";
    std::cout << "===========================================\n";

    auto [x_adaptive, y_adaptive] = rk5.solve(f, x0, y0, x_end, 0.1, 1e-8);

    auto [x_fixed, y_fixed] = rk5.solve_fix(f, x0, y0, x_end, 0.01);

    std::ofstream file("solution.csv");

    file << "x,Адаптивный шаг,Фиксированный шаг,Точное решение,Ошибка адапт.,Ошибка фикс.\n";
    file << std::scientific << std::setprecision(10);

    // Сравниваем результаты в конечной точке
    std::cout << "Сравнение результатов в конечной точке x = " << x_end << ":\n";

    double y_adaptive_end = y_adaptive.back();
    double y_fixed_end = y_fixed.back();
    double y_exact_end = true_value(x_end);

    std::cout << "Адаптивный шаг:  y(" << x_end << ") = " << y_adaptive_end << "\n";
    std::cout << "Фиксированный шаг: y(" << x_end << ") = " << y_fixed_end << "\n";
    std::cout << "Точное решение:  y(" << x_end << ") = " << y_exact_end << "\n";
    std::cout << "Ошибка адаптивного шага:  " << std::abs(y_adaptive_end - y_exact_end) << "\n";
    std::cout << "Ошибка фиксированного шага: " << std::abs(y_fixed_end - y_exact_end) << "\n";

    size_t adaptive_size = x_adaptive.size();
    size_t fixed_size = x_fixed.size();

    size_t sample_step_adaptive = std::max<size_t>(1, adaptive_size / 1000);
    size_t sample_step_fixed = std::max<size_t>(1, fixed_size / 1000);

    for (size_t i = 0; i < adaptive_size; i += sample_step_adaptive) {
        double x = x_adaptive[i];
        double y_adapt = y_adaptive[i];
        double y_exact = true_value(x);

        double y_fixed_val = 0;
        for (size_t j = 0; j < fixed_size; j++) {
            if (std::abs(x_fixed[j] - x) < 1e-6) {
                y_fixed_val = y_fixed[j];
                break;
            }
        }

        file << x << "," << y_adapt << "," << y_fixed_val << "," << y_exact << "," << std::abs(y_adapt - y_exact)
                << "," << std::abs(y_fixed_val - y_exact) << "\n";
    }

    double x_last = x_adaptive.back();
    double y_adapt_last = y_adaptive.back();
    double y_fixed_last = y_fixed.back();
    double y_exact_last = true_value(x_last);

    file << x_last << "," << y_adapt_last << "," << y_fixed_last << ","
            << y_exact_last << "," << std::abs(y_adapt_last - y_exact_last)
            << "," << std::abs(y_fixed_last - y_exact_last) << "\n";

    file.close();

    std::cout << "\nПроверка в нескольких точках:\n";
    std::vector<double> check_points = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0};

    for (double x_check: check_points) {
        double y_numeric = y_adaptive[0];
        for (size_t i = 0; i < x_adaptive.size(); i++) {
            if (std::abs(x_adaptive[i] - x_check) < 1e-6) {
                y_numeric = y_adaptive[i];
                break;
            }
            if (x_adaptive[i] > x_check && i > 0) {
                double x1 = x_adaptive[i - 1], y1 = y_adaptive[i - 1];
                double x2 = x_adaptive[i], y2 = y_adaptive[i];
                double t = (x_check - x1) / (x2 - x1);
                y_numeric = y1 + t * (y2 - y1);
                break;
            }
        }

        double y_exact = true_value(x_check);
        double error = std::abs(y_numeric - y_exact);

        std::cout << "x = " << x_check << ": численное = " << std::scientific <<
                std::setprecision(6) << y_numeric << ", точное = " << y_exact << ", ошибка = " << error;

        if (y_exact != 0) {
            std::cout << ", относит. ошибка = " << error / std::abs(y_exact);
        }
        std::cout << std::endl;
    }

    return 0;
}
