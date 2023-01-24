#include "cephimageview.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFile>
#include <QFileDialog>

CephImageView::CephImageView(QWidget *parent)
    : QWidget{parent}
{
    setAcceptDrops(true);   //Drag & Drop true

    m_layout.addWidget(&m_area, 0, 0, 1, 4);
    m_layout.addWidget(&m_zoomOut, 1, 0);
    m_layout.addWidget(&m_zoomIn, 1, 1);
    m_layout.addWidget(&m_scaleLabel, 1, 2);
    m_layout.addWidget(&m_zoomReset, 1, 3);

    m_imageLabel = new QLabel(this);
    m_area.setWidget(m_imageLabel);
    m_imageLabel->setScaledContents(true);

    m_imageLabel->setStyleSheet("border: none;");
    m_scaleLabel.setStyleSheet("border: none;");

    connect(&m_zoomIn, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="")
            scaleImage(1.1);
    });

    connect(&m_zoomOut, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="100.0%" && m_scaleLabel.text() !="")
            scaleImage(1.0/1.1);
    });

    connect(&m_zoomReset, &QPushButton::clicked, [this]{
        if(m_scaleFactor != 1){
            m_imageLabel->resize(dentalViewWidth,dentalViewHeight);
            m_scaleFactor=1;
            m_scaleLabel.setText("100.0%");
        }
    });
}

void CephImageView::scaleImage(double factor)
{
    m_scaleFactor *= factor;
    m_scaleLabel.setText(QStringLiteral("%1%").arg(m_scaleFactor*100, 0, 'f', 1));
    QSize size = m_imageLabel->pixmap().size() * m_scaleFactor;
    m_imageLabel->resize(size);
}
/********************************************************************************************/
void CephImageView::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void CephImageView::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void CephImageView::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    m_imageLabel->setPixmap(QPixmap());
    QPixmap pixmap;

    if (mimeData->hasUrls())
    {
        QList<QUrl> paths = mimeData->urls();
        foreach(QUrl path, paths) {
            pixmap.load(path.toLocalFile());

            QString fileName = path.toLocalFile();

            if(!pixmap.isNull()) {
                m_imageLabel->setPixmap(pixmap.scaled(dentalViewWidth, dentalViewHeight));
                scaleImage(1.0);

                emit send(pixmap, fileName);

                prevImg = pixmap.toImage();
            }
        }
    }
}

/* panoramaForm 에서 로드 버튼 클릭 시 or 연산 후,  뷰로 픽스맵 데이터 전송하는 함수. */
void CephImageView::receiveLoadImg(QPixmap pixmap)
{
    m_imageLabel->setPixmap(pixmap.scaled(dentalViewWidth, dentalViewHeight));
    scaleImage(1.0);

    prevImg = pixmap.scaled(panoWidth, panoHeight).toImage();

}

void CephImageView::receiveResetCeph(QPixmap& pixmap)
{
    m_imageLabel->setPixmap(pixmap.scaled(dentalViewWidth, dentalViewHeight));
    scaleImage(1.0);
    qDebug()<< __FUNCTION__ << __LINE__;
}

void CephImageView::receiveSaveCeph()
{
    emit sendSave(prevImg);
}



