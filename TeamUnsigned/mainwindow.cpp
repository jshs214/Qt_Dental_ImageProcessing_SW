#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "panoramaform.h"
#include "cephaloform.h"

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

    /* ui 설정 */
    ui->stackedWidget->insertWidget(0, panoramaForm);
    ui->stackedWidget->insertWidget(1, cephaloForm);
    ui->stackedWidget->setCurrentIndex(0);

}

MainWindow::~MainWindow()
{
    delete panoramaForm;
    delete cephaloForm;
    delete ui;
}
void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   qDebug()<< size();
   // Your code here.
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

