#ifndef PROGRESSBARUNDEFINEDVALUE_H
#define PROGRESSBARUNDEFINEDVALUE_H

#include <QThread>
#include <QMutex>

class ProgressBarUndefinedValue : public QThread
{
    Q_OBJECT
public:
    explicit ProgressBarUndefinedValue(QObject *parent = 0);

    //! Starter da thread
    void run();

    //! Parar ou manter a thread rodando
    void setKeepRunning(bool state);

signals:
    //! Emissor da solicitação de inversão de direção do progressbar indefinido
    void setInvertedAppearance(bool onOff);

    //! Emissor do valor de deslocamento
    void setValue(int i);

public slots:

private:
    //! Define incremento ou decremento do progressbar
    void ProgressBarUpDown();

    //! Manter o progressbar rodando?
    bool keepRunning=false;

    //! Proteção da variável contra concorrência
    QMutex mutex;

};

#endif // PROGRESSBARUNDEFINEDVALUE_H
