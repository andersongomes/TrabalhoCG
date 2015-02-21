#include "vetor.h"
#include <stdio.h>
#include <stdlib.h>

void Vetor::inicializaVetor(float x, float y, float z){
    vetor[0] = x;
    vetor[1] = y;
    vetor[2] = z;
    /*
    static int contador=1;

    cout<<"Entre com o valor do Vetor "<<contador<<":"<<endl;
    for(int i=0;i<3;i++){
        if(i==0){
            cout<<"Entre com o valor i:";
            cin>>vetor[i];
        }
        else{
            if(i==1){
                cout<<"Entre com o valor j:";
                cin>>vetor[i];
            }
            else{
                if(i==2){
                    cout<<"Entre com o valor k:";
                    cin>>vetor[i];
                }
            }
        }
    }
    contador ++;
    */
}

void Vetor::showVetor(int i){
   cout<<"("<<vetor[0]<<","<<vetor[1]<<","<<vetor[2]<<")"<<endl;
   //Imprimi os parâmetros dos vetores
   FILE *saida;
   if(i == 1){
        saida = fopen("outV1.txt", "w");
   }else if(i == 2){
        saida = fopen("outV2.txt", "w");
   }else if(i == 3){
        saida = fopen("outV3.txt", "w");
   }

   if(saida != NULL){
       fprintf(saida, "%.2f %.2f %.2f", vetor[0], vetor[1], vetor[2]);
   }

   fclose(saida);
}


