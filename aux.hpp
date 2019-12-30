#ifndef aux
#define aux

#include <random>
#include <ctime>


double** newMatrix (int n, int m){
    srand(time(NULL));
    double** Result = new double*[n];
    for (int i=0;i<n;++i){
        Result[i] = new double[n];
        for (int j=0;j<m;++j){
            Result[i][j] = (-100 + rand()%100);
        }
    }
}

double** newMatrix0 (int n, int m){
    double** Result = new double*[n];
    for (int i=0;i<n;++i){
        Result[i] = new double[n];
        for (int j=0;j<m;++j){
            Result[i][j] = 0;
        }
    }
}

double** matrProd (double** A, double** B, int n, int m, int k){
    double **result = new double*[n];
    for (int i = 0; i<n;++i){
        result[i] = new double[k];
        for (int j =0;j<k;++j){
            result[i][j] = 0;
        }
    }
    for (int i =0;i<n;++i){
        for (int j = 0;j<k;++j){
            for (int l = 0;l<m;++l){
                result[i][j]+=A[i][l]*B[l][j];
            }
        }
    }
    return result;
}



#endif