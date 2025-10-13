#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iomanip>

/**
 * @brief Подынтегральная функция f(x) = sin(100x) * exp(-x^2) * cos(2x).
 *
 * @param x Точка, в которой вычисляется значение функции.
 * @return f(x) Значение функции в точке x.
 */
double f(double const x) {
    return sin(100.0 * x) * exp(-x * x) * cos(2.0 * x);
}

/**
 * @brief Функция метода средних прямоугольников.
 *
 * @param a Нижний предел интегрирования.
 * @param b Верхний предел интегрирования.
 * @param n Количество разбиений отрезка [a,b].
 * @return Приближенное значение интеграла.
 *
 * Алгоритм:
 * - Разбивает отрезок [a,b] на n равных частей.
 * - В каждом подинтервале вычисляет значение функции в средней точке.
 * - Суммирует площади прямоугольников.
 *
 * h - шаг разбиения, h = (b - a) / n.
 * sum - накопленная сумма площадей прямоугольников.
 * x - середина текущего подинтервала.
 */
double rectangle_method(double const a, double const b, int const n) {
    double h = (b - a) / n;
    double sum = 0.0;

    for (int i = 0; i < n; i++) {
        double x = a + (i + 0.5) * h;
        sum += f(x);
    }

    return sum * h;
}

/**
 * @brief Функция метода трапеций.
 *
 * @param a Нижний предел интегрирования.
 * @param b Верхний предел интегрирования.
 * @param n Количество разбиений отрезка [a,b].
 * @return Приближенное значение интеграла.
 *
 * Алгоритм:
 * - Разбивает отрезок [a,b] на n равных частей.
 * - В каждом подинтервале аппроксимирует функцию линейно.
 * - Суммирует площади трапеций.
 *
 * h - шаг разбиения.
 * sum - сумма с весами (начало и конец с весом 0.5, остальные с весом 1).
 * x - текущая точка на отрезке интегрирования.
 */
double trapezoidal_method(double const a, double const b, int const n) {
    double h = (b - a) / n;
    double sum = 0.5 * (f(a) + f(b));

    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }

    return sum * h;
}

/**
 * @brief Функция метода Симпсона.
 *
 * @param a Нижний предел интегрирования.
 * @param b Верхний предел интегрирования.
 * @param n Количество разбиений отрезка [a,b].
 * @return Приближенное значение интеграла.
 *
 * Алгоритм:
 * - Разбивает отрезок [a,b] на n равных частей (n четное).
 * - На каждой паре подинтервалов аппроксимирует функцию параболой.
 * - Использует веса 1-4-2-4-2-...-4-1.
 *
 * h - шаг разбиения.
 * sum - сумма с весами Симпсона.
 * x - текущая точка разбиения.
 */
double simpson_method(double const a, double const b, int n) {
    if (n % 2 != 0) n++;
    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        if (i % 2 == 0) {
            sum += 2.0 * f(x);
        } else {
            sum += 4.0 * f(x);
        }
    }
    return sum * h / 3.0;
}

/**
 * @brief Функция правила 3/8.
 *
 * @param a Нижний предел интегрирования.
 * @param b Верхний предел интегрирования.
 * @param n Количество разбиений отрезка [a,b].
 * @return Приближенное значение интеграла.
 *
 * Алгоритм:
 * - Разбивает отрезок [a,b] на n равных частей (n кратно 3).
 * - На каждой тройке подинтервалов использует кубическую аппроксимацию.
 * - Использует веса 1-3-3-2-3-3-2-...-3-3-1.
 *
 * h - шаг разбиения
 * sum - сумма с весами правила 3/8
 * x - текущая точка разбиения
 */
double three_eights_method(double const a, double const b, int n) {
    if (n % 3 != 0) n += (3 - n % 3);
    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        if (i % 3 == 0) {
            sum += 2.0 * f(x);
        } else {
            sum += 3.0 * f(x);
        }
    }
    return sum * h * 3.0 / 8.0;
}

