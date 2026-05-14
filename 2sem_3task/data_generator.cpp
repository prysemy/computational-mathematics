#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>

const double P_ATM = 101325.0;           // 1 атм в Па
const double L = 500.0;                   // длина пласта, м
const double p0 = 100.0 * P_ATM;          // начальное давление, Па
const double P_LEFT = 150.0 * P_ATM;      // давление нагнетания (x=0), Па
const double P_RIGHT = 50.0 * P_ATM;      // давление добычи (x=L), Па
const double k = 1e-14;                   // проницаемость, м² (10 мД)
const double mu = 1e-3;                   // вязкость, Па·с (1 сПз)
const double phi = 0.2;                   // пористость
const double cf = 1e-4 / P_ATM;           // сжимаемость, 1/Па
const double rho0 = 1e3;                  // плотность при p0, кг/м³

const int DAYS = 10;                      // время моделирования, дни
const double T_MAX = DAYS * 24.0 * 3600.0; // время, с
const int N = 100;                        // число узлов по пространству
const int M = DAYS * 24 + 1;              // число временных шагов
const double h = L / (N - 1);             // шаг по пространству
const double tau = T_MAX / (M - 1);       // шаг по времени (1 час)

const std::vector<double> t_output_days = {0.1, 0.25, 0.5, 1.0, 3.0, 5.0, 7.0, 10.0};

/**
 * Расчет плотности от давления
 * ρ = ρ₀(1 + c_f(p - p₀))
 */
inline double calculate_density(double p) {
    return rho0 * (1.0 + cf * (p - p0));
}

/**
 * Метод прогонки для трехдиагональной СЛАУ
 *
 * Система: a[i]*x[i] + b[i]*x[i+1] = d[i] - c[i-1]*x[i-1]
 * где a - главная диагональ, b - верхняя, c - нижняя
 *
 * @param a главная диагональ (размер n)
 * @param b верхняя диагональ (размер n-1)
 * @param c нижняя диагональ (размер n-1)
 * @param d правая часть (размер n)
 * @return решение x (размер n)
 */
std::vector<double> thomas_solve(const std::vector<double>& a,
                                  const std::vector<double>& b,
                                  const std::vector<double>& c,
                                  const std::vector<double>& d) {
    int n = a.size();
    std::vector<double> a_mod = a;
    std::vector<double> d_mod = d;
    std::vector<double> x(n);

    // Прямой ход
    for (int i = 1; i < n; ++i) {
        double m = c[i-1] / a_mod[i-1];
        a_mod[i] -= m * b[i-1];
        d_mod[i] -= m * d_mod[i-1];
    }

    // Обратный ход
    x[n-1] = d_mod[n-1] / a_mod[n-1];
    for (int i = n - 2; i >= 0; --i) {
        x[i] = (d_mod[i] - b[i] * x[i+1]) / a_mod[i];
    }

    return x;
}

void save_profile(const std::string& filename,
                  const std::vector<double>& x,
                  const std::vector<double>& p,
                  double t_days) {
    std::ofstream out(filename);
    out << std::scientific << std::setprecision(6);
    out << "# t = " << t_days * 24.0 * 3600.0 << " s = " << t_days << " days\n";
    out << "# x(m) p(Pa) p(atm)\n";

    for (size_t i = 0; i < x.size(); ++i) {
        out << x[i] << " " << p[i] << " " << p[i] / P_ATM << "\n";
    }
    out.close();
}

int main() {
    std::cout << "=== Плоскопараллельная фильтрация ===" << std::endl;
    std::cout << "N = " << N << ", h = " << h << " м" << std::endl;
    std::cout << "tau = " << tau << " с (1 час)" << std::endl;
    std::cout << "T_max = " << T_MAX/86400.0 << " дней" << std::endl;

    std::vector<double> x(N);
    for (int i = 0; i < N; ++i) {
        x[i] = i * h;
    }

    // Начальное условие (p = 100 атм везде)
    std::vector<double> p(N, p0);

    std::vector<double> a(N, 0.0);  // главная диагональ
    std::vector<double> b(N-1, 0.0); // верхняя диагональ
    std::vector<double> c(N-1, 0.0); // нижняя диагональ
    std::vector<double> d(N, 0.0);   // правая часть

    double t = 0.0;
    size_t output_idx = 0;
    double t_next_output = t_output_days[0] * 86400.0; // перевод в секунды

    system("mkdir -p output");

    save_profile("output/profile_0.dat", x, p, 0.0);
    std::cout << "Saved: t = 0.00 days" << std::endl;

    int profile_count = 1;
    for (int step = 1; step < M; ++step) {
        t += tau;

        std::fill(a.begin(), a.end(), 0.0);
        std::fill(b.begin(), b.end(), 0.0);
        std::fill(c.begin(), c.end(), 0.0);
        std::fill(d.begin(), d.end(), 0.0);

        // i = 0: p = P_LEFT
        a[0] = 1.0;
        b[0] = 0.0; 
        d[0] = P_LEFT;

        // i = N-1: p = P_RIGHT
        a[N-1] = 1.0;
        c[N-2] = 0.0;
        d[N-1] = P_RIGHT;

        double coeff_time = phi * cf * rho0 / tau;

        for (int i = 1; i < N - 1; ++i) {
            // rho_{i-1/2} - плотность на левой границе ячейки
            double rho_left;
            if (p[i-1] >= p[i]) {
                // Поток слева направо: берем плотность слева
                rho_left = calculate_density(p[i-1]);
            } else {
                // Поток справа налево: берем плотность справа
                rho_left = calculate_density(p[i]);
            }

            // rho_{i+1/2} - плотность на правой границе ячейки
            double rho_right;
            if (p[i] >= p[i+1]) {
                // Поток слева направо: берем плотность слева
                rho_right = calculate_density(p[i]);
            } else {
                // Поток справа налево: берем плотность справа
                rho_right = calculate_density(p[i+1]);
            }

            c[i-1] = k * rho_left / (mu * h * h);   // нижняя диагональ (i-1)
            b[i]   = k * rho_right / (mu * h * h);  // верхняя диагональ (i+1)
            a[i]   = -(c[i-1] + b[i] + coeff_time); // главная диагональ
            d[i]   = -coeff_time * p[i];             // правая часть
        }

        std::vector<double> p_new = thomas_solve(a, b, c, d);
        p = p_new;
        
        if (output_idx < t_output_days.size() && t >= t_next_output - 1e-10) {
            double t_days = t / 86400.0;
            std::string filename = "output/profile_" + std::to_string(profile_count) + ".dat";
            save_profile(filename, x, p, t_days);

            std::cout << "Saved: t = " << std::fixed << std::setprecision(2)
                      << t_days << " days" << std::endl;

            profile_count++;
            output_idx++;

            if (output_idx < t_output_days.size()) {
                t_next_output = t_output_days[output_idx] * 86400.0;
            }
        }
    }

    std::cout << "\n=== Расчет завершен ===" << std::endl;
}
