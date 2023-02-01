#include "panopreset.h"
#include "qdebug.h"

#include <QImage>
#include <QPixmap>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

PanoPreset::PanoPreset(QObject *parent)
    : QObject{parent}
{

}

void PanoPreset::insertion(int a[], int n){
    int i, j;
    for (i = 1; i < n; i++) {
        int tmp = a[i];
        for (j = i; j > 0 && a[j - 1] > tmp; j--)
            a[j] = a[j - 1];
        a[j] = tmp;
    }
}

void PanoPreset::receievePreset_1(QPixmap& pixmap){
    QImage image(pixmap.toImage().convertToFormat(QImage::Format_Grayscale8));
    int width = image.width();
    int height = image.height();

    QImage *filterImage = new QImage(image.width(), image.height(), QImage::Format_Grayscale8);
    filterImage->fill(0);
    outimg = filterImage->bits();

    const uchar* inimg = image.bits();

    int arr[9] = {0,};
    int rowSize = width ;
    int imageSize = rowSize * height;

    int widthCnt = 0, heightCnt = -1, cnt = 0;

    for (int i = 0; i < imageSize; i++) {
        widthCnt = i % width;
        if (i % width == 0) heightCnt++;

        if (widthCnt == 0) {
            //좌측 상단 Vertex
            if (heightCnt == 0) {
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[2] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }
            //좌측 하단 Vertex
            else if (heightCnt == height - 1) {
                arr[0] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[widthCnt + ((heightCnt * rowSize))];
                arr[8] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
            }
            else {
                arr[0] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }

            insertion(arr, 9);
            outimg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (widthCnt == (rowSize - 1)) {
            //우측 상단 Vertex
            if (heightCnt == 0) {
                arr[0] = arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[7] = arr[8] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
            }
            //우측 하단 Vertex
            else if (heightCnt == height - 1) {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[1] = arr[2] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[widthCnt + (heightCnt * rowSize)];
            }
            else {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[2] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[5] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt + 1) * rowSize)];
                arr[8] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
            }

            insertion(arr, 9);
            outimg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (heightCnt == 0) {
            if (widthCnt != 1 && widthCnt != rowSize - 1) {
                arr[0] = arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[1] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[2] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt + 1) * rowSize)];
                arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }
            insertion(arr, 9);
            outimg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (heightCnt == (height - 1)) {
            if (widthCnt != 1 && widthCnt != rowSize - 1) {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[4] = arr[7] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[5] = arr[8] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
            }

            insertion(arr, 9);
            outimg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else {
            cnt = 0;
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    arr[cnt++] = inimg[((widthCnt + i) + (heightCnt + j) * width)];
                }
            }
            insertion(arr, 9);
            outimg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
    }   //for문 i, imageSize

    QPixmap medianPixmap;
    medianPixmap = medianPixmap.fromImage(filterImage->convertToFormat(QImage::Format_RGB32));

    emit sendMedian(medianPixmap);
}