/**
 * @brief Функция квадратуры Гаусса с 2 узлами (степень 3).
 *
 * @param a Нижний предел интегрирования.
 * @param b Верхний предел интегрирования.
 * @return Приближенное значение интеграла.
 *
 * Алгоритм:
 * - Использует 2 оптимально расположенных узла на [-1,1].
 * - Линейное преобразование на отрезок [a,b].
 *
 * nodes[2] - узлы на стандартном отрезке [-1,1].
 * weights[2] - веса квадратурной формулы.
 * scale - масштабный коэффициент для преобразования отрезка.
 * shift - сдвиг для преобразования отрезка.
 * x - преобразованная точка на [a,b].
 */
double gauss_2_nodes(double const a, double const b) {
    const double nodes[2] = {-1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};
    const double weights[2] = {1.0, 1.0};

    double sum = 0.0;
    double scale = (b - a) / 2.0;
    double shift = (a + b) / 2.0;

    for (int i = 0; i < 2; i++) {
        double x = shift + scale * nodes[i];
        sum += weights[i] * f(x);
    }
    return sum * scale;
}

/**
 * @brief Функция квадратуры Гаусса с 3 узлами (степень 5).
 *
 * @param a Нижний предел интегрирования.
 * @param b Верхний предел интегрирования.
 * @return Приближенное значение интеграла.
 *
 * Алгоритм:
 * - Использует 3 оптимально расположенных узла на [-1,1].
 * - Узлы: корни многочлена Лежандра 3-й степени.
 *
 * nodes[3] - узлы Гаусса на [-1,1].
 * weights[3] - соответствующие веса.
 * scale, shift - параметры преобразования отрезка.
 */
double gauss_3_nodes(double const a, double const b) {
    const double nodes[3] = {-sqrt(3.0 / 5.0), 0.0, sqrt(3.0 / 5.0)};
    const double weights[3] = {5.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0};

    double sum = 0.0;
    double scale = (b - a) / 2.0;
    double shift = (a + b) / 2.0;

    for (int i = 0; i < 3; i++) {
        double x = shift + scale * nodes[i];
        sum += weights[i] * f(x);
    }
    return sum * scale;
}

/**
 * @brief Функция квадратуры Гаусса с 4 узлами (степень 7).
 *
 * @param a Нижний предел интегрирования.
 * @param b Верхний предел интегрирования.
 * @return Приближенное значение интеграла.
 *
 * Алгоритм:
 * - Использует 4 оптимально расположенных узла на [-1,1].
 * - Узлы: корни многочлена Лежандра 4-й степени.
 *
 * nodes[4] - узлы Гаусса 4-го порядка.
 * weights[4] - соответствующие веса.
 * scale, shift - параметры линейного преобразования.
 */
double gauss_4_nodes(double const a, double const b) {
    const double nodes[4] = {
        -sqrt(3.0 / 7.0 + 2.0 / 7.0 * sqrt(6.0 / 5.0)),
        -sqrt(3.0 / 7.0 - 2.0 / 7.0 * sqrt(6.0 / 5.0)),
        sqrt(3.0 / 7.0 - 2.0 / 7.0 * sqrt(6.0 / 5.0)),
        sqrt(3.0 / 7.0 + 2.0 / 7.0 * sqrt(6.0 / 5.0))
    };
    const double weights[4] = {
        (18.0 - sqrt(30.0)) / 36.0,
        (18.0 + sqrt(30.0)) / 36.0,
        (18.0 + sqrt(30.0)) / 36.0,
        (18.0 - sqrt(30.0)) / 36.0
    };

    double sum = 0.0;
    double scale = (b - a) / 2.0;
    double shift = (a + b) / 2.0;

    for (int i = 0; i < 4; i++) {
        double x = shift + scale * nodes[i];
        sum += weights[i] * f(x);
    }
    return sum * scale;
}

