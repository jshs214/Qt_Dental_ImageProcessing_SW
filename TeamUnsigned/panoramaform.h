#ifndef PANORAMAFORM_H
#define PANORAMAFORM_H

#include "dentalimageview.h"

#include <QWidget>
#include <QObject>

class QFile;
class DentalImageView;

namespace Ui {
class PanoramaForm;
}

class PanoramaForm : public QWidget
{
    Q_OBJECT

public:
    explicit PanoramaForm(QWidget *parent = nullptr);
    ~PanoramaForm();
protected:
    QImage* sharpenFliter(const uchar*, int);
    QImage* blurFilter(const uchar*, int);

private slots:
    void on_filePushButton_clicked();

    void on_brightSlider_valueChanged(int value);
    void on_brightMinusButton_clicked();
    void on_brightPlusButton_clicked();
    void on_brightLineEdit_textChanged(const QString &arg1);

    void on_contrastSlider_valueChanged(int value);
    void on_contrastMinusButton_clicked();
    void on_contrastPlusButton_clicked();
    void on_contrastLineEdit_textChanged(const QString &arg1);

    void on_sbSlider_valueChanged(int value);
    void on_sharpenButton_clicked();
    void on_blurButton_clicked();

    void on_preset_Button1_clicked();
    void on_preset_Button2_clicked();
    void on_preset_Button3_clicked();
    void on_preset_Button4_clicked();
    void on_preset_Button5_clicked();
    void on_preset_Button6_clicked();

    void on_resetButton_clicked();

    void receieveDefaultImg(QPixmap, QString);
    void text(QPixmap&);
    void receieveImg(QPixmap&);

    void on_imageSaveButton_clicked();

    void panoImageSave(QImage&);

private:
    Ui::PanoramaForm *ui;
    QFile* file;
    QImage defaultImg;

    DentalImageView* dentalImageView;

    int imageWidth;
    int imageHeight;
    int panoImgLabelWidth = 360;
    int panoImgLabelHeight = 287;
    int brightValue;
    int contrastValue;
    int sbValue;

signals:
    void sendPanoView(QPixmap);
    void sendPanoAdj(QString);

    void sendPanoValue(int, int);

    void sendResetPano(QPixmap&);
    void savePanoSignal();

};

#endif // PANORAMAFORM_H
