#include "panovalueadjustment.h"

#include "qdebug.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

PanoValueAdjustment::PanoValueAdjustment(QObject *parent)
    : QObject{parent}
{
}

void PanoValueAdjustment::receiveFile(QString file)
{
    pixmap.load(file);
    defaultImg = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);

    inimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    outimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    mask = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);

    sharpenImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(sharpenImg, 0, sizeof(unsigned char) * imageSize);

    set3x3MaskValue();  // 영상의 Mask 값 구함

    for(int i = 0; i < imageSize; i ++){ //영상의 평균 value를 저장하기 위함
        avg += inimg[i];
    }

    avg = avg/imageSize;

}

void PanoValueAdjustment::changePanoValue(int brightValue, int contrastValue, int sbValue)
{
    QImage image = defaultImg;

    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    /* 밝기값만 조정되는 case */
    if(contrastValue == 0 && sbValue == 0){
        int value =  brightValue / 2.5;
        for(int i = 0; i < imageSize; i ++){
            *(outimg + i) = LIMIT_UBYTE( *(inimg + i) + value );
        }
    }
    /* 대비값만 조정되는 case */
    else if(brightValue == 0 && sbValue == 0){
        if (contrastValue > 0) {

            float contrast = (100.0+contrastValue/2)/100.0;
            qDebug()<< contrast;
            for(int i = 0; i < imageSize; i ++){
                 *(outimg + i) = LIMIT_UBYTE( avg + (*(inimg+i)-avg) *contrast );
            }
        }
        else {
            contrastValue *= 0.5;
            float contrast = (50.0+contrastValue/2)/50.0;
            for(int i = 0; i < imageSize; i ++){
                 *(outimg + i) = LIMIT_UBYTE( avg + (*(inimg+i)-avg) *contrast );
            }
        }
    }
    /* 필터값만 조정되는 case */
    else if(brightValue == 0 && contrastValue == 0){
        switch(sbValue) {
        case -4:
            break;
        case -3:
            blur5x5();
            break;
        case -2:
            blur3x3(sbValue);
            break;
        case -1:
            blur3x3(sbValue);
            break;
        case 0:
            prevImg = defaultImg;
            break;
        default:
            highBoost(sbValue);
            break;
        }
        image = prevImg;
    }

    /* 두값 or 세 값이 조정되는 case */
    else{
        if(sbValue != 0){
            switch(sbValue) {
            case -4:
                break;
            case -3:
                blur5x5();
                break;
            case -2:
                blur3x3(sbValue);
                break;
            case -1:
                blur3x3(sbValue);
                break;
            default:
                highBoost(sbValue);
                break;
            }
            image = prevImg;
            sharpenImg = image.bits();  //sharpen한 연산 결과가져를 옴.
            if (contrastValue > 0) {
                float contrast = (50.0+contrastValue/2)/50.0;
                for(int i = 0; i < imageSize; i ++){
                     *(outimg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + brightValue );
                }
            }
            else {
                contrastValue *= 0.5;
                float contrast = (50.0+contrastValue/2)/50.0;
                for(int i = 0; i < imageSize; i ++){
                     *(outimg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + brightValue);
                }
            }
        }
        else if(sbValue == 0){
            if (contrastValue > 0) {
                float contrast = (50.0+contrastValue)/50.0;
                for(int i = 0; i < imageSize; i ++){
                     *(outimg + i) = LIMIT_UBYTE( 128 + (*(inimg+i)-128) *contrast  + brightValue );
                }
            }
            else {
                contrastValue *= 0.5;
                float contrast = (50.0+contrastValue)/50.0;
                for(int i = 0; i < imageSize; i ++){
                     *(outimg + i) = LIMIT_UBYTE( 128 + (*(inimg+i)-128) *contrast  + brightValue);
                }
            }
        }

    }

    image = QImage(outimg, width, height, QImage::Format_Grayscale8);
    pixmap = pixmap.fromImage(image);
    emit panoImgSend(pixmap);
}
void PanoValueAdjustment::set3x3MaskValue(){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);

    double kernel[3][3] = { {1/9.0, 1/9.0, 1/9.0},  //평균값 필터를 이용한 mask 값
                           {1/9.0, 1/9.0, 1/9.0},
                           {1/9.0, 1/9.0, 1/9.0}};
//    double kernel[3][3] = { {-1, -1, -1},         //라플라시안 필터를 이용한 mask 값
//                           {-1, 9, -1},
//                           {-1, -1, -1}};


    int arr[9] = {0};


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
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i));
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
            *(outimg + i ) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
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
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
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
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) =LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
            }
        }
        else{
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*inimg[((widthCnt+i*1)+(heightCnt+j)*width) ];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
        }
    }
}

void PanoValueAdjustment::highBoost(int sbValue){ //unsharp mask = 원본이미지 + mask 값
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(inimg + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8);
}

void PanoValueAdjustment::blur3x3(int sbValue){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    double edge =0.0, mask = 0.0, median = 0.0;

    if (sbValue == -1) {
        edge = 1/28.0; mask = 1/7.0; median = 2/7.0;
    }
    else if (sbValue == -2) {
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
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8);
}
void PanoValueAdjustment::blur5x5(){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    double blur[5][5] = { {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
                          {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0} };

    unsigned char arr[25]={0,};
    double sum = 0.0;

    int rowSize = width ;
    int imageSize = rowSize * height;

    int widthCnt = 0, heightCnt = -1;

    // inSide
    for(int i = 0; i < imageSize; i++){
        widthCnt = i % rowSize;
        if(i % rowSize == 0) heightCnt++;

        sum = 0.0;
        if ( (widthCnt > 1 && widthCnt < (width-2)*1) && (heightCnt >1 && heightCnt < height-2) ) {

            for(int i = -2; i < 3; i++) {
                for(int j = -2; j < 3; j++) {
                    sum += blur[i+2][j+2]*inimg[(widthCnt+i*1)+((heightCnt+j)*rowSize) ];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
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
             *(outimg + i) = LIMIT_UBYTE(sum);
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
            *(outimg + i) = LIMIT_UBYTE(sum);
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
            *(outimg + i) = LIMIT_UBYTE(sum);
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
            *(outimg + i) = LIMIT_UBYTE(sum);
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
            *(outimg + i) = LIMIT_UBYTE(sum);
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
            *(outimg + i) = LIMIT_UBYTE(sum);
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
            *(outimg + i) = LIMIT_UBYTE(sum);
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
            *(outimg + i) = LIMIT_UBYTE(sum);
        }
    }   //for문 i, imageSize
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8);
}

