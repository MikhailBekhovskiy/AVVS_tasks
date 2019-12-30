#include "aux.hpp"
#include <iostream>
#include <chrono>

using namespace std;

template <typename F>
double* printExecTime(F &function, double** M, double* V, int n, int m) {
    auto start = chrono::high_resolution_clock::now();
    double* res = function(M, V, n, m);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>
                            (end - start).count() / 1e9;
    cout << "Elapsed time: " << duration << endl;
    return res;
}

double* MatVecMult(double** M, double* V, int n, int m) {
    double* res = new double[n];
    for (int i = 0; i < n; ++i) {
        res[i] = 0;
        for (int j = 0; j < m; ++j) {
            res[i] += M[i][j] * V[i];
        }
    }
    return res;
}

// rowwise decomposition
double* MatVecMultRow(double** M, double* V, int n, int m) {
    double * res = new double[n];
#pragma omp parallel for shared(M, V, res) default(none)
    for (int i = 0; i < n; ++i) {
        res[i] = 0;
        for (int j = 0; j < m; ++j) {
            res[i] += M[i][j] * V[i];
        }
    }
    return res;
}

// columnwise decomposition
double* MatVecMultCol(double** M, double* V, int n, int m) {
    double * res = new double[n];
#pragma omp parallel  shared(M, V, res) default(none)
    {
#pragma omp for
        for (int i = 0; i < n; ++i) {
            res[i] = 0;
        }

#pragma omp for
        for (int j = 0; j < m; ++j) {
            for (int i = 0; i < n; ++i) {
#pragma omp atomic
                res[i] += M[i][j] * V[i];
            }
        }
    };
    return res;
}


// checkerboard decomposition
double* MatVecMultCB(double** M, double* V, int n, int m) {
    int blocksVer = 2;
    int blocksHor = 2;
    double * res = new double[n];
#pragma omp parallel shared(M, V, res, blocksVer, blocksHor) default(none)
    {
#pragma omp for
        for (int i = 0; i < n; ++i) {
            res[i] = 0;
        }

#pragma omp for collapse(2)
        for (int vi = 0; vi < blocksVer; ++vi) {
            for (int hi = 0; hi < blocksHor; ++hi) {
                int vbb = vi / blocksVer * n; // vertical block begin
                int vbe = (vi + 1) / blocksVer * n; // vertical block end
                int hbb = hi / blocksHor * m; // vertical block begin
                int hbe = (hi + 1) / blocksHor * m; // vertical block end
                for (int i = vbb; i < vbe; ++i) {
                    for (int j = hbb; j < hbe; ++j) {
#pragma omp atomic
                        res[i] += M[i][j] * V[i];
                    }
                }
            }
        }
    }
    return res;
}

int main(){
    int rows = 5000;
    int columns = 5000;
    double** M = newMatrix(rows, columns);
    double* V = newVector(columns);
    auto v1 = printExecTime(MatVecMult, M, V, rows, columns);
    auto v2 = printExecTime(MatVecMultRow, M, V, rows, columns);
    auto v3 = printExecTime(MatVecMultCol, M, V, rows, columns);
    auto v4 = printExecTime(MatVecMultCB, M, V, rows, columns);

    cout << (v1 == v2 and v2 == v3 and v3 == v4 ? "Results match" : "Egregious calculation") << endl;


    return 0;
}