#include "setbrightness.h"
#include "qdebug.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

SetBrightness::SetBrightness(QObject *parent)
    : QObject{parent}
{


}

void SetBrightness::receiveFile(QString file)
{
    if(file != "")
        pixmap.load(file);
    else
        qDebug()<< __FUNCTION__ << __LINE__;

    defaultImage = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();

}

void SetBrightness::changeBrightness(int value)
{
    QImage image = defaultImage;
    for(int y=0; y < image.height(); y++){
        for (int x=0; x < image.width(); x++){
            QRgb rgb = image.pixel(x,y);
            int r = LIMIT_UBYTE(qRed(rgb) + value);
            int g = LIMIT_UBYTE(qGreen(rgb) + value);
            int b = LIMIT_UBYTE(qBlue(rgb)+ value);
            image.setPixelColor(x, y, QColor(r, g, b));
        }
    }
    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));

    emit panoBrightPixmap(pixmap);
}
