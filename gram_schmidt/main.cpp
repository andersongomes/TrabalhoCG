#include <iostream>
#include <math.h>

using namespace std;

class Vetor{
public:
    float vetor[3];
    void inicializaVetor();
    void showVetor();
    void projecaoVetor(Vetor, Vetor);
};

void Vetor::inicializaVetor(){
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
}

void Vetor::showVetor(){
   cout<<"("<<vetor[0]<<","<<vetor[1]<<","<<vetor[2]<<")"<<endl;
}

void gramSchimidt(Vetor v1, Vetor v2,Vetor v3){
    Vetor resultado1,resultado2;
    static int contador=1;
    float produto, escalar;
    if(contador == 1){
        produto = (v1.vetor[0]*v2.vetor[0] + v1.vetor[1]*v2.vetor[1] + v1.vetor[2]*v2.vetor[2]);
        escalar = pow(sqrt(pow(v1.vetor[0],2) + pow(v1.vetor[1],2) + pow(v1.vetor[2],2)),2);
        resultado1.vetor[0] = v2.vetor[0] - (produto/escalar)*v1.vetor[0];
        resultado1.vetor[1] = v2.vetor[1] - (produto/escalar)*v1.vetor[1];
        resultado1.vetor[2] = v2.vetor[2] - (produto/escalar)*v1.vetor[2];
        contador++;
    }
    else{
        if(contador==2){
            produto = (v1.vetor[0]*v2.vetor[0] + v1.vetor[1]*v2.vetor[1] + v1.vetor[2]*v2.vetor[2]);
            escalar = pow(sqrt(pow(v1.vetor[0],2) + pow(v1.vetor[1],2) + pow(v1.vetor[2],2)),2);
            resultado2.vetor[0] = v2.vetor[0] - (produto/escalar)*v1.vetor[0];
            resultado2.vetor[1] = v2.vetor[1] - (produto/escalar)*v1.vetor[1];
            resultado2.vetor[2] = v2.vetor[2] - (produto/escalar)*v1.vetor[2];
        }
    }
    cout<<"v2:";
    resultado1.showVetor();
    cout<<"v3:";
    resultado2.showVetor();
}

void inicializacaoGramSchimidt(Vetor v1,Vetor v2,Vetor v3){
    cout<<"v1:";
    v1.showVetor();
    gramSchimidt(v1,v2,v3);
}

int main()
{
    Vetor vetor1,vetor2,vetor3;
    vetor1.inicializaVetor();
    vetor2.inicializaVetor();
    vetor3.inicializaVetor();
    inicializacaoGramSchimidt(vetor1,vetor2,vetor3);
    //vetor1.showVetor();
    //vetor2.showVetor();
    //vetor3.showVetor();
    return 0;
}
