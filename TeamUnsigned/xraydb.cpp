#include "xraydb.h"
#include "ui_xraydb.h"
#include "listwidget.h"
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QPixmap>
#include <QImage>
#include <QLabel>
#include <QMessageBox>
#include <QListWidget>
#include  <QDebug>
#include <QButtonGroup>
#include <QPixmap>

XrayDB::XrayDB(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::XrayDB)
{
    ui->setupUi(this);



    type = new QButtonGroup(this);
    type->addButton(ui->panoramaRadioButton);
    type->addButton(ui->cephRadioButton);

    ui->panoramaRadioButton->setChecked(true);

    gender = new QButtonGroup(this);
    gender->addButton(ui->maleRadioButton);
    gender->addButton(ui->femaleRadioButton);

    ui->maleRadioButton->setChecked(true);

    loadData();
}
XrayDB::~XrayDB()
{
    delete ui;

    QFile file("person.txt");

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto &v : dbList) {
        ListWidget *L = v;
        out << L->id() << ", " << L->getName() << ", ";
        out << L->getBirthdate() << ", "  << L->getDirectory() << ", " << L->getDirectory2() << ", " ;
        out << L->getType() << ", " << L->getGender() << ", " << L->getImage() <<"\n" ;
    }
    file.close();
    qDebug() << "Success Close Program" ;
}
int XrayDB::makeId()
{
    if(dbList.size() == 0) {
        return 1;
    }

    else {
        auto id = dbList.lastKey();
        return ++id;
    }
}

void XrayDB::loadData()
{
    QFile file("person.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    { qDebug() <<  "file not opened" << Qt::endl;
        return;
    }

    QTextStream in(&file);
    while(!in.atEnd())
    {

        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        int id = row[0].toInt();
        ListWidget* L = new ListWidget(id, row[1], row[2], row[3], row[4], row[5], row[6], row[7]);

        dbList.insert(id, L);

        QListWidgetItem *item = new QListWidgetItem;

        ui->personList->addItem(item);
        item->setSizeHint(QSize(400,130));
        ui->personList->setItemWidget(item, L);
    }
}

void XrayDB::on_addPushButton_clicked()
{
    QString name, birthdate, directory, directory2, type, gender, path;

    directory = ui->directoryLineEdit->text();

    directory2 = ui->directoryLineEdit_2->text();

    int id = makeId();

    QPixmap image;

    name = ui->nameLineEdit->text();
    birthdate = ui->birthDateLineEdit->text();


    if( ui->panoramaRadioButton->isChecked() )
    {
        type = "Pano";
    }
    else if( ui->cephRadioButton->isChecked() )
    {
        type = "Ceph";
    }

    if(ui->maleRadioButton->isChecked() )
    {
        gender = "Male";

        image = QPixmap("male.png");

        path = "male.png";
    }
    else if(ui->femaleRadioButton->isChecked() )
    {
        gender = "Female";

        path = "female.png";

        image = QPixmap(path);
    }

    ui->nameLineEdit->setText(name);
    ui->birthDateLineEdit->setText(birthdate);
    ui->directoryLineEdit->setText(directory);
    ui->directoryLineEdit_2->setText(directory2);



    if (ui->nameLineEdit->text().length() && ui->birthDateLineEdit->text().length()
            && ui->directoryLineEdit->text().length()
            ||
            ui->nameLineEdit->text().length() && ui->birthDateLineEdit->text().length()
            && ui->directoryLineEdit_2->text().length()
            ||
            ui->nameLineEdit->text().length() && ui->birthDateLineEdit->text().length()
            && ui->directoryLineEdit->text().length() && ui->directoryLineEdit_2->text().length()
            )
    {
        QListWidgetItem* item= new QListWidgetItem;
        item->setSizeHint(QSize(380,130));

        ListWidget* L ;  L = new ListWidget(id, name, birthdate, directory, directory2, type, gender, path);

        ui->personList->addItem(item);

        ui->personList->setItemWidget(item, L);

        dbList.insert(id, L);
    }
    else {
        QMessageBox::warning(this, "알림", "환자 정보를 입력해 주세요");
    }

    ui->nameLineEdit->clear();
    ui->birthDateLineEdit->clear();
    ui->directoryLineEdit->clear();
    ui->directoryLineEdit_2->clear();
}


