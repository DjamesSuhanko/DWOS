#include "mainwindow.h"
#include "ui_mainwindow.h"

#define IMGTAB 1
#define TABMON 2
#define IMGTBOX 2
#define STRTAB 0
#define TBOXLOG 3
#define TBOXSTR 0
#define EXITCRASH 1
#define STOPPED -2
#define EXITOK 0
#define ABORTED 10
#define WRITE 2
#define READ 1

/*
objdump -d binario - encontra os métodos e seus nomes

The Defense Cyber Crime Center (DC3) is an United States Department of Defense agency that provides digital forensics support
to the DoD and to other law enforcement agencies. DC3's main focus is in criminal, counterintelligence, counterterrorism, and
fraud investigations from the Defense Criminal Investigative Organizations (DCIOs), DoD counterintelligence groups, and various
Inspector General groups. The Air Force Office of Special Investigations is the executive agent of DC3.[1]
http://en.wikipedia.org/wiki/Department_of_Defense_Cyber_Crime_Center

---------------------------------
 /home/djames/Qt/5.3/gcc_64/bin/lupdate -pro DjamesWatson.pro
o lrelease é feito pelo linguist, nao precisa se preocupar com isso
*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->PID = QString::number(QCoreApplication::applicationPid());
    QFile pid("/var/log/DWatson/"+this->PID);
    if (pid.exists()){
        this->close();
    }
    pid.open(QIODevice::Text|QIODevice::WriteOnly);
    pid.write(dateTimeNowString().toUtf8()+"\n");
    pid.close();

    //QTimer::singleShot(1000, this, SLOT(showMaximized()));
    //this->showMaximized();

    //QFontDatabase::addApplicationFont("/usr/share/fonts/truetype/droid/DroidSans.ttf");
    //QFontDatabase::addApplicationFont("/usr/share/fonts/truetype/droid/DroidSans-Bold.ttf");

    this->common         = new Common();
    this->wiper          = new Wiping();
    this->gpt            = new GPTandFormat(0,this->common);
    this->sysInfo        = new SystemInfo();
    this->bitwise        = new BitWiseCopy();
    this->graph          = new GraphRealTime();
    this->fingerprint    = new FingerPrintAndTraces();
    this->doc            = new documentation();
    this->screenshot     = new Screenshot(this);
    this->screencast     = new Screencast();
    this->pbarUndefVal   = new ProgressBarUndefinedValue();
    this->autoRenameShot = new AutoRenameShot();
    this->ewf            = new ewfRelat();
    this->ewfAcquiring   = new ExpertWitnessFormat();
    this->crypt          = new Encrypt();
    this->iostat         = new IOStats();

    //image viewer
    this->imageLabel1    = new QLabel;
    this->imageLabel2    = new QLabel;
    this->imageLabel3    = new QLabel;
    this->imageLabel4    = new QLabel;

    this->showLogInDetailWindowIfStringsOff = false;

    //valores carregados pelo qsettings
    this->Settings();

    QDateTime now = QDateTime::currentDateTime();
    ui->dateTimeEdit->setDateTime(now);

    //Componentes da UI
    ui->progressBar->setValue(0);
    ui->progressBar->setDisabled(true);
    ui->radio_md5->setEnabled(false);
    ui->radio_sha->setEnabled(false);
    ui->radioButton_bw_src2file->setChecked(true);
    ui->radioButton_show_dst_data->setEnabled(false);
    ui->radioButton_show_src_data->setEnabled(false);
    ui->comboBox_hash->setEnabled(false);
    ui->comboBox_Target_bitwise->setEnabled(false);
    ui->comboBox_target_format->setEnabled(false);
    ui->comboBox_bitwiseTools->setEnabled(false);
    ui->checkBoxHashEmArquivo->setEnabled(false);
    ui->checkBox_hash->setEnabled(false);
    ui->checkBox_gerarLog->setEnabled(false);
    ui->checkBox_readOnly_bw->setChecked(true);
    ui->checkBox_wipe->setDisabled(true);
    ui->pushButton_colect->setDisabled(true);
    ui->pushButton_selecionar->setDisabled(true);
    ui->pushButton_show_data->setDisabled(true);
    ui->pushButton_datetime->setDisabled(true);
    //ui->groupBox_ddrescue->setDisabled(true);
    ui->frame_split->setEnabled(false);
    ui->groupBox_imgInfo->setDisabled(true);

    //Textos da Interface
    ui->label_image->setStyleSheet("color: gray;");
    ui->label_image->setText(tr("(Clique no botão ao lado para selecionar imagem)"));

    //ToolBox text
    ui->toolBox->setItemText(0,tr("..."));
    ui->toolBox->setItemText(1,"HDDs/SSDs");
    ui->toolBox->setItemText(2,tr("Geração de Imagem"));
    ui->toolBox->setItemText(3,tr("Log"));
    ui->toolBox->setItemText(4,tr("Ajustes"));

    //divisao de imagens raw
    ui->checkBox_split->setText(tr("Dividir a cada"));

    //toolbox, aba log log...
    ui->radioButton_off->setChecked(true);

    //TabWidget
    ui->tabWidget->setTabText(3,tr("Documentação"));

    this->enableDisableItems(false);
    ui->toolBox->setItemEnabled(0,false);

    //Combo SHA
    QStringList shaType;
    shaType << "Sha1" << "Sha224" << "Sha256" << "Sha384" << "Sha512"; //TODO: << "Sha3_224" << "Sha3_256" << "Sha3_384" << "Sha3_512";
    ui->comboBox_hash->addItems(shaType);

    //Titulo da mainWindow
    this->setWindowTitle(tr("Djames Watson 1.0 - Cadeia de Custódia - 04.2015"));

    ui->progressBar->setMaximum(100);

    ui->radio_sha->setChecked(true);
    QStringList feed = this->common->connectedDevices(false,"none");
    if (feed.length() > 0){
        feed.removeAt(0);
        ui->listWidget_medias->addItems(feed);
    }

    this->connections();

    //!Essa thread roda constantemente, coletando variáveis de ambiente para exibição na aba Monitor.
    QString val = ui->label_executing_tasks->text()+"Monitor ";
    ui->label_executing_tasks->setText(val);
    this->sysInfo->start();

    //Atalhos de teclado como esse deverão ser implementados em um método para tentar despoluir o construtor dessa classe
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this,SLOT(screenShort()));

    //Monitora as threads em execução
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkTasks()));
    timer->start(5000);

    QString d;
    QString t;
    QString fullVal = dateTimeNowString();
    QRegExp rxDT("(\\d{2}:\\d{2}:\\d{2})");
    rxDT.indexIn(fullVal);
    t = rxDT.capturedTexts().last();
    d = fullVal.remove(rxDT);
    ui->label_date->setText(d);
    ui->label_hour->setText(t);
    ui->textEdit->setText(tr("Olá. Lembrarei sempre que para iniciar as operações, você deve selecionar um idioma, confirmar ou ajustar data e hora e confirmar em 'Ok'."));
    //ui->textEdit->setStyleSheet("text-align:justify; background-color:transparent;");
    ui->textEdit->setAlignment(Qt::AlignJustify);

    QStringList docMenu;
    docMenu << tr("0.0 - Introdução") << tr("1.0 - Liberando a Interface") << tr("2.0 - Apresentação da Interface");
    docMenu << tr("2.1 - Responsividade interativa") << tr("2.2 - Auto limpeza da interface") << tr("3.0 - Componentes da Interface");
    docMenu << tr("3.1 - Componentes de rodapé") << tr("3.2 - Monitor") << tr("4.0 - ScreenShot") << tr("5.0 - ScreenCast") << tr("6.0 - Identificação dos dispositivos");
    docMenu << tr("7.0 - Dúvidas e Suporte") << tr("8.0 - Licença, direitos e fontes");

    ui->comboBox_topics->addItems(docMenu);

    //openssl enc types
    QStringList encTypes;
    encTypes<< "aes-256-cbc"      << "aes-256-ecb"      << "base64"            <<    "bf";
    encTypes<< "bf-cbc"           << "bf-cfb"           << "bf-ecb"            <<    "bf-ofb";
    encTypes<< "camellia-128-cbc" << "camellia-128-ecb" << "camellia-192-cbc"  <<    "camellia-192-ecb";
    encTypes<< "camellia-256-cbc" << "camellia-256-ecb" << "cast"              <<    "cast-cbc";
    encTypes<< "cast5-cbc"        << "cast5-cfb"        << "cast5-ecb"         <<    "cast5-ofb";
    encTypes<< "des"              << "des-cbc"          << "des-cfb"           <<    "des-ecb";
    encTypes<< "des-ede"          << "des-ede-cbc"      << "des-ede-cfb"       <<    "des-ede-ofb";
    encTypes<< "des-ede3"         << "des-ede3-cbc"     << "des-ede3-cfb"      <<    "des-ede3-ofb";
    encTypes<< "des-ofb"          << "des3"             << "desx"              <<    "rc2";
    encTypes<< "rc2-40-cbc"       << "rc2-64-cbc"       << "rc2-cbc"           <<    "rc2-cfb";
    encTypes<< "rc2-ecb"          << "rc2-ofb"          << "rc4"               <<    "rc4-40";
    encTypes<< "seed"             << "seed-cbc"         << "seed-cfb"          <<    "seed-ecb";
    encTypes<< "seed-ofb";

    ui->comboBox_openSSL_type->addItems(encTypes);

    //ocultos
    ui->frame_relatorio->setVisible(false);
    ui->label_description->setVisible(false);
    ui->label_notes->setVisible(false);

    this->shotsViewer();
}

int MainWindow::bitwiseValidator()
{
    //-1 = inconsistencia na operacao, 0 = ok, 1 = cancelado pelo usuário
    QMessageBox msg;
    msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    //O target só é alimentado e habilitado se a origem já tiver sido escolhida,
    //então são necessárias as duas condicionais separadas
    if (ui->comboBox_Source_bitwise->currentText().contains("Dev")){
        msg.setWindowTitle(tr("Origem não selecionada"));
        msg.setText(tr("Você deve escolher origem e destino da cópia primeiro."));
        msg.exec();
        log(tr("[user] Origem não selecionada:").toUtf8());
        return -1;
    }
    if (ui->comboBox_Target_bitwise->currentText().contains("Dev")){
        msg.setWindowTitle(tr("Destino não selecionado"));
        msg.setText(tr("Você deve escolher origem e destino da cópia primeiro."));
        msg.exec();
        log(tr("[user] Destino não escolhido:").toUtf8());
        return -1;
    }
    //O código já protege a seleção de origem no destino, mas cuidado nunca é demais
    //(caso algum dia a validação sofra alguma alteração, por exemplo)
    if (ui->comboBox_Source_bitwise->currentText() == ui->comboBox_Target_bitwise->currentText() && !ui->comboBox_Source_bitwise->currentText().contains("Dev")){
        msg.setWindowTitle(tr("Origem e destino iguais"));
        msg.setText(tr("O destino não deve ser igual a origem. Verifique também se a seleção está correta."));
        msg.exec();
        log(tr("[warning] Erro grave: Src e Dst iguais").toUtf8());
        return -1;
    }
    //ferramenta a utilizar na copia bitwise
    if (ui->comboBox_bitwiseTools->currentIndex() == 0){
        msg.setWindowTitle(tr("Escolha ferramenta de cópia"));
        msg.setText(tr("Você não escolheu uma ferramenta para iniciar a cópia."));
        msg.exec();
        log(tr("[user] Ferramenta de cópia não selecionada:").toUtf8());
        return -1;
    }
    return 0;
}

//TODO: pbar da origem e destino: testar no live pra ver se funciona
void MainWindow::monitorSettings(QString srcOrDst)
{
    QProcess diskSize;
    QStringList params;
    QString dev,devRaw;
    QString result;
    //Disk /dev/sdd: 7803 MB, 7803174912 bytes
    QRegExp  rx("Disk\\s{1,}/dev/sd\\w:\\s{1,}(\\d{1,}\\.?\\d{0,}\\s\\w{1,}),\\s{1,}\\d{1,}\\s{1,}\\w{1,}");
    QRegExp rxDigit("\\d");
    if (srcOrDst == "src"){
        ui->label_serial_monitor_src->setText(ui->pushButton_serial_info->text());
        //isso já é garantido fora, mas se houver uma mudança no código, isso pode passar despercebido, então, melhor proteger
        if (ui->comboBox_Source_bitwise->currentIndex() >0 && !ui->comboBox_Source_bitwise->currentText().isEmpty()){
            dev = "/dev/"+ui->comboBox_Source_bitwise->currentText();
            devRaw = dev.remove(rxDigit);
            params << "-l" << devRaw;
            diskSize.start("fdisk",params);
            if (diskSize.waitForFinished()){
                result = diskSize.readAllStandardOutput();
                diskSize.close();
            }
            else{
                log(tr("[warning] monitor settings src: processo não pronto para leitura (disk size) - ").toUtf8());
                return;
            }

            rx.indexIn(result);
            QString srcSize = rx.capturedTexts().last();
            ui->label_pbar_mon_src->setText(srcSize);
            ui->label_size_src->setText(srcSize);

        }
        return;
    }
    ui->label_serial_monitor_dst->setText(ui->pushButton_serial2_info->text());
    if (ui->comboBox_Target_bitwise->currentIndex() >0 && !ui->comboBox_Target_bitwise->currentText().isEmpty()){
        dev = "/dev/"+ui->comboBox_Target_bitwise->currentText();
        devRaw = dev.remove(rxDigit);
        params << "-l" << devRaw;
        diskSize.start("fdisk",params);
        if (diskSize.waitForFinished()){
            result = diskSize.readAllStandardOutput();
            diskSize.close();
        }
        else{
            log(tr("[warning] monitor settings dst: processo não pronto para leitura - ").toUtf8());
            return;
        }

        rx.indexIn(result);
        QString dstSize = rx.capturedTexts().last();
        ui->label_pbar_mon_dst->setText(dstSize);
        ui->label_size_dst->setText(dstSize);

        QRegExp rxDigit("\\d");
        if (!ui->label_size_src->text().contains(rxDigit) || !ui->label_size_dst->text().contains(rxDigit)){
            return;
        }
        float src,dst;
        src = ui->label_size_src->text().split(" ").at(0).toFloat();
        dst = ui->label_size_dst->text().split(" ").at(0).toFloat();
        if (ui->label_size_src->text().contains("TB")){
            src = (src*1000)*1000;
        }
        else if (ui->label_size_src->text().contains("GB")){
            src = src*1000;
        }

        if (ui->label_size_dst->text().contains("TB",Qt::CaseInsensitive)){
            dst = (dst*1000)*1000;
        }
        else if (ui->label_size_dst->text().contains("GB",Qt::CaseInsensitive)){
            dst = dst*1000;
        }

        if (src > dst){
            ui->comboBox_bitwiseTools->setDisabled(true);
            QMessageBox msgSize;
            msgSize.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msgSize.setWindowTitle(tr("Destino menor que Origem"));
            msgSize.setText(tr("A mídia de Destino é menor que a mídia de Origem. Não será possível gerar a imagem."));
            msgSize.exec();
            log(tr("[user] Midia de destino menor que disco de origem: ").toUtf8());
            return;
        }
        if (src == dst && !ui->radioButton_bw_disc2disc->isChecked()){
            ui->comboBox_bitwiseTools->setDisabled(true);
            QMessageBox msgSize;
            msgSize.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msgSize.setWindowTitle(tr("Destino igual Origem"));
            msgSize.setText(tr("A mídia de Destino é do mesmo tamanho da Origem. Somente Origem a Destino é possível."));
            msgSize.setDetailedText(tr("No menu Geração de Imagem a opção Origem a Destino deve ser marcada. Ainda assim\
                                    há um pequeno mas possível risco de não-alocação dos dados."));
            msgSize.exec();
            log(tr("[user] Midias do mesmo tamanho: ").toUtf8());
            return;
        }
    }
}

QString MainWindow::mountCommandToEWFthread(QString fileNameSelected)
{
    //ewfacquire /dev/sdb -t DESTINO             -b ReadAtOnce -B BytesToAcquire -c CompressionMethod -C CaseNumber -d HashType -D Description       -e ExaminerName     -E EvidenceNumber -f UseEWFfileFormat -g Granularity -l FileNameHash -m MediaType -M MediaCharacteristics -N Notes       -o StartAtOffset -P BytesPerSector -r RetriesOnError -S EvidenceSegment (-w) -u
    //           src         fullPathAndFileName -b 64         -B 4126146560     -c deflate:none      -C 01         -d sha1     -D 'Description_aaa' -e 'Djames_Suhanko' -E 001            -f encase6          -g 64          -l FileNameHash -m removable -M logical              -N 'Notes_aaa' -o 0             -P 512            -r 2              -S 1048576000           -u
    QString fileName = fileNameSelected.replace(" ","_");

    QString command;
    QString hash;
    command  = "/dev/"+ui->comboBox_Source_bitwise->currentText() + " ";
    command += "-t "+this->common->TARGET_DIR+"/"+fileName+" ";
    command += "-b "+ui->label_readOnce->text().split(" ").at(0)+" ";
    command += "-B "+ui->lineEdit_limit->text()+" ";
    command += "-c "+ui->comboBox_compressMethod->currentText()+":"+ui->comboBox_compressLevel->currentText()+" ";
    QString caseNumber = ui->label_caseNumberInfo->text();
    if (!caseNumber.contains(QRegExp("\\d"))){
        caseNumber = "00";
    }
    command += "-C "+caseNumber+" ";
    if (ui->checkBox_hash->isChecked()){
        if (ui->radio_sha->isChecked()){
            if (!ui->comboBox_hash->currentText().contains("sha1") && !ui->comboBox_hash->currentText().contains("sha256")){
                hash = "sha256";
            }
            else{
                hash = ui->comboBox_hash->currentText();
            }
            command += "-d "+hash+" ";
        }
    }
    QString description = ui->label_description->text();
    if (description.isEmpty() || description.length() < 2){
        description = tr("Nulo");
    }
    else{
        description = description.replace(" ","_");
    }
    command += "-D "+description+" ";
    QString examiner;
    if (ui->label_examinerInfo->text().isEmpty()){
        examiner = tr("Não_Informado");
    }
    else{
        examiner = ui->label_examinerInfo->text().replace(" ","_");
    }
    command += "-e "+examiner+" ";
    QString evidenceNumber = ui->label_evidenceInfo->text();
    if (evidenceNumber.isEmpty() || !evidenceNumber.contains(QRegExp("\\d"))){
        evidenceNumber = "00";
    }
    command += "-E "+evidenceNumber+" ";
    command += "-f "+ui->comboBox_ewfType->currentText()+" ";
    command += "-g "+ui->label_granularity->text().split(" ").at(0)+" ";
    command += "-l /var/log/ewf/DWatson_EWF-"+this->dateTimeNowString().replace(" ","_")+".log ";
    command += "-m "+ui->comboBox_midia->currentText()+" ";
    command += "-M "+ui->comboBox_charact->currentText()+" ";
    QString notes = ui->label_notes->text();
    if (notes.isEmpty() || notes.length() < 2){
        notes = tr("Sem_anotações");
    }
    else{
        notes = notes.replace(" ","_");
    }
    command += "-N "+notes+" ";
    command += "-o "+ui->lineEdit_offset->text()+" ";
    command += "-P "+ui->label_bytesPerSector->text().split(" ").at(0)+" ";
    command += "-r "+ui->label_RetriesOnError->text().split(" ").at(0)+" ";
    command += "-S "+ui->label_imgSplitVal->text().split(" ").at(0)+"000 ";

    if (ui->checkBox_WipeSectorOnError->isChecked()){
        command += "-w ";
    }
    command += "-u";

    return command;
}

void MainWindow::parseValueToGraph(QString strVal, QString toolUsed)
{
    float value  = 0;
    float result = 0;

    if (toolUsed == "dcfldd"){
        if (strVal.contains(QRegExp("\\d"))){
            value  = strVal.toFloat();
            result = value - this->lastVal;
            this->lastVal = value;
        }
    }
    else if (toolUsed == "wipe"){
        if (strVal.contains(QRegExp("\\d"))){
            result = strVal.toInt();
            this->graph->itemName->setText(ui->label_status->text());
        }
    }
    else if (toolUsed == "dc3dd" || toolUsed == "dd"){
         value  = strVal.toFloat();
         result = value - this->lastVal;
         this->lastVal = value;
    }

    int tamanho_janela = 120;
    if (j > tamanho_janela) {
        this->graph->setValuesX(j-tamanho_janela, j);
    }

    //abaixo, reajusta o tamanho da matriz se o valor for maior que Y ----//
    if (result > limit){
        limit = result+1;
        this->graph->setValuesY(0, limit);
    }
    //acima, reajusta o tamanho da matriz se o valor for maior que Y ----//

    this->graph->addNewValue(j, result);
    j++;
}

void MainWindow::preserveOnReload()
{
    //Monta o combobox das ferramentas
    QStringList bitWiseToolsList;
    bitWiseToolsList << tr("Escolha ferramenta de cópia") << tr("DD - Tradicional Disc Dump (dd)") << tr("DCFLDD - Disc Dump Forense (dcfldd)") << tr("DC3DD - Disc Dump Forense (dc3dd)") << tr("EWF - Aquisição para ler no EnCase(c)");
    ui->comboBox_bitwiseTools->clear();
    ui->comboBox_bitwiseTools->addItems(bitWiseToolsList);
    //outros
    QString d,h;
    d = ui->label_date->text();
    h = ui->label_hour->text();
    QString bkp = ui->label_executing_tasks->text();
    ui->label_executing_tasks->setText(bkp);
    ui->groupBox_language->setDisabled(true);
    ui->pushButton_datetime->setEnabled(true);
    ui->checkBox_gerarLog->setEnabled(true);
    ui->textEdit->setText(tr("Olá. Lembrarei sempre que para iniciar as operações, você deve selecionar um idioma, confirmar ou ajustar data e hora e confirmar em 'Ok'."));
    ui->label_date->setText(tr(d.toUtf8()));
    ui->label_hour->setText(tr(h.toUtf8()));
}

void MainWindow::resetGraph()
{
    this->graph->clear();
    this->graph->setValuesY(0, 10);
    this->lastVal = 0;
    this->j       = 0;
}

void MainWindow::resetParams()
{
    ui->comboBox_Source_bitwise->setCurrentIndex(0);
    ui->comboBox_Target_bitwise->setCurrentIndex(0);
    enableFooterWidgets(false);
    ui->checkBox_wipe->setChecked(false);
    ui->progressBar->setValue(0);
    ui->progressBar->setDisabled(true);
    ui->pushButton_colect->setEnabled(false);

    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->setVisible(false);

}

void MainWindow::screenshotRename()
{
    if (ui->checkBox_rename_shots_wayland->isChecked()){
        if (!this->autoRenameShot->isRunning()){
            this->autoRenameShot->start();
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+"renameShot ");
        }
        return;
    }
    this->autoRenameShot->terminate();
}

void MainWindow::screenShort()
{
    QDateTime now = QDateTime::currentDateTime();
    QString FileName = now.toString().replace(" ","_");
    this->screenshot->shootScreen(FileName,true);
}

void MainWindow::screenShotSelfShot()
{
    this->screenshot->shootScreen("screenshot-checkbox-"+dateTimeNowString().replace(" ","_").toUtf8(),true);
}
//TODO: testar no live e ver se aplicou
void MainWindow::setDeviceToReadOnly(QString device, bool setRO)
{
    QMessageBox msg;
    msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    if (device.contains("Dev")){
        msg.setText(tr("Dispositivo não selecionado"));
        msg.setWindowTitle(tr(" A T E N Ç Ã O "));
        msg.exec();
        log("[user] dispositivo não selecionado para somente-leitura");
        return;
    }
    if (device.contains("/")){
        msg.setText(tr("O dispositivo não deve ter caminho precedente em setDeviceToReadOnly"));
        msg.setWindowTitle(tr(" ERRO DO PROGRAMA "));
        log(tr("[warning] ERRO DO PROGRAMA ").toUtf8());
        msg.exec();
        return;
    }

    if (!QFile::exists("/dev/"+device)){
        msg.setText(tr("Impossível abrir dispositivo"));
        msg.setWindowTitle(tr(" A T E N Ç Ã O "));
        msg.exec();
        log(tr("[warning] Dispositivo aparece na lista, mas não há permissões de acesso: ").toUtf8());
        return;
    }

    QProcess setROorRW;
    QStringList params;

    if (setRO){
        params << "--setro";
        log(tr("[user] Dispositivo marcado para somente-leitura:").toUtf8());

    }
    else{
        params << "--setrw";
        log(tr("[user] Dispositivo marcado para leitura-escrita").toUtf8());
    }
    //o blockdev bloqueia por partição e dispositivo, então melhor aplicar em tudo, protegendo inclusive a MBR ou GPT.
    params << "/dev/"+device.left(3)+"*";

    QString comm = "blockdev";
    if (QFile::exists("/home/djames/DEVEL")){
        comm = "ls";
    }
    setROorRW.start(comm,params);
    if (!setROorRW.waitForFinished()){
        msg.setText(tr("Não foi possível aplicar a operação solicitada!"));
        msg.setWindowTitle(tr(" A T E N Ç Ã O "));
        log(tr("[warning] Não foi possive aplicar o modo solicitado:").toUtf8());
        msg.exec();
        return;
    }
    //Checando se o dispositivo foi migrado para a opção selecionada
    QString rawDevice = device.left(3);
    if (!QFile::exists("/sys/block/"+rawDevice+"/ro")){
        QRegExp rxRaw("(sd\\w)");
        if (device.contains(rxRaw)){
            rxRaw.indexIn(device);
            QString rawDevice = rxRaw.capturedTexts().last();
        }
        if (!QFile::exists("/sys/block/"+rawDevice+"/ro")){
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            QString rwRo = tr(" leitura e escrita.");
            if (ui->checkBox_readOnly_bw->isChecked()){
                rwRo = tr(" proteção contra escrita.");
            }
            msg.setText(tr("Não foi possível verificar se o dispositivo foi migrado para")+tr(rwRo.toUtf8()));
            msg.setWindowTitle(tr("Leitura e escrita ou somente leitura"));
            log(tr("[warning] Não foi possivel verificar se o dispositivo foi migrado para").toUtf8() + tr(rwRo.toUtf8()).toUtf8());
            msg.exec();
            return;
        }
    }
    QFile systemFileToSeeIfIsReadOnlyOrReadAndWrite("/sys/block/"+rawDevice+"/ro");
    if (!systemFileToSeeIfIsReadOnlyOrReadAndWrite.open(QIODevice::ReadOnly | QIODevice::Text)){
        msg.setText(tr("Não foi possível confirmar se o dispositivo está em modo somente-leitura."));
        msg.setDetailedText(tr("Não foi possível verificar se a mídia está em modo 'somente leitura' ou 'leitura e escrita'.\
                        Não há problema aparente, mas não há garantia de proteção da origem caso se\
                        tente gravar nela."));
        msg.setWindowTitle(tr(" A T E N Ç Ã O "));
        log(tr("[warning] Não foi verificado o modo de operação da origem.").toUtf8());
        msg.exec();
        return;
    }

        QByteArray line;
        line = systemFileToSeeIfIsReadOnlyOrReadAndWrite.readLine();
        if (line.isEmpty()){
            msg.setText(tr("Não foi possível confirmar se o dispositivo está em modo somente-leitura."));
            msg.setDetailedText(tr("Não foi possível verificar o identificado de modo de operação do sistema.\
                            Não há problema aparente, mas não há garantia de proteção da origem caso se\
                            tente gravar nela."));
            msg.setWindowTitle(tr(" A T E N Ç Ã O "));
            log(tr("[warning] Não foi verificado o modo de operação da origem.").toUtf8());
            msg.exec();
            return;
        }
        systemFileToSeeIfIsReadOnlyOrReadAndWrite.close();
        if (QString(line).contains("1") && setRO){
            return;
        }
        else if (QString(line).contains("0") && !setRO){
            return;
        }
        else{
            QString rwOrRo = tr(" SOMENTE LEITURA ");
            if (!setRO){
                rwOrRo = tr(" LEITURA E ESCRITA ");
            }
            msg.setText(tr("Operação não efetuada. Você selecionou")+rwOrRo+tr("porém não foi possível executar a operação no dispositivo"));
            msg.setDetailedText(tr("Isso significa que o modo de proteção ou desproteção não foi realizado.\
                            Não há problema aparente, exceto a midia esteja protegida e você deseje gravar nela."));
            msg.setWindowTitle(tr(" A T E N Ç Ã O "));
            msg.exec();
        }
}

void MainWindow::Settings(){
    QString iniFile = "/home/djames/dwatson.ini";

    QSettings settings(iniFile,QSettings::IniFormat);
    QString nome = settings.value("wayland.general/nome","dwos").toString();

    this->graph->setVisible(true);
    QString itemName = settings.value("dwatson.graph/itemName","Djames Watson I/O").toString();
    this->graph->itemName->setText(itemName);
    this->graph->setMarginConfig(5,15,25,15);
    this->graph->setMinimumHeight(200);
    this->graph->setMinimumWidth(100);

    // init values
    QString graphX,graphY;
    graphX = settings.value("dwatson.graph/setTimeSampleInMinutes","2").toString();
    graphY = settings.value("dwatson.graph/setValuesY","0,10").toString();
    //TODO: ver o qunto a matriz suporta e limitar o tempo pro cara não colocar 4 semanas de amostragem
    this->graph->setValuesX(0,graphX.toInt()*60);
    this->graph->setValuesY(graphY.split(",").at(0).toInt(),graphY.split(",").at(1).toInt());

    ui->verticalLayout_9->addWidget(graph);
    this->graph->itemName->move(40,5);

    QString dir = settings.value("dwatson.general/fonts","/usr/share/fonts/truetype/droid/").toString();
    QDir directory(dir);
    QStringList entries = directory.entryList();
    for (int i=2;i<entries.length();i++){
        QFontDatabase::addApplicationFont("/usr/share/fonts/truetype/droid/"+entries.at(i));
    }
    bool maximizing = settings.value("dwatson.general/windowMaximized","false").toBool();
    if (maximizing){
        int delay = settings.value("dwatson.general/maximizedDelay","2").toInt();
        delay = delay * 1000;
        QTimer::singleShot(delay, this, SLOT(showMaximized()));
    }
    ui->checkBox_gerarLog->setEnabled(settings.value("dwatson.general/makeLog","false").toBool());
    ui->checkBox_gerarLog->setChecked(settings.value("dwatson.general/makeLog","false").toBool());
    this->lang = settings.value("dwatson.general/lang","none").toString();
    if (!this->lang.contains("none")){
        QTimer::singleShot(1000,this,SLOT(slotLangFromIniFile()));
    }

    //intervalo do shot
    this->shotInterval = settings.value("dwatson.general/shotInterval","1500").toInt();

    this->toolTips();

    //cast convertion
    ui->label_conversion_cast_value->setText(tr("Não iniciado"));
}

void MainWindow::setRadioTextTo(QString srcOrDst, QString value)
{
    ui->pushButton_show_data->setEnabled(true);
    if (srcOrDst == "src"){
        ui->radioButton_show_src_data->setText(value);
        ui->radioButton_show_src_data->setEnabled(true);
        return;
    }
    ui->radioButton_show_dst_data->setText(value);
    ui->radioButton_show_dst_data->setEnabled(true);
}

void MainWindow::slotCastConvFinished(QString msg)
{
    ui->label_conversion_cast_value->setText(msg);
    if (msg.contains(tr("Finalizado")) && ui->checkBox_remove_wcap->isChecked()){
        QString path = this->screencast->getWcapFullPath();
        if (path.isEmpty()){
            return;
        }
        QFile wcap(path);
        wcap.remove();
    }
}

void MainWindow::SlotChangeStackTabFirst()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::SlotChangeStackTabMiddle()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::SlotChangeStackTabNext()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::slotChangeTabWidget(int pos)
{
    if (pos != IMGTBOX){
        return;
    }
    log(tr("[user] Operando na janela de imagem: ").toUtf8());
    //limpa os labels se não tiver uma thread em execução (exceto a systeminfo, que apenas coleta dados para monitoramento)
    ui->tabWidget->setCurrentIndex(IMGTAB);
    if (!gpt->isRunning() && !wiper->isRunning() && !bitwise->isRunning() && !fingerprint->isRunning() && !crypt->isRunning() && !ewfAcquiring->isRunning()){
        ui->pushButton_model2_info->setText("");
        ui->pushButton_model_info->setText("");
        ui->pushButton_serial2_info->setText("");
        ui->pushButton_serial_info->setText("");
    }
    //Monta o combobox das ferramentas
    QStringList bitWiseToolsList;
    bitWiseToolsList << tr("Escolha ferramenta de cópia") << tr("DD - Tradicional Disc Dump (dd)") << tr("DCFLDD - Disc Dump Forense (dcfldd)") << tr("DC3DD - Disc Dump Forense (dc3dd)") << tr("EWF - Aquisição para ler no EnCase(c)");
    ui->comboBox_bitwiseTools->clear();
    ui->comboBox_bitwiseTools->addItems(bitWiseToolsList);
}

void MainWindow::slotChangeToolBoxTab(int pos)
{
    //wipe só é permitido na aba de geração de imagem, onde os dispositivos podem ser manipulados
    //mudança da tab deve reajustar o texto do label de status
    if (pos != IMGTAB){
        ui->checkBox_wipe->setChecked(false);
        ui->checkBox_wipe->setDisabled(true);
    }
    //abre o menu de opções do toolbox relacionado à geração de imagens
    if (pos == IMGTAB){
        ui->toolBox->setCurrentIndex(IMGTBOX);
        ui->label_status->setStyleSheet("color:black;");
    }
    //isso protege de uma segunda operação
    if (gpt->isRunning() || wiper->isRunning() || bitwise->isRunning() || fingerprint->isRunning() || ewfAcquiring->isRunning() || crypt->isRunning()){
        log("[step] Bloqueio dos componetes de geração de imagem durante a execução de um processo ");
        ui->comboBox_Source_bitwise->setDisabled(true);
        ui->comboBox_Target_bitwise->setDisabled(true);
        ui->comboBox_target_format->setDisabled(true);
        ui->comboBox_bitwiseTools->setDisabled(true);
        ui->label_status->setStyleSheet("color:black;");
        return;
    }
    ui->comboBox_Source_bitwise->clear();
    ui->comboBox_target_format->clear();
    ui->comboBox_Source_bitwise->setEnabled(true);
    ui->comboBox_Source_bitwise->addItems(this->common->connectedDevices(true,"none"));
    this->comboBoxFS();
    this->enableFooterWidgets(false);

    ui->pushButton_model2_info->setText("");
    ui->pushButton_model_info->setText("");
    ui->pushButton_serial2_info->setText("");
    ui->pushButton_serial_info->setText("");

    ui->pushButton_model2_info->setDisabled(true);
    ui->pushButton_model_info->setDisabled(true);
    ui->pushButton_serial2_info->setDisabled(true);
    ui->pushButton_serial_info->setDisabled(true);

    ui->label_size_dst->clear();
    ui->label_size_src->clear();
    ui->label_status->setText(tr("Nenhum processo em execução no momento."));

    //Valida tamanho do destino
    if (pos == TABMON){
        QRegExp rx("\\d");
        if (!ui->label_pbar_mon_dst->text().contains(rx)){
            return;
        }
        rx.setPattern("(\\s\\w{1,})");
        QString src_compare,dst_compare;
        rx.indexIn(ui->label_pbar_mon_dst->text());
        dst_compare = rx.capturedTexts().last();

        rx.indexIn(ui->label_pbar_mon_src->text());
        src_compare = rx.capturedTexts().last();

        if (!src_compare.isEmpty() && src_compare == dst_compare){
            rx.setPattern("(\\d{1,}\\.?\\d?).*");
            int src_size,dst_size;
            rx.indexIn(ui->label_pbar_mon_src->text());
            src_size = rx.capturedTexts().last().toFloat();
            if (src_size > 0){
                rx.indexIn(ui->label_pbar_mon_dst->text());
                dst_size = rx.capturedTexts().last().toFloat();
                if (dst_size < src_size){
                    log("[warning] 826 MW: Destino menor que Origem ");
                }
            }
        }
    }
}

void MainWindow::shotWindowOnly(QString name)
{
    this->grab().save("/var/log/screenshots/"+name+".png","png");
    emit nameFromShot(name);

}

void MainWindow::slotSliderBytesPerSector(int val)
{
    ui->label_bytesPerSector->setText(QString::number(val)+" (Bytes)");
}

void MainWindow::slotSliderDataBlock(int val)
{
    int result = 1;
    for (int i=1;i<val;i++){
        result = result * 2;
    }
    result = result < ui->horizontalSlider_gran->value() ? ui->horizontalSlider_gran->value() : result;
    ui->label_readOnce->setText(tr(QString::number(result).toUtf8())+tr(" (setores)"));
}

void MainWindow::slotSliderEvidenceSegment(int val)
{
   ui->label_imgSplitVal->setText(QString::number(val)+" (MB)");
}

void MainWindow::slotSliderGran(int val)
{
    ui->label_granularity->setText(QString::number(val)+tr(" (setores)"));
}

void MainWindow::slotSliderRetriesOnError(int val)
{
    ui->label_RetriesOnError->setText(QString::number(val)+tr(" (vezes)"));
}

void MainWindow::slotAllDevsAndPartsToImgRead()
{
    QStringList feed = this->common->connectedDevices(false,"none");
    for (int i=1;i<feed.length();i++){
        if (!feed.at(i).contains(QRegExp("\\d"))){
            feed.removeAt(i);
        }
    }
    ui->comboBox_srcImgToRead->addItems(feed);
}

void MainWindow::slotButtonMsg(QString CancelColect)
{
    ui->pushButton_colect->setText(CancelColect);
    if (CancelColect.contains("Cancel")){
        ui->pushButton_colect->setEnabled(true);
    }
    else{
        ui->pushButton_colect->setEnabled(false);
    }
}

void MainWindow::cancelAllRunningProcess()
{
    if (this->gpt->isRunning()){
        this->gpt->exit(); //TODO: incluir terminate, signal e exit dentro - incluir outras threads
    }
    if (this->wiper->isRunning()){
        emit signalTerminateWipe();
    }
    if (this->bitwise->isRunning()){
        emit signalStopBitWiseCopyNow();
    }
    if (this->ewfAcquiring->isRunning()){
        emit signalStopEWFprocess();
    }
    if (this->crypt->isRunning()){
        emit signalStopCrypt();
    }
}

void MainWindow::colectHashInfo(int tab)
{

    //se mudar de aba sem iniciar o processo, os widgets se reiniciam, portanto é seguro essa validação abaixo
    if (tab == IMGTAB){
        //se nao estiverem vazios, as midias ja foram selecionadas
        if ((!ui->pushButton_serial_info->text().isEmpty() && !ui->pushButton_serial2_info->text().isEmpty())){
            bool inputStatus;
            QString text = QInputDialog::getText(this,tr("Digite um nome para o arquivo"),tr("Nome para a Imagem:"),QLineEdit::Normal,
                                                ui->pushButton_model_info->text()+"_"+ui->pushButton_serial_info->text(),&inputStatus);

            if (!inputStatus || text.isEmpty()){
                QMessageBox box;
                box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
                box.setText(tr("Não é possível iniciar a cópia se houver cancelamento ou nome de arquivo vazio."));
                box.setWindowTitle(tr("Cópia não iniciada"));
                box.exec();
                return;
            }
            this->files << text;
        }
        else{
            QMessageBox box;
            box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            QString srcOrdst;
            if (ui->pushButton_serial_info->text().isEmpty()){
                srcOrdst = tr("origem");
            }
            else{
                srcOrdst = tr("destino");
            }
            box.setText(tr("Midia de ")+srcOrdst.toUpper()+tr(" não informada?"));
            box.setWindowTitle(tr("Midia não informada"));
            box.setDetailedText(tr("O campo de serial da mídia de ")+srcOrdst.toUpper()+tr(" não apresenta informações."));
            box.exec();
            log(tr("[warning] Serial de ").toUtf8()+srcOrdst.toUpper().toUtf8()+tr(" não foi exibido na janela de cópia bitwise: ").toUtf8());
            return;
        }
    }

    if (this->files.isEmpty()){
        QMessageBox box;
        box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        box.setText(tr("Melhor selecionar uma imagem primeiro..."));
        box.exec();
        log(tr("[user] Imagem não selecionada - aba 0 (MW SCHI (95) 614): ").toUtf8());
        return;
    }

    if (ui->checkBoxHashEmArquivo->isChecked() && !ui->checkBox_hash->isChecked()){
        QMessageBox box;
        box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        box.setText(tr("Marque a opção 'Gerar hash' e escolha um tipo primeiro."));
        box.exec();
        this->log(tr("[user] Faltou selecionar o tipo de hash: ").toUtf8());
        return;
    }

    QStringList hashParams;
    QString hash = "hash=";
    QString strSha;
    QString logName = "hashlog=";

    if (ui->checkBox_hash->isChecked()){
        ui->pushButton_colect->setText(tr("Aguarde"));//TODO: Verificar se deve ser removido ou trocado para Cancel, caso nao seja Cancel automaticamente
        ui->pushButton_colect->setDisabled(true);
        if (ui->radio_md5->isChecked()){
            strSha = "Md5";
            hash  += "Md5";
            log(tr("[step] Hash:md5 - ").toUtf8());
        }
        else if (ui->radio_sha->isChecked()){
            strSha = ui->comboBox_hash->currentText();
            hash += strSha;
            log("[step] Hash:" + hash.toUtf8() + " - ");
        }

        logName += this->files.at(0).split(".").at(0) + "." + strSha;
        hashParams << hash << logName << this->files.at(0);
        bool writeToFile = false;
        if (ui->checkBoxHashEmArquivo->isChecked()){
            writeToFile = true;
            log(tr("[user] Gravar hash em arquivo: Ok  - ").toUtf8());
        }
        this->common->varToHashFeed(hashParams,writeToFile);

        ui->pushButton_colect->setEnabled(true);
    }
}

void MainWindow::comboboxFeed()
{
    QStringList feed;
    feed << "logical" << "physical";
    ui->comboBox_charact->clear();
    ui->comboBox_charact->addItems(feed);
    feed.clear();
    feed << "deflate" << "bzip2";
    ui->comboBox_compressMethod->clear();
    ui->comboBox_compressMethod->addItems(feed);
    feed.clear();
    feed << "none" << "empty-block" << "fast" << "best";
    ui->comboBox_compressLevel->clear();
    ui->comboBox_compressLevel->addItems(feed);
    feed.clear();
    feed << "encase6" << "smart" << "ftk" << "encase1"<< "encase2"<< "encase3"<< "encase4"<< "encase5"<<  "encase7"<< "linen5"<< "linen6"<< "linen7"<< "ewfx";
    ui->comboBox_ewfType->clear();
    ui->comboBox_ewfType->addItems(feed);
    feed.clear();

    QString result,first,second;
    QDir directory("/dev/disk/by-id/");
    QStringList entries = directory.entryList();

    first  = "fixed";
    second = "removable";

    for (int i=2;i<entries.length();i++){

        QFileInfo fileinfo("/dev/disk/by-id/"+entries.at(i));

        if (entries.at(i).contains("usb-")){
            result = fileinfo.symLinkTarget();
            if (result.contains(ui->comboBox_Source_bitwise->currentText())){
                first  = "removable";
                second = "fixed";
                break;
            }
        }
    }

    feed.clear();
    feed << first << second << "optical" << "memory";
    ui->comboBox_midia->clear();
    ui->comboBox_midia->addItems(feed);
    feed.clear();

    QProcess limits;
    limits.start(QString("fdisk -l /dev/sda"));
    limits.waitForFinished(3000);
    result = limits.readAllStandardOutput();
    limits.close();
    QRegExp rx("Disk /dev/\\w{1,}: \\d{1,}\\.\\d{1,} \\w{1,}, (\\d{3,}) bytes");
    int stat = rx.indexIn(result);
    QString value = rx.capturedTexts().last();
    if (stat < 0){
        ui->lineEdit_limit->setText("0");
        return;
    }
    ui->lineEdit_limit->setText(value);
}

void MainWindow::comboBoxFS()
{
    QStringList fs;
    //somente fs suportado também pelo freeba, que é um forte candidato a ser o sistema
    fs << tr("Não") << "NTFS" << "EXT2" << "EXT3" << "XFS";
    ui->comboBox_target_format->addItems(fs);
}

void MainWindow::slotChooseTimeIntervalToScreenShot()
{
    if (!ui->checkBox_screenshot->isChecked()){
        return;
    }
    bool ok;
    QString interval = QInputDialog::getText(this, tr("Defina atraso da captura (ms)"),
                                             tr("Tempo de espera até que as caixas de mensagem se abram (ms):"), QLineEdit::Normal,
                                             QString::number(this->shotInterval), &ok);
    if (ok){
        this->shotInterval = interval.toInt();
    }
}

void MainWindow::slotComboboxDst(int val)
{
    if (val == 0){
        ui->comboBox_target_format->setCurrentIndex(0);
        ui->comboBox_target_format->setEnabled(false);
        ui->label_size_dst->clear();
        return;
    }

    ui->comboBox_target_format->setEnabled(true);
    if (ui->comboBox_Target_bitwise->currentText().contains(QRegExp("\\d"))){
        this->enableFooterWidgets(true);
    }
    else{
        this->enableFooterWidgets(false);
    }
    //apenas pra evitar que o botão seja desabilitado se houver processo em execução
    if (!ui->pushButton_colect->text().contains("Cancel")){
        ui->pushButton_colect->setDisabled(true);
    }

    if (ui->comboBox_Target_bitwise->currentIndex() > 0){
        ui->comboBox_bitwiseTools->setEnabled(true);
    }
    ui->comboBox_bitwiseTools->setCurrentIndex(0);

    if (ui->comboBox_Target_bitwise->currentIndex() > 0){
        ui->checkBox_wipe->setEnabled(true);
        this->justCheckIfIsReadOnly(ui->comboBox_Target_bitwise->currentText());
        ui->comboBox_target_format->setCurrentIndex(0);

        ui->pushButton_serial2_info->setDisabled(false);
        ui->pushButton_model2_info->setDisabled(false);

        //rótulos para os radios da aba Monitor do tabwidget e progressbar
        if (ui->comboBox_Source_bitwise->currentIndex() >0){
            this->setRadioTextTo("dst","/dev/"+ui->comboBox_Target_bitwise->currentText());
            int perc = this->getAndShowDiskFreeInProgressBarMonitor("/dev/"+ui->comboBox_Target_bitwise->currentText());
            ui->progressBar_dst_space->setValue(perc);
        }

        this->labelsFeed("dst");
        return;
    }
    ui->pushButton_model2_info->setText("");
    ui->pushButton_serial2_info->setText("");

    ui->pushButton_serial2_info->setDisabled(true);
    ui->pushButton_model2_info->setDisabled(true);
}

void MainWindow::slotCheckBoxChangeReadOnly()
{
    if (ui->comboBox_Source_bitwise->currentIndex() >0){
        bool readonly;
        QString dev;
        if (ui->checkBox_readOnly_bw->isChecked()){
            readonly = true;
        }
        else{
            readonly = false;
        }
        dev = ui->comboBox_Source_bitwise->currentText();
        if (!dev.contains("Dev") && !dev.isEmpty()){
            this->setDeviceToReadOnly(dev, readonly);
        }
    }
}

void MainWindow::slotConvertCast()
{
    // /bin/castconvert.sh t w d
    QString wcap = "/capture.wcap";
    QFile temp("/home/djames/DEVEL");
    if (temp.exists()){
        wcap = "/tmp/capture.wcap";
    }

    QFile castFile(wcap);
    if (!castFile.exists()){
        slotRunMessageBox(QString("Arquivo da captura não encontrado (Você utilizou Super+R para gravar e parar?). - Arquivo não encontrado").split("-"));
        return;
    }

    if (ui->pushButton_wayland_screencast->text().contains("Cancel")){
        emit abortConvertion();
        this->screenshot->shootScreen("castConvertAborted-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        ui->pushButton_wayland_screencast->setText(tr("Converter"));
        this->pbarUndefVal->setKeepRunning(false);
        if (pbarUndefVal->isRunning()){
            this->pbarUndefVal->setKeepRunning(false);
        }
        if (this->screencast->isRunning()){
            this->screencast->exit();
        }
        return;
    }
    ui->pushButton_wayland_screencast->setText(tr("Cancelar"));
    this->pbarUndefVal->setKeepRunning(true);
    this->pbarUndefVal->start();

    //TODO: melhorar tratamento das possiveis excessoes
        QStringList msg;
        //tem outro processo rodando?
        if (this->wiper->isRunning() || this->gpt->isRunning() || this->bitwise->isRunning() || ewfAcquiring->isRunning() || crypt->isRunning()){
            log(tr("[user] Já existe um processo de outro tipo rodando ").toUtf8());
            msg << tr("Já existe um processo de outro tipo rodando") << "" << "";
            emit signalRunMessageBox(msg);
            return;
        }

        if (!QFile::exists(wcap)){
            msg.clear();
            msg << tr("Arquivo de captura não encontrado.");
            msg << " " << "";
            emit signalRunMessageBox(msg);
            return;
        }
        QFile arq2size(wcap);
        if (!arq2size.open(QIODevice::ReadOnly)){
            msg.clear();
            emit log(tr("[warning] Não passou pela validação de tamanho do arquivo ").toUtf8());
            msg << tr("Não foi possível ler o tamanho do arquivo.") << " " << "";
            emit signalRunMessageBox(msg);
            return;
        }
        int fileSize = arq2size.size()/1024;
        arq2size.close();

        int estimatedWebmFileSize = fileSize/9;

        QRegExp rx("MemFree:\\s{1,}\\d{1,}\\s\\w{1,}");
        QFile meminfo("/proc/meminfo");
        meminfo.open(QIODevice::Text|QIODevice::ReadOnly);
        QString content = meminfo.readAll();

        rx.indexIn(content);
        QString result = rx.capturedTexts().last();
        if (result.length() < 2){
            //"nao deu";
            return;
        }

        rx.setPattern("(\\d{1,})");
        if (rx.indexIn(result) < 1){
            return;
        }
        result = rx.capturedTexts().last();

        int freeMem  = result.toInt();
        freeMem      = freeMem/1024;
        int media    = estimatedWebmFileSize/1024 < 1 ? 1 : estimatedWebmFileSize/1024;
        int isEnough = freeMem-media;

        if (isEnough < 100){
            QTimer::singleShot(this->shotInterval,this->screenshot,SLOT(shootScreen("espaco-casting",ui->checkBox_screenshot->isChecked())));
            emit log(tr("[user] Não há espaço para a conversão do casting ").toUtf8());
            msg << tr("Não há espaço para a conversão do casting") << "" << "";
            emit signalRunMessageBox(msg);
            return;
        }
        ui->tabWidget->setCurrentIndex(1);
        QString name = "/var/log/screencasts/cast-" + dateTimeNowString().replace(" ","_") + ".webm";
        this->screencast->setSourceAndTarget(wcap,name);
        this->screencast->start();
        this->screenshot->shootScreen("castConvertStarted-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
}

void MainWindow::slotCryptFinished()
{
    ui->label_crypt_status->setText("Finalizado");
    this->pbarUndefVal->setKeepRunning(false);

    //restaurar estado dos widgets
    if (ui->checkBox_OpenSSL->isChecked()){
        ui->checkBox_OpenSSL->setChecked(false);
        ui->checkBox_aescrypt->setEnabled(true);
    }
    else if (ui->checkBox_aescrypt->isChecked()){
        ui->checkBox_aescrypt->setChecked(false);
        ui->checkBox_OpenSSL->setEnabled(true);
    }
    ui->lineEdit_pass->clear();
    ui->lineEdit_passConfirm->clear();
    ui->stackedWidget_crypt->setCurrentIndex(0);

    ui->checkBox_wipe->setChecked(false);
    ui->checkBox_readOnly_bw->setEnabled(true);
    slotPass("");
    this->graph->itemName->setText("Finalizado");
    slotIOFinished("crypt");
}

void MainWindow::slotCryptStarted()
{
    ui->label_crypt_status->setText(tr("Iniciado"));
    ui->label_status->setText(tr("Iniciado o processo de criptografia."));
    if (!this->pbarUndefVal->isRunning()){
        this->pbarUndefVal->setKeepRunning(true);
        this->pbarUndefVal->start();
        this->graph->itemName->setText(tr("(Criptografia)"));
        slotIOStarted("crypt");
    }
}

void MainWindow::connections()
{
    // treeview: identificar o pushbutton logs ou images
    signalMapperTreeView = new QSignalMapper(this);
    signalMapperTreeView->setMapping(ui->pushButton_logsTreeView,QString("logs"));
    signalMapperTreeView->setMapping(ui->pushButton_acessMidia,QString("images"));

    connect(ui->pushButton_logsTreeView,SIGNAL(clicked()), signalMapperTreeView,SLOT(map()));
    connect(ui->pushButton_acessMidia,SIGNAL(clicked()),signalMapperTreeView,SLOT(map()));
    connect(signalMapperTreeView,SIGNAL(mapped(QString)),this,SLOT(slotTreeView(QString)));

    // idioma on-the-fly
    signalMapperLang = new QSignalMapper(this);
    signalMapperLang->setMapping(ui->radioButton_en, QString("en"));
    signalMapperLang->setMapping(ui->radioButton_es, QString("es"));
    signalMapperLang->setMapping(ui->radioButton_pt, QString("pt"));

    connect(ui->radioButton_en, SIGNAL(toggled(bool)), signalMapperLang, SLOT(map()));
    connect(ui->radioButton_es, SIGNAL(toggled(bool)), signalMapperLang, SLOT(map()));
    connect(ui->radioButton_pt, SIGNAL(toggled(bool)), signalMapperLang, SLOT(map()));
    connect(signalMapperLang, SIGNAL(mapped(QString)), this, SLOT(translate2(QString)));

    connect(ui->checkBox_hash,SIGNAL(clicked()),this,SLOT(slotEnableHashes()));
    connect(ui->checkBox_wipe,SIGNAL(clicked()),this,SLOT(slotWipeNow())); 
    connect(ui->checkBox_readOnly_bw,SIGNAL(clicked()),this,SLOT(slotCheckBoxChangeReadOnly()));
    connect(ui->checkBox_aescrypt,SIGNAL(clicked()),this,SLOT(slotUnchecker()));
    connect(ui->checkBox_OpenSSL,SIGNAL(clicked()),this,SLOT(slotUnchecker()));
    connect(ui->checkBox_screenshot,SIGNAL(clicked()),this,SLOT(slotChooseTimeIntervalToScreenShot()));

    connect(ui->listWidget_medias, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotPartitionClicked(QListWidgetItem*)));

    connect(ui->pushButton_colect,SIGNAL(clicked()),this, SLOT(slotPushButtonColect()));
    connect(ui->pushButton_reread,SIGNAL(clicked()),SLOT(slotRereadDevicesTab()));
    connect(ui->pushButton_selecionar,SIGNAL(clicked()),this,SLOT(slotSelectImage4read()));
    connect(ui->pushButton_partitions,SIGNAL(clicked()),this,SLOT(slotPartitionsView()));
    connect(ui->pushButton_datetime,SIGNAL(clicked()),this,SLOT(slotSettime()));
    connect(ui->pushButton_serial_info,SIGNAL(clicked()),this,SLOT(slotShowingMyTextValueSerialInfo()));
    connect(ui->pushButton_serial2_info,SIGNAL(clicked()),this,SLOT(slotShowingMyTextValueSerialInfo2()));
    connect(ui->pushButton_model_info,SIGNAL(clicked()),this,SLOT(slotShowingMyTextValueModelInfo()));
    connect(ui->pushButton_model2_info,SIGNAL(clicked()),this,SLOT(slotShowingMyTextValueModelInfo2()));
    connect(ui->pushButton_show_data,SIGNAL(clicked()),this,SLOT(slotShowTrueTreeFromDevice()));
    connect(ui->pushButton_wayland_screencast,SIGNAL(clicked()),this,SLOT(slotConvertCast()));
    connect(ui->pushButton_logHandler,SIGNAL(clicked()),this,SLOT(slotLogHandler()));
    connect(ui->pushButton_secondTab,SIGNAL(clicked()),this,SLOT(SlotChangeStackTabMiddle()));
    connect(ui->pushButton_back,SIGNAL(clicked()),this,SLOT(SlotChangeStackTabMiddle()));
    connect(ui->pushButton_next,SIGNAL(clicked()),this,SLOT(SlotChangeStackTabNext()));
    connect(ui->pushButton_first,SIGNAL(clicked()),this,SLOT(SlotChangeStackTabFirst()));
    connect(ui->pushButton_ewfInfo,SIGNAL(clicked()),this,SLOT(slotEwfInfo()));
    connect(ui->pushButton_imgHashView,SIGNAL(clicked()),this,SLOT(slotHashView()));
    connect(ui->horizontalSlider_screenshots,SIGNAL(valueChanged(int)),this,SLOT(shotsViewer()));
    connect(ui->pushButton_remove,SIGNAL(clicked()),this,SLOT(slotRemoveFile()));
    connect(ui->pushButton_encrypt,SIGNAL(clicked()),this,SLOT(slotPushButtonEncrypt()));

    //ewf - um signalmapper cai bem nesses 3
    connect(ui->pushButton_changeParams,SIGNAL(clicked()),this,SLOT(slotOpenEWFform()));
    connect(ui->pushButton_showDescription,SIGNAL(clicked()),this,SLOT(slotOpenEWFform()));
    connect(ui->pushButton_showNotes,SIGNAL(clicked()),this,SLOT(slotOpenEWFform()));

    connect(this,SIGNAL(signalStopEWFprocess()),this->ewfAcquiring,SLOT(slotStopEWFprocess()));
    connect(ui->horizontalSlider_evidenceSegment,SIGNAL(valueChanged(int)),this,SLOT(slotSliderEvidenceSegment(int)));
    connect(ui->horizontalSlider_bytesPerSector,SIGNAL(valueChanged(int)),this,SLOT(slotSliderBytesPerSector(int)));
    connect(ui->horizontalSlider_dataBlock,SIGNAL(valueChanged(int)),this,SLOT(slotSliderDataBlock(int)));
    connect(ui->horizontalSlider_gran,SIGNAL(valueChanged(int)),this,SLOT(slotSliderGran(int)));
    connect(ui->horizontalSlider_retriesOnError,SIGNAL(valueChanged(int)),this,SLOT(slotSliderRetriesOnError(int)));
    connect(this->ewfAcquiring,SIGNAL(signalProgressBar(int)),this,SLOT(slotSetProgressBarValNow(int)));

    connect(this,SIGNAL(signalRetries(int)),this->bitwise,SLOT(slotRetries(int)));
    connect(this,SIGNAL(signalTimeout(int)),this->bitwise,SLOT(slotTimeout(int)));

    connect(ui->radio_md5,SIGNAL(toggled(bool)),this,SLOT(slotRadioButtonHashMd5(bool)));
    connect(ui->radio_sha,SIGNAL(toggled(bool)),this,SLOT(slotRadioButtonHashSha(bool)));

    connect(ui->radioButton_fire_src,SIGNAL(clicked()),this,SLOT(slotWipeFreeSectors()));

    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),this,SLOT(slotChangeToolBoxTab(int)));

    connect(ui->comboBox_Source_bitwise,SIGNAL(currentIndexChanged(int)),this,SLOT(slotTargetBitwiseComboHandler(int)));
    connect(ui->comboBox_Target_bitwise,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComboboxDst(int)));
    connect(ui->comboBox_target_format,SIGNAL(currentIndexChanged(int)),SLOT(slotFormatMidiaHandler(int)));
    connect(ui->comboBox_bitwiseTools,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotInfoAboutSelectedTool(QString)));
    connect(ui->comboBox_srcImgToRead,SIGNAL(currentIndexChanged(QString)),SLOT(slotMount(QString)));

    connect(this->wiper,SIGNAL(msgFromWipe(QString)),this,SLOT(slotMsgFromWiper(QString)));

    connect(this,SIGNAL(signalPercentOfWipe(int)),this,SLOT(slotSetProgressBarValNow(int)));
    connect(this,SIGNAL(signalTerminateWipe()),this->wiper,SLOT(slotTerminateWipe()));

    connect(this->sysInfo,SIGNAL(cpu(QString,int)),this,SLOT(slotProgressBarCPU(QString,int)));
    connect(this->sysInfo,SIGNAL(memory(int,int,int)),this,SLOT(slotSetMemoryValues(int,int,int)));

    connect(this,SIGNAL(buttonMsg(QString)),this,SLOT(slotButtonMsg(QString)));

    connect(ui->toolBox,SIGNAL(currentChanged(int)),this,SLOT(slotChangeTabWidget(int)));

    //label status
    connect(this->gpt,SIGNAL(signalStatusLabel(QString)),ui->label_status,SLOT(setText(QString)));
    connect(this,SIGNAL(signalLabelStatusSetText(QString)),ui->label_status,SLOT(setText(QString)));
    connect(this->ewfAcquiring,SIGNAL(signalMsgStatus(QString)),ui->label_status,SLOT(setText(QString)));
    connect(this->crypt,SIGNAL(status(QString)),ui->label_status,SLOT(setText(QString)));

    connect(this->bitwise,SIGNAL(signalStringStatusFromToolUsedInBitWiseCopy(QString)),this,SLOT(slotStringStatusFromToolUsedInBitWiseCopy(QString)));
    connect(this,SIGNAL(signalSetCommandToBitWiseCopy(QString)),this->bitwise,SLOT(slotSetCommandToBitWiseCopy(QString)));
    connect(this,SIGNAL(signalMountOrUmontAndCheckConditionsOf(QString,QString)),this->bitwise,SLOT(slotMountOrUmontAndCheckConditionsOf(QString,QString)));
    connect(this->bitwise,SIGNAL(signalIsFinished(int)),this,SLOT(slotFinishedCopyWithStatus(int)));
    connect(this,SIGNAL(signalStopBitWiseCopyNow()),this->bitwise,SLOT(slotStopBitWiseCopyNow()));

    connect(this,SIGNAL(signalValuesToPlot(int,int)),this->graph,SLOT(addNewValue(int,int)));

    //msgbox
    connect(this->fingerprint,SIGNAL(signalMsgBox(QStringList)),this,SLOT(slotRunMessageBox(QStringList)));
    connect(this->ewfAcquiring,SIGNAL(signalMsgbox(QStringList)),this,SLOT(slotRunMessageBox(QStringList)));
    connect(this->gpt,SIGNAL(signalRunMessageBox(QStringList)),this,SLOT(slotRunMessageBox(QStringList)));
    connect(this->common,SIGNAL(signalMsgBox(QStringList)),this,SLOT(slotRunMessageBox(QStringList)));
    connect(this->wiper,SIGNAL(signalMsgBox(QStringList)),this,SLOT(slotRunMessageBox(QStringList)));
    connect(this->crypt,SIGNAL(signalMsgbox(QStringList)),this,SLOT(slotRunMessageBox(QStringList)));

    //LOGs
    connect(this->fingerprint,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));
    //o pushbutton esta na seção dos pushbutton, bem acima
    connect(this->bitwise,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));
    connect(this->common,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));
    connect(this->gpt,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));
    connect(this->wiper,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));
    connect(this->ewfAcquiring,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));
    connect(this->crypt,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));

    //start das threads alimentam o label de tarefas
    //DOC
    connect(ui->comboBox_topics,SIGNAL(currentIndexChanged(QString)),this->doc,SLOT(getSection(QString)));

    //screenshot no Wayland e no qt
    connect(this->screenshot,SIGNAL(shoot(QString)),this,SLOT(shotWindowOnly(QString)));
    connect(ui->checkBox_screenshot,SIGNAL(clicked()),this,SLOT(screenShotSelfShot()));
    connect(ui->checkBox_rename_shots_wayland,SIGNAL(clicked()),this,SLOT(screenshotRename()));

    //shots viewer
    connect(this->screenshot,SIGNAL(imageName(QString)),this,SLOT(slotImageFeed(QString)));
    connect(this,SIGNAL(nameFromShot(QString)),this,SLOT(slotImageFeed(QString)));

    //screencast
    connect(this->screencast,SIGNAL(log(QByteArray)),this,SLOT(log(QByteArray)));
    connect(this->screencast,SIGNAL(signalRunMessageBox(QStringList)),this,SLOT(slotRunMessageBox(QStringList)));
    connect(this->screencast,SIGNAL(signalLabelStatus(QString)),ui->label_status,SLOT(setText(QString)));
    connect(this->screencast,SIGNAL(signalIsFinished(int)),this,SLOT(slotFinishedCopyWithStatus(int)));
    connect(this->screencast,SIGNAL(signalLabelBytesConverted(QString)),this,SLOT(slotCastConvFinished(QString)));
    connect(this,SIGNAL(abortConvertion()),this->screencast,SLOT(abortConvertion()));

    //pbar
    connect(this->gpt,SIGNAL(signalSetProgressBarValNow(int)),this,SLOT(slotSetProgressBarValNow(int)));
    connect(this->pbarUndefVal,SIGNAL(setInvertedAppearance(bool)),this,SLOT(slotInvert(bool)));
    connect(this->pbarUndefVal,SIGNAL(setValue(int)),ui->progressBar_screencast,SLOT(setValue(int)));

    //crypt - passwd
    connect(ui->checkBox_showPasswd,SIGNAL(clicked()),this,SLOT(slotSetPasswdEchoMode()));
    connect(ui->lineEdit_pass,SIGNAL(textEdited(QString)),this,SLOT(slotPass(QString)));
    connect(ui->lineEdit_passConfirm,SIGNAL(textEdited(QString)),this,SLOT(slotPassConfirm(QString)));

    connect(this,SIGNAL(signalCrypt()),this,SLOT(encrypt()));
    connect(this,SIGNAL(signalStopCrypt()),this->crypt,SLOT(slotStopCrypt()));

    connect(this->doc,SIGNAL(signalSelectedSection(QString)),ui->textBrowser_doc,SLOT(setHtml(QString)));

    //---------- MAPA DE SINAIS ----------------------------------------------------------------
    signalMapperStarted = new QSignalMapper(this);                                            //
    signalMapperStarted->setMapping(this->ewfAcquiring,QString("ewfacquiring"));              //
    signalMapperStarted->setMapping(this->bitwise,QString("bitwise"));                        //
    signalMapperStarted->setMapping(this->wiper,QString("wiper"));                            //
    signalMapperStarted->setMapping(this->crypt,QString("crypt"));                            //
    signalMapperStarted->setMapping(this->fingerprint,QString("hash"));                       //
    signalMapperStarted->setMapping(this->gpt,QString("formatar"));                           //
    signalMapperStarted->setMapping(this->screencast,QString("castConvert"));
    //                                                                                        //
    connect(this->ewfAcquiring,SIGNAL(started()),signalMapperStarted,SLOT(map()));            //
    connect(this->bitwise,SIGNAL(started()),signalMapperStarted,SLOT(map()));                 //
    connect(this->wiper,SIGNAL(started()),signalMapperStarted,SLOT(map()));                   //
    connect(this->crypt,SIGNAL(started()),signalMapperStarted,SLOT(map()));                   //
    connect(this->fingerprint,SIGNAL(started()),signalMapperStarted,SLOT(map()));             //
    connect(this->gpt,SIGNAL(started()),signalMapperStarted,SLOT(map()));                     //
    connect(this->screencast,SIGNAL(started()),signalMapperStarted,SLOT(map()));
    connect(signalMapperStarted,SIGNAL(mapped(QString)),this,SLOT(slotIOStarted(QString)));   //
    //                                                                                        //
    signalMapperFinished = new QSignalMapper(this);                                           //
    signalMapperFinished->setMapping(this->ewfAcquiring,QString("ewfacquiring"));             //
    signalMapperFinished->setMapping(this->bitwise,QString("bitwise"));                       //
    signalMapperFinished->setMapping(this->wiper,QString("wiper"));                           //
    signalMapperFinished->setMapping(this->crypt,QString("crypt"));                           //
    signalMapperFinished->setMapping(this->fingerprint,QString("hash"));                      //
    signalMapperFinished->setMapping(this->gpt,QString("formatar"));                          //
    signalMapperFinished->setMapping(this->screencast,QString("castConvert"));
    //                                                                                        //
    connect(this->ewfAcquiring,SIGNAL(finished()),signalMapperFinished,SLOT(map()));          //
    connect(this->bitwise,SIGNAL(finished()),signalMapperFinished,SLOT(map()));               //
    connect(this->wiper,SIGNAL(finished()),signalMapperFinished,SLOT(map()));                 //
    connect(this->crypt,SIGNAL(finished()),signalMapperFinished,SLOT(map()));                 //
    connect(this->fingerprint,SIGNAL(finished()),signalMapperFinished,SLOT(map()));           //
    connect(this->gpt,SIGNAL(finished()),signalMapperFinished,SLOT(map()));                   //
    connect(this->screencast,SIGNAL(finished()),signalMapperFinished,SLOT(map()));
    connect(signalMapperFinished,SIGNAL(mapped(QString)),this,SLOT(slotIOFinished(QString))); //
    //                                                                                        //
    //iostats                                                                                 //
    connect(this->iostat,SIGNAL(signalMbps(QString)),this,SLOT(slotStatsToGraph(QString)));   //
    //                                                                                        //
    //----------------------------------------------------------------------------------------//
}

void MainWindow::copyFilesAndDirs(QStringList dirs,QString target)
{
    QDir d;
    QString newPath;
    QString file;
    for (int i=0;i<dirs.length();i++){
        QStringList dirsToMake = dirs.at(i).split("/");
        if (dirsToMake.at(0).length() < 2){
            dirsToMake.removeFirst();
        }
        QString targetDirs;
        QString concatDirs;
        for (int j=0;j<dirsToMake.length();j++){
            concatDirs += "/" + dirsToMake.at(j);
            targetDirs = target  + concatDirs;
            d.mkdir(targetDirs);
        }

        QDirIterator it(dirs.at(i), QDirIterator::Subdirectories);
        while (it.hasNext()) {
            file =it.next();
            newPath = target + file;
            QFile::copy(file , newPath);
        }
    }
    QDir isLog("/mnt/var/log");
    if (isLog.exists()){
        QFile::copy("/var/log/ForensicMessages.log" , "/mnt/var/log/ForensicMessages.log");
    }
}

void MainWindow::copyLogsTo()
{
    QComboBox *box;
    box = new QComboBox();
    QStringList devicesWithoutSourceDevice = this->common->connectedDevices(true,"none");
    QString devToRemove = ui->radioButton_show_src_data->text().remove("/dev/");

    for (int i=0;i<devicesWithoutSourceDevice.length();i++){
        if (devicesWithoutSourceDevice.at(i) == devToRemove){
            devicesWithoutSourceDevice.removeAt(i);
            break;
        }
    }
    box->addItems(devicesWithoutSourceDevice);

    connect(box,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotSaveLog(QString)));
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(box);

    QDialog dialog;
    dialog.setLayout(layout);
    dialog.setWindowTitle(tr("Destino"));

    connect(box,SIGNAL(currentIndexChanged(int)),&dialog,SLOT(close()));

    dialog.exec();
    delete layout;
    delete box;
}

QString MainWindow::dateTimeNowString()
{
    QDateTime now = QDateTime::currentDateTime();
    return now.currentDateTime().toString();
}

void MainWindow::enableDisableItems(bool trueFalse){
    if (trueFalse){
        log(tr("[step] Ativando ítens da janela: ").toUtf8());
    }
    else{
        log(tr("[step] Desativando ítens da janela: ").toUtf8());
    }

    for (int i=1;i<4;i++){
        ui->toolBox->setItemEnabled(i,trueFalse);
        if (i<4){
            ui->tabWidget->setTabEnabled(i,trueFalse);
        }
    }
}

void MainWindow::enableFooterWidgets(bool condition)
{
    if (!condition){
        ui->checkBox_hash->setChecked(condition);
        //ui->checkBox_gerarLog->setChecked(condition);
        ui->checkBoxHashEmArquivo->setChecked(condition);
        ui->checkBoxHashEmArquivo->setEnabled(condition);
        ui->radio_md5->setChecked(condition);
        ui->radio_sha->setChecked(condition);
        ui->radio_md5->setEnabled(condition);
        ui->radio_sha->setEnabled(condition);
        ui->comboBox_hash->setEnabled(condition);
    }

    ui->label_image->setStyleSheet("color:black;");
    ui->pushButton_colect->setEnabled(condition);
    ui->checkBox_hash->setEnabled(condition);
    //ui->checkBox_gerarLog->setEnabled(condition);
}

void MainWindow::slotEnableHashes()
{
    if (ui->checkBox_hash->isChecked()){
        ui->radio_md5->setEnabled(true);
        ui->radio_sha->setEnabled(true);
        ui->radio_sha->setChecked(true);
        ui->comboBox_hash->setEnabled(true);
        ui->checkBoxHashEmArquivo->setEnabled(true);
        log("[user] Checkbox hashes ativado: - ");
    }
    else{
        ui->radio_md5->setChecked(false);
        ui->radio_sha->setChecked(false);
        ui->radio_md5->setEnabled(false);
        ui->radio_sha->setEnabled(false);
        ui->checkBoxHashEmArquivo->setChecked(false);
        ui->comboBox_hash->setEnabled(false);
        ui->checkBoxHashEmArquivo->setEnabled(false);
        log(tr("[user] Checkbox hashes desativado: - ").toUtf8());
    }
}

void MainWindow::slotEwfInfo()
{
    if (!ui->label_image->text().contains("/")){
        return;
    }

    ui->stackedWidget_fromImg->setCurrentIndex(0);
    QProcess ewfinfo;
    ewfinfo.start(QString("ewfinfo ")+ui->label_image->text());
    ewfinfo.waitForFinished(3000);
    QString result = ewfinfo.readAllStandardOutput();
    ui->textBrowserInfo->setText(result);
    ewfinfo.close();

}

void MainWindow::slotFinishedCopyWithStatus(int exitCode)
{
    this->exitCode = exitCode;
    if (this->exitCode == EXITOK && ui->progressBar->value() < 100){
        slotSetProgressBarValNow(100);
    }
}

void MainWindow::slotFormatMidiaHandler(int index)
{
    QMessageBox msg;
    msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    msg.setText(tr("Alguma anomalia ocorreu."));
    if (!ui->comboBox_target_format->isEnabled()){
        return;
    }
    QString dev = "/dev/"+ui->comboBox_Target_bitwise->currentText();

    if (ui->comboBox_Target_bitwise->currentText().length() < 4 && index != 0){
        if (ui->radioButton_bw_disc2disc->isChecked()){
            msg.setText(tr("Você selecionou formatar uma mídia para cópia dispositivo-a-dispositivo. Leia os detalhes antes de confirmar. Seguir adiante?"));
            msg.setDetailedText(tr("Uma simples formatação não é o suficiente para eliminar dados anteriores contidos na mídia. \
                             O ideal é que se faça wipe, um processo demorado, porém que garantirá a subescrição de dados anteriores."));
            msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msg.setButtonText(QMessageBox::Yes, tr("Sim"));
            msg.setButtonText(QMessageBox::No, tr("Não"));
        }
        else if (ui->radioButton_bw_src2file->isChecked() && !ui->comboBox_Target_bitwise->currentText().isEmpty()){

            int havePartition = this->common->isTherePartition(dev);
            if (havePartition == 0){
                msg.setText(tr("Você selecionou um dispositivo que contém tabela de partições, mas será recriada uma tabela de partições com apenas 1. Deseja reformatá-lo?"));
                msg.setDetailedText(tr("Você selecionou um dispositivo invés de uma partição, mas na aba geração de imagem escolheu 'Origem a Arquivo'. Se é isso que\
                                      pretende, então selecione um sistema de arquivos para formatar a mídia de destino. Se pretende gravar diretamente no dispositivo\
                                      então selecione a opção Origem a Destino no menu Geração de Imagem.\
                                      Você pode mudar sua escolha tanto na aba geração de imagem quanto no box de dispositivos, selecionando uma partição\
                                      invés do dispositivo. Além disso, é recomendado que seja executado um wipe previamente na mídia.Os identificadores \
                                      funcionam desse modo:\n\
                                      sd - representa um disco SCSI, SATA, IDE ou USB\n\
                                      a,b,c... - representa a unidade física\n\
                                      1,2,3... - representa a partição da mídia\n\
                                      Portanto, sdb1 é a primeira partição da segunda mídia.\n\
                                      Mais detalhes na documentação."));
                msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msg.setButtonText(QMessageBox::Yes, tr("Sim"));
                msg.setButtonText(QMessageBox::No, tr("Não"));
            }
            else{
                msg.setWindowTitle(tr("Iniciar particionamento"));
                msg.setText(tr("O dispositivo não contém partições. Será criado, conforme a escolha do sistema de arquivos. Seguir?"));
                msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msg.setButtonText(QMessageBox::Yes, tr("Sim"));
                msg.setButtonText(QMessageBox::No, tr("Não"));
            }
        }

        if (ui->comboBox_target_format->currentText().isEmpty()){
            return;
        }
        //se é menor que 4 e index != 0 só pode ser device
        QTimer::singleShot(this->shotInterval,this->screenshot,SLOT(shootScreen("formatar-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked())));
        int code = msg.exec();
        //this->screenshot->shootScreen("formatar-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());

        if (code == QMessageBox::Yes && ui->comboBox_target_format->currentIndex() >0){
                if (ui->radioButton_bw_src2file->isChecked()){
                    msg.setText(tr("Você escolheu formatar o dispositivo de destino cujo serial é ")+ui->pushButton_serial2_info->text()+tr(".\nConfirma a operação?"));
                    msg.setWindowTitle(tr("Formatar Midia de Destino"));
                    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                    msg.setButtonText(QMessageBox::Yes, tr("Formatar"));
                    msg.setButtonText(QMessageBox::No, tr("Não"));
                    msg.setButtonText(QMessageBox::Cancel,tr("Cancelar Operação"));
                    QTimer::singleShot(this->shotInterval,this->screenshot,SLOT(shootScreen("formatar2-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked())));
                    int answer = msg.exec();
                    //this->screenshot->shootScreen("formatar2-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
                    switch (answer){
                        case QMessageBox::Yes:
                            log(tr("[user] Usuário confirmou formatação:").toUtf8());
                            this->gpt->deviceAndFileSystemToFormat(dev,ui->comboBox_target_format->currentText());
                            this->gpt->start();
                            return;

                        case QMessageBox::No:
                            log(tr("[user] Usuário solicitou não formatar:").toUtf8());
                            ui->comboBox_target_format->setCurrentIndex(0);
                            this->screenshot->shootScreen("formatar3-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
                            break;

                        default:
                            //default cancelar porque é o mais seguro em caso de dúvidas
                            log(tr("[user] Usuário desistiu da operação de cópia bitwise:").toUtf8());
                            ui->comboBox_target_format->setCurrentIndex(0);
                            this->screenshot->shootScreen("formatar3-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
                            return;
                    }
                }
        }
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setWindowTitle(tr("Cancelamento de operação"));
        msg.setText(tr("A mídia de destino não foi modificada."));
        QTimer::singleShot(this->shotInterval,this->screenshot,SLOT(shootScreen("formatar4-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked())));
        msg.exec();
        //this->screenshot->shootScreen("formatar4-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
    }
    if (ui->comboBox_Target_bitwise->currentText().length() == 4 && ui->radioButton_bw_src2file->isChecked() && ui->comboBox_target_format->currentIndex() >0){ //é partição
        msg.setWindowTitle(tr("Seleção de partição"));
        msg.setText(tr("A mídia será totalmente manipulado e 1 partição será criada com o sistema de arquivos escolhido. Confirma?"));
        msg.setDetailedText(tr("Isso significa que a mídia de destino será preparada para receber exclusivamente a origem.\
                            Essa é a forma ideal de preparar uma mídia para essa tarefa, apesar de possíveis dados poderem continuar a existir\
                            nos setores. Nesse caso, é melhor que se faça wipe, selecionando essa opção no menu <b>Geração de Imagem</b> e\
                            posteriormente executar a formatação."));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes,tr("Sim"));
        msg.setButtonText(QMessageBox::No,tr("Não"));

        QTimer::singleShot(this->shotInterval,this->screenshot,SLOT(shootScreen("formatar5a-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked())));
        int answer = msg.exec();
        switch (answer){
            case QMessageBox::Yes:
                log(tr("[user] Usuário confirmou formatação:").toUtf8());
                ui->label_time_inicio->setText(dateTimeNowString());
                this->gpt->deviceAndFileSystemToFormat(dev,ui->comboBox_target_format->currentText());
                this->gpt->start();
                this->screenshot->shootScreen("formatar5-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());

                //trava o combobox dos fs
                ui->comboBox_target_format->setDisabled(true);
                break;

            case QMessageBox::No:
                log(tr("[user] Usuário solicitou não formatar:").toUtf8());
                ui->comboBox_target_format->setCurrentIndex(0);
                break;

            default:
                //default cancelar porque é o mais seguro em caso de dúvidas
                log(tr("[user] Usuário desistiu da operação de cópia bitwise:").toUtf8());
                ui->comboBox_target_format->setCurrentIndex(0);
                return;
        }
    }
}

void MainWindow::encrypt()
{
    if (!ui->checkBox_aescrypt->isChecked() && !ui->checkBox_OpenSSL->isChecked()){
        return;
    }
    if (!ui->label_passMatch->text().contains(tr("Senha Ok")) && !ui->label_passMatch->text().contains(tr("Visível"))){
        log("[step] Validação para criptografia: emit da senha nao confere. ");
        slotRunMessageBox(tr("Não será possível executar a criptografia. Razão: senha - Criptografia").split("-"));
        return;
    }

    QString fullPathAndFileName = this->common->TARGET_DIR + "/" + this->common->TARGET_FILENAME;
    QString cryptComm;
    if (ui->checkBox_OpenSSL->isChecked()){
        cryptComm = "openssl enc -"+ ui->comboBox_openSSL_type->currentText() + " -pass pass:"+ui->lineEdit_pass->text()+" -salt -in "+fullPathAndFileName;
        //QString comando = "aescrypt -e -p teste /mnt/TESTE/teste.01";
        //QString comando = "openssl enc -aes-256-cbc -pass pass:teste -salt -in /mnt/TESTE/teste.E01";
        ui->checkBox_OpenSSL->setChecked(false);
        ui->stackedWidget_crypt->setCurrentIndex(0);
        ui->checkBox_aescrypt->setEnabled(true);
    }
    else if (ui->checkBox_aescrypt->isChecked()){
        cryptComm = "aescrypt -e -p "+ui->lineEdit_pass->text()+" "+fullPathAndFileName;
        ui->checkBox_aescrypt->setChecked(false);
        ui->checkBox_OpenSSL->setEnabled(true);
    }
    else{
        log("[step] Nenhum método de criptografia escolhido. ");

        return;
    }
    this->crypt->setCryptCommand(cryptComm);
    this->crypt->start();
}

int MainWindow::getAndShowDiskFreeInProgressBarMonitor(QString device)
{
    QRegExp rxPart("\\d");
    QStringList params;
    if (!device.contains(rxPart)){
        if (!QFile::exists(device+"1")){
            if (ui->radioButton_show_dst_data->text().contains(device)){
                ui->progressBar_dst_space->setDisabled(true);
            }
            else{
                ui->progressBar_src_space->setDisabled(true);
            }
            return 0;
        }
        device = device+"1";
    }
    //agora basta montar e ler
    QProcess freeSize;

    // /tmp/sda1
    QString pathToMount = "/tmp/"+device.split("/").last();
    QDir dir;
    if (!dir.exists(pathToMount)){
        dir.mkdir(pathToMount);
    }
    params << device << pathToMount;

    freeSize.start("mount",params);
    if (!freeSize.waitForFinished()){
        params.clear();
        freeSize.close();
        params << pathToMount;
        freeSize.start("umount",params);
        freeSize.waitForFinished(2000);
    }

    freeSize.close();
    params.clear();
    params << "-h";
    freeSize.start("df",params);
    freeSize.waitForFinished(2000);
    QString space = freeSize.readAllStandardOutput().trimmed();
    freeSize.close();

    QString pat = device+"\\s{1,}\\d{1,},?\\d{1,}\\w\\s{1,}\\d{1,},?\\d?\\w\\s{1,}\\d,?\\d{1,}\\w\\s{1,}(\\d{1,}[%,]?\\d?%?)";
    rxPart.setPattern(pat);
    rxPart.indexIn(space);
    QStringList result = rxPart.capturedTexts();

    if (result.length() < 2 || result.last().isEmpty()){
        return 0;
    }

    params.clear();
    params << pathToMount;
    freeSize.start("umount",params);
    if (!freeSize.waitForFinished()){
        return 0;
    }
    freeSize.close();

    return result.last().remove("%").toInt();
}

void MainWindow::slotIOFinished(QString sigVal)
{
    //parar a thread
    this->iostat->setStop(true);
    //limpar dispositivo
    this->iostat->resetDevice();
    this->graph->itemName->setText(tr("Finalizado"));
    log("Processo "+sigVal.toUtf8()+" finalizado ");
    if (this->pbarUndefVal->isRunning()){
        this->pbarUndefVal->setKeepRunning(false); //usando mutex, cuidado se gerar excessão
    }
    if (sigVal == "castConvert"){
        ui->pushButton_wayland_screencast->setText(tr("Converter"));
    }
    if (sigVal == "bitwise"){
        //openssl ou aescrypt
        emit signalCrypt();
    }
    if (sigVal == "crypt"){
        //Se foi escolhida a opção de hotwipe na GUI, então ajusta as coisas e
        //inicia a thread somente ao término da remoção das imagens originais
        if (this->hotWipe){
            this->hotWipe = false;
            this->wiper->setFullWipe(false);
            this->wiper->start();
        }
    }
}

void MainWindow::slotIOStarted(QString sigVal)
{
    bool isImage = true;
    this->iostat->setWriteOrRead(WRITE);
    QMap <QString,QString> sigType;
    sigType.insert("ewfacquiring",tr("Expert Witness Format"));
    sigType.insert("bitwise",tr("Criando Arquivo RAW"));
    sigType.insert("wiper",tr("Wiping"));
    sigType.insert("crypt",tr("Encriptando"));
    sigType.insert("hash",tr("Gerando Digest"));
    sigType.insert("formatar",tr("Formatando"));
    sigType.insert("castConvert",tr("Convertendo Captura de Video"));

    if (sigType.keys().contains(sigVal)){
        this->graph->itemName->setText(sigType[sigVal]);
        if (sigVal == "ewfacquiring"){
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+"EWF ");
        }
        else if (sigVal == "bitwise"){
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+tr("copia "));
            ui->label_time_inicio->setText(dateTimeNowString());
        }
        else if (sigVal == "hash"){
            //split do 'monitor ' resulta em 2 partes e Monitor está sempre rodando
            if (ui->label_executing_tasks->text().split(" ").length() <3){
                     this->iostat->setWriteOrRead(READ);
            }
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+"hash ");

        }
        else if (sigVal == "formatar"){
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+tr("formatar "));
            ui->label_time_inicio->setText(dateTimeNowString());
            //formatar é gpt
            emit buttonMsg(tr("Cancelar"));
            //trava o combobox dos fs e da tools
            ui->comboBox_target_format->setDisabled(true);
            ui->comboBox_bitwiseTools->setDisabled(true);
            //Tira um shot
            this->screenshot->shootScreen("formatar3-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        }
        else if ( sigVal == "wiper"){
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+"wipe ");
            this->graph->itemName->setText("Wiping");
        }
        else if (sigVal == "castConvert"){
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+"castConvert ");
        }
        else if (sigVal == "crypt"){
            ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+"crypt ");
        }
    }
    else{
        this->graph->itemName->setText("...");
    }

    //alimentar dispositivo
    if (isImage){
        this->iostat->setDevice(ui->comboBox_Target_bitwise->currentText());
    }
    //mantenedor do process
    this->iostat->setStop(false);
    //iniciar a thread
    this->iostat->start();
    ui->label_executing_tasks->setText(ui->label_executing_tasks->text()+"IOStat ");
}

void MainWindow::slotHashView()
{
    if (!ui->label_image->text().contains("/")){
        return;
    }

    ui->textBrowserInfo->clear();
    ui->stackedWidget_fromImg->setCurrentIndex(0);
    if (ui->label_image->text().contains("E01")){
        ui->textBrowserInfo->setText(tr("Use o botão EWFInfo e vá ao final da barra de rolagem."));
        return;
    }
    QString filename = ui->label_image->text().split("/").last();
    QString dirname  = ui->label_image->text().remove(filename);
    QString hashFile;

    QDir directory(dirname);
    QStringList entries = directory.entryList();

    QString filenameWithoutExtension = filename.remove(".E01");
    filenameWithoutExtension.remove(".000");
    filenameWithoutExtension.remove(".aa");
    filenameWithoutExtension.remove(".dd");
    filenameWithoutExtension.remove(".iso");

    for (int i=2;i<entries.length();i++){
        if (entries.at(i).contains(filenameWithoutExtension)){
            if (entries.at(i).contains("sha",Qt::CaseInsensitive) || entries.at(i).contains("md5",Qt::CaseInsensitive)){
                hashFile = entries.at(i);
                break;
            }
        }
    }
    if (hashFile.isEmpty()){
        log("[user] Arquivo de imagem solicitado não contém arquivo de hash");
        QMessageBox confirm;
        confirm.setText(tr("Não há arquivo de hash para essa imagem e seus componentes. Deseja criar um com os parâmetros padrão do rodapé?"));
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setButtonText(QMessageBox::Yes, tr("Sim"));
        confirm.setButtonText(QMessageBox::No, tr("Não"));
        confirm.setWindowTitle(tr("Confirma?"));
        int yesNo = confirm.exec();
        if (yesNo == QMessageBox::No){
            slotRunMessageBox(tr("Ok, processo cancelado. - Saindo").split("-"));
            return;
        }
        else{
            ui->checkBox_hash->setChecked(true);
            ui->checkBoxHashEmArquivo->setChecked(true);
            QString digestFile = dirname+filenameWithoutExtension+"."+ui->comboBox_hash->currentText();
            this->fingerprint->setHashType(ui->comboBox_hash->currentText(),digestFile,true);
            this->fingerprint->start();
            return;
        }
    }
    QString fileToOpen = dirname + hashFile;
    QFile file(fileToOpen);
    if (!file.open(QIODevice::Text|QIODevice::ReadOnly)){
        log("[warning] Não foi possível abrir o arquivo de hash para leitura. ");
        slotRunMessageBox(tr("Não foi possivel abrir o arquivo para leitura - Abrir Arquivo Hash").split("-"));
        return;
    }
    QString toTextBrowser = file.readAll();
    ui->stackedWidget_fromImg->setCurrentIndex(0);
    ui->textBrowserInfo->setText(toTextBrowser);
    file.close();
}

void MainWindow::slotImageFeed(QString imgName)
{
    this->imageName << "/var/log/screenshots/"+imgName+".png";
    //se for decidido gerar hash da imagem, colocar aqui
    //QString digestFile = "/var/log/screenshots/"+imgName+"."+ui->comboBox_hash->currentText();
    //this->fingerprint->setHashType(ui->comboBox_hash->currentText(),digestFile,true);
    //this->fingerprint->start();
    this->shotsViewer();
}

void MainWindow::slotInfoAboutSelectedTool(QString tool)
{
    if (ui->comboBox_bitwiseTools->currentIndex() == 4){
        comboboxFeed();
        slotOpenEWFform();
    }
    else{
        ui->frame_relatorio->setVisible(false);
    }
    if (ui->comboBox_bitwiseTools->currentIndex() == 0){
        ui->pushButton_colect->setDisabled(true);
        ui->label_status->setText(tr("Nenhum processo em execução no momento."));

    }
    QRegExp rx("\\((\\w{1,})\\)");
    rx.indexIn(tool);
    tool = rx.capturedTexts().last();
    QMap <QString,QString> info;
    info.insert("dd",tr("Essa ferramenta é pouco informativa mas robusta"));
    info.insert("dc3dd",tr("dd forense bastante informativo"));
    info.insert("dcfldd",tr("dd forense comumente utilizado"));
    if (info.keys().contains(tool)){
        ui->label_status->setText(info[tool]);
        this->bitwiseSelectedTool = tool;
    }
    if (ui->comboBox_bitwiseTools->currentIndex() != 0 && ui->comboBox_Source_bitwise->currentIndex() != 0 && ui->comboBox_Target_bitwise->currentIndex() != 0){
        ui->pushButton_colect->setEnabled(true);
    }

    if (ui->comboBox_bitwiseTools->currentText().contains("dcfldd")){
        ui->frame_split->setEnabled(true);
    }
    else{
        ui->lineEdit_split->setText("1");
        ui->checkBox_split->setChecked(false);
        ui->frame_split->setEnabled(false);
    }
}

void MainWindow::justCheckIfIsReadOnly(QString dev)
{
    if (dev.contains("/dev")){
        dev = dev.remove("/dev/");
    }
    if (dev.length() >3){
        dev = dev.left(3);
    }
    QFile file("/sys/block/"+dev+"/ro");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        log(tr("[warning] Não foi possível ler RW/RO de /sys/block/").toUtf8()+dev.toUtf8()+"/ro: ");
        QMessageBox box;
        box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        box.setWindowTitle(tr("Falha na leitura"));
        box.setText(tr("Não foi possível confirmar o estado da mídia (RO ou RW)."));
        box.exec();
        return;
    }

    QString status = file.readLine();
    file.close();

    if (status.toInt() == 1){
        this->setDeviceToReadOnly(dev,false);
    }
}

void MainWindow::labelsFeed(QString srcOrDst)
{
    QByteArray device = "/dev/";
    if (srcOrDst == "src"){
        device += ui->comboBox_Source_bitwise->currentText().toUtf8();
    }
    else if (srcOrDst == "dst"){
        device += ui->comboBox_Target_bitwise->currentText().toUtf8();
    }
    this->common->hdparmExecutionAndResult(device);
    QMap <QString,QString> hdparmResult;
    hdparmResult = this->common->getHDInfo();

    if (srcOrDst == "src"){
        ui->pushButton_serial_info->setText(hdparmResult["Serial"]);
        ui->pushButton_model_info->setText(hdparmResult["Modelo"]);
        if (ui->comboBox_Source_bitwise->currentIndex() >0 && !ui->comboBox_Source_bitwise->currentText().isEmpty()){
            ui->label_serial_monitor_src->setText(ui->pushButton_serial_info->text());
            this->monitorSettings("src");
        }
        return;
    }
    ui->pushButton_serial2_info->setText(hdparmResult["Serial"]);
    ui->pushButton_model2_info->setText(hdparmResult["Modelo"]);
    if (ui->comboBox_Target_bitwise->currentIndex() >0 && !ui->comboBox_Source_bitwise->currentText().isEmpty()){
        this->monitorSettings("dst");
    }
}

void MainWindow::log(QByteArray logType)
{
    if (!ui->checkBox_gerarLog->isChecked()){
        return;
    }
    QFile file("/var/log/ForensicMessages.log");
    if (!file.open(QIODevice::Text|QIODevice::Append)){
        slotRunMessageBox(QString("Arquivo de log nao pode ser gerado. Permissoes no destino? - Erro ao tentar gravar log").split("-"));
        return;
    }

    QString finalMsg = this->makeTimeStr();
    finalMsg = logType + finalMsg + "\n";

    file.write(finalMsg.toUtf8());
    file.close();

    if (ui->radioButton_on->isChecked()){
        ui->textBrowserInfo->append(finalMsg);
    }
}

void MainWindow::slotInvert(bool onOff)
{
    if (this->screencast->isRunning()){
        ui->progressBar_screencast->setInvertedAppearance(onOff);
    }
    else if (this->crypt->isRunning()){
        ui->progressBar_crypting->setInvertedAppearance(onOff);
    }
}

void MainWindow::slotLangFromIniFile()
{
    if (this->lang.isEmpty()){
        this->lang="pt";
    }
    if (lang.contains("en")){
    ui->radioButton_en->setChecked(true);
    }
    else if(lang.contains("es")){
        ui->radioButton_es->setChecked(true);
    }
    else{
        ui->radioButton_pt->setChecked(true);
    }
}

void MainWindow::slotLogHandler()
{
    QMessageBox msg;
    msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    if (!QFile::exists(this->common->filenameToLogPath)){
        msg.setText(tr("Arquivo de log não existente."));
        msg.exec();
        return;
    }

    if (ui->radioButton_openLog->isChecked()){
        QFile openLog(this->common->filenameToLogPath);
        if (!openLog.open(QIODevice::Text|QIODevice::ReadOnly)){
            msg.setText(tr("Não foi possível abrir o arquivo de log"));
            msg.exec();
            return;
        }
        QString content = openLog.readAll();
        openLog.close();

        popup pop;
        pop.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        pop.setWindowTitle(tr("Informações de log"));
        pop.setGroupBoxTitle(this->common->filenameToLogPath);
        pop.setTextEditText(content);
        pop.exec();
    }
    else if (ui->radioButton_removeLog->isChecked()){
        QFile openLog(this->common->filenameToLogPath);
        if (!openLog.remove()){
            msg.setText(tr("Não foi possível remover o arquivo de log"));
        }
        else{
            msg.setText(tr("Arquivo de log removido com sucesso"));
        }
        msg.exec();
    }
    else if (ui->radioButton_copyLog->isChecked()){
        copyLogsTo();
    }
}

QString MainWindow::makeTimeStr()
{
    QString dateStr =  ui->dateTimeEdit->date().toString().split(" ").at(2) + " " + ui->dateTimeEdit->date().toString().split(" ").at(1) + " " + ui->dateTimeEdit->date().toString().split(" ").at(3);
    dateStr += " " + ui->dateTimeEdit->time().toString();
    return dateStr;
}

int MainWindow::makeRawFile()
{
    QString fileName;
    if (ui->comboBox_bitwiseTools->currentIndex() == 0 || ui->comboBox_Source_bitwise->currentIndex() == 0 || ui->comboBox_Target_bitwise == 0){
        QMessageBox msg;
        msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        msg.setWindowTitle(tr("Parâmetros incompletos"));
        msg.setText(tr("Não há parâmetros suficientes para a operação"));
        msg.exec();
        return STOPPED;
    }

    QString pre_of = " ";
    if (ui->comboBox_bitwiseTools->currentText().contains("dcfldd")){
        if (ui->checkBox_split->isChecked()){
            pre_of = " split="+ui->lineEdit_split->text()+"G splitformat=000 ";
        }
    }

    QString commandConcat;
    QRegExp rx("\\d");
    QString src = ui->comboBox_Source_bitwise->currentText();
    QString dst = ui->comboBox_Target_bitwise->currentText();

    if (dst.contains(rx)){
        ui->label_status->setText(tr("Retendo controles da interface para aguardar montagem..."));
        emit signalMountOrUmontAndCheckConditionsOf(dst,this->common->TARGET_DIR);

        QString is_mounted;
        bool wait = true;

        QProcess mounting;
        int counter = 0;
        while (wait){
            mounting.start("mount");
            if (!mounting.waitForFinished()){
                is_mounted = " ";
            }
            else{
                is_mounted = mounting.readAllStandardOutput();
            }
            mounting.close();

            if (is_mounted.contains(dst)){
                wait = false;
                ui->label_status->setText(tr("Seguindo..."));
            }

            counter += 1;
            if (counter == 5){
                wait = false;
                ui->label_status->setText(tr("NAO FOI POSSIVEL MONTAR A MIDIA DE DESTINO"));
                ui->label_status->setStyleSheet("color:#FA0000");
                return STOPPED;
            }
        }

        bool ok;
        fileName = QInputDialog::getText(this, tr("Defina um nome de arquivo"),
                                                 tr("Nome para o arquivo:"), QLineEdit::Normal,
                                                 ui->label_serial_monitor_src->text().remove(" ->")+QString(".dd").replace(" ","_"), &ok);
        if (!ok || fileName.isEmpty()){
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msg.setWindowTitle(tr("Cópia de baixo nível"));
            msg.setText(tr("Não é possível seguir em caso de cancelamento ou nome vazio."));
            msg.exec();
            return STOPPED;
        }
        this->common->TARGET_FILENAME = fileName;

        if (!targetIsWriteable(this->common->TARGET_DIR+"/"+fileName,"/dev/"+dst)){
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msg.setWindowTitle(tr("Escrita não permitida"));
            msg.setText(tr("Não é possível escrever no destino. Saindo."));
            msg.exec();
            if (ui->pushButton_colect->text().contains("Cancelar")){
                this->slotPushButtonColect();
            }
            return STOPPED;
        }

        commandConcat = this->bitwiseSelectedTool + " if=/dev/" + src + pre_of + " of=" + this->common->TARGET_DIR + "/" + fileName;
        this->ImageNameGeneratedInBitwiseCopy = this->common->TARGET_DIR + "/" + fileName;
    }
    //nao havera montagem se a copia for de midia-para-midia
    else if (!dst.contains(rx) && ui->radioButton_bw_disc2disc->isChecked()){
        commandConcat = this->bitwiseSelectedTool + " if=/dev/" + src + pre_of + " of=/dev/" + dst;
    }
    else{
        QMessageBox alert;
        alert.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        alert.setWindowTitle(tr("Opções incorretas"));
        alert.setText(tr("Você escolheu gerar um arquivo RAW, mas escolheu apenas dispositivo como destino."));
        alert.setDetailedText(tr("Se a intenção é copiar origem diretamente ao destino, então selecione essa opção no\
                              menu Geração de Imagem. Se realmente pretende copiar a origem a arquivo, selecione a\
                              partição na caixa de Destino. Ainda, se não houver partição na mídia de destino, use\
                              a opção Formatar"));
        alert.exec();
        return STOPPED;
    }

    //EWF
    if (ui->comboBox_bitwiseTools->currentText().contains("EWF")){
        //avisar que a cópia é origem a arquivo
        if (ui->radioButton_bw_disc2disc->isChecked()){
            QMessageBox box;
            box.setWindowTitle(tr("Cópia Origem a Destino"));
            box.setText(tr("No momento somente Origem a Arquivo é possível utilizando o formato EWF."));
            box.setStandardButtons(QMessageBox::Ok);
            box.exec();
            return STOPPED;
        }

        //montar comando
        QString commandToEWF = mountCommandToEWFthread(fileName);

        //chamar metodo da thread pra alimentar o comando
        this->ewfAcquiring->setParamsToEWFcopy(commandToEWF);

        //iniciar
        this->ewfAcquiring->start();

        //sair do metodo
        return EXITOK;
    }
    this->bitwise->slotSetCommandToBitWiseCopy(commandConcat);
    this->bitwise->start();
    return EXITOK;
}

void MainWindow::slotMount(QString target)
{
    if (ui->comboBox_srcImgToRead->currentIndex() == 0){
        return;
    }

    QFile file("/proc/mounts");
    if(!file.open(QIODevice::Text|QIODevice::ReadOnly)){
        log("[warning] Não foi possivel acessar informações de dispositivos disponíveis");
        slotRunMessageBox(QString("Não foi possível acessar informações de disponibilidade atual. - Acesso privado").split("-"));
        return;
    }

    QString contents = file.readAll();
    file.close();
    target = "/dev/"+target;
    QRegExp rx(target);
    if (rx.indexIn(contents) > -1){
        return;
    }
    //Se a particao selecionada nao esta montada, verificar se o diretorio esta disponivel
    rx.setPattern("/media/images");
    if (rx.indexIn(contents) > -1){
        QString result = rx.capturedTexts().last();
        QString dirMounted = result.split(" ").at(0);
        //nao chega aqui se estiver montado porque retorna acima, mas se o codigo for modificado, evita bug.
        if (result.split(" ").at(0).contains(target)){
            return;
        }
        QMessageBox confirm;
        confirm.setText(tr("Diretório de destino atualmente em uso pelo dispositivo ")+dirMounted+tr(". Devo disponibilizá-lo para o novo dispositivo?"));
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        confirm.setButtonText(QMessageBox::Yes, tr("Sim"));
        confirm.setButtonText(QMessageBox::Cancel, tr("Cancelar"));
        confirm.setWindowTitle(tr("Confirma?"));
        int yesNo = confirm.exec();
        if (yesNo == QMessageBox::Cancel){
            return;
        }
    }

    QProcess umount;
    umount.start(QString("umount /media/images"));
    if (umount.waitForFinished(3000)){
        slotRunMessageBox(tr("Não foi possivel desfazer a conexão com o dispositivo em uso. Saindo. - Acesso ao Dispositivo").split("-"));
        log("[warning] *umount* Não foi possivel desfazer a conexão com o dispositivo em uso. ");
        umount.close();
        return;
    }

    QProcess mount;
    QString mountComm = "mount " + target + " /media/images";
    log("[step] Montando diretório: [LABEL:mount] ");
    mount.start(mountComm);
    mount.waitForFinished(2000);
    mount.close();
}

void MainWindow::slotMsgFromWiper(QString msg){
    if (msg.contains(tr("Processo finalizado pelo usuário."))){
        ui->label_status->setText(tr("Processo finalizado pelo usuário."));
        return;
    }
    if (!msg.contains(QRegExp("\\d"))){
        //uma garantia extra para eventual falha na traducao acima em um possivel novo idioma
        return;
    }
    QStringList msgs;
    msgs = msg.split("|");
    if (msgs.at(0).length() >4){
        QRegExp rx("(\\d{1,})");
        QString percent;
        percent = msgs.at(1);
        int pos = rx.indexIn(percent);
        if (pos < 0){
            return;
        }
        percent = rx.capturedTexts().last();
        ui->label_status->setText(msgs.at(0)+tr(" limpos. Velocidade: ")+msgs.at(2));
        if (percent.length() > 0){
            emit signalPercentOfWipe(percent.toInt());
        }
    }
}

void MainWindow::slotPartitionsView()
{
    ui->textBrowserInfo->clear();
    if (this->common->sourceStrings.isEmpty()){
        log("[warning] A variável de origem se encontra vazia. ");
        return;
    }
    QFile file(this->common->sourceStrings);
    if (file.exists()){
        ui->stackedWidget_fromImg->setCurrentIndex(0);
        ui->textBrowserInfo->setText(this->common->PartedAndFdisk(this->common->sourceStrings));
        return;
    }
    slotRunMessageBox(QString("O arquivo não foi encontrado (você substituiu a mídia?) - Arquivo Não Encontrado").split("-"));
}

void MainWindow::slotPass(QString txt)
{
    if (ui->checkBox_showPasswd->isChecked()){
        ui->label_passMatch->setText(tr("Visível"));
        ui->label_passMatch->setStyleSheet("color: rgb(0, 132, 203);");
        return;
    }
    if (txt != ui->lineEdit_passConfirm->text()){
        ui->label_passMatch->setText(tr("Senhas não casam"));
        ui->label_passMatch->setStyleSheet("color: rgb(255, 153, 51);");
    }
    else if (ui->lineEdit_pass->text().isEmpty()){
        ui->label_passMatch->setText(tr("Senha vazia"));
        ui->label_passMatch->setStyleSheet("color: rgb(255, 153, 51);");
    }
    else{
        ui->label_passMatch->setText(tr("Senha Ok"));
        ui->label_passMatch->setStyleSheet("color: rgb(0, 132, 203);");
    }
}

void MainWindow::slotPassConfirm(QString txt)
{
    if (ui->lineEdit_pass->text() != txt){
        ui->label_passMatch->setText(tr("Senhas não casam"));
        ui->label_passMatch->setStyleSheet("color: rgb(255, 153, 51);");
    }
    else if (ui->lineEdit_pass->text().isEmpty()){
        ui->label_passMatch->setText(tr("Senha vazia"));
        ui->label_passMatch->setStyleSheet("color: rgb(255, 153, 51);");
    }
    else{
        ui->label_passMatch->setText(tr("Senha Ok"));
        ui->label_passMatch->setStyleSheet("color: rgb(0, 132, 203);");
    }
}

void MainWindow::slotProgressBarCPU(QString cores, int usage)
{
    //int cpuVal = 100 4 8
    QString xOf = "1/";
    if (usage > -1){
        int cpuVal = usage / cores.toInt();
        if (cpuVal >24 && cpuVal < 50){
            xOf = "2/";
        }
        if (cpuVal > 50 && cpuVal<75){
            xOf = "3/";
        }
        else if (cpuVal >=75){
            xOf = "4/";
        }
        ui->label_cpus_number->setText(tr("Núcleos: ")+xOf+cores);
        ui->progressBar_cpu->setValue(cpuVal);

    }
    else{
        ui->progressBar_cpu->setValue(100);
    }
}

void MainWindow::slotOpenEWFform()
{
    connect(this->ewf,SIGNAL(stringsFromForm(QString)),this,SLOT(slotReceivedFromForm(QString)));
    this->ewf->exec();
    ui->frame_relatorio->setVisible(true);
    disconnect(this->ewf,SIGNAL(stringsFromForm(QString)),this,SLOT(slotReceivedFromForm(QString)));
}

void MainWindow::slotPartitionClicked(QListWidgetItem *item)
{

    QString itemClicked = item->text();
    itemClicked = "/dev/" + itemClicked;
    log(tr("[user] Item de disco clicado: ").toUtf8() + itemClicked.toUtf8());

    QRegExp rx("\\d+");
    QString dev2hdparm = itemClicked.remove(rx);

    QStringList gdiskParam;
    gdiskParam << "-l" << dev2hdparm;

    //this->generalMsgs.clear();

    QProcess gdisk;
    gdisk.start("gdisk",gdiskParam);
    if (!gdisk.waitForFinished(2000)){
        gdisk.close();
        log(tr("[warning] Gdisk nao retornou o esperado. Passando ao fdisk: ").toUtf8());
        gdisk.start("fdisk",gdiskParam);
        if (!gdisk.waitForFinished(2000)){
            log(tr("[warning] Fdisk tambem nao foi capaz de retorno valido: ").toUtf8());
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msg.setText(tr("Ocorreu um erro ao tentar ler a mídia."));
            msg.setWindowTitle(tr("Identificação da Mídia."));
            msg.exec();
            return;
        }
    }
    log(tr("[step] Coleta de info do HD: OK - ").toUtf8());

    QString firstLine = QString(gdisk.readAll());
    gdisk.close();


    QString msgToPop = tr("<b>INFORMAÇÕES SOBRE PARTICIONAMENTO:</b><br>");
    msgToPop += firstLine.replace("\n","<br>");
    msgToPop += tr("<br><b>INFORMAÇÕES SOBRE SERIAL E MODELO:</b><br>");

    QProcess hdparm;
    QStringList hdparmParam;
    hdparmParam << "-i" << dev2hdparm;
    hdparm.start("hdparm", hdparmParam);
    if (!hdparm.waitForFinished()){
        QMessageBox msg;
        msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        msg.setText(tr("Ocorreu um erro ao tentar ler a mídia."));
        msg.setWindowTitle(tr("Identificação da Mídia."));
        msg.exec();
        ui->tabWidget->setCurrentIndex(TABMON);
        return;
    }

    firstLine = QString(hdparm.readAll());
    msgToPop += firstLine.replace("\n","<br>");

    popup pop;
    pop.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    pop.setWindowTitle(tr("Informações sobre a Mídia"));
    pop.setGroupBoxTitle(itemClicked);
    pop.setTextEditText(msgToPop);
    pop.exec();

    hdparm.close();
}

void MainWindow::slotPushButtonColect()
{
    ui->label_status->setStyleSheet("color:#000000");
    ui->checkBox_readOnly_bw->setDisabled(true);
    if (ui->pushButton_colect->text().contains(tr("Coletar"))){
        if (ui->label_time_final->text().contains(tr("Cancelado"))){
            ui->label_time_final->setText(tr("Aguardando informação..."));
        }
        ui->label_time_inicio->setText(dateTimeNowString());
        this->resetGraph();
        if (ui->tabWidget->currentIndex() == STRTAB){
            this->screenshot->shootScreen("coletar-strtab",ui->checkBox_screenshot->isChecked());

            QTimer::singleShot(this->shotInterval,this->screenshot,SLOT(shootScreen("coletar-nada_a_coletar",ui->checkBox_screenshot->isChecked())));
            QMessageBox msgbox;
            msgbox.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msgbox.setText(tr("Nenhuma opção foi selecionada para fazer a coleta."));
            msgbox.setWindowTitle(tr("Nada a coletar"));
            msgbox.setDetailedText(tr("Nessa aba é necessário selecionar a opção 'gerar hash'.\
                                   Em caso de dúvidas, consulte a documentação."));
            msgbox.exec();
            ui->checkBox_readOnly_bw->setDisabled(true);
        }
        else if (ui->tabWidget->currentIndex() == IMGTAB){
            this->screenshot->shootScreen("coletar-imgtab", ui->checkBox_screenshot->isChecked());
            ui->checkBox_readOnly_bw->setDisabled(true);
            ui->comboBox_bitwiseTools->setDisabled(true);
            emit buttonMsg(tr("Cancelar"));
            int statGoAhead = this->makeRawFile();
            if (statGoAhead == STOPPED){
                ui->pushButton_colect->setText(tr("Coletar"));
                return;
            }
        }
    }
    else if (ui->pushButton_colect->text().contains("Cancel")){
        ui->checkBox_readOnly_bw->setEnabled(true);
        emit buttonMsg(tr("Coletar"));
        this->cancelAllRunningProcess();
        resetParams();
        ui->label_time_final->setText(tr("Cancelado pelo Usuário"));
        if (ui->checkBox_screenshot->isChecked()){
            QString localFromClick = QString::number(ui->tabWidget->currentIndex());
            localFromClick = "cancelado-index_" + localFromClick + this->dateTimeNowString().replace(" ","_");
            this->screenshot->shootScreen(localFromClick.toUtf8(),true);
            QThread::msleep(250);
            ui->tabWidget->setCurrentIndex(2);
            QThread::msleep(250);
            localFromClick = "cancelamento-monitor_info" + this->dateTimeNowString().replace(" ","_");
            this->screenshot->shootScreen(localFromClick,true);
        }
    }
}

void MainWindow::slotPushButtonEncrypt()
{
    if (!ui->checkBox_OpenSSL->isChecked() && !ui->checkBox_aescrypt->isChecked()){
        log("[user] Tentativa de criptografia sem informações suficientes ");
        slotRunMessageBox(tr("Escolha na aba ao lado o tipo de criptografia primeiro (e não se esqueça de escolher uma senha - Faltam informações").split("-"));
        ui->toolBox->setCurrentIndex(2);
        QThread::msleep(100);
        ui->tabWidget->setCurrentIndex(0);
        return;
    }
    if (ui->lineEdit_pass->text().isEmpty()){
        log("[user] Tentativa de criptografia sem informações suficientes ");
        slotRunMessageBox(tr("Você esqueceu de escolher uma senha - Faltam informações").split("-"));
        return;
    }
    if (!ui->checkBox_showPasswd->isChecked() && ui->lineEdit_passConfirm->text().isEmpty()){
        log("[user] Tentativa de criptografia sem informações suficientes ");
        slotRunMessageBox(tr("Você não confirmou a senha ou não marcou a opção 'Exibir Senha' - Faltam informações").split("-"));
        return;
    }
    if (!ui->label_passMatch->text().contains("Senha Ok") && !ui->checkBox_showPasswd->isChecked()){
        log("[user] Tentativa de criptografia com senhas descasadas ");
        slotRunMessageBox(tr("As senhas não casam (vide mensagem abaixo das caixas de senha) - Informações incorretas").split("-"));
        return;
    }
    ui->stackedWidget_fromImg->setCurrentIndex(0);
    ui->textBrowserInfo->setText(tr("Processo de criptografia iniciado."));
    this->common->TARGET_FILENAME = ui->label_image->text().split("/").last();
    emit signalCrypt();
}

void MainWindow::slotPushButton_wayland_convert_txt(QString text)
{
    ui->pushButton_wayland_screencast->setText(text);
}

void MainWindow::slotRadioButtonHashMd5(bool ck)
{
    ui->radio_sha->setChecked(!ck);
}

void MainWindow::slotRadioButtonHashSha(bool ck)
{
    ui->radio_md5->setChecked(!ck);
}

void MainWindow::slotReceivedFromForm(QString msg)
{
    QStringList msgs = msg.split("|");
    if (msgs.length() <5){
        log(tr("[warning] Os textos para utilização do EWF não atende ao requisito de campos. ").toUtf8());
        return;
    }
    ui->label_caseNumberInfo->setText(msgs.at(0));
    ui->label_evidenceInfo->setText(msgs.at(1));
    ui->label_examinerInfo->setText(msgs.at(2));
    ui->label_description->setText(msgs.at(3));
    ui->label_notes->setText(msgs.at(4));
}

void MainWindow::slotRemoveFile()
{
    QMessageBox msg;
    msg.setText(tr("Deseja realmente remover esse arquivo e suas partes relacionadas (hash, partes,etc)?"));
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setButtonText(QMessageBox::Yes, tr("Sim"));
    msg.setButtonText(QMessageBox::No, tr("Não"));
    int yesNo = msg.exec();

    if (yesNo == QMessageBox::No){
        ui->textBrowserInfo->setText("Operação de remoção cancelada pelo usuário");
        this->screenshot->shootScreen("remocao_de_arquivos-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        return;
    }

    QString fullPath = ui->label_image->text();
    QString filename = fullPath.split("/").last();
    QString dirname  = fullPath.remove(filename);
    QDir directory(dirname);
    QStringList entries = directory.entryList();

    QString filenameWithoutExtension = filename.remove(".dd");
    filenameWithoutExtension.remove(".000");
    filenameWithoutExtension.remove(".aa");
    filenameWithoutExtension.remove(QRegExp("\\.sha\\d{1,}",Qt::CaseInsensitive));
    filenameWithoutExtension.remove(QRegExp("\\.md\\d"));

    for (int i=2;i<entries.length();i++){
        if (entries.at(i).contains(filenameWithoutExtension)){
            ui->textBrowserInfo->append("Removendo arquivo "+entries.at(i));
            QFile::remove(dirname+entries.at(i));
        }
    }
    ui->textBrowserInfo->append("Feito.");

}

void MainWindow::slotRereadDevicesTab()
{
    ui->listWidget_medias->clear();
    QStringList feed = this->common->connectedDevices(false,"none");
    feed.removeAt(0);
    ui->listWidget_medias->addItems(feed);
    this->screenshot->shootScreen("releitura_de_midias"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
}

void MainWindow::slotRunMessageBox(QStringList msgs)
{
    QMessageBox msgbox;
    msgbox.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");

    //abaixo, garante que mesmo q a stringlist seja menor que 3 não haverá erros
    msgs << " " << " ";

    //posicao 0: mensagem
    if (msgs.at(0) == " "){
        return;
    }
    msgbox.setText(msgs.at(0));

    //posicao 1: titulo da janela
    if (msgs.at(1) != " "){
        msgbox.setWindowTitle(msgs.at(1));
    }
    //posicao 2: detalhes da operacao
    if (msgs.at(2) != " "){
        msgbox.setDetailedText(msgs.at(2));
    }
    msgbox.exec();
}

void MainWindow::slotSaveLog(QString target)
{
    QMessageBox confirm;
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setButtonText(QMessageBox::Yes, tr("Sim"));
    confirm.setButtonText(QMessageBox::Cancel, tr("Cancelar"));
    confirm.setWindowTitle(tr("Confirma?"));
    confirm.setInformativeText(tr("Você escolheu salvar os logs em ")+target+tr(". Confirmar escolha?"));
    int yesNo = confirm.exec();
    if (yesNo == QMessageBox::Cancel){
        QMessageBox * msgBox = new QMessageBox();
        QString msg = tr("[user] Salvamento dos logs no dispositivo ")+target+tr(" cancelado pelo usuário. ");
        msgBox->setText(msg);
        log(msg.toUtf8());
        msgBox->setStandardButtons(QMessageBox::Ok);

        QTimer::singleShot(3000, msgBox, SLOT(close()));
        msgBox->exec();
        delete msgBox;
        return;
    }
    QProcess mount;
    mount.start(QString("umount /mnt"));
    mount.waitForFinished(3000);
    mount.close();

    mount.start(QString("mount"));
    mount.waitForFinished(3000);
    QString result = mount.readAllStandardOutput();
    mount.close();

    if (result.contains("/mnt")){
        log(tr("[warning] Aparentemente o diretório de destino dos logs se encontra ocupado. ").toUtf8());
        QMessageBox msgbox;
        msgbox.setWindowTitle(tr("Diretório ocupado"));
        msgbox.setText(tr("Aparentemente o diretório de destino dos logs se encontra ocupado. Tente uma vez mais, se desejar."));
        msgbox.exec();
        return;
    }

    QFile sdx("/dev/"+target+"1");
    if (!sdx.exists()){
        log(tr("[step] Não existe partição na midia de destino dos logs. ").toUtf8());
        QMessageBox msgbox;
        msgbox.setWindowTitle(tr("Sem partição"));
        msgbox.setText(tr("O dispositivo que você indicou como destino dos logs deve conter 1 partição formatada."));
        msgbox.exec();
        return;
    }

    QString mountCommand = "mount /dev/"+target+"1 /mnt";
    mount.start(QString(mountCommand));
    if (mount.waitForFinished(3000)){
        QString m = tr("Não foi possível montar o dispositivo selecionado (")+target+")";
        log(m.toUtf8());
        QMessageBox msgbox;
        msgbox.setWindowTitle(tr("Dispositivo inacessível"));
        msgbox.setText(tr("Não foi possível acessar o dispositivo. Tente uma vez mais, ou ainda, opte por formatá-lo através da interface do programa ou externamente e repita o processo de salvamento dos logs."));
        msgbox.exec();
        mount.close();
        return;
    }
    //teste de escrita
    QFile canWrite("/mnt/test.txt");
    if (!canWrite.open(QIODevice::Text|QIODevice::WriteOnly)){
        QMessageBox msgbox;
        msgbox.setWindowTitle(tr("Escrita"));
        msgbox.setText(tr("Não é possível escrever no destino dos logs. Verifique se há alguma trava no destino ou se há algum problema com a mídia."));
        msgbox.exec();
        canWrite.close();
        log(tr("Não foi possível escrever no destino do log. Operação cancelada").toUtf8());
        return;
    }
    canWrite.close();
    if (!canWrite.remove()){
        log(tr("Arquivo de teste de escrita não pôde ser removido da mídia de destino do log. ").toUtf8());
    }
    //copia
    QStringList directories;
    directories << "/var/log/screenshots" << "/var/log/screencasts" << "/var/log/ewf";
    QSettings settings("/home/djames/dwatson.ini",QSettings::IniFormat);
    QString targetDir = settings.value("dwatson.general/copyLogsTo","/mnt").toString();
    this->copyFilesAndDirs(directories,targetDir);
    mountCommand = "umount /mnt";
    mount.start(mountCommand);
    mount.waitForFinished(3000);
    mount.close();
    directories.clear();
    directories << tr("Processo finalizado sem erros aparentes. Verifique o conteúdo.") << ("Processo Finalizado");
    slotRunMessageBox(directories);
}

void MainWindow::slotSelectImage4read()
{
    this->files = QFileDialog::getOpenFileNames(this,
                                                     trUtf8("Selecione a imagem a visualizar"),
                                                     this->common->getEnv("imagesPlace"),
                                                     "*.dd *.dump *.img *.iso *.raw *.E01 *.000");

    if (this->files.length() < 1 && !ui->label_image->text().contains(tr("Clique"))){
        this->files << ui->label_image->text();
    }
    if (this->files.length() > 0){
        if (this->files.at(0).contains("/")){
            this->enableFooterWidgets(true);
            ui->groupBox_imgInfo->setEnabled(true);
            ui->label_image->setText(files.at(0));
            this->common->sourceStrings = this->files.at(0);
        }
    }
}

void MainWindow::slotSetMemoryValues(int total, int used, int cache)
{
    //total = 100% ; used = ?%
    int usedWithoutCache = used-cache;
    int percent = (usedWithoutCache*100)/total;
    percent = percent > -1 ? percent : 0;
    ui->progressBar_mem->setValue(percent);
    ui->label_mem_total->setText("Total  : "+QString::number(total));
    ui->label_mem_used->setText("Usado: "+QString::number(used));
    ui->label_mem_cache->setText("Cache: "+QString::number(cache));
}

void MainWindow::checkout(int tab)
{
    QStringList tasks;
    QString imageName;
    //se for a tab de strings, pega o nome do label
    if (tab == STRTAB){
        imageName = ui->label_image->text();
    }
    else if (tab == IMGTAB){
        //--------------------------------------------------------------------------------
        // ETAPA 1: COLETAR O NOME PARA O ARQUIVO DE HASH, SEM EXTENSAO
        //--------------------------------------------------------------------------------

        //se chegou até esse ponto, é porque o botão de coleta estava liberado, então validar apenas o nome a utilizar

        if ((!ui->pushButton_serial_info->text().isEmpty() && !ui->pushButton_model_info->text().isEmpty())){
            if (this->ImageNameGeneratedInBitwiseCopy.isEmpty()){
                //jamais deverá ser vazia. nesse caso, log e saida
                log(tr("[warning] O nome da imagem gerada na cópia não foi armazenada na variável global: ").toUtf8());
                return;
            }
            imageName = this->ImageNameGeneratedInBitwiseCopy;
            //Pega da variável privada e a limpa após isso
            this->ImageNameGeneratedInBitwiseCopy.clear();

            if (imageName.isEmpty()){
                QMessageBox box;
                box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
                box.setText(tr("Não é possível iniciar a cópia se houver cancelamento ou nome de arquivo vazio."));
                box.setWindowTitle(tr("Cópia não iniciada"));
                box.exec();
                return;
            }
            this->files << imageName.replace(" ","_");
        }
        //Não dever entrar nesse else jamais considerando que chegou nesse método através da cópia bitwise
        else{
            QMessageBox box;
            box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            QString srcOrdst;
            if (ui->pushButton_serial_info->text().isEmpty()){
                srcOrdst = tr("origem");
            }
            else{
                srcOrdst = tr("destino");
            }
            box.setText(tr("Midia de ")+srcOrdst.toUpper()+(" não informada?"));
            box.setWindowTitle(tr("Midia não informada"));
            box.setDetailedText(tr("O campo de serial da mídia de ")+srcOrdst.toUpper()+tr(" não apresenta informações."));
            box.exec();
            log(tr("[warning] Serial de "+srcOrdst.toUpper().toUtf8()+" não foi exibido na janela de cópia bitwise: ").toUtf8());
            return;
        }
    }

    //---------------------
    // ETAPA 2: GERAR HASH
    //---------------------
    if (ui->checkBoxHashEmArquivo->isChecked() && !ui->checkBox_hash->isChecked()){
        QMessageBox box;
        box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        box.setText(tr("Marque a opção 'Gerar hash' e escolha um tipo primeiro."));
        box.exec();
        this->log(tr("[warning] Faltou selecionar o tipo de hash: ").toUtf8());
        return;
    }

    QString hash;
    QStringList params;
    if (ui->checkBox_hash->isChecked() && !ui->comboBox_bitwiseTools->currentText().contains("EWF")){
        //MD5
        if (ui->radio_md5->isChecked()){
            hash  = "md5";
            log(tr("[step] Hash:md5 - ").toUtf8());
        }
        //SHA
        else if (ui->radio_sha->isChecked()){
            hash = ui->comboBox_hash->currentText();
            log(tr("[step] Hash:").toUtf8() + hash.toUtf8() + " - ");
        }
        else{
            log(tr("[warning] sha e md5 não marcados: ").toUtf8());
            return;
        }
        //logName guardará o nome do arquivo a salvar junto à imagem dd. Esse nome é utilizado pelo QFile
        QString logName;
        QString fullPathToImageAndImageName = imageName; //UNKOWN_DEVICE
        logName = imageName.remove(".dd") + "." + hash;
        //resulta em algo como "modelo-xyx-serial-34643333.sha1 modelo-xyx-serial-34643333.dd"

        /* As próximas 15 linhas envolvem um trabalho maior porque esse método é chamado em 3 condições diferentes:
         * 1 - Geração do hash a partir da imagem selecionada
         * 2 - Geração do hash a partir de uma imagem única
         * 3 - Geração do hash a partir de uma imagem dividida em partes
         * Quando a imagem é dividida pelo dcfldd, recebe o sufixo .000 e quando não é dividida, sufixo .dd
        */
        QString fileName = fullPathToImageAndImageName.split("/").last();
        QString dirname  = fullPathToImageAndImageName.remove(fileName);
        QDir directory(dirname);
        QStringList entries = directory.entryList();
        QString fileNameSplited = fileName + ".000";

        if (entries.contains(fileName) || entries.contains(fileNameSplited)){
            for (int i=0;i<entries.length();i++){
                if (entries.at(i).contains(fileName) || entries.at(i).contains(fileNameSplited)){
                    fullPathToImageAndImageName = dirname+entries.at(i);
                    params << fullPathToImageAndImageName;
                    break;
                }
            }
        }
        else{
            this->files.clear();
            this->files = QFileDialog::getOpenFileNames(this,
                                                             trUtf8("Selecione a imagem para o cálculo hash"),
                                                             this->common->getEnv("imagesPlace"),
                                                             "*.dd *.dump *.img *.iso *.raw *.000 *.aa");
            if (this->files.isEmpty()){
                QByteArray msg = "Não será possível seguir com a geração de hash sem escolher o arquivo inicial. - Nome de arquivo vazio";
                slotRunMessageBox(tr(msg).split("-"));
                return;
            }
            logName = this->files.at(0);
            QString extension = "." + hash;
            logName = logName.replace(QRegExp("\\.dd\\.?\\d{0,}"),extension);
        }

        if (this->files.at(0).length() < 5){
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msg.setText(tr("Não será possível prosseguir sem a indicação do arquivo de imagem a gerar o cálculo hash."));
            msg.exec();
            return;
        }

        bool logInFile = true;
        if (!ui->checkBoxHashEmArquivo->isChecked()){
            logInFile = false;
            log(tr("[step] Gravar hash em arquivo: Não  - ").toUtf8());
        }
        //TODO: acima limpa a variavel logName. Nao esta errado?
        //3 -  chamar o método público de fingerprint setcommand passando a qstring
        this->fingerprint->setHashType(ui->comboBox_hash->currentText(),logName,logInFile);

        this->fingerprint->start();
        logName.clear();
    }
}

