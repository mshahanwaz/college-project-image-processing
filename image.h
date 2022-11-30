#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

typedef struct Image{
    int width;
    int height;
    int bitDepth;
    unsigned char header[54];
    unsigned char colorTable[1024];
}IMAGE;

int sizeImage(FILE * fpi,char * image){
    fpi = fopen(image,"rb");
    if(!fpi){
        printf("\nError Opening File : %s\n",image);
        fclose(fpi);
        exit(0);
    }
    unsigned char tempHeader[54];
    fread(tempHeader,sizeof(unsigned char),54,fpi);

    int w = *(int *)&tempHeader[18],h = *(int *)&tempHeader[22];
    int size = w*h;
    fclose(fpi);

    return size;
}

void readImage(FILE * fpi,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    fpi = fopen(image,"rb");
    if(!fpi){
        printf("\nError Opening File : %s\n",image);
        fclose(fpi);
        exit(0);
    }
    fread(IMG->header,sizeof(unsigned char),54,fpi);

    IMG->width = *(int *)&IMG->header[18];
    IMG->height = *(int *)&IMG->header[22];
    IMG->bitDepth = *(int *)&IMG->header[28];

    int size = IMG->height*IMG->width;

    if(IMG->bitDepth <= 8)
        fread(IMG->colorTable,sizeof(unsigned char),1024,fpi);     //Color Table if present

    int i;
    fread(buffer,sizeof(unsigned char),size*3,fpi);
    fclose(fpi);
}

void writeImage(FILE * fpo,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    fpo = fopen(image,"wb");

    fwrite(IMG->header,sizeof(unsigned char),54,fpo);

    if(IMG->bitDepth <= 8)
        fwrite(IMG->colorTable,sizeof(unsigned char),1024,fpo);

    int size = IMG->height*IMG->width;
    int i;

    fwrite(buffer,sizeof(unsigned char),3*size,fpo);
    fclose(fpo);
}

void getImageDetail(char * image,IMAGE * IMG){
    printf("-----------------------------------------\n");
    printf("\t  Details of %s\n",image);
    printf("-----------------------------------------\n");
    printf(" Width\t\t\t\t| %d\n",IMG->width);
    printf(" Height\t\t\t\t| %d\n",IMG->height);
    printf(" No. of Bits per Pixel(bpp)\t| %d\n",IMG->bitDepth);
    printf(" Compression Type\t\t| %d\n",*(int *)&IMG->header[30]);
    printf(" Size of Image Data(in Bytes)\t| %d\n",*(int *)&IMG->header[34]);
    printf(" Horizontal Resolution(in ppm)\t| %d\n",*(int *)&IMG->header[38]);
    printf(" Vertical Resolution(in ppm)\t| %d\n",*(int *)&IMG->header[42]);
    printf(" No. of Colors\t\t\t| %d\n",*(int *)&IMG->header[46]);
    printf(" No. of Important Colors\t| %d\n",*(int *)&IMG->header[50]);
    printf("-----------------------------------------\n");
}

void getPixelVal(int r,int c,IMAGE * IMG,unsigned char buffer[][3]){
    if(IMG->bitDepth <= 8){
        if(r < IMG->height && c < IMG->width)
            printf("Pixel Value at (%d,%d) : %d",r,c,buffer[r*IMG->width + c][2]);
        else{
            printf("\nInvalid coordinates!");
        }
    }
    else{
        if(r < IMG->height && c < IMG->width)
            printf("Pixel Value at (%d,%d)\n R : %d\n G : %d\n B : %d",r,c,buffer[r*IMG->width + c][2],buffer[r*IMG->width + c][1],buffer[r*IMG->width + c][0]);
        else{
            printf("\nInvalid coordinates!");
        }
    }
}

