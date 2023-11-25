#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qnt_customers(FILE *arquivo);

int qnt_argumentos(FILE *arquivo);

int qnt_request(FILE *arquivo);

int qnt_recursos(FILE *arquivo);

void format_tabela(int arg, FILE *resultado);

void format_titulo(int arg, FILE *resultado);

void format_titulo2(int arg, FILE *resultado);

void format_tabela2(int arg, FILE *resultado);

int deadlock(int num_resourses, int num_customer, int *available, int need[num_customer][num_resourses], int allocation[num_customer][num_resourses]);

int main (int argc, char *argv[]){

    int tamanho = argc -1;
    int avaliable[tamanho];

    for(int i = 1; i < tamanho + 1; i++){
        avaliable[i-1] = atoi(argv[i]);
    }
    printf("");

    FILE *comandos1 = fopen("commands.txt", "r");
    if (comandos1 == NULL){
        printf("Fail to read commands.txt\n");
        return 0;
    }
    fclose(comandos1);

     

    FILE *arquivo;
    arquivo = fopen("customer.txt", "r");

    if (arquivo == NULL){
        printf("Fail to read customer.txt");
        return 0;
    }

     

    int n_clientes = qnt_customers(arquivo);
    fseek(arquivo, 0 , SEEK_SET);
    int n_argumentos = qnt_argumentos(arquivo);
    fseek(arquivo, 0 , SEEK_SET);

    int maximun[n_clientes][n_argumentos];
    int allocation[n_clientes][n_argumentos];
    int need[n_clientes][n_argumentos];

     

    // inserindo clientes
    // Lê os valores dos clientes do arquivo
    char linha_buffer[1000];

    for (int i = 0; i < n_clientes && fgets(linha_buffer, sizeof(linha_buffer), arquivo); i++) {
        // Separa os números pela vírgula
        char* token = strtok(linha_buffer, ",");
        for (int j = 0; token != NULL && j < n_argumentos; j++) {
            maximun[i][j] = atoi(token);
            allocation[i][j] = 0;
            token = strtok(NULL, ",");
        }
    }

     

    for(int i = 0; i<n_clientes; i++){
        for (int j = 0; j < n_argumentos; j++) {
            need[i][j] = maximun[i][j];
        }
    }
     
    fclose(arquivo);
     
    char requisicao[1000];
    FILE *comandos ;
    comandos = fopen("commands.txt", "r");
    int n_request = qnt_request(comandos);
    fseek(comandos, 0 , SEEK_SET);
    int n_recursos = qnt_recursos(comandos) - 2;
    fseek(comandos, 0 , SEEK_SET);
     

    //printf(" A %d ,R %d , T %d \n", n_argumentos, n_recursos, tamanho);

    if(n_argumentos!=tamanho){
        printf("Incompatibility between customer.txt and command line");
        return 0;
    }
     
    if(n_recursos!=tamanho){
        printf("Incompatibility between commands.txt and command line");
        return 0;
    }

    FILE* resultado = fopen("result.txt", "w");

    for(int l = 0; l< n_request; l++) {

        char* token2 = NULL;
        char* op = NULL;

        fgets(requisicao, 1000, comandos);

        if(strstr(requisicao, "*" ) != NULL){
            token2 = strtok(requisicao, " ");
            op = "*";
        }else{
            token2 = strtok(requisicao, " ");
            op = token2;
            token2 = strtok(NULL, " ");
        }

        char* processo = token2;
        token2 = strtok(NULL, " ");

        // Salva os itens do terceiro item em diante em um array de números
        int numeros[n_argumentos+1];

        int t = 0;
        while(token2 != NULL){
            numeros[t] = atoi(token2);
            token2 = strtok(NULL, " ");
            t++;
        }

        int c = atoi(processo);
        
        if(strcmp(op, "*") == 0){
            // Imprime as matrizes
            fprintf(resultado ,"MAXIMUM");
            format_titulo(n_argumentos, resultado);
            fprintf(resultado ," | ALLOCATION ");
            format_titulo2(n_argumentos, resultado);
            fprintf(resultado ,"| NEED\n");

            for (int i = 0; i < n_clientes; i++) {
                int trava = 0;

                for (int j = 0; j < n_argumentos; j++) {

                    fprintf(resultado ,"%d ", maximun[i][j]);
                    if(maximun[i][j]>=10){
                        trava=1;
                    }
                }
                format_tabela(n_argumentos, resultado);
                if(trava == 1){
                    fprintf(resultado ," |");
                    trava =0;
                }else{
                    fprintf(resultado ,"  |");
                }
                for (int j = 0; j < n_argumentos; j++) {
                    fprintf(resultado ," %d", allocation[i][j]);
                    if(allocation[i][j]>=10){
                        trava=1;
                    }
                }
                format_tabela(n_argumentos, resultado);
                format_tabela2(n_argumentos, resultado);
                if(trava == 1){
                    fprintf(resultado ," |");
                    trava =0;
                }else{
                    fprintf(resultado ,"  |");
                }
                for (int j = 0; j < n_argumentos; j++) {
                    fprintf(resultado ," %d", need[i][j]);
                }
                fprintf(resultado ," \n");
            }
            fprintf(resultado ,"AVAILABLE ");
            for (int j = 0; j < n_argumentos; j++) {
                fprintf(resultado ,"%d ", avaliable[j]);
            }
            fprintf(resultado ,"\n");
        }else if(strcmp(op, "RQ") == 0){
            //request(numeros, processo, &allocation, &need, avaliable, n_argumentos);
            int passou = 0;

            for(int i = 0; i < n_argumentos; i++){
   
                if(numeros[i] > need[c][i]){   

                    fprintf(resultado ,"The customer %d request ", c);
                    for(int p = 0; p< n_argumentos;p++){
                        fprintf(resultado ,"%d ", numeros[p]);
                    }
                    fprintf(resultado ,"was denied because exceed its maximum need\n");
                    passou = 1;
                    break;
                }else if(numeros[i] > avaliable[i]){
                    fprintf(resultado ,"The resources ");
                    for(int p = 0; p< n_argumentos;p++){
                        fprintf(resultado ,"%d ", avaliable[p]);
                    }
                    fprintf(resultado ,"are not enough to customer %d request ", c);
                    for(int p = 0; p< n_argumentos;p++){
                        fprintf(resultado ,"%d ", numeros[p]);
                    }
                    fprintf(resultado ,"\n");
                    passou = 1;
                    break;
                }
            }

            if(passou == 0){
                for(int i = 0; i < n_argumentos; i++){  
                    need[c][i] -= numeros[i];
                    allocation[c][i] += numeros[i];
                    avaliable[i] -= numeros[i];       
                }
            }

            if(deadlock(n_argumentos, n_clientes, avaliable, need, allocation) == 1 && passou == 0){

                fprintf(resultado ,"The customer %d request ", c);
                for(int p = 0; p< n_argumentos;p++){
                    fprintf(resultado ,"%d ", numeros[p]);
                }
                fprintf(resultado ,"was denied because result in an unsafe state\n");
                for(int i = 0; i < n_argumentos; i++){  
                    need[c][i] += numeros[i];
                    allocation[c][i] -= numeros[i];
                    avaliable[i] += numeros[i];       
                }

            }else if(passou == 0){
                fprintf(resultado ,"Allocate to customer %d the resources ", c);
                for(int p = 0; p< n_argumentos;p++){
                    fprintf(resultado ,"%d ", numeros[p]);
                }
                fprintf(resultado ,"\n"); 
            }

        }else if(strcmp(op, "RL") == 0){
            
            int relizou = 0;
            for(int i = 0; i < n_argumentos; i++){
                if(numeros[i]> allocation[c][i]){
                    fprintf(resultado ,"The customer %d released ", c);
                    for(int p = 0; p< n_argumentos;p++){
                        fprintf(resultado ,"%d ", numeros[p]);
                    }  
                    fprintf(resultado ,"was denied because exceed its maximum allocation\n");
                    relizou = 1;
                    break;
                }
        }
        if(relizou == 0){
            fprintf(resultado ,"Release from customer %d the resources ", c);
            for(int p = 0; p< n_argumentos;p++){
                fprintf(resultado ,"%d ", numeros[p]);
            }
            fprintf(resultado ,"\n"); 
            for(int i = 0; i < n_argumentos; i++){   
                need[c][i] += numeros[i];
                allocation[c][i] -= numeros[i];
                avaliable[i] += numeros[i];       
            }  
        } 
        }
    }
    fclose(comandos);
    fclose(resultado);
}

