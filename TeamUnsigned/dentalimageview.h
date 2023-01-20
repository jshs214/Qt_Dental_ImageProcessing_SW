#ifndef DENTALIMAGEVIEW_H
#define DENTALIMAGEVIEW_H

#include <QWidget>

#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
class DentalImageView  : public QWidget
{
    Q_OBJECT;

    QGridLayout m_layout{this};
    QScrollArea m_area;
    QLabel *m_imageLabel, m_scaleLabel;
    QPushButton m_zoomOut{"Zoom Out"}, m_zoomIn{"Zoom In"}, m_zoomReset{"Zoom Reset"};
    double m_scaleFactor = 1.0;
    QImage prevImg;

public:
    DentalImageView();

    void scaleImage(double);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

    int panoWidth = 3000;
    int panoHeight = 1628;

private slots:
    void receiveLoadImg(QPixmap);
    void receiveResetPano(QPixmap&);
    void receiveSavePano();

signals:
    void send(QPixmap , QString);
    void sendSave(QImage&);
};

#endif // DENTALIMAGEVIEW_H