/**
 * @brief Функция правила Рунге для оценки погрешности
 *
 * @param I_h Значение интеграла на сетке с шагом h.
 * @param I_h2 Значение интеграла на сетке с шагом h/2.
 * @param p Порядок точности метода.
 * @return Оценка погрешности по правилу Рунге.
 *
 * Формула:
 * e = |I_h - I_h2| / (2^p - 1).
 *
 * где:
 * I_h - приближение на грубой сетке.
 * I_h2 - приближение на мелкой сетке.
 * p - порядок метода (для Симпсона p=4, для трапеций p=2, etc.).
 * e - оценка погрешности приближения I_h2.
 */
double runge_error(const double I_h, const double I_h2, const int p) {
    return fabs(I_h - I_h2) / (pow(2, p) - 1);
}

int main() {
    std::cout << "ВЫЧИСЛЕНИЕ ИНТЕГРАЛА БЫСТРООСЦИЛЛИРУЮЩЕЙ ФУНКЦИИ\n";
    std::cout << "I = int_0^3 sin(100x) * exp(-x²) * cos(2x) dx\n";
    std::cout << "=============================================\n\n";

    const double a = 0.0, b = 3.0;
    const int n_base = 100000; // Базовое количество разбиений

    // Сохранение данных для графика
    std::ofstream file("data/results.txt");
    file << std::fixed << std::setprecision(12);

    file << "ИНТЕГРАЛ БЫСТРООСЦИЛЛИРУЮЩЕЙ ФУНКЦИИ\n";
    file << "I = int_0^3 sin(100x) * exp(-x²) * cos(2x) dx\n\n";

    file << "СРАВНЕНИЕ МЕТОДОВ ЧИСЛЕННОГО ИНТЕГРИРОВАНИЯ\n";
    file << "===========================================\n";

    std::vector<std::pair<std::string, double> > results;

    results.push_back({"Метод средних прямоугольников", rectangle_method(a, b, n_base)});
    results.push_back({"Метод трапеций", trapezoidal_method(a, b, n_base)});
    results.push_back({"Метод Симпсона", simpson_method(a, b, n_base)});
    results.push_back({"Правило 3/8", three_eights_method(a, b, n_base)});

    results.push_back({"Гаусс 2 узла", gauss_2_nodes(a, b)});
    results.push_back({"Гаусс 3 узла", gauss_3_nodes(a, b)});
    results.push_back({"Гаусс 4 узла", gauss_4_nodes(a, b)});

    std::cout << "РЕЗУЛЬТАТЫ ВЫЧИСЛЕНИЙ:\n";
    std::cout << "======================\n";

    for (const auto &result: results) {
        std::cout << std::setw(25) << std::left << result.first << ": "
                << std::scientific << result.second << "\n";
        file << std::setw(25) << std::left << result.first << ": "
                << std::scientific << result.second << "\n";
    }

    file << "\nАНАЛИЗ СХОДИМОСТИ МЕТОДА СИМПСОНА:\n";
    file << "N\t\tI_h\t\t\t\tПогрешность (правило Рунге)\n";

    std::cout << "\nАНАЛИЗ СХОДИМОСТИ МЕТОДА СИМПСОНА:\n";
    std::cout << "N\t\tI_h\t\t\t\tПогрешность (правило Рунге)\n";

    double I_prev = 0;
    for (int n = 1000; n <= 100000; n *= 2) {
        double I_current = simpson_method(a, b, n);
        double error = (n > 1000) ? runge_error(I_prev, I_current, 4) : 0;

        file << n << "\t\t" << I_current << "\t\t" << error << "\n";
        std::cout << n << "\t\t" << I_current << "\t\t" << error << "\n";

        I_prev = I_current;
    }

    file << "\nДАННЫЕ ДЛЯ ГРАФИКА ФУНКЦИИ:\n";
    file << "x\t\tf(x)\n";

    int plot_points = 1000;
    for (int i = 0; i <= plot_points; i++) {
        double x = a + (b - a) * i / plot_points;
        file << x << "\t\t" << f(x) << "\n";
    }

    file.close();

    std::cout << "\n=============================================\n";
    std::cout << "Результаты сохранены в файл: data/results.txt\n";
    std::cout << "Для визуализации запустите C# программу.\n";

    return 0;
}
