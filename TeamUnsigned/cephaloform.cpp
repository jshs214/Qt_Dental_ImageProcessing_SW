#include "cephaloform.h"
#include "ui_cephaloform.h"

#include <QLabel>
#include <QLineEdit>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFile>
#include <QFileDialog>
#include <QBuffer>
#include <QPainter>
#include <QIntValidator>
#include <QMessageBox>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

CephaloForm::CephaloForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CephaloForm)
{
    ui->setupUi(this);

    imageWidth = 1125 ,imageHeight = 611;

    // 정수형 0~100까지만 입력가능한 QValidator 정의
    QValidator *validator = new QIntValidator(-100, 100, this);
    // lineEdit에 validator 설정
    ui->brightLineEdit->setValidator(validator);
    ui->contrastLineEdit->setValidator(validator);
    QValidator *sbValidator = new QIntValidator(-4, 4, this);
    ui->sbLineEdit->setValidator(sbValidator);

    cephImageView = new CephImageView;
    cephImageView->setFixedSize(1020, 655);
    cephImageView->setStyleSheet("border: 1px solid rgb(184,191,200);");



    ui->verticalLayout_7->insertWidget(2, cephImageView);

    connect(ui->exitButton, SIGNAL(clicked()), qApp, SLOT(closeAllWindows()) ); //종료 버튼

    /* Load Image SIGNAL/SLOT */
    connect(cephImageView, SIGNAL(send(QPixmap,QString)),
            this, SLOT(receieveDefaultImg(QPixmap,QString)));
    connect(this, SIGNAL(sendCephView(QPixmap)),
            cephImageView, SLOT(receiveLoadImg(QPixmap)));

    /* Reset PanoImage SIGNAL/SLOT */
    connect(this, SIGNAL(sendResetCeph(QPixmap&)),
            cephImageView, SLOT(receiveResetCeph(QPixmap&)));

    /* SaveSignal SIGNAL/SLOT */
    connect(this, SIGNAL(saveCephSignal()),
            cephImageView, SLOT(receiveSaveCeph()));
    /* panoimg save 하기위한 SIGNAL/SLOT */
    connect(cephImageView, SIGNAL(sendSave(QImage&)),
            this, SLOT(cephImageSave(QImage&)));


}

CephaloForm::~CephaloForm()
{
    delete ui;
}

void CephaloForm::on_brightSlider_valueChanged(int brightValue)
{
    QPixmap pixmap;

    if(ui->pathLineEdit->text() != "")
    {
        int contrastValue = ui->contrastSlider->value();
        emit sendCephValue(brightValue , contrastValue);
    }
    ui->brightLineEdit->setText( QString::number(ui->brightSlider->value()) );
}

void CephaloForm::on_brightMinusButton_clicked()
{
    brightValue = ui->brightSlider->value();
    brightValue -= 10;
    if(brightValue < -100) return;
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );
}
void CephaloForm::on_brightPlusButton_clicked()
{
    brightValue = ui->brightSlider->value();
    brightValue += 10;
    if(brightValue > 100) return;
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );
}
void CephaloForm::on_brightLineEdit_textChanged(const QString &brightString)
{
    brightValue = brightString.toInt();
    ui->brightSlider->setValue(brightValue);
}
/********************************************************************************************/
void CephaloForm::on_contrastSlider_valueChanged(int contrastValue)
{
    QPixmap pixmap;
    QImage image = defaultImg;

    int brightValue = ui->brightSlider->value();

    if(ui->pathLineEdit->text() != "")
    {
        emit sendCephValue(brightValue , contrastValue);
    }
    ui->contrastLineEdit->setText( QString::number(ui->contrastSlider->value()) );

}
void CephaloForm::on_contrastMinusButton_clicked()
{
    contrastValue = ui->contrastSlider->value();
    contrastValue -= 10;
    if(contrastValue < -100) return;
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );
}
void CephaloForm::on_contrastPlusButton_clicked()
{
    contrastValue = ui->contrastSlider->value();
    contrastValue += 10;
    if(contrastValue > 100) return;
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );
}
void CephaloForm::on_contrastLineEdit_textChanged(const QString &contrastString)
{
    contrastValue = contrastString.toInt();
    ui->contrastSlider->setValue(contrastValue);
}
/********************************************************************************************/


