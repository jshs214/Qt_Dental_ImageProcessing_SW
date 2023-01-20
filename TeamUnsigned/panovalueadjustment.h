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
    QImage  defaultImg, prevImg;

    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

private slots:
    void receiveFile(QString);
    void changeBrightness(int value);
    void changeContrast(int value);
signals:
    void panoBrightPixmap(QPixmap&);
    void panoContrastPixmap(QPixmap&);
};

#endif // PANOVALUEADJUSTMENT_H
