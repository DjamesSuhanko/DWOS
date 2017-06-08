#include "ewfrelat.h"
#include "ui_ewfrelat.h"

ewfRelat::ewfRelat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ewfRelat)
{
    ui->setupUi(this);
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(mountString()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(close()));
    ui->label_case->setText(tr("Caso Nº"));
}

void ewfRelat::mountString()
{
    QString iniFile = "/home/djames/dwatson.ini";

    QSettings settings(iniFile,QSettings::IniFormat);
    QString nome = settings.value("wayland/nome","wayland/nome").toString();

    forms  = ui->lineEdit_caseNumber->text()+"|"+ui->lineEdit_evidenceNumber->text()+"|"+ui->lineEdit_Examiner->text()+"|";
    QString desc = ui->textEdit_description->toPlainText();
    if (desc.isEmpty() || desc.length() < 2){
        desc = settings.value("dwatson.ewfform/desc","___").toString();
    }
    QString notesStr = ui->textEdit_notes->toPlainText();
    if (notesStr.length() <2 || notesStr.isEmpty()){
        notesStr = settings.value("dwatson.ewfform/notesStr","___").toString();
    }
    forms += desc+"|"+notesStr;
    emit stringsFromForm(forms);
    emit close();
}

ewfRelat::~ewfRelat()
{
    delete ui;
}
