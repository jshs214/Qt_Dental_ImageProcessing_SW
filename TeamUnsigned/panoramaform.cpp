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

    ui->verticalLayout_7->insertWidget(2, dentalImageView);


}

PanoramaForm::~PanoramaForm()
{
    delete ui;
}

/********************************************************************************************/
void PanoramaForm::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void PanoramaForm::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void PanoramaForm::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    QPixmap pixmap;
    //ui->imgLabel->clear();
    if (mimeData->hasUrls())
    {

        QList<QUrl> paths = mimeData->urls();
        foreach(QUrl path, paths) {
            ui->pathLineEdit->setText(path.toLocalFile());
            pixmap.load(path.toLocalFile());

            if(!pixmap.isNull()) {
               // ui->imgLabel->setPixmap(pixmap.scaled(imageWidth, imageHeight));

                defaultImg = pixmap.scaled(imageWidth, imageHeight).toImage();
            }
        }
    }
}

void PanoramaForm::on_filePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);
    QPixmap pixmap;
    if(filename.length()) {          // 파일이 존재한다면
        file = new QFile(filename);
        ui->pathLineEdit->setText(file->fileName());
        pixmap.load(file->fileName());

        if(!pixmap.isNull()) {
            //ui->imgLabel->setPixmap(pixmap.scaled(imageWidth, imageHeight));
            defaultImg = pixmap.scaled(imageWidth, imageHeight).toImage();
        }
    }
}


/********************************************************************************************/
void PanoramaForm::changeBrightness(QImage &image, int value)
{
    for(int y=0; y < image.height(); y++){
        for (int x=0; x < image.width(); x++){
            QRgb rgb = image.pixel(x,y);
            int r = LIMIT_UBYTE(qRed(rgb) + value);
            int g = LIMIT_UBYTE(qGreen(rgb) + value);
            int b = LIMIT_UBYTE(qBlue(rgb)+ value);
            image.setPixelColor(x, y, QColor(r, g, b));
        }
    }
}

void PanoramaForm::changeContrast(QImage &image, int value)
{
    float contrast = (50.0+value)/50.0;
    for(int y=0; y < image.height(); y++){
        for (int x=0; x < image.width(); x++){
            QRgb rgb = image.pixel(x,y);
            float r = LIMIT_UBYTE(128 + contrast*(qRed(rgb) - 128));
            float g = LIMIT_UBYTE(128 + contrast*(qGreen(rgb) - 128));
            float b = LIMIT_UBYTE(128 + contrast*(qBlue(rgb) - 128));
            image.setPixelColor(x, y, QColor(r, g, b));
        }
    }
}

