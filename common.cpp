#include "common.h"
Common::Common(QObject *parent) :
    QObject(parent)
{
    QString programs = "md5sum,sha1sum,sha256sum,sha384sum,sha512,dcfldd,fdisk,parted,xmount";
    this->varsEnv.insert("imagesPlace","/media");
    this->varsEnv.insert("dependences",programs);
    this->timeoutTimer = 1000;
}

QStringList Common::connectedDevices(bool discOnly, QString exclude)
{
    QRegExp rx("sd.\\d?");
    if (discOnly){
    rx.setPattern("(sd\\w?)\n");
    }

    QProcess colectDisks;
    QStringList params;
    params << "/dev/";
    QByteArray results;
    colectDisks.start("ls",params);
    colectDisks.waitForFinished();
    results = colectDisks.readAll();
    colectDisks.close();

    int pos = 0;
    QStringList devices;
    devices << "Dev";
    while ((pos = rx.indexIn(results, pos)) != -1) {
         devices << rx.cap(0).replace("\n","");
         pos += rx.matchedLength();
     }

    for (pos = 0;pos<devices.length();pos++){
        if (devices.at(pos) == exclude){
           devices.removeAt(pos);
           break;
        }
    }
    if (exclude != "none"){
        QString valOfPostion;
        for (pos=0;pos<devices.length();pos++){
            valOfPostion = devices.at(pos);
            if (valOfPostion.contains(exclude)){
                devices.removeAt(pos);
                pos -= 1;
            }
        }
    }
    return devices;
}

void Common::figureOutAssociatedDevice(QString device)
{
    if (device.length() > 6){
        if (device.contains("/dev/")){
            device = device.remove("/dev/");
        }
    }
    if (!this->info["Modelo"].contains(tr("OCORRENCIAS"))){
        return;
    }
    emit log("[step] Buscando dispositivo por ID (provavelmente USB)");
    QProcess identifier;
    QStringList params;
    params << "-l" << "/dev/disk/by-id/";
    identifier.start("ls",params);
    if (!identifier.waitForFinished()){
        return; //nesse caso, deixe como está
    }
    QStringList usbTarget = QString(identifier.readAllStandardOutput()).split("\n");
    identifier.close();
    QString regex = "(.*/"+device+")";
    QRegExp rx(regex);
    for (int i=0;i<usbTarget.length();i++){
        rx.indexIn(usbTarget.at(i));
        QStringList result = rx.capturedTexts();
        if (!result.at(0).isEmpty()){
            rx.setPattern(".*(usb.*->|ata.*->)");
            rx.indexIn(result.at(1));
            result = rx.capturedTexts();
            QString everything =result.last();
            this->info.clear();
            this->info.insert("Modelo",everything);
            this->info.insert("Serial",everything);
            return;
        }
    }
}

QString Common::getEnv(QString varEnv)
{
    if (this->varsEnv.contains(varEnv)){
        return this->varsEnv[varEnv];
    }
    return "NULL";
}
QMap <QString, QString> Common::getHDInfo()
{
    return this->info;
}

void Common::hdparmExecutionAndResult(QString deviceToHDParm)
{

    QStringList hdparmParam;
    hdparmParam << "-i" << deviceToHDParm;

    QProcess hdparmComm;
    emit log("[step] hdparm: obtendo informação do dispositivo");
    hdparmComm.start("/sbin/hdparm",hdparmParam);
    if (!hdparmComm.waitForFinished(5000)){
        QStringList box;
        emit log("[step] Info: Dispositivo não identificado. Informando ao usuário para fazer nova tentativa");
        box << tr("Parece que o sistema operacional não fez a identificação correta do dispositivo. Tente novamente.") << "" << "";
        emit signalMsgBox(box);
        return;
    }
    QString firstLine = QString(hdparmComm.readAll());
    QRegExp rxHdparm("\\b\\Model=.*Config\\b");

    this->hdParmParser(firstLine.toUtf8());
    this->figureOutAssociatedDevice(deviceToHDParm);
}

