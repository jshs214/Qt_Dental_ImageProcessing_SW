#include "panoramaform.h"
#include "ui_panoramaform.h"

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

unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}

PanoramaForm::PanoramaForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanoramaForm)
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

    dentalImageView = new DentalImageView;
    dentalImageView->setFixedSize(1020, 655);

    dentalImageView->setStyleSheet("border: 1px solid rgb(184,191,200);");

    ui->verticalLayout_7->insertWidget(2, dentalImageView);

    connect(ui->exitButton, SIGNAL(clicked()), qApp, SLOT(closeAllWindows()) ); //종료 버튼

    /* Load Image SIGNAL/SLOT */
    connect(dentalImageView, SIGNAL(send(QPixmap,QString)),
            this, SLOT(receieveDefaultImg(QPixmap,QString)));
    connect(this, SIGNAL(sendPanoView(QPixmap)),
            dentalImageView, SLOT(receiveLoadImg(QPixmap)));

    /* Reset PanoImage SIGNAL/SLOT */
    connect(this, SIGNAL(sendResetPano(QPixmap&)),
            dentalImageView, SLOT(receiveResetPano(QPixmap&)));
    /* SaveSignal SIGNAL/SLOT */
    connect(this, SIGNAL(savePanoSignal()),
            dentalImageView, SLOT(receiveSavePano()));
    /* panoimg save 하기위한 SIGNAL/SLOT */
    connect(dentalImageView, SIGNAL(sendSave(QImage&)),
            this, SLOT(panoImageSave(QImage&)));

}

PanoramaForm::~PanoramaForm()
{
    delete ui;
}

/********************************************************************************************/
/********************************************************************************************/
void PanoramaForm::on_brightSlider_valueChanged(int brightValue)
{
    QPixmap pixmap;

    if(ui->pathLineEdit->text() != "")
    {
        int contrastValue = ui->contrastSlider->value();
        int sbValue = ui->sbSlider->value();
        emit sendPanoValue(brightValue , contrastValue, sbValue);
    }
    ui->brightLineEdit->setText( QString::number(ui->brightSlider->value()) );
}

void PanoramaForm::on_brightMinusButton_clicked()
{
    brightValue = ui->brightSlider->value();
    brightValue -= 10;
    if(brightValue < -100) return;
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );
}

void PanoramaForm::on_brightPlusButton_clicked()
{
    brightValue = ui->brightSlider->value();
    brightValue += 10;
    if(brightValue > 100) return;
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );
}

void PanoramaForm::on_brightLineEdit_textChanged(const QString &brightString)
{
    brightValue = brightString.toInt();
    ui->brightSlider->setValue(brightValue);
}


/********************************************************************************************/
void PanoramaForm::on_contrastSlider_valueChanged(int contrastValue)
{
    QPixmap pixmap;
    QImage image = defaultImg;

    int brightValue = ui->brightSlider->value();
    int sbValue = ui->sbSlider->value();
    if(ui->pathLineEdit->text() != "")
    {
        emit sendPanoValue(brightValue , contrastValue, sbValue);
    }

    ui->contrastLineEdit->setText( QString::number(ui->contrastSlider->value()) );
}

void PanoramaForm::on_contrastMinusButton_clicked()
{
    contrastValue = ui->contrastSlider->value();
    contrastValue -= 10;
    if(contrastValue < -100) return;
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );
}

void PanoramaForm::on_contrastPlusButton_clicked()
{
    contrastValue = ui->contrastSlider->value();
    contrastValue += 10;
    if(contrastValue > 100) return;
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );
}

void PanoramaForm::on_contrastLineEdit_textChanged(const QString &contrastString)
{
    contrastValue = contrastString.toInt();
    ui->contrastSlider->setValue(contrastValue);
}


/********************************************************************************************/
void PanoramaForm::on_sharpenButton_clicked()
{
    if(ui->pathLineEdit->text() == "")  return;

    sbValue = ui->sbSlider->value();
    sbValue--;

    if(sbValue < -4) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}

void PanoramaForm::on_blurButton_clicked()
{
    if(ui->pathLineEdit->text() == "")  return;

    sbValue = ui->sbSlider->value();
    sbValue++;

    if(sbValue > 4) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}


void PanoramaForm::on_sbSlider_valueChanged(int sbValue)
{
    if(ui->pathLineEdit->text() == "")  return;

    int brightValue = ui->brightSlider->value();
    int contrastValue = ui->contrastSlider->value();

    emit sendPanoValue(brightValue , contrastValue, sbValue);

    ui->sbLineEdit->setText( QString::number(ui->sbSlider->value()) );
}


/********************************************************************************************/
void PanoramaForm::on_preset_Button1_clicked()
{
    /* preset button ui 초기화 */
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button1->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");

}

void PanoramaForm::on_preset_Button2_clicked()
{
    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
}

void PanoramaForm::on_preset_Button3_clicked()
{
    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
}

void PanoramaForm::on_preset_Button4_clicked()
{
    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
}

void PanoramaForm::on_preset_Button5_clicked()
{
    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
}
void PanoramaForm::on_preset_Button6_clicked()
{
    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
}


/********************************************************************************************/
void PanoramaForm::on_resetButton_clicked()
{
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");

    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);

    QPixmap pixmap;
    pixmap = pixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));
    emit sendResetPano(pixmap);

}


void PanoramaForm::on_imageSaveButton_clicked()
{
    emit savePanoSignal();
}

void PanoramaForm::on_filePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);

    QPixmap pixmap;

    if(filename.length()) {          // 파일이 존재한다면
        file = new QFile(filename);
        pixmap.load(file->fileName());
        emit sendPanoAdj(file->fileName());

        QStringList nameStr = file->fileName().split("/").last().split(".");
        QString fileName = nameStr.first();
        ui->pathLineEdit->setText(fileName);

        if(!pixmap.isNull()) {
            emit sendPanoView(pixmap);
            ui->panoImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));
            defaultImg = pixmap.toImage();

            ui->panoProgressBar->setValue(100);
            ui->progressbarLabel->setText("Success Load Panorama Image !!!");
        }
        file->close();
        delete file;
    }
    else {
        QMessageBox::warning(this, "Error", "Can't Load this file", QMessageBox::Ok); ;
        return;
    }
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");

    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);

}

void PanoramaForm::receieveDefaultImg(QPixmap pixmap, QString file)
{
    ui->pathLineEdit->clear();

    defaultImg = pixmap.toImage();
    emit sendPanoAdj(file);

    ui->panoImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));
    ui->panoProgressBar->setValue(100);
    ui->progressbarLabel->setText("Success Load Panorama Image !!!");

    QStringList nameStr = file.split("/").last().split(".");
    QString fileName = nameStr.first();
    ui->pathLineEdit->setText(fileName);
}

void PanoramaForm::text(QPixmap &pixmap)
{
    ui->panoImgLabel->setPixmap(QPixmap());
    ui->panoImgLabel->setPixmap(pixmap.scaled(1105, 600));
}

void PanoramaForm::receieveImg(QPixmap& pixmap)
{
    emit sendPanoView(pixmap);
}

void PanoramaForm::panoImageSave(QImage& saveimg)
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
