#include "encrypt.h"

Encrypt::Encrypt(QObject *parent) :
    QThread(parent)
{

}

void Encrypt::AESCrypt(QString fileToEncryptWithFullPath)
{
    //QTimer::singleShot(3000, this, SLOT(close())); //TODO: CHAMAR O PROGRESSBAR DE CRIPTOGRAFIA ANTES DE INICIAR ESSA THREAD
    QProcess aescrypt;
    connect(&aescrypt,SIGNAL(finished(int)),this,SLOT(cryptIsFinished(int)));
    connect(this,SIGNAL(stopTask()),&aescrypt,SLOT(terminate()));
    QString comm = this->command.split("/").at(0) + " " + fileToEncryptWithFullPath;
    aescrypt.start(comm);
    if (aescrypt.pid() >0){
        emit log("[step] AESCrypt iniciado ");
        aescrypt.waitForFinished(-1);
    }
    aescrypt.close();
    emit log("[step] AESCrypt finalizado ");
    disconnect(&aescrypt,SIGNAL(finished(int)),this,SLOT(cryptIsFinished(int)));
    //disconnect(this,SIGNAL(stopTask(),&aescrypt,SLOT(terminate()));
    if (this->removeOriginal){
        emit log ("[step] (crypt) Removendo arquivo original como solicitado pelo usuario (radiobutton 'Remover Original': checked) ");
        QFile original(fileToEncryptWithFullPath);
        if (!original.remove()){
            emit log("[warning] (crypt) Não foi possível remover o arquivo original. ");
            QStringList msg;
            msg << tr("Não foi possível remover o arquivo original. Tente remover o arquivo correspondente manualmente.");
            emit signalMsgbox(msg);
            return;
        }
    }
}

void Encrypt::cryptIsFinished(int stat)
{
    QStringList m;
    if (stat == QProcess::CrashExit){
        m.clear();
        m << tr("Crypt - Ocorreu um erro durante o processo que o fez finalizar");
        emit signalMsgbox(m);
        emit log("[warning] (crypt) Ocorreu um erro durante o processo que o fez finalizar. ");
        return;
    }    
    emit log("[step] (crypt) Processo finalizado com sucesso sobre o arquivo "+this->file.toUtf8()+" ");

    if (this->counter < 1){
        for (int i=2;i<this->entries.length();i++){
            if (entries.at(i).contains(file.split(".").at(0))){
                counter++;
            }
        }
    }
    if (counter > waitCounter){
        waitCounter++;
    }
    else if(counter == waitCounter){
        emit signalMsgbox(tr("Crypt: Processo de criptografia finalizado sem erros aparentes - Final de Operação").split("-"));
        counter     = 0;
        waitCounter = 1;
    }
}

void Encrypt::fileSingleOrSplited()
{
    filename = this->command.split(" ").last(); //pega /media/images
    if (filename.length() < 2){
        emit log("[warning] (crypt) Nome de arquivo de 1 byte? ");
        return;
    }

    this->file        = filename.split("/").last(); //file.dd
    this->mountPoint  = filename.remove("/"+file);  // /media/images
    QDir directory(mountPoint);
    this->entries     = directory.entryList();
}

void Encrypt::getFreeSpace()
{
    QProcess df;
    df.start(QString("df"));
    if (!df.waitForFinished()){
        emit log("[warning] (crypt) Não foi possível determinar espaço da mídia de destino. Seguindo mesmo assim. ");
    }

    // /dev/sda1       36424612  1648492  34776120   5% /mnt
    // ex.: /media/images/file.dd
    if (this->command.isEmpty()){
        emit log("[warning] (crypt) Comando vazio (90) ");
        emit signalMsgbox(QString(" Erro causado por valor indeterminado em uma variável interna: 91En - Erro Interno").split("-"));
        return;
    }
    this->file       = this->command.split(" ").last().split("/").last(); //file.dd
    this->mountPoint = this->command.split(" ").last().remove("/"+file);  // /media/images

    if (mountPoint.isEmpty()){
        emit log("[warning] (crypt) Não foi possível determinar espaço livre na mídia. Seguindo mesmo assim. ");
    }

    QRegExp rx("/dev/sd\\w\\d{1,}\\s{1,}\\d{1,}\\s{1,}\\d{1,}\\s{1,}(\\d{1,})\\s{1,}\\d{1,}%\\s{1,}/"+mountPoint.split("/").at(1));
    QString result = df.readAllStandardOutput();
    rx.indexIn(result);
    result = rx.capturedTexts().last();
    QRegExp rxDigit("\\d");
    if (result.contains(rxDigit)){
        this->freeSpace = (result.toLong()/1000);
    }
    //o tamanho do arquivo deverá estar guardado, precedendo a chamada desse método com getFileSize()
    //Comparar se o espaço livre é '=> IMG +300KB'.
    df.close();
}