void Common::hdParmParser(QByteArray hdp)
{
    QRegExp rx("\n");
    QString str = QString(hdp).remove(rx);
    QRegExp rx2("^/.+SerialNo=.+ Config=");
    int pos = 0;
    QStringList list;
    QString msg;

    while ((pos = rx2.indexIn(str, pos)) != -1) {
         list << rx2.cap(0);
         pos += rx2.matchedLength();
    }

    this->info.clear();
    if (!list.isEmpty()){
            if (list.at(0).contains(":")){
                info.insert(QString("Origem"),list.at(0).split(":").at(0));
            }
    }

    QRegExp rxModel("Model=.+,\\sFwRev");
    pos = 0;
    while ((pos = rxModel.indexIn(str, pos)) != -1) {
         msg = rxModel.cap(0).split("=").at(1).split(",").at(0);
         pos += rxModel.matchedLength();
     }
    if (!msg.isEmpty()){
        info.insert("Modelo",msg);
    }

    QRegExp rxSerial("SerialNo=.* ");
    pos = 0;
    QString spliting;
    if (!list.isEmpty()){
        while ((pos = rxSerial.indexIn(list[0], pos)) != -1) {
            spliting = rxSerial.cap(0);
            msg = spliting.split("=").at(1).left(spliting.split("=").at(1).length()-1);
            pos += rxSerial.matchedLength();
        }
    }
    if (!msg.isEmpty()){
        info.insert("Serial",msg);
    }

    if (info.isEmpty()){
        info.insert("Modelo",tr("SEM OCORRENCIAS"));
        info.insert("Serial",tr("DISPOSITIVO_DESCONHECIDO"));
    }
}

int Common::isTherePartition(QString device)
{
    //-1 - erro, 0 - ok, 1 - nao validado
    QProcess readPartition;
    QRegExp partValidate(".*\\s{,3}[1-128]\\s{,15}\\d\\s{,15}\\d\\s{,15}\\d\\.?\\d?\\s.*\\s{,15}\\d.*");
    QStringList params;
    params << "-l" << device;
    readPartition.start("gdisk",params);
    if (!readPartition.waitForFinished(5000)){
        params.clear();
        emit log("[warning] Erro na tentativa de leitura das partições do dispositivo indicado. Usuário informado via msgbox ");
        params << "Ocorreu algum erro na tentativa de leitura das partições do dispositivo indicado." << "Erro na leitura do dispositivo" << "Esse erro não significa que a mídia é inválida, porém se utilizada, uma nova\
                  tabela de partições será gerada sem validar previamente a mídia. Se você considerar\
                  isso um problema, experimente outra mídia de destino.";
        emit signalMsgBox(params);
        return -1;
    }

    QString t = readPartition.readAllStandardOutput();
    readPartition.close();
    if (partValidate.exactMatch(t)){
        return 0;
    }
    return 1;
}

QString Common::PartedAndFdisk(QString devOrFileTarget)
{
    QString partitions;
    QProcess gdiskCommand;
    QStringList gdiskParams;
    gdiskParams << devOrFileTarget << "-l";
    this->comm = "/sbin/gdisk";
    gdiskCommand.start(comm,gdiskParams);
    gdiskCommand.waitForFinished();
    this->allPartitionsFromDevice = gdiskCommand.readAll();
    gdiskCommand.close();
    partitions = QString(this->PartedParser(allPartitionsFromDevice));
    emit log("[step] retornando informações do gdisk");
    return partitions;
}

QString Common::PartedParser(QByteArray fdiskResults)
{
    if (this->comm.contains("fdisk")){
        return fdiskResults;
    }

    QString msg;
    int pos = 0;
    QRegExp rx("[0-9]{,2}:.*;");
    while ((pos = rx.indexIn(fdiskResults, pos)) != -1) {
         msg = rx.cap(0);
         pos += rx.matchedLength();
    }

    QStringList lines;
    lines << msg.split("\n");

    pos   = 0;

    QString strings;
    this->partitions.clear();
    QRegExp rxd("^\\d+:");
    for (int i=0;i<lines.length();i++){
        if (lines.at(i).contains(rxd)){
            this->partitions.insert(lines.at(i)[0],lines.at(i));
            strings += lines.at(i);
            strings.replace(';','\n');
        }
    }
    return strings+fdiskResults;
}

void Common::slotMakeHash()
{
    QProcess hash;
    QString hashcommand = this->varToHash.at(0).split("=").at(1)+"sum";
    hash.start(hashcommand,this->varToHash);
    hash.waitForFinished();
    //era qstring result local
    this->hashResult = hash.readAll();
    hash.close();

    QString textTitle = "Hash " + hashcommand + tr(" do arquivo:");
    QStringList msgs;
    msgs << this->hashResult.replace(" ","\n") << textTitle << "";
    emit signalMsgBox(msgs);
    emit log("[step] Retornando hash");
    return;
}

void Common::varToHashFeed(QStringList vth,bool makeFileWith)
{
    this->varToHash = vth;
    this->slotMakeHash();

    if (makeFileWith){
        QString rePath;
        rePath = vth.at(1).split("=").at(1);
        QFile file(rePath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outDataTo(&file);
        outDataTo << this->hashResult;
        file.close();
    }
}

void Common::writeOperation()
{
    emit log("[step] Configuração de horário");
    QDateTime now = QDateTime::currentDateTime();
}

Common::~Common()
{

}
