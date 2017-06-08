#include "iostats.h"

IOStats::IOStats(QObject *parent) :
    QThread(parent)
{

}

void IOStats::run()
{
    if (this->device.isEmpty()){
        return;
    }
    QProcess iostat;
    QString result;
    QString teste;
    this->device = this->device.remove(QRegExp("\\d"));
    QRegExp rx("^"+this->device+"$");
    QString command = "iostat  -h -m "  + this->device + " 1";
    iostat.start(command);
    while (iostat.pid() > 0){
        if (this->done){
            break;
        }
        if (iostat.waitForReadyRead()){
            result = iostat.readAllStandardOutput();
            this->statsFromDeviceSelected(result,&rx);
        }
    }
}

void IOStats::resetDevice()
{
    if (!this->isRunning()){
        this->device.clear();
    }
}

void IOStats::setDevice(QString dev)
{
    if (dev.contains("sd")){
        this->device = dev;
    }
}

void IOStats::statsFromDeviceSelected(QString fromIOstat,QRegExp *regex)
{
    //Device:            tps    MB_read/s    MB_wrtn/s    MB_read    MB_wrtn
    //sda
    //                  0,00         0,00         0,00          0          0
    //descartar as primeiras 2 linhas pra garantir
    this->startCapture = this->startCapture > 2 ? this->startCapture : this->startCapture+1;

    if (fromIOstat.isEmpty() || !fromIOstat.contains(QRegExp("\\d")) || this->startCapture < 2){
        return;
    }
    QString result;
    QStringList res = fromIOstat.split("\n");
    for (int i=0;i<res.length();i++){
        if (res.at(i).contains(*regex)){
            if (res.at(i+1).contains(QRegExp("\\d"))){
                result = res.at(i+1).trimmed().simplified().split(" ").at(this->readOrWrite);
                  emit signalMbps(result);
            }
        }
    }
}

void IOStats::setStop(bool stat)
{
    this->done = stat;
}

void IOStats::setWriteOrRead(int value)
{
    this->readOrWrite = value;
}
