#include "autorenameshot.h"
#define FILE_PLACE "/wayland-screenshot.png"
#define PREFIX "screenshot-autorenamed-"
#define TARGET_SCREENSHOT_DIR "/var/log/screenshot/"

AutoRenameShot::AutoRenameShot(QObject *parent) :
    QThread(parent)
{
    QString iniFile = "/home/djames/dwatson.ini";
    QFile settingsFile(iniFile);
    //Se não existir, não modifica os valores padrão
    if (!settingsFile.exists()){
        return;
    }

    QSettings settings(iniFile,QSettings::IniFormat);
    QString nome = settings.value("wayland/nome","wayland/nome").toString();

    QString filePlace = settings.value("wayland.general/filePlace",FILE_PLACE).toString();

    file.setFileName(FILE_PLACE);    
}

void AutoRenameShot::renameAndMoveTo()
{
    if (QFile::exists(FILE_PLACE)){
        QDateTime now     = QDateTime::currentDateTime();
        QString  date     = now.currentDateTime().toString().replace(" ","_");
        this->newFileName = PREFIX + date + ".png";
        if (!file.rename(FILE_PLACE,TARGET_SCREENSHOT_DIR)){
            //nada por enquanto
        }
    }
}

void AutoRenameShot::run()
{
    while (true){
        this->renameAndMoveTo();
        sleep(1);
    }
}
