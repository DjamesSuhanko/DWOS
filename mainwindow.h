#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QListWidgetItem>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QTextCursor>
#include <QInputDialog>
#include <QShortcut>
#include <QKeyEvent>
#include <QMetaType>
#include <QFontDatabase>
#include <QImage>
#include <QPixmap>
#include <QWidget>
#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QTranslator>
#include <QSignalMapper>
#include <QSettings>
#include <QFileSystemModel>
#include "common.h"
#include "wiping.h"
#include "gptandformat.h"
#include "systeminfo.h"
#include "bitwisecopy.h"
#include "graphrealtime.h"
#include "popup.h"
#include "fingerprintandtraces.h"
#include "documentation.h"
#include "screenshot.h"
#include "screencast.h"
#include "progressbarundefinedvalue.h"
#include "autorenameshot.h"
#include "ewfrelat.h"
#include "expertwitnessformat.h"
#include "encrypt.h"
#include "iostats.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Common * common;
    Wiping *wiper;
    GPTandFormat * gpt;
    SystemInfo * sysInfo;
    BitWiseCopy * bitwise;
    FingerPrintAndTraces * fingerprint;
    documentation *doc;
    Screenshot * screenshot;
    Screencast * screencast;
    ProgressBarUndefinedValue * pbarUndefVal;
    AutoRenameShot * autoRenameShot;
    ewfRelat * ewf;
    ExpertWitnessFormat * ewfAcquiring;
    Encrypt * crypt;
    IOStats * iostat;

    GraphRealTime * graph;
    QSignalMapper * signalMapperStarted;
    QSignalMapper * signalMapperFinished;

    //! Objeto que realiza a traducao
    QTranslator translator;

    //!Arquivo contendo o nome da imagem selecionada
    /*! É alimentada na conexão do PushButton_selecionar e seu contúdo é lido no método colectHashInfo()*/
    QStringList files;

    //! Variável para guardar o path do arquivo de destino
    QString targetToSave;

public slots:


    //! Clicando no listWidget, responderá as informações de partição e disco na aba "Detalhes"
    /*!
     * \brief partitionClicked
     * \param item
     * Esse slot tem relação direta com o ListWidget.
     * O ListWidget é alimentado com a lista de dispositivos encontrados em outro método.
     * Um duplo-clique ativa esse slot, que alimenta o textBrowser da aba "Detalhes".
     * Os dados que serão encontrados nesta aba, se disco:
     * Informações do tamanho do dispositivo, serial, partições e modelo
     * Se clicado em uma partição:
     * Tamanho, tipo e, quando possível, sistema de arquivos
     */
    void slotPartitionClicked(QListWidgetItem *item);

    //! \brief Método inicial para leitura de imagem
    /*! Esse método inicial libera diversas funcionalidades da MainWindow logo após
     * a seleção de um arquivo de imagem, podendo ser *.dd, *.img, *.dump, *.iso.
    */
    void slotSelectImage4read();

    //!set val to pbar
    void slotSetProgressBarValNow(int val);

