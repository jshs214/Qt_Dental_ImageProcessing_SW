#ifndef SETBRIGHTNESS_H
#define SETBRIGHTNESS_H

#include <QPixmap>
#include <QObject>

class QPixmap;

class SetBrightness : public QObject
{
    Q_OBJECT
public:
    explicit SetBrightness(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage  defaultImage;

    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

private slots:
    void receiveFile(QString);
    void changeBrightness(int value);

signals:
    void panoBrightPixmap(QPixmap&);
};

#endif // SETBRIGHTNESS_H
