#include "blursharpenmask.h"
#include "qdebug.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

BlurSharpenMask::BlurSharpenMask(int width, int height)
{
    this->width = width;
    this->height = height;
    filteredImage = new QImage(width, height, QImage::Format_Grayscale8);
    outimg = filteredImage->bits();
}

QImage* BlurSharpenMask::sharpen(const uchar* inimg, int value)
{
    float mask = -(value/4);
    float median = value + 1;
    float kernel[3][3] = { {0, mask, 0},
                           {mask, median, mask},
                           {0, mask, 0}};

    int arr[9] = {0};

    int imageSize = width * height;

    int widthCnt = 0, heightCnt = -1, cnt = 0;
    for(int i = 0; i < imageSize; i ++){
        widthCnt = i % width;
        if(i % width == 0) heightCnt++;

        if(widthCnt==0){
            //LeftUpVertex
            if(heightCnt==0){
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = inimg[widthCnt+1 + (heightCnt*width) ];
                arr[6] = arr[7] = inimg[widthCnt+ ((heightCnt+1)*width)  ];
                arr[8] = inimg[widthCnt+1+((heightCnt+1)*width) ];
            }
            //LeftDownVertex
            else if(heightCnt==height-1){
                arr[0] = arr[1] =inimg[widthCnt+((heightCnt-1)*width) ];
                arr[2] = inimg[widthCnt+1 + ((heightCnt-1)*width) ];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[widthCnt+(heightCnt*width)  ];
                arr[8] = arr[5] = inimg[widthCnt+1 + (heightCnt*width)  ];
            }
            else{
                arr[0] = arr[1] = inimg[widthCnt+( (heightCnt-1)*width)  ];
                arr[2] = inimg[widthCnt+1+( (heightCnt-1)*width)  ];
                arr[3] = arr[4] = inimg[widthCnt+(heightCnt*width) ];
                arr[5] = inimg[widthCnt+1+(heightCnt*width) ];
                arr[6] = arr[7] = inimg[widthCnt+ ( (heightCnt+1)*width)  ];
                arr[8] = inimg[widthCnt+1+( (heightCnt+1)*width)  ];
            }

            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg +(widthCnt+heightCnt*width) ) = LIMIT_UBYTE(sum);
        }

        else if( widthCnt==(width*1 -1) ){
            //RightUpVertex
            if(heightCnt==0){
                arr[0] = arr[3] = inimg[widthCnt-1 + (heightCnt*width)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[widthCnt + (heightCnt*width)  ];
                arr[6] = inimg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[7] = arr[8] = inimg[widthCnt+((heightCnt+1)*width) ];
            }
            //RightDownVertex
            else if(heightCnt==height-1){
                arr[0] = inimg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[1] = arr[2] = inimg[widthCnt-1 +((heightCnt-1)*width)  ];
                arr[3] = arr[6] = inimg[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[widthCnt+(heightCnt*width) ];
            }
            else{
                arr[0] = inimg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[2] = arr[1] = inimg[widthCnt + ((heightCnt-1)*width)  ];
                arr[3] = inimg[widthCnt-1 + (heightCnt*width)  ];
                arr[5] = arr[4] = inimg[widthCnt+(heightCnt*width)  ];
                arr[6] = inimg[widthCnt-1 + ((heightCnt+1)*width)  ];
                arr[8] = arr[7] = inimg[widthCnt+((heightCnt+1)*width)  ];
            }
            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg +(widthCnt+heightCnt*width) ) = LIMIT_UBYTE(sum);
        }
        else if(heightCnt==0){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = arr[3] = inimg[widthCnt-1+(heightCnt*width)  ];
                arr[1] = arr[4] = inimg[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = inimg[widthCnt+1+(heightCnt*width)  ];
                arr[6] = inimg[widthCnt-1+((heightCnt+1)*width)  ];
                arr[7] = inimg[widthCnt+((heightCnt+1)*width)  ];
                arr[8] = inimg[widthCnt+1 + ((heightCnt+1)*width)  ];

                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg +(widthCnt+heightCnt*width) ) = LIMIT_UBYTE(sum);
            }
        }
        else if( heightCnt ==(height -1) ){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = inimg[widthCnt-1+((heightCnt-1)*width) ];
                arr[1] = inimg[widthCnt+((heightCnt-1)*width) ];
                arr[2] = inimg[widthCnt+1+((heightCnt-1)*width) ];
                arr[3] = arr[6] = inimg[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[7] = inimg[widthCnt+(heightCnt*width) ];
                arr[5] = arr[8] = inimg[widthCnt+1+(heightCnt*width) ];
                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg +(widthCnt+heightCnt*width) ) = LIMIT_UBYTE(sum);
            }
        }

        else{
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*inimg[((widthCnt+i*1)+(heightCnt+j)*width) ];
                }
            }
            *(outimg +(widthCnt+heightCnt*width) ) = LIMIT_UBYTE(sum);
        }
    }
    return filteredImage;
}

