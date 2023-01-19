#ifndef CEPHALOFORM_H
#define CEPHALOFORM_H

#include <QWidget>

namespace Ui {
class CephaloForm;
}

class CephaloForm : public QWidget
{
    Q_OBJECT

public:
    explicit CephaloForm(QWidget *parent = nullptr);
    ~CephaloForm();

private:
    Ui::CephaloForm *ui;
};

#endif // CEPHALOFORM_H
