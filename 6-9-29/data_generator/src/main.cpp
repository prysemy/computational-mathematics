#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

/**
 * @brief Структура популяция-дата.
*/
struct population_data {
    int year;
    double population;
};

/**
 * @brief Функция создания таблицы разделенных разностей.
*/
std::vector<std::vector<double> > divided_differences(const std::vector<double> &x,
                                                      const std::vector<double> &y) {
    int n = x.size();
    std::vector<std::vector<double> > diff(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        diff[i][0] = y[i];
    }
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            diff[i][j] = (diff[i + 1][j - 1] - diff[i][j - 1]) / (x[i + j] - x[i]);
        }
    }
    return diff;
}

/**
 * @brief Интерполяционный полином Ньютона.
 *
 * @param x_point точка, в которой вычисляем полином.
 * @param x вектор годов.
 * @param diff таблица разделенных разностей.
 *
 * n - количество точек(годов).
 *
 * product накапливает произведение $(x - x_0)\cdot(x-x1)\cdot...$.
 *
 * result - значение полинома в точке x_point.
*/
double newton_interpolation(double x_point, const std::vector<double> &x,
                            const std::vector<std::vector<double> > &diff) {
    int n = x.size();
    double result = diff[0][0];
    double product = 1.0;
    for (int i = 1; i < n; i++) {
        product *= (x_point - x[i - 1]);
        result += diff[0][i] * product;
    }
    return result;
}

/**
 * @brief Линейная сплайн-интерполяция.
 *
 * @param x_point точка, в которой вычисляем значение.
 * @param x вектор годов.
 * @param y вектор значений населения.
 *
 * n - количество точек данных.
 *
 * interval - индекс интервала, в котором находится x_point.
 * Если x_point выходит за пределы данных, используется последний интервал для экстраполяции.
 *
 * result - вычисленное значение по линейной интерполяции/экстраполяции.
 * Формула: y_i + (y_{i+1} - y_i) * (x - x_i) / (x_{i+1} - x_i)
 *
 * Принцип работы:
 * - Для интерполяции (x_point между точками): используется линейная интерполяция на найденном интервале
 * - Для экстраполяции (x_point за пределами): продолжается наклон последнего интервала
 * - Метод устойчив и не склонен к "выстреливанию" как полиномы высокой степени
*/
double linear_spline(double x_point, const std::vector<double> &x, const std::vector<double> &y) {
    int n = x.size();
    int interval = 0;
    for (int i = 0; i < n - 1; i++) {
        if (x_point >= x[i] && x_point <= x[i + 1]) {
            interval = i;
            break;
        }
    }
    if (x_point > x[n - 1]) {
        interval = n - 2;
    }
    double result = y[interval] + (y[interval + 1] - y[interval]) * (x_point - x[interval]) / (
                        x[interval + 1] - x[interval]);
    return result;
}

int main() {
    std::vector<population_data> data = {
        {1910, 92228496},
        {1920, 106021537},
        {1930, 123202624},
        {1940, 132164569},
        {1950, 151325798},
        {1960, 179323175},
        {1970, 203211926},
        {1980, 226545805},
        {1990, 248709873},
        {2000, 281421906}
    };

    double actual_2010 = 308745538;

    std::vector<double> years, population;
    for (const auto &entry: data) {
        years.push_back(entry.year);
        population.push_back(entry.population);
    }

    auto diff = divided_differences(years, population);

    double newton_2010 = newton_interpolation(2010, years, diff);
    double spline_2010 = linear_spline(2010, years, population);

    double newton_error = abs(newton_2010 - actual_2010);
    double spline_error = abs(spline_2010 - actual_2010);


    std::cout << std::fixed << std::setprecision(0);
    std::cout << "ЭКСТРАПОЛЯЦИЯ НАСЕЛЕНИЯ США НА 2010 ГОД" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Точное значение: " << actual_2010 << " человек" << std::endl;
    std::cout << std::endl;
    std::cout << "а) ИНТЕРПОЛЯЦИОННЫЙ ПОЛИНОМ НЬЮТОНА:" << std::endl;
    std::cout << "   Экстраполированное значение: " << newton_2010 << " человек" << std::endl;
    std::cout << "   Ошибка: " << newton_error << " человек" << std::endl;
    std::cout << "   Относительная ошибка: " << std::fixed << std::setprecision(2) << (newton_error / actual_2010) * 100
            << "%" <<
            std::endl;
    std::cout << std::endl;
    std::cout << "б) СПЛАЙН-АППРОКСИМАЦИЯ:" << std::endl;
    std::cout << "   Экстраполированное значение: " << std::fixed << std::setprecision(0) << spline_2010 << " человек"
            << std::endl;
    std::cout << "   Ошибка: " << spline_error << " человек" << std::endl;
    std::cout << "   Относительная ошибка: " << std::fixed << std::setprecision(2) << (spline_error / actual_2010) * 100
            << "%" <<
            std::endl;
    std::cout << std::endl;
    std::cout << "в) СРАВНЕНИЕ:" << std::endl;
    std::cout << "   Более точный метод: " << (newton_error < spline_error ? "ПОЛИНОМ НЬЮТОНА" : "СПЛАЙН-АППРОКСИМАЦИЯ")
            <<
            std::endl;

    std::ofstream outfile("data/results.txt");
    outfile << std::fixed << std::setprecision(0);
    outfile << "ИСХОДНЫЕ ДАННЫЕ:" << std::endl;
    for (const auto &entry: data) {
        outfile << entry.year << "\t" << entry.population << std::endl;
    }
    outfile << "2010\t" << actual_2010 << "\t# Точное значение" << std::endl;

    outfile << "РЕЗУЛЬТАТЫ ЭКСТРАПОЛЯЦИИ:" << std::endl;
    outfile << "newton_2010 = " << newton_2010 << std::endl;
    outfile << "spline_2010 = " << spline_2010 << std::endl;
    outfile << "actual_2010 = " << actual_2010 << std::endl;
    outfile << "newton_error = " << newton_error << std::endl;
    outfile << "spline_error = " << spline_error << std::endl;

    outfile << "ДАННЫЕ ДЛЯ ГРАФИКА:" << std::endl;
    outfile << "Год\tНаселение\tТип" << std::endl;

    outfile << "исходные" << std::endl;
    for (const auto &entry: data) {
        outfile << entry.year << "\t" << entry.population << "\n";
    }

    outfile << "точное\n";
    outfile << 2010 << "\t" << actual_2010 << std::endl;

    outfile << "ньютон" << std::endl;
    for (int year = 1910; year <= 2010; year ++) {
        double newton_val = newton_interpolation(year, years, diff);
        outfile << year << "\t" << newton_val << "\n";
    }

    outfile << "сплайн" << std::endl;
    for (int year = 1910; year <= 2010; year += 5) {
        double spline_val = linear_spline(year, years, population);
        outfile << year << "\t" << spline_val << "\n";
    }
    outfile.close();

    return 0;
}