int qnt_customers(FILE *arquivo){

    // Contador de n_clientes
    int n_clientes;
    char linha[1000];

    // Lê o arquivo linha a linha
    while (fgets(linha, 1000, arquivo) != NULL) {
        n_clientes++;
    }

    return n_clientes;
}

int qnt_argumentos(FILE *arquivo){
    char linha_buffer[1000];
    fgets(linha_buffer, sizeof(linha_buffer), arquivo);

    // Contar os números na primeira linha
    int count = 0;
    char *token = strtok(linha_buffer, ",");
    while (token != NULL) {
        count++;
        token = strtok(NULL, ",");
    }

    return count;
}

int qnt_request(FILE *arquivo){

    // Contador de n_request
    int n_request;
    char linha[1000];

    // Lê o arquivo linha a linha
    while (fgets(linha, 1000, arquivo) != NULL) {
        n_request++;
    }

    return n_request;
}

int qnt_recursos(FILE *arquivo){
    char linha_buffer[1000];
    fgets(linha_buffer, sizeof(linha_buffer), arquivo);

    // Contar os números na primeira linha
    int count = 0;
    char *token = strtok(linha_buffer, " ");
    while (token != NULL) {
        count++;
        token = strtok(NULL, " ");
    }

    return count;
}

void format_tabela(int arg, FILE *resultado){
    if( arg < 3){
        for(int i = 1; i < arg; i++){
            fprintf(resultado ," ");
        }
    }
}

