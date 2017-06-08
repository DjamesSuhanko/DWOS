#ifndef WIPING_H
#define WIPING_H

#include <QThread>
#include <QProcess>
#include <QRegExp>
#include <QFile>

class Wiping : public QThread
{
    Q_OBJECT
public:
    explicit Wiping(QObject *parent = 0);

    //! Dispositivo para limpar
    QString dev;

    //! Starter da thread
    void run();

    //! Identificador do tipo de wipe a executar.
    void setFullWipe(bool fullWipe);

private:
    //! Método de início da limpeza do disco
    void wiping(QString device);

    //! interruptor do processo (que não deverá ser de todo eficiente, considerando que o perdido estará perdido)
    bool killWipe;

    //! Hot wipe, útil para eliminar dados em setores do disco após criptografar e remover os arquivos originais
    void hotWipe();

    //! Boolean verificado no run() para full ou hot wipe
    bool doFullWipe=false;

signals:
    //! Emissor de mensagem do processo de wiping
    void msgFromWipe(QString msg);

    //! Status da operação
    void status(int stat);

    //! Envia mensagem para exibição em message box popup.
    void signalMsgBox(QStringList);

    //! Emissor das mensagens a gravar no log. O método receptor log() está declarado na MainWindow.
    void log(QByteArray msg);

public slots:
    //! Slot que recebe sinal para executar a finalização do processo
    void slotTerminateWipe();

private slots:
    void error(QProcess::ProcessError err);


};

#endif // WIPING_H
