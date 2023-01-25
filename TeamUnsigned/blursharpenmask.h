#ifndef BLURSHARPENMASK_H
#define BLURSHARPENMASK_H

#include <QObject>

class BlurSharpenMask : public QObject
{
    Q_OBJECT
public:
    explicit BlurSharpenMask(int, int);
    QImage* sharpen(const uchar* inimg, int value);
    QImage* blur3x3(const uchar* inimg, int value);
    QImage* blur5x5(const uchar* inimg);
    QImage* blur7x7(const uchar* inimg);
    bool operator=(const QImage* other) const;

private:
    QImage* filteredImage;
    const uchar* inimg;
    unsigned char* outimg;
    int width = 0;
    int height = 0;

signals:

};

#endif // BLURSHARPENMASK_H
