#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <filesystem>
#include <sys/stat.h>

namespace fs = std::filesystem;

// Константы задачи
const double L = 20.0;
const double T = 18.0;
const double pi = std::acos(-1.0);

/**
 * @brief Структура для хранения результатов численного решения уравнения переноса.
 *
 * Хранит данные,полученные в результате численного решения задачи переноса:
 *   u_t + u_x = 0, u(x,0) = sin(4pi x/L), u(0,t) = u(L,t)
 *
 * @param x Вектор координат узлов сетки (равномерная сетка от 0 до L).
 * @param u_numerical Численное решение в момент времени T, полученное по разностной схеме.
 * @param u_analytical Точное аналитическое решение u(x,T) = sin(4pi(x-T)/L) с учетом периодичности.
 * @param l2_error Ошибка в среднеквадратичной норме.
 * @param max_error Ошибка в равномерной норме.
 */
struct result {
    std::vector<double> x;
    std::vector<double> u_numerical;
    std::vector<double> u_analytical;
    double l2_error;
    double max_error;
};

// Начальное условие
double u0(double x) {
    return std::sin(4.0 * pi * x / L);
}

// Аналитическое решение
double u_analytical(double x, double t) {
    // Используем периодичность: (x - t) может выйти за пределы [0, L]
    double arg = std::fmod(x - t, L);
    if (arg < 0) arg += L;
    return std::sin(4.0 * pi * arg / L);
}

// Создание папки
void createDirectory(const std::string &path) {
    if (!fs::exists(path)) {
        fs::create_directory(path);
    }
}

// Вычисление ошибок
result computeErrors(const std::vector<double> &x,
                     const std::vector<double> &u_num,
                     const std::vector<double> &u_ana) {
    result res;
    res.x = x;
    res.u_numerical = u_num;
    res.u_analytical = u_ana;

    int n = x.size();
    double h = L / (n - 1);

    // L2 ошибка (среднеквадратичная)
    double l2 = 0.0;
    // Max ошибка (равномерная норма)
    double max_err = 0.0;

    for (int i = 0; i < n; ++i) {
        double err = std::abs(u_num[i] - u_ana[i]);
        l2 += err * err;
        if (err > max_err) max_err = err;
    }

    l2 = std::sqrt(l2 * h); // интегрирование по пространству

    res.l2_error = l2;
    res.max_error = max_err;

    return res;
}

// Схема "Уголок"
result solveCorner(int nx, double sigma, const std::string &filename) {
    double h = L / (nx - 1);
    double tau = sigma * h;
    int nt = static_cast<int>(T / tau) + 1;

    std::vector<double> x(nx);
    std::vector<double> u_curr(nx);
    std::vector<double> u_next(nx);

    // Инициализация сетки и начального условия
    for (int i = 0; i < nx; ++i) {
        x[i] = i * h;
        u_curr[i] = u0(x[i]);
    }

    // Основной цикл по времени
    for (int n = 0; n < nt; ++n) {
        // Расчет для внутренних узлов (i = 1, ..., nx-1)
        for (int i = 1; i < nx; ++i) {
            u_next[i] = u_curr[i] - sigma * (u_curr[i] - u_curr[i - 1]);
        }
        // Периодическое ГУ: u(0) = u(L)
        u_next[0] = u_next[nx - 1];

        // Переход к следующему слою
        u_curr.swap(u_next);
    }

    // Вычисляем аналитическое решение
    std::vector<double> u_ana(nx);
    for (int i = 0; i < nx; ++i) {
        u_ana[i] = u_analytical(x[i], T);
    }

    // Вычисляем ошибки
    result res = computeErrors(x, u_curr, u_ana);

    // Запись результата в CSV
    std::ofstream file(filename);
    file << "x,u_numerical,u_analytical\n";
    for (int i = 0; i < nx; ++i) {
        file << std::fixed << std::setprecision(8) << x[i] << ","
                << res.u_numerical[i] << ","
                << res.u_analytical[i] << "\n";
    }
    file.close();

    std::cout << "  Saved: " << filename
            << ", L2 error = " << std::scientific << res.l2_error
            << ", Max error = " << res.max_error << std::endl;

    return res;
}