QImage* PanoramaForm::sharpenFliter(const uchar* inimg, int value) {

    QImage* filteredImage = new QImage(imageWidth, imageHeight, QImage::Format_RGB32);
    unsigned char* outimg = filteredImage->bits();

        float mask = -(value/4);
        float median = value + 1;
        float kernel[3][3] = { {0, mask, 0},
                             {mask, median, mask},
                             {0, mask, 0}};

#if 1
    int arr[9] = {0};
    int elemSize = 4;
    int cnt = 0;
    int size = imageWidth * elemSize;

    for(int y = 0; y < imageHeight ; y++) {
        for(int x = 0; x < (imageWidth ) * elemSize; x+=elemSize) {
            for(int z = 0; z < elemSize; z++) {
                if(x==0){
                    //LeftUpVertex
                    if(y==0){
                        arr[0] = arr[1] = arr[3] = arr[4] = inimg[x+(y*size)+z];
                        arr[2] = arr[5] = inimg[x+elemSize + (y*size)+z];
                        arr[6] = arr[7] = inimg[x+ ((y+1)*size) +z];
                        arr[8] = inimg[x+elemSize+((y+1)*size)+z];
                    }
                    //LeftDownVertex
                    else if(y==imageHeight-1){
                        arr[0] = arr[1] =inimg[x+((y-1)*size)+z];
                        arr[2] = inimg[x+elemSize + ((y-1)*size)+z];
                        arr[3] = arr[6] = arr[7] = arr[4] = inimg[x+(y*size) +z];
                        arr[8] = arr[5] = inimg[x+elemSize + (y*size) +z];
                    }
                    else{
                        arr[0] = arr[1] = inimg[x+( (y-1)*size) +z];
                        arr[2] = inimg[x+elemSize+( (y-1)*size) +z];
                        arr[3] = arr[4] = inimg[x+(y*size)+z];
                        arr[5] = inimg[x+elemSize+(y*size)+z];
                        arr[6] = arr[7] = inimg[x+ ( (y+1)*size) +z];
                        arr[8] = inimg[x+elemSize+( (y+1)*size) +z];
                    }

                    cnt=0;
                    float sum = 0.0;
                    for(int i = -1; i < 2; i++) {
                        for(int j = -1; j < 2; j++) {
                            sum += kernel[i+1][j+1]*arr[cnt++];
                        }
                    }
                    outimg[(x+y*size)+z] = clip(sum,0,255);
                }

                else if( x==(imageWidth*elemSize -elemSize) ){
                    //RightUpVertex
                    if(y==0){
                        arr[0] = arr[3] = inimg[x-elemSize + (y*size) +z];
                        arr[1] = arr[2] = arr[5] = arr[4] = inimg[x + (y*size) +z];
                        arr[6] = inimg[x-elemSize + ((y-1)*size) +z];
                        arr[7] = arr[8] = inimg[x+((y+1)*size)+z];
                    }
                    //RightDownVertex
                    else if(y==imageHeight-1){
                        arr[0] = inimg[x-elemSize + ((y-1)*size) +z];
                        arr[1] = arr[2] = inimg[x-elemSize +((y-1)*size) +z];
                        arr[3] = arr[6] = inimg[x-elemSize+(y*size)+z];
                        arr[4] = arr[5] = arr[7] = arr[8] = inimg[x+(y*size)+z];
                    }
                    else{
                        arr[0] = inimg[x-elemSize + ((y-1)*size) +z];
                        arr[2] = arr[1] = inimg[x + ((y-1)*size) +z];
                        arr[3] = inimg[x-elemSize + (y*size) +z];
                        arr[5] = arr[4] = inimg[x+(y*size) +z];
                        arr[6] = inimg[x-elemSize + ((y+1)*size) +z];
                        arr[8] = arr[7] = inimg[x+((y+1)*size) +z];
                    }
                    cnt=0;
                    float sum = 0.0;
                    for(int i = -1; i < 2; i++) {
                        for(int j = -1; j < 2; j++) {
                            sum += kernel[i+1][j+1]*arr[cnt++];
                        }
                    }
                    outimg[(x+y*size)+z] = clip(sum,0,255);
                }
                else if(y==0){
                    if( x!=1 && x!=size-elemSize ){
                        arr[0] = arr[3] = inimg[x-elemSize+(y*size) +z];
                        arr[1] = arr[4] = inimg[x+(y*size)+z];
                        arr[2] = arr[5] = inimg[x+elemSize+(y*size) +z];
                        arr[6] = inimg[x-elemSize+((y+1)*size) +z];
                        arr[7] = inimg[x+((y+1)*size) +z];
                        arr[8] = inimg[x+elemSize + ((y+1)*size) +z];

                        cnt=0;
                        float sum = 0.0;
                        for(int i = -1; i < 2; i++) {
                            for(int j = -1; j < 2; j++) {
                                sum += kernel[i+1][j+1]*arr[cnt++];
                            }
                        }
                        outimg[(x+y*size)+z] = clip(sum,0,255);
                    }
                }
                else if( y ==(imageHeight -1) ){
                    if( x!=1 && x!=size-elemSize ){
                        arr[0] = inimg[x-elemSize+((y-1)*size)+z];
                        arr[1] = inimg[x+((y-1)*size)+z];
                        arr[2] = inimg[x+elemSize+((y-1)*size)+z];
                        arr[3] = arr[6] = inimg[x-elemSize+(y*size)+z];
                        arr[4] = arr[7] = inimg[x+(y*size)+z];
                        arr[5] = arr[8] = inimg[x+elemSize+(y*size)+z];
                        cnt=0;
                        float sum = 0.0;
                        for(int i = -1; i < 2; i++) {
                            for(int j = -1; j < 2; j++) {
                                sum += kernel[i+1][j+1]*arr[cnt++];
                            }
                        }
                        outimg[(x+y*size)+z] = clip(sum,0,255);
                    }
                }

                else{
                    float sum = 0.0;
                    for(int i = -1; i < 2; i++) {
                        for(int j = -1; j < 2; j++) {
                            sum += kernel[i+1][j+1]*inimg[((x+i*elemSize)+(y+j)*size)+z];
                        }
                    }
                    outimg[(x+y*size)+z] = clip(sum,0,255);
                }

            }
        }
    }

    return filteredImage;

#else
    int width = image.width();

    for(int y=1; y < image.height()-1; y++){
        for (int x=1; x < image.width()-1; x++){
            int r=0, g=0, b=0;
            int offset = 4*(x+(y*width));

            for (int i = -1; i < 2; i++) {
                for(int j = -1; j <2; j++) {

                    QRgb rgb = image.pixel(x+i,y+j);
                    //                    r += kernel[i+1][j+1]*qRed(rgb);
                    //                    g += kernel[i+1][j+1]*qGreen(rgb);
                    //                    b += kernel[i+1][j+1]*qBlue(rgb);
                    r += kernel[i+1][j+1]* ( inimg[offset]);
                    g += kernel[i+1][j+1]* ( inimg[offset + 1]);
                    b += kernel[i+1][j+1]* ( inimg[offset + 2]);
                }
            }

            //qDebug() << r << g << b << Qt::endl;
            image.setPixelColor(x, y, QColor(LIMIT_UBYTE(r), LIMIT_UBYTE(g), LIMIT_UBYTE(b)));
        }
    }
    qDebug()<< width;
#endif
}