void XrayDB::on_searchPushButton_clicked()
{

    QString name, birthdate, directory, directory2, type, gender, path;

    QString index;

    QPixmap image;

    int id;

    ui->searchWidget->clear();

    index =  ui->searchLineEdit->text();

    id = index.toInt();

    auto Val = dbList.find( id );

    if( Val == dbList.end() )
    {
        QMessageBox::warning(this,"알림", "환자 정보가 존재하지 않습니다.");
    }
    else{
        name = Val.value()->getName();
        birthdate = Val.value()->getBirthdate();
        directory = Val.value()->getDirectory();
        directory2 = Val.value()->getDirectory2();
        type = Val.value()->getType();
        path = Val.value()->getImage();
        gender = Val.value()->getGender();
        image = QPixmap(path);

        ListWidget *li = new ListWidget(id, name, birthdate, directory, directory2, type, gender, path);

        QListWidgetItem* item= new QListWidgetItem;

        item->setSizeHint(QSize(373,130));
        ui->searchWidget->addItem(item);
        ui->searchWidget->setItemWidget(item, li);
        ui->searchWidget->addItem(item);
        ui->searchWidget->setItemWidget(item, li);
    }

}
void XrayDB::on_modifyPushButton_clicked()
{

    // QListWidgetItem *item, ListWidget *li ;

    QListWidgetItem* item =  ui->personList->currentItem();

    if (item != nullptr)
    {
        ListWidget* li = (ListWidget*)ui->personList->itemWidget(item);

        QString name, birthdate, directory, directory2, type, gender;

        QPixmap image;

        name = ui->nameLineEdit->text();
        birthdate = ui->birthDateLineEdit->text();
        directory = ui->directoryLineEdit->text();
        directory2 = ui->directoryLineEdit_2->text();

        ui->nameLineEdit->setText(name);
        ui->birthDateLineEdit->setText(birthdate);
        ui->directoryLineEdit->setText(directory);
        ui->directoryLineEdit_2->setText(directory2);

        if( ui->panoramaRadioButton->isChecked() )
        {
            type = "Pano";
        }
        else if( ui->cephRadioButton->isChecked() )
        {
            type = "Ceph";
        }
        if(ui->maleRadioButton->isChecked() )
        {
            gender = "Male";

            image = QPixmap("male.png");
        }
        else if(ui->femaleRadioButton->isChecked() )
        {
            gender = "Female";

            image = QPixmap("female.png");
        }

        if (name.length())
        {
            item->setSizeHint(QSize(400,130));
            li->setName(name);
            li->setBirthdate(birthdate);
            li->setDirectory(directory);
            li->setDirectory2(directory2);
            li->setImage(image);
            li->setType(type);
            li->setGender(gender);
        }
        else {
            QMessageBox::warning(this, "알림", "환자 정보를 입력해 주세요");
        }
    }
}
void XrayDB::on_personList_itemClicked(QListWidgetItem *item)
{
    item =  ui->personList->currentItem();

    if (item != nullptr)
    {
        ListWidget* li = (ListWidget*)ui->personList->itemWidget(item);
        QString name, birthdate, directory, directory2, type, gender;

        QPixmap image;

        name = li->getName();

        birthdate = li->getBirthdate();

        directory = li->getDirectory();

        directory2 = li->getDirectory2();

        image = li->getImage();

        ui->nameLineEdit->setText(name);

        ui->birthDateLineEdit->setText(birthdate);

        ui->directoryLineEdit->setText(directory);

        ui->directoryLineEdit_2->setText(directory2);

        if( li->getGender() == "Female")
        {
            ui->femaleRadioButton->setChecked(true);
        }
        else if ( li->getGender() == "Male")
        {
            ui->maleRadioButton->setChecked(true);
        }

        if(  li->getType() == "Ceph"  )
        {
            ui->cephRadioButton->setChecked(true);
        }
        else  if ( li->getType() == "Pano")
        {
            ui->panoramaRadioButton->setChecked(true);
        }
    }
}
void XrayDB::on_deletePushButton_clicked()
{
    int id;
    QString name, birthdate, directory, directory2, type, gender, path;

    QListWidgetItem* item =  ui->personList->currentItem();
    ListWidget *L = (ListWidget*)ui->personList->itemWidget(item);

    if (item != nullptr)
    {
        delete item;
        id = L->id();
        dbList.remove(id);
    }
    update();
}

void XrayDB::on_filePushButton_clicked()
{
     QString filename = QFileDialog::getOpenFileName(this);

     ui->directoryLineEdit->setText(filename);
}


void XrayDB::on_filePushButton2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);

    ui->directoryLineEdit_2->setText(filename);
}