void format_tabela2(int arg, FILE *resultado){
    if( arg < 5){
        for(int i = 1; i < arg; i++){
            fprintf(resultado ," ");
        }
    }
    if( arg = 5){
        fprintf(resultado ,"  ");
    }
}

void format_titulo(int arg, FILE *resultado){
    if(arg >3){
        for(int i = 4; i < arg; i++){
            fprintf(resultado ,"  ");
        }
    }
}

void format_titulo2(int arg, FILE *resultado){
    if(arg >5){
        for(int i = 5; i < arg; i++){
            fprintf(resultado ," ");
        }
    }
}

int deadlock(int num_resourses, int num_customer, int *available, int need[num_customer][num_resourses], int allocation[num_customer][num_resourses]){
    int work[num_resourses]; 
    int finish[num_customer]; 

    for(int i = 0; i < num_resourses; i++){ 
        work[i] = available[i];
    }

    for(int i = 0; i < num_customer; i++){
        finish[i] = 0;
    }

    while(1){ 
        int found = 0;

        for(int i = 0; i < num_customer; i++){ 
            if(finish[i] == 0){ 
                int j;
                for(j = 0; j < num_resourses; j++){ 
                    if(need[i][j] > work[j]){ 
                        break;
                    }
                }
                if(j == num_resourses){ 
                    for(j = 0; j < num_resourses; j++){
                        work[j] += allocation[i][j];
                    }
                    finish[i] = 1; 
                    found = 1; 
                }
            }
        }

        if(found == 0){ 
            break;
        }
    }

    for(int i = 0; i < num_customer; i++){ 
        if(finish[i] == 0){ 
            return 1; 
        }
    }
    return 0; 
}
