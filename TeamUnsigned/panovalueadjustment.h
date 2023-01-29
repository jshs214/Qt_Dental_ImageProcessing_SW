#ifndef PANOVALUEADJUSTMENT_H
#define PANOVALUEADJUSTMENT_H

#include <QPixmap>
#include <QObject>

class QPixmap;
class PanoValueAdjustment : public QObject
{
    Q_OBJECT
public:
    explicit PanoValueAdjustment(QObject *parent = nullptr);

private:
    QPixmap pixmap, sharpenPixmap;
    QImage defaultImg, image;
    const uchar* inimg;
    unsigned char  *mask, *outimg, *sharpenImg;
    QImage prevImg;

    int width = 0, height = 0, imageSize = 0;
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;
    double avg = 0;

    void set3x3MaskValue();

    void highBoost(int);
    void blur3x3(int);
    void blur5x5();
private slots:
    void receiveFile(QString);
    void changePanoValue(int, int, int);

signals:
    void panoImgSend(QPixmap&);
    void sendSharpenValue(int);
};

#endif // PANOVALUEADJUSTMENT_H
