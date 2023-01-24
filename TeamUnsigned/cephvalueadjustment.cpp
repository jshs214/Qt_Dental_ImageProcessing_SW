#include "cephvalueadjustment.h"
#include "qdebug.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

CephValueAdjustment::CephValueAdjustment(QObject *parent)
    : QObject{parent}
{

}

void CephValueAdjustment::receiveFile(QString file)
{
    pixmap.load(file);
    defaultImg = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();
}

void CephValueAdjustment::changeCephValue(int brightValue, int contrastValue)
{
    QImage image = defaultImg;

    /* 밝기값만 조정되는 case */
    if(contrastValue == 0){
        for(int y=0; y < image.height(); y++){
            for (int x=0; x < image.width(); x++){
                QRgb rgb = image.pixel(x,y);
                int r = LIMIT_UBYTE(qRed(rgb) + brightValue);
                int g = LIMIT_UBYTE(qGreen(rgb) + brightValue);
                int b = LIMIT_UBYTE(qBlue(rgb)+ brightValue);
                image.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }

    /* 대조값만 조정되는 case */
    if(brightValue == 0){
        if (contrastValue > 0) {
            float contrast = (50.0+contrastValue)/50.0;
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
            contrastValue *= 0.5;
            float contrast = (50.0+contrastValue)/50.0;
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
    }

    /* 두 값이 조정되는 case */
    if(brightValue != 0 && contrastValue != 0){
        if (contrastValue > 0) {
            float contrast = (50.0+contrastValue)/50.0;
            for(int y=0; y < image.height(); y++){
                for (int x=0; x < image.width(); x++){
                    QRgb rgb = image.pixel(x,y);
                    float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) + brightValue - 128));
                    float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) + brightValue - 128));
                    float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb)  + brightValue - 128));
                    image.setPixelColor(x, y, QColor(r, g, b));
                }
            }
        }

        else {
            contrastValue *= 0.5;
            float contrast = (50.0+contrastValue)/50.0;
            for(int y=0; y < image.height(); y++){
                for (int x=0; x < image.width(); x++){
                    QRgb rgb = image.pixel(x,y);
                    float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) + brightValue - 128));
                    float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) + brightValue - 128));
                    float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) + brightValue - 128));
                    image.setPixelColor(x, y, QColor(r, g, b));
                }
            }
        }
    }

    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));
    emit cephImgSend(pixmap);
    qDebug()<<__LINE__ << brightValue << contrastValue;
}

