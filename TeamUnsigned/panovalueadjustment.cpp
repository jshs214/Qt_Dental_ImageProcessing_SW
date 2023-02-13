#include "panovalueadjustment.h"
#include "fourierprocessing.h"
#include "qdebug.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f


void PanoValueAdjustment::insertion(ushort a[], int n)
{
    int i, j;
    for (i = 1; i < n; i++) {
        int tmp = a[i];
        for (j = i; j > 0 && a[j - 1] > tmp; j--)
            a[j] = a[j - 1];
        a[j] = tmp;
    }
}



PanoValueAdjustment::PanoValueAdjustment(QObject *parent)
    : QObject{parent}
{
}

void PanoValueAdjustment::receiveFile(QPixmap& roadPixmap)
{

    pixmap = roadPixmap;

    //defaultImg 저장.
    defaultImg = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);

    inimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);

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

    copyImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);

    gammaImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(gammaImg, 0, sizeof(unsigned char) * imageSize);

    fftImg = (unsigned char*)malloc(sizeof(unsigned char) * dentalViewWidth * dentalViewHeight);
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth * dentalViewHeight);

    medianFilterImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);

    set3x3MaskValue();  // 영상의 Mask 값 구함

    for(int i = 0; i < imageSize; i ++){ //영상의 평균 value를 저장하기 위함
        avg += inimg[i];
    }

    avg = avg/imageSize;
}

void PanoValueAdjustment::changePanoValue(int brightValue, int contrastValue, int sbValue,
                                          int deNoiseValue, int gammaValue)
{
    QImage image;

    float gamma;
    float contrast;
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    /* 밝기값만 조정되는 case */
    if(contrastValue == 0 && sbValue == 0 && deNoiseValue == 0 && gammaValue ==0){
        int value =  brightValue / 2.5;
        for(int i = 0; i < imageSize; i ++){
            *(outimg + i) = LIMIT_UBYTE( *(inimg + i) + value );
        }
    }
    /* 대비값만 조정되는 case */
    if(brightValue == 0 && sbValue == 0 && deNoiseValue == 0 && gammaValue == 0){
        if (contrastValue > 0)
            contrast = (100.0+contrastValue/2)/100.0;
        else if(contrastValue == 0)
            contrast = 1;
        else {
            contrastValue *= 0.5;
            contrast = (100.0+contrastValue/2)/100.0;
        }
        for(int i = 0; i < imageSize; i ++){
            *(outimg + i) = LIMIT_UBYTE( avg + (*(inimg+i)-avg) *contrast );
        }
    }
    /* unsharp값만 조정되는 case */
    if(brightValue == 0 && contrastValue == 0 && deNoiseValue == 0 && gammaValue ==0){
        if(sbValue < 0)
            gaussian(inimg, (float)sbValue*(-0.5));
        else if(sbValue > 0)
            highBoost(inimg, sbValue);
        else  prevImg = defaultImg;

        image = prevImg;
    }

    /* DeNoising 만 조정되는 case */
    if(brightValue == 0 && contrastValue == 0 && sbValue == 0 && gammaValue ==0) {
        int adfValue = 2 * deNoiseValue;

        ADFilter(inimg, adfValue);

        image = prevImg;
    }
    /* Gamma 조정 case */
    if(brightValue == 0 && contrastValue == 0 && sbValue == 0 && deNoiseValue ==0){
        if(gammaValue ==0){
            for(int i = 0; i < imageSize; i ++){
                *(outimg + i) = *(inimg + i);
            }
        }
        else{   //gammaValue가 0이 아닌 경우
            gamma = 1.0 + gammaValue*0.02;

            for(int i = 0; i < imageSize; i ++){
                *(outimg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
            }
        }
    }
    /* 두 값 이상 조정되는 case */
    else{
        int value =  brightValue / 2.5;
        if(deNoiseValue == 0){  // deNoising이 조정되지 않을 경우
            if(gammaValue ==0){
                for(int i = 0; i < imageSize; i ++){
                    *(gammaImg + i) = *(inimg + i);
                }
            }
            else{   //gammaValue가 0이 아닌 경우
                gamma = 1.0 + gammaValue*0.02;
                for(int i = 0; i < imageSize; i ++){
                    *(gammaImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
                }
            }
            if(sbValue != 0){   // unsharp이 조정된 경우
                if(sbValue < 0)
                    gaussian(gammaImg, (float)sbValue*(-0.5));
                else if(sbValue > 0)
                    highBoost(gammaImg, sbValue);

                image = prevImg;
                sharpenImg = image.bits();  //sharpen한 연산 후 bright, contrast 연산.
                if (contrastValue > 0)
                    contrast = (100.0+contrastValue/2)/100.0;
                else if(contrastValue == 0)
                    contrast = 1;
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(outimg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + value );
                }
            }
            else if(sbValue == 0){ // unsharp이 조정되지 않은 경우
                if (contrastValue > 0){
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                else if(contrastValue == 0) {
                    contrast = 1;
                }
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(outimg + i) = LIMIT_UBYTE( avg + (*(gammaImg+i)- avg) *contrast  + value );
                }

            }
        }
        else { //deNoising 이 조정 된 경우
            int adfValue = 2 * deNoiseValue;

            if(gammaValue ==0){
                for(int i = 0; i < imageSize; i ++){
                    *(gammaImg + i) = *(inimg + i);
                }
            }
            else{   //gammaValue가 0이 아닌 경우
                gamma = 1.0 + gammaValue*0.02;

                for(int i = 0; i < imageSize; i ++){
                    *(gammaImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
                }
            }

            if(sbValue != 0){   // unsharp이 조정된 경우
                if(sbValue < 0)
                    gaussian(gammaImg, (float)sbValue*(-0.5));
                else if(sbValue > 0)
                    highBoost(gammaImg, sbValue);

                image = prevImg;
                sharpenImg = image.bits();  //sharpen한 연산 후 bright, contrast 연산.
                if (contrastValue > 0)
                    contrast = (100.0+contrastValue/2)/100.0;
                else if(contrastValue == 0)
                    contrast = 1;
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(copyImg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + value );
                }

                ADFilter(copyImg, adfValue);
                image = prevImg;
            }
            else if(sbValue == 0){ // unsharp이 조정되지 않은 경우
                if (contrastValue > 0)
                    contrast = (100.0+contrastValue/2)/100.0;
                else if(contrastValue == 0)
                    contrast = 1;
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(copyImg + i) = LIMIT_UBYTE( avg + (*(gammaImg+i)-avg) *contrast  + value );
                }
                ADFilter(copyImg, adfValue);
                image = prevImg;
            }

        }

    }

    image = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
    pixmap = pixmap.fromImage(image);
    emit panoImgSend(pixmap);

    calcImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();      //연산 결과 이미지 저장
}