void MainWindow::checkTasks()
{
    /*Esse método faz um repasse pelas tarefas pra ver se estão rodando ainda.
     * Os valores do label atual são verificados e as tarefas em execução são passados
     * para uma nova variável label. Depois, o label de tarefas é sobrescrito por essa segunda
     * variável que conterá apenas as tarefas que ainda estão em execução. E assim, periodicamente.
    */

    QString newLabelVal;
    tasks = ui->label_executing_tasks->text();
    if (tasks.length() < 1){
        return;
    }
    if (tasks.contains("wipe")){
        if (this->wiper->isRunning()){
            newLabelVal += "wipe ";
        }
    }
    if (tasks.contains(tr("formatar"))){
        if (this->gpt->isRunning()){
            newLabelVal += tr("formatar ");
        }
    }
    if (tasks.contains("Monitor")){
        if (this->sysInfo->isRunning()){
            newLabelVal += "Monitor ";
        }
    }
    if (tasks.contains(tr("copia"))){
        if (this->bitwise->isRunning()){
            newLabelVal += tr("copia ");
        }
    }
    if (tasks.contains("hash")){
        if (this->fingerprint->isRunning()){
            newLabelVal += "hash ";
        }
    }
    if (tasks.contains("castConvert")){
        if (this->screencast->isRunning()){
            newLabelVal += "castConvert ";
        }
    }
    if (tasks.contains("renameShot")){
        if (this->autoRenameShot->isRunning()){
            newLabelVal += "renameShot ";
        }
    }
    if (tasks.contains("EWF")){
        if (this->ewfAcquiring->isRunning()){
            newLabelVal += "EWF ";
        }
    }
    if (tasks.contains("cripto")){
        if (this->crypt->isRunning()){
            newLabelVal += "cripto ";
        }
    }
    if (tasks.contains("IOStat")){
        if (this->iostat->isRunning()){
            newLabelVal += "IOStat ";
        }
    }

    ui->label_executing_tasks->setText(newLabelVal);
}

