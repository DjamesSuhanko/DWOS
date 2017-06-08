#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <QThread>
#include <QProcess>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QSysInfo>
#include <QRegExp>
#include <QFileInfo>

class Encrypt : public QThread
{
    Q_OBJECT
public:
    explicit Encrypt(QObject *parent = 0);

    //! Esse método recebe o comando completo montado a partir da MainWindow e guarda na variável QString command.
    void setCryptCommand(QString comm);

    //! Alimenta booleano para identificar se o arquivo original deve ser removido após criptografia.
    void removeFileAfterCrypt(bool yesno);

    //! Starter da thread
    void run();

private:
    //! Contador das partes, para saber se deve emitir mensagem de finalizado após ler de todas as imagens
    int counter     = 0;
    //! Contador das partes, para saber se deve emitir mensagem de finalizado após ler de todas as imagens
    int waitCounter = 1;

    //! Guarda o comando a executar
    QString command;
    //! Guarda somente o nome do arquivo
    QString filename;
    //!Guarda relação de nomes de arquivos coincidentes com o nome passado no comando, excluindo sua extensão.
    QStringList entries;
    //! Guarda somente o mount point passado através do comando completo
    QString mountPoint;
    //! Para manipular arquivos na raiz do sistema (remoção, listagem, etc).
    QString file;
    //! Guarda somente o nome do arquivo, sem a extensão.
    QString fileWithoutExtension;

    //! Dados estatísticos para avaliação prévia do sucesso da operação
    long int freeSpace=0;
    //! Dados estatísticos para avaliação prévia do sucesso da operação
    long int imgTotalSize=0;
    //! Dados estatísticos para avaliação prévia do sucesso da operação
    long int imgFirstSlice=0;

    //!Remove arquivo original após criptográ-lo
    bool removeOriginal=false;

    //! Método para medição do espaço livre no destino.
    void getFreeSpace();
    //! Define se o arquivo é único ou se foi dividido em partes.
    void fileSingleOrSplited();
    //! Inicializador de encriptação utilizando o AESCrypt no backend.
    void AESCrypt(QString fileToEncryptWithFullPath);
    //! Inicializador de encriptação utilizando o OpenSSL como backend.
    void OpenSSL(QString fileToEncryptWithFullPath);
    //! Totaliza o espaço necessário para armazenamento da imagem criptografada para saber se caberá em disco, caso não seja removido o original.
    int totalizer();

public:



signals:
    //! Emissor das mensagens a gravar no log. O método receptor log() está declarado na MainWindow.
        void log(QByteArray msg);
        //! Emissão de sinal com código de saída, de forma diferenciada do finished() da própria QThread.
        void signalIsFinished(int exitCode);
        //! Envia mensagem para exibição em message box popup.
        void signalMsgbox(QStringList msg);
        //! Interruptor do processo em execução.
        void stopTask();
        //! Status da operação, enviada para a MainWindow.
        void status(QString message);

public slots:
        //! Interrutor da operação.
        /*! Recebe o sinal de finalização solicitada pelo usuário através da MainWindow. Então emite a mensagem de log e seguidamente
         * emite o sinal stopTask, que interrompe o QProcess através de seu método terminate().
         *
         */
        void slotStopCrypt();

private slots:
        //! Sinal de finalização da tarefa, implementado de forma diferente do método finished() da própria QThread
        void cryptIsFinished(int stat);

        //!Garantir que saiu sem erro para enviar o status de saída à MainWindow, que por sua vez, informa ao usuário
        void slotCryptProcessStatusError(QProcess::ProcessError);

};

#endif // ENCRYPT_H
