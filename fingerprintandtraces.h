#ifndef FINGERPRINTANDTRACES_H
#define FINGERPRINTANDTRACES_H

#include <QThread>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>

class FingerPrintAndTraces : public QThread
{
    Q_OBJECT
public:
    explicit FingerPrintAndTraces(QObject *parent = 0);
    void run();

    //! Alimentador das variáveis utilizadas para a geração de hash
    /*! Esse método é chamado na MainWindow previamente ao start da thread. Esse método simplesmente alimenta duas variáveis privadas,
     * descritas em detalhes na documentação.
     * Verifique também a documentação relacionada ao método makeHash() desta thread.
     */
    void setHashType(QString hashSelected, QString fileTarget, bool writeInAfile);

private:
    //! Cálculo do hash e geração de arquivo do hash
    /*! Esse método avalia duas variáveis (commands e hashToFile) para saber que procedimentos adotar. Se for selecionado apenas a geração de
     * base de strings, então esse método não apresentará nada. De qualquer modo, ele é chamado em run(), porque ele próprio avalia sua condição
     * de uso.
     */
    void isHashPossible();

    //! Flag para informar se o hash deve ser gravado em arquivo ou apenas exibido na tela
    bool writeHashInFile=false;

    //! Utiliza recurso interno do Qt para geração do hash
    QByteArray makeHash(QString hashtype, QString targetToHashing);

    //! Comandos para a execução do QProcess relacionado à geração do hash
    QString commands;

    //! Variável que guarda a informação necessária para a geração do arquivo de hash no destino.
    QString hashToFile;

    //! Algoritmo selecionado para a geração de hash
    QString algorithym;


signals:
    //! MessageBox das ocorrências durante as operações
    /*! Na MainWindow, no método connections, encontra-se uma conexão direta com o método slotRunMessageBox. O envio da mensagem é feito
     * utilizando uma QStringList porque o método que recebe a mensagem espera 3 informações:
     * 1 - A mensagem a exibir ao usuário
     * 2 - Titulo da janela (pode ser vazio)
     * 3 - Texto detalhado (pode ser vazio)
     */
    void signalMsgBox(QStringList resultOfHash);

    //! Emite a mensagem a ser armazenada pelo log, da MainWindow
    void log(QByteArray msg);

public slots:

};

#endif // FINGERPRINTANDTRACES_H
