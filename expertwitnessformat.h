#ifndef EXPERTWITNESSFORMAT_H
#define EXPERTWITNESSFORMAT_H

#include <QThread>
#include <QProcess>
#include <QRegExp>
#include <QFile>
#include <QStringList>
#include <QMap>

class ExpertWitnessFormat : public QThread
{
    Q_OBJECT
public:
    explicit ExpertWitnessFormat(QObject *parent = 0);

    //! Inicializador da thread
    void run();

    //! Alimentador do comando, sendo a única forma pública de acessar a string de comando
    void setParamsToEWFcopy(QString str);

private:
    //! Variável que recebe a string formada a partir dos widgets da MainWindow para geração de imagem EWF
    QString params;

    //! boolean para que o loop saiba o momento de interromper o processo, caso seja solicitado.
    bool stopNow=false;

    //! Alternador de mensagem
    /*! A ferramenta de background envia duas informações importantes relacionadas ao andamento do processo. Para
     * exibição de ambas as informações, é feito um alternamento entre as mensagens a emitir para a MainWindow, de
     * forma que ambas sejam visualizadas pelo usuário.
    */
    int alternate=0;

    //! Método que faz a alternância da mensagem recebida pela ferramenta de background.
    void alternateMessageToSend(QString zero, QString one);

    //! A mensagem de alternância é armazenada e substituida nesse QMap, que atua como um simples array de duas posições.
    QMap <int,QString> mainMessage;

signals:
    //! Emissor das mensagens a gravar no log. O método receptor log() está declarado na MainWindow.
    void log(QByteArray msg);

    //! Envia mensagem para exibição em message box popup.
    void signalMsgbox(QStringList msg);

    //! Emissor de status da operação
    void signalMsgStatus(QString statMsg);

    //! Emissor de progresso, conectado ao progressbar na MainWindow
    void signalProgressBar(int val);

public slots:
    //! Interruptor do processo de geração de imagem EWF.
    void slotStopEWFprocess();

private slots:
    //! Mensagem de finalização do processo, atuando diferentemente da finished() contida na própria QThread.
    void slotFinished(int status);


};

#endif // EXPERTWITNESSFORMAT_H
