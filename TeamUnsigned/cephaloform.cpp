#include "cephaloform.h"
#include "ui_cephaloform.h"

CephaloForm::CephaloForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CephaloForm)
{
    ui->setupUi(this);
}

CephaloForm::~CephaloForm()
{
    delete ui;
}
