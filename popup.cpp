#include "popup.h"
#include "ui_popup.h"
#include <QDebug>

popup::popup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::popup)
{
    ui->setupUi(this);
}

void popup::setGroupBoxTitle(const QString &val)
{
    ui->groupBox->setTitle(val);
}

void popup::setTextEditText(const QString &val)
{
    ui->textBrowser->setText(val);
    qDebug() << this->value;
}

popup::~popup()
{
    delete ui;
}