private:
    Ui::MainWindow *ui;

    //! Guarda o PID do processo principal (Ainda não implementado o uso)
    QString PID;

    //! Instância de imagem para o visualizador de screenshots
    QLabel *imageLabel1;
    //! Instância de imagem para o visualizador de screenshots
    QLabel *imageLabel2;
    //! Instância de imagem para o visualizador de screenshots
    QLabel *imageLabel3;
    //! Instância de imagem para o visualizador de screenshots
    QLabel *imageLabel4;

    //! Diferenciador do tipo de wipe (hotwipe ou fullwipe)
    /*! A thread wipe executa dois tipos de wipe, sendo o completo, aplicado  diretamente
     *  sobre o dispositivo ou, o hotwipe - aplicado sobre o sistema de arquivos da midia
     *  de destino para eliminar dados residuais quando  aplicada  criptografia  sobre  a
     *  imagem gerada.
    */
    bool hotWipe = false;

    //! Lista de imagens do visualizador de screenshot
    QStringList imageName;

    //! Idioma pré-definido até que seja feito o load do arquivo settings
    QString lang="pt";

    //! Tempo de delay dos disparos automáticos do screenshot
    int shotInterval;

    //! Boolean de idioma, após seleção na tela inicial
    bool langSelected=false;

    //! Mapa de sinais dos botões de idioma
    QSignalMapper * signalMapperLang;

    //! Mapa de sinais da árvore de diretórios
    QSignalMapper * signalMapperTreeView;


    //QString generalMsgs;

    //! Método de alimentação de widgets relacionados à geração de imagens EWF
    void comboboxFeed();

    //! Carga de alguns parâmetros provenientes do arquivo dwatson.ini
    void Settings();

    //! controle de alguns widgets durante mundaça de idioma
    void preserveOnReload();

    /*! Compositor da string de comando, baseando-se em todos os valores de widgets relacionados
     * à geração de imagens EWF
     */
    QString mountCommandToEWFthread(QString fileNameSelected);

    //! Copia dos logs para um dispositivo externo
    void copyLogsTo();

    //! Copia recursiva dos diretórios passados via stringlist, utilizado atualmente para cópia dos logs
    void copyFilesAndDirs(QStringList dirs, QString target);

    //! String de tarefas monitoradas
    QString tasks;

    //QTimer *timer2;

    //! Armazena o nome selecionado na geração de cópia bitwise. Essa variável é utilizada pela geração de base de strings e hash.
    QString ImageNameGeneratedInBitwiseCopy;

    //! Método auxiliar para a criação de hash e base de string ao término da cópia bitwise
    /*! Esse método é chamado dentro do método slotSetProgressBarValNow, assim que recebe 100%. Nesse momento, o programa
     * avalia os checkbox do rodapé e executa a respectiva tarefa.
    */
    void checkout(int tab);

    //!  Cálculo de velocidade de transferência
    /*! Esse método faz o cálculo da velocidade de transferência, dependendo da ferramenta utilizada, pois algumas
     * devolvem automaticamente o valor médio em MB/s. No segundo caso, apenas faz parsing da mensagem.
    */
    void parseValueToGraph(QString strVal, QString toolUsed);

    //! Reset do gráfico para iniciar nova amostragem
    void resetGraph();

    //! Memória do ultimo valor
    float lastVal = 0;

    //! Permissão para pegar amostra
    bool getSample = false;

    //! Limite inicial de MB/s do gráfico
    int limit = 10;

    //! Marcador de posição para o gráfico de tempo-real
    int j = 0;

    //!
    //! \brief targetIsWriteable
    //! \return
    //! Verifica se é possível escrever no destino após montado e muda para rw se estiver como ro.
    bool targetIsWriteable(QString path, QString dev);

    //! Atribui o valor de datetime para os labels de operação de cópia.
    QString dateTimeNowString();
    //! Status do processo de copia bitwise
    int exitCode = 2;
    //! Ativa ou desativa as tabs e tools até que datatime seja confirmado.
    /*! Ao iniciar o programa, não deve haver navegação até que o horário seja ajustado.
     * Passando true ou false ativará ou desativará esses componentes respectivamente.
    */
    void enableDisableItems(bool trueFalse);

    //! Gerador de datetime, utilizado principalmente pelo gerador de log.
    QString makeTimeStr();

    //! Exibir log na janela de detalhes
    /*! Exibe as mensagens sendo geradas pelo log na janela de detalhes se não estiver
     * sendo exibido dados de criação da base de strings.
    */
    bool showLogInDetailWindowIfStringsOff;

    //! Alimenta o combobox de arquivos de sistemas suportados para formatação.
    void comboBoxFS();

    //! Ativar os componentes de rodapé da janela
    /*! Os dois lugares para habilitar os widgets abaixo do tabwidget são (i) respectivamente
     * a aba de leitura de strings da imagem raw e (ii) a aba de bitwise, após selecionar o
     * dispositivo de destino (nesse momento, a origem também é verificada por questões de
     * consistência). Para não repetir o conjunto de widgets a habilitar/desabilitar, esse método
     * deverá ser invocado com o parâmetro booleano que definirá a ação a tomar.
     * Esse método é chamado dentro do slot de seleção de imagens e no slot do combobox de
     * destino.
    */
    void enableFooterWidgets(bool condition);

    //! Validação das variáveis a passar através do botão 'Coletar'
    /*! Quando selecionada a função de geração de cópia bitwise, diversas variáveis necessitam ser
     * validadas. Esse método verifica essa consistência e passa os valores das variáveis para dentro
     * da thread através da chamada de um método de alimentação.
    */
    int bitwiseValidator();

    //! Alimentação dos labels de bitwise
    void labelsFeed(QString srcOrDst);

    //! Executa o comando para escrita ou leitura do dispositivo em questão
    void setDeviceToReadOnly(QString device, bool setRO);

    //!Método para alimentar os radio buttons da aba Monitor do tabWidget
    void setRadioTextTo(QString srcOrDst,QString value);

    //! Validação do dispositivo de origem como sendo somente-leitura
    void justCheckIfIsReadOnly(QString dev);

    //!Mudanças nos componentes do monitor
    /*! As mudanças nos componentes do monitor são efetuados a partir dos eventos da aba 'Gerar Imagem Forense'. Esse método efetiva algumas
     * dessas mudanças nos labels e calcula as porcentagens para o progressbar de origem e destino, também da aba 'Monitor'.
     */
    void monitorSettings(QString srcOrDst);

    //! Método utilizado para alimentar o progressbar e espaço livre das mídias selecionadas na aba imagens
    int getAndShowDiskFreeInProgressBarMonitor(QString device);

    //! Método utilizado apenas para reduzir o número de linhas no construtor.
    void connections();

    //! Método utilizado apenas para reduzir o número de linhas no construtor.
    void toolTips();

    //! Método que trata da geração de imagem forense ao clicar em Coletar.
    /*! Esse método é chamado quando clicado em Coletar, caso a tarefa seja de sua responsabilidade. Esse controle é feito pelo método slotPushButtonColect.
     * É possível que o usuário selecione todos os ítens e então mude de tab. Esse caso já é tratado como abandono de
     * tarefa.
    */
    int makeRawFile();

    //! Coletor de parâmetros para geração do hash para o dcfldd
    void colectHashInfo(int tab);

    //! Cancelar qualquer operação em execução
    /*! Quando iniciada qualquer das tarefas, dois controles são necessários:
     * 1 - Texto do botão de coleta deve ser Coletar ou Cancelar
     * 2 - As tarefas devem então ser iniciadas ou finalizadas respectivamente.
     * A melhor opção é controlar em um único local invés de distribuidamente. Para tal, esse método faz a tarefa de cancelar
     * as threads e seus respectivos qprocess. Os demais controles são feitos no slotPushButtonColect.
    */
    void cancelAllRunningProcess();

    //! Reinicia todos os parâmetros da aba de geração de imagem
    /*! Quando clicado em cancelar, todos os parâmetros serão reiniciados para poder iniciar um novo processo
    */
    void resetParams();

    //! Guarda o valor da ferramenta selecionada para ser utilizada pelo método makeRawFile.
    QString bitwiseSelectedTool;

