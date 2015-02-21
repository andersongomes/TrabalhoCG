#include <stdio.h>
#include <stdlib.h>
void openFile(){
    int x, y, z;
    FILE *entrada;
    FILE *saida;

    //r = read , w = write
    entrada = fopen("in.txt", "r");
    saida = fopen("out.txt", "w");
    if(saida != NULL){
        fprintf(saida, "Teste");
    }else{
        printf("Houve um problema ao tentar abrir o arquivo!");
    }

    if(entrada == NULL){
        printf("Houve um problema ao tentar abrir o arquivo!");
    } else {
        while(fscanf(entrada ,"%d %d %d", &x ,&y, &z) != EOF){
             fprintf(saida, "%d %d %d\n", x, y, z);
        }
    }

    fclose(entrada);
    fclose(saida);
}

