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
    prevImg = defaultImg;

}

void PanoValueAdjustment::changeBrightness(int value)
{
    QImage image;
    image = defaultImg;

    for(int y=0; y < image.height(); y++){
        for (int x=0; x < image.width(); x++){
            QRgb rgb = image.pixel(x,y);
            int r = LIMIT_UBYTE(qRed(rgb) + value);
            int g = LIMIT_UBYTE(qGreen(rgb) + value);
            int b = LIMIT_UBYTE(qBlue(rgb)+ value);
            image.setPixelColor(x, y, QColor(r, g, b));
        }
    }
    prevImg = image;
    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));

    emit panoBrightPixmap(pixmap);
}

void PanoValueAdjustment::changeContrast(int value)
{
    QImage image;

    image = prevImg;

    if (value > 0) {
        float contrast = (50.0+value)/50.0;
        for(int y=0; y < image.height(); y++){
            for (int x=0; x < image.width(); x++){
                QRgb rgb = image.pixel(x,y);
                float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) - 128));
                float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) - 128));
                float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) - 128));
                image.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }

    else {
        value *= 0.5;
        float contrast = (50.0+value)/50.0;
        for(int y=0; y < image.height(); y++){
            for (int x=0; x < image.width(); x++){
                QRgb rgb = image.pixel(x,y);
                float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) - 128));
                float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) - 128));
                float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) - 128));
                image.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }

    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));

    emit panoContrastPixmap(pixmap);
}
