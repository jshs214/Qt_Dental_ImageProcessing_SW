#include "panovalueadjustment.h"
#include "fourierprocessing.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f

PanoValueAdjustment::PanoValueAdjustment(QObject *parent)
    : QObject{parent}
{
}
/* 평균값 필터를 이용한 영상의 mask 값 연산 함수
 * 영상의 mask 값은 *mask에 저장
 */
void PanoValueAdjustment::set3x3MaskValue()
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);

    double kernel[3][3] = { {1/9.0, 1/9.0, 1/9.0},  //평균값 필터를 이용한 mask 값
                            {1/9.0, 1/9.0, 1/9.0},
                            {1/9.0, 1/9.0, 1/9.0}};

    int arr[9] = {0};


    int x = 0, y = -1, cnt = 0;
    for(int i = 0; i < imageSize; i ++){
        x = i % width;
        if(i % width == 0) y++;

        if(x==0){
            //LeftUpVertex
            if(y==0){
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[x+(y*width) ];
                arr[2] = arr[5] = inimg[x+1 + (y*width) ];
                arr[6] = arr[7] = inimg[x+ ((y+1)*width)  ];
                arr[8] = inimg[x+1+((y+1)*width) ];
            }
            //LeftDownVertex
            else if(y==height-1){
                arr[0] = arr[1] =inimg[x+((y-1)*width) ];
                arr[2] = inimg[x+1 + ((y-1)*width) ];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[x+(y*width)  ];
                arr[8] = arr[5] = inimg[x+1 + (y*width)  ];
            }
            else{
                arr[0] = arr[1] = inimg[x+( (y-1)*width)  ];
                arr[2] = inimg[x+1+( (y-1)*width)  ];
                arr[3] = arr[4] = inimg[x+(y*width) ];
                arr[5] = inimg[x+1+(y*width) ];
                arr[6] = arr[7] = inimg[x+ ( (y+1)*width)  ];
                arr[8] = inimg[x+1+( (y+1)*width)  ];
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

        else if( x==(width*1 -1) ){
            //RightUpVertex
            if(y==0){
                arr[0] = arr[3] = inimg[x-1 + (y*width)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[x + (y*width)  ];
                arr[6] = inimg[x-1 + ((y-1)*width)  ];
                arr[7] = arr[8] = inimg[x+((y+1)*width) ];
            }
            //RightDownVertex
            else if(y==height-1){
                arr[0] = inimg[x-1 + ((y-1)*width)  ];
                arr[1] = arr[2] = inimg[x-1 +((y-1)*width)  ];
                arr[3] = arr[6] = inimg[x-1+(y*width) ];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[x+(y*width) ];
            }
            else{
                arr[0] = inimg[x-1 + ((y-1)*width)  ];
                arr[2] = arr[1] = inimg[x + ((y-1)*width)  ];
                arr[3] = inimg[x-1 + (y*width)  ];
                arr[5] = arr[4] = inimg[x+(y*width)  ];
                arr[6] = inimg[x-1 + ((y+1)*width)  ];
                arr[8] = arr[7] = inimg[x+((y+1)*width)  ];
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
        else if(y==0){
            if( x!=1 && x!=width-1 ){
                arr[0] = arr[3] = inimg[x-1+(y*width)  ];
                arr[1] = arr[4] = inimg[x+(y*width) ];
                arr[2] = arr[5] = inimg[x+1+(y*width)  ];
                arr[6] = inimg[x-1+((y+1)*width)  ];
                arr[7] = inimg[x+((y+1)*width)  ];
                arr[8] = inimg[x+1 + ((y+1)*width)  ];

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
        else if( y ==(height -1) ){
            if( x!=1 && x!=width-1 ){
                arr[0] = inimg[x-1+((y-1)*width) ];
                arr[1] = inimg[x+((y-1)*width) ];
                arr[2] = inimg[x+1+((y-1)*width) ];
                arr[3] = arr[6] = inimg[x-1+(y*width) ];
                arr[4] = arr[7] = inimg[x+(y*width) ];
                arr[5] = arr[8] = inimg[x+1+(y*width) ];
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
                    sum += kernel[i+1][j+1]*inimg[((x+i*1)+(y+j)*width) ];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
        }
    }

}
/* 하이부스트 필터(선예도) 함수
 * @param 연산할 이미지의 픽셀 데이터
 * @param unsharp Value
 * 함수 호출 후 연산 결과는 prevImg에 저장
 */
void PanoValueAdjustment::highBoost(unsigned char* in, int sbValue)
{ //unsharp mask = 원본이미지 + mask 값
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(in + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }

    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 가우시안 필터(블러) 함수
 * 2차원 가우시안 함수 값을 이용하여 마스크를 생성, 입력 영상과 마스크 연산을 수행
 * x 방향과 y 방향으로의 1차원 마스크 연산을 각각 수행
 *
 * @param 연산할 이미지의 픽셀 데이터
 * @param sigma 값
 * 함수 호출 후 연산 결과는 prevImg에 저장
 */
void PanoValueAdjustment::gaussian(unsigned char* in, float sigma)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    float* pBuf;
    pBuf = (float*)malloc(sizeof(float) * width * height);

    int i, j, k, x;

    int dim = static_cast<int>(2 * 4 * sigma + 1.0);    //마스크 크기

    if (dim < 3) dim = 3;                               //최소 마스크 크기 계산(3x3)
    if (dim % 2 == 0) dim++;                            //마스크 크기는 홀수
    int dim2 = dim / 2;

    //1차원 가우시안 마스크 생성
    float* pMask = new float[dim];

    for (i = 0; i < dim; i++) {
        //x 값의 범위는 -4 * sigma부터 +4 * sigma까지
        x = i - dim2;
        //평균이 0이고, 표준 편차가 sigma인 1차원 가우시안 분포의 함수 식 표현
        pMask[i] = exp(-(x*x) / (2 * sigma * sigma)) / (sqrt(2 * PI) * sigma);
    }

    float sum1, sum2;

    //세로 방향 마스크 연산
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
    //가로 방향 마스크 연산
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
/* 비등방성 확산 필터(DeNoise) 함수
 * 상하 좌우 대칭의 형태를 갖는 필터 마스크를 사용하는 필터를 등방성 필터
 *
 * @param 연산할 이미지의 픽셀 데이터
 * @param 비등방성 필터 반복 수
 * 함수 호출 후 연산 결과는 prevImg에 저장
*/
void PanoValueAdjustment::ADFilter(unsigned char * in, int iter)
{    //deNoising , 다른 연산 수행 함수
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    unsigned char * copy;
    copy = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memcpy(copy, in, sizeof(unsigned char) * imageSize);

    int i;
    float gradn=0.0, grads=0.0, grade=0.0, gradw=0.0;
    float gcn=0.0, gcs=0.0, gce=0.0, gcw=0.0;
    float lambda = 0.25;
    float k = 4;
    float k2 = k * k;

    /* iter 횟수만큼 비등방성 확산 알고리즘 수행 */
    for (i = 0; i < iter; i++)
    {
        int x = 0, y = -1;
        for (int j = 0; j < imageSize; j += 1) {
            x = j % width;
            if(j % width == 0) y++;

            if(y == 0) {
                //(0,0)
                if(x == 0) {
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcs = grads / (1.0f + grads * grads / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcs + gcw);
                }
                //(width,0)
                else if(x == width -1) {
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];

                    gcs = grads / (1.0f + grads * grads / k2);
                    gce = grade / (1.0f + grade * grade / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcs + gce);
                }
                //(x,0)
                else {
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcs = grads / (1.0f + grads * grads / k2);
                    gce = grade / (1.0f + grade * grade / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcs + gce + gcw);
                }
            }

            else if(y == height - 1) {
                //(0, height)
                if(x == 0) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcn + gcw);
                }
                //(width, height)
                else if (x == width - 1) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gce = grade / (1.0f + grade * grade / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcn + gce);
                }
                //(x, height)
                else {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gce = grade / (1.0f + grade * grade / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcn + gce + gcw);
                }
            }

            else if(x == 0) {
                //(0, y)
                if(0 < y && y < height - 1) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gcs = grads / (1.0f + grads * grads / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcn + gcs + gcw);
                }
            }

            else if(x == width - 1) {
                //(width, y)
                if(0 < y && y < height - 1) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gcs = grads / (1.0f + grads * grads / k2);
                    gce = grade / (1.0f + grade * grade / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcn + gcs + gce);
                }
            }

            else {
                //비등방성 확산 필터 수식(안쪽)
                gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                grads = copy[(y + 1) * width + x] - copy[y * width + x];
                grade = copy[y * width + (x-1)] - copy[y * width + x];
                gradw = copy[y * width + (x+1)] - copy[y * width + x];

                gcn = gradn / (1.0f + gradn * gradn / k2);
                gcs = grads / (1.0f + grads * grads / k2);
                gce = grade / (1.0f + grade * grade / k2);
                gcw = gradw / (1.0f + gradw * gradw / k2);

                outimg[y * width + x] = copy[y * width + x]
                        + lambda * (gcn + gcs + gce + gcw);
            }

        }
        if (i < iter - 1)
            memcpy(copy, outimg, sizeof(unsigned char) * width * height);
    }
    free(copy);
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}

/* median filter 사용 위한 정렬 함수
 * @param 배열
 * @param 배열의 크기
 */
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

/* 영상 load 시 연산 클래스 메모리 할당 및 설정
 * @param cephaloForm 에서 Load 하거나 DB에서 load 한 영상 Pixmap
 */
void PanoValueAdjustment::receiveFile(QPixmap& roadPixmap)
{
    pixmap = roadPixmap;

    //defaultImg 저장.
    defaultImg = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();

    //연산을 위한 QImage 저장
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);

    //QImage를 배열로 복사
    inimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    inimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    outimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    mask = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    sharpenImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    copyImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    gammaImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    fftImg = (unsigned char*)malloc(sizeof(unsigned char) * dentalViewWidth * dentalViewHeight);
    medianFilterImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);
    memset(sharpenImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(gammaImg, 0, sizeof(unsigned char) * imageSize);
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth * dentalViewHeight);
    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);

    set3x3MaskValue();  // 영상의 Mask 값 구함

    /* 영상의 평균 value를 저장하기 위한 연산 */
    for(int i = 0; i < imageSize; i ++){
        avg += inimg[i];
    }
    avg = avg/imageSize;
}
/* 프리셋 버튼 클릭 시, 설정 슬롯
 * @param 프리셋 클래스의 presetImg
 */
