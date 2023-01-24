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
    QPixmap pixmap;
    QImage  defaultImg;

    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

private slots:
    void receiveFile(QString);
    void changePanoValue(int, int);

signals:
    void panoImgSend(QPixmap&);
};

#endif // PANOVALUEADJUSTMENT_H
