#ifndef PANOPRESET_H
#define PANOPRESET_H

#include <QObject>
#include <QImage>
#include <QPixmap>

class PanoPreset : public QObject
{
    Q_OBJECT
public:
    explicit PanoPreset(QObject *parent = nullptr);

private slots:
    void receievePreset(int);
    void receiveFile(QPixmap&);

signals:
    void panoPresetSend(QPixmap&);
    void panoPresetAdj(QPixmap&);

private:
    QImage defaultImg, image;
    QImage presetImg;

    QPixmap pixmap;
    unsigned char * inimg, *mask, *outimg, *copyImg, *copyImg2;

    int width, height, imageSize;
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;
    double avg = 0;

    void setPreset_1();
    void setPreset_2();
    void setPreset_3();
    void setPreset_4();
    void setPreset_5();
    void setPreset_6();

    void set3x3MaskValue();

    unsigned char* gaussian(float);
    unsigned char* highBoost(int);
    unsigned char* highBoost(unsigned char*, int);
    unsigned char* ADFilter(unsigned char* ,int);
};

#endif // PANOPRESET_H