void setPixelVal(int r,int c,IMAGE * IMG,char * image,unsigned char buffer[][3]){
    int pxl,R,G,B;

    if(IMG->bitDepth <= 8){
        if(r < IMG->height && c < IMG->width){
            printf("Enter pixel value : ");
            scanf("%d",&pxl);
            buffer[r*IMG->width + c][2] = pxl;
        }
        else
            printf("\nInvalid coordinates!");
    }
    else{
        if(r < IMG->height && c < IMG->width){
            printf("Enter pixel value\n R : ");
            scanf("%d",&R);
            printf(" G : ");
            scanf("%d",&G);
            printf(" B : ");
            scanf("%d",&B);
            buffer[r*IMG->width + c][2] = R;
            buffer[r*IMG->width + c][1] = G;
            buffer[r*IMG->width + c][0] = B;
        }
        else
            printf("\nInvalid coordinates!");
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

void colorToGray(char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int i,y;
    if(IMG->bitDepth <= 8)
        printf("\nAlready in Gray!");
    else{
        FILE * fpo = fopen(image,"wb");
        fwrite(IMG->header,sizeof(unsigned char),54,fpo);
        int size = IMG->height*IMG->width;
        if(IMG->bitDepth <= 8)
            fwrite(IMG->colorTable,sizeof(unsigned char),1024,fpo);
        for(i=0;i<size;i++){
            y = buffer[i][0]*0.3 + buffer[i][1]*0.59 + buffer[i][2]*0.11;
            buffer[i][0] = buffer[i][1] = buffer[i][2] = y;
            fputc(y,fpo);
            fputc(y,fpo);
            fputc(y,fpo);
        }
        fclose(fpo);
    }
}

void meanGray(IMAGE * IMG,unsigned char buffer[][3]){
    long sum = 0,mean = 0;
    int i,j,size = IMG->width*IMG->height;

    for(i=0;i<size;i++){
        sum += buffer[i][2];
    }
    mean = (sum/size);
    printf("Gray-Level (avg)value : %d",mean);
}

void grayToBinary(char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int thresh = 128,i,j,k,avg;
    for(i=0;i<IMG->height;i++){
        for(j=0;j<IMG->width;j++){
            avg = (*(int *)&buffer[i*IMG->width + j][0]+*(int *)&buffer[i*IMG->width + j][1]+*(int *)&buffer[i*IMG->width + j][2])/3;
            for(k=0;k<3;k++){
                if(avg > thresh)
                    *(int *)&buffer[i*IMG->width + j][k] = 255;
                else
                    *(int *)&buffer[i*IMG->width + j][k] = 0;
            }
        }
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

void cropImage(char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int firstRow,firstCol,lastRow,lastCol,w,h;
    printf("Enter coordinates(r,c) of\nfirst pt.  : ");
    scanf("%d %d",&firstRow,&firstCol);
    printf("second pt. : ");
    scanf("%d %d",&lastRow,&lastCol);

    w = (lastCol - firstCol + 1);
    h = (lastRow - firstRow + 1);
    if(w%4 != 0){
        w -= w%4;
    }
    if(h%4 != 0){
        h -= h%4;
    }
    if(w > IMG->width || h > IMG->height){
        printf("Size exceeds!!");
        getche();
        return;
    }
    int i,j,k,size = w*h;

    *(int *)&IMG->header[18] = w;      //width
    *(int *)&IMG->header[22] = h;      //height
    *(int *)&IMG->header[34] = size*3; //size of memory

    unsigned char bufferNew[size][3];

    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            for(k=0;k<3;k++){
                bufferNew[i*w + j][k] = buffer[(i + firstRow)*IMG->width + (j + firstCol)][k];
                buffer[i*w + j][k] = bufferNew[i*w + j][k];
            }
        }
    }
    IMG->width = w;
    IMG->height = h;

    FILE * fpo;
    writeImage(fpo,image,IMG,bufferNew);
}

int reflectImage(char c,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    if(c == 'h' || c == 'H'){
        int i,j,k,temp;
        for(i=0;i<IMG->height;i++){
            for(j=0;j<IMG->width/2;j++){
                for(k=0;k<3;k++){
                    temp = buffer[i*IMG->width + j][k];
                    buffer[i*IMG->width + j][k] = buffer[i*IMG->width + IMG->width - j - 1][k];
                    buffer[i*IMG->width + IMG->width - j - 1][k] = temp;
                }
            }
        }
        FILE * fpo;
        writeImage(fpo,image,IMG,buffer);
    }
    else if(c == 'v' || c == 'V'){
        int i,j,k,temp;
        for(i=0;i<IMG->height/2;i++){
            for(j=0;j<IMG->width;j++){
                for(k=0;k<3;k++){
                    temp = buffer[i*IMG->width + j][k];
                    buffer[i*IMG->width + j][k] = buffer[(IMG->height - i - 1)*IMG->width + j][k];
                    buffer[(IMG->height - i - 1)*IMG->width + j][k] = temp;
                }
            }
        }
        FILE * fpo;
        writeImage(fpo,image,IMG,buffer);
    }
    else{
        printf("Invalid Input!");
        return 0;
    }
    return 1;
}

int filterImage(char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int x,y,i,j,k,n,sum[3],ch,z;
    n = 5;
    int a[n*n][3];
    float prod[3],b[n*n][3];

    printf(" FILTER #1 : Blur\n FILTER #2 : Motion Blur\n FILTER #3 : Sharpen\nEnter choice : ");
    scanf("%d",&ch);

    if(ch == 1){
        for(y=n/2;y<IMG->height-n/2;y++){
            for(x=n/2;x<IMG->width-n/2;x++){
                for(k=0;k<3;k++)
                    sum[k] = 0;
                for(j=-n/2;j<=n/2;j++){
                    for(i=-n/2;i<=n/2;i++){
                        for(k=0;k<3;k++){
                            sum[k] += buffer[(y+j)*IMG->width + (x+i)][k];
                        }
                    }
                }
                for(k=0;k<3;k++){
                    buffer[y*IMG->width + x][k] = sum[k]/n/n;
                }
            }
        }
    }
    else if(ch == 2){
        int p = 9;

        double filter[9][9] =
        {
            1, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 1,
        };
        double factor = 1.0/9.0;
        double bias = 0.0;

        long w = IMG->height, h = IMG->width;
        int filterX,filterY;
        for(int x = 0; x < w; x++)
            for(int y = 0; y < h; y++){

                double red = 0.0, green = 0.0, blue = 0.0;

                for(filterY = 0; filterY < p; filterY++)
                    for(filterX = 0; filterX < p; filterX++)
                    {

                        int imageX = (x - p/2 + filterX + w) % w;
                        int imageY = (y - p/2 + filterY + h) % h;

                        red += buffer[imageY * w + imageX][0] * filter[filterY][filterX];
                        green += buffer[imageY * w + imageX][1] * filter[filterY][filterX];
                        blue += buffer[imageY * w + imageX][2] * filter[filterY][filterX];
                    }
                    if(abs(factor*red + bias)>255){
                        buffer[y * w + x][0] = 255;
                    }else{
                        buffer[y * w + x][0] = abs(factor*red + bias);
                    }
                    if(abs(factor*green + bias)>255){
                        buffer[y * w + x][1] = 255;
                    }else{
                        buffer[y * w + x][1] = abs(factor*green + bias);
                    }
                    if(abs(factor*blue + bias)>255){
                        buffer[y * w + x][2] = 255;
                    }else{
                        buffer[y * w + x][2] = abs(factor*blue + bias);
                    }
            }
    }else if(ch == 3){
        int p = 5;

        double filter[5][5] =
        {
            -1, -1, -1, -1, -1,
            -1,  2,  2,  2, -1,
            -1,  2,  8,  2, -1,
            -1,  2,  2,  2, -1,
            -1, -1, -1, -1, -1,
        };
        double factor = 1.0/8.0;
        double bias = 0.0;

        long w = IMG->height, h = IMG->width;
        int filterX,filterY;
        for(int x = 0; x < w; x++)
            for(int y = 0; y < h; y++){

                double red = 0.0, green = 0.0, blue = 0.0;

                for(filterY = 0; filterY < p; filterY++)
                    for(filterX = 0; filterX < p; filterX++)
                    {

                        int imageX = (x - p/2 + filterX + w) % w;
                        int imageY = (y - p/2 + filterY + h) % h;

                        red += buffer[imageY * w + imageX][0] * filter[filterY][filterX];
                        green += buffer[imageY * w + imageX][1] * filter[filterY][filterX];
                        blue += buffer[imageY * w + imageX][2] * filter[filterY][filterX];
                    }
                    if(abs(factor*red + bias)>255){
                        buffer[y * w + x][0] = 255;
                    }else{
                        buffer[y * w + x][0] = abs(factor*red + bias);
                    }
                    if(abs(factor*green + bias)>255){
                        buffer[y * w + x][1] = 255;
                    }else{
                        buffer[y * w + x][1] = abs(factor*green + bias);
                    }
                    if(abs(factor*blue + bias)>255){
                        buffer[y * w + x][2] = 255;
                    }else{
                        buffer[y * w + x][2] = abs(factor*blue + bias);
                    }
            }
    }else{
        printf("Invalid Choice!");
        return 0;
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
    return 1;
}

void enlargeImage(int factor,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int h,w,i,j,k,m=0,n=0,p,q;
    int x,y;
    printf("Enter initial (r,c) : ");
    scanf("%d %d",&x,&y);

    h = IMG->height - IMG->height%lcm(factor,4);
    w = IMG->width - IMG->width%lcm(factor,4);

    int size = h*w;
    unsigned char bufferNew[size][3];
    for(i=x;i<x+h/factor;i++){
        for(j=y;j<y+w/factor;j++){
            for(p=0;p<factor;p++){
                for(q=0;q<factor;q++){
                    for(k=0;k<3;k++)
                        bufferNew[(m+p)*w + (n+q)][k] = buffer[i*IMG->width + j][k];
                }
            }
            n += factor;
        }
        m += factor;
        n = 0;
    }
    for(i=0;i<size;i++){
        for(j=0;j<3;j++){
            buffer[i][j] = bufferNew[i][j];
        }
    }
    *(int *)&IMG->header[18] = w;
    *(int *)&IMG->header[22] = h;
    *(int *)&IMG->header[34] = size*3;
    IMG->width = w;
    IMG->height = h;

    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

void shrinkImage(int factor,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int h,h1,w,w1,i,j,k,m=0,n=0,p,q;

    h = IMG->height - IMG->height%lcm(factor,4);
    w = IMG->width - IMG->width%lcm(factor,4);

    int size;
    h1 = h/factor;
    w1 = w/factor;
    size = h1*w1;
    unsigned char bufferNew[size][3];
    for(i=0;i<h;i+=factor){
        for(j=0;j<w;j+=factor){
            for(k=0;k<3;k++){
                bufferNew[m*w1 + n][k] = buffer[i*IMG->width + j][k];
            }
            n++;
        }
        m++;
        n = 0;
    }
    for(i=0;i<size;i++){
        for(j=0;j<3;j++){
            buffer[i][j] = bufferNew[i][j];
        }
    }
    *(int *)&IMG->header[18] = w1;
    *(int *)&IMG->header[22] = h1;
    *(int *)&IMG->header[34] = size*3;
    IMG->width = w1;
    IMG->height = h1;

    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

void translateImage(int t,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int h = IMG->height;
    int w = IMG->width;
    int size = h*w,i,j,k;
    unsigned char bufferNew[size][3];
    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            for(k=0;k<3;k++){
                if(i < t || j < t)
                    bufferNew[i*w + j][k] = 0;
                else
                    bufferNew[i*w + j][k] = buffer[(i-t)*w + j-t][k];
            }
        }
    }
    for(i=0;i<size;i++){
        for(j=0;j<3;j++){
            buffer[i][j] = bufferNew[i][j];
        }
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

void rotateImage(int a,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    double x0,y0,angle;
    x0 = IMG->width/2.0;
    y0 = IMG->height/2.0;

    angle = (double)3.14*(a/180.0);

    int h = IMG->height,w = IMG->width,x,y;

    int size = h*w,i,j,k,m=2;
    double cosa = cos(angle),sina = sin(angle);

    unsigned char bufferNew[size][3];

    for(i=0;i<IMG->height;i++){
        for(j=0;j<IMG->width;j++){
            for(k=0;k<3;k++){
                x = (x0 + (double)(j-x0)*cosa - (double)(i-y0)*sina);
                y = (y0 + (double)(i-y0)*cosa + (double)(j-x0)*sina);
                if((x>=0 && x<w) && (y>=0 && y<h))
                    bufferNew[y*w + x][k] = buffer[i*w + j][k];
            }
        }
    }
    while(m--){
        for(i=0;i<h;i++){
            for(j=0;j<w;j++){
                {
                    if((i<h-1) && (j<w-1) && (bufferNew[i*w + j][0] == 0) && (bufferNew[i*w + j][1] == 0) && (bufferNew[i*w + j][2] == 0)){
                        if((bufferNew[(i+1)*w + (j+1)][0] == 0) || (bufferNew[(i+1)*w + (j+1)][1] == 0) || (bufferNew[(i+1)*w + (j+1)][2] == 0))
                            continue;
                        else{
                            for(k=0;k<3;k++)
                                bufferNew[i*w + j][k] = bufferNew[(i)*w + (j+1)][k];
                        }
                    }
                }
            }
        }
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,bufferNew);

    for(i=0;i<size;i++){
        for(j=0;j<3;j++){
            bufferNew[i][j] = 0;
        }
    }
}

void morphImage(char * image,char * image2,IMAGE * IMG,unsigned char buffer[][3]){
    int h = IMG->height,w = IMG->width;
    int size = h*w;
    unsigned char buffer2[size][3];

    FILE * fpi;
    readImage(fpi,image2,IMG,buffer2);

    float t=0.5,t1,t2;
    int i,j,k;

    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            for(k=0;k<3;k++){
                t1 = buffer[i*w + j][k];
                t2 = buffer2[i*w + j][k];
                buffer[i*w + j][k] = (int)(t*t1 + (1-t)*t2);
            }
        }
    }

    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

void detectImage(char * image,char * image2,IMAGE * IMG,unsigned char buffer[][3]){
    int h = IMG->height,w = IMG->width,i,j,k,bl = 0,wh = 255;
    int size = h*w,buf[3],buf2[3],t = 100;
    unsigned char buffer2[size][3];

    FILE * fpi;
    readImage(fpi,image2,IMG,buffer2);

    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            for(k=0;k<3;k++){
                buf[k] = buffer[i*w + j][k];
                buf2[k] = buffer2[i*w + j][k];
                if((abs(buf[0]-buf2[0])>t)){
                    buffer[i*w + j][k] = bl;
                }
                else{
                    buffer[i*w + j][k] = wh;
                }
            }
        }
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

void negativeImage(char * image,IMAGE * IMG,unsigned char buffer[][3]){

    int size = IMG->height*IMG->width;
    int i,j;

    for(i=0;i<size;i++){
        for(j=0;j<3;j++){
            buffer[i][j] = 255 - buffer[i][j];
        }
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
}

#endif // IMAGE_H_INCLUDED
