#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"

int lcm(int a,int b){
    static int m = 0;
    m += a;
    if(m%b == 0)
        return m;
    return lcm(a,b);
}

int main(){
    IMAGE IMG;
    FILE * fpi;

    char imageIn[] = {"img.bmp"};
    char imageOut[] = {"new.bmp"};

    int size = sizeImage(fpi,imageIn);
    unsigned char buffer[size][3];

    restore:
    readImage(fpi,imageIn,&IMG,buffer);

    FILE * fpo;
    writeImage(fpo,imageOut,&IMG,buffer);

    int r,c,choice,factor,a,flag = 0;
    char horv,* image;

    while(1){
        printf("\n****** IMAGE PROCESSING ******\n\n");
        printf(" 1. Details of Image\n");
        printf(" 2. Pixel Value at (r,c)\n");
        printf(" 3. Set Pixel Value at (r,c)\n");
        printf(" 4. Convert to Gray Image\n");
        printf(" 5. Mean Gray-level Value\n");
        printf(" 6. Convert Gray to Binary\n");
        printf(" 7. Crop Image\n");
        printf(" 8. Reflect Image\n");
        printf(" 9. Filter\n");
        printf("10. Zoom In by Factor s\n");
        printf("11. Zoom Out by Factor s\n");
        printf("12. Translate Image by t\n");
        printf("13. Rotate by Angle %c\n",233);
        printf("14. Morph 2 Images\n");
        printf("15. Detection Change\n");
        printf("16. Negative Image\n");
        printf("17. RESTORE IMAGE\n");
        printf(" 0. EXIT\n");
        printf("\n******************************\n");
        printf("\nEnter choice : ");
        int ch;
        scanf("%d",&ch);
        printf("\n******************************\n");
        switch(ch){
        case 1:
            getImageDetail(imageIn,&IMG);
            break;
        case 2:
            printf("Rows\t: ");
            scanf("%d",&r);
            printf("Column\t: ");
            scanf("%d",&c);
            getPixelVal(r,c,&IMG,buffer);
            break;
        case 3:
            printf("Rows\t: ");
            scanf("%d",&r);
            printf("Column\t: ");
            scanf("%d",&c);
            setPixelVal(r,c,&IMG,imageOut,buffer);
            flag = 1;
            break;
        case 4:
            colorToGray(imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 5:
            meanGray(&IMG,buffer);
            break;
        case 6:
            grayToBinary(imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 7:
            cropImage(imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 8:
            printf("Reflection : H or V\nEnter choice : ");
            fflush(stdin);
            scanf("%c",&horv);
            flag = reflectImage(horv,imageOut,&IMG,buffer);
            break;
        case 9:
            flag = filterImage(imageOut,&IMG,buffer);
            break;
        case 10:
            printf("Enter factor : ");
            scanf("%d",&factor);
            enlargeImage(factor,imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 11:
            printf("Enter factor : ");
            scanf("%d",&factor);
            shrinkImage(factor,imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 12:
            printf("Enter amount : ");
            scanf("%d",&factor);
            translateImage(factor,imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 13:
            printf("Enter angle : ");
            scanf("%d",&a);
            rotateImage(a,imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 14:
            printf("Enter image file : ");
            scanf("%s",image);
            fflush(stdin);
            morphImage(imageOut,image,&IMG,buffer);
            flag = 1;
            break;
        case 15:
            printf("Enter image file : ");
            scanf("%s",image);
            fflush(stdin);
            detectImage(imageOut,image,&IMG,buffer);
            flag = 1;
            break;
        case 16:
            negativeImage(imageOut,&IMG,buffer);
            flag = 1;
            break;
        case 17:
            system("cls");
            goto restore;
            break;
        case 0:
            exit(0);
        default:
            printf("\nINVALID CHOICE!!");
        }
        if(flag)
            printf("\n...DONE...");
        getche();
        flag = 0;
        system("cls");
    }
    return 0;
}
