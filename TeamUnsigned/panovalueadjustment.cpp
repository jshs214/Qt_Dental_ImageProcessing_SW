#include "panovalueadjustment.h"
#include "blursharpenmask.h"

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
}

void PanoValueAdjustment::changePanoValue(int brightValue, int contrastValue, int sbValue)
{
    QImage image = defaultImg;

    int width = image.width();
    int imageSize = image.width() * image.height();

    /* 밝기값만 조정되는 case */
    if(contrastValue == 0 && sbValue == 0){

        int widthCnt = 0, heightCnt = -1;
        for(int i = 0; i < imageSize; i ++){
            widthCnt = i % width;
            if(i % width == 0) heightCnt++;

            QRgb rgb = image.pixel(widthCnt, heightCnt);
            int r = LIMIT_UBYTE(qRed(rgb) + brightValue);
            int g = LIMIT_UBYTE(qGreen(rgb) + brightValue);
            int b = LIMIT_UBYTE(qBlue(rgb)+ brightValue);
            image.setPixelColor(widthCnt, heightCnt, QColor(r, g, b));
        }

    }

    /* 대조값만 조정되는 case */
    if(brightValue == 0 && sbValue == 0){

        int widthCnt = 0, heightCnt = -1;
        if (contrastValue > 0) {
            float contrast = (50.0+contrastValue)/50.0;

            for(int i = 0; i < imageSize; i ++){
                widthCnt = i % width;
                if(i % width == 0) heightCnt++;

                QRgb rgb = image.pixel(widthCnt, heightCnt);
                float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) - 128));
                float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) - 128));
                float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) - 128));
                image.setPixelColor(widthCnt, heightCnt, QColor(r, g, b));
            }
        }
        else {
            contrastValue *= 0.5;
            float contrast = (50.0+contrastValue)/50.0;

            for(int i = 0; i < imageSize; i ++){
                widthCnt = i % width;
                if(i % width == 0) heightCnt++;

                QRgb rgb = image.pixel(widthCnt, heightCnt);
                float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) - 128));
                float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) - 128));
                float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) - 128));
                image.setPixelColor(widthCnt, heightCnt, QColor(r, g, b));
            }

        }
    }
    /* 필터값만 조정되는 case */
    if(brightValue == 0 && contrastValue == 0){
        BlurSharpenMask filteredImage(image.width(), image.height());

        switch(sbValue) {
        case 0:
            break;
        case -1:
            image = *filteredImage.blur3x3(image.bits(), sbValue);
            break;
        case -2:
            image = *filteredImage.blur3x3(image.bits(), sbValue);
            break;
        case -3:
            image = *filteredImage.blur5x5(image.bits());
            break;
        case -4:
            image = *filteredImage.blur7x7(image.bits());
            break;
        default:
            image = *filteredImage.sharpen(image.bits(), sbValue*4);
            break;
        }
    }

    /* 두값 or 세 값이 조정되는 case */
    else{
        BlurSharpenMask filteredImage(image.width(), image.height());
        if(sbValue != 0){
            switch(sbValue) {
            case 0:
                break;
            case -1:
                image = *filteredImage.blur3x3(image.bits(), sbValue);
                break;
            case -2:
                image = *filteredImage.blur3x3(image.bits(), sbValue);
                break;
            case -3:
                image = *filteredImage.blur5x5(image.bits());
                break;
            case -4:
                image = *filteredImage.blur7x7(image.bits());
                break;
            default:
                image = *filteredImage.sharpen(image.bits(), sbValue*4);
                break;
            }
        }
        if (contrastValue > 0) {
            float contrast = (50.0+contrastValue)/50.0;
            for(int y=0; y < image.height(); y++){
                for (int x=0; x < image.width(); x++){
                    QRgb rgb = image.pixel(x,y);
                    float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) - 128) + brightValue);
                    float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) - 128) + brightValue);
                    float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) - 128) + brightValue);
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
                    float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) - 128) + brightValue);
                    float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) - 128) + brightValue);
                    float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) - 128) + brightValue);
                    image.setPixelColor(x, y, QColor(r, g, b));
                }
            }
        }


    }

    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));
    emit panoImgSend(pixmap);
}