void PanoValueAdjustment::set3x3MaskValue(){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);

    double kernel[3][3] = { {1/9.0, 1/9.0, 1/9.0},  //평균값 필터를 이용한 mask 값
                            {1/9.0, 1/9.0, 1/9.0},
                            {1/9.0, 1/9.0, 1/9.0}};

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
void PanoValueAdjustment::highBoost(unsigned char* in, int sbValue){ //unsharp mask = 원본이미지 + mask 값
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(in + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }

    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}


void PanoValueAdjustment::receivePrev(QPixmap& pixmap)  // 평탄화
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    QImage image;
    image = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();
    image = image.convertToFormat(QImage::Format_Grayscale8).copy();

    unsigned char *histoInimg;
    histoInimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    int histo[256], sum_of_h[256];
    int value,k;
    float constant;

    for(int i = 0; i < 256; i ++) {
        histo[i] =0;
        sum_of_h[i] = 0;
    }

    for (int i = 0; i < imageSize; i++) {   //histogram 분포
        value = histoInimg[i];
        histo[value] += 1;
        outimg[i] = value;
    }

    //histogram
    for (int i = 0, sum = 0; i < 256; i++){
        sum += histo[i];
        sum_of_h[i] = sum;
    }

    /* constant = new # of gray levels div by area */
    constant = (float)(256) / (float)(height * width);
    for (int i = 0; i < imageSize; i++) {
        k = outimg[i];
        outimg[i] = LIMIT_UBYTE( sum_of_h[k] * constant );

        inimg[i] = outimg[i];   //평활화 후 이미지 연산은 평활화 한 이미지로 진행
    }

    image = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
    pixmap = pixmap.fromImage(image);
    emit panoImgSend(pixmap);
}


void PanoValueAdjustment::gaussian(unsigned char* in, float sigma){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    float* pBuf;
    pBuf = (float*)malloc(sizeof(float) * width * height);

    int i, j, k, x;

    int dim = static_cast<int>(2 * 4 * sigma + 1.0);

    if (dim < 3) dim = 3;
    if (dim % 2 == 0) dim++;
    int dim2 = dim / 2;

    float* pMask = new float[dim];

    for (i = 0; i < dim; i++) {
        x = i - dim2;
        pMask[i] = exp(-(x*x) / (2 * sigma * sigma)) / (sqrt(2 * PI) * sigma);
    }

    float sum1, sum2;

    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {

            sum1 = sum2 = 0.f;
            for (k = 0; k < dim; k++) {

                x = k - dim2 + j;
                if (x>= 0 && x <height) {
                    sum1 += pMask[k];
                    sum2 += (pMask[k] * in[x + i*height]);
                }
            }
            pBuf[j+ i*height] = sum2 / sum1;
        }
    }

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            sum1 = sum2 = 0.f;

            for (k = 0; k < dim; k++) {

                x = k - dim2 + i;
                if ( x>= 0 && x < width) {
                    sum1 += pMask[k];
                    sum2 += (pMask[k] * pBuf[j*width + x]);
                }
            }
            outimg[i + j * width] = sum2 / sum1;
        }
    }
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();

    free(pBuf);
    delete[] pMask;
}

