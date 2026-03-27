#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

int main() {
    double gamma = 5.0 / 3.0;
    double L = 10.0;
    double T = 0.01;
    int Nx = 500;  
    double h = 2 * L / Nx;

    double x[501];  // Nx+1 = 501
    double rho[501], u[501], P[501], e[501];
    double w_old[501][3];
    double w_new[501][3];

    // Инициализация начальных условий
    for (int i = 0; i <= Nx; i++) {
        x[i] = -L + i * h;
        if (x[i] < 0) {
            rho[i] = 13.0;
            P[i] = 10.0*100000;
            u[i] = 0;
        } else {
            rho[i] = 1.3;
            P[i] = 1.0*100000;
            u[i] = 0;
        }
        e[i] = P[i] / ((gamma - 1.0) * rho[i]);

        w_old[i][0] = rho[i];
        w_old[i][1] = rho[i] * u[i];
        w_old[i][2] = rho[i] * e[i];
    }

    double t = 0.0;
    int step = 0;
    
    // Открываем файл для записи результатов
    ofstream outfile("results2.csv");
    outfile << "x,rho,u,P,e,t\n";
    
    // Записываем начальное состояние
    for (int i = 0; i <= Nx; i++) {
        outfile << x[i] << "," << rho[i] << "," << u[i] << "," << P[i] << "," << e[i] << "," << t << "\n";
    }

    while (t < T) {
        double lambda_max = 0.0;
        for (int i = 0; i <= Nx; i++) {
            double c = sqrt(gamma * P[i] / rho[i]);
            double lambda1 = fabs(u[i] + c);
            double lambda2 = fabs(u[i]);
            double lambda3 = fabs(u[i] - c);
            double max_local = max(lambda1, max(lambda2, lambda3));
            if (max_local > lambda_max) {
                lambda_max = max_local;
            }
        }

        double tau = 0.001 * h / lambda_max;  // CFL = 0.1
        if (t + tau > T) {
            tau = T - t;
        }

        for (int i = 0; i <= Nx; i++) {
            w_new[i][0] = w_old[i][0];
            w_new[i][1] = w_old[i][1];
            w_new[i][2] = w_old[i][2];
        }

        for (int l = 1; l < Nx; l++) {
            double c_l = sqrt(gamma * P[l] / rho[l]);

            double A[3][3] = {
                {0.0, 1.0, 0.0},
                {-u[l] * u[l], 2.0 * u[l], gamma - 1.0},
                {-gamma * u[l] * e[l], gamma * e[l], u[l]}
            };

            double OmegaT[3][3] = {
                {-u[l] * c_l, c_l, gamma - 1.0},
                {-c_l * c_l, 0.0, gamma - 1.0},
                {u[l] * c_l, -c_l, gamma - 1.0}
            };

            double OmegaT_inv[3][3] = {
                {0.5 / (c_l * c_l), (-1.0) / (c_l * c_l), 0.5 / (c_l * c_l)},
                {0.5 * (c_l + u[l]) / (c_l * c_l), (-1.0) * u[l] / (c_l * c_l), 0.5 * (u[l] - c_l) / (c_l * c_l)},
                {0.5 / (gamma - 1.0), 0.0, 0.5 / (gamma - 1.0)}
            };

            double absLambda[3][3] = {
                {fabs(u[l] + c_l), 0.0, 0.0},
                {0.0, fabs(u[l]), 0.0},
                {0.0, 0.0, fabs(u[l] - c_l)}
            };

            double temp[3][3] = {{0.0}};
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    for (int k = 0; k < 3; k++) {
                        temp[i][j] += OmegaT_inv[i][k] * absLambda[k][j];
                    }
                }
            }

            double D[3][3] = {{0.0}};
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    for (int k = 0; k < 3; k++) {
                        D[i][j] += temp[i][k] * OmegaT[k][j];
                    }
                }
            }

            for (int m = 0; m < 3; m++) {
                w_new[l][m] = w_old[l][m];
                for (int n = 0; n < 3; n++) {
                    w_new[l][m] += -tau * A[m][n] * (w_old[l+1][n] - w_old[l-1][n]) / (2.0*h) 
                                 + tau * D[m][n] * (w_old[l+1][n] - 2*w_old[l][n] + w_old[l-1][n]) / (2.0*h);
                }
            }
        }

        // Граничные условия (нулевой градиент)
        w_new[0][0] = w_new[1][0];
        w_new[0][1] = w_new[1][1];
        w_new[0][2] = w_new[1][2];

        w_new[Nx][0] = w_new[Nx-1][0];
        w_new[Nx][1] = w_new[Nx-1][1];
        w_new[Nx][2] = w_new[Nx-1][2];

        // Копируем новый слой в старый
        for (int i = 0; i <= Nx; i++) {
            w_old[i][0] = w_new[i][0];
            w_old[i][1] = w_new[i][1];
            w_old[i][2] = w_new[i][2];
        }

        // Обновление примитивных переменных
        for (int i = 0; i <= Nx; i++) {
            rho[i] = w_old[i][0];
            u[i] = w_old[i][1] / rho[i];
            e[i] = w_old[i][2] / rho[i];
            P[i] = (gamma - 1.0) * rho[i] * e[i];
        }

        t += tau;
        step++;
        
        // Записываем результаты чаще, чтобы анимация была плавной
        if (step % 50 == 0 || t >= T) {
            for (int i = 0; i <= Nx; i++) {
                outfile << x[i] << "," << rho[i] << "," << u[i] << "," << P[i] << "," << e[i] << "," << t << "\n";
            }
            cout << "t = " << t << " с, шаг " << step << endl;
        }
    }
    
    outfile.close();
    cout << "\nРасчет завершен! Шагов: " << step << endl;
    cout << "Результаты сохранены в файл results2.csv" << endl;
    cout << "Количество точек: " << Nx+1 << endl;

    return 0;
}
