#include "systeminfo.h"
#define DELAY_GET_INFO 5
SystemInfo::SystemInfo(QObject *parent) :
    QThread(parent)
{
}

void SystemInfo::getCpuInfo()
{
    //LOAD AVERAGE
    QString result;
    QFile avg("/proc/loadavg");
    if (!avg.open(QIODevice::ReadOnly | QIODevice::Text)){
        emit cpu("Error 0",-1);
        return;
    }
    result = avg.readLine().trimmed();
    avg.close();

    if (result.length() <10){
        emit cpu("Error 1",-1);
        return;
    }

    QString temp = result.split(" ").at(0);
    temp = temp.remove(".");
    int cpuLoad = temp.toInt();

    //CORES
    QFile cores("/sys/devices/system/cpu/online");
    if (!cores.open(QIODevice::ReadOnly | QIODevice::Text)){
        emit cpu("Error 2",-1);
        return;
    }
    QString results;
    results = cores.readAll();
    results = results.trimmed();
    if (results.contains("-")){
        results = results.split("-").at(1);
    }
    else{
        QRegExp rx("\\d");
        if (!results.contains(rx)){
            emit cpu("erro 2 (NULL)",-1);
        }
    }

    cores.close();

    QRegExp rxNum("(\\d)");
    if (results.isEmpty()){
        emit cpu("Error 3",-1);
        return;
    }

    int conv = results.toInt()+1;
    emit cpu(QString::number(conv),cpuLoad);

}

void SystemInfo::getMemoryInfo()
{
    QProcess mem;
    mem.start(QString("free -m"));
    if (!mem.waitForFinished()){
        return;
    }
    QString result = mem.readAllStandardOutput().trimmed();
    QRegExp rx("Mem:\\s{1,}(\\d{1,})\\s{1,}(\\d{1,})");
    int pos = rx.indexIn(result);
    QStringList strs = rx.capturedTexts();
    rx.setPattern(".*cache:\\s{1,}\\d{1,}\\s{1,}(\\d{1,})");
    pos = rx.indexIn(result);
    QStringList buffers = rx.capturedTexts();
    emit memory(strs.at(1).toInt(),strs.at(2).toInt(), buffers.last().toInt());
    mem.close();

}

void SystemInfo::stopNow()
{
    this->stop = true;
}

void SystemInfo::run()
{
    this->stop = false;
    while (!stop){
        this->getMemoryInfo();
        this->getCpuInfo();
        sleep(DELAY_GET_INFO);
    }
}