void PanoValueAdjustment::ADFilter(unsigned char * inimg, int iter){    //deNoising , 다른 연산 수행 함수
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    float lambda = 0.25;
    float k = 4;

    auto copy(inimg);

    /* iter 횟수만큼 비등방성 확산 알고리즘 수행 */
    int i;
    float gradn=0.0, grads=0.0, grade=0.0, gradw=0.0;
    float gcn=0.0, gcs=0.0, gce=0.0, gcw=0.0;
    float k2 = k * k;

    for (i = 0; i < iter; i++)
    {
        int widthCnt = 0, heightCnt = -1;
        for (int i = 0; i < imageSize; i += 1) {
            widthCnt = i % width;
            if(i % width == 0) heightCnt++;

            gradn = copy[(heightCnt - 1) * width + widthCnt] - copy[heightCnt * width + widthCnt];
            grads = copy[(heightCnt + 1) * width + widthCnt] - copy[heightCnt * width + widthCnt];
            grade = copy[heightCnt * width + (widthCnt-1)] - copy[heightCnt * width + widthCnt];
            gradw = copy[heightCnt * width + (widthCnt+1)] - copy[heightCnt * width + widthCnt];

            gcn = gradn / (1.0f + gradn * gradn / k2);
            gcs = grads / (1.0f + grads * grads / k2);
            gce = grade / (1.0f + grade * grade / k2);
            gcw = gradw / (1.0f + gradw * gradw / k2);

            outimg[heightCnt * width + widthCnt] = copy[heightCnt * width + widthCnt] + lambda * (gcn + gcs + gce + gcw);
        }
        if (i < iter - 1)
            std::memcpy((unsigned char*)copy, outimg, sizeof(unsigned char) * width * height);
    }

    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}

void PanoValueAdjustment::median(int value) {
    Q_UNUSED(value);

    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    int imageSize = width * height;
    int rowSize = width;
    int widthCnt = 0, heightCnt = -1;
    int cnt = 0;

    ushort arr[9] = { 0, };

    for (int i = 0; i < imageSize; i++) {
        widthCnt = i % width;
        if (i % width == 0) heightCnt++;

        if (widthCnt == 0) {
            //좌측 상단 Vertex
            if (heightCnt == 0) {
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[2] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }
            //좌측 하단 Vertex
            else if (heightCnt == height - 1) {
                arr[0] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[widthCnt + ((heightCnt * rowSize))];
                arr[8] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
            }
            else {
                arr[0] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (widthCnt == (rowSize - 1)) {
            //우측 상단 Vertex
            if (heightCnt == 0) {
                arr[0] = arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[7] = arr[8] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
            }
            //우측 하단 Vertex
            else if (heightCnt == height - 1) {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[1] = arr[2] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[widthCnt + (heightCnt * rowSize)];
            }
            else {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[2] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[5] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt + 1) * rowSize)];
                arr[8] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (heightCnt == 0) {
            if (widthCnt != 1 && widthCnt != rowSize - 1) {
                arr[0] = arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[1] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[2] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt + 1) * rowSize)];
                arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (heightCnt == (height - 1)) {
            if (widthCnt != 1 && widthCnt != rowSize - 1) {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[4] = arr[7] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[5] = arr[8] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else {
            cnt = 0;
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    arr[cnt++] = inimg[((widthCnt + i) + (heightCnt + j) * width)];
                }
            }
            insertion(arr, 9);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
    }
    QPixmap medianPixmap;

    currentImg = QImage(medianFilterImg, width, height, QImage::Format_Grayscale8).copy();
    medianPixmap = pixmap.fromImage(currentImg);

    inimg = currentImg.bits();

    emit panoImgSend(medianPixmap);
    emit exitFilterSignal();
}


void PanoValueAdjustment::lowPassFFT(int cutoff) {
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth*dentalViewHeight);

    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    FourierProcessing fourier(dentalViewWidth, dentalViewHeight, inimg);
    fourier.lowPassGaussian(fftImg, cutoff);
    currentImg = QImage(fftImg, dentalViewWidth, dentalViewHeight, QImage::Format_Grayscale8).copy();
    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);
    emit panoImgSend(fourierPixmap);

    inimg = currentImg.bits();

    fourier.deleteMemory();
    emit exitFilterSignal();
}

void PanoValueAdjustment::highPassFFT(int cutoff) {
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth*dentalViewHeight);

    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    FourierProcessing fourier(dentalViewWidth, dentalViewHeight, inimg);
    fourier.highFrequencyPass(fftImg, cutoff);
    currentImg = QImage(fftImg, dentalViewWidth, dentalViewHeight, QImage::Format_Grayscale8).copy();

    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);
    emit panoImgSend(fourierPixmap);

    inimg = currentImg.bits();

    fourier.deleteMemory();
    emit exitFilterSignal();
}



/* preset img 받고 전송 */
void PanoValueAdjustment::receiveSetPresetImg(QPixmap& prePixmap){
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    QImage presetImg;
    presetImg = prePixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();
    currentImg = presetImg.convertToFormat(QImage::Format_Grayscale8).copy();

    inimg = currentImg.bits();
}

void PanoValueAdjustment::setResetImg() {
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();

    calcImg = QImage();
}
