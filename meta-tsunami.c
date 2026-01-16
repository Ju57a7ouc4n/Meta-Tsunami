#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *archOrig;
    FILE *archNew;
    unsigned char buffer1, buffer2;
    unsigned char lenBytes[2];
    unsigned short length;

    if (argc >= 2) {
        archOrig = fopen(argv[1], "rb");
        archNew = fopen("cleaned_image.jpg", "wb");
        if (archOrig != NULL && archNew != NULL) {
            if (strstr(argv[1], ".jpg") != NULL || strstr(argv[1], ".jpeg") != NULL) {
                printf("Woshh!!!\n");
                fread(&buffer1, 1, 1, archOrig);
                fread(&buffer2, 1, 1, archOrig);
                if (buffer1 == 0xFF && buffer2 == 0xD8) {
                    fwrite(&buffer1, 1, 1, archNew);
                    fwrite(&buffer2, 1, 1, archNew);
                } else {
                    printf("Invalid JPEG Header!\n");
                    return 1;
                }
                while (fread(&buffer1, 1, 1, archOrig) == 1) {
                    if (buffer1 == 0xFF) {
                        fread(&buffer2, 1, 1, archOrig);
                        if (buffer2 == 0xE1 || buffer2 == 0xE2 || buffer2 == 0xED || buffer2 == 0xFE || buffer2 == 0xE0) {
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
            } else {
                printf("Format Not Supported! \n");
            }
        } else {
            printf("There Was A Problem Opening Files! \n");
        }
    } else {
        printf("Uso: %s imagen.jpg\n", argv[0]);
    }
    return 0;
}

