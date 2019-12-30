#include <iostream>
#include "aux.hpp"
#include <chrono>
using namespace std;

int rows = 1000;
int columns = 1000;
double* minsInRows(double**A, int n, int m){
    double* result = new double[n];
    for (int i = 0;i<n;++i){
        result[i] = A[i][0];
    }
    for (int i = 0;i<n;++i){
        for(int j=1;j<m;++j){
            if (result[i] > A[i][j]){
                result[i] = A[i][j];
            }
        }
    }
    return result;
}

double maxInArray(double* b, int n){
    double result = b[0];
    for (int i = 1;i<n;++i){
        if (b[i] > result){
            result = b[i];
        }
    }
    return result;
}

double maxFromMinsSeq(double** A, int n, int m){
    double* rowMins = minsInRows(A, n, m);
    double result = maxInArray(rowMins, n);
    cout << "Max: " << result << endl;
    return result;
}

double maxFromMinsParallel(double** A, int n, int m) {
    int rows = n;
    int cols = m;
    int rowMin[rows];
    int max = -1000;
#pragma omp parallel shared(A, rows, cols, max, rowMin) default(none)
    {
#pragma omp for
        for (int i = 0; i < rows; ++i) {
            rowMin[i] = A[i][0];
        }

#pragma omp for
        for (int i = 0; i < rows; ++i) {
            for (int j = 1; j < cols; ++j) {
                if (A[i][j] < rowMin[i]) {
                    rowMin[i] = A[i][j];
                }
            }
        }

#pragma omp for reduction(max:max)
        for (int i = 0; i < rows; ++i) {
            if (max < rowMin[i]) {
                max = rowMin[i];
            }
        }
    };

    cout << "Max: " << max << endl;

    return max;
}

template <typename F>
void printExecTime(F &function, double** A, int n, int m) {
    auto start = chrono::high_resolution_clock::now();
    function(A, n, m);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>
                            (end - start).count() / 1e9;
    cout << "Elapsed time: " << duration << endl;

}

int main(){
    double** A = newMatrix(rows, columns);
    printExecTime(maxFromMinsSeq, A, rows, columns);
    printExecTime(maxFromMinsParallel, A, rows, columns);



    return 0;
}