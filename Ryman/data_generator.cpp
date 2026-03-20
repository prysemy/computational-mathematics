#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

const double GAMMA = 5.0 / 3.0;
const double P_ATM = 101325.0;
const double L = 10.0;
const double T_MAX = 0.02;
const double H = 0.2;
const double CFL = 1e-4;

const int NX = int(2 * L / H) + 1;

using Vec3 = std::array<double, 3>;

void initPrimitive(double x, Vec3& v) {
    if (x < 0) {
        v = {13.0, 0.0, 10.0 * P_ATM};
    } else {
        v = {1.3, 0.0, 1.0 * P_ATM};
    }
}

void primToCons(const Vec3& v, Vec3& w) {
    w[0] = v[0];
    w[1] = v[0] * v[1];
    double e_int = v[2] / ((GAMMA - 1.0) * v[0]);
    w[2] = v[0] * e_int + 0.5 * v[0] * v[1] * v[1];
}

void consToPrim(const Vec3& w, Vec3& v) {
    v[0] = w[0];
    v[1] = w[1] / w[0];
    double e_total = w[2] / w[0];
    double e_int = e_total - 0.5 * v[1] * v[1];
    v[2] = (GAMMA - 1.0) * w[0] * e_int;
}

void computeA(double u, double e, double gamma, double A[3][3]) {
    A[0][0] = 0.0;        A[0][1] = 1.0;        A[0][2] = 0.0;
    A[1][0] = -u * u;     A[1][1] = 2.0 * u;    A[1][2] = gamma - 1.0;
    A[2][0] = -gamma * u * e; A[2][1] = gamma * e; A[2][2] = u;
}

void computeOmegaT(double u, double c, double gamma, double OmegaT[3][3]) {
    OmegaT[0][0] = -u * c;    OmegaT[0][1] = c;        OmegaT[0][2] = gamma - 1.0;
    OmegaT[1][0] = -c * c;    OmegaT[1][1] = 0.0;      OmegaT[1][2] = gamma - 1.0;
    OmegaT[2][0] = u * c;     OmegaT[2][1] = -c;       OmegaT[2][2] = gamma - 1.0;
}

void computeLambda(double u, double c, double Lambda[3][3], double LambdaAbs[3][3]) {
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) Lambda[i][j] = 0.0;
    Lambda[0][0] = u + c;
    Lambda[1][1] = u;
    Lambda[2][2] = u - c;

    for(int i=0; i<3; i++) for(int j=0; j<3; j++) LambdaAbs[i][j] = 0.0;
    LambdaAbs[0][0] = std::abs(u + c);
    LambdaAbs[1][1] = std::abs(u);
    LambdaAbs[2][2] = std::abs(u - c);
}

