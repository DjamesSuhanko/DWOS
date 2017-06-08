#include "gptandformat.h"
#define WAITING_TIME 6000
GPTandFormat::GPTandFormat(QObject *parent, Common *common) :
    QThread(parent)
{
    this->comm = common;

}

void GPTandFormat::deviceAndFileSystemToFormat(QString device, QString fileSystem)
{
    this->dev = device;
    this->fs  = fileSystem;
}

int GPTandFormat::formatThisDeviceUsingThisFS(QString device, QString fileSystem)
{

    QString dev;
    dev = this->prepareDevice(device);

    QString formatCommand = "mkfs."+fileSystem.toLower();
    QStringList strings;
    QStringList commands;
    QString fs;
    QStringList paramsToFormat;
    if (fileSystem.contains("NTFS")){
        fs = "0700\n";
        paramsToFormat << "-v" << "-f";
        emit log("[user] Formatar com NTFS");
    }
    else{
        fs = "8300\n";
        if (fileSystem.contains("XFS")){
            paramsToFormat << "-f";
            emit log("[user] Formatar com XFS");
        }
        else{
            paramsToFormat << "-v";
            emit log("[user] Formatar com EXTn");
        }
    }

    QProcess formatDisk;
    QStringList params;
    int status = 1;
    params << dev;
    strings << ".*help\\): " << ".*Proceed\\? \\(Y/N\\): " << ".*Command \\(\\? for help\\): " << ".*Partition number \\(1-128, default 1\\): ";
    strings << " " << " " << " ";
    strings << ".*Command \\(\\? for help\\): " << ".*Do you want to proceed\\? \\(Y/N\\): ";
    commands << "o\n" << "y\n" << "n\n" << "1\n" << "\n" << "\n" << fs << "w\n" << "y\n";
    formatDisk.start("gdisk",params); //"gdisk /dev/sdX", sem digito, tratado no prepareDevice

    if (!formatDisk.waitForReadyRead(WAITING_TIME)){
        QStringList msgs;
        emit log("[warning] Processo finalizado inesperadamente");
        msgs << tr("O processo finalizou inesperadamente logo após ser iniciado. Informe o suporte.");
        msgs << tr("Processo finalizado com erro");
        msgs << " ";
        emit signalRunMessageBox(msgs);
        msgs.clear();
        return -1;
    }

    QString t =  formatDisk.readAllStandardOutput();
    if (t.isEmpty()){
        QStringList msgs;
        emit log("[warning] Falha no processo de controle do particionamento");
        msgs << tr("Houve falha na intercomunicação com o processo de controle do particionamento.");
        msgs << tr("Processo finalizado com erro");
        msgs << " ";
        emit signalRunMessageBox(msgs);
        msgs.clear();
        return -1;
    }

    QRegExp rx;
    int percent = 1;
    rx.setPattern("GPT\\sfdisk\\s\\(gdisk\\)\\sversion\\s\\d{1,}\\.?\\d{0,}\\.?\\d{0,}\n\n$");
    if (t.contains(rx)){
        rx.setPattern("Your\\sanswer:\\s");
        while (!t.contains(rx)){
            formatDisk.waitForReadyRead(WAITING_TIME);
            t = formatDisk.readAllStandardOutput();
        }
        formatDisk.write("2\n");
        formatDisk.waitForReadyRead(WAITING_TIME);
        t = formatDisk.readAllStandardOutput();
    }

    for (int i=0;i<commands.length();i++){
        if (i>3 && i<7){
            status = formatDisk.write(commands.at(i).toUtf8());
            if (status < 0){
                QStringList msgs;
                msgs << tr("O processo falhou no passo ")+QString::number(i)+tr(". Informe ao suporte.");
                emit log("[warning] " + msgs.at(0).toUtf8());
                msgs << tr("Falha durante execução");
                msgs << tr("Você pode tentar outra mídia, ou então preparar essa mídia previamente ao uso, descartando a opção formatar.");
                emit signalRunMessageBox(msgs);
                msgs.clear();
                return status;
            }
            if (!formatDisk.waitForReadyRead(WAITING_TIME)){
                QStringList msgs;
                msgs << tr("O processo finalizou inesperadamente no passo ")+QString::number(i)+tr(", ao aguardar para o próximo passo. Informe ao suporte.");
                emit log("[warning] " + msgs.at(0).toUtf8());
                msgs << tr("Processo finalizado com erro");
                msgs << tr("Você pode tentar outra mídia, ou então preparar essa mídia previamente ao uso, descartando a opção formatar.");
                emit signalRunMessageBox(msgs);
                msgs.clear();
                return -1;
            }
            t =  formatDisk.readAllStandardOutput();
            if (t.isEmpty()){
                QStringList msgs;
                msgs << tr("O processo finalizou inesperadamente no passo ")+QString::number(i)+tr(", ao aguardar para o próximo passo. Informe ao suporte.");
                emit log("[warning] " + msgs.at(0).toUtf8());
                msgs << tr("Falha do processo em segundo plano");
                msgs <<  tr("Você pode tentar outra mídia, ou então preparar essa mídia previamente ao uso, descartando a opção formatar.");
                emit signalRunMessageBox(msgs);
                msgs.clear();
                return -1;
            }
        }
        else if (i<4 || i>6){
            rx.setPattern(strings.at(i));
            rx.indexIn(t);
            QString result = rx.capturedTexts().last();
            if (result.length() < 2){
                QString regex = strings.at(i);
                QString msgToUser = tr("Ocorreu um erro no passo ")+QString::number(i)+tr(" aguardando pela seguinte porção de bytes:\n");
                emit log("[warning] " + msgToUser.toUtf8());
                msgToUser += regex + "\n";
                msgToUser += tr("A resposta obtida foi:\n");
                msgToUser += t;

                QStringList msgs;
                msgs << tr("Ocorreu um erro inesperado. Veja informações nos detalhes.");
                msgs << tr("Erro durante operação");
                msgs << msgToUser;
                emit signalRunMessageBox(msgs);
                msgs.clear();
                return -1;
            }
            status = formatDisk.write(commands.at(i).toUtf8());
            if (status < 0){
                QStringList msgs;
                msgs << tr("O processo falhou no passo ")+QString::number(i)+tr(". Informe ao suporte.");
                emit log("[warning] " + msgs.at(0).toUtf8());
                msgs << tr("Falha durante execução");
                msgs << tr("Você pode tentar outra mídia, ou então preparar essa mídia previamente ao uso, descartando a opção formatar.");
                emit signalRunMessageBox(msgs);
                msgs.clear();
                return status;
            }
            if (!formatDisk.waitForReadyRead(WAITING_TIME)){
                QStringList msgs;
                msgs << tr("O processo finalizou inesperadamente no passo ")+QString::number(i)+tr(", ao aguardar para o próximo passo. Informe ao suporte.");
                emit log("[warning] " + msgs.at(0).toUtf8());
                msgs << tr("Processo finalizado com erro");
                msgs << tr("Você pode tentar outra mídia, ou então preparar essa mídia previamente ao uso, descartando a opção formatar.");
                emit signalRunMessageBox(msgs);
                msgs.clear();
                return -1;
            }
            t =  formatDisk.readAllStandardOutput();
            if (t.isEmpty()){
                QStringList msgs;
                msgs << tr("Houve falha na intercomunicação com o processo de controle do particionamento.");
                emit log("[warning] " + msgs.at(0).toUtf8());
                msgs << tr("Processo finalizado com erro");
                msgs << tr("Você pode tentar outra mídia, ou então preparar essa mídia previamente ao uso, descartando a opção formatar.");
                emit signalRunMessageBox(msgs);
                msgs.clear();
                return -1;
            }

        }
        emit signalSetProgressBarValNow(percent);
    }
    formatDisk.close();

    //O disco de perícia deve conter apenas 1 partição, exceto se opte por não, mas é incomum,
    //portanto agora é presumida sua formatação.
    int thereIsANewPartition = this->isTherePartition(dev);
    if (thereIsANewPartition != 0){
        return -1;
    }

    QProcess formatThisNewerPartition;
    QRegExp digits("(\\d)");
    QString newDevice = dev.remove(digits) + "1";
    if (!QFile::exists(newDevice)){
        QStringList msgs;
        msgs << tr("Por alguma razão o dispositivo não foi criado. Tente repetir o processo ou reinserir a mídia de destino.");
        emit log("[warning] " + msgs.at(0).toUtf8());
        msgs << tr("Dispositivo inexistente");
        msgs << tr("A primeira fase do processo consiste na recriação da tabela de partições com uma única partição do tipo escolhido. O erro ocorreu na segunda fase, que é validação do dispositivo, isto é, se ele foi criado pelo sistema operacional.");
        emit signalRunMessageBox(msgs);
        msgs.clear();
        return -1;
    }

    paramsToFormat << newDevice;

    formatThisNewerPartition.setProcessChannelMode(QProcess::MergedChannels);
    formatThisNewerPartition.start(formatCommand,paramsToFormat);

    QString line;
    QRegExp rxPercent("(\\d{1,}/\\d{1,})");

    if (formatCommand.contains(QRegExp("ext\\d"))){
        while (formatThisNewerPartition.waitForReadyRead(15000)){
            line = formatThisNewerPartition.readAllStandardOutput();
            rxPercent.indexIn(line);
            QString resultOfParsing = rxPercent.capturedTexts().last();
            if (resultOfParsing.contains(rxPercent) && !resultOfParsing.contains("0/239")){
                emit signalStatusLabel(resultOfParsing);
                percent = resultOfParsing.split("/").at(0).toInt()*100/resultOfParsing.split("/").at(1).toInt();
                emit signalSetProgressBarValNow(percent);
            }
            else{
                emit signalStatusLabel(tr("Um momento..."));
            }
        }
    }
    else if (formatCommand.contains(QRegExp("ntfs"))){
        while (formatThisNewerPartition.waitForReadyRead(15000)){
            line = formatThisNewerPartition.readAllStandardOutput();
            emit signalStatusLabel(line);
        }
    }

    else if (formatCommand.contains("xfs")){
        emit signalStatusLabel(tr("Aguardando retorno do programa..."));
        QStringList msgs;
        msgs << tr("Esse sistema de arquivos só devolverá informações ao final da operação, mas não se preocupe. Você será informado sobre qualquer anomalia");
        msgs << tr("Sistema de arquivos silencioso");
        msgs << " ";
        emit signalRunMessageBox(msgs);
        while (formatThisNewerPartition.waitForReadyRead(15000)){
            line = formatThisNewerPartition.readAllStandardOutput();
            emit signalStatusLabel(line);
        }
    }

    emit signalSetProgressBarValNow(100);

    formatThisNewerPartition.close();
    return 4;
}

