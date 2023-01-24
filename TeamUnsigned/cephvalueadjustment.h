#ifndef CEPHVALUEADJUSTMENT_H
#define CEPHVALUEADJUSTMENT_H

#include <QPixmap>
#include <QObject>

class CephValueAdjustment : public QObject
{
    Q_OBJECT
public:
    explicit CephValueAdjustment(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage  defaultImg, prevImg;

    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

private slots:
    void receiveFile(QString);
    void changeCephValue(int, int);

signals:
    void cephImgSend(QPixmap&);
};

#endif // CEPHVALUEADJUSTMENT_H
