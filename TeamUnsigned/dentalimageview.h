#ifndef DENTALIMAGEVIEW_H
#define DENTALIMAGEVIEW_H

#include <QWidget>

#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
class DentalImageView  : public QWidget
{
    QGridLayout m_layout{this};
    QScrollArea m_area;
    QLabel m_imageLabel, m_scaleLabel;
    QPushButton m_zoomOut{"Zoom Out"}, m_zoomIn{"Zoom In"}, m_zoomReset{"Zoom Reset"};
    double m_scaleFactor = 1.0;
public:
    DentalImageView();

    void setImage(const QImage &);
    void scaleImage(double);
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    QImage defaultImg;

};

#endif // DENTALIMAGEVIEW_H