QImage* PanoramaForm::blurFilter(const uchar* inimg, int value) {

    QImage* filteredImage = new QImage(imageWidth, imageHeight, QImage::Format_RGB32);
    unsigned char* outimg = filteredImage->bits();

    float edge =0.0, mask = 0.0, median = 0.0;

    if (value == -1) {
        mask = edge = median = 1/9.0;
    }
    else if (value <= -2) {
        edge = 1/28.0; mask = 1/7.0; median = 2/7.0;
    }

    float kernel[3][3] = { {edge, mask, edge},
                     {mask, median, mask},
                     {edge, mask, edge}};

    int arr[9] = {0};
    int elemSize = 4;
    int cnt = 0;
    int size = imageWidth * elemSize;

    for(int y = 0; y < imageHeight ; y++) {
        for(int x = 0; x < (imageWidth ) * elemSize; x+=elemSize) {
            for(int z = 0; z < elemSize; z++) {
                if(x==0){
                    //LeftUpVertex
                    if(y==0){
                        arr[0] = arr[1] = arr[3] = arr[4] = inimg[x+(y*size)+z];
                        arr[2] = arr[5] = inimg[x+elemSize + (y*size)+z];
                        arr[6] = arr[7] = inimg[x+ ((y+1)*size) +z];
                        arr[8] = inimg[x+elemSize+((y+1)*size)+z];
                    }
                    //LeftDownVertex
                    else if(y==imageHeight-1){
                        arr[0] = arr[1] =inimg[x+((y-1)*size)+z];
                        arr[2] = inimg[x+elemSize + ((y-1)*size)+z];
                        arr[3] = arr[6] = arr[7] = arr[4] = inimg[x+(y*size) +z];
                        arr[8] = arr[5] = inimg[x+elemSize + (y*size) +z];
                    }
                    else{
                        arr[0] = arr[1] = inimg[x+( (y-1)*size) +z];
                        arr[2] = inimg[x+elemSize+( (y-1)*size) +z];
                        arr[3] = arr[4] = inimg[x+(y*size)+z];
                        arr[5] = inimg[x+elemSize+(y*size)+z];
                        arr[6] = arr[7] = inimg[x+ ( (y+1)*size) +z];
                        arr[8] = inimg[x+elemSize+( (y+1)*size) +z];
                    }

                    cnt=0;
                    float sum = 0.0;
                    for(int i = -1; i < 2; i++) {
                        for(int j = -1; j < 2; j++) {
                            sum += kernel[i+1][j+1]*arr[cnt++];
                        }
                    }
                    outimg[(x+y*size)+z] = clip(sum,0,255);
                }

                else if( x==(imageWidth*elemSize -elemSize) ){
                    //RightUpVertex
                    if(y==0){
                        arr[0] = arr[3] = inimg[x-elemSize + (y*size) +z];
                        arr[1] = arr[2] = arr[5] = arr[4] = inimg[x + (y*size) +z];
                        arr[6] = inimg[x-elemSize + ((y-1)*size) +z];
                        arr[7] = arr[8] = inimg[x+((y+1)*size)+z];
                    }
                    //RightDownVertex
                    else if(y==imageHeight-1){
                        arr[0] = inimg[x-elemSize + ((y-1)*size) +z];
                        arr[1] = arr[2] = inimg[x-elemSize +((y-1)*size) +z];
                        arr[3] = arr[6] = inimg[x-elemSize+(y*size)+z];
                        arr[4] = arr[5] = arr[7] = arr[8] = inimg[x+(y*size)+z];
                    }
                    else{
                        arr[0] = inimg[x-elemSize + ((y-1)*size) +z];
                        arr[2] = arr[1] = inimg[x + ((y-1)*size) +z];
                        arr[3] = inimg[x-elemSize + (y*size) +z];
                        arr[5] = arr[4] = inimg[x+(y*size) +z];
                        arr[6] = inimg[x-elemSize + ((y+1)*size) +z];
                        arr[8] = arr[7] = inimg[x+((y+1)*size) +z];
                    }
                    cnt=0;
                    float sum = 0.0;
                    for(int i = -1; i < 2; i++) {
                        for(int j = -1; j < 2; j++) {
                            sum += kernel[i+1][j+1]*arr[cnt++];
                        }
                    }
                    outimg[(x+y*size)+z] = clip(sum,0,255);
                }
                else if(y==0){
                    if( x!=1 && x!=size-elemSize ){
                        arr[0] = arr[3] = inimg[x-elemSize+(y*size) +z];
                        arr[1] = arr[4] = inimg[x+(y*size)+z];
                        arr[2] = arr[5] = inimg[x+elemSize+(y*size) +z];
                        arr[6] = inimg[x-elemSize+((y+1)*size) +z];
                        arr[7] = inimg[x+((y+1)*size) +z];
                        arr[8] = inimg[x+elemSize + ((y+1)*size) +z];

                        cnt=0;
                        float sum = 0.0;
                        for(int i = -1; i < 2; i++) {
                            for(int j = -1; j < 2; j++) {
                                sum += kernel[i+1][j+1]*arr[cnt++];
                            }
                        }
                        outimg[(x+y*size)+z] = clip(sum,0,255);
                    }
                }
                else if( y ==(imageHeight -1) ){
                    if( x!=1 && x!=size-elemSize ){
                        arr[0] = inimg[x-elemSize+((y-1)*size)+z];
                        arr[1] = inimg[x+((y-1)*size)+z];
                        arr[2] = inimg[x+elemSize+((y-1)*size)+z];
                        arr[3] = arr[6] = inimg[x-elemSize+(y*size)+z];
                        arr[4] = arr[7] = inimg[x+(y*size)+z];
                        arr[5] = arr[8] = inimg[x+elemSize+(y*size)+z];
                        cnt=0;
                        float sum = 0.0;
                        for(int i = -1; i < 2; i++) {
                            for(int j = -1; j < 2; j++) {
                                sum += kernel[i+1][j+1]*arr[cnt++];
                            }
                        }
                        outimg[(x+y*size)+z] = clip(sum,0,255);
                    }
                }

                else{
                    float sum = 0.0;
                    for(int i = -1; i < 2; i++) {
                        for(int j = -1; j < 2; j++) {
                            sum += kernel[i+1][j+1]*inimg[((x+i*elemSize)+(y+j)*size)+z];
                        }
                    }
                    outimg[(x+y*size)+z] = clip(sum,0,255);
                }

            }
        }
    }

    return filteredImage;
}


