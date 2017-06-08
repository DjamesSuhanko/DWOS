#include "bitwisecopy.h"
#include <QDebug>
#define EXITCRASH 1
#define STRANGECODE 2
#define EXITOK 0
#define ABORTED 10

BitWiseCopy::BitWiseCopy(QObject *parent) :
    QThread(parent)
{
    timeout = retries = 0;
}

void BitWiseCopy::run()
{
    if (!this->fullCommand.isEmpty()){
        QString selectedTool = this->fullCommand.split(" ").at(0);

        if ( selectedTool == "dd"){
            startDDcopy();
            emit log("[user] Cópia iniciada com dd");
        }
        else if (selectedTool == "dcfldd"){
            startDCFLDDcopy();
            emit log("[user] Cópia iniciada com dcfldd");
        }
        else if (selectedTool == "dc3dd"){
            startDC3DDcopy();
            emit log("[user] Cópia iniciada com dc3dd");
        }
        //else if (selectedTool == "ddrescue"){

       // }
        this->fullCommand.clear();
    }
}

void BitWiseCopy::slotMountOrUmontAndCheckConditionsOf(QString device, const QString dirPath)
{
    QProcess mount;
    QStringList params;
    if (device.length() <5){
        device = "/dev/"+device;
    }

    QDir target;
    if (!target.exists(dirPath)){
        if (!target.mkdir(dirPath)){
            emit log("[user] Não foi possível criar o diretório");
            //implementar interface com usuario
        }
    }
    params <<  device << dirPath;
    mount.start("mount",params);
    if (!mount.waitForFinished(10000)){
        emit log("[warning] Falha na montagem do diretório de destino ");
        return;
    }
    mount.close();
}

void BitWiseCopy::slotIsFinished(int exitCode)
{
    QString lastMsg;
    if (exitCode != EXITCRASH && exitCode != STRANGECODE){
         lastMsg = tr("Processo finalizado com sucesso.");
         emit log("[step] Bitwise finalizado com sucesso ");
    }
    else{
         lastMsg = tr("Ocorreu alguma anomalia durante a operação.");
         emit log("[warning] Algum erro durante a cópia bitwise");
    }
    emit signalIsFinished(exitCode);
    emit signalStringStatusFromToolUsedInBitWiseCopy(lastMsg);
}

void BitWiseCopy::slotSetCommandToBitWiseCopy(QString command)
{
    this->fullCommand = command;
}

void BitWiseCopy::slotRetries(int t)
{
    this->retries = t;
}

