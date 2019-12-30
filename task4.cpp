#include "aux.hpp"
#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;

template <typename F>
double** printExecTime(F &function, double** A, double** B, int n, int m, int k) {
    auto start = chrono::high_resolution_clock::now();
    auto res = function(A, B, n, m, k);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>
                            (end - start).count() / 1e9;
    cout << "Elapsed time: " << duration << endl;
    return res;
}

// block-striped decomposition
double** prodBS(double** A, double** B, int n, int m, int k) {
    auto res = newMatrix0(n, k);
#pragma omp parallel shared(A, B, res, cout) default(none)
    {
        int threadsNum = omp_get_num_threads();
#pragma omp for schedule(static) collapse(2)
        for (int iVer = 0; iVer < threadsNum; ++iVer) {
            for (int iHor = 0; iHor < threadsNum; ++iHor) {
                int ABlockBegin = iVer * n / threadsNum;
                int ABlockEnd = (iVer + 1) * n / threadsNum;
                int BBlockBegin = iHor * k / threadsNum;
                int BBlockEnd = (iHor + 1) * k / threadsNum;
                // cout << threadsNum << ";" << thread << endl
                //     << ABlockBegin << ":" << ABlockEnd << ":" << BBlockBegin << ":" << BBlockEnd << endl;
                for (int i = ABlockBegin; i < ABlockEnd; ++i) {
                    for (int j = BBlockBegin; j < BBlockEnd; ++j) {
                        res[i][j] = A[i][0] * B[0][j];
                        for (int k = 1; k < m; ++k) {
                            res[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
        }
    }
    return res;
}

// checkerboard decomposition
double** prodCB(double** A, double** B, int n, int m, int k) {
    auto res = newMatrix0(n,k);

#pragma omp parallel shared(A, B, res) default(none)
    {
        int blocksVer = omp_get_num_threads();
        int blocksHor = omp_get_num_threads();
        int blocksK = omp_get_num_threads();
#pragma omp for
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < k; ++j) {
                res[i][j] = 0;
            }
        }

#pragma omp for collapse(2)
        for (int vi = 0; vi < blocksVer; ++vi) {
            for (int hi = 0; hi < blocksHor; ++hi) {
                for (int ki = 0; ki < blocksK; ++ki) {
                    int vbb = vi * n / blocksVer; // vertical block begin
                    int vbe = (vi + 1) * n / blocksVer; // vertical block end
                    int hbb = hi * k / blocksHor; // vertical block begin
                    int hbe = (hi + 1) * k / blocksHor; // vertical block end
                    int kbb = ki * m / blocksK; // k block begin
                    int kbe = (ki + 1) * m / blocksK; // k block begin

                    for (int i = vbb; i < vbe; ++i) {
                        for (int j = hbb; j < hbe; ++j) {
                            for (int k = kbb; k < kbe; ++k) {
#pragma omp atomic
                                res[i][j] += A[i][k] * B[k][j];
                            }
                        }
                    }
                }
            }
        }
    }

    return res;
}


int main() {
    int rows = 1000;
    int columns = 1000;
    int columns2 = 1000;
    double** A = newMatrix(rows, columns);
    double** B = newMatrix(columns, columns2);
    auto res1 = printExecTime(matrProd, A, B, rows, columns, columns2);
    auto res2 = printExecTime(prodBS, A, B, rows, columns, columns2);
    auto res3 = printExecTime(prodCB, A, B, rows, columns, columns2);

    cout << (*res1 == *res2 and *res2 == *res3 ? "COOL" : "NOT COOL") << endl;

    return 0;
}