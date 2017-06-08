#include "progressbarundefinedvalue.h"

ProgressBarUndefinedValue::ProgressBarUndefinedValue(QObject *parent) :
    QThread(parent)
{
}

void ProgressBarUndefinedValue::ProgressBarUpDown()
{
    int i;

    for (i=0;i<21;i++){
        emit setValue(i);
        this->msleep(15);
    }
    emit setInvertedAppearance(true);
    for (i=20;i>-1;i--){
        setValue(i);
        this->msleep(15);
    }
    for (i=0;i<21;i++){
        setValue(i);
        this->msleep(15);
    }
    emit setInvertedAppearance(false);
    for (i=20;i>-1;i--){
        setValue(i);
        this->msleep(15);
    }
}

void ProgressBarUndefinedValue::run()
{
    while (this->keepRunning){
        this->ProgressBarUpDown();
    }
}

void ProgressBarUndefinedValue::setKeepRunning(bool state)
{
    //melhor testar antes de fazer lock. isso pode dar excessao
    mutex.lock();
    this->keepRunning = state;
    mutex.unlock();
}
