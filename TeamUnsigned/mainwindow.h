#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CephaloForm;
class PanoramaForm;
class PanoValueAdjustment;
class CephValueAdjustment;

class PanoSharpenType;
class PanoPreset;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    PanoramaForm *panoramaForm;    /* pano form */
    CephaloForm *cephaloForm;      /* ceph form */
    PanoValueAdjustment* panoValueAdjustment;
    CephValueAdjustment* cephValueAdjustment;
    PanoSharpenType* panoSharpenType;
    PanoPreset* panoPreset;

private slots:
    void on_panoToolButton_clicked();
    void on_cephToolButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
