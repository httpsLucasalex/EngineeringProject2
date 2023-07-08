#include <stdio.h>
#include <stdlib.h>
#include "matrizes.h"
#include "pds_telecom.h"
#include <time.h>

int main()
{
//    int Nr = 4,Nt = 4;
//    struct Complex **H,**J,**K;
//    double rmax=0,rmin=0;
//    srand(time(NULL));  // Inicializa a semente do gerador de números aleatórios
//
//
//    H = (struct Complex **)malloc(Nr * sizeof(struct Complex *));
//    J = (struct Complex **)malloc(Nr * sizeof(struct Complex *));
//
//    for (int i = 0; i < Nr; i++)
//    {
//        H[i] = (struct Complex *)malloc(Nt * sizeof(struct Complex));
//        J[i] = (struct Complex *)malloc(Nt * sizeof(struct Complex));
//    }
//
//    H = channel_gen(Nr,H, Nt);
//
//    printf("matriz H:\n");
//
//    for(int i=0; i<Nr; i++){
//        for(int j=0; j<Nt; j++){
//            printf("%.2f + %.2fj\t", H[i][j].real, H[i][j].img);
//        }
//        printf("\n");
//    }
//
//    J = channel_gen(Nr,J, Nt);
//
//    printf("matriz J:\n");
//
//    for(int i=0; i<Nr; i++){
//        for(int j=0; j<Nt; j++){
//            printf("%.2f + %.2fj\t", J[i][j].real, J[i][j].img);
//        }
//        printf("\n");
//    }
//
//    K = channel_transmission(rmax,rmin,J,H,Nr,Nt);
//
//    printf("matriz K:\n");
//
//    for(int i=0; i<Nr; i++){
//        for(int j=0; j<Nt; j++){
//            printf("%.2f + %.2fj\t", K[i][j].real, K[i][j].img);
//        }
//        printf("\n");
//    }
//
//    for (int i = 0; i < Nr; i++){
//            free(H[i]);
//            free(J[i]);
//        }
//        free(H);
//        free(J);
//
        long tamanho, q_bits;
        char* arquivo_txt = "src/matrizes/arquivo.txt";
        char* arquivo_bin = "src/matrizes/arquivo.bin";

        int* vetor_txt = tx_data_read(arquivo_txt, &tamanho);
        rx_data_write(vetor_txt, &q_bits);
        int* vetor_bin = tx_data_read(arquivo_bin, &q_bits);
        printf("\n______Vetor gerado pelo arquivo.txt______\n\n");
        for(int i = 0; i < tamanho; i++)
        {
            printf("%d",vetor_txt[i]);
        }
        printf("\n_________________________________________\n");

        printf("\n______Vetor gerado pelo arquivo.bin______\n\n");
        for(int i = 0; i < q_bits; i++)
        {
            printf("%d",vetor_bin[i]);
        }
        printf("\n_________________________________________\n");

        return 0;
    }

void print_binario(unsigned char byte, int* vet_int, long int* index) {
    for (int i = 6; i >= 0; i -= 2) {
        int num = ((byte >> i) & 1) + ((byte >> (i + 1)) & 1) * 2;
        vet_int[(*index)++] = num;
    }
}

