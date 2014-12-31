#include<iostream>

using namespace std;

void openFile(){
    int x, y, z;
    FILE *entrada;
    FILE *saida;
    
    //r = read , w = write
    entrada = fopen("in.txt", "r");
    if(entrada == NULL){
        printf("Houve um problema ao tentar abrir o arquivo!");
    } else {
        while(fscanf(entrada ,"%d %d %d", &x ,&y, &z) != EOF){
             printf("%d %d %d\n", x, y, z);                 
        }
    }
    
    fclose(entrada);
}
