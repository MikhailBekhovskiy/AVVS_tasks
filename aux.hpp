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




#endif