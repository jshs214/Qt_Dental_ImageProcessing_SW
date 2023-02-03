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
    void receiveFile(QString);

signals:
    void panoPresetSend(QPixmap&);
    void panoPresetAdj(QPixmap&, int);
private:

    QImage defaultImg, image, prevImg;
    QImage presetImg_1;
    QPixmap pixmap;
    const uchar* inimg;
    unsigned char *mask, *outimg ,*sharpenImg, *copyImg;


    int width, height, imageSize;
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;
    double avg = 0;

    void setPreset_1();

    void gaussian(float);
    void set3x3MaskValue();
    void highBoost(int);
    void ADFilter(unsigned char* ,int);
};

#endif // PANOPRESET_H
