#ifndef COMMON_H
#define COMMON_H
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QRegExp>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QObject>
//...
class Common : public QObject
{
    Q_OBJECT
public:
    explicit Common(QObject *parent = 0);
    ~Common();

    //!Nome de arquivo a gerar imagem
    QString TARGET_FILENAME;

    //! Variável para guardar o nome do arquivo a gerar o log.
    /*! A MainWindow oferece a opção de coletar log. Se essa opção for marcada,
     * Um box solicita um nome para o log, sugerindo o nome de arquivo acrescido
     * da extensão .log. O parser do nome para o arquivo de log é feito pelo slot
     * makeLog(bool) desta classe, conectado ao checkbox de log da MainWindow.
     * Quando recebe o sinal, envia outro sinal (fireFile) para o slot parseNameToFileLog(QString f)
     * desta classe.
    */
    const QString filenameToLogPath = "/var/log/ForensicMessages.log";

    QStringList msgs;
    //! Método para exibir a saída do fdisk e parted
    /*! Devido à possibilidade de um disco utlizar GPT para criar a tabela
     * de partição, a saída é mostrada com o parted e com fdisk sem condicionais.
    */
    QString PartedAndFdisk(QString devOrFileTarget);

    //! Montar lista de dispositivos de armazenamento
    /*! Para montar a lista do ToolBox correspondente aos HDDs/SSDs, esse método monta
     * uma QStringList a partir deste método, chamado na classe MainWindow.
     * Esse método é reaproveitado para alimentação do combobox de cópia bit a bit. Na
     * lista de dispositivos na toolbox, o campo "Dev" é removido, enquanto no combobox de
     * origem ela é mantida.
     * No combobox de destino, o dispositivo de origem é removido da lista. Para isso, deve-se
     * passar 2 parâmetros para esse método: (i) booleano discOnly true, se for para mostrar apenas
     * dispositivos e (ii) QString exclude "none" se não for para excluir nenhum dispositivo.
     * Então, na origem da clonagem utiliza-se connectedDevices(true,"none").
     * No destino utiliza-se connectedDevices(true, SelecaoDaOrigem).
     * O destino só é disponibilizado após selecionar a origem.
    */
    QStringList connectedDevices(bool discOnly, QString exclude);

    //! Procura pelo valor de variável de ambiente passado através do parâmetro
    /*! Esse método está sendo utilizado na classe MainWindow para auxilio na composição
     * da janela de seleção de origem e destino. Essas variáveis de caminho inicial estão
     * definidas hardcoded no momento e necessitará de um pré-configurador na instalação
     * do programa (versão 0.5 ou maior).
    */
    QString getEnv(QString varEnv);

    //! Variável de controle de tempo da atualização do progressbar da aba "Ler imagem DD".
    /*! Essa variável é utilizada pelo start do QTimer timer, da classe MainWindow, para novo
     * disparo do slot updateProgressBar(), também da classe MainWindow.
    */
    int timeoutTimer;

    //! Armazena o destino
    QString targetStrings;

    //! Armazena a origem
    QString sourceStrings;

    //! Criação do arquivo de hash, chamado na MainWindow
    void varToHashFeed(QStringList vth, bool makeFileWith);

    //! Método para parser da saída do hdparm
    /*! Coleta de dados específicos, como serial number e tipo de midia,
     * utilizado para compor o relatório de cadeia de custódia.
     */
    void hdParmParser(QByteArray hdp);

    //! Método para executar o hdparm.
    /*! Executa o hdparm no dispositivo passado como parâmetro. A saída é passada
     * integra para o hdParmParser(QByteArray hdp). O resultado ficará armazenado em um QMap info para ser consultado
     * pela MainWindow posteriormente (nos combobox)
    */
    void hdparmExecutionAndResult(QString deviceToHDParm);

    //! Apenas para pegar o valor de info (que é private) e retorná-lo na MainWindow
    QMap <QString,QString> getHDInfo();

    //! Validador de partição existente
    int isTherePartition(QString device);

    //!Ajuda a gerar informações de operação para a cadeia de custódia
    /*! Com esse recurso o perito não necessitará anotar os horários de utilização
     * de cada recurso, pois desde a abertura do programa será apoiado pela coleta
     * de operações. Ao final, haverá um relatório gerado em texto plano ou html,
     * bastando ajustá-lo ao formato desejado no documento final de perícia.
    */
    void writeOperation();
    //! Diretório padrão para gravação das imagens (futuramente será implementado via QSettings).
    const  QString TARGET_DIR = "/media/images";

public slots:


private slots:
    //! Compõe o tipo de hash selecionado
    /*! Esse slot compõe o comando a gerar hash a partir da seleção do combobox
     * da MainWindow.
    */
    void slotMakeHash();

private:
    //! Variável para receber o comando binário fdisk ou parted.
    /*! Utilizada no método partedAndFdisk para receber alternadamente ambos
     * os comandos binários.
    */
    QString comm;

    //! Parser do particionamento
    /*! QMap para parser do particionamento de discos, utilizada no
     * método PartedParser(QByteArray fdiskResults)
    */
    QMap <QString,QString> partitions;

    //! Variável para receber a saída do fdisk e parted
    /*! Essa variável recebe toda a saída do qprocess utilizado no
     * método PartedAndFdisk, depois é exibida na caixa de texto.
    */
    QByteArray allPartitionsFromDevice;

    //! Utilizada no parser do método makeHash
    QStringList varToHash;

    //! Aba "Ler imagem DD" - Opção "Gerar hash".
    /*! Quando marcada essa opção (Gerar hash), a ação é executada pelo método @getHash(), invocado pelo método
     * @makeHash(), e gravado nessa variável.
     * O Hash gerado é exibido no QDialog invocado desse mesmo método.
    */
    QString hashResult;

    //! Parser da saída do comando binário Parted
    QString PartedParser(QByteArray fdiskResults);

    //! Alvo para salvar ou ler imagens
    /*! Essa variável deve ser passada no construtor da classe, quando
     * estanciando-a. Servirá como referência em qualquer parte onde seja
     * necessário especificar a origem ou destino para imagens.
     * Inicialmente, implementado apenas para dcfldd.
    */
    QMap <QString, QString> varsEnv;

    //! QMap para relação de informe dos dispositivos encontrados e exibidos na aba HDDs/SSDs, do menu esquerdo da GUI.
    QMap <QString,QString> info;

    //! Agenda de tarefas pós processamento, por exemplo geração de hash, caso não seja a primeira aba do tabwidget
    QStringList tasks;

    //! Descoberta de memória USB
    /*! O método HDParmParser insere serial e modelo em um QMap para alimentar os labels na aba de cópia. Se o dispositivo não for reconhecido
     * pelo HDParm, então provavelmente é um pendrive ou um cartão de memória. Nesse caso, é necessário identificá-lo de outro modo.
     */
    void figureOutAssociatedDevice(QString device);


signals:
    //! Envio das mensagens a exibir via popup.
    void signalMsgBox(QStringList msg);
    //! Emissor das mensagens a gravar no log. O método receptor log() está declarado na MainWindow.
    void log(QByteArray msg);

};

#endif // COMMON_H