QImage* BlurSharpenMask::blur3x3(const uchar* inimg, int value) {

    double edge =0.0, mask = 0.0, median = 0.0;

    if (value == -1) {
        edge = 1/28.0; mask = 1/7.0; median = 2/7.0;
    }
    else if (value == -2) {
        mask = edge = median = 1/9.0;
    }

    double kernel[3][3] = { {edge, mask, edge},
                            {mask, median, mask},
                            {edge, mask, edge}};

    unsigned char arr[9] = {0};
    double sum = 0.0;

    int rowSize = width ;
    int imageSize = rowSize * height;

    int widthCnt = 0, heightCnt = -1, cnt = 0;

    for (int i = 0; i < imageSize; i += 1) {
        widthCnt = i % rowSize;
        if(i % rowSize == 0) heightCnt++;

        if(widthCnt ==0) {
            //좌측 상단 Vertex
            if(heightCnt ==0){
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[widthCnt + (heightCnt * rowSize) ];
                arr[2] = arr[5] = inimg[widthCnt+1+(heightCnt*rowSize) ];
                arr[6] = arr[7] = inimg[widthCnt+((heightCnt+1)*rowSize)  ];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt+1)*rowSize)  ];
            }
            //좌측 하단 Vertex
            else if(heightCnt == height -1){
                arr[0] = arr[1] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[2] = inimg[widthCnt+1 +((heightCnt-1)*rowSize) ];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[widthCnt + ((heightCnt*rowSize)) ];
                arr[8] = arr[5] = inimg[widthCnt + 1 + (heightCnt*rowSize) ];
            }
            else {
                arr[0] = arr[1] = inimg[widthCnt+( (heightCnt-1)*rowSize)  ];
                arr[2] = inimg[widthCnt+1+( (heightCnt-1)*rowSize)  ];
                arr[3] = arr[4] = inimg[widthCnt+(heightCnt*rowSize) ];
                arr[5] = inimg[widthCnt+1+(heightCnt*rowSize) ];
                arr[6] = arr[7] = inimg[widthCnt+ ( (heightCnt+1)*rowSize)  ];
                arr[8] = inimg[widthCnt+1+( (heightCnt+1)*rowSize)  ];
            }
            cnt=0;
            sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            //outimg[(widthCnt+heightCnt*rowSize) ] = LIMIT_UBYTE(sum);
            *(outimg + i) = LIMIT_UBYTE(sum);

        }
        else if( widthCnt == (rowSize -1) ){
            //우측 상단 Vertex
            if(heightCnt==0){
                arr[0] = arr[3] = inimg[widthCnt-1 + (heightCnt*rowSize)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[widthCnt + (heightCnt*rowSize)  ];
                arr[6] = inimg[widthCnt-1 + ((heightCnt-1)*rowSize)  ];
                arr[7] = arr[8] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
            }
            //우측 하단 Vertex
            else if(heightCnt==height-1){
                arr[0] = inimg[widthCnt-1 + ((heightCnt-1)*rowSize)  ];
                arr[1] = arr[2] = inimg[widthCnt-1 +((heightCnt-1)*rowSize)  ];
                arr[3] = arr[6] = inimg[widthCnt-1+(heightCnt*rowSize) ];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[widthCnt+(heightCnt*rowSize) ];
            }
            else{
                arr[0] = inimg[widthCnt-1 + ((heightCnt-1)*rowSize)  ];
                arr[2] = arr[1] = inimg[widthCnt + ((heightCnt-1)*rowSize)  ];
                arr[3] = inimg[widthCnt-1 + (heightCnt*rowSize)  ];
                arr[5] = arr[4] = inimg[widthCnt+(heightCnt*rowSize)  ];
                arr[6] = inimg[widthCnt-1 + ((heightCnt+1)*rowSize)  ];
                arr[8] = arr[7] = inimg[widthCnt+((heightCnt+1)*rowSize)  ];
            }
            cnt=0;
            sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
//            outimg[(widthCnt+heightCnt*rowSize) ] = LIMIT_UBYTE(sum);
            *(outimg + i) = LIMIT_UBYTE(sum);
        }
        else if(heightCnt==0){
            if( widthCnt!=1 && widthCnt!=rowSize-1 ){
                arr[0] = arr[3] = inimg[widthCnt-1+(heightCnt*rowSize)  ];
                arr[1] = arr[4] = inimg[widthCnt+(heightCnt*rowSize) ];
                arr[2] = arr[5] = inimg[widthCnt+1+(heightCnt*rowSize)  ];
                arr[6] = inimg[widthCnt-1+((heightCnt+1)*rowSize)  ];
                arr[7] = inimg[widthCnt+((heightCnt+1)*rowSize)  ];
                arr[8] = inimg[widthCnt+1 + ((heightCnt+1)*rowSize)  ];
            }
            cnt=0;
            sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            //outimg[(widthCnt+heightCnt*rowSize) ] = LIMIT_UBYTE(sum);
            *(outimg + i) = LIMIT_UBYTE(sum);
        }
        else if( heightCnt ==(height -1) ){
            if( widthCnt!=1 && widthCnt!=rowSize-1 ){
                arr[0] = inimg[widthCnt-1+((heightCnt-1)*rowSize) ];
                arr[1] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[2] = inimg[widthCnt+1+((heightCnt-1)*rowSize) ];
                arr[3] = arr[6] = inimg[widthCnt-1+(heightCnt*rowSize) ];
                arr[4] = arr[7] = inimg[widthCnt+(heightCnt*rowSize) ];
                arr[5] = arr[8] = inimg[widthCnt+1+(heightCnt*rowSize) ];
            }
            cnt=0;
            sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            //outimg[(widthCnt+heightCnt*rowSize) ] = LIMIT_UBYTE(sum);
            *(outimg + i) = LIMIT_UBYTE(sum);
        }
        else{
            double sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*inimg[((widthCnt+i*1)+(heightCnt+j)*rowSize) ];
                }
            }
            //outimg[(widthCnt+heightCnt*rowSize) ] = LIMIT_UBYTE(sum);
            *(outimg + i) = LIMIT_UBYTE(sum);
        }
    }   //for문 i imageSize

    return filteredImage;
}



