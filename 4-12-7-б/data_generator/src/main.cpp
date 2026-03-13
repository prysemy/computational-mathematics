#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>

/**
 * @brief Требуемая точность вычислений.
 * 
 * Используется как критерий остановки в итерационных методах.
 * Итерации прекращаются при |x_new - x| < EPS.
*/
constexpr double EPS = 1e-3;

/**
 * @brief Исходная функция f(x) = x * exp(-x^2).
 *
 * @param x аргумент функции.
 * 
 * Возвращает значение функции в точке x.
 * Функция имеет максимум при x = 1/sqrt(2).
*/
double f(const double x) {
    return x * exp(-x * x);
}

/**
 * @brief Функция метода простой итерации для левой ветви функции.
 *
 * @param t целевое значение функции (полувысота).
 * @param x0 начальное приближение.
 * @param iterations счетчик итераций (выходной параметр).
 * 
 * Решает уравнение x * exp(-x^2) = t для x < x_max.
 * Использует итерационную формулу: x = t * exp(x^2).
 * Возвращает найденный корень и количество итераций.
*/
double simple_iteration_left(double t, double x0, int &iterarions) {
    // x < t
    double x = x0;
    iterarions = 0;
    for (int i = 0; i < 1000; i++) {
        iterarions++;
        double x_new = t * exp(x * x);
        if (fabs(x_new - x) < EPS) return x_new;
        x = x_new;
    }
    return x;
}

/**
 * @brief Функция метода простой итерации для правой ветви функции.
 *
 * @param t целевое значение функции (полувысота).
 * @param x0 начальное приближение.
 * @param iterations счетчик итераций (выходной параметр).
 * 
 * Решает уравнение x * exp(-x^2) = t для x > x_max.
 * Использует итерационную формулу: x = √sqrtln(x/t)).
 * Возвращает найденный корень и количество итераций.
*/
double simple_iteration_right(double t, double x0, int &iterations) {
    // x > t
    double x = x0;
    iterations = 0;
    for (int i = 0; i < 1000; i++) {
        iterations++;
        double x_new = sqrt(log(x / t));
        if (fabs(x_new - x) < EPS) return x_new;
        x = x_new;
    }
    return x;
}

/**
 * @brief Сохранение результатов вычислений в файл.
 *
 * @param x_left левая точка полувысоты.
 * @param x_right правая точка полувысоты.
 * @param x_max точка максимума функции.
 * @param f_max значение максимума функции.
 * @param target значение полувысоты.
 * @param fwhm ширина на полувысоте.
 * @param iter1 количество итераций для левой точки.
 * @param iter2 количество итераций для правой точки.
 * 
 * Создает файл с подробными результатами вычислений
 * и данными для построения графика функции.
*/
void save(double x_left, double x_right, double x_max, double f_max, double target, double fwhm, int iter1, int iter2) {
    std::ofstream file("data/fwhm_results.txt");
    file << std::fixed << std::setprecision(6);

    file << "РЕЗУЛЬТАТЫ ВЫЧИСЛЕНИЯ ШИРИНЫ НА ПОЛУВЫСОТЕ" << "\n";
    file << "===========================================" << "\n";
    file << "Функция: f(x) = x * exp(-x^2), x ≥ 0" << "\n" << "\n";

    file << "ОСНОВНЫЕ ПАРАМЕТРЫ:" << "\n";
    file << "Максимум функции:" << "\n";
    file << "x_max = " << x_max << "\n";
    file << "f_max = " << f_max << "\n";
    file << "Полувысота: " << target << "\n" << "\n";

    file << "ТОЧКИ ПОЛУВЫСОТЫ:" << "\n";
    file << "Левая точка: x_left = " << x_left << "\n";
    file << "f(x_left) = " << f(x_left) << "\n";
    file << "Количество итераций: " << iter1 << "\n";
    file << "Правая точка: x_right = " << x_right << "\n";
    file << "f(x_right) = " << f(x_right) << "\n";
    file << "Количество итераций: " << iter2 << "\n\n";

    file << "РЕЗУЛЬТАТ:" << "\n";
    file << "Ширина на полувысоте (FWHM) = " << fwhm << "\n" << "\n";

    file << "ДАННЫЕ ДЛЯ ГРАФИКА:" << "\n";
    file << "x\tf(x)" << "\n";

    for (double x = 0; x <= 2.0; x += 0.01) {
        file << x << "\t" << f(x) << "\n";
    }
    file.close();
}

int main() {
    std::cout << "ВЫЧИСЛЕНИЕ ШИРИНЫ НА ПОЛУВЫСОТЕ МЕТОДОМ ПРОСТОЙ ИТЕРАЦИИ\n";
    std::cout << "==========================================================\n\n";

    double x_max = 1.0 / sqrt(2.0);
    double f_max = f(x_max);
    double t = f_max / 2.0;

    int iter1, iter2;
    double x_left = simple_iteration_left(t, x_max - 0.2, iter1);
    double x_right = simple_iteration_right(t, x_max + 0.4, iter2);

    double fwhm = x_right - x_left;

    std::cout << "ШИРИНА ФУНКЦИИ НА ПОЛУВЫСОТЕ (FWHM)\n";
    std::cout << "f(x) = x ⋅ exp(-x^2), x ≥ 0\n\n";

    std::cout << "ОСНОВНЫЕ ПАРАМЕТРЫ:\n";
    std::cout << "Максимум функции:\n";
    std::cout << "x_max = 1/√2 = " << x_max << "\n";
    std::cout << "f_max = " << f_max << "\n";
    std::cout << "Полувысота: " << t << "\n\n";

    std::cout << "ТОЧКИ ПОЛУВЫСОТЫ:" << "\n";
    std::cout << "Левая точка: x1 = " << x_left << "\n";
    std::cout << "f(x1) = " << f(x_left) << "\n";
    std::cout << "Количество итераций: " << iter1 << "\n";
    std::cout << "Правая точка: x2 = " << x_right << "\n";
    std::cout << "f(x2) = " << f(x_right) << "\n";
    std::cout << "Количество итераций: " << iter2 << "\n\n";

    std::cout << "РЕЗУЛЬТАТ:\n";
    std::cout << "Ширина на полувысоте: FWHM = x2 - x1 = " << fwhm << "\n\n";

    std::cout << "ПРОВЕРКА ТОЧНОСТИ:\n";
    std::cout << "|f(x1) - t| = " << fabs(f(x_left) - t) << "\n";
    std::cout << "|f(x2) - t| = " << fabs(f(x_right) - t) << "\n";
    std::cout << "Требуемая точность: " << EPS << "\n";

    save(x_left, x_right, x_max, f_max, t, fwhm, iter1, iter2);

    return 0;
}
