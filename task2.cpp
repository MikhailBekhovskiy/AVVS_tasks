#include <iostream>
#include "aux.hpp"

int n = 1000;
int m = 1000;
int k = 1000;

int main(){
    double** A = newMatrix(n, m);
    double** B = newMatrix(m, k);
    double** C = matrMult(A, B, n, m, k);


    return 0;
}