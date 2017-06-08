#ifndef BITWISECOPY_H
#define BITWISECOPY_H

#include <QThread>
#include <QStringList>
#include <QProcess>
#include <QDir>

class BitWiseCopy : public QThread
{
    Q_OBJECT
public:
    explicit BitWiseCopy(QObject *parent = 0);
    void run();

private:
    //! Valores para utilização com DDRescue
    /*! A MainWindow alimenta diversas variáveis desta classe para a cópia bitwise. Os inteiro timeout e retries
     * tem por objetivo armazenar os valores provindos dos sliders na aba Geração de Imagem, da toolBox.
    */
    int timeout,retries;

    //! Método para utilização da ferramenta DD
    /*! Cada ferramenta escolhida tem um comportamento diferente, recebe parâmetros específicos e tem um retorno específico.
     * Portanto, cada ferramenta sendo tratada em seu próprio método facilitará a interpretação d chamada principal.
    */
    void startDDcopy();

    //! Método para utilização da ferramenta DD
    /*! Cada ferramenta escolhida tem um comportamento diferente, recebe parâmetros específicos e tem um retorno específico.
     * Portanto, cada ferramenta sendo tratada em seu próprio método facilitará a interpretação d chamada principal.
    */
    void startDCFLDDcopy();

    //! Método para utilização da ferramenta DD
    /*! Cada ferramenta escolhida tem um comportamento diferente, recebe parâmetros específicos e tem um retorno específico.
     * Portanto, cada ferramenta sendo tratada em seu próprio método facilitará a interpretação d chamada principal.
    */
    void startDC3DDcopy();

    //! Método para utilização da ferramenta DD
    /*! Cada ferramenta escolhida tem um comportamento diferente, recebe parâmetros específicos e tem um retorno específico.
     * Portanto, cada ferramenta sendo tratada em seu próprio método facilitará a interpretação d chamada principal.
    */
    //void startDDRESCUEcopy();

    //!Comando completo a utilizar na cópia bitwise
    QString fullCommand;

    //! Interruptor do processo, alimentado através d método público stopBitWiseCopyNow()
    bool killBitWiseCopy=false;

signals:

    //! Esse sinal envia a string lida do QProcess da cópia bitwise para  MainWindow::slotBitWiseString.
    /*! Ao receber, a string é dividida e distribuida, conforme a ferramenta utilizada na cópia.
     * Esse sinal é emitido através do método que faz o parsing adequadamente e monta a QString pronta para uso.
    */
    void signalStringStatusFromToolUsedInBitWiseCopy(QString string);
    //! Emite sinal com código de saída, com tratamento diferente do método finished(), da própria thread.
    void signalIsFinished(int exitCode);
    //! Emissor das mensagens a gravar no log. O método receptor log() está declarado na MainWindow.
    void log(QByteArray msg);

public slots:

    //! Slot para alimentar parâmetros do DDRescue, da aba de Geração de Imagem.
    /*! Os valores são coletados na ToolBox e enviado um sinal contendo o valor.
    */
    void slotTimeout(int t);

    //! Slot para alimentar parâmetros do DDRescue, da aba de Geração de Imagem.
    /*! Os valores são coletados na ToolBox e enviado um sinal contendo o valor.
    */
    void slotRetries(int t);

    //!Guarda o comando para a cópia bitwise na variável fullCommand;
    void slotSetCommandToBitWiseCopy(QString command);


    //! Montagem e desmontagem da mídia de destino
    /*! Quando a cópia for do modo tradicional (origem à arquivo), é necessário montar a mídia de destino previamente.
     * Nesse caso é fundamental fazer algumas avaliações, como:
     * - sistema de arquivos no destino é suportado?
     * - NÃO: Solicitar ao usuário que proceda com a formatação.
     * - mídia de destino contém arquivos alocados ou desalocados?
     * - SIM: Perguntar ao usuário se deve fazer wipe.
     *
     * Obedecendo a todos os critérios, pode-se então seguir adiante do método que invocou a montagem. Ao término, esse
     * método é chamado novamente e ele avalia uma única condição: A mídia já está montada?
     * - SIM: Desmonta
    */
    void slotMountOrUmontAndCheckConditionsOf(QString device, const QString dirPath);

    //! Método público para finalizar o processo de cópia a qualquer momento, quando clicado em Cancelar, da MainWindow
    void slotStopBitWiseCopyNow();

private slots:

    //! Validar o final do processo e informar o usuário fazendo o último emit.
    /*! Após iniciado, o processo é tratado e o resultado é enviado para a MainWIndows através de um signal. Ao término
     * do processo, um último emit é necessário para saber se o processo finalizou sem erros. Essa etapa é muito importante
     * para garantir a integridade da cópia
    */
    void slotIsFinished(int exitCode);

};

#endif // BITWISECOPY_H
