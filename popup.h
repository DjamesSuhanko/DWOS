#ifndef POPUP_H
#define POPUP_H

#include <QDialog>

namespace Ui {
class popup;
}

class popup : public QDialog
{
    Q_OBJECT

public:
    explicit popup(QWidget *parent = 0);

    //! Configuração do texto de exibição
    void setTextEditText(const QString &val);

    //! Configuração do nome do groupbox
    void setGroupBoxTitle(const QString &val);

    ~popup();
    Ui::popup *ui;

    //! Valor de mensagem
    QString value;

private:


};

#endif // POPUP_H
