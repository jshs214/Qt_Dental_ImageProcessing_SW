#ifndef CEPHVALUEADJUSTMENT_H
#define CEPHVALUEADJUSTMENT_H

#include <QPixmap>
#include <QObject>

class CephValueAdjustment : public QObject
{
    Q_OBJECT
public:
    explicit CephValueAdjustment(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage defaultImg, image, prevImg;
    QImage currentImg, calcImg;

    unsigned char* inimg;
    unsigned char  *mask, *outimg, *gammaImg, *sharpenImg, *copyImg;
    unsigned char *fftImg, *medianFilterImg, *blenImg;

    int width = 0, height = 0, imageSize = 0;

    int cephViewWidth = 800;
    int cephViewHeight = 600;
    double avg = 0;

    void set3x3MaskValue();

   void highBoost(unsigned char* , int);
   void gaussian(unsigned char* , float);
   void ADFilter(unsigned char* ,int);

private slots:
   void receiveFile(QPixmap&);  //defaultImg receive 수정 해야댈 듯
   void changeCephValue(int, int, int, int, int);   //밝기, 대조, unsharp, deNoise
   void receivePrev(QPixmap&); //prevImg receive
   void receiveSetPresetImg(QPixmap&);
   void setResetImg();
   void median(int value);
   void lowPassFFT(int cutoff);
   void highPassFFT(int cutoff);

signals:
    void cephImgSend(QPixmap&);
    void exitFilterSignal();
};

#endif // CEPHVALUEADJUSTMENT_H
