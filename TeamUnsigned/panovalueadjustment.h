#ifndef PANOVALUEADJUSTMENT_H
#define PANOVALUEADJUSTMENT_H

#include <QPixmap>
#include <QObject>

/*
 * panorama 영상의 후처리 연산하는
 * PanoValueAdjustment 클래스
 */
class PanoValueAdjustment : public QObject
{
    Q_OBJECT
public:
    explicit PanoValueAdjustment(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage defaultImg, image, prevImg;  //원본, 임시, 연산한 영상의 QImage 변수
    QImage currentImg, calcImg;         //현재 View에 보이는 영상, 연산중인 영상의 QImage 변수

    /* QImage -> 픽셀 단위 데이터로 변환하여
    연산 처리 수행을 위한 unsigned char 선언 */
    unsigned char* inimg, *mask, *outimg;           //입력, mask값, 출력
    unsigned char *gammaImg, *sharpenImg, *copyImg; //감마, unsharpness
    unsigned char *fftImg, *medianFilterImg;        //FFT, median

    int width = 0, height = 0, imageSize = 0;       //이미지 가로, 세로, 넓이

    //scaled image width, height
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

    double avg = 0;

    void set3x3MaskValue();                         //unsharp mask filter 적용을 위한 (원본 - 평균값필터) mask 값 연산 함수

    void highBoost(unsigned char*, int);            //하이부스트 필터 함수
    void gaussian(unsigned char*, float);           //가우시안 필터 함수
    void ADFilter(unsigned char* ,int);             //비등방성 확산 필터 함수

    void insertion(ushort a[], int n);              //정렬 함수

signals:
    void panoImgSend(QPixmap&);         //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송
    void exitFilterSignal();            //slider 초기화를 위한 시그널

private slots:
    void receiveFile(QPixmap&);                      //영상 load 시 연산 클래스 메모리 할당 및 설정
    void receiveSetPresetImg(QPixmap&);              //프리셋 버튼 클릭 시, 설정 슬롯
    void changePanoValue(int, int, int, int, int);   //panoramaForm의 slider 값에 따라 연산하는 슬롯
    void receivePrev(QPixmap&);                      //평탄화버튼 클릭 시, 평탄화 연산 슬롯
    void setResetImg();                              //리셋 버튼 클릭 시, 초기 설정 슬롯

    void median(int value);         //median filter 연산 슬롯
    void lowPassFFT(int cutoff);    //주파수 내 저역 가우시안 필터링 적용 함수
    void highPassFFT(int cutoff);   //주파수 내 고역 가우시안 필터링 적용 함수

};

#endif // PANOVALUEADJUSTMENT_H
