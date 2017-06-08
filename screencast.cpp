#include "screencast.h"
#define EXITCRASH 1
#define EXITOK 0
#define ABORTED 10
Screencast::Screencast(QObject *parent) :
    QThread(parent)
{
}

void Screencast::abortConvertion()
{
    this->kill = true;
}

void Screencast::castingConvert()
{
    QProcess webm;
    QRegExp rx;
    //"Pass 1/1 frame    6/5     223065B 4318668 us 1.39 fps"
    //Pass 1/1 frame   17/16    228672B   18005 ms 56.65 fpm [ETA  unknown] [K"
    QRegExp rxBytes("\\d{1,}/\\d{1,}\\s{1,}(\\d{2,}\\w)");
    QString value ;
    QString command = "/bin/castconvert.sh " + this->source + " " + this->target;
    emit log("Arquivo de origem: "+this->source.toUtf8()+" Arquivo de destino: "+this->target.toUtf8()+" ");

    webm.start(QString(command));
    emit log("[step] Processo de conversão de casting iniciado. ");
    rx.setPattern("Pass.*fp\\w\\)?");
    while(webm.pid() > 0){
        if (this->kill){
            this->kill = false;
            emit log("[user] Usuário cancelou o processo de conversão do screencast. ");
            emit signalIsFinished(ABORTED);
            emit signalLabelBytesConverted(tr("Interrompido"));
            webm.terminate();
            return;
        }
        webm.waitForReadyRead();
        QString stat = webm.readAllStandardOutput();
        if (stat.contains("Fatal:")){
            QStringList msg;
            msg << tr("Ocorreu algum erro na conversão. Copie o arquivo wcap e converta em outro equipamento") << " " << " ";
            emit signalRunMessageBox(msg);
            emit log("[warning] Ocorreu algum erro na conversão. Copie o arquivo wcap e converta em outro equipamento");
            emit signalLabelBytesConverted(tr("::: Erro :::"));
            return;
        }
        rx.indexIn(stat);
        stat = rx.capturedTexts().last();
        if (stat.contains(QRegExp("fp\\w"))){
            emit signalLabelStatus(stat);
            rxBytes.indexIn(stat);
            value = rxBytes.capturedTexts().last();
            emit signalLabelBytesConverted(value);
        }
    }
    webm.close();
    emit log("[step] Processo de conversão de casting finalizado sem erros aparentes. ");
    QFile newCap(this->target);
    if (newCap.exists()){
        emit signalLabelBytesConverted(tr("Finalizado"));
    }
    else {
        emit signalLabelBytesConverted(tr("Falha no Processo"));
    }
}

QString Screencast::getWcapFullPath()
{
    return this->source;
}

void Screencast::run()
{
    this->castingConvert();
}

void Screencast::setSourceAndTarget(QString s, QString t)
{
    this->source = s;
    this->target = t;
}
