#ifndef SCREENCAST_H
#define SCREENCAST_H

#include <QThread>
#include <QRegExp>
#include <QProcess>
#include <QFile>

class Screencast : public QThread
{
    Q_OBJECT
public:
    explicit Screencast(QObject *parent = 0);

    //! Starter da thread
    void run();

    //! Identificação de origem e destino
    void setSourceAndTarget(QString s, QString t);

    //! Recebe o caminho absoluto do arquivo wcap
    QString getWcapFullPath();

private:
    //! Conversor do casting
    void castingConvert();

    //! Recebe identificação de origem
    QString source;

    //! Recebe identificação de destino
    QString target;

    //! Recebe atribuição de comando para finalizar ou não
    bool kill=false;

public slots:
    //! Alimentador da variável de finalização da conversão
    void abortConvertion();

signals:
    //! Sinal para que a classe Common crie os messagebox
    void signalRunMessageBox(QStringList texts);

    //! Emissor das mensagens a gravar no log. O método receptor log() está declarado na MainWindow.
    void log(QByteArray msg);

    //! Emissão de mensagem para o label_status da MainWindow
    void signalLabelStatus(QString msg);

    //! Sinal de finalização com status, aplicado de forma diferenciada ao finished() da thread
    void signalIsFinished(int stat);

    //! Label do próprio menu correspondente à conversão do casting
    void signalLabelBytesConverted(QString val);

public slots:

};

#endif // SCREENCAST_H
