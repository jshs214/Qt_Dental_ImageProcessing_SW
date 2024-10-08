#include "listwidgetitem.h"
#include "ui_listwidget.h"

#include "mainwindow.h"

/*  XrayDB Class에서 환자 정보를 입력, 수정, 삭제
 *  해당 클래스 에서 정보 입력.
 *  @param 환자ID
 *  @param 이름
 *  @param 생일
 *  @param cephalo file 경로
 *  @param panorama file 경로
 *  @param type
 *  @param 성별
 *  @param 성별에 따른 icon 경로
 *
*/
ListWidgetItem::ListWidgetItem(int id, QString name, QString birthdate,
                               QString directory, QString directory2, QString type,
                               QString gender, QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListWidget)
{
    QString i;

    QPixmap image;

    ui->setupUi(this);
    ui->nameLabel->setText(name);
    ui->birthDateLabel->setText(birthdate);
    ui->directoryLabel->setText(directory);
    ui->directoryLabel_2->setText(directory2);
    ui->typeLabel->setText(type);
    ui->pathLabel->setText(path);
    ui->genderLabel->setText(gender);

    ui->idLabel->setText(i = QString::number(id));      // label에 String 형태로 넣기 위한 형변환
    image = QPixmap(path);
    ui->imageLabel->setPixmap(image);


}
ListWidgetItem::~ListWidgetItem()
{
    delete ui;
}
/* 환자의 이름 반환 함수 */
QString ListWidgetItem::getName()
{
    QString name;
    name = ui->nameLabel->text();
    return name;
}
/* 환자의 이름 setter */
void ListWidgetItem::setName(QString& name)
{
    ui->nameLabel->setText(name);
}
/* 환자의 생일 반환 함수 */
QString ListWidgetItem::getBirthdate()
{
    QString birthdate;
    birthdate = ui->birthDateLabel->text();
    return birthdate;
}
/* 환자의 생일 setter */
void ListWidgetItem::setBirthdate(QString& birthdate)
{
    ui->birthDateLabel->setText(birthdate);
}
/* cephalo raw file 경로 반환 함수 */
QString ListWidgetItem::getDirectory()
{
    QString directory;
    directory = ui->directoryLabel->text();
    return directory;
}
/* cephalo raw file 경로 setter
* @param cephalo file path
*/
void ListWidgetItem::setDirectory(QString& directory)
{
    ui->directoryLabel->setText(directory);
}
/* panorama raw file 경로 반환 함수 */
QString ListWidgetItem::getDirectory2()
{
    QString directory2;
    directory2 = ui->directoryLabel_2->text();
    return directory2;
}
/* panorama raw file 경로 setter
* @param panorama file path
*/
void ListWidgetItem::setDirectory2(QString& directory2)
{
    ui->directoryLabel_2->setText(directory2);
}
/* type 반환 함수 */
QString ListWidgetItem::getType()
{
    QString type;
    type = ui->typeLabel->text();
    return type;
}
/* type setter
* @param type
*/
void ListWidgetItem::setType(QString& type)
{
    ui->typeLabel->setText(type);
}
/* Icon path 반환 함수 */
QString ListWidgetItem::getImage()
{
    QString path;

    path = ui->pathLabel->text();

    return path;
}
/* Icon path setter
* @param Icon Pixmap
*/
void ListWidgetItem::setImage(QPixmap& image)
{
    ui->imageLabel->setPixmap(image);
}
/* gender 반환 함수 */
QString ListWidgetItem::getGender()
{
    QString gender;

    gender = ui->genderLabel->text();

    return gender;
}
/* gender setter
* @param gender
*/
void ListWidgetItem::setGender(QString& gender)
{
    ui->genderLabel->setText(gender);
}
/* id 반환 */
int ListWidgetItem::id()
{
    return ui->idLabel->text().toInt();
}
/* type(Pano, Ceph)에 따른 후처리 뷰어 load 슬롯 */
void ListWidgetItem::on_loadPushButton_clicked()
{
    if(flag == true)    return;

    QString type;

    QString directory = ui->directoryLabel->text();
    QString directory2 = ui->directoryLabel_2->text();

    QPixmap *buffer = new QPixmap;

    QImage *image = new QImage(directory);

    *buffer = QPixmap::fromImage((*image));

    type = ui->typeLabel->text();

    cephFile = new QFile(directory);
    panoFile = new QFile(directory2);

    unsignedViewer = new MainWindow();
    connect(this, SIGNAL(setLoadMainWindow(QString, QString, QString) ),
            unsignedViewer, SLOT(setReceiveMainWindow(QString, QString, QString) ));
    unsignedViewer->show();

    emit setLoadMainWindow(type, cephFile->fileName(), panoFile->fileName());   // type, Ceph, pano Path 전송 시그널, type에 따라 초기화면 결정

    connect(unsignedViewer, SIGNAL(closeMainWindow( ) ),
            this, SLOT(delMainWindow() ));
    flag = true;
}
/* 종료 신호 발생 시, delete MainWindow */
void ListWidgetItem::delMainWindow(){
    flag = false;
    delete unsignedViewer;
}