/********************************************************************************************/
void PanoramaForm::on_brightSlider_valueChanged(int value)
{
    QPixmap pixmap;
    QImage image = defaultImg;
    changeBrightness(image, value);
    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));
    //ui->imgLabel->setPixmap(pixmap);
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
void PanoramaForm::on_contrastSlider_valueChanged(int value)
{
    QPixmap pixmap;
    QImage image = defaultImg;
    if (value > 0) changeContrast(image, value);
    else changeContrast(image, value*0.5);
    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));
    //ui->imgLabel->setPixmap(pixmap);
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
    sbValue = ui->sbSlider->value();
    sbValue--;
    if(sbValue < -4) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}

void PanoramaForm::on_blurButton_clicked()
{
    sbValue = ui->sbSlider->value();
    sbValue++;
    if(sbValue > 4) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}

void PanoramaForm::on_sbLineEdit_textChanged(const QString &sbString)
{
    sbValue = sbString.toInt();
    ui->sbSlider->setValue(sbValue);
}

void PanoramaForm::on_sbSlider_valueChanged(int value)
{
    QPixmap pixmap;
    QImage image = defaultImg;

    if (value > 0) image = *sharpenFliter(image.bits(), value*4);
    else if (value < 0) image = *blurFilter(image.bits(), value);

    pixmap = pixmap.fromImage(image.convertToFormat(QImage::Format_Grayscale8));
    //ui->imgLabel->setPixmap(pixmap);
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

    defaultImg = tempImg;
    QPixmap pixmap;
    pixmap = pixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));
    //ui->imgLabel->setPixmap(pixmap);
}


void PanoramaForm::on_imageSaveButton_clicked()
{
    tempImg = defaultImg;
    //defaultImg=ui->imgLabel->pixmap().toImage();
    QMessageBox::information(this, "Success", "Image Setting Saves!");
}



