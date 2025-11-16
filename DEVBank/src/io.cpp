#include "io.h"
#include <stdio.h>
#include <string.h>

void ler_linha(char* buffer, size_t tamanho_maximo) {
    if (fgets(buffer, (int)tamanho_maximo, stdin)) {

        buffer[strcspn(buffer, "\n")] = 0;
    }
}