// Схема Лакса-Вендроффа
result solveLaxWendroff(int nx, double sigma, const std::string &filename) {
    double h = L / (nx - 1);
    double tau = sigma * h;
    int nt = static_cast<int>(T / tau) + 1;

    std::vector<double> x(nx);
    std::vector<double> u_curr(nx);
    std::vector<double> u_next(nx);

    // Инициализация
    for (int i = 0; i < nx; ++i) {
        x[i] = i * h;
        u_curr[i] = u0(x[i]);
    }

    // Цикл по времени
    for (int n = 0; n < nt; ++n) {
        // Внутренние узлы (i = 1, ..., nx-2)
        for (int i = 1; i < nx - 1; ++i) {
            u_next[i] = u_curr[i]
                        - 0.5 * sigma * (u_curr[i + 1] - u_curr[i - 1])
                        + 0.5 * sigma * sigma * (u_curr[i + 1] - 2.0 * u_curr[i] + u_curr[i - 1]);
        }

        // Граничные узлы с учетом периодичности
        // i = 0
        u_next[0] = u_curr[0]
                    - 0.5 * sigma * (u_curr[1] - u_curr[nx - 2])
                    + 0.5 * sigma * sigma * (u_curr[1] - 2.0 * u_curr[0] + u_curr[nx - 2]);

        // i = nx-1
        u_next[nx - 1] = u_curr[nx - 1]
                         - 0.5 * sigma * (u_curr[1] - u_curr[nx - 2])
                         + 0.5 * sigma * sigma * (u_curr[1] - 2.0 * u_curr[nx - 1] + u_curr[nx - 2]);

        u_curr.swap(u_next);
    }

    // Вычисляем аналитическое решение
    std::vector<double> u_ana(nx);
    for (int i = 0; i < nx; ++i) {
        u_ana[i] = u_analytical(x[i], T);
    }

    // Вычисляем ошибки
    result res = computeErrors(x, u_curr, u_ana);

    // Запись результата в CSV
    std::ofstream file(filename);
    file << "x,u_numerical,u_analytical\n";
    for (int i = 0; i < nx; ++i) {
        file << std::fixed << std::setprecision(8) << x[i] << ","
                << res.u_numerical[i] << ","
                << res.u_analytical[i] << "\n";
    }
    file.close();

    std::cout << "  Saved: " << filename
            << ", L2 error = " << std::scientific << res.l2_error
            << ", Max error = " << res.max_error << std::endl;

    return res;
}

int main() {
    // Параметры расчетов
    std::vector<int> nx_values = {41, 81, 161};
    std::vector<double> sigma_values = {1.0, 0.6, 0.3};

    // Создаем папку для данных
    std::string data_dir = "data";
    createDirectory(data_dir);

    // Цикл по всем комбинациям
    for (int nx: nx_values) {
        for (double sigma: sigma_values) {
            std::cout << "Calculating for nx = " << nx << ", sigma = " << sigma << std::endl;

            // Формируем пути к файлам в папке data
            std::string corner_filename = data_dir + "/corner_nx" + std::to_string(nx) +
                                          "_sigma" + std::to_string(sigma).substr(0, 3) + ".csv";
            std::string lw_filename = data_dir + "/lw_nx" + std::to_string(nx) +
                                      "_sigma" + std::to_string(sigma).substr(0, 3) + ".csv";

            // Решаем
            result res_corner = solveCorner(nx, sigma, corner_filename);
            result res_lw = solveLaxWendroff(nx, sigma, lw_filename);

            std::cout << "  Corner vs LW error ratio (L2): "
                    << res_corner.l2_error / res_lw.l2_error << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }
    }

    std::cout << "======================================" << std::endl;
    std::cout << "All calculations finished!" << std::endl;

    return 0;
}
