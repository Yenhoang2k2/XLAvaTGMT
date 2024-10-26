#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
int Gy[3][3] = {{-1, -2, -1}, {0,  0,  0}, {1,  2,  1}};
int LoG[5][5] = {
    {0, 0, -1, 0, 0},
    {0, -1, -2, -1, 0},
    {-1, -2, 16, -2, -1},
    {0, -1, -2, -1, 0},
    {0, 0, -1, 0, 0}
};

// Hàm d?c ?nh BMP xám
int** loadImage(char *filename, int *width, int *height) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // BMP header

    *width = *(int*)&info[18];
    *height = *(int*)&info[22];

    int **image = (int**)malloc(*height * sizeof(int*));
    for (int i = 0; i < *height; i++) {
        image[i] = (int*)malloc(*width * sizeof(int));
    }

    for (int i = 0; i < *height; i++) {
        for (int j = 0; j < *width; j++) {
            unsigned char pixel;
            fread(&pixel, sizeof(unsigned char), 1, f);
            image[i][j] = (int)pixel;
        }
    }

    fclose(f);
    return image;
}

// Hàm ghi ?nh xám BMP
void saveImage(char *filename, int **image, int width, int height) {
    FILE *f = fopen(filename, "wb");
    if (!f) return;

    unsigned char bmpPad[3] = {0, 0, 0};
    int paddingAmount = ((4 - (width % 4)) % 4);

    unsigned char fileHeader[14] = {
        'B', 'M', 0,0,0,0, 0,0, 0,0, 54,0,0,0
    };
    unsigned char infoHeader[40] = {
        40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 8,0
    };

    int fileSize = 54 + (width * height) + (paddingAmount * height);

    fileHeader[ 2] = fileSize;
    fileHeader[ 3] = fileSize >> 8;
    fileHeader[ 4] = fileSize >> 16;
    fileHeader[ 5] = fileSize >> 24;

    infoHeader[ 4] = width;
    infoHeader[ 5] = width >> 8;
    infoHeader[ 6] = width >> 16;
    infoHeader[ 7] = width >> 24;
    infoHeader[ 8] = height;
    infoHeader[ 9] = height >> 8;
    infoHeader[10] = height >> 16;
    infoHeader[11] = height >> 24;

    fwrite(fileHeader, 1, 14, f);
    fwrite(infoHeader, 1, 40, f);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char pixel = image[i][j] > 255 ? 255 : (image[i][j] < 0 ? 0 : image[i][j]);
            fwrite(&pixel, sizeof(unsigned char), 1, f);
        }
        fwrite(bmpPad, sizeof(unsigned char), paddingAmount, f);
    }

    fclose(f);
}

// Hàm áp d?ng Sobel
void applySobel(int **image, int width, int height, int **output) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0, gy = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    gx += image[y + i][x + j] * Gx[i + 1][j + 1];
                    gy += image[y + i][x + j] * Gy[i + 1][j + 1];
                }
            }
            output[y][x] = sqrt(gx * gx + gy * gy);
        }
    }
}

// Hàm áp d?ng LoG
void applyLoG(int **image, int width, int height, int **output) {
    for (int y = 2; y < height - 2; y++) {
        for (int x = 2; x < width - 2; x++) {
            int sum = 0;
            for (int i = -2; i <= 2; i++) {
                for (int j = -2; j <= 2; j++) {
                    sum += image[y + i][x + j] * LoG[i + 2][j + 2];
                }
            }
            output[y][x] = sum;
        }
    }
}

int main() {
    int width, height;
    int **image = loadImage("input.bmp", &width, &height);
    if (image == NULL) {
        printf("Error loading image.\n");
        return 1;
    }

    int **sobel_output = (int**)malloc(height * sizeof(int*));
    int **log_output = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        sobel_output[i] = (int*)malloc(width * sizeof(int));
        log_output[i] = (int*)malloc(width * sizeof(int));
    }

    applySobel(image, width, height, sobel_output);
    applyLoG(image, width, height, log_output);

    saveImage("sobel_output.bmp", sobel_output, width, height);
    saveImage("log_output.bmp", log_output, width, height);

    // Gi?i phóng b? nh?
    for (int i = 0; i < height; i++) {
        free(image[i]);
        free(sobel_output[i]);
        free(log_output[i]);
    }
    free(image);
    free(sobel_output);
    free(log_output);

    printf("Images processed and saved.\n");
    return 0;
}

