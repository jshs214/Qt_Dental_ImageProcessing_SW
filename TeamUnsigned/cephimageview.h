#ifndef CEPHIMAGEVIEW_H
#define CEPHIMAGEVIEW_H

#include <QWidget>

#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

class CephImageView : public QWidget
{
    Q_OBJECT

    QGridLayout m_layout{this};
    QScrollArea m_area;
    QLabel *m_imageLabel, m_scaleLabel;
    QPushButton m_zoomOut{"Zoom Out"}, m_zoomIn{"Zoom In"}, m_zoomReset{"Zoom Reset"};
    double m_scaleFactor = 1.0;
    QImage prevImg;

public:
    explicit CephImageView(QWidget *parent = nullptr);

    void scaleImage(double);
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    int cephViewWidth = 800;
    int cephViewHeight = 600;

    int cephWidth = 3000;
    int cephHeight = 2400;

private slots:
    void receiveLoadImg(QPixmap);
    void receiveResetCeph(QPixmap&);
    void receiveSaveCeph();

signals:
    void send(QPixmap , QString);
    void sendSave(QImage&);
};

#endif // CEPHIMAGEVIEW_H