void MainWindow::slotSetPasswdEchoMode()
{
    if (ui->checkBox_showPasswd->isChecked()){
        ui->lineEdit_passConfirm->clear();
        ui->lineEdit_passConfirm->setDisabled(true);
        ui->lineEdit_pass->setEchoMode(QLineEdit::Normal);
        ui->lineEdit_passConfirm->setStyleSheet("background-color: rgb(214, 210, 208);");
        ui->label_passMatch->setText(tr("Visível"));
        ui->label_passMatch->setStyleSheet("color: rgb(0, 132, 203);");
        return;
    }
    if (ui->lineEdit_pass->text().isEmpty()){
        ui->label_passMatch->setText(tr("Senha vazia"));
        ui->label_passMatch->setStyleSheet("color: rgb(255, 153, 51);");
    }
    else if (!ui->lineEdit_pass->text().isEmpty()){
        ui->label_passMatch->setText(tr("Senhas não casam"));
        ui->label_passMatch->setStyleSheet("color: rgb(255, 153, 51);");
    }
    ui->lineEdit_pass->setEchoMode(QLineEdit::Password);
    ui->lineEdit_passConfirm->setEnabled(true);
    ui->lineEdit_passConfirm->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(255, 153, 51);");
}

void MainWindow::slotSetProgressBarValNow(int val)
{
    if (!ui->progressBar->isVisible()){
        ui->progressBar->show();
        ui->progressBar->setEnabled(true);
        if (ui->pushButton_colect->text().contains("Cancel")){
            ui->pushButton_colect->setEnabled(true);
        }
    }

    if (val == 100 && ui->progressBar->value() < 100 && ui->pushButton_colect->text().contains("Cancel")){
        ui->progressBar->setValue(100);
        QMessageBox box;
        box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        if (ui->checkBox_hash->isChecked()){
            //Invés de messageBox, apenas a mensagem de status no label, senão o usuário precisará ficar de olho na operação
            ui->label_status->setText(tr("Operação concluída com sucesso. Iniciando a geração de hash..."));
            this->checkout(IMGTAB);
        }

        ui->label_time_final->setText(dateTimeNowString());
        ui->pushButton_colect->setText(tr("Coletar"));
        ui->pushButton_colect->setEnabled(false);
        ui->comboBox_Source_bitwise->setEnabled(true);
        ui->comboBox_Source_bitwise->setCurrentIndex(0);

        box.setWindowTitle(tr("Tarefa finalizada"));
        box.setText(tr("Tarefa principal concluída sem erros aparentes (aguarde por respostas de outras operações, se houver)."));
        box.exec();

        if (ui->checkBox_screenshot->isChecked()){
            ui->tabWidget->setCurrentIndex(2);
            this->screenshot->shootScreen("fim_de_tarefa-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        }
    }
    ui->progressBar->setValue(val);
}

void MainWindow::slotSettime()
{
    if (!this->langSelected){
        slotRunMessageBox(QString(tr("Selecione um idioma primeiro. - Idioma")).split("-"));
        return;
    }
    if (!ui->checkBox_gerarLog->isChecked()){
        QMessageBox msg;
        msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        msg.setText(tr("Não deseja habilitar a geração de log?"));
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, tr("Sim, por favor"));
        msg.setButtonText(QMessageBox::No, tr("Não, obrigado"));
        int choice = msg.exec();
        if (choice == QMessageBox::Yes){
            ui->checkBox_gerarLog->setChecked(true);
            log(tr("[step] Geração de log iniciada: ").toUtf8());
        }
    }

    QStringList dateToSet;
    QString dateStr =  ui->dateTimeEdit->date().toString().split(" ").at(2) + " " + ui->dateTimeEdit->date().toString().split(" ").at(1) + " " + ui->dateTimeEdit->date().toString().split(" ").at(3);

    dateStr += " " + ui->dateTimeEdit->time().toString();
    QString finalMsg = tr("Ok inicial:") + dateStr + "\n";
    QString commandToSetDate = "date --set='" + dateStr + "'";

    if (system(commandToSetDate.toUtf8()) == EXIT_FAILURE){
        slotRunMessageBox(QString("Nao foi possivel ajustar data e hora - Erro no Ajuste de Data e Hora").split("-"));
        log(tr("[warning] Saida com erro ao ajustar a data: ").toUtf8());
    }

    ui->pushButton_selecionar->setEnabled(true);
    QString newDate = "Pós ajuste:\n" + ui->dateTimeEdit->date().toString();
    QString newHour = "Pós ajuste:\n" + dateTimeNowString().split(" ").at(3);
    ui->label_hour->setText(newHour);
    ui->label_date->setText(newDate);
    ui->textEdit->setHtml("<p style='text-align:justify'>Para dúvidas, auxílio e dicas, consulte a documentação. <br>Bons trabalhos.<br><br><b>Djames Watson</b>");
    log(tr("[step] Ativando recursos do programa ").toUtf8());
    this->enableDisableItems(true);
    //Não há como o usuário selecionar screenshot antes de clicar em Ok, então é melhor guardar preventivamente.
    this->screenshot->shootScreen("Inicio-"+dateTimeNowString().replace(" ","_"),true);
    if (ui->checkBox_gerarLog->isChecked()){
        ui->toolBox->setCurrentIndex(3);
    }
    this->slotAllDevsAndPartsToImgRead();
}

void MainWindow::slotShowingMyTextValueSerialInfo()
{
    QMessageBox box;
    box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    box.setWindowTitle(tr("Serial da Mídia de Origem"));
    QString txt = ui->pushButton_serial_info->text();
    box.setText("\t"+txt+"\t");
    if (!txt.contains(tr("DESCONHECIDO"))){
        box.setDetailedText(tr("O serial da mídia de origem é '")+txt+tr("'.\nO resultado foi obtido através de\
                                                                                          consulta direta ao firmware do dispositivo."));
    }
    else{
        box.setDetailedText(tr("A consulta ao dispositivo de origem não retornou informações válidas. Nesse caso, é comum que não exista mídia no dispositivo\
                        indicado, ou o dispositivo perdeu a comunicação com o sistema, ou trata-se de um cartão de memória. Tente reiniciar o processo ou \
                        indique outro dispositivo na origem, caso não se trate de um cartão de memória.\
                        O apontamento de um drive simulado também ocasiona esse tipo de retorno, mas não se trata de erro nesse caso."));
    }
    box.setStyleSheet("font-weight: bold; font-size: 16px;");
    box.exec();
}

void MainWindow::slotShowingMyTextValueSerialInfo2()
{
    QMessageBox box;
    box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    box.setWindowTitle(tr("Serial da Mídia de Destino"));
    QString txt = ui->pushButton_serial2_info->text();
    box.setText("\t"+txt+"\t");
    if (!txt.contains(tr("DESCONHECIDO"))){
        box.setDetailedText(tr("O serial da mídia de destino é '")+txt+tr("'.\nO resultado foi obtido através de\
                                                                                          consulta direta ao firmware do dispositivo."));
    }
    else{
        box.setDetailedText(tr("A consulta ao dispositivo de destino não retornou informações válidas. Nesse caso, é comum que não exista mídia no dispositivo\
                            indicado, ou o dispositivo perdeu a comunicação com o sistema, ou trata-se de um cartão de memória. Tente reiniciar o processo ou \
                            indique outro dispositivo no destino, caso não se trate de um cartão de memória.\
                            O apontamento de um drive simulado também ocasiona esse tipo de retorno, mas não se trata de erro nesse caso."));
    }
    box.setStyleSheet("font-weight: bold; font-size: 16px;");
    box.exec();
}

void MainWindow::slotShowTrueTreeFromDevice()
{

    QString target = ui->radioButton_show_dst_data->text();
    if (ui->radioButton_show_src_data->isChecked()){
        target = ui->radioButton_show_src_data->text();
    }
    if (!target.contains("1")){
        target += "1";
    }
    QProcess forensicLs;
    forensicLs.start(QString("fls "+target));
    if (!forensicLs.waitForFinished()){
        QMessageBox msg;
        msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        msg.setWindowTitle("Forensic ls");
        msg.setText(tr("Algum comportamento anômalo não permitiu o sucesso da operação."));
        msg.exec();
        log(tr("Alguma anomalia ocorreu durante o processo. ").toUtf8());
        return;
    }

    QString tree = forensicLs.readAllStandardOutput();
    forensicLs.close();

    popup pop;
    pop.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    pop.setWindowTitle(tr("Raiz da mídia selecionada"));
    pop.setGroupBoxTitle(target);
    if (tree.isEmpty()){
        tree = tr("Não foi possível avaliar a mídia selecionada. Entre as razões possíveis, um sistema de arquivos não suportado ou partição 1 inexistente.");
    }
    pop.setTextEditText(tree);
    pop.exec();

}

void MainWindow::slotShowingMyTextValueModelInfo()
{
    QMessageBox box;
    box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    box.setWindowTitle(tr("Modelo da Mídia de Origem"));
    QString txt = ui->pushButton_model_info->text();
    box.setText("\t"+txt+"\t");
    if (!txt.contains(tr("OCORRENCIAS"))){
        box.setDetailedText(tr("O modelo da mídia de origem é '")+txt+tr("'.\nO resultado foi obtido através de\
                                                                                          consulta direta ao firmware do dispositivo."));
    }
    else{
        box.setDetailedText(tr("A consulta ao dispositivo de origem não retornou informações válidas. Nesse caso, é comum que não exista mídia no dispositivo\
                            indicado, ou o dispositivo perdeu a comunicação com o sistema, ou trata-se de um cartão de memória. Tente reiniciar o processo ou \
                            indique outro dispositivo na origem, caso não se trate de um cartão de memória."));
    }

    box.setStyleSheet("font-weight: bold; font-size: 16px;");
    box.exec();
}

void MainWindow::slotShowingMyTextValueModelInfo2()
{
    QMessageBox box;
    box.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    box.setWindowTitle(tr("Modelo da Mídia de Destino"));
    QString txt = ui->pushButton_model2_info->text();
    box.setText("\t"+txt+"\t");
    if (!txt.contains(tr("OCORRENCIAS"))){
        box.setDetailedText(tr("O modelo da mídia de destino é '")+txt+tr("'.\nO resultado foi obtido através de\
                                                                                          consulta direta ao firmware do dispositivo."));
    }
    else{
        box.setDetailedText(tr("A consulta ao dispositivo de destino não retornou informações válidas. Nesse caso, é comum que não exista mídia no dispositivo\
                            indicado, ou o dispositivo perdeu a comunicação com o sistema, ou trata-se de um cartão de memória. Tente reiniciar o processo ou \
                            indique outro dispositivo no destino, caso não se trate de um cartão de memória."));
    }
    box.setStyleSheet("font-weight: bold; font-size: 16px;");
    box.exec();
}

void MainWindow::slotStatsToGraph(QString value)
{
    float result = value.replace(",",".").toFloat();

    int tamanho_janela = 120;
    if (j > tamanho_janela) {
        this->graph->setValuesX(j-tamanho_janela, j);
    }

    //abaixo, reajusta o tamanho da matriz se o valor for maior que Y ----//
    if (result > limit){
        limit = result+1;
        this->graph->setValuesY(0, limit);
    }
    //acima, reajusta o tamanho da matriz se o valor for maior que Y ----//

    this->graph->addNewValue(j, result);
    j++;
}

void MainWindow::slotStringStatusFromToolUsedInBitWiseCopy(QString string)
{
    int currentTab = ui->tabWidget->currentIndex();
    if ( currentTab == 1){
        ui->label_status->setText(string);
        if (this->exitCode == EXITCRASH){
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msg.setText(tr("O processo finalizou de maneira inesperada."));
            msg.setDetailedText(tr("Uma das possiblidades é esgotamento no espaço em disco, caso não estivesse vazio. Ainda, setores\
                                defeituosos, desconexão física do dispositivo de destino, etc.\
                                Se a cópia for de uma memória flash, é possível que o resultado esteja integro e a diferença deu-se\
                                por um erro de informação do firmware da memória."));
            msg.exec();
            this->exitCode = 5;
            if (ui->pushButton_colect->text().contains("Cancel")){
                this->slotPushButtonColect(); //isso está certo? nao seria buttonMsg?
            }
            return;
        }
        else if (this->exitCode == EXITOK){
            slotSetProgressBarValNow(100);
            log(tr("[step] Copia bitwise finalizada sem erros: ").toUtf8());
            this->exitCode = 5;
            this->lastVal = 0;
            return;
        }
    }
    else if (currentTab == 2){
        if (this->exitCode == EXITCRASH){
            //TODO: medir a partição selecionada pelo usuário para exibir na tela de monitoramento (já não é assim?)
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msg.setText(tr("O processo finalizou de maneira inesperada."));
            msg.setDetailedText(tr("Uma das possiblidades é esgotamento no espaço em disco, caso não estivesse vazio. Ainda, setores\
                                defeituosos, desconexão física do dispositivo de destino, etc.\
                                Se a cópia for de uma memória flash, é possível que o resultado esteja integro e a diferença deu-se\
                                por um erro de informação do firmware da memória."));
            msg.exec();
            this->exitCode = 5;
            return;
        }
        //TODO: clicar em cancelar em uma segunda clonagem faz o progressbar receber 100% e por consequencia emite mensagem de sucesso.
        else if (this->exitCode == EXITOK){
            slotSetProgressBarValNow(100);
            //TODO: o slot de reset do progressar emite mensagem. verificar se deve remover este abaixo
            log(tr("[step] Copia bitwise finalizada sem erros: ").toUtf8());
            this->exitCode = 5;
            this->lastVal = 0;
            return;
        }
    }

    int percent         = 0;
    QString target      = string;

    QString sizeSrc     = ui->label_size_src->text();
    float intSizeSrc    = 0;
    float intSizeCopied = 0;
    QRegExp rx;
    //QRegExp ddPercent;
    QString tool = "none";
    QString toolSelected = ui->comboBox_bitwiseTools->currentText();
    if (toolSelected.contains("(dd)")){
        tool = "dd";
        //60719104 bytes (58 M) copied ( 1%), 5,00686 s, 12 M/s
        rx.setPattern("\\d{1,}\\s\\w{1,}\\s\\((\\d{1,},?\\d{0,})\\s\\w{1,}\\)\\s");
        //ddPercent.setPattern("\\d{1,}\\s\\w{1,}\\s\\(\\d{1,},?\\d{0,}\\s\\w{1,}\\)\\s\\w{1,}\\s\\(\\s{0,}(\\d{1,})%\\),\\s");
    }
    else if (toolSelected.contains("(dcfldd)")){
        tool = "dcfldd";
        //dcfldd: 5120 blocks (168Mb) written.
        rx.setPattern("\\s{0,}\\d{1,}\\s\\w{1,}\\s\\((\\d{1,},?\\d{0,}\\w{1,})\\)\\s\\w{1,}\\.");
    }
    else if (toolSelected.contains("dc3dd")){
        if (!string.contains("%")){
            return;
        }
        tool = "dc3dd";

        rx.setPattern("\\d{1,}\\s\\w{1,}\\s\\(\\d{1,},?\\d{0,}\\s\\w\\)\\s\\w{1,}\\s\\(\\s{0,}(\\d{1,})%\\)");
        rx.indexIn(string);
        QString s = rx.capturedTexts().last();
        percent = s.toInt();

        slotSetProgressBarValNow(percent);
        this->graph->itemName->setText(string);

        return;
    } //DC3DD

    QString sizeCopied;
    //se o label nao estiver vazio, tenta fazer parsing no texto
    if (target.length() > 1){
        int stat = rx.indexIn(target);
        if (stat == -1){
            //nao trata nada para o progressbar se a str nao casar.
            return;
        }
        //pega valor contido na string enviada pela thread, sem a unidade de medida
        sizeCopied = rx.capturedTexts().last();

        QRegExp rxDigitOnly("(\\d{1,}\\.?\\d{0,})");//WARNING: adicionado virgula opcional - ADICIONAR PRA TESTAR
        rxDigitOnly.indexIn(sizeCopied);

        QString t  = rxDigitOnly.capturedTexts().last();
        sizeCopied = t;

        this->graph->itemName->setText(string);

        QString v = string.split(" ").at(2);
        //      v = "(384Mb)"
        v.remove(QRegExp(".b\\)"));
        v.remove("(");

        //converte para int e se estiver em TeraByte ou GigaByte, converte para MegaByte. se estiver em kB, divide por 1k. conferir(TODO)
        if (string.contains("TB)",Qt::CaseInsensitive) || string.contains("T)")){
            sizeCopied = QString::number((v.replace(",",".").toLongLong()*1000)*1000); //dd
        }
        else if (string.contains("GB)",Qt::CaseInsensitive) || string.contains("G)")){
            sizeCopied = QString::number(v.replace(",",".").toLongLong()*1000); //pra dd
        }
        else if (string.contains("MB)",Qt::CaseInsensitive) || string.contains("M)")){
            sizeCopied = QString::number(v.replace(",",".").toLongLong());
            //dcfldd ja vem em MB. a divisao acima vem de bytes, e olhe la se nao tem que tirar 1000 para outras tools - TODO: <-
            //WARNING: testar com gb e tb todas as funçoes por aqui. pode ser que mudança decimal influencie no pbar
            if (tool == "dcfldd"){
                sizeCopied = QString::number(v.toLongLong());
            }
        }
        else if (string.contains("kB)",Qt::CaseInsensitive)){ //é menor que MB
            sizeCopied = QString::number(v.toLongLong()/1000);
        }
        else{
            // "3564 mainwindow - area de problemas";
            return; //esse else pode ser um grande problema. ATENCAO!
        }

        //verifica se o label está com o valor adequado
        if (sizeSrc.length() > 2){
            //converte para int e se estiver em TeraByte ou GigaByte, converte para MegaByte
            if (sizeSrc.contains("TB",Qt::CaseInsensitive)){
                intSizeSrc = (sizeSrc.split(" ").at(0).toFloat()*1000)*1000;
            }
            else if (sizeSrc.contains("GB",Qt::CaseInsensitive)){ //estva string inves de sizeSrc
                intSizeSrc = sizeSrc.split(" ").at(0).toFloat()*1000;
            }
            else{
                intSizeSrc = sizeSrc.split(" ").at(0).toFloat();
            }
        }
        else{
            if (ui->progressBar->isEnabled()){
                ui->progressBar->setDisabled(true);
            }
            return;
        }

        //regra de 3 para alimentar o progressbar
        intSizeCopied = sizeCopied.toLongLong();
        percent = intSizeCopied*100/intSizeSrc >= 100 ? 100 : intSizeCopied*100/intSizeSrc ;

        slotSetProgressBarValNow(percent);
    }
}

void MainWindow::slotTargetBitwiseComboHandler(int val)
{

    if (val == 0){
        ui->comboBox_Target_bitwise->setEnabled(false);
        ui->comboBox_target_format->setEnabled(false);
        ui->comboBox_bitwiseTools->setCurrentIndex(0);

        ui->pushButton_model2_info->setDisabled(true);
        ui->pushButton_model_info->setDisabled(true);
        ui->pushButton_serial2_info->setDisabled(true);
        ui->pushButton_serial_info->setDisabled(true);

        ui->pushButton_model2_info->setText("");
        ui->pushButton_model_info->setText("");
        ui->pushButton_serial2_info->setText("");
        ui->pushButton_serial_info->setText("");

        ui->checkBox_split->setChecked(false);
        ui->frame_split->setEnabled(false);

        return;
    }
    //Esse slot é chamado pelo próprio checkbox se clicado para atuar em tempo real, mas aqui também é necessário
    //porque ao escolher um dispositivo de origem, é necessário aplicar a proteção a ele.
    this->slotCheckBoxChangeReadOnly();
    ui->comboBox_Target_bitwise->setEnabled(true);
    QStringList feed = this->common->connectedDevices(false,ui->comboBox_Source_bitwise->currentText());

    //Alimenta o label do monitoramento com a origem e progressbar de uso
    QString deviceToRadioText = "/dev/"+ui->comboBox_Source_bitwise->currentText();
    if (ui->comboBox_Source_bitwise->currentIndex() >0){
        this->setRadioTextTo("src",deviceToRadioText);
        QRegExp rx("\\d");
        int perc = this->getAndShowDiskFreeInProgressBarMonitor(deviceToRadioText.remove(rx));
        ui->progressBar_src_space->setValue(perc);
    }

    ui->comboBox_Target_bitwise->clear();
    ui->comboBox_Target_bitwise->addItems(feed);


    ui->pushButton_model2_info->setDisabled(false);
    ui->pushButton_model_info->setDisabled(false);
    ui->pushButton_serial2_info->setDisabled(false);
    ui->pushButton_serial_info->setDisabled(false);

    if (ui->comboBox_Source_bitwise->currentIndex() != 0){
        ui->pushButton_model2_info->setText("");
        ui->pushButton_serial2_info->setText("");

        ui->pushButton_model2_info->setDisabled(true);
        ui->pushButton_serial2_info->setDisabled(true);
        this->labelsFeed("src");
    }

}

void MainWindow::slotTreeView(QString signalSource)
{
    QString jailDir = "/media/images";
    if (signalSource == "logs"){
        jailDir = "/var/log";
    }
    ui->stackedWidget_fromImg->setCurrentIndex(1);
    QFileSystemModel *model = new QFileSystemModel;
    QDir::setCurrent(jailDir);
    model->setRootPath(jailDir);
    model->setReadOnly(true);
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setModel(model);
    QModelIndex idx = model->index(jailDir);
    ui->treeView->setRootIndex(idx);
    ui->treeView->setColumnWidth(0,300);
    ui->treeView->setColumnWidth(1,80);
    qRegisterMetaType<QModelIndex>("QModelIndex");
    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotTreeViewItemSelected(QModelIndex)));
    //TODO: testar delete do model, mas provavelmente nao da.
}