int* tx_data_read(const char* texto_str, long int* tamanho_retornado) {
    FILE* file = fopen(texto_str, "rb");
    if (file == NULL) {
        printf("Erro! O arquivo não pode ser aberto.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long int tamanho = ftell(file);
    rewind(file);
    unsigned char* buffer = (unsigned char*)malloc(tamanho);
    if (buffer == NULL) {
        printf("Erro! Memória não pode ser alocada.\n");
        fclose(file);
        return NULL;
    }

    size_t leitura_bytes = fread(buffer, 1, tamanho, file);
    if (leitura_bytes != tamanho) {
        printf("Erro ao ler o arquivo.\n");
        free(buffer);
        fclose(file);
        return NULL;
    }

    int* vet_int = (int*)malloc((tamanho * 4) * sizeof(int));
    if (vet_int == NULL) {
        printf("Erro! Memória não pode ser alocada.\n");
        free(buffer);
        fclose(file);
        return NULL;
    }

    long int index = 0;
    for (long i = 0; i < tamanho; i++) {
        print_binario(buffer[i-1], vet_int, &index);
    }

    free(buffer);
    fclose(file);

    *tamanho_retornado = (tamanho) * 4;
    return vet_int;
}

void rx_data_write(int* entrada_vet_int, long int *q_bits_retornado) {

    FILE* binario = fopen("src/matrizes/arquivo.bin", "wb");
    long int q_bits = sizeof(entrada_vet_int);
    if (binario == NULL) {
        printf("Erro! Arquivo.bin nao pode ser aberto!\n");
        return;

    } else {
        printf("Arquivo.bin gerado com sucesso.\n");
    }

    for (int i = 0; i < q_bits; i++) {

        unsigned char byte = 0;
        for (int j = 0; j < 4; j++)
        {
            unsigned int bit = entrada_vet_int[(i * 4) + j];
            byte |= (bit << (2 * j));
        }
        fwrite(&byte, sizeof(byte), 1, binario);
    }

    fclose(binario);
    *q_bits_retornado = (q_bits) * 4;
}

struct Complex *tx_qam_mapper(int* indice, int size) {
    struct Complex *symbol;

    symbol = (struct Complex*)malloc(size * sizeof( struct Complex ));

    for (int i = 0; i < size; i++) {
        if (indice[i] == 0){
            symbol[i].real = -1;
            symbol[i].img = 1;
        }
        else if (indice[i] == 1){
            symbol[i].real = -1;
            symbol[i].img = -1;
        }
        else if (indice[i] == 2){
            symbol[i].real = 1;
            symbol[i].img = 1;
        }
        else if (indice[i] == 2){
            symbol[i].real = 1;
            symbol[i].img = -1;
        }
    }

    return symbol;
}

int *rx_qam_demapper(struct Complex * symbol,int size){

    int *indice;

    indice = (int*)malloc(size * sizeof( int ));

    for (int i = 0; i < size; i++) {
        if (symbol[i].real == -1 && symbol[i].img == 1){
            indice[i] = 0;
        }
        else if (symbol[i].real == -1 && symbol[i].img == -1){
            indice[i] = 1;
        }
        else if (symbol[i].real == 1 && symbol[i].img == 1){
            indice[i] = 2;
        }
        else if (symbol[i].real == 1 && symbol[i].img == -1){
            indice[i] = 3;
        }
    }

    return indice;
}

struct Complex *rx_layer_demapper(int a, struct Complex *s_mapped,struct Complex *s, int Nstreams){

    // Loop para percorrer os símbolos de entrada
    for (int i = 0; i < Nstreams; i++) {
        // Mapeia o símbolo QAM para a stream correspondente
        s[(a * Nstreams) + i].real = s_mapped[i].real;
        s[(a * Nstreams) + i].img = s_mapped[i].img ;
    }
    return s;
}

struct Complex *tx_layer_mapper(int a, struct Complex *s,struct Complex *s_mapped, int Nstreams) {

    // Loop para percorrer os símbolos de entrada
    for (int i = 0; i < Nstreams; i++) {
        // Mapeia o símbolo QAM para a stream correspondente
        s_mapped[i].real = s[a * Nstreams + i].real;
        s_mapped[i].img = s[a * Nstreams + i].img;
    }
    return s_mapped;
}

struct Complex **channel_gen(int Nr,struct Complex **H, int Nt) {


    for (int i = 0; i < Nr; i++) {
        for (int j = 0; j < Nt; j++) {
            H[i][j].real = ((double)rand() / RAND_MAX) * 2 - 1;  // Números aleatórios entre -1 e 1
            H[i][j].img = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }

    return H;
}

struct Complex **channel_transmission(double rmax, double rmin, struct Complex **mtx_cod, struct Complex **H, int Nr, int Nt){
    struct Complex **rmtx;

    rmtx = produto_matricial( mtx_cod, H, Nr, Nt);

    for (int i = 0; i < Nr; i++) {
        for (int j = 0; j < Nt; j++) {
            rmtx[i][j].real = rmtx[i][j].real + ((double)rand() / RAND_MAX) * (rmax - rmin) + rmin;
            rmtx[i][j].img = rmtx[i][j].img + ((double)rand() / RAND_MAX) * (rmax - rmin) + rmin;
        }
    }

    return rmtx;

}
