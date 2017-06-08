#include "fingerprintandtraces.h"
#include <QDebug>
FingerPrintAndTraces::FingerPrintAndTraces(QObject *parent) :
    QThread(parent)
{
}

QByteArray FingerPrintAndTraces::makeHash(QString hashtype, QString targetToHashing)
{
    QFile file(targetToHashing);
    if (!file.exists()){
        log("[warning] Arquivo indicado para aplicar digest não existe. Operação cancelada. ");
    }

    QMap <QString, QCryptographicHash::Algorithm> hashes;

    hashes.insert("Md4",QCryptographicHash::Md4);
    hashes.insert("Md5",QCryptographicHash::Md5);
    hashes.insert("Sha1",QCryptographicHash::Sha1);
    hashes.insert("Sha224",QCryptographicHash::Sha224);
    hashes.insert("Sha256",QCryptographicHash::Sha256);
    hashes.insert("Sha384",QCryptographicHash::Sha384);
    hashes.insert("Sha512",QCryptographicHash::Sha512);
    hashes.insert("Sha3_224",QCryptographicHash::Sha3_224);
    hashes.insert("Sha3_256",QCryptographicHash::Sha3_256);
    hashes.insert("Sha3_384",QCryptographicHash::Sha3_384);
    hashes.insert("Sha3_512",QCryptographicHash::Sha3_512);

    if (!hashes.keys().contains(hashtype)){
        log("[warning] digest escolhido é invalido. Usando sha1. ");
        hashtype = "Sha1";
    }

    QCryptographicHash crypto(hashes[hashtype]);

    file.open(QFile::ReadOnly);
    while(!file.atEnd()){
      crypto.addData(file.read(8192));
    }
    QByteArray hash = crypto.result();
    hash = hash.toHex();
    return hash;
}

void FingerPrintAndTraces::isHashPossible()
{
    QStringList msg;
    //não é pra gerar hash
    /*
     * "sha1sum /media/images/UNKNOWN_DEVICE.dd"
     */

    QString saveFileOnThisPlace = this->hashToFile;
    this->hashToFile.clear();

    //2 - SÓ EXECUTAR SE O ARQUIVO FOR .DD
    //3 - PBARUNDEF SERIA BOM NESSE MOMENTO

    //1 - LOOP COM O NOME DO ARQUIVO SEM A EXTENSAO
    QString filename = saveFileOnThisPlace.split("/").last();
    QString dirname  = saveFileOnThisPlace.remove(filename);
    QDir directory(dirname);
    QStringList entries = directory.entryList();
    QString filenameWithoutExtension = filename.remove(".dd");
    filenameWithoutExtension.remove(".000");
    filenameWithoutExtension.remove(".aa");
    filenameWithoutExtension.remove(QRegExp("\\.sha\\d{1,}",Qt::CaseInsensitive));
    filenameWithoutExtension.remove(QRegExp("\\.md\\d"));
    QString hashes = "";
    int written = -1;

    for (int i=2;i<entries.length();i++){
        if (entries.at(i).contains(filenameWithoutExtension) && (!entries.at(i).contains("Sha")||!entries.at(i).contains("Md5"))){
            QByteArray result = makeHash(this->algorithym,dirname+entries.at(i));
            if (result.length() > 5){
                emit log("[step] hash do arquivo "+entries.at(i).toUtf8()+":"+result);
                QString filenameAndPath = dirname + filename;
                if (!filenameAndPath.isEmpty()){
                    QFile file(filenameAndPath);
                    if (file.exists()){
                        if (this->writeHashInFile && !file.open(QIODevice::Text|QIODevice::Append)){
                            emit log("Não foi possível gravar o hash em arquivo (openning). ");
                            return;
                        }
                    }
                    else{
                        if (this->writeHashInFile && !file.open(QIODevice::Text|QIODevice::WriteOnly)){
                            emit log("Não foi possível gravar o hash em arquivo (openning). ");
                            return;
                        }
                    }
                    QByteArray infoHash = result + " " + entries.at(i).toUtf8() + "\n";
                    if (this->writeHashInFile){
                        written = file.write(infoHash);
                    }
                    hashes += infoHash;
                    if (written < 0 && this->writeHashInFile){
                        msg.clear();
                        msg << tr("Não foi possível gravar o hash em arquivo (written).") << "" << "";
                        //emit signalMsgBox(msg);
                        emit log("Não foi possível gravar o hash em arquivo (written). ");
                    }
                    if (this->writeHashInFile){
                        file.close(); //gravar em arquivo finalizado, com ou sem sucesso
                    }
                }
            }
            else{
                msg.clear();
                msg << tr("Não foi possível ler o resultado do cálculo hash. ") << "" << "";
                //emit signalMsgBox(msg);
                emit log("Não foi possível ler o resultado do cálculo hash. ");
            }
            emit log("Hash finalizado: ");
        }
    }
    msg.clear();
    msg << hashes << "Hashes";
    emit signalMsgBox(msg);
}

void FingerPrintAndTraces::run()
{
    QStringList msg;
    emit log("Cálculo do hash iniciado:");
    msg << tr("Cálculo do hash iniciado. Por favor, aguarde, o hash será exibido ao final desta operação.") << "" << "";
    emit signalMsgBox(msg);
    this->isHashPossible();
}

void FingerPrintAndTraces::setHashType(QString hashSelected, QString fileTarget, bool writeInAfile)
{
    this->algorithym = hashSelected;
    this->hashToFile.clear();
    // /path/file.sha1 por exemplo
    this->hashToFile = fileTarget;
    this->writeHashInFile = writeInAfile;
}

