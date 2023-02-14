#include "filterbuttonform.h"
#include "ui_filterbuttonform.h"

#include <QString>
#include <QValidator>
#include <QSettings>

/* filterButton 클래스의 생성자
* 초기 설정
*/
FilterButtonForm::FilterButtonForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterButtonForm)
{
    ui->setupUi(this);
    ui->valueLineEdit->setEnabled(false);

    /* 8bit 연산진행하므로 최소 ~ 최대값 설정 */
    validator = new QIntValidator(1, 256, this);
    ui->valueLineEdit->setValidator(validator);

    connect(this, SIGNAL(destroyed()),
            this, SLOT(deleteLater()));

    this->setWindowTitle("Filter");
}
/* filterButton 클래스의 소멸자
* filterButton 종료 시 메모리 제거
*/
FilterButtonForm::~FilterButtonForm()
{
    delete ui;
    delete validator;
}
/* filterButton 폼의 Title setter 함수 */
void FilterButtonForm::setTitle(QString title) {
    ui->titleLabel->setText(title);
}
/* filterButton 폼의 Title 을 반환하는 함수 */
QString FilterButtonForm::getTitle() {
    return ui->titleLabel->text();
}

/* cephaloForm 의 filterButton Settings 값 load */
void FilterButtonForm::cephReadSettings()
{
    QSettings settings("FilterFormCeph.ini", QSettings::IniFormat);
    settings.beginGroup("FilterFormCeph");
    ui->sortComboBox->setCurrentIndex(settings.value("name").toInt());
    ui->valueLineEdit->setText(settings.value("value").toString());
    settings.endGroup();
}

/* panorama 의 filterButton Settings 값 load */
void FilterButtonForm::panoReadSettings()
{
    QSettings settings("FilterFormPano.ini", QSettings::IniFormat);
    settings.beginGroup("FilterFormPano");
    ui->sortComboBox->setCurrentIndex(settings.value("name").toInt());
    ui->valueLineEdit->setText(settings.value("value").toString());
    settings.endGroup();
}
/* 창 닫기 시, 소멸자 호출 함수 */
void FilterButtonForm::closeEvent(QCloseEvent *event){
    Q_UNUSED(event);
    if(ui->titleLabel->text() == "Cephalo") cephWriteSettings();
    if(ui->titleLabel->text() == "Panorama") panoWriteSettings();
    FilterButtonForm::~FilterButtonForm();
}
/* cephaloForm의 filterButton 이전 Settings 값 save*/
void FilterButtonForm::cephWriteSettings()
{
    QSettings settings("FilterFormCeph.ini", QSettings::IniFormat);
    settings.beginGroup("FilterFormCeph");
    settings.setValue("name", ui->sortComboBox->currentIndex());
    settings.setValue("value", ui->valueLineEdit->text());
    settings.endGroup();
}
/* panoramaForm의 filterButton Settings 값 save 슬롯 */
void FilterButtonForm::panoWriteSettings()
{
    QSettings settings("FilterFormPano.ini", QSettings::IniFormat);
    settings.beginGroup("FilterFormPano");
    settings.setValue("name", ui->sortComboBox->currentIndex());
    settings.setValue("value", ui->valueLineEdit->text());
    settings.endGroup();
}
/* ComboBox Index에 맞는 파라미터 값 시그널 전송 슬롯 */
void FilterButtonForm::on_okPushButton_clicked()
{
    int idx = ui->sortComboBox->currentIndex();
    QString name = ui->titleLabel->text();
    /* ceph, pano Form 으로 median Fiter 시그널 전송 */
    if (idx == 1) {
        if (name == "Panorama")
            emit sendPanoMedian(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit sendCephMedian(ui->valueLineEdit->text().toInt());
    }
    /* ceph, pano Form 으로 low-pass Fiter 시그널, 파라미터 값 전송 */
    else if (idx == 2) {
        if(ui->valueLineEdit->text().toInt() > 256) return;
        if (name == "Panorama")
            emit panoLowPassCutOff(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit cephLowPassCutOff(ui->valueLineEdit->text().toInt());
    }
    /* ceph, pano Form 으로 high-pass Fiter 시그널, 파라미터 값 전송 */
    else if (idx == 3) {
        if(ui->valueLineEdit->text().toInt() > 256) return;
        if (name == "Panorama")
            emit panoHighPassCutOff(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit cephHighPassCutOff(ui->valueLineEdit->text().toInt());
    }

    this->close();
}
/* ComboBox Index에 따른 ui 설정  */
void FilterButtonForm::on_sortComboBox_currentIndexChanged(int index)
{
    switch(index) {
    case 1:
        ui->valueLineEdit->setEnabled(false);
        ui->valueLineEdit->setText(0);
        ui->valueLineEdit->setPlaceholderText("Click OK Button");
        break;
    case 2:
        ui->valueLineEdit->setEnabled(true);
        ui->valueLineEdit->setPlaceholderText("Input Cut-Off freq");
        break;
    case 3:
        ui->valueLineEdit->setEnabled(true);
        ui->valueLineEdit->setPlaceholderText("Input Cut-Off freq");
        break;
    default:
        ui->valueLineEdit->setEnabled(false);
        ui->valueLineEdit->setPlaceholderText("Choose Filter");
        ui->valueLineEdit->clear();
        break;
    }
}

