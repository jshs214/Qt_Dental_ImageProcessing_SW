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
class PanoPreset;
class CephPreset;

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
    PanoPreset* panoPreset;
    CephPreset* cephPreset;

protected:
    void closeEvent(QCloseEvent *event);
signals:
    void loadPanoDB(QString);
    void loadCephDB(QString);

    void closeMainWindow();
private slots:
    void on_panoToolButton_clicked();
    void on_cephToolButton_clicked();

    void setReceiveMainWindow(QString, QString, QString);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
