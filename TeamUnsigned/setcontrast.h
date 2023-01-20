#ifndef SETCONTRAST_H
#define SETCONTRAST_H

#include <QPixmap>
#include <QObject>

class QPixmap;

class SetContrast : public QObject
{
    Q_OBJECT
public:
    explicit SetContrast(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage  defaultImage;

    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

private slots:
    void receiveFile(QString);
    void changeContrast(int value);

signals:
    void panoContrastPixmap(QPixmap&);
};

#endif // SETCONTRAST_H
