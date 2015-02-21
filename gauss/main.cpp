#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[]){
    int i, j, k, n;
    int o, p;
    double fator, sum;
    double a[3][3]= {
        {1.0, 1.0, 1.0},
        {0.0, 1.0, 1.0},
        {0.0, 0.0, 1.0}
     };
    double b[3]= {0.0, 0.0, 0.0};
    double x[3];
    n = 3;
    for(k=0; k<=n-2; k++){
        for(i=k+1; i<=n-1; i++){
            fator = a[i][k]/a[k][k];
            for(j=k+1;j<=n-1;j++){
                a[i][j] = a[i][j] - (fator * a[k][j]);
            }
            b[i] = b[i] - (fator * b[k]);
            a[i][k] = 0.0f;
        }
     }
     x[n-1]=b[n-1]/a[n-1][n-1];
     for(i=n-2; i>=0; i--){
        sum = 0.0f;
        for(j=i+1; j<=n-1; j++){
            sum = sum + a[i][j] * x[j];
        }
        x[i] = (b[i] - sum)/a[i][i];
     }
    if(x[0]==0 && x[1]==0 && x[2]==0){
        printf("LI\n");
    }
    else{
        printf("LD\n");
    }
    /*for(i=0; i<=n-1; i++){
        printf("x(%i) = %+g \n", i, x[i]);
    }*/
    system("PAUSE");
    return 0;
}
