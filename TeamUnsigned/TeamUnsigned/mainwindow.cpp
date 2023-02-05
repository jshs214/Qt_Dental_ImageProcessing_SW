#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "panoramaform.h"
#include "cephaloform.h"
#include "panovalueadjustment.h"
#include "cephvalueadjustment.h"
#include "panopreset.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Unsigned Viewer");

    panoramaForm = new PanoramaForm(this);
    connect(panoramaForm, SIGNAL(destroyed()),
            panoramaForm, SLOT(deleteLater()));
    cephaloForm = new CephaloForm(this);
    connect(cephaloForm, SIGNAL(destroyed()),
            cephaloForm, SLOT(deleteLater()));

    panoValueAdjustment = new PanoValueAdjustment(this);
    connect(panoValueAdjustment, SIGNAL(destroyed()),
            panoValueAdjustment, SLOT(deleteLater()));
    cephValueAdjustment = new CephValueAdjustment(this);
    connect(cephValueAdjustment, SIGNAL(destroyed()),
            cephValueAdjustment, SLOT(deleteLater()));

    panoPreset = new PanoPreset(this);
    connect(panoPreset, SIGNAL(destroyed()),
            panoPreset, SLOT(deleteLater()));

    /* ui 설정 */
    ui->stackedWidget->insertWidget(0, panoramaForm);
    ui->stackedWidget->insertWidget(1, cephaloForm);
    ui->stackedWidget->setCurrentIndex(0);

    /* panorama SIGNAL/SLOT */
    /* Load 시, 밝기, 대조, 블러 이미지 경로 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoAdj(QString)),
            panoValueAdjustment, SLOT(receiveFile(QString)));
    /* 밝기, 대조, 블러 Value SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoValue(int, int, int, int)),
            panoValueAdjustment, SLOT(changePanoValue(int, int, int, int)));
    /* 밝기, 대조, 블러 연산 결과 pixmap 반환 SIGNAL/SLOT */
    connect(panoValueAdjustment, SIGNAL(panoImgSend(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));
    /* 히스토 연산을 위한 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoPrev(QPixmap&)),
            panoValueAdjustment, SLOT(receivePrev(QPixmap&)));

    /* Preset SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoAdj(QString)),
            panoPreset, SLOT(receiveFile(QString)));
    connect(panoramaForm, SIGNAL(sendPanoPreset(int)),
            panoPreset, SLOT(receievePreset(int)));
    connect(panoPreset, SIGNAL(panoPresetSend(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));

    /* preset 연산을 위한 이미지 설정 변경 SIGNAL/SLOT  */
    connect(panoPreset, SIGNAL(panoPresetAdj(QPixmap&, int)),
            panoValueAdjustment, SLOT(receiveSetPresetImg(QPixmap&, int)));
    /* preset_ Reset  SIGNAL/SLOT*/
    connect(panoramaForm, SIGNAL(sendSetReset()),
            panoValueAdjustment, SLOT(setResetImg()));


    /* cephalo SIGNAL/SLOT */
    /* Load 시, 밝기, 대조, 블러 이미지 경로 SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephAdj(QString)),
            cephValueAdjustment, SLOT(receiveFile(QString)));
    /* 밝기, 대조, 블러 Value SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephValue(int, int)),
            cephValueAdjustment, SLOT(changeCephValue(int, int)));
    /* 밝기, 대조, 블러 연산 pixmap SIGNAL/SLOT */
    connect(cephValueAdjustment, SIGNAL(cephImgSend(QPixmap&)),
            cephaloForm, SLOT(receieveImg(QPixmap&)));

}

MainWindow::~MainWindow()
{
    delete panoramaForm;
    delete cephaloForm;
    delete panoValueAdjustment;
    delete cephValueAdjustment;
    delete ui;
}

void MainWindow::on_panoToolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->cephToolButton->setStyleSheet("");
    ui->panoToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);");
}

void MainWindow::on_cephToolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->panoToolButton->setStyleSheet("");
    ui->cephToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);");

}

