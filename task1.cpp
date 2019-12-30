#include <iostream>
#include "aux.hpp"
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


int main(){
    double** A = newMatrix(rows, columns);
    double* minimums = minsInRows(A, rows, columns);
    double max = maxInArray(minimums, rows);



    return 0;
}