void Encrypt::OpenSSL(QString fileToEncryptWithFullPath)
{
    //openssl enc -aes-256-cbc -pass pass:teste -salt -in teste.E01 -out file.out
    //decript: openssl enc -d -aes-256-cbc -in file.out
    //o comando recebido conterá até a origem apenas. esse método incluirá a partir de -out
    QString cryptType = this->command.split(" ").at(2);
    cryptType = cryptType.remove(0,1);

    QProcess openSSL;
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    connect(&openSSL,SIGNAL(error(QProcess::ProcessError)),this,SLOT(slotCryptProcessStatusError(QProcess::ProcessError)));
    connect(&openSSL,SIGNAL(finished(int)),this,SLOT(cryptIsFinished(int)));
    connect(this,SIGNAL(stopTask()),&openSSL,SLOT(terminate()));

    //openssl enc -aes-256-cbc -pass pass:teste -salt -in + /mnt/TESTE/teste.E01    +   -out   +  /mnt/TESTE/teste.E01.aes-256-cbc
    QString comm = this->command.split("/").at(0) + " " + fileToEncryptWithFullPath + " -out " +  fileToEncryptWithFullPath + "." + cryptType;

    openSSL.start(comm);
    if (openSSL.pid() >0){
        emit log("[step] OpenSSL: Processo de criptografia iniciado ");
        openSSL.waitForFinished(-1);
    }
    openSSL.close();
    emit log("[step] OpenSSL: Processo de criptografia finalizado ");
    disconnect(&openSSL,SIGNAL(finished(int)),this,SLOT(cryptIsFinished(int)));
    if (this->removeOriginal){
        QFile original(fileToEncryptWithFullPath);
        if (!original.remove()){
            emit log("[warning] (crypt) Não foi possível remover o arquivo original. ");
            QStringList msg;
            msg << tr("Não foi possível remover o arquivo original. Tente remover o arquivo correspondente manualmente.");
            emit signalMsgbox(msg);
        }
    }
}

void Encrypt::removeFileAfterCrypt(bool yesno)
{
    this->removeOriginal = yesno;
}

void Encrypt::run()
{
    this->getFreeSpace();         // Em MB
    this->fileSingleOrSplited();  // QStringList com nomes de arquivos contidos no diretório, independente de nome ou extensão
    int stat = this->totalizer(); // calcula slice e total
    if (stat < 1){
        emit log("[warning] (crypt) Valor inesperado no totalizador ");
        return;
    }
    //espaço
    long int spaceNeeded = this->imgFirstSlice < this->imgTotalSize ? this->imgFirstSlice : this->imgTotalSize;
    if (this->freeSpace < spaceNeeded){
        emit log("[warning] Não há espaço suficiente para a imagem no destino ");
        emit signalMsgbox(tr("Não há espaço suficiente na mídia de destino para aplicar criptografia - Espaço no Destino").split("-"));
        return;
    }
    //criptografia
    if (this->command.contains("aescrypt")){
        emit log("[step] Iniciado AESCript ");
        for (int i=2;i<this->entries.length();i++){
            if (entries.at(i).contains(this->fileWithoutExtension)){
                this->AESCrypt(this->mountPoint+"/"+entries.at(i)); // O método se encarrega apenas de criptograr o que for passado.
            }
        }
    }
    else if (this->command.contains("openssl")){
        emit log("[step] Iniciado OpenSSL ");
        for (int i=0;i<this->entries.length();i++){
            if (entries.at(i).contains(this->fileWithoutExtension)){
                this->OpenSSL(this->mountPoint+"/"+entries.at(i)); // O método se encarrega apenas de criptograr o que for passado.
            }
        }
    }
}

void Encrypt::setCryptCommand(QString comm)
{
    QString logMsg = "[step] Atribuição de comando: " + comm;
    emit log(logMsg.toUtf8());
    this->command =  comm;
}

void Encrypt::slotCryptProcessStatusError(QProcess::ProcessError error)
{
    QString errorType;
    if (error == QProcess::FailedToStart){
        errorType = tr("Crypt: Erro tentando iniciar o processo - Encrypt");
    }
    else if (error == QProcess::Crashed){
        errorType = tr("Crypt: Processo destruido após ser iniciado - Encrypt");
    }
    else if (error == QProcess::Timedout){
        errorType = tr("Crypt: Tempo limite excedido para a operação - Encrypt");
    }
    else if (error == QProcess::WriteError){
        errorType = tr("Crypt: Erro de escrita (esgotamento de disco, falha da mídia, etc) - Encrypt");
    }
    else if (error == QProcess::ReadError){
        errorType = tr("Crypt: Ocorreu um erro durante a tentativa de leitura do processo - Encrypt");
    }
    else if (error == QProcess::UnknownError){
        errorType = tr("Crypt: Erro de origem desconhecida - Encrypt");
    }
    emit signalMsgbox(errorType.split("-"));
}

void Encrypt::slotStopCrypt()
{
    emit log("[user] Solicitada interrupção da criptografia ");
    emit stopTask();
}

int Encrypt::totalizer()
{
    //se retornar < 1, nao houve sucesso
    this->fileWithoutExtension = this->file.split(".").at(0);
    if (fileWithoutExtension.isEmpty()){
        emit log("[warning] (crypt) Não houve consistencia na identificação do nome de arquivo. ");
        return 0;
    }

    QFileInfo fileinfo(mountPoint+"/"+entries.at(2));
    this->imgFirstSlice = fileinfo.size()/1024;
    if (this->entries.length() > 3){
        for (int i=2;i<this->entries.length();i++){
            if (this->entries.at(i).contains(fileWithoutExtension)){
                QFileInfo fileinfo(this->mountPoint+"/"+this->entries.at(i));
                this->imgTotalSize += fileinfo.size()/1024;
            }
        }
    }
    else{
        this->imgTotalSize  = fileinfo.size();
    }
    return 1;
}