private slots:

    //! Criptografia em arquivo selecionado a partir do treeview da aba de leitura de imagens
    void slotPushButtonEncrypt();

    //! Preparação para início da criptografia
    void encrypt();

    //! Treeview do diretório de imagens e logs, utilizado na aba de imagens
    void slotTreeViewItemSelected(QModelIndex model);

    //! Slot para remoção de arquivos selecionados, quando clicado no pushbutton da aba de leitura de imagens
    void slotRemoveFile();

    //! Visualizador do arquivo hash de um determinado arquivo selecionado através da aba de leitura de imagens
    void slotHashView();

    //! Exibição de informações de imagens EWF, na aba de leitura de imagens
    void slotEwfInfo();

    //! Controle de cliques do treeview, da aba de leitura de imagens
    void slotTreeView(QString signalSource);

    //! Visualizador de screenshots, exibido na aba de leitura de imagens
    void shotsViewer();

    //! Alimentador da base de imagens a exibir no visualizador de screenshots
    void slotImageFeed(QString imgName);

    //! Visualizador da tabela de partições, quando possível, utilizado na aba de leitura de imagens
    void slotPartitionsView();

    //! Montagem do combobox com todos os dispositivos encontrados no sistema operacional
    void slotAllDevsAndPartsToImgRead();

    //! Montador do dispositivo de destino
    void slotMount(QString target);

    //! Timer, previamente ao disparo do screenshot. Configurável via dwatson.ini
    void slotChooseTimeIntervalToScreenShot();

    //! Muda o idioma, se definido no arquivo ini
    void slotLangFromIniFile();

    //! Aplica idioma
    /*! A alimentação do radio button de idioma é executada inicialmente a partir da definição dos settings.
     * A partir dele, o valor de idioma é definido e o radio button correspondente ao idioma é marcado.
     * Esse recurso é executado no construtor. Posteriormente, os radio buttons são conectados a esse slot,
     * que é chamado ao acontecer um evento de mudança de estado.
    */
    void translate2(QString locale);

    //! iostats
    void slotStatsToGraph(QString value);
    //! iostats
    void slotIOStarted(QString sigVal);
    //! iostats
    void slotIOFinished(QString sigVal);

    //! Configura o formato de exibição do campo de senha
    void slotSetPasswdEchoMode();
    //! Controle da senha
    void slotPass(QString txt);
    //! Controle de confirmação da senha
    void slotPassConfirm(QString txt);
    //! Receptor do status de finalização do método de criptografia
    void slotCryptFinished();
    //! Método de questionamento sobre hot wipe em setores desalocados
    void slotWipeFreeSectors();

    //! Status de inicializção da thread de criptografia
    void slotCryptStarted();

    //! EWF - Modificações de posicionamento do stack da ferramenta de controle EWF
    void SlotChangeStackTabNext();
    //! EWF - Modificações de posicionamento do stack da ferramenta de controle EWF
    void SlotChangeStackTabMiddle();
    //! EWF - Modificações de posicionamento do stack da ferramenta de controle EWF
    void SlotChangeStackTabFirst();
    //! Aplicação dos valores recebidos através do formulário da ferramenta EWF
    void slotReceivedFromForm(QString msg);

    //! Execução da classe do formulário
    void slotOpenEWFform();

    //! Slider de ajuste EWF
    void slotSliderEvidenceSegment(int val);
    //! Slider de ajuste EWF
    void slotSliderBytesPerSector(int val);
    //! Slider de ajuste EWF
    void slotSliderDataBlock(int val);
    //! Slider de ajuste EWF
    void slotSliderGran(int val);
    //! Slider de ajuste EWF
    void slotSliderRetriesOnError(int val);

    //! Controle visual dos componentes relacionados ao tipo de criptografia
    void slotUnchecker();

    //! Slot de controle de salvamento dos logs, incluindo screenshot, screencast e qualquer outro contido em /var/log do sistema operacional.
    void slotSaveLog(QString target);

    //! Monta comando para screenshot próprio
    void screenShort();
    //! Auto-screenshot
    void screenShotSelfShot();
    /*! Há diferença na captura de tela entre um sistema convencional e um sistema utilizando DRM com Wayland, de forma que o Qt só tem capacidade
     * de capturar a janela em caso de estar rodando no sistema embarcado
     */
    void shotWindowOnly(QString name);
    //! Renomeador do screenshot
    void screenshotRename();
    //! Configuração do nome do botão do screencast converter, conforme o status
    void slotPushButton_wayland_convert_txt(QString text);
    //! Mensagem de finalização da conversão de casting e remoção do arquivo original
    void slotCastConvFinished(QString msg);

    //! Conversor do casting para formato de video tocável
    void slotConvertCast();
    //! Inversor de direção do progressbar indefinido
    void slotInvert(bool onOff);

    //! Manipulação dos arquivos de log
    /*! Na aba de log do toolbox, as opções são escolhidas nos radio buttons. Esse slot está conectado ao pushbutton
     * relacionado à verificação destes radiobuttons, e toma a ação conforme a escolha.
     */
    void slotLogHandler();

    //! Alimentador do label de tarefas em execução
    /*! Esse slot é disparado pelo sinal de um QTimer no construtor da classe MainWindow. Previamente ao início das threads,
     * é feita a inserção do respectivo nome de tarefa no label de tarefas, na aba Monitor. Esse slot verifica esse label a
     * cada chamada do QTimer e se encontra nomes de tarefas, analisa se esta respectiva thread ainda está em execução. EM caso
     * de não estar em execução, remove seu nome do label de tarefas.
     */
    void checkTasks();

    //! Slot para unir os comandos que compõe a escrita de log em um único arquivo
    /*! Cada método deverá estar conectado a este slot a partir da MainWindow.
    */
    void log(QByteArray logType);

    //! Lista de mensagens para rodar o msgbox dos eventos da thread
    void slotRunMessageBox(QStringList msgs);

    //! Slot para iniciar o wipe a partir do checkbox
    /*! Não há motivos para processar em batch por dois motivos:
     * 1 - Nada poderá ser esquecido no método controlador
     * 2 - Se juntar todos os processos, não será possível utilizar ferramentas independente do processo final.
     * Portanto, ao clicar em wipe, um messagebox é exibido apenas para confirmar a ação. Considera-se agora a pré-seleção
     * do dispositivo no destino.
    */
    void slotWipeNow();

    //!Recebe  mensagem da thead wiping
    void slotMsgFromWiper(QString msg);

    //! Ativador de componentes da MainWindow
    /*! Slot de ativação de componentes da MainWindow, ativado ao selecionar
     * o checkbox de criação de hash.
    */
    void slotEnableHashes();

    //! Ajuste de hora antes de iniciar as operações
    void slotSettime();

    //! Apenas troca a posição do toolBox quando selecionar a aba de bitwise
    void slotChangeToolBoxTab(int pos);

    //! Slot de alimentação do destino de cópia de baixo nível.
    /*! Esse slot é necessário por 2 motivos; (i)  habilitar o combo de destino apenas quando o evento de mudança na lista de origem
     * ocorrer e (ii), analisar se a mudança voltou a ser para o item "Dev".
     * A validação é pelo index (0, nesse caso).
    */
    void slotTargetBitwiseComboHandler(int val);

    //! Slot do combobox de destino.
    /*! Esse slot é o mais importante na interface de cópia bitwise. Ele libera todas as opções bloqueadas da interface
     * e habilita o modo de operação de clonagem de baixo nível.
    */
    void slotComboboxDst(int val);

    //! Função exclusiva de gerar um message box
    /*! Os pushbutton que exibem serial e modelo dos HDs de origem e destino tem um tamanho bastante reduzido. Caso se ache necessário,
     * basta um clique sobre esse pseudo-label para sua exibição em tamanho bem maior em uma janela exclusiva.
    */
    void slotShowingMyTextValueSerialInfo();

    //! Função exclusiva de gerar um message box
    /*! Os pushbutton que exibem serial e modelo dos HDs de origem e destino tem um tamanho bastante reduzido. Caso se ache necessário,
     * basta um clique sobre esse pseudo-label para sua exibição em tamanho bem maior em uma janela exclusiva.
    */
    void slotShowingMyTextValueSerialInfo2();

    //! Função exclusiva de gerar um message box
    /*! Os pushbutton que exibem serial e modelo dos HDs de origem e destino tem um tamanho bastante reduzido. Caso se ache necessário,
     * basta um clique sobre esse pseudo-label para sua exibição em tamanho bem maior em uma janela exclusiva.
    */
    void slotShowingMyTextValueModelInfo();

    //! Função exclusiva de gerar um message box
    /*! Os pushbutton que exibem serial e modelo dos HDs de origem e destino tem um tamanho bastante reduzido. Caso se ache necessário,
     * basta um clique sobre esse pseudo-label para sua exibição em tamanho bem maior em uma janela exclusiva.
    */
    void slotShowingMyTextValueModelInfo2();

    //! Realimentar a tab de dispositivos
    /*! O objetivo desse slot é chamar o método de leitura de HDs para realimentar a tab de HDs,
     * da classe @Commom::connectedDevices().
    */
    void slotRereadDevicesTab();

    //! Slot para desmarcar radioButton do sha
    void slotRadioButtonHashMd5(bool ck);

    //! SLot para desmarcar a opção radioButton do MD5
    void slotRadioButtonHashSha(bool ck);

    //! Slot para marcar o disco como somente leitura ou leitura/escrita
    void slotCheckBoxChangeReadOnly();

    //! Slot para manipulação de tabela de partição e formatação.
    /*! Se for selecionado dispositivo invés de partição, deve-se saber alguns parametros importantes:
     * 1 - É para fazer cópia origem a destino?
     * 2 - O dispositivo contém partições; então: criar nova tabela?
     * 3 - O dispositivo não contém partições, portanto perguntar se deve criar (considerando cópia origem a arquivo)
    */
    void slotFormatMidiaHandler(int index);

    //! Manipulação dos widgets após execução de alguma tarefa que se utilize do progressbar

    //! Alimentação do progressbar da carga de CPU, na aba Monitor
    void slotProgressBarCPU(QString cores,int usage);

    //! Slot de alimentação das variáveis expostas nos labels do groupbox direito, na aba Monitor
    void slotSetMemoryValues(int total, int used, int cache);

    //! \brief Slot para executar um ls forense na raiz do dispositivo selecionado
    /*! O propósito é analisar se existem dados alocados ou excluidos na raiz da mídia selecionada. A saída do comando é exibida de forma
     * integra no textbrowser da aba Monitor, sem detalhes ou tratamentos, os quais poderão ser implementados posteriormente.
    */
    void slotShowTrueTreeFromDevice();

    //!Recebe o sinal das threads, informando quando a tarefa foi finalizada ou iniciada.
    void slotButtonMsg(QString CancelColect);

    //!Apenas exibe uma mensagem relacionada à ferramenta escolhida para cópia forense no label status.
    void slotInfoAboutSelectedTool(QString tool);

    //! Muda a tabwidget para a posição de geração de imagens forense quando a toolbox estiver no index 2.
    void slotChangeTabWidget(int pos);

    //! Slot para definir qual método será chamado ao clicar no pushbutton Coletar.
    void slotPushButtonColect();

    //! Modifica o texto do label relacionado ao slider timeout
    //void slotTimeout(int t);

    //! Modifica o texto do label relacionado ao slider retries
    //void slotRetries(int t);

    //! Receptor de mensagens da thread bitwise
    /*! Esse slot recebe a mensagem formatada emitida pela thread bitwise e alimenta o label status. Além disso,
     * esse slot faz parsing da mensagem para alimentar o progressbar e o gráfico de tempo-real.
     * Antes de alimentar o label ou o gráfico, um if é utilizado para analisar a aba atual; se for a aba Monitor,
     * então alimenta o gráfico; se for a aba Gerar Imagem Forense, então alimenta o label. O progressbar é alimentado
     * sempre pois está visível sempre durante o processo.
    */
    void slotStringStatusFromToolUsedInBitWiseCopy(QString string);

    //! Recebe o status de saída dos qprocess das ferramentas de cópia
    void slotFinishedCopyWithStatus(int exitCode);

