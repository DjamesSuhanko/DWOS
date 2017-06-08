#include "wiping.h"

Wiping::Wiping(QObject *parent) :
    QThread(parent)
{
    this->killWipe = false;
}

void Wiping::error(QProcess::ProcessError err)
{
    if (err == QProcess::UnknownError){
        emit log("[warning] Erro desconhecido durante full wipe ");
        emit signalMsgBox(tr("Erro desconhecido durante execução do processo de wipe - Erro Desconhecido").split("-"));
    }
    else{
        emit signalMsgBox(tr("Processo de wipe finalizado. - Final de Processo").split("-"));
        emit log("[step] Wipe finalizado ");
    }
}

void Wiping::hotWipe()
{
    QProcess wiper;
    QString commandWiper = "dd if=/dev/zero of=/media/images/DjamesWatson";

    wiper.start(commandWiper);

    wiper.waitForFinished(-1);
    if (wiper.isOpen()){
        wiper.close();
    }
    QFile::remove("/media/images/DjamesWatson");
    emit signalMsgBox(tr("Processo de eliminação de dados residuais finalizado. - HotWipe").split("-"));
}


void Wiping::run()
{
    if (this->dev.isEmpty() && this->doFullWipe){
        emit log("Dispositivo não informado ao processo wipe - Processo interrompido");
        emit signalMsgBox(tr("Dispositivo não informado ao processo wipe - Processo interrompido").split("-"));
        return;
    }
    if (this->doFullWipe){
        this->wiping(this->dev);
        this->dev.clear();
        return;
    }
    this->hotWipe(); //sempre em /media/images

}

void Wiping::wiping(QString device)
{
    if (!device.contains("/dev/")){
        if (device.length() < 3){
            log("Dispositivo não informado ao processo wipe - Processo interrompido");
            return;
        }
        dev = "/dev/"+device;
    }

    //Se estiver em somente leitura no destino, certamente dará problema, portanto, tratando:
    if (device.length() >4){
        device = device.split("/").last().left(3);
    }
        QFile file("/sys/block/"+device+"/ro");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QStringList msgs;
            msgs << tr("Não foi possível ler o estado da mídia (RO ou RW).") << tr("Impossível ler estado da Mídia") << "";
            emit log("[warning] " + msgs.at(0).toUtf8());
            emit signalMsgBox(msgs);
            return;
        }

        QString status = file.readLine();
        file.close();

        if (status.isEmpty()){
            emit log("[warning] Não foi possível ler status da midia para wipe. ");

        }
        else if (status.toInt() == 1){
            QStringList msgs;
            msgs << tr("A mídia de destino está protegida contra gravação.") << tr("Mídia em Modo Somente-Leitura");
            emit log("[warning] " + msgs.at(0).toUtf8());
            emit signalMsgBox(msgs);
            return;
        }

    QRegExp digits("\\d");
    dev = dev.remove(digits);

    QProcess wipe;
    QString line,result,rate;
    QString percent;

    wipe.setProcessChannelMode(QProcess::MergedChannels);
    QStringList param;
    param << "wipe="+dev;

    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    connect(&wipe,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
    wipe.start("dc3dd",param);
    emit log("[user] Iniciado wipe ");
    int go = 1;
    while (wipe.pid() > 0){
        if (!wipe.waitForReadyRead()){
            emit log("[warning] Tempo limite expirado para wipe ");
            emit signalMsgBox(tr("Tempo limite excedido para o processo de wipe - Tempo Excedido").split("-"));
            return;
        }
        if (this->killWipe){
            wipe.terminate();
            emit msgFromWipe(tr("Processo finalizado pelo usuário."));
            emit log("[user] Processo finalizado pelo usuário ");
            this->killWipe = false;
            return;
        }
        bool haveOne = false;
        line = wipe.readAllStandardOutput().trimmed();
        QRegExp rx("(\\d{4,}\\s\\w{1,}\\s\\(\\d{1,},?\\d?\\s\\w\\))");
        if (rx.indexIn(line) > -1){
            result = rx.capturedTexts().last();
            haveOne = true;
        }

        rx.setPattern("(\\d{1,}%)");
        if (rx.indexIn(line) > -1){
            percent = rx.capturedTexts().last();
            haveOne = true;
        }

        rx.setPattern(",\\s(\\d{1,},?\\d?\\s\\w/\\w)");
        if (rx.indexIn(line)){
            rate = rx.capturedTexts().last();
            haveOne = true;
        }

        if (go > 0){
            if (haveOne){
                emit msgFromWipe(result+"|"+percent+"|"+rate);
            }
        }
        go = percent.toInt() >99 ? 0 : 1;
    }
    wipe.close();
    this->doFullWipe = false;
}

void Wiping::setFullWipe(bool fullWipe)
{
    this->doFullWipe = fullWipe;
}

void Wiping::slotTerminateWipe()
{
    this->killWipe = true;
}
