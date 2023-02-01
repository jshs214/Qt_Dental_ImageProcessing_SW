#ifndef PANOPRESET_H
#define PANOPRESET_H

#include <QObject>

class PanoPreset : public QObject
{
    Q_OBJECT
public:
    explicit PanoPreset(QObject *parent = nullptr);

private slots:
    void receievePreset_1(QPixmap&);

signals:
    void sendMedian(QPixmap&);

private:
    unsigned char* outimg;
    void insertion(int a[], int n);
    int setMax(int a[], int n);
    int setMin(int a[], int n);
};

#endif // PANOPRESET_H
