#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

typedef struct
{
    char r;
    char g;
    char b;
} charRGB;

static rgb   hsv2rgb(hsv in);



void Mandelbrot(float dx, float dy, float x1, float y1, int width, int height, int maxIters, int * table, int roundedWidth);
void usage(char * programName);


int main(int argc, char **argv)
{

    if(argc < 8)
    {
        usage(argv[0]);
    }

    const char * patternFile = "128.bmp";
    const char * resultFileName = "result.bmp";
    FILE * file, *buffer;
    float x1, y1, x2, y2, dx, dy;
    int width, height, roundedWidth, maxIters;
    int * iterationValues;
	char * pixelColors;

    //Convert input parameters
    x1 = atof(argv[1]);
    y1 = atof(argv[2]);
    x2 = atof(argv[3]);
    y2 = atof(argv[4]);
    width = atoi(argv[5]);
    height = atoi(argv[6]);
    maxIters = atoi(argv[7]);

    dx = (x2 - x1)/width;
    dy = (y2 - y1)/height;
    roundedWidth = (width+7) & ~7UL;

    //Prepare color table - it will be used to map iteration values to colors
    rgb * colors = malloc(maxIters * sizeof(rgb));
    charRGB * charColors = malloc(maxIters * sizeof(charRGB));

    int k;
    for(k = 0; k < maxIters; ++k)
    {
        hsv in;
        in.h = k*360.0/maxIters;
        in.s = 1.0;
        in.v = k/(k+8.0);
        colors[k] = hsv2rgb(in);
    }

    for(k = 0; k < maxIters; ++k)
    {
        double r = colors[k].r*255;
        double g = colors[k].g*255;
        double b = colors[k].b*255;
        charColors[k].r = (char)r;
        charColors[k].g = (char)g;
        charColors[k].b = (char)b;
    }



    //Open files
    if((file = fopen(patternFile, "rb")) == NULL)
    {
        fprintf(stderr, "Blad otwarcia pliku\n");
        exit(1);
    }

    if((buffer = fopen("result.bmp", "wb")) == NULL)
    {
        fprintf(stderr, "Blad otwarcia pliku\n");
        exit(1);
    }

    //Copyt headers of pattern file
    int sizeOfPattern;
    int offset;
    fseek(file, 2L, SEEK_SET);
    fread(&sizeOfPattern, sizeof(int),1,file);
    fseek(file, 4L, SEEK_CUR);
    fread(&offset, sizeof(int),1,file);
    rewind(file);

    char * fileCopy = (char *)malloc(sizeOfPattern);

    fread(fileCopy, sizeof(char), sizeOfPattern, file);
    fwrite(fileCopy, sizeof(char), sizeOfPattern, buffer);
    rewind(file);
    rewind(buffer);

    //Compute information about new picture
    int padding;
    if(width % 4 == 1)
        padding = 3;
    else if(width % 4 == 2)
        padding = 2;
    else if(width % 4 == 3)
        padding = 1;
    else
        padding = 0;

    int width_in_bytes = 3*width + padding;
    int sizeOfTable = width_in_bytes* height;
    int sizeOfFile = offset + sizeOfTable;

    iterationValues = (int *)malloc(sizeOfTable*sizeof(int));
	pixelColors = (char*)malloc(sizeOfTable*sizeof(char));

    //Write new size, width, heigth and size of table with pictures to new image
    fseek(buffer, 2L, SEEK_SET);
    fwrite(&sizeOfFile, sizeof(int), 1, buffer);
    fseek(buffer, 12L, SEEK_CUR);
    fwrite(&width, sizeof(int),1,buffer);
    fwrite(&height, sizeof(int),1,buffer);
    fseek(buffer, 8L, SEEK_CUR);
    fwrite(&sizeOfTable, sizeof(int),1,buffer);
    rewind(buffer);


    fseek(file, (long) offset, SEEK_SET);
    fseek(buffer, (long) offset, SEEK_SET);

    //Compute iteration values
    Mandelbrot(dx, dy, x1, y1, width, height, maxIters, iterationValues, roundedWidth);


    //Fill the piksel table with appropriate clor values
    int i, j;
	int m = 0;
    for(i = 0; i < height; ++i)
    {
        for(j = 0; j < width; ++j)
        {

            int o = iterationValues[i*height + j];
			pixelColors[m] = charColors[o].b;
			pixelColors[++m] = charColors[o].g;
			pixelColors[++m] = charColors[o].r;
            ++m;
        }
        m += padding;
    }

	fwrite(pixelColors, sizeof(char), sizeOfTable, buffer);

    printf("The image was saved as %s\n", resultFileName);


    free(iterationValues);
	free(pixelColors);
    free(fileCopy);
    free(charColors);
    free(colors);
    fclose(buffer);
    fclose(file);

    return 0;
}


void usage(char * programName)
{
    printf("Usage:\n%s x1 y1 x2 y2 width, heigth, maxIterations\n", programName);
    exit(-1);
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0)
    {
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i)
    {
        case 0:
            out.r = in.v;
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = in.v;
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = in.v;
            out.b = t;
            break;

        case 3:
            out.r = p;
            out.g = q;
            out.b = in.v;
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = in.v;
            break;
        case 5:
        default:
            out.r = in.v;
            out.g = p;
            out.b = q;
            break;
    }
    return out;
}
