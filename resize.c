#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "Please run program with three require arguments\n");
        return 1;
    }
    if(atoi(argv[1]) > 100 || atoi(argv[1]) < 1){
        fprintf(stderr, "Please provide a valid(>1, <= 100) positive resize factor.\n");
        return 2;
    }

    int factor = atoi(argv[1]);
    char *inFile = argv[2];
    char *outFile = argv[3];

    FILE *iFile = fopen(inFile, "r");

    if (iFile == NULL){
        fprintf(stderr, "Input file not valid, Please provide a valid file.\n");
        return 3;
    }

    BITMAPFILEHEADER bfh;
    fread(&bfh, sizeof(BITMAPFILEHEADER), 1, iFile);
    if (bfh.bfType != 19778){
        fprintf(stderr, "This is not a valid BMP file.\n");
        return 4;
    }

    BITMAPINFOHEADER bfih;
    fread(&bfih, sizeof(BITMAPINFOHEADER), 1, iFile);
    if(bfih.biBitCount != 24){
        fprintf(stderr, "This is not a 24Bit BMP File.\n");
        return 5;
    }else if (bfih.biCompression != 0){
        fprintf(stderr, "This is not an uncompressed BMP file.\n");
        return 5;
    }

    FILE *oFile = fopen(outFile, "w+");
    if (oFile == NULL){
        fprintf(stderr, "Output file cannot be created.\n");
        return 6;
    }
    int initPadding = (4 - (bfih.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int oldWidth = bfih.biWidth;
    int oldHeight = abs(bfih.biHeight);

    bfih.biWidth *= factor;
    bfih.biHeight = (abs(bfih.biHeight) * factor) * -1;
    int upPadding = (4 - (bfih.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bfih.biSizeImage = ((bfih.biHeight * 1) * bfih.biWidth) + upPadding;
    bfh.bfSize = bfih.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, oFile);
    fwrite(&bfih, sizeof(BITMAPINFOHEADER), 1, oFile);

    RGBTRIPLE *buffer = malloc(sizeof(RGBTRIPLE) * bfih.biWidth);

    for(int i = 0, biH = oldHeight; i < biH; i++){

        int element = 0;

        for(int j = 0, biW = oldWidth; j < biW; j++){

            RGBTRIPLE line;

            fread(&line, sizeof(RGBTRIPLE),1, iFile);

            for(int l = 0; l < factor; l++){

                buffer[element] = line;
                element++;
            }

        }

        fseek(iFile, initPadding, SEEK_CUR);

        for(int p = 0; p < factor; p++){

            fwrite(buffer, sizeof(RGBTRIPLE), bfih.biWidth, oFile);

            for(int k = 0; k < upPadding; k++){

                fputc(0x00, oFile);

            }
        }

    }

    free(buffer);

    fclose(oFile);

}