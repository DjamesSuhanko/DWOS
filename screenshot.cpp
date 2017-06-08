#include "screenshot.h"

Screenshot::Screenshot(QWidget *parent)
{

}

void Screenshot::saveScreenshot(QString name)
{
    QString fullName = "/var/log/screenshots/" + name + ".png";
    QString fileName = tr(fullName.toUtf8());
    if (QFile::exists(fileName)){
        fileName = fileName.split((".")).at(0) + "-X";
        fileName = fileName + ".png";
        fileName = tr(fileName.toUtf8());
    }
    originalPixmap.save(fileName, "png");
}

void Screenshot::shootScreen(QString filename, bool fire)
{
    if (!fire){
        return;
    }

    if (QFile::exists("/weston.ini")){
        //se for o dwos, nao pega a tela toda, entao capturar a propria janela, que é bem mais fácil
        QString name = filename;
        emit shoot(name);
        return;
    }

    //captura o desktop quando nao for wayland
    QScreen *screen = QGuiApplication::primaryScreen();
       if (screen){
           originalPixmap = screen->grabWindow(0);
           saveScreenshot(filename);
       }
       emit imageName(filename);
}