void matMul3x3(double A[3][3], double B[3][3], double C[3][3]) {
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            C[i][j] = 0.0;
            for(int k=0; k<3; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matVecMul3x3(double A[3][3], const Vec3& x, Vec3& y) {
    for(int i=0; i<3; i++) {
        y[i] = 0.0;
        for(int j=0; j<3; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

void inv3x3(double A[3][3], double Ainv[3][3]) {
    double det = A[0][0]*(A[1][1]*A[2][2] - A[1][2]*A[2][1]) -
                 A[0][1]*(A[1][0]*A[2][2] - A[1][2]*A[2][0]) +
                 A[0][2]*(A[1][0]*A[2][1] - A[1][1]*A[2][0]);

    double invdet = 1.0 / det;

    Ainv[0][0] =  (A[1][1]*A[2][2] - A[1][2]*A[2][1]) * invdet;
    Ainv[0][1] = -(A[0][1]*A[2][2] - A[0][2]*A[2][1]) * invdet;
    Ainv[0][2] =  (A[0][1]*A[1][2] - A[0][2]*A[1][1]) * invdet;

    Ainv[1][0] = -(A[1][0]*A[2][2] - A[1][2]*A[2][0]) * invdet;
    Ainv[1][1] =  (A[0][0]*A[2][2] - A[0][2]*A[2][0]) * invdet;
    Ainv[1][2] = -(A[0][0]*A[1][2] - A[0][2]*A[1][0]) * invdet;

    Ainv[2][0] =  (A[1][0]*A[2][1] - A[1][1]*A[2][0]) * invdet;
    Ainv[2][1] = -(A[0][0]*A[2][1] - A[0][1]*A[2][0]) * invdet;
    Ainv[2][2] =  (A[0][0]*A[1][1] - A[0][1]*A[1][0]) * invdet;
}

int main() {
    fs::create_directory("gas_data");

    std::vector<double> x(NX);
    for(int i=0; i<NX; i++) {
        x[i] = -L + i * H;
    }

    std::vector<Vec3> w_curr(NX);
    std::vector<Vec3> v_curr(NX);

    for(int i=0; i<NX; i++) {
        Vec3 v;
        initPrimitive(x[i], v);
        primToCons(v, w_curr[i]);
    }

    std::vector<std::vector<Vec3>> ws;
    ws.push_back(w_curr);

    double t = 0.0;
    int step = 0;

    std::ofstream("gas_data/init.csv") << "x,rho,u,P,t\n";
    for(int i=0; i<NX; i++) {
        consToPrim(w_curr[i], v_curr[i]);
        std::ofstream file("gas_data/init.csv", std::ios::app);
        file << x[i] << "," << v_curr[i][0] << "," << v_curr[i][1] << "," << v_curr[i][2] << "," << t << "\n";
    }

    while(t < T_MAX) {
        std::vector<Vec3> w_next(NX);
        double lambda_max = 0.0;

        for(int i=1; i<NX-1; i++) {
            double rho = w_curr[i][0];
            double rho_u = w_curr[i][1];
            double rho_e = w_curr[i][2];

            double u = rho_u / rho;
            double e_int = (rho_e - 0.5 * rho * u * u) / rho;
            double c = std::sqrt(GAMMA * (GAMMA - 1.0) * e_int);

            double A[3][3];
            computeA(u, e_int, GAMMA, A);

            double OmegaT[3][3];
            computeOmegaT(u, c, GAMMA, OmegaT);

            double Lambda[3][3], LambdaAbs[3][3];
            computeLambda(u, c, Lambda, LambdaAbs);

            double OmegaT_inv[3][3];
            inv3x3(OmegaT, OmegaT_inv);

            double Temp[3][3], B[3][3];
            matMul3x3(LambdaAbs, OmegaT, Temp);
            matMul3x3(OmegaT_inv, Temp, B);

            Vec3 dw_center;
            for(int j=0; j<3; j++) {
                dw_center[j] = w_curr[i+1][j] - w_curr[i][j];
            }

            Vec3 dw_laplace;
            for(int j=0; j<3; j++) {
                dw_laplace[j] = w_curr[i+1][j] + w_curr[i-1][j] - 2.0 * w_curr[i][j];
            }

            Vec3 A_dw;
            matVecMul3x3(A, dw_center, A_dw);

            Vec3 B_dw;
            matVecMul3x3(B, dw_laplace, B_dw);

            for(int j=0; j<3; j++) {
                w_next[i][j] = w_curr[i][j] - 0.5 * CFL * A_dw[j] + 0.5 * CFL * B_dw[j];
            }

            lambda_max = std::max(lambda_max, std::max(std::abs(u + c), std::max(std::abs(u), std::abs(u - c))));
        }

        w_next[0] = w_next[1];
        w_next[NX-1] = w_next[NX-2];

        ws.push_back(w_next);
        w_curr = w_next;

        double tau = CFL * H / lambda_max;
        t += tau;
        step++;

        if(step % 500 == 0 || t >= T_MAX) {
            char filename[100];
            sprintf(filename, "gas_data/t_%.6f.csv", t);
            std::ofstream file(filename);
            file << "x,rho,u,P,t\n";
            for(int i=0; i<NX; i++) {
                consToPrim(w_curr[i], v_curr[i]);
                file << x[i] << "," << v_curr[i][0] << "," << v_curr[i][1] << "," << v_curr[i][2] << "," << t << "\n";
            }
            std::cout << "t=" << t << ", step=" << step << std::endl;
        }
    }

    std::cout << "Done! Total steps: " << step << std::endl;
    return 0;
}