#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QThread>
#include <QProcess>
#include <QFile>
#include <QRegExp>

class SystemInfo : public QThread
{
    Q_OBJECT
public:
    explicit SystemInfo(QObject *parent = 0);

    //! Starter da thread
    void run();

    //! Coletor de informações relacionadas ao processador
    void getCpuInfo();

    //!  Coletor de informações de memória
    void getMemoryInfo();

    //! Paralização da thread
    void stopNow();

private:
    //! Variável de controle de parada
    bool stop;

signals:
    //! Emissão dos valores coletados do processador para a MainWindow
    void cpu(QString numberOfCPUs,int cpuUsage);

    //! Emissão dos valores de memória coletados, a exibir na MainWindow
    void memory(int total, int used, int cache);

public slots:

};

#endif // SYSTEMINFO_H
