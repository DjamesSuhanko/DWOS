#ifndef IOSTATS_H
#define IOSTATS_H

#include <QThread>
#include <QProcess>
#include <QRegExp>

class IOStats : public QThread
{
    Q_OBJECT
public:
    explicit IOStats(QObject *parent = 0);

    //! Starter da thread
    void run();

    //! Atribuição de dispositivo a monitorar
    void setDevice(QString dev);

    //! Limpa a variável de atribuição após utilização
    void resetDevice();

    //! Ordem de parada
    void setStop(bool stat);

    //! Configura o modo de operação da thread, para capturar valores da leitura ou da escrita em disco.
    void setWriteOrRead(int value);

private:
    //! Filtragem do resultado da leitura de background do iostat.
    void statsFromDeviceSelected(QString fromIOstat,QRegExp *regex);

    //! Apenas deslocamento de posição da string para inicio do IOStat
    int startCapture=0;

    //! Dispositivo a monitorar
    QString device;

    //! Encerrar tarefa (boolean)
    bool done=false;

    //! Define se a classe deve ler da escrita ou da leitura em disco. Da leitura, útil na geração de hash.
    int readOrWrite = 2; //1 e 2, respectivamente

signals:
    //! Emissor do valor em MBps para a MainWindow
    void signalMbps(QString mbps);

public slots:

};

#endif // IOSTATS_H
