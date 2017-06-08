#ifndef AUTORENAMESHOT_H
#define AUTORENAMESHOT_H

#include <QThread>
#include <QFile>
#include <QDateTime>
#include <QSettings>

class AutoRenameShot : public QThread
{
    Q_OBJECT
public:
    explicit AutoRenameShot(QObject *parent = 0);
    //! starter da thread
    void run();

private:
    //! Método único para renomear screenshots tirados a partir do Wayland.
    void renameAndMoveTo();
    //!Novo nome para o arquivo, somado do dateTime do momento da renomeação.
    QString newFileName;
    //! Manipulação do arquivo no sistema de arquivos.
    QFile file;

signals:

public slots:

};

#endif // AUTORENAMESHOT_H