void PanoValueAdjustment::receiveSetPresetImg(QPixmap& prePixmap)
{
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    QImage presetImg;
    presetImg = prePixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();
    currentImg = presetImg.convertToFormat(QImage::Format_Grayscale8).copy();

    inimg = currentImg.bits();
}
/* panoramaForm의 slider 값에 따라 연산하는 슬롯
 * @param 밝기 값
 * @param 대조 값
 * @param 선예도
 * @param deNoising
 * @param 감마 값
 */
void PanoValueAdjustment::changePanoValue(int brightValue, int contrastValue, int sbValue,
                                          int deNoiseValue, int gammaValue)
{
    QImage image;

    float gamma;
    float contrast;
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(gammaImg, 0, sizeof(unsigned char) * imageSize);

    if(brightValue == 0 && contrastValue == 0 && sbValue == 0  && gammaValue !=0 && deNoiseValue != 0){
        gamma = 1.0 + gammaValue*0.02;

        for(int i = 0; i < imageSize; i ++){
            *(gammaImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
        }
        ADFilter(gammaImg, deNoiseValue * 2);
        image = prevImg;
    }
    /* 밝기값만 조정되는 case */
    if(contrastValue == 0 && sbValue == 0 && deNoiseValue == 0 && gammaValue ==0){
        int value =  brightValue / 2.5;
        for(int i = 0; i < imageSize; i ++){
            *(outimg + i) = LIMIT_UBYTE( *(inimg + i) + value );
        }
    }
    /* 대조값만 조정되는 case */
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
        if(inimg != nullptr)
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
    if(brightValue != 0) {
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

    else {
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
    emit panoImgSend(pixmap);   //후처리 연산 결과를 panoramaForm으로 시그널 전송

    //현재 연상중인 이미지 저장
    calcImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();      //연산 결과 이미지 저장
}

/* 평탄화버튼 클릭 시, 평탄화 연산 슬롯
 * @param 현재 View에 있는 후처리 영상
 */
void PanoValueAdjustment::receivePrev(QPixmap& pixmap)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    //현재 View의 QPixmap -> QImage 변환
    QImage image;
    image = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();
    image = image.convertToFormat(QImage::Format_Grayscale8).copy();

    //QImage -> 배열로 복사
    unsigned char *histoInimg;
    histoInimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    int histo[256], sum_of_h[256];
    int value,k;
    float constant;

    /* 히스토그램 배열 초기화 */
    for(int i = 0; i < 256; i ++) {
        histo[i] =0;
        sum_of_h[i] = 0;
    }
    /* 히스토그램 분포 저장 */
    for (int i = 0; i < imageSize; i++) {   //histogram 분포
        value = histoInimg[i];
        histo[value] += 1;
        outimg[i] = value;
    }
    /* 히스토그램 누적 합 계산 */
    for (int i = 0, sum = 0; i < 256; i++){
        sum += histo[i];
        sum_of_h[i] = sum;
    }

    /* 히스토그램 평활화를 위한 정규화 계수
     * constant = ( max gray value x 이미지 사이즈)  */
    constant = (float)(256) / (float)(height * width);
    for (int i = 0; i < imageSize; i++) {
        k = outimg[i];
        outimg[i] = LIMIT_UBYTE( sum_of_h[k] * constant );

        inimg[i] = outimg[i];   //평활화 후 이미지 연산은 평활화 한 이미지로 진행
    }

    image = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
    pixmap = pixmap.fromImage(image);
    emit panoImgSend(pixmap);   //평탄화 연산 결과를 panoramaForm으로 시그널 전송
}
/* 리셋 버튼 클릭 시, 초기 설정 슬롯 */
void PanoValueAdjustment::setResetImg()
{
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();

    calcImg = QImage();
}
/* median filter 연산 슬롯
 * @parama UNUSED
 */
void PanoValueAdjustment::median(int value)
{
    Q_UNUSED(value);

    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    int imageSize = width * height;
    int rowSize = width;
    int x = 0, y = -1;
    int cnt = 0;

    ushort arr[9] = { 0, };

    for (int i = 0; i < imageSize; i++) {
        x = i % width;
        if (i % width == 0) y++;

        if (x == 0) {
            //좌측 상단 Vertex
            if (y == 0) {
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[x + (y * rowSize)];
                arr[2] = arr[5] = inimg[x + 1 + (y * rowSize)];
                arr[6] = arr[7] = inimg[x + ((y + 1) * rowSize)];
                arr[8] = inimg[x + 1 + ((y + 1) * rowSize)];
            }
            //좌측 하단 Vertex
            else if (y == height - 1) {
                arr[0] = arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[2] = inimg[x + 1 + ((y - 1) * rowSize)];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[x + ((y * rowSize))];
                arr[8] = arr[5] = inimg[x + 1 + (y * rowSize)];
            }
            else {
                arr[0] = arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[2] = inimg[x + 1 + ((y - 1) * rowSize)];
                arr[3] = arr[4] = inimg[x + (y * rowSize)];
                arr[5] = inimg[x + 1 + (y * rowSize)];
                arr[6] = arr[7] = inimg[x + ((y + 1) * rowSize)];
                arr[8] = inimg[x + 1 + ((y + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else if (x == (rowSize - 1)) {
            //우측 상단 Vertex
            if (y == 0) {
                arr[0] = arr[3] = inimg[x - 1 + (y * rowSize)];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[x + (y * rowSize)];
                arr[6] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[7] = arr[8] = inimg[x + ((y + 1) * rowSize)];
            }
            //우측 하단 Vertex
            else if (y == height - 1) {
                arr[0] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[1] = arr[2] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[3] = arr[6] = inimg[x - 1 + (y * rowSize)];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[x + (y * rowSize)];
            }
            else {
                arr[0] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[2] = arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[3] = inimg[x - 1 + (y * rowSize)];
                arr[5] = arr[4] = inimg[x + (y * rowSize)];
                arr[6] = inimg[x - 1 + ((y + 1) * rowSize)];
                arr[8] = arr[7] = inimg[x + ((y + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else if (y == 0) {
            if (x != 1 && x != rowSize - 1) {
                arr[0] = arr[3] = inimg[x - 1 + (y * rowSize)];
                arr[1] = arr[4] = inimg[x + (y * rowSize)];
                arr[2] = arr[5] = inimg[x + 1 + (y * rowSize)];
                arr[6] = inimg[x - 1 + ((y + 1) * rowSize)];
                arr[7] = inimg[x + ((y + 1) * rowSize)];
                arr[8] = inimg[x + 1 + ((y + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else if (y == (height - 1)) {
            if (x != 1 && x != rowSize - 1) {
                arr[0] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[2] = inimg[x + 1 + ((y - 1) * rowSize)];
                arr[3] = arr[6] = inimg[x - 1 + (y * rowSize)];
                arr[4] = arr[7] = inimg[x + (y * rowSize)];
                arr[5] = arr[8] = inimg[x + 1 + (y * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else {
            cnt = 0;
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    arr[cnt++] = inimg[((x + i) + (y + j) * width)];
                }
            }
            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
    }
    QPixmap medianPixmap;

    currentImg = QImage(medianFilterImg, width, height, QImage::Format_Grayscale8).copy();
    medianPixmap = pixmap.fromImage(currentImg);

    inimg = currentImg.bits();      //inimg를 연산한 이미지 데이터로 복사

    emit panoImgSend(medianPixmap); //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송
    emit exitFilterSignal();        //필터 연산 후 panoramaForm으로 시그널 전송
}

/* 주파수 내 저역 가우시안 필터링 적용 함수
 * @param 저역 통과 대역 설정 (filterButton에서 입력한 값)
 */
void PanoValueAdjustment::lowPassFFT(int cutoff)
{
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth*dentalViewHeight);

    /* 이미 밝기, 대조, 선예도, 감마, 노이즈 제거 연산이 되어있을 경우
     * 연산된 이미지 사용 */
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    /* 푸리에 변환을 위한 객체 생성 */
    FourierProcessing fourier(dentalViewWidth, dentalViewHeight, inimg);

    //가우시안 필터링 적용
    fourier.lowPassGaussian(fftImg, cutoff);
    currentImg = QImage(fftImg, dentalViewWidth, dentalViewHeight, QImage::Format_Grayscale8).copy();

    //필터링 적용한 이미지 전달
    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);
    emit panoImgSend(fourierPixmap);    //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송

    inimg = currentImg.bits();

    fourier.deleteMemory();             //메모리 제거
    emit exitFilterSignal();            //필터 연산 후 panoramaForm으로 시그널 전송
}

/* 주파수 내 고역 가우시안 필터링 적용 함수
 * @param 고역 통과 대역 설정 (filterButton에서 입력한 값)
 */
void PanoValueAdjustment::highPassFFT(int cutoff)
{
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth*dentalViewHeight);

    /* 이미 밝기, 대조, 선예도, 감마, 노이즈 제거 연산이 되어있을 경우
     * 연산된 이미지 사용 */
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    /* 푸리에 변환을 위한 객체 생성 */
    FourierProcessing fourier(dentalViewWidth, dentalViewHeight, inimg);

    //가우시안 필터링 적용
    fourier.highFrequencyPass(fftImg, cutoff);
    currentImg = QImage(fftImg, dentalViewWidth, dentalViewHeight, QImage::Format_Grayscale8).copy();

    //필터링 적용한 이미지 전달
    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);
    emit panoImgSend(fourierPixmap);    //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송

    inimg = currentImg.bits();

    fourier.deleteMemory();             //메모리 제거
    emit exitFilterSignal();            //필터 연산 후 panoramaForm으로 시그널 전송
}