int GPTandFormat::isTherePartition(QString device)
{
    //-1 - erro, 0 - ok, 1 - nao validado
    QProcess readPartition;
    QRegExp partValidate(".*\\s{,3}[1-128]\\s{,15}\\d\\s{,15}\\d\\s{,15}\\d\\.?\\d?\\s.*\\s{,15}\\d.*");
    QStringList params;
    params << "-l" << device;
    readPartition.start("gdisk",params);
    if (!readPartition.waitForFinished(WAITING_TIME)){
        QStringList msgs;
        msgs << tr("Ocorreu algum erro na tentativa de leitura das partições do dispositivo indicado.");
        emit log("[warning] " + msgs.at(0).toUtf8());
        msgs << tr("Erro na leitura do dispositivo");
        QString longMsg = tr("Esse erro não significa que a mídia é inválida, porém se utilizada, uma nova ");
        longMsg        += tr("tabela de partições será gerada sem validar previamente o disco. Se você considerar ");
        longMsg        += tr("isso um problema, experimente outra mídia de destino.");
        msgs << longMsg;
        emit signalRunMessageBox(msgs);
        msgs.clear();
        return -1;
    }

    QString t = readPartition.readAllStandardOutput();
    readPartition.close();
    partValidate.indexIn(t);
    QString result = partValidate.capturedTexts().last();
    if (result.contains(t)){
        return 0;
    }
    return 1;
}

QString GPTandFormat::prepareDevice(QString device){
    QRegExp rx("\\d{1,}");
    if (device.contains(rx)){
        device = device.remove(rx);
    }
    if (!device.contains("/dev/")){
        return "/dev/"+device;
    }
    else{
       return device;
    }
}

void GPTandFormat::run(){
    int result = this->formatThisDeviceUsingThisFS(this->dev,this->fs);
    if (result == 4){
        emit sucess();
    }
}