void MainWindow::slotTreeViewItemSelected(QModelIndex model)
{
    //DUVIDA: double click funciona no live system?
    //TODO: esses diretorios esparramados devem ser trocados por uma variavel global vinda do .ini
    QDirIterator find("/media/images",QDirIterator::Subdirectories);
    QString selectedFile = model.data().toByteArray();
    QString fullPath;
    while (find.hasNext()){
        fullPath = find.next();
        if (fullPath.contains(selectedFile)){
            ui->label_image->setText(fullPath);
            break;
        }
    }
}

void MainWindow::slotUnchecker()
{
    if (ui->checkBox_aescrypt->isChecked()){
        ui->checkBox_OpenSSL->setDisabled(true);
    }
    else if (ui->checkBox_OpenSSL->isChecked()){
        ui->checkBox_aescrypt->setDisabled(true);
        ui->stackedWidget_crypt->setCurrentIndex(1);
    }
    else if (!ui->checkBox_aescrypt->isChecked() && !ui->checkBox_OpenSSL->isChecked()){
        ui->checkBox_aescrypt->setEnabled(true);
        ui->checkBox_OpenSSL->setEnabled(true);
        ui->stackedWidget_crypt->setCurrentIndex(0);
    }
}

void MainWindow::slotWipeFreeSectors()
{
    if (!ui->radioButton_fire_src->isChecked()){
        return;
    }
    QMessageBox wipe;
    wipe.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    wipe.setText(tr("Limpar setores não alocados da mídia de destino?"));
    wipe.setWindowTitle("Hot Wipe");
    wipe.setDetailedText(tr("Não há risco para os dados salvos e isso garantirá que nenhum dado seja recuperado desses setores não utilizados."));
    int yesNo = wipe.exec();
    if (yesNo == QMessageBox::Yes){
        this->hotWipe = true;
    }
    this->crypt->removeFileAfterCrypt(true);
}

