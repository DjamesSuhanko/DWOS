#include <QWidget>
#include <QTimer>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>

class Screenshot : public QWidget
{
    Q_OBJECT

public:
    Screenshot(QWidget *parent = 0);

    //! Salvamento do screenshot
    void saveScreenshot(QString name);


protected:

private slots:

public slots:
    //! Receptor do disparo
    void shootScreen(QString filename, bool fire);


private:

    QPixmap originalPixmap;

signals:
    //! Disparo
     void shoot(QString name);

     //! miniaturas do viewer, na MainWindow
     void imageName(QString fname);
};
