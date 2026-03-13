#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iomanip>

/**
 * @brief Функция вычисления частичной суммы ряда Маклорена для sin(t) и exp(t).
 *
 * @param function_type тип функции: "sin" или "exp".
 * @param t аргумент функции.
 * @param n_terms количество слагаемых в ряде.
 * 
 * Для sin(t): использует ряд t - t^3/3! + t^5/5! - ...
 * Для exp(t): использует ряд 1 + t + t^2/2! + t^3/3! + ...
 * Возвращает приближенное значение функции.
*/
double maclaurin_sum(const std::string &function_type, double const t, int const n_terms) {
    double sum = 0;
    if (function_type == "sin") {
        // Для sin: t - t^3/3! + t^5/5! - ...
        double term = t;
        sum = term;
        for (int n = 3; n <= n_terms; n += 2) {
            term = -term * t * t / (n * (n - 1));
            sum += term;
        }
    } else {
        // exp
        // Для exp: 1 + t + t^2/2! + t^3/3! + ...
        double term = 1.0;
        sum = term;
        for (int n = 1; n <= n_terms; n++) {
            term = term * t / n;
            sum += term;
        }
    }
    return sum;
}

/**
 * @brief Функция улучшенного алгоритма вычисления sin(t) для больших аргументов.
 *
 * @param t аргумент функции.
 * 
 * Использует периодичность sin(t): sin(t) = sin(t + 2pi k).
 * Приводит аргумент к диапазону [-pi pi] для улучшения сходимости ряда.
 * Возвращает значение sin(t) с высокой точностью.
*/
double improved_sin(double const t) {
    double reduced_t = fmod(t, 2 * M_PI);
    if (reduced_t > M_PI) {
        reduced_t -= 2 * M_PI;
    } else if (reduced_t < -M_PI) {
        reduced_t += 2 * M_PI;
    }
    return maclaurin_sum("sin", reduced_t, 15);
}

/**
 * @brief Функция улучшенного алгоритма вычисления exp(t) для больших аргументов.
 *
 * @param t аргумент функции.
 * 
 * Использует свойство: exp(t) = exp(t/2)^2.
 * Рекурсивно уменьшает аргумент до значения <= 1.0.
 * Возвращает значение exp(t) с высокой точностью.
*/
double improved_exp(double t) {
    int k = 0;
    double reduced_t = t;
    while (reduced_t > 1.0) {
        reduced_t /= 2.0;
        k++;
    }
    double result = maclaurin_sum("exp", reduced_t, 15);
    for (int i = 0; i < k; i++) {
        result *= result;
    }
    return result;
}

/**
 * @brief Функция подбора оптимального числа слагаемых.
 *
 * @param function_type тип функции: "sin" или "exp".
 * @param t аргумент функции.
 * @param target_error целевая погрешность.
 * 
 * Для sin(t): перебирает нечетные n (3, 5, 7, ...).
 * Для exp(t): перебирает все n (1, 2, 3, ...).
 * Возвращает минимальное n, обеспечивающее требуемую точность.
*/
int find_optimal_n(const std::string &function_type, double const t, double const target_error) {
    double exact_value;
    if (function_type == "sin") {
        exact_value = sin(t);
    } else { // exp
        exact_value = exp(t);
    }
    int n = 1;
    double error = 1.0;
    int max_iterations = (function_type == "sin") ? 50 : 40;
    while ((error > target_error) && n < max_iterations) {
        double approx_value = maclaurin_sum(function_type, t, n);
        error = std::abs(exact_value - approx_value);
        if (error <= target_error) {
            break;
        }
        n += (function_type == "sin") ? 2 : 1;
    }

    return n;
}

/**
 * @brief Функция анализа точности рядов Маклорена и сохранения результатов.
 * 
 * Сравнивает три метода вычисления:
 * 1. Точное значение
 * 2. Ряд Маклорена с оптимальным n
 * 3. Улучшенные алгоритмы с приведением аргумента
 * 
 * Сохраняет результаты для интервалов [0,1] и [10,11] в файл.
 * Выводит статистику точности в консоль.
 */
