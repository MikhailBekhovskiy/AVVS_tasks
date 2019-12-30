#include <iostream>
#include "aux.hpp"


using namespace std;
int n = 1000;
int m = 1000;
int k = 1000;

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

double** prodParallel(double** A, double** B, int n, int m, int k) {
    auto result = newMatrix0(n,k);
    int i, j, k;
#pragma parallel for, private(i)
    for (i = 0; i < n; ++i) {
#pragma parallel for, private(i, j)
        for (j = 0; j < k; ++j) {
            double sum = 0;
#pragma parallel for reduction(+:sum), private(i, j, k)
            for (k = 0; k < m; ++k) {
                sum += A[i][k] * B[k][j];
            }
            result[i][j] = sum;
        }
    }

    return result;
}

int main(){
    double** A = newMatrix(n, m);
    double** B = newMatrix(m, k);
    auto C = printExecTime(matrProd, A, B, n, m, k);
    auto D = printExecTime(prodParallel, A, B, n, m, k);
    if (C==D){
        cout << "Same results" << endl;
    }else{
        cout << "Results differ" << endl;
    }

    return 0;
}