void CephaloForm::on_sbSlider_valueChanged(int value)
{
    if(ui->pathLineEdit->text() == "")  return;

    QPixmap pixmap;
    QImage image = defaultImg;

    //if (value > 0) image = *sharpenFliter(image.bits(), value*4);
    //else if (value < 0) image = *blurFilter(image.bits(), value);

    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));
    //ui->imgLabel->setPixmap(pixmap);
    ui->sbLineEdit->setText( QString::number(ui->sbSlider->value()) );
}
void CephaloForm::on_sharpenButton_clicked()
{
    if(ui->pathLineEdit->text() == "")  return;

    sbValue = ui->sbSlider->value();
    sbValue--;
    if(sbValue < -4) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}
void CephaloForm::on_blurButton_clicked()
{
    if(ui->pathLineEdit->text() == "")  return;

    sbValue = ui->sbSlider->value();
    sbValue++;
    if(sbValue > 4) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}

void CephaloForm::on_ceph_Preset_Button1_clicked()
{
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button1->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");

}
void CephaloForm::on_ceph_Preset_Button2_clicked()
{
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");

}
void CephaloForm::on_ceph_Preset_Button3_clicked()
{
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
}
void CephaloForm::on_ceph_Preset_Button4_clicked()
{
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
}


void CephaloForm::on_ceph_Preset_Button5_clicked()
{
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
}
void CephaloForm::on_ceph_Preset_Button6_clicked()
{
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
}
void CephaloForm::on_resetButton_clicked()
{
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");

    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);

    QPixmap pixmap;
    pixmap = pixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));
    emit sendResetCeph(pixmap);
}


void CephaloForm::on_imageSaveButton_clicked()
{
    emit saveCephSignal();
}
void CephaloForm::on_filePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);

    QPixmap pixmap;

    if(filename.length()) {          // 파일이 존재한다면
        file = new QFile(filename);
        pixmap.load(file->fileName());
        emit sendCephAdj(file->fileName());

        QStringList nameStr = file->fileName().split("/").last().split(".");
        QString fileName = nameStr.first();
        ui->pathLineEdit->setText(fileName);

        if(!pixmap.isNull()) {
            emit sendCephView(pixmap);
            ui->cephImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));
            defaultImg = pixmap.toImage();
        }
    }
    file->close();
    delete file;
}
void CephaloForm::receieveDefaultImg(QPixmap pixmap, QString file)
{
    ui->pathLineEdit->clear();

    defaultImg = pixmap.toImage();
    emit sendCephAdj(file);

    ui->cephImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));

    QStringList nameStr = file.split("/").last().split(".");
    QString fileName = nameStr.first();
    ui->pathLineEdit->setText(fileName);
}

void CephaloForm::text(QPixmap &pixmap)
{
    ui->cephImgLabel->setPixmap(QPixmap());
    ui->cephImgLabel->setPixmap(pixmap.scaled(1000, 600));
}

void CephaloForm::receieveImg(QPixmap& pixmap)
{
    emit sendCephView(pixmap);
}

void CephaloForm::cephImageSave(QImage& saveimg)
{
    QString filename = QFileDialog::getSaveFileName(this, "Select a file to save", ".",
                                                    "Image File(*.jpg *.bmp *.raw *.png)");
    QFile * file = new QFile(filename);
    file->open(QIODevice::WriteOnly | QIODevice::Text);
    QFileInfo fileInfo(filename);

    if(fileInfo.isWritable()){
        saveimg.save(filename);
    }
    else {
        QMessageBox::warning(this, "Error", "Can't Save this file", QMessageBox::Ok);
    }

    file->close();
    delete file;
}