void BitWiseCopy::slotTimeout(int t)
{
    this->timeout = t;
}
void BitWiseCopy::startDC3DDcopy()
{
    //dc3dd if=fdp of=lixo.dd hash=md5 log=teste.log
    QProcess dc3dd;
    dc3dd.setProcessChannelMode(QProcess::MergedChannels);
    dc3dd.start(QString(this->fullCommand));//,QProcess::Unbuffered|QProcess::ReadWrite);

    if(!dc3dd.waitForStarted()){
        //dc3dd_exit(4);
    }

    if (dc3dd.state() == QProcess::NotRunning){
        emit signalIsFinished(EXITCRASH);
        emit log("[warning] dc3dd não iniciado ");
        return;
    }

    //291733504 bytes (278,0 M) copied (14%), 20,6222 s, 13 M/s
    emit log("[step] dc3dd iniciado ");
    while (dc3dd.pid() > 0){
        while (dc3dd.waitForReadyRead(1000)){
            QString output = dc3dd.readAllStandardOutput();
            if (!output.contains("%")){
                break;
            }
            if (this->killBitWiseCopy){
                emit signalIsFinished(ABORTED);
                this->killBitWiseCopy = false;
                dc3dd.terminate();
                this->exit();
            }

            QRegExp rx;
            rx.setPattern("(\\d{1,}\\s\\w{1,}\\s\\(\\d{1,},?\\d{0,}\\s\\w\\)\\s\\w{1,}\\s\\(\\s{0,}\\d{1,}%\\),\\s\\d{1,},?\\d{1,}\\s\\w,\\s\\d{1,},?\\d{0,}\\s\\w/\\w)");
            int match = rx.indexIn(output);
            if (match >0){
                output = rx.capturedTexts().last();
                emit signalStringStatusFromToolUsedInBitWiseCopy(output);
            }
            else{
                rx.setPattern("\\d{1,}\\s\\w{1,}\\s\\(\\d{1,}\\s\\w\\)\\s\\w{1,}\\s\\(\\s{0,}\\d{1,}%\\)");
                match =rx.indexIn(output);
                if (match >0){
                    output = rx.capturedTexts().last();
                    emit signalStringStatusFromToolUsedInBitWiseCopy(output);
                }
            }
        }
    }

    if (dc3dd.exitStatus() == QProcess::CrashExit){
        emit log("[warning] dc3dd saiu com código de erro ");
        emit signalIsFinished(EXITCRASH);
        return;
    }
    emit signalIsFinished(EXITOK);
    dc3dd.close();
}
//TODO: na montagem, garantir que está montado e que é possível escrever para evitar falha de segmentação na escrita.
//TODO: implementar o tratamento do código de saída em todas as classes que utilizam qprocess.
void BitWiseCopy::startDCFLDDcopy()
{
    QProcess dcfldd;
    dcfldd.setProcessChannelMode(QProcess::MergedChannels);
    connect(&dcfldd,SIGNAL(finished(int)),this,SLOT(slotIsFinished(int)));
    dcfldd.start(QString(this->fullCommand));
    //dcfldd.start(QString("dcfldd if=/dev/zero of=/dev/null")); //usado para testes
    dcfldd.waitForStarted();
    while (dcfldd.pid() > 0){
        dcfldd.waitForReadyRead();
        if (dcfldd.isReadable()){
            QString output = dcfldd.readLine();
            if (output.contains("records")){
                break;
            }
            if (this->killBitWiseCopy){
                this->killBitWiseCopy = false;
                emit log("[user] Processo interrompido pelo usuário ");
                emit signalIsFinished(ABORTED);
                dcfldd.terminate();
                return;
            }
            emit signalStringStatusFromToolUsedInBitWiseCopy(output);
        }
    }
    if (dcfldd.exitStatus() == QProcess::CrashExit){
        emit log("[warning] dcfldd saiu com código de erro");
        emit signalIsFinished(EXITCRASH);
        return;
    }
    //disconnect(&dcfldd,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(slotIsFinished(int,QProcess::ExitStatus)));
    dcfldd.close();
    emit signalIsFinished(EXITOK);
    emit log("[step] dcfldd: cópia bitwise finalizada com sucesso ");
}

void BitWiseCopy::startDDcopy()
{
    QString command = this->fullCommand;
    if (command.isEmpty()){
        emit log("[warning] parâmetros vazios para dd ");
        return;
    }
    QProcess dd;
    QString pid;
    QString result;

    dd.start(QString(command));

    pid = QString::number(dd.pid());

    QRegExp rxWords("\\d");
    if (!pid.contains(rxWords) || pid.length() < 2){
        emit log("[warning] processo dd não iniciado ");
    }

    QProcess pushCommand;
    QString comm = "kill -USR1 " + pid;

    while ( dd.pid() > 0 && !dd.waitForReadyRead(100)){
        if (this->killBitWiseCopy){
            this->killBitWiseCopy = false;
            emit signalIsFinished(ABORTED);
            dd.terminate();
            this->exit();
            return;
        }
        pushCommand.start(comm);
        dd.waitForReadyRead(100);
        QString output = dd.readAllStandardError();
        pushCommand.close();

        QRegExp hotLine("\\n\\d{1,}\\s\\w{1,}\\s\\(\\d{1,},?\\d{0,}\\s\\w{1,}\\)\\s\\w{1,},\\s\\d{1,},?\\d{0,}\\ss,\\s\\d{1,},?\\d{0,}\\s\\w{1,}/s");
        int status = hotLine.indexIn(output);
        if (status > -1){
            QString result = hotLine.capturedTexts().last().remove("\n");
            //qDebug() << result;
           emit signalStringStatusFromToolUsedInBitWiseCopy(result);
        }
    }

    if (dd.exitStatus() == QProcess::CrashExit){
        emit log("[warning] saida com erro da thead dd ");
        emit signalIsFinished(EXITCRASH);
        return;
    }

    emit signalIsFinished(EXITOK);
    dd.close();
}

//void BitWiseCopy::startDDRESCUEcopy()
//{

//}

void BitWiseCopy::slotStopBitWiseCopyNow()
{
    this->killBitWiseCopy = true;
}
