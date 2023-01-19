#include "dentalimageview.h"
#include "qevent.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFile>
#include <QFileDialog>

DentalImageView::DentalImageView()
{
    setAcceptDrops(true);

    m_layout.addWidget(&m_area, 0, 0, 1, 4);
    m_layout.addWidget(&m_zoomOut, 1, 0);
    m_layout.addWidget(&m_zoomIn, 1, 1);
    m_layout.addWidget(&m_scaleLabel, 1, 2);
    m_layout.addWidget(&m_zoomReset, 1, 3);


    m_area.setWidget(&m_imageLabel);
    m_imageLabel.setScaledContents(true);

    connect(&m_zoomIn, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="")
        scaleImage(1.1);

    });
    connect(&m_zoomOut, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="100.0%" && m_scaleLabel.text() !="")
        scaleImage(1.0/1.1);


    });
    connect(&m_zoomReset, &QPushButton::clicked, [this]{
        m_imageLabel.resize(1000,600);
        m_scaleFactor=1;
        m_scaleLabel.setText("100.0%");
    });



}

void DentalImageView::setImage(const QImage & img)
{
   m_scaleFactor = 1.0;
   m_imageLabel.setPixmap(QPixmap::fromImage(img));
   scaleImage(1.0);
}

void DentalImageView::scaleImage(double factor)
{
   m_scaleFactor *= factor;
   m_scaleLabel.setText(QStringLiteral("%1%").arg(m_scaleFactor*100, 0, 'f', 1));
   QSize size = m_imageLabel.pixmap().size() * m_scaleFactor;
   m_imageLabel.resize(size);
}
/********************************************************************************************/
void DentalImageView::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void DentalImageView::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void DentalImageView::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    QPixmap pixmap;
    m_imageLabel.clear();

    if (mimeData->hasUrls())
    {

        QList<QUrl> paths = mimeData->urls();
        foreach(QUrl path, paths) {
            pixmap.load(path.toLocalFile());

            if(!pixmap.isNull()) {
                m_imageLabel.setPixmap(pixmap.scaled(1000, 600));
                scaleImage(1.0);
                defaultImg = pixmap.scaled(1000, 600).toImage();
            }
        }
    }
}
