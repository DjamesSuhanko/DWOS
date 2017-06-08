#include "expertwitnessformat.h"

ExpertWitnessFormat::ExpertWitnessFormat(QObject *parent) :
    QThread(parent)
{
}
/* 1 - MONTAR IMAGEM DE INICIO COM ESSAS INFORMAÇÕES
<b>Informações do dispositivo</b>
Barramento: USB
Fabricante: -
Modelo    : USB Flash Memory
Serial    : -

<b>Informações de Armazenamento</b>
Tipo             : Dispositivo
Tipo de mídia    : Removível
Tamanho da mídia : 4.1 GB (4126146560 bytes)
Bytes por setor  : 512

<b>Início da aquisição de dados:</b> Sat Mar 14 14:22:06 2015
*/

void ExpertWitnessFormat::alternateMessageToSend(QString zero, QString one)
{
    mainMessage[0] = zero;
    mainMessage[1] = one;
    this->alternate = this->alternate > 0 ? 0 : 1;
    emit signalMsgStatus(mainMessage[this->alternate]);
}

void ExpertWitnessFormat::run()
{
    this->stopNow = false;
    QProcess encase;
    QFile ewfLog("/var/log/ewf/ewf.log");

    connect(&encase,SIGNAL(finished(int)),this,SLOT(slotFinished(int)));

    //QString command = "ewfacquire /dev/sdb -t DESTINO -b ReadAtOnce -B BytesToAcquire -c CompressionMethod -C CaseNumber -d HashType -D Description -e ExaminerName -E EvidenceNumber -f UseEWFfileFormat -g Granularity -l FileNameHash -m MediaType -M MediaCharacteristics -N Notes -o StartAtOffset -P BytesPerSector -r RetriesOnError -S EvidenceSegment -w -u";
    //this->params = "/dev/sdb -t /mnt/LIXO -b 64 -B 4126146560 -c deflate:none -C 01 -d sha1 -D 'Description_aaa' -e 'Djames_Suhanko' -E 001 -f encase6 -g 64 -l FileNameHash -m removable -M logical -N 'Notes_aaa' -o 0 -P 512 -r 2 -S 1048576000 -u";
    if (this->params.isEmpty()){
        //TODO: logar
        return;
    }
    this->params.remove(".dd");
    encase.start("ewfacquire",this->params.split(" "));
    int pid = encase.pid();
    int stat;
    QString result;
    QString msgSizeCopiedOfTotal;
    QString elepsedTimeOnArrival;
    QString header  = " ";
    QString footer  = " ";
    bool readHeader = true;

    mainMessage.insert(0,"Starting...");
    mainMessage.insert(1,"Starting...");

    QRegExp rx("Written:.*SHA1 hash calculated over data:\\s{1,}\\d{0,}\\w{0,}");
    QRegExp rxMsg1("Status:\\s{1,}at\\s{1,}(\\d{1,})%.*");

    //acquired 74 MiB (78053376 bytes) of total 7.2 GiB (7803174912 bytes).
    QRegExp rxMsg2("acquired\\s\\d{1,}\\.?\\d{0,}\\s\\w{1,}\\s\\(\\d{1,}\\s\\w{1,}\\)\\sof\\stotal\\s\\d{1,}\\.?\\d{0,}\\s\\w{1,}");

    //completion in 4 minute(s) and 54 second(s) with 13 MiB/s (13753821 bytes/second).
    QRegExp rxMsg3("completion in \\d{0,} \\w{0,}\\(?s?\\)? \\w{0,} \\d{1,} second\\(s\\) with \\d{1,} \\w{1,}/s");

    while (pid >0){
        if (stopNow){
            encase.terminate();
            break;
        }
        encase.waitForReadyRead(-1);
        //result = QString(encase.readLine()).remove("\n");
        result = encase.readAllStandardOutput();
        if (readHeader){
            header += result+"\n";
            if (result.contains("Acquiry started at:")){
                readHeader = false;
                if (!ewfLog.open(QIODevice::Text|QIODevice::WriteOnly)){
                    emit log("[warning] Não foi possível abrir /var/log/ewf/ewf.log para gravar cabeçalho. ");
                }
                else{
                    ewfLog.write(header.toUtf8());
                    ewfLog.close();
                }
            }
        }//header

        //middle - tratar a mensagem e fazer os emits
        //msg 1: porcentagem
        stat = rxMsg1.indexIn(result);
        if (stat > -1){
            int percent = rxMsg1.capturedTexts().last().toInt();
            if (!stopNow){
                emit signalProgressBar(percent);
            }
        }

        //msg 2:
        stat = rxMsg2.indexIn(result);
        if (stat > -1){
            msgSizeCopiedOfTotal = rxMsg2.capturedTexts().last();
        }

        //msg 3:
        stat = rxMsg3.indexIn(result);
        if (stat > -1){
            elepsedTimeOnArrival = rxMsg3.capturedTexts().last();
        }

        if (result.contains(rx)){
            footer = result;
            if (!ewfLog.open(QIODevice::Text|QIODevice::Append)){
                emit log("[warning] Não foi possível abrir /var/log/ewf/ewf.log para gravar rodapé. ");
            }
            else{
                ewfLog.write(footer.toUtf8());
                ewfLog.close();
            }
        }//footer
        if (!stopNow){
            alternateMessageToSend(msgSizeCopiedOfTotal,elepsedTimeOnArrival);
        }
        pid = encase.pid();

    }//while

    disconnect(&encase,SIGNAL(finished(int)),this,SLOT(slotFinished(int)));
    encase.close();
    this->params.clear();
}

void ExpertWitnessFormat::setParamsToEWFcopy(QString str)
{
    this->params = str;
}

void ExpertWitnessFormat::slotStopEWFprocess()
{
    this->stopNow = true;
}

void ExpertWitnessFormat::slotFinished(int status)
{
    QStringList m;
    if (status == QProcess::CrashExit){
        m.clear();
        m << tr("EWF - Ocorreu um erro durante o processo que o fez finalizar");
        emit signalMsgbox(m);
        emit log("[warning] Ocorreu um erro durante o processo que o fez finalizar. ");
        return;
    }
    m.clear();
    m << tr("Processo finalizado com sucesso.") << tr("Fim do processo");
    emit signalMsgbox(m);
    emit log("[step] Processo finalizado com sucesso. ");
}
