#ifndef DOCUMENTATION_H
#define DOCUMENTATION_H

#include <QObject>
#include <QRegExp>
#include <QStringList>

class documentation : public QObject
{
    Q_OBJECT
public:
    explicit documentation(QObject *parent = 0);

private:

signals:
    //! Sinal do texto selecionado. Após emitir o sinal, um slot da MainWindow alimenta o textbrowser com o respectivo texto.
    void signalSelectedSection(QString section);

public slots:
    //! Método para pegar o valor  de capítulo correspondente.
    void getSection(QString fullLine);

};

#endif // DOCUMENTATION_H
