#ifndef PANORAMAFORM_H
#define PANORAMAFORM_H

#include "dentalimageview.h"

#include <QWidget>

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
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    void changeBrightness(QImage &image, int value);
    void changeContrast(QImage &image, int value);
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
    void on_sbLineEdit_textChanged(const QString &arg1);

    void on_preset_Button1_clicked();
    void on_preset_Button2_clicked();
    void on_preset_Button3_clicked();
    void on_preset_Button4_clicked();
    void on_preset_Button5_clicked();

    void on_resetButton_clicked();

    void on_imageSaveButton_clicked();

    void on_preset_Button6_clicked();

private:
    Ui::PanoramaForm *ui;
    QFile* file;
    QImage defaultImg;
    QImage tempImg;
    DentalImageView* dentalImageView;
    int imageWidth;
    int imageHeight;
    int brightValue;
    int contrastValue;
    int sbValue;
};

#endif // PANORAMAFORM_H