signals:
    //! O slot encrypt prepara os comandos para iniciar a thread de criptografia.
    void signalCrypt();
    //! Comunicação com o shot viewer
    void nameFromShot(QString filename);

    //! Capturar tela
    void signalTakeShot(QString filename,bool fire);

    //! Parar processo EWF
    void signalStopEWFprocess();

    //! Parar processo de criptografia
    void signalStopCrypt();

    //! Parar conversão do casting
    void abortConvertion();

    //!ProgressBar para wipe
    void signalPercentOfWipe(int val);
    //! Mensagem de botão
    void buttonMsg(QString msg);

    //!Para encerrar o wipe a qualquer momento, esse signal chama o slot terminateWipe da thread wiping
    void signalTerminateWipe();

    //!Para terminar cópia bitwise a qualquer momento, conectado ao slot stopBitWiseCopyNow da classe BitWise
    void signalStopBitWiseCopyNow();

    //!Envia o valor do slider timeout para a thread bitwise
    void signalTimeout(int t);

    //!Envia o valor do slider retries para a thread bitwise
    void signalRetries(int t);

    //!Envia o sinal para o método que armazena o comando da cópia bitwise
    void signalSetCommandToBitWiseCopy(QString command);

    //!Envia dispositivo e path para a montagem, utilizado na cópia bitwise dispositivo-para-arquivo.
    void signalMountOrUmontAndCheckConditionsOf(QString device, const QString dirPath);

    //! Valores para alimentar o gráfico
    void signalValuesToPlot(int x,int y);

    void signalWriteToFile(bool yesNo);

    //! Emissor de status para o label_status
    void signalLabelStatusSetText(QString);

    //! Exibição de mensagens advindas de todos os métodos
    void signalRunMessageBox(QStringList fire);
};

#endif // MAINWINDOW_H