QImage* BlurSharpenMask::blur5x5(const uchar* inimg) {

    double blur[5][5] = { {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0} };

    unsigned char arr[25]={0,};
    double sum = 0.0;

    int elemSize = 4;
    int rowSize = width * elemSize;
    int imageSize = rowSize * height;

    int widthCnt = 0, heightCnt = -1, cnt = 0;

    // inSide
    for(int i = 0; i < imageSize; i+=elemSize){
        widthCnt = i % rowSize;
        if(i % rowSize == 0) heightCnt++;
        //int offset = widthCnt + (heightCnt * rowSize) + z;

        sum = 0.0;
        if ( (widthCnt > 1 && widthCnt < (width-2)*1) && (heightCnt >1 && heightCnt < height-2) ) {

            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*inimg[(widthCnt+i*1)+((heightCnt+j)*rowSize) ];
                }
            }
            outimg[heightCnt*rowSize + widthCnt  ] = LIMIT_UBYTE(sum);
        }
        //LeftVertex
        else if(widthCnt ==0){
            //LeftTopVertex
            if(heightCnt==0){
                arr[0] = arr[1]= arr[2] = arr[5] = arr[6] = arr[7] = arr[10]
                        = arr[11] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[3] = arr[8] = arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[4] = arr[9] = arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[20] = arr[21] = arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+2*1)+((heightCnt+2)*rowSize) ];
            }
            else if(heightCnt == 1){
                arr[0] = arr[1] = arr[2] = arr[5] = arr[6] = arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[3] = arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[4] = arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[20] = arr[21] = arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+2*1)+((heightCnt+2)*rowSize) ];
            }
            else if(heightCnt==height-2){
                arr[0] = arr[1] = arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = arr[6] = arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = arr[17] = arr[20]
                        = arr[21] = arr[22] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = arr[23] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = arr[24] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
            }
            //LeftBottomVertex
            else if(heightCnt == height-1){
                arr[0] = arr[1] = arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = arr[6] = arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11]= arr[15] = arr[16] = arr[17]
                        = arr[20] = arr[21] = arr[22] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = arr[18] = arr[23] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = arr[19] = arr[24] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
            }
            //LeftSide
            else{
                arr[0] = arr[1] = arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = arr[6] = arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[20] = arr[21] = arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+2*1)+((heightCnt+2)*rowSize) ];
            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }
        //LeftSide
        else if(widthCnt==1){
            //LeftTopVertex
            if(heightCnt==0){
                arr[0] = arr[1] = arr[5] = arr[6] = arr[10] = arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[2] = arr[7] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[3] = arr[8] = arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[4] = arr[9] = arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[20] = arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+2*1)+((heightCnt+2)*rowSize) ];
            }
            else if(heightCnt==1){
                arr[0] = arr[1] = arr[5] = arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[2] = arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[3] = arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[4] = arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[20] = arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
            }
            //LeftBottomVertex
            else if(heightCnt == height -1){
                arr[0] = arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11] = arr[15] = arr[16] = arr[20] = arr[21] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = arr[17] = arr[22] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = arr[18] = arr[23] = inimg[(widthCnt+1*1)+(heightCnt*rowSize)   ];
                arr[14] = arr[19] = arr[24] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
            }
            else if(heightCnt == height -2){
                arr[0] = arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = arr[20] = arr[21] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = arr[22] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = arr[23] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = arr[24] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
            }
            //LeftSide
            else{
                arr[0] = arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[20] = arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+2*1)+((heightCnt+2)*rowSize) ];
            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }
        //RightSide
        else if(widthCnt==(width-2)*1){
            //RightTopVertex
            if(heightCnt==0){
                arr[0] = arr[5] = arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[1] = arr[6] = arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[2] = arr[7] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[3] = arr[8] = arr[4] = arr[9] = arr[14] = arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[15] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = arr[19] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[(widthCnt-2*1)+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = arr[24] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
            }
            else if(heightCnt==1){
                arr[0] = arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[1] = arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[2] = arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[3] = arr[4] = arr[8] = arr[9] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = arr[14] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[15] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = arr[19] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[(widthCnt-2*1)+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = arr[24] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
            }
            //RightBottomVertex
            else if(heightCnt==height-2){
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = arr[4] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = arr[9] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = arr[14] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[15] = arr[20] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = arr[21] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = arr[22] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = arr[23] = arr[19] = arr[24] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
            }
            else if(heightCnt == height-1){
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = arr[4] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = arr[9] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[15] = arr[20] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = arr[16] = arr[21] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = arr[17] = arr[22] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = arr[14] = arr[18] = arr[19] = arr[23] = arr[24] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
            }
            //RightSide
            else{
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = arr[4] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = arr[9] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = arr[14] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[15] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = arr[19] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[(widthCnt-2*1)+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = arr[24] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }
        //RightSide
        else if(widthCnt==(width-1)*1){
            //RightTopVertex
            if(heightCnt==0){
                arr[0] = arr[5] = arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[1] = arr[6] = arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[2] = arr[7] = arr[3] = arr[8] = arr[13]
                        = arr[4] = arr[9] = arr[14] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[15] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = arr[18] = arr[19] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[(widthCnt-2*1)+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = arr[23] = arr[24] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
            }
            else if(heightCnt==1){
                arr[0] = arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[1] = arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[2] = arr[7] = arr[3] = arr[4] = arr[8] = arr[9] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = arr[13] = arr[14] = inimg[widthCnt + heightCnt*rowSize ];
                arr[15] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = arr[18] = arr[19] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[(widthCnt-2*1)+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = arr[23] = arr[24] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
            }
            //RightBottomVertex
            else if(heightCnt==height-1){
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = arr[3] = arr[4] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = arr[8] = arr[9] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[10] = arr[15] = arr[20] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = arr[16] = arr[21] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = arr[13] = arr[14] = arr[17]
                        = arr[18] = arr[19] = arr[22] = arr[23] = arr[24] = inimg[widthCnt + heightCnt*rowSize];
            }
            else if(heightCnt==height-2){
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = arr[3] = arr[4] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = arr[8] = arr[9] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = arr[13] = arr[14] = inimg[widthCnt + heightCnt*rowSize ];
                arr[15] = arr[20] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = arr[21] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = arr[18] = arr[19] = arr[22] = arr[23] = arr[24] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
            }
            //RightSide
            else{
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = arr[3] = arr[4] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = arr[8] = arr[9] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = arr[13] = arr[14] = inimg[widthCnt + heightCnt*rowSize ];
                arr[15] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = arr[18] = arr[19] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[widthCnt-2*1+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[widthCnt-1*1+((heightCnt+2)*rowSize) ];
                arr[22] = arr[23] = arr[24] = inimg[widthCnt+((heightCnt+2)*rowSize) ];

            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }

        //TopSide
        else if( heightCnt==0){
            if(widthCnt>1 && widthCnt <(width-2)*1){
                arr[0] = arr[5] = arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[1] = arr[6] = arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[2] = arr[7] = arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[3] = arr[8] = arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[4] = arr[9] = arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[(widthCnt-2*1)+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+2*1)+((heightCnt+2)*rowSize) ];
            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }
        else if(heightCnt==1){
            if(widthCnt>1 && widthCnt <(width-2)*1){
                arr[0] = arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[1] = arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[2] = arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[3] = arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[4] = arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
                arr[20] = inimg[(widthCnt-2*1)+((heightCnt+2)*rowSize) ];
                arr[21] = inimg[(widthCnt-1*1)+((heightCnt+2)*rowSize) ];
                arr[22] = inimg[widthCnt+((heightCnt+2)*rowSize) ];
                arr[23] = inimg[(widthCnt+1*1)+((heightCnt+2)*rowSize) ];
                arr[24] = inimg[(widthCnt+2*1)+((heightCnt+2)*rowSize) ];
            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }
        //BottomSide
        else if(heightCnt==height-2){
            if(widthCnt>1 && widthCnt <(width-2)*1){
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
                arr[15] = arr[20] = inimg[(widthCnt-2*1)+((heightCnt+1)*rowSize) ];
                arr[16] = arr[21] = inimg[(widthCnt-1*1)+((heightCnt+1)*rowSize) ];
                arr[17] = arr[22] = inimg[widthCnt+((heightCnt+1)*rowSize) ];
                arr[18] = arr[23] = inimg[(widthCnt+1*1)+((heightCnt+1)*rowSize) ];
                arr[19] = arr[24] = inimg[(widthCnt+2*1)+((heightCnt+1)*rowSize) ];
            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }
        else if( heightCnt==height-1){
            if(widthCnt>1 && widthCnt <(width-2)*1){
                arr[0] = inimg[(widthCnt-2*1)+((heightCnt-2)*rowSize) ];
                arr[1] = inimg[(widthCnt-1*1)+((heightCnt-2)*rowSize) ];
                arr[2] = inimg[widthCnt+((heightCnt-2)*rowSize) ];
                arr[3] = inimg[(widthCnt+1*1)+((heightCnt-2)*rowSize) ];
                arr[4] = inimg[(widthCnt+2*1)+((heightCnt-2)*rowSize) ];
                arr[5] = inimg[(widthCnt-2*1)+((heightCnt-1)*rowSize) ];
                arr[6] = inimg[(widthCnt-1*1)+((heightCnt-1)*rowSize) ];
                arr[7] = inimg[widthCnt+((heightCnt-1)*rowSize) ];
                arr[8] = inimg[(widthCnt+1*1)+((heightCnt-1)*rowSize) ];
                arr[9] = inimg[(widthCnt+2*1)+((heightCnt-1)*rowSize) ];
                arr[10] = arr[15] = arr[20] = inimg[(widthCnt-2*1)+(heightCnt*rowSize) ];
                arr[11] = arr[16] = arr[21] = inimg[(widthCnt-1*1)+(heightCnt*rowSize) ];
                arr[12] = arr[17] = arr[22] = inimg[widthCnt + heightCnt*rowSize ];
                arr[13] = arr[18] = arr[23] = inimg[(widthCnt+1*1)+(heightCnt*rowSize) ];
                arr[14] = arr[19] = arr[24] = inimg[(widthCnt+2*1)+(heightCnt*rowSize) ];
            }

            int cnt=0;
            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*arr[cnt++];
                }
            }
            outimg[widthCnt + heightCnt*rowSize ] = LIMIT_UBYTE(sum);
        }

    }   //for문 i, imageSize
    return filteredImage;
}
/*
QImage* BlurSharpenMask::blur7x7(const uchar* in) {

    int cols = width;
    int rows = height;
    int elemSize = 4, z;

    // define the kernel
    float kernel[7][7] = { {1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0},
                           {1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0},
                           {1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0},
                           {1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0},
                           {1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0},
                           {1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0},
                           {1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0, 1 / 49.0} };

    float sum2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width * elemSize; x+= elemSize) {
            int offset = x + (y * width * elemSize);
            for (z = 0; z < elemSize; z++) {
                outimg[offset + z] = in[offset + z];
            }
        }
    }

    for (int y = 0; y < height -1; y++) {
        for (int x = 0; x < width * elemSize ; x += elemSize ) {

            int offset = x + (y * cols * elemSize );

            for (z = 0; z < elemSize; z++) {
                sum2 = 0.0;
                for (int i = -3; i < 4; i++) {
                    for (int j = -3; j < 4; j++) {

                        if (((x >= 3 *elemSize ) && (x < (cols - 3)*elemSize)) && ((y >= 4) && (y < (rows - 3))))
                        {

                            sum2 += kernel[i + 3][j + 3] * in[(x+i*elemSize) + (y+j) * width * elemSize + z];

                            outimg[offset + z] = LIMIT_UBYTE(sum2);
                            //	printf("outimgpixel: %d ", outimg[offset + z]);
                        }


                        else if (y == 0) {
                            float sum = 0.0;
                            unsigned char arr[49];
                            if (x == 0) {
                                arr[0] = arr[1] = arr[2] = arr[3] = arr[7] = arr[8] = arr[9]
                                        = arr[10] = arr[14] = arr[15] = arr[16] = arr[17] =
                                        arr[21] = arr[22] = arr[23] = arr[24] = in[x * elemSize + z];
                                arr[4] = arr[11] = arr[18] = arr[25] = in[(x + 1) * elemSize + z];
                                arr[5] = arr[12] = arr[19] = arr[26] = in[(x + 2) * elemSize + z];
                                arr[6] = arr[13] = arr[20] = arr[27] = in[(x + 3) * elemSize + z];
                                arr[28] = arr[29] = arr[30] = arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[35] = arr[36] = arr[37] = arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[42] = arr[43] = arr[44] = arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[40] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[41] = in[(x + 3 + (y + 2) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 3) * cols) * elemSize + z];
                                arr[47] = in[(x + 2 + (y + 3) * cols) * elemSize + z];
                                arr[48] = in[(x + 3 + (y + 3) * cols) * elemSize + z];

                            }
                            else if (x == 1 )
                            {
                                arr[0] = arr[1] = arr[2] = arr[7] = arr[8] = arr[9] = arr[14] = arr[15] = arr[16] = arr[21] = arr[22] = arr[23] = in[(x - 1) * elemSize + z];
                                arr[3] = arr[10] = arr[17] = arr[24] = in[(x)*elemSize + z];
                                arr[4] = arr[11] = arr[18] = arr[25] = in[(x + 1) * elemSize + z];
                                arr[5] = arr[12] = arr[19] = arr[26] = in[(x + 2) * elemSize + z];
                                arr[6] = arr[13] = arr[20] = arr[27] = in[(x + 3) * elemSize + z];
                                arr[28] = arr[29] = arr[30] = in[((x - 1) + (y + 1) * cols) * elemSize + z];
                                arr[35] = arr[36] = arr[37] = in[((x - 1) + (y + 2) * cols) * elemSize + z];
                                arr[42] = arr[43] = arr[44] = in[((x - 1) + (y + 3) * cols) * elemSize + z];
                                arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                                arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[40] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[41] = in[(x + 3 + (y + 2) * cols) * elemSize + z];
                                arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 3) * cols) * elemSize + z];
                                arr[47] = in[(x + 2 + (y + 3) * cols) * elemSize + z];
                                arr[48] = in[(x + 3 + (y + 3) * cols) * elemSize + z];


                            }
                            else if (x == 2)
                            {
                                arr[0] = arr[1] = arr[7] = arr[8] = arr[14] = arr[15] = arr[21] = arr[22] = in[(x - 2) * elemSize + z];
                                arr[2] = arr[9] = arr[16] = arr[23] = in[(x - 1) * elemSize + z];
                                arr[3] = arr[10] = arr[17] = arr[24] = in[(x)*elemSize + z];
                                arr[4] = arr[11] = arr[18] = arr[25] = in[(x + 1) * elemSize + z];

                                arr[5] = arr[12] = arr[19] = arr[26] = in[(x + 2) * elemSize + z];
                                arr[6] = arr[13] = arr[20] = arr[27] = in[(x + 3) * elemSize + z];
                                arr[28] = arr[29] = in[((x - 2) + (y + 1) * cols) * elemSize + z];
                                arr[35] = arr[36] = in[((x - 2) + (y + 2) * cols) * elemSize + z];
                                arr[42] = arr[43] = in[((x - 2) + (y + 3) * cols) * elemSize + z];
                                arr[30] = in[((x - 1) + (y + 1) * cols) * elemSize + z];
                                arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[40] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[41] = in[(x + 3 + (y + 2) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];
                                arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 3) * cols) * elemSize + z];
                                arr[47] = in[(x + 2 + (y + 3) * cols) * elemSize + z];
                                arr[48] = in[(x + 3 + (y + 3) * cols) * elemSize + z];
                            }

                            else if (x == (cols - 1) * elemSize) {
                                arr[3] = arr[4] = arr[5] = arr[6] = arr[10] = arr[11] = arr[12] = arr[13] =
                                        arr[17] = arr[18] = arr[19] = arr[20] = arr[24] = arr[25] = arr[26] = arr[27] = in[x * elemSize + z];
                                arr[2] = arr[9] = arr[16] = arr[23] = in[(x - 1) * elemSize + z];
                                arr[1] = arr[8] = arr[15] = arr[22] = in[(x - 2) * elemSize + z];
                                arr[0] = arr[7] = arr[14] = arr[21] = in[(x - 3) * elemSize + z];
                                arr[31] = arr[32] = arr[33] = arr[34] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[38] = arr[39] = arr[40] = arr[41] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[45] = arr[46] = arr[47] = arr[48] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                                arr[35] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[42] = in[(x - 3 + (y + 3) * cols) * elemSize + z];
                                arr[43] = in[(x - 2 + (y + 3) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];
                            }
                            else if (x == (cols - 2) * elemSize) {
                                arr[4] = arr[5] = arr[6] = arr[11] = arr[12] = arr[13] = arr[18] = arr[19] = arr[20] = arr[25] = arr[26] = arr[27] = in[(x + 1) * elemSize + z];
                                arr[32] = arr[33] = arr[34] = in[((x + 1) + (y + 1) * cols) * elemSize + z];
                                arr[39] = arr[40] = arr[41] = in[((x + 1) + (y + 2) * cols) * elemSize + z];
                                arr[46] = arr[47] = arr[48] = in[((x + 1) + (y + 3) * cols) * elemSize + z];
                                arr[3] = arr[10] = arr[17] = arr[24] = in[x * elemSize + z];
                                arr[2] = arr[9] = arr[16] = arr[23] = in[(x - 1) * elemSize + z];
                                arr[1] = arr[8] = arr[15] = arr[22] = in[(x - 2) * elemSize + z];
                                arr[0] = arr[7] = arr[14] = arr[21] = in[(x - 3) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                                arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[35] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[42] = in[(x - 3 + (y + 3) * cols) * elemSize + z];
                                arr[43] = in[(x - 2 + (y + 3) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];
                                arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                            }
                            else if (x == (cols - 3) * elemSize) {
                                arr[5] = arr[6] = arr[12] = arr[13] = arr[19] = arr[20] = arr[26] = arr[27] = in[(x + 2) * elemSize + z];
                                arr[33] = arr[34] = in[((x + 2) + (y + 1) * cols) * elemSize + z];
                                arr[40] = arr[41] = in[((x + 2) + (y + 2) * cols) * elemSize + z];
                                arr[47] = arr[48] = in[((x + 2) + (y + 3) * cols) * elemSize + z];
                                arr[4] = arr[11] = arr[18] = arr[25] = in[(x + 1) * elemSize + z];
                                arr[3] = arr[10] = arr[17] = arr[24] = in[x * elemSize + z];
                                arr[2] = arr[9] = arr[16] = arr[23] = in[(x - 1) * elemSize + z];
                                arr[1] = arr[8] = arr[15] = arr[22] = in[(x - 2) * elemSize + z];
                                arr[0] = arr[7] = arr[14] = arr[21] = in[(x - 3) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                                arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[35] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[42] = in[(x - 3 + (y + 3) * cols) * elemSize + z];
                                arr[43] = in[(x - 2 + (y + 3) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];
                                arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 3) * cols) * elemSize + z];

                            }
                            else {
                                arr[0] = arr[7] = arr[14] = arr[21] = in[(x - 3) * elemSize + z];
                                arr[1] = arr[8] = arr[15] = arr[22] = in[(x - 2) * elemSize + z];
                                arr[2] = arr[9] = arr[16] = arr[23] = in[(x - 1) * elemSize + z];
                                arr[3] = arr[10] = arr[17] = arr[24] = in[x * elemSize + z];
                                arr[4] = arr[11] = arr[18] = arr[25] = in[(x + 1) * elemSize + z];
                                arr[5] = arr[12] = arr[19] = arr[26] = in[(x + 2) * elemSize + z];
                                arr[6] = arr[13] = arr[20] = arr[27] = in[(x + 3) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                                arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                                arr[35] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[40] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[41] = in[(x + 3 + (y + 2) * cols) * elemSize + z];
                                arr[42] = in[(x - 3 + (y + 3) * cols) * elemSize + z];
                                arr[43] = in[(x - 2 + (y + 3) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];
                                arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 3) * cols) * elemSize + z];
                                arr[47] = in[(x + 2 + (y + 3) * cols) * elemSize + z];
                                arr[48] = in[(x + 3 + (y + 3) * cols) * elemSize + z];

                            }

                            for (int i = 0; i < 49; i++)
                            {
                                sum += arr[i];

                            }
                            outimg[offset + z] = sum / 49;


                        }

                        else if (y == (rows - 1)) {
                            float sum = 0.0;
                            unsigned char arr[49];
                            if (x == 0) {
                                arr[21] = arr[22] = arr[23] = arr[24] = arr[28] = arr[29] = arr[30] = arr[31] =
                                        arr[35] = arr[36] = arr[37] = arr[38] = arr[42] = arr[43] = arr[44] = arr[45] = in[(x + y * cols) * elemSize + z];
                                arr[25] = arr[32] = arr[39] = arr[46] = in[(x + 1 + y * cols) * elemSize + z];
                                arr[26] = arr[33] = arr[40] = arr[47] = in[(x + 2 + y * cols) * elemSize + z];
                                arr[27] = arr[34] = arr[41] = arr[48] = in[(x + 3 + y * cols) * elemSize + z];
                                arr[0] = arr[1] = arr[2] = arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[7] = arr[8] = arr[9] = arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[14] = arr[15] = arr[16] = arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[5] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[6] = in[(x + 3 + (y - 3) * cols) * elemSize + z];
                                arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];

                            }
                            else if (x == 1) {
                                arr[21] = arr[22] = arr[23] = arr[28] = arr[29] = arr[30] = arr[35] = arr[36] =
                                        arr[37] = arr[42] = arr[43] = arr[44] = in[(x - 1 + y * cols) * elemSize + z];
                                arr[24] = arr[31] = arr[38] = arr[45] = in[(x + y * cols) * elemSize + z];
                                arr[25] = arr[32] = arr[39] = arr[46] = in[(x + 1 + y * cols) * elemSize + z];
                                arr[26] = arr[33] = arr[40] = arr[47] = in[(x + 2 + y * cols) * elemSize + z];
                                arr[27] = arr[34] = arr[41] = arr[48] = in[(x + 3 + y * cols) * elemSize + z];
                                arr[0] = arr[1] = arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[7] = arr[8] = arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[14] = arr[15] = arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[5] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[6] = in[(x + 3 + (y - 3) * cols) * elemSize + z];
                                arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];

                            }
                            else if (x == 2) {
                                arr[21] = arr[22] = arr[28] = arr[29] = arr[35] = arr[36] = arr[42] = arr[43] = in[(x - 2 + y * cols) * elemSize + z];
                                arr[0] = arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[7] = arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[14] = arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[23] = arr[30] = arr[37] = arr[44] = in[(x - 1 + y * cols) * elemSize + z];
                                arr[24] = arr[31] = arr[38] = arr[45] = in[(x + y * cols) * elemSize + z];
                                arr[25] = arr[32] = arr[39] = arr[46] = in[(x + 1 + y * cols) * elemSize + z];
                                arr[26] = arr[33] = arr[40] = arr[47] = in[(x + 2 + y * cols) * elemSize + z];
                                arr[27] = arr[34] = arr[41] = arr[48] = in[(x + 3 + y * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[5] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[6] = in[(x + 3 + (y - 3) * cols) * elemSize + z];
                                arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];
                            }
                            else if (x == (cols - 1)) {
                                arr[24] = arr[25] = arr[26] = arr[27] = arr[31] = arr[32] = arr[33] = arr[34] =
                                        arr[38] = arr[39] = arr[40] = arr[41] = arr[45] = arr[46] = arr[47] = arr[48] = in[(x + y * cols) * elemSize + z];
                                arr[21] = arr[28] = arr[35] = arr[42] = in[(x - 3 + y * cols) * elemSize + z];
                                arr[22] = arr[29] = arr[36] = arr[43] = in[(x - 2 + y * cols) * elemSize + z];
                                arr[23] = arr[30] = arr[37] = arr[44] = in[(x - 1 + y * cols) * elemSize + z];
                                arr[3] = arr[4] = arr[5] = arr[6] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[10] = arr[11] = arr[12] = arr[13] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[17] = arr[18] = arr[19] = arr[20] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[0] = in[(x - 3 + (y - 3) * cols) * elemSize + z];
                                arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[7] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];

                            }
                            else if (x == (cols - 2)) {
                                arr[25] = arr[26] = arr[27] = arr[32] = arr[33] = arr[34] = arr[39] = arr[40] = arr[41] =
                                        arr[46] = arr[47] = arr[48] = in[(x + 1 + y * cols) * elemSize + z];
                                arr[24] = arr[31] = arr[38] = arr[45] = in[(x + y * cols) * elemSize + z];
                                arr[23] = arr[30] = arr[37] = arr[44] = in[(x - 1 + y * cols) * elemSize + z];
                                arr[22] = arr[29] = arr[36] = arr[43] = in[(x - 2 + y * cols) * elemSize + z];
                                arr[21] = arr[28] = arr[35] = arr[42] = in[(x - 3 + y * cols) * elemSize + z];
                                arr[4] = arr[5] = arr[6] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[11] = arr[12] = arr[13] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[18] = arr[19] = arr[20] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[0] = in[(x - 3 + (y - 3) * cols) * elemSize + z];
                                arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[7] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                            }
                            else if (x == (cols - 3)) {
                                arr[26] = arr[27] = arr[33] = arr[34] = arr[40] = arr[41] = arr[47] = arr[48] = in[(x + 2 + y * cols) * elemSize + z];
                                arr[25] = arr[32] = arr[39] = arr[46] = in[(x + 1 + y * cols) * elemSize + z];
                                arr[24] = arr[31] = arr[38] = arr[45] = in[(x + y * cols) * elemSize + z];
                                arr[23] = arr[30] = arr[37] = arr[44] = in[(x - 1 + y * cols) * elemSize + z];
                                arr[22] = arr[29] = arr[36] = arr[43] = in[(x - 2 + y * cols) * elemSize + z];
                                arr[21] = arr[28] = arr[35] = arr[42] = in[(x - 3 + y * cols) * elemSize + z];
                                arr[5] = arr[6] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[12] = arr[13] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[19] = arr[20] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[0] = in[(x - 3 + (y - 3) * cols) * elemSize + z];
                                arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[7] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[8] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[9] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[10] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[11] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];

                            }
                            else {
                                arr[21] = arr[28] = arr[35] = arr[42] = in[(x - 3 + y * cols) * elemSize + z];
                                arr[22] = arr[29] = arr[36] = arr[43] = in[(x - 2 + y * cols) * elemSize + z];
                                arr[23] = arr[30] = arr[37] = arr[44] = in[(x - 1 + y * cols) * elemSize + z];
                                arr[24] = arr[31] = arr[38] = arr[45] = in[(x + y * cols) * elemSize + z];
                                arr[25] = arr[32] = arr[39] = arr[46] = in[(x + 1 + y * cols) * elemSize + z];
                                arr[26] = arr[33] = arr[40] = arr[47] = in[(x + 2 + y * cols) * elemSize + z];
                                arr[27] = arr[34] = arr[41] = arr[48] = in[(x + 3 + y * cols) * elemSize + z];
                                arr[0] = in[(x - 3 + (y - 3) * cols) * elemSize + z];
                                arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[5] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[6] = in[(x + 3 + (y - 3) * cols) * elemSize + z];
                                arr[7] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];
                            }

                            for (int i = 0; i < 49; i++)
                                sum += arr[i];
                            outimg[offset + z] = sum / 49;

                        }
                        else if (x == 0) {
                            float sum = 0.0;
                            unsigned char arr[49];
                            if (y == 1) {
                                arr[0] = arr[1] = arr[2] = arr[3] = arr[7] = arr[8] =
                                        arr[9] = arr[10] = arr[14] = arr[15] = arr[16] = arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[4] = arr[11] = arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[5] = arr[12] = arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[6] = arr[13] = arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];
                                arr[21] = arr[22] = arr[23] = arr[24] = in[(x + y * cols) * elemSize + z];
                                arr[28] = arr[29] = arr[30] = arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[35] = arr[36] = arr[37] = arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[42] = arr[43] = arr[44] = arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[25] = in[(x + 1 + y * cols) * elemSize + z];
                                arr[26] = in[(x + 2 + y * cols) * elemSize + z];
                                arr[27] = in[(x + 3 + y * cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[40] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[41] = in[(x + 3 + (y + 2) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 3) * cols) * elemSize + z];
                                arr[47] = in[(x + 2 + (y + 3) * cols) * elemSize + z];
                                arr[48] = in[(x + 3 + (y + 3) * cols) * elemSize + z];
                            }
                            else if (y == 2) {
                                arr[0] = arr[1] = arr[2] = arr[3] = arr[7] = arr[8] = arr[9] = arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[4] = arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[5] = arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[6] = arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[14] = arr[15] = arr[16] = arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[21] = arr[22] = arr[23] = arr[24] = in[(x + (y)*cols) * elemSize + z];
                                arr[28] = arr[29] = arr[30] = arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[35] = arr[36] = arr[37] = arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[42] = arr[43] = arr[44] = arr[45] = in[(x + (y + 3) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];
                                arr[25] = in[(x + 1 + (y)*cols) * elemSize + z];
                                arr[26] = in[(x + 2 + (y)*cols) * elemSize + z];
                                arr[27] = in[(x + 3 + (y)*cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[40] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[41] = in[(x + 3 + (y + 2) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 3) * cols) * elemSize + z];
                                arr[47] = in[(x + 2 + (y + 3) * cols) * elemSize + z];
                                arr[48] = in[(x + 3 + (y + 3) * cols) * elemSize + z];

                            }
                            else if (y == (rows - 2))
                            {
                                arr[28] = arr[29] = arr[30] = arr[31] = arr[35] = arr[36] = arr[37] =
                                        arr[38] = arr[42] = arr[43] = arr[44] = arr[45] = in[(x + (y + 1) * cols) * elemSize + z];

                                arr[0] = arr[1] = arr[2] = arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[7] = arr[8] = arr[9] = arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[14] = arr[15] = arr[16] = arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[21] = arr[22] = arr[23] = arr[24] = in[(x + (y)*cols) * elemSize + z];



                                arr[32] = arr[39] = arr[46] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = arr[40] = arr[47] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = arr[41] = arr[48] = in[(x + 3 + (y + 1) * cols) * elemSize + z];

                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[5] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[6] = in[(x + 3 + (y - 3) * cols) * elemSize + z];
                                arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];
                                arr[25] = in[(x + 1 + (y)*cols) * elemSize + z];
                                arr[26] = in[(x + 2 + (y)*cols) * elemSize + z];
                                arr[27] = in[(x + 3 + (y)*cols) * elemSize + z];
                            }
                            else if (y == (rows - 3))
                            {
                                arr[35] = arr[36] = arr[37] = arr[38] =
                                        arr[42] = arr[43] = arr[44] = arr[45] = in[(x + (y + 2) * cols) * elemSize + z];

                                arr[0] = arr[1] = arr[2] = arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[7] = arr[8] = arr[9] = arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[14] = arr[15] = arr[16] = arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[21] = arr[22] = arr[23] = arr[24] = in[(x + (y)*cols) * elemSize + z];
                                arr[28] = arr[29] = arr[30] = arr[31] = in[(x + (y + 1) * cols) * elemSize + z];

                                arr[39] = arr[46] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[40] = arr[47] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[41] = arr[48] = in[(x + 3 + (y + 1) * cols) * elemSize + z];

                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[5] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[6] = in[(x + 3 + (y - 3) * cols) * elemSize + z];
                                arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];
                                arr[25] = in[(x + 1 + (y)*cols) * elemSize + z];
                                arr[26] = in[(x + 2 + (y)*cols) * elemSize + z];
                                arr[27] = in[(x + 3 + (y)*cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                            }

                            else {

                                arr[0] = arr[1] = arr[2] = arr[3] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[7] = arr[8] = arr[9] = arr[10] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[14] = arr[15] = arr[16] = arr[17] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[21] = arr[22] = arr[23] = arr[24] = in[(x + (y)*cols) * elemSize + z];
                                arr[28] = arr[29] = arr[30] = arr[31] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[35] = arr[36] = arr[37] = arr[38] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[42] = arr[43] = arr[44] = arr[45] = in[(x + (y + 3) * cols) * elemSize + z];

                                arr[4] = in[(x + 1 + (y - 3) * cols) * elemSize + z];
                                arr[5] = in[(x + 2 + (y - 3) * cols) * elemSize + z];
                                arr[6] = in[(x + 3 + (y - 3) * cols) * elemSize + z];
                                arr[11] = in[(x + 1 + (y - 2) * cols) * elemSize + z];
                                arr[12] = in[(x + 2 + (y - 2) * cols) * elemSize + z];
                                arr[13] = in[(x + 3 + (y - 2) * cols) * elemSize + z];
                                arr[18] = in[(x + 1 + (y - 1) * cols) * elemSize + z];
                                arr[19] = in[(x + 2 + (y - 1) * cols) * elemSize + z];
                                arr[20] = in[(x + 3 + (y - 1) * cols) * elemSize + z];
                                arr[25] = in[(x + 1 + (y)*cols) * elemSize + z];
                                arr[26] = in[(x + 2 + (y)*cols) * elemSize + z];
                                arr[27] = in[(x + 3 + (y)*cols) * elemSize + z];
                                arr[32] = in[(x + 1 + (y + 1) * cols) * elemSize + z];
                                arr[33] = in[(x + 2 + (y + 1) * cols) * elemSize + z];
                                arr[34] = in[(x + 3 + (y + 1) * cols) * elemSize + z];
                                arr[39] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[40] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[41] = in[(x + 3 + (y + 2) * cols) * elemSize + z];
                                arr[46] = in[(x + 1 + (y + 2) * cols) * elemSize + z];
                                arr[47] = in[(x + 2 + (y + 2) * cols) * elemSize + z];
                                arr[48] = in[(x + 3 + (y + 3) * cols) * elemSize + z];

                            }

                            for (int i = 0; i < 49; i++)
                                sum += arr[i];
                            outimg[offset + z] = sum / 49;
                        }
                        else if (x == (cols - 1)) {
                            float sum = 0.0;
                            unsigned char arr[49];

                            if (y == 1)
                            {
                                arr[3] = arr[4] = arr[5] = arr[6] = arr[10] = arr[11] = arr[12] = arr[13] =
                                        arr[17] = arr[18] = arr[19] = arr[20] = in[(x + (y - 1) * cols) * elemSize + z];

                                arr[24] = arr[25] = arr[26] = arr[27] = in[(x + (y)*cols) * elemSize + z];
                                arr[31] = arr[32] = arr[33] = arr[34] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[38] = arr[39] = arr[40] = arr[41] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[45] = arr[46] = arr[47] = arr[48] = in[(x + (y + 3) * cols) * elemSize + z];

                                arr[0] = arr[7] = arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[1] = arr[8] = arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[2] = arr[9] = arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];


                                arr[21] = in[(x - 3 + (y)*cols) * elemSize + z];
                                arr[22] = in[(x - 2 + (y)*cols) * elemSize + z];
                                arr[23] = in[(x - 1 + (y)*cols) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                                arr[35] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[42] = in[(x - 3 + (y + 3) * cols) * elemSize + z];
                                arr[43] = in[(x - 2 + (y + 3) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];

                            }
                            else if (y == 2)
                            {
                                arr[3] = arr[4] = arr[5] = arr[6] =
                                        arr[10] = arr[11] = arr[12] = arr[13] = in[(x + (y - 2) * cols) * elemSize + z];

                                arr[17] = arr[18] = arr[19] = arr[20] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[24] = arr[25] = arr[26] = arr[27] = in[(x + (y)*cols) * elemSize + z];
                                arr[31] = arr[32] = arr[33] = arr[34] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[38] = arr[39] = arr[40] = arr[41] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[45] = arr[46] = arr[47] = arr[48] = in[(x + (y + 3) * cols) * elemSize + z];

                                arr[0] = arr[7] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[1] = arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[2] = arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];

                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[21] = in[(x - 3 + (y)*cols) * elemSize + z];
                                arr[22] = in[(x - 2 + (y)*cols) * elemSize + z];
                                arr[23] = in[(x - 1 + (y)*cols) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                                arr[35] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[42] = in[(x - 3 + (y + 3) * cols) * elemSize + z];
                                arr[43] = in[(x - 2 + (y + 3) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];
                            }

                            else if (y == (rows - 2))
                            {
                                arr[31] = arr[32] = arr[33] = arr[34] = arr[38] = arr[39] = arr[40] = arr[41] =
                                        arr[45] = arr[46] = arr[47] = arr[48] = in[(x + (y + 1) * cols) * elemSize + z];

                                arr[3] = arr[4] = arr[5] = arr[6] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[10] = arr[11] = arr[12] = arr[13] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[17] = arr[18] = arr[19] = arr[20] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[24] = arr[25] = arr[26] = arr[27] = in[(x + (y)*cols) * elemSize + z];

                                arr[28] = arr[35] = arr[42] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = arr[36] = arr[43] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = arr[37] = arr[44] = in[(x - 1 + (y + 1) * cols) * elemSize + z];

                                arr[0] = in[(x - 3 + (y - 3) * cols) * elemSize + z];
                                arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[7] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[21] = in[(x - 3 + (y)*cols) * elemSize + z];
                                arr[22] = in[(x - 2 + (y)*cols) * elemSize + z];
                                arr[23] = in[(x - 1 + (y)*cols) * elemSize + z];
                            }
                            else if (y == (rows - 3))
                            {
                                arr[38] = arr[39] = arr[40] = arr[41] = arr[45] = arr[46] =
                                        arr[47] = arr[48] = in[(x + (y + 2) * cols) * elemSize + z];

                                arr[3] = arr[4] = arr[5] = arr[6] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[10] = arr[11] = arr[12] = arr[13] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[17] = arr[18] = arr[19] = arr[20] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[24] = arr[25] = arr[26] = arr[27] = in[(x + (y)*cols) * elemSize + z];
                                arr[31] = arr[32] = arr[33] = arr[34] = in[(x + (y + 1) * cols) * elemSize + z];

                                arr[35] = arr[42] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = arr[43] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = arr[44] = in[(x - 1 + (y + 2) * cols) * elemSize + z];

                                arr[0] = in[(x - 3 + (y - 3) * cols) * elemSize + z];
                                arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[7] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[21] = in[(x - 3 + (y)*cols) * elemSize + z];
                                arr[22] = in[(x - 2 + (y)*cols) * elemSize + z];
                                arr[23] = in[(x - 1 + (y)*cols) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                            }
                            else {
                                arr[3] = arr[4] = arr[5] = arr[6] = in[(x + (y - 3) * cols) * elemSize + z];
                                arr[10] = arr[11] = arr[12] = arr[13] = in[(x + (y - 2) * cols) * elemSize + z];
                                arr[17] = arr[18] = arr[19] = arr[20] = in[(x + (y - 1) * cols) * elemSize + z];
                                arr[24] = arr[25] = arr[26] = arr[27] = in[(x + (y)*cols) * elemSize + z];
                                arr[31] = arr[32] = arr[33] = arr[34] = in[(x + (y + 1) * cols) * elemSize + z];
                                arr[38] = arr[39] = arr[40] = arr[41] = in[(x + (y + 2) * cols) * elemSize + z];
                                arr[45] = arr[46] = arr[47] = arr[8] = in[(x + (y + 3) * cols) * elemSize + z];

                                arr[0] = in[(x - 3 + (y - 3) * cols) * elemSize + z];
                                arr[1] = in[(x - 2 + (y - 3) * cols) * elemSize + z];
                                arr[2] = in[(x - 1 + (y - 3) * cols) * elemSize + z];
                                arr[7] = in[(x - 3 + (y - 2) * cols) * elemSize + z];
                                arr[8] = in[(x - 2 + (y - 2) * cols) * elemSize + z];
                                arr[9] = in[(x - 1 + (y - 2) * cols) * elemSize + z];
                                arr[14] = in[(x - 3 + (y - 1) * cols) * elemSize + z];
                                arr[15] = in[(x - 2 + (y - 1) * cols) * elemSize + z];
                                arr[16] = in[(x - 1 + (y - 1) * cols) * elemSize + z];
                                arr[21] = in[(x - 3 + (y)*cols) * elemSize + z];
                                arr[22] = in[(x - 2 + (y)*cols) * elemSize + z];
                                arr[23] = in[(x - 1 + (y)*cols) * elemSize + z];
                                arr[28] = in[(x - 3 + (y + 1) * cols) * elemSize + z];
                                arr[29] = in[(x - 2 + (y + 1) * cols) * elemSize + z];
                                arr[30] = in[(x - 1 + (y + 1) * cols) * elemSize + z];
                                arr[35] = in[(x - 3 + (y + 2) * cols) * elemSize + z];
                                arr[36] = in[(x - 2 + (y + 2) * cols) * elemSize + z];
                                arr[37] = in[(x - 1 + (y + 2) * cols) * elemSize + z];
                                arr[42] = in[(x - 3 + (y + 3) * cols) * elemSize + z];
                                arr[43] = in[(x - 2 + (y + 3) * cols) * elemSize + z];
                                arr[44] = in[(x - 1 + (y + 3) * cols) * elemSize + z];

                            }
                            for (int i = 0; i < 49; i++)
                                sum += arr[i];
                            outimg[offset + z] = sum / 49;

                        }




                    }
                }
            }
        }
    }

    return filteredImage;
}
*/
