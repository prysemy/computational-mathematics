#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>

const double lambda = 1e-4;           // коэффициент теплопроводности
const double T_final = 0.01;          // время моделирования (уменьшено для скорости)
const double dt = 0.001;              // шаг по времени

// Сетка для исследования сходимости
const std::vector<int> NX_values = {10, 20, 50, 100, 200, 500, 700, 1000};

// Параметры итерационного метода
const int MAX_ITER = 1000000;
const double EPS = 1e-10;  // чуть менее строгий критерий

/**
 * Аналитическое решение:
 * φ(x, y, t) = cos(πx)·sin(5πy)·e^(-50π²λt)
 */
inline double analytical_solution(double x, double y, double t) {
    return cos(M_PI * x) * sin(5.0 * M_PI * y) * exp(-50.0 * M_PI * M_PI * lambda * t);
}

/**
 * Начальное условие (t = 0)
 */
inline double initial_condition(double x, double y) {
    return cos(M_PI * x) * sin(5.0 * M_PI * y);
}

/**
 * Граничное условие слева (x = 0)
 */
inline double boundary_left(double y, double t) {
    return sin(5.0 * M_PI * y) * exp(-50.0 * M_PI * M_PI * lambda * t);
}

/**
 * Граничное условие справа (x = 1)
 */
inline double boundary_right(double y, double t) {
    return -sin(5.0 * M_PI * y) * exp(-50.0 * M_PI * M_PI * lambda * t);
}
/**
 * Точечный метод Гаусса-Зейделя для решения неявной схемы
 *
 * Уравнение: 25λ·∂²φ/∂x² + λ·∂²φ/∂y² = ∂φ/t
 *
 * Разностная схема:
 * φ_{i,j}^{n+1} = (φ_{i,j}^n + σ_x(φ_{i-1,j} + φ_{i+1,j}) + σ_y(φ_{i,j-1} + φ_{i,j+1})) / a
 * где a = 1 + 2(σ_x + σ_y)
 */
void gauss_seidel(std::vector<std::vector<double>>& phi,
                  const std::vector<std::vector<double>>& phi_old,
                  double sigma_x, double sigma_y,
                  int Nx, int Ny, double t, double dt) {

    double a = 1.0 + 2.0 * (sigma_x + sigma_y);

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        double max_diff = 0.0;

        // Обновление внутренних узлов
        for (int i = 1; i < Nx - 1; ++i) {
            for (int j = 1; j < Ny - 1; ++j) {
                double phi_new = (phi_old[i][j] +
                                  sigma_x * (phi[i-1][j] + phi[i+1][j]) +
                                  sigma_y * (phi[i][j-1] + phi[i][j+1])) / a;

                double diff = std::abs(phi_new - phi[i][j]);
                max_diff = std::max(max_diff, diff);

                phi[i][j] = phi_new;
            }
        }

        // Проверка сходимости
        if (max_diff < EPS) {
            break;
        }
    }
}


void save_profile(const std::string& filename,
                  const std::vector<std::vector<double>>& phi,
                  const std::vector<double>& x,
                  const std::vector<double>& y,
                  double t, int Nx, int Ny) {
    std::ofstream out(filename);
    out << std::scientific << std::setprecision(8);
    out << "# t = " << t << "\n";
    out << "# Nx = " << Nx << ", Ny = " << Ny << "\n";
    out << "# x y phi_numerical phi_analytical error\n";

    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            double phi_anal = analytical_solution(x[i], y[j], t);
            double err = std::abs(phi[i][j] - phi_anal);
            out << x[i] << " " << y[j] << " "
                << phi[i][j] << " " << phi_anal << " " << err << "\n";
        }
    }
    out.close();
}

