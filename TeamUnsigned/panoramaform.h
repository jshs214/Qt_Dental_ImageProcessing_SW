#ifndef PANORAMAFORM_H
#define PANORAMAFORM_H

#include "dentalimageview.h"

#include <QWidget>
#include <QObject>

class QFile;
class DentalImageView;
class FilterButtonForm;

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

private slots:
    void loadDB_Data(QString);

    void on_filePushButton_clicked();

    void on_brightSlider_valueChanged(int value);
    void on_brightMinusButton_clicked();
    void on_brightPlusButton_clicked();

    void on_contrastSlider_valueChanged(int value);
    void on_contrastMinusButton_clicked();
    void on_contrastPlusButton_clicked();

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

    void text(QPixmap&);
    void receieveImg(QPixmap&);

    void on_imageSaveButton_clicked();

    void panoImageSave(QImage&);

    void on_hePushButton_clicked();

    void on_deNoisePlusButton_clicked();
    void on_deNoiseMinusButton_clicked();
    void on_deNoiseSlider_valueChanged(int value);

    void on_exitButton_clicked();

    void sendFourierSignal(int);
    void send2FourierSignal(int);
    void sendMedianSignal(int);

    void on_filterPushButton_clicked();

    void on_gammaPlusButton_clicked();

    void on_gammaMinusButton_clicked();

    void on_gammaSlider_valueChanged(int value);

private:
    Ui::PanoramaForm *ui;
    QFile* file;
    QImage defaultImg;
    QPixmap defaultPixmap, prevPixmap;

    DentalImageView* dentalImageView;
    FilterButtonForm* filterWidget;

    int imageWidth;
    int imageHeight;
    int panoImgLabelWidth = 360;
    int panoImgLabelHeight = 260;

    int brightValue;
    int contrastValue;
    int sbValue;
    int gammaValue;
    int deNoiseValue;

signals:
    void sendPanoView(QPixmap);
    void sendPanoAdj(QPixmap&);

    void sendPanoValue(int, int, int, int, int);  //밝기, 대조, 필터 값

    void sendResetPano(QPixmap&);
    void savePanoSignal();

    void sendPanoPrev(QPixmap&);
    void sendPanoPreset(int);
    void sendSetReset();
    void exitPanoSignal();

    void sendCutOffValue(int);
    void send2CutOffValue(int);
    void sendMedianValue(int);
};

#endif // PANORAMAFORM_H
