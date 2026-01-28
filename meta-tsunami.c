#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int limpiar_imagen(const char *filename);

int main(int argc, char *argv[]) {
    int i = 1; 
    if (argc < 2) {
        printf("Uso: %s imagen1.jpg imagen2.jpg ...\n", argv[0]);
        return 1;
    }
    while (i < argc) {
        limpiar_imagen(argv[i]);
        i++;
    }
    return 0;
}

int limpiar_imagen(const char *filename) {
    FILE *archOrig;
    FILE *archNew;
    unsigned char buffer1, buffer2;
    unsigned char lenBytes[2];
    unsigned short length;
    const char *tempFile = "temp_cleaned.jpg";
    archOrig = fopen(filename, "rb");
    archNew = fopen(tempFile, "wb");
    if (archOrig == NULL || archNew == NULL) {
        printf("Error al abrir los archivos para: %s\n", filename);
        if (archOrig) fclose(archOrig);
        return 1;
    }
    if (strstr(filename, ".jpg") == NULL && strstr(filename, ".jpeg") == NULL) {
        printf("Formato no soportado para: %s\n", filename);
        fclose(archOrig);
        fclose(archNew);
        remove(tempFile);
        return 1;
    }
    printf("Procesando %s... Woshh!!!\n", filename);
    if (fread(&buffer1, 1, 1, archOrig) == 1 && fread(&buffer2, 1, 1, archOrig) == 1) {
        if (buffer1 == 0xFF && buffer2 == 0xD8) {
            fwrite(&buffer1, 1, 1, archNew);
            fwrite(&buffer2, 1, 1, archNew);
        } else {
            printf("¡Cabecera JPEG inválida en %s!\n", filename);
            fclose(archOrig); fclose(archNew);
            remove(tempFile);
            return 1;
        }
    }
    while (fread(&buffer1, 1, 1, archOrig) == 1) {
        if (buffer1 == 0xFF) {
            if (fread(&buffer2, 1, 1, archOrig) != 1) break;
            if ((buffer2 >= 0xE0 && buffer2 <= 0xEF) || buffer2 == 0xFE) {
                fread(lenBytes, 1, 2, archOrig);
                length = (lenBytes[0] << 8) | lenBytes[1];
                fseek(archOrig, length - 2, SEEK_CUR);
            } 
            else if (buffer2 == 0xD9) {
                fwrite(&buffer1, 1, 1, archNew);
                fwrite(&buffer2, 1, 1, archNew);
                break; 
            }
            else {
                fwrite(&buffer1, 1, 1, archNew);
                fwrite(&buffer2, 1, 1, archNew);
            }
        } else {
            fwrite(&buffer1, 1, 1, archNew);
        }
    }
    fclose(archOrig);
    fclose(archNew);
    if (remove(filename) != 0) {
        printf("Error al eliminar el archivo original.\n");
        return 1;
    }
    if (rename(tempFile, filename) != 0) {
        printf("Error al renombrar el archivo limpio.\n");
        return 1;
    }
    printf("¡Imagen %s limpiada con éxito!\n", filename);
    return 0;
}