void analyze_and_save_results() {
    std::ofstream file("data/results.txt");
    file << std::fixed << std::setprecision(10);

    std::cout << "Анализ оптимального числа слагаемых в ряде Маклорена\n";
    std::cout << "Погрешность аргумента: dt = 0.001\n";
    std::cout << "==================================================\n" << "\n";

    const int n_sin_01 = find_optimal_n("sin", 0.5, 0.001); // середина [0,1]
    const int n_exp_01 = find_optimal_n("exp", 0.5, 0.001); // середина [0,1]
    const int n_sin_1011 = find_optimal_n("sin", 10.5, 0.001); // середина [10,11]
    const int n_exp_1011 = find_optimal_n("exp", 10.5, 0.001); // середина [10,11]

    std::cout << "ОПТИМАЛЬНЫЕ ЗНАЧЕНИЯ:\n";
    std::cout << "sin(t) на [0,1]: n = " << n_sin_01 << "\n";
    std::cout << "exp(t) на [0,1]: n = " << n_exp_01 << "\n";
    std::cout << "sin(t) на [10,11]: n = " << n_sin_1011 << "\n";
    std::cout << "exp(t) на [10,11]: n = " << n_exp_1011 << "\n";

    file << "ОПТИМАЛЬНЫЕ ЗНАЧЕНИЯ:\n";
    file << "sin(t) на [0,1]: n = " << n_sin_01 << "\n";
    file << "exp(t) на [0,1]: n = " << n_exp_01 << "\n";
    file << "sin(t) на [10,11]: n = " << n_sin_1011 << "\n";
    file << "exp(t) на [10,11]: n = " << n_exp_1011 << "\n\n";

    file << "Значения функций на [0,1]:\n";
    file << "t\tsin_exact\tsin_approx\texp_exact\texp_approx\tsin_improved\texp_improved\n";

    for (double t = 0; t <= 1.0; t += 0.02) {
        double exact_sin = sin(t);
        double approx_sin = maclaurin_sum("sin", t, n_sin_01);
        double improved_sin_val = improved_sin(t);

        double exact_exp = exp(t);
        double approx_exp = maclaurin_sum("exp", t, n_exp_01);
        double improved_exp_val = improved_exp(t);

        file << t << "\t" << exact_sin << "\t" << approx_sin << "\t"
                << exact_exp << "\t" << approx_exp << "\t"
                << improved_sin_val << "\t" << improved_exp_val << "\n";
    }

    file << "\nЗначения функций на [10,11]:\n";
    file << "t\tsin_exact\tsin_approx\texp_exact\texp_approx\tsin_improved\texp_improved\n";
    
    for (double t = 10; t <= 11; t += 0.05) {
        double exact_sin = sin(t);
        double approx_sin = maclaurin_sum("sin", t, n_sin_1011);
        double improved_sin_val = improved_sin(t);

        double exact_exp = exp(t);
        double approx_exp = maclaurin_sum("exp", t, n_exp_1011);
        double improved_exp_val = improved_exp(t);

        file << t << "\t" << exact_sin << "\t" << approx_sin << "\t"
                << exact_exp << "\t" << approx_exp << "\t"
                << improved_sin_val << "\t" << improved_exp_val << "\n";
    }
    file.close();

    std::cout << "\n=== ПРОВЕРКА ТОЧНОСТИ ===" << "\n";
    double t1 = 1.0;
    double exact_sin1 = sin(t1);
    double approx_sin1 = maclaurin_sum("sin", t1, n_sin_01);
    double improved_sin1 = improved_sin(t1);

    std::cout << "\nДля t = 1.0:" << "\n";
    std::cout << "sin(1.0): точное = " << exact_sin1 << ", приближение = " << approx_sin1
            << ", улучшенное = " << improved_sin1 << "\n";
    std::cout << "погрешность = " << abs(exact_sin1 - approx_sin1) << "\n";

    double exact_exp1 = exp(t1);
    double approx_exp1 = maclaurin_sum("exp", t1, n_exp_01);
    double improved_exp1 = improved_exp(t1);
    std::cout << "exp(1.0): точное = " << exact_exp1 << ", приближение = " << approx_exp1
            << ", улучшенное = " << improved_exp1 << "\n";
    std::cout << "погрешность = " << abs(exact_exp1 - approx_exp1) << "\n";


    double t2 = 10.5;
    double exact_sin2 = sin(t2);
    double approx_sin2 = maclaurin_sum("sin", t2, n_sin_1011);
    double improved_sin2 = improved_sin(t2);

    std::cout << "\nДля t = 10.5:" << "\n";
    std::cout << "sin(10.5): точное = " << exact_sin2 << ", приближение = " << approx_sin2
            << ", улучшенное = " << improved_sin2 << "\n";
    std::cout << "погрешность = " << abs(exact_sin2 - approx_sin2) << "\n";

    double exact_exp2 = exp(t2);
    double approx_exp2 = maclaurin_sum("exp", t2, n_exp_1011);
    double improved_exp2 = improved_exp(t2);
    std::cout << "exp(10.5): точное = " << exact_exp2 << ", приближение = " << approx_exp2
            << ", улучшенное = " << improved_exp2 << "\n";
    std::cout << "погрешность = " << abs(exact_exp2 - approx_exp2) << "\n";
}

int main() {
    std::cout << "==============================================" << "\n";
    std::cout << "Анализ ряда Маклорена для sin(t) и exp(t)" << "\n";
    std::cout << "==============================================" << "\n";

    analyze_and_save_results();

    std::cout << "\n==============================================" << "\n";
    std::cout << "Результаты сохранены в файл: results.txt" << "\n";
    std::cout << "Для визуализации запустите C# программу." << "\n";
    std::cout << "==============================================" << "\n";

    return 0;
}
