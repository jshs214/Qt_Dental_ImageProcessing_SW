#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "panoramaform.h"
#include "cephaloform.h"
#include "panovalueadjustment.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    panoramaForm = new PanoramaForm(this);
    connect(panoramaForm, SIGNAL(destroyed()),
            panoramaForm, SLOT(deleteLater()));

    cephaloForm = new CephaloForm(this);
    connect(cephaloForm, SIGNAL(destroyed()),
            cephaloForm, SLOT(deleteLater()));

    panoValueAdjustment = new PanoValueAdjustment(this);
    connect(panoValueAdjustment, SIGNAL(destroyed()),
            panoValueAdjustment, SLOT(deleteLater()));


    /* ui 설정 */
    ui->stackedWidget->insertWidget(0, panoramaForm);
    ui->stackedWidget->insertWidget(1, cephaloForm);
    ui->stackedWidget->setCurrentIndex(0);

    /* Load 시, 밝기, 대조, 블러 이미지 경로 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoAdj(QString)),
            panoValueAdjustment, SLOT(receiveFile(QString)));

    /* 밝기, 대조, 블러 Value SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoBright(int)),
             panoValueAdjustment, SLOT(changeBrightness(int)));
    connect(panoramaForm, SIGNAL(sendPanoContrast(int)),
             panoValueAdjustment, SLOT(changeContrast(int)));

    /* 밝기, 대조, 블러 연산 pixmap SIGNAL/SLOT */
    connect(panoValueAdjustment, SIGNAL(panoBrightPixmap(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));
    connect(panoValueAdjustment, SIGNAL(panoContrastPixmap(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));


}

MainWindow::~MainWindow()
{
    delete panoramaForm;
    delete cephaloForm;
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

