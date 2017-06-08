#ifndef GPTANDFORMAT_H
#define GPTANDFORMAT_H

#include <QThread>
#include <QStringList>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include "common.h"

class GPTandFormat : public QThread
{
    Q_OBJECT
public:
    explicit GPTandFormat(QObject *parent = 0, Common *common = 0);

    //! Método que armazena os valores de alvo e tipo de sistema de arquivos.
    void deviceAndFileSystemToFormat(QString device, QString fileSystem);

    //! Dispositivo
    QString dev;
    //! Sistema de arquivos
    QString fs;
    //! Starter da thread
    void run();
    //! Instancia da classe common, para acessar dados compartilhados pela MainWindow
    Common *comm;

private:

    //! Método de formatação da mídia, utilizando o gdisk
    int formatThisDeviceUsingThisFS(QString device, QString fileSystem);

    //! Validador de partição existente
    int isTherePartition(QString device);

    //! Valida o dispositivo e adiciona o prefixo /dev a ele
    QString prepareDevice(QString device);

signals:
    //! Devolve status de sucesso se tudo ocorrer sem problemas
    void sucess();
    //! Envia a mensagem de status para a MainWindow
    void signalStatusLabel(QString txt);

    //! Sinal para que a classe Common crie os messagebox
    void signalRunMessageBox(QStringList msgs);

    //! Alimenta o progressBar da MainWindow com porcentagem baseada na execução dos comandos de formatação
    void signalSetProgressBarValNow(int val);

    //! Emissor das mensagens a gravar no log. O método receptor log() está declarado na MainWindow.
    void log(QByteArray msg);

public slots:

};

#endif // GPTANDFORMAT_H
