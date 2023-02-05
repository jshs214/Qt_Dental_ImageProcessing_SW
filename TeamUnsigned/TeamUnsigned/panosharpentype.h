#ifndef PANOSHARPENTYPE_H
#define PANOSHARPENTYPE_H

#include <QObject>
#include <QPixmap>
class PanoSharpenType : public QObject
{
    Q_OBJECT
public:
    explicit PanoSharpenType(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage defaultImg, image;
    const uchar* inimg;
    unsigned char  *mask, *outimg;

    int width = 0, height = 0, imageSize = 0;
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

    void set3x3MaskValue();
    void unSharpMask3x3();

private slots:
    void receiveFile(QString);
    void sharpenType(int);

signals:
    void sendPrevImg(QPixmap&);

};

#endif // PANOSHARPENTYPE_H