void MainWindow::slotWipeNow()
{
    if (!ui->checkBox_wipe->isChecked()){
        return;
    }
    //TODO: QUALQUER THREAD DE TAREFA DEVE SER INCLUIDA AQUI ATE O FIM DO PROGRAMA - fazer um metodo porque isso é usado em mais lugares
    if (this->wiper->isRunning() || this->gpt->isRunning() || this->bitwise->isRunning() || ewfAcquiring->isRunning() || crypt->isRunning()){
        QMessageBox msg;
        msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
        msg.setText(tr("Você já possui uma tarefa em execução e não poderá executar wipe agora."));
        msg.setWindowTitle(tr("Outra tarefa em execução"));
        msg.exec();
        this->screenshot->shootScreen("tarefa_em_execucao-"+this->dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        return;
    }

    if (!ui->checkBox_wipe->isChecked()){
        if (this->wiper->isRunning()){
            emit signalTerminateWipe();
            this->wiper->exit(0);
            this->screenshot->shootScreen("wipe_cancelado-"+this->dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        }
    }

    QMessageBox msgbox;
    msgbox.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
    if (!ui->comboBox_Target_bitwise->isEnabled() || ui->comboBox_Target_bitwise->currentIndex() == 0){
        msgbox.setWindowTitle(tr("Selecione destino primeiro"));
        msgbox.setText(tr("Você deve escoher o dispositivo de destino previamente a essa opção."));
        msgbox.exec();
    }
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgbox.setButtonText(QMessageBox::Yes,tr("Sim"));
    msgbox.setButtonText(QMessageBox::Cancel,tr("Cancelar"));
    msgbox.setWindowTitle(tr("Eliminação de dados na midia de destino"));
    msgbox.setText(tr("Essa operação eliminará qualquer dado existente na mídia de destino. Continuar?"));

    int stat = msgbox.exec();
    this->screenshot->shootScreen("confirma_wipe-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());

    if (stat == QMessageBox::Cancel){
        msgbox.setWindowTitle(tr("Processo não iniciado"));
        msgbox.setText(tr("Operação cancelada. Nada modificado na midia de destino."));
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.exec();
        ui->checkBox_wipe->setChecked(false);
        this->screenshot->shootScreen("desistencia_wipe-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        return;
    }
    if (this->wiper->isRunning()){
        msgbox.setWindowTitle(tr("Processo em execução"));
        msgbox.setText(tr("Devo interromper o processo atual para iniciar um novo?"));
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgbox.setButtonText(QMessageBox::Yes,tr("Sim"));
        msgbox.setButtonText(QMessageBox::No,tr("Não"));
        int yesno = msgbox.exec();
        this->screenshot->shootScreen("wipe-interromper_existente-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
        if (yesno == QMessageBox::No){
            msgbox.setWindowTitle(tr("Nada modificado"));
            msgbox.setText(tr("Nada será modificado."));
            msgbox.exec();
            this->screenshot->shootScreen("wipe-nao_interromper-"+dateTimeNowString().replace(" ","_").toUtf8(),ui->checkBox_screenshot->isChecked());
            ui->checkBox_wipe->setChecked(false);
            return;
        }
    }
    ui->checkBox_wipe->setDisabled(true);
    slotSetProgressBarValNow(0);
    QString device = "/dev/"+ui->comboBox_Target_bitwise->currentText();
    this->wiper->dev = device;
    emit buttonMsg(tr("Cancelar"));
    ui->pushButton_colect->setEnabled(true);
    ui->label_time_inicio->setText(dateTimeNowString());
    this->wiper->setFullWipe(true);
    this->wiper->start();
    QTimer::singleShot(1000,this->screenshot,SLOT(shootScreen("coletar-strtab",ui->checkBox_screenshot->isChecked())));
}

bool MainWindow::targetIsWriteable(QString path, QString dev)
{

    QFile file(path);
    file.open(QIODevice::Text |QIODevice::ReadWrite);
    if (file.isWritable()){
        file.close();
        file.remove();
        return true;
    }
    else{
        if (!dev.contains("/dev/")){
            dev = "/dev/" + dev;
        }
        QRegExp rx("\\d{1,}");
        dev = dev.remove(rx);
        QProcess blockdev;
        QStringList params;
        params << "--getro" << dev;
        blockdev.start("blockdev",params);
        blockdev.waitForFinished();
        QString result = blockdev.readAllStandardOutput();
        blockdev.close();
        if (result.contains("1")){
            QMessageBox msg;
            msg.setStyleSheet("QMessageBox{border: 2px solid; border-color:  rgb(85, 170, 255);}");
            msg.setText(tr("O dispositivo de destino está em modo somente-leitura. Modificar para permitir escrita?"));
            msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msg.setButtonText(QMessageBox::Yes,tr("Sim"));
            msg.setButtonText(QMessageBox::No,tr("Não"));
            int status = msg.exec();
            if (status == QMessageBox::No){
                return false;
            }
            ui->label_status->setText(tr("Tentando modificar o dispositivo para leitura e escrita..."));
            params.clear();
            params << "--setrw" << dev;
            blockdev.start("blockdev",params);
            blockdev.waitForFinished();
            blockdev.close();
            QThread::sleep(3);
            ui->label_status->setText(tr("Operação concluída. Experimente repetir o processo."));
        }
    }
    return false;
}

void MainWindow::toolTips()
{
    //Tips
    ui->checkBoxHashEmArquivo->setToolTip(tr("Gravar hash gerado em arquivo com extensão .sha ou .md5"));
    ui->checkBox_gerarLog->setToolTip(tr("Gera log detalhado dos processos"));
    ui->checkBox_hash->setToolTip(tr("Gerar hash apenas em video, sem gravá-lo em midia física"));
    ui->checkBox_readOnly_bw->setToolTip(tr("Proteção contra escrita na mídia de origem"));
    ui->checkBox_wipe->setToolTip(tr("Excluir quaisquer dados existentes na mídia de destino"));
    ui->checkBox_screenshot->setToolTip(tr("Recolhe telas de início e fim de operação, incluíndo hash"));
    ui->checkBox_relatorio->setToolTip(tr("Gerar relatório de operação"));
    ui->checkBox_screenshot->setToolTip(tr("Gerar auto-screenshot das operações"));
    ui->checkBox_rename_shots_wayland->setToolTip(tr("Renomeia e move os shots tirados pelo Wayland, invés do DWatson"));
    ui->checkBox_remove_wcap->setToolTip(tr("Remove o arquivo wcap após converter para formato de video\n(somente se a operação ocorrer com sucesso)"));
    ui->checkBox_OpenSSL->setToolTip(tr("Criptografar a imagem ao final (será necessário mais que 50% de espaço livre no destino se a imagem não estiver dividida em partes)"));
    ui->checkBox_aescrypt->setToolTip(tr("Criptografar a imagem ao final (será necessário mais que 50% de espaço livre no destino se a imagem não estiver dividida em partes)"));

    ui->tabWidget->setTabToolTip(0,tr("Informações sobre uma imagem bit-a-bit"));
    ui->tabWidget->setTabToolTip(1,tr("Gerar imagem forense (bit-a-bit)"));
    ui->tabWidget->setTabToolTip(2,tr("Informações de saída de eventos"));
    ui->tabWidget->setTabToolTip(3,tr("Registros das operações do programa"));

    ui->toolBox->setItemToolTip(1,tr("Lista de dispositivos físicos<br>encontrados no sistema"));
    ui->toolBox->setItemToolTip(2,tr("Configurações para geração de imagem"));
    ui->toolBox->setItemToolTip(3,tr("Interação com os logs.<br> Veja 'Ajuda' ou 'Documentação para maiores detalhes"));
    ui->toolBox->setItemToolTip(4,tr("Ajuste do relógio para consistência da coleta"));

    ui->radioButton_off->setToolTip(tr("Não ativa visualização, mas mantém a geração de log habilitada"));
    ui->radioButton_on->setToolTip(tr("Liga a visualização de logs <br>do programa na tab 'Ler Imagem'"));
    ui->radio_md5->setToolTip(tr("Gerar hash com MD5. Rápido, porém não mais recomendado"));
    ui->radio_sha->setToolTip(tr("Gerar hash com SHA. Escolha o tipo na caixa ao lado"));
    ui->radioButton_bw_src2file->setToolTip(tr("Salvar a imagem da origem em um arquivo RAW de destino"));
    ui->radioButton_bw_disc2disc->setToolTip(tr("Imagem clone, sobrescrevendo o destino com os dados da origem"));
    ui->radioButton_show_dst_data->setToolTip(tr("Selecione para acesso aos dados contidos na mídia de destino"));
    ui->radioButton_show_src_data->setToolTip(tr("Selecione para acessar os dados contidos na mídia de origem (Somente-Leitura)"));
    ui->radioButton_show_dst_data->setText(tr("inativo"));
    ui->radioButton_show_src_data->setText(tr("inativo"));
    ui->radioButton_openLog->setToolTip(tr("Abre o log para visualização"));
    ui->radioButton_removeLog->setToolTip(tr("Remove todos os arquivos relacionados ao log"));
    ui->radioButton_copyLog->setToolTip(tr("Copia todo o conteúdo de log para uma mídia externa"));
    ui->radioButton_en->setToolTip("English");
    ui->radioButton_es->setToolTip("Español");
    ui->radioButton_pt->setToolTip("Português Brasil");

    ui->pushButton_selecionar->setToolTip(tr("Selecione imagem a obter informações"));
    ui->pushButton_datetime->setToolTip(tr("Clique após ajustar o relógio ou agora, se estiver tudo Ok"));
    ui->pushButton_colect->setToolTip(tr("Botão para coleta e cancelamento da operação"));
    ui->pushButton_serial_info->setToolTip(tr("Clique para visualizar em outra janela"));
    ui->pushButton_serial2_info->setToolTip(tr("Clique para visualizar em outra janela"));
    ui->pushButton_model_info->setToolTip(tr("Clique para visualizar em outra janela"));
    ui->pushButton_model2_info->setToolTip(tr("Clique para visualizar em outra janela"));
    ui->pushButton_show_data->setToolTip(tr("Apenas exibe dados contidos na raiz da mídia\nantes ou após qualquer operação"));
    ui->pushButton_wayland_screencast->setToolTip(tr("Converte screencast para formato de video tradicional (use após criar screencast)"));

    ui->comboBox_Source_bitwise->setToolTip(tr("Selecione Origem/Suspeito"));
    ui->comboBox_Target_bitwise->setToolTip(tr("Selecione Destino/Evidência"));
    ui->comboBox_target_format->setToolTip(tr("Apagará qualquer dado existente nesse dispositivo"));
    ui->comboBox_bitwiseTools->setToolTip(tr("Selecione a ferramenta para a cópia de baixo nível"));
    ui->comboBox_hash->setToolTip(tr("Escolha do tipo de hash SHA a utilizar"));

    ui->label_cpu_usage->setToolTip(tr("Exibe consumo de CPU durante toda a execução do programa"));
    ui->label_memory_usage->setToolTip(tr("Exibe consumo de memória durante toda a execução do programa"));
    ui->label_mem_total->setToolTip(tr("Exibe total de memória física"));
    ui->label_cpus_number->setToolTip(tr("Exibe número de núcleos, incluindo HTs"));
    ui->label_final->setToolTip(tr("Exibe o período de utilização da ferramenta para a última operação executada"));
    ui->label_status->setToolTip(tr("Todas as mensagens possíveis durante a operação são exibidas aqui"));
    ui->label_inicio->setToolTip(tr("O início de um processo é registrado aqui"));
    ui->label_conversion_cast_value->setToolTip(tr("Exibe valores já lidos do arquivo wcap"));
}

void MainWindow::translate2(QString locale)
{
    //translator.load(QString("dwos_en"),":/i18n/resources/i18n");
    this->langSelected = true;
    // idioma padrao
    if(locale == "en" || locale == "es")
    {
        //buscar na raiz do sistema
        //translator.load("/etc/locales/dwos_"+ locale);
        //buscar no ambiente de desenvolvimento
        QFile devel("/home/djames/DEVEL");
        if (devel.exists()){
            translator.load("/home/djames/DjamesWatson/CadeiaDeCustodia/resources/i18n/dwos_"+locale);
        }
        else{
            translator.load("/etc/locales/dwos_"+ locale);
        }
        //buscar nos resources
        //translator.load(QString("dwos_en"),":/i18n/resources/i18n");
    }
    else{
        translator.load(0);
    }

    // aplicar
    ui->retranslateUi(this);

    //coisas que precisam ser ajustadas na seleção do idioma
    this->preserveOnReload();
}

void MainWindow::shotsViewer()
{
    QSettings settings("/home/djames/dwatson.ini",QSettings::IniFormat);

    QMap <int, QLabel*> labels;
    QMap <int,QScrollArea*> scrolls;
    QImage image;

    labels.insert(0,this->imageLabel1);
    labels.insert(1,this->imageLabel2);
    labels.insert(2,this->imageLabel3);
    labels.insert(3,this->imageLabel4);

    scrolls.insert(0,ui->scrollArea_1);
    scrolls.insert(1,ui->scrollArea_2);
    scrolls.insert(2,ui->scrollArea_3);
    scrolls.insert(3,ui->scrollArea_4);

    int j = ui->horizontalSlider_screenshots->value()*4;
    int k = 0;

    for (int i=j-4;i<j;i++){
        if (this->imageName.length() < i+1){
            //mesmo que o arquivo nao exista, nao da excessao
            image = QImage(settings.value("dwatson.screenshot/background","/home/djames/lupa.png").toString());
            QPainter* painter = new QPainter(&image);
            painter->setPen(Qt::white);
            painter->setFont(QFont(settings.value("dwatson.screenshot/fontName","/usr/share/fonts/truetype/droid/DroidSans-Bold.ttf").toString(), settings.value("dwatson.screenshot/fontSize","130").toInt()));
            painter->drawText(image.rect(), Qt::AlignCenter, (QString::number(i+1)));
            delete painter;
        }
        else{
            image = QImage(this->imageName.at(i));
            labels[k]->setToolTip(this->imageName.at(i).split("/").last());
        }

        labels[k]->setBackgroundRole(QPalette::Base);
        labels[k]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        labels[k]->setScaledContents(true);

        scrolls[k]->setBackgroundRole(QPalette::Dark);
        scrolls[k]->setWidget(labels[k]);

        labels[k]->setPixmap(QPixmap::fromImage(image));
        //scaleFactor = 1.0;
        labels[k]->adjustSize();
        k++;
    }
}

MainWindow::~MainWindow()
{
    this->log(tr("[step] Fim da utilização do programa: ").toUtf8());
    delete common;

    wiper->terminate();
    wiper->wait(2000);
    delete wiper;

    gpt->terminate();
    gpt->wait(2000);
    delete gpt;

    bitwise->terminate();
    bitwise->wait(2000);
    delete bitwise;

    fingerprint->terminate();
    fingerprint->wait(2000);
    delete fingerprint;

    sysInfo->terminate();
    sysInfo->wait(2000);
    delete sysInfo;

    iostat->terminate();
    iostat->wait(2000);
    delete iostat;

    autoRenameShot->terminate();
    autoRenameShot->wait(2000);
    delete autoRenameShot;

    delete doc;

    pbarUndefVal->terminate();
    pbarUndefVal->wait(2000);
    delete pbarUndefVal;

    delete screenshot;

    delete graph;

    screencast->terminate();
    screencast->wait(2000);
    delete screencast;

    ewfAcquiring->terminate();
    ewfAcquiring->wait(3000);
    delete ewfAcquiring;

    crypt->terminate();
    crypt->wait(3000);
    delete crypt;

    delete ui;
}