void save_slice(const std::string& filename,
                const std::vector<std::vector<double>>& phi,
                const std::vector<double>& x,
                double y_mid, double t, int Nx) {
    std::ofstream out(filename);
    out << std::scientific << std::setprecision(8);
    out << "# t = " << t << ", y = " << y_mid << "\n";
    out << "# x phi_numerical phi_analytical\n";

    int j_mid = Nx / 2;
    for (int i = 0; i < Nx; ++i) {
        double phi_anal = analytical_solution(x[i], y_mid, t);
        out << x[i] << " " << phi[i][j_mid] << " " << phi_anal << "\n";
    }
    out.close();
}


int main() {
    std::cout << "=== 2D Heat Equation Solver ===" << std::endl;
    std::cout << "T_final = " << T_final << ", dt = " << dt << std::endl;
    std::cout << "Lambda = " << lambda << std::endl;
    std::cout << "Grid sizes: ";
    for (int Nx : NX_values) std::cout << Nx << " ";
    std::cout << std::endl << std::endl;

    // Создание директорий
    system("mkdir -p output_2d plots_2d");

    // Файл для сохранения ошибок
    std::ofstream error_file("output_2d/convergence_data.txt");
    error_file << "# Nx hy L2_error\n";

    // ========================================================================
    // ЦИКЛ ПО РАЗМЕРНОСТЯМ СЕТКИ
    // ========================================================================
    for (size_t k = 0; k < NX_values.size(); ++k) {
        int Nx = NX_values[k];
        int Ny = Nx;  // квадратная область

        double hx = 1.0 / (Nx - 1);
        double hy = 1.0 / (Ny - 1);

        std::cout << "=== Расчет для сетки " << Nx << "x" << Ny << " ===" << std::endl;
        std::cout << "hx = " << hx << ", hy = " << hy << std::endl;

        // Параметры схемы
        double sigma_x = 25.0 * lambda * dt / (hx * hx);
        double sigma_y = lambda * dt / (hy * hy);

        std::cout << "sigma_x = " << sigma_x << ", sigma_y = " << sigma_y << std::endl;

        std::vector<double> x(Nx), y(Ny);
        for (int i = 0; i < Nx; ++i) x[i] = i * hx;
        for (int j = 0; j < Ny; ++j) y[j] = j * hy;

        std::vector<std::vector<double>> phi(Nx, std::vector<double>(Ny, 0.0));
        std::vector<std::vector<double>> phi_old(Nx, std::vector<double>(Ny, 0.0));

        for (int i = 0; i < Nx; ++i) {
            for (int j = 0; j < Ny; ++j) {
                phi[i][j] = initial_condition(x[i], y[j]);
            }
        }

        double t = 0.0;
        int step = 0;

        while (t < T_final - 1e-10) {
            phi_old = phi;

            // Граничные условия
            // Нижняя и верхняя границы (y = 0, y = 1)
            for (int i = 0; i < Nx; ++i) {
                phi[i][0] = 0.0;
                phi[i][Ny-1] = 0.0;
            }

            // Левая и правая границы (x = 0, x = 1)
            for (int j = 0; j < Ny; ++j) {
                phi[0][j] = boundary_left(y[j], t + dt);
                phi[Nx-1][j] = boundary_right(y[j], t + dt);
            }

            gauss_seidel(phi, phi_old, sigma_x, sigma_y, Nx, Ny, t, dt);

            t += dt;
            step++;
        }

        std::cout << "Временных шагов: " << step << std::endl;

        double error = 0.0;
        for (int i = 0; i < Nx; ++i) {
            for (int j = 0; j < Ny; ++j) {
                double diff = phi[i][j] - analytical_solution(x[i], y[j], T_final);
                error += diff * diff;
            }
        }
        error = std::sqrt(error * hx * hy);

        std::cout << "L2 ошибка = " << error << std::endl;
        std::cout << std::endl;

        error_file << Nx << " " << hy << " " << error << "\n";

        if (k == NX_values.size() - 1 || Nx == 100) {
            save_profile("output_2d/profile_N" + std::to_string(Nx) + ".dat",
                        phi, x, y, T_final, Nx, Ny);
            save_slice("output_2d/slice_N" + std::to_string(Nx) + ".dat",
                      phi, x, 0.5, T_final, Nx);
        }
    }

    error_file.close();
}