#ifndef EWFRELAT_H
#define EWFRELAT_H

#include <QDialog>
#include <QTranslator>
#include <QSettings>
namespace Ui {
class ewfRelat;
}

class ewfRelat : public QDialog
{
    Q_OBJECT

public:
    explicit ewfRelat(QWidget *parent = 0);
    ~ewfRelat();

private:
    Ui::ewfRelat *ui;
    //! Linha completa com todas as entradas para a geração de relatório de operação do formato EWF.
    QString forms;

private slots:
    //! Montador da string, recolhendo as informações do formulário popup.
    void mountString();

signals:
    //! Emissor do resultado da string formatada, que será posteriormente tratada pela MainWindow.
    void stringsFromForm(QString str);
};

#endif // EWFRELAT_H
