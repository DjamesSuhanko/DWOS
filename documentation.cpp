#include "documentation.h"

documentation::documentation(QObject *parent) :
    QObject(parent)
{
}

void documentation::getSection(QString fullLine)
{
    QRegExp rx("(\\d\\.?\\d?\\.?\\d?)");
    int stat = rx.indexIn(fullLine);
    if (stat < 0){
       return;
    }
    QString chapter = rx.capturedTexts().last();

    QString text;
    if(chapter == "0.0"){
        text = tr("<b>Introdução</b><br>\
                <p align=justify><b>Cadeia de Custódia</b> é o primeiro módulo da suite forense <b>Djames Watson</b> , que tem o propósito de auxiliar o perito na coleta de forma a despreocupá-lo em relação a detalhes.<br> \
                Todo o processo é guiado e o comportamento dos componentes da janela tem características específicas com o propósito de auxílio. Portanto, é recomendada a leitura dos recursos do programa previamente a seu uso.<br>\
                <br>\
                A suite é composta por 10 módulos que podem ser adquiridos de forma independente, conforme sua necessidade. Entre os módulos, você encontrará coletor de tráfego de rede, dados seriais e voz.  Também há módulos para auxílio de análise de Phishing, rastreamento, Ransonware e mais!<br>\
                <br>\
                Agradecemos a escolha e desejamos o mais absoluto sucesso em seu caso.</p>");
    }
    else if (chapter == "1.0"){
        text = tr("<p align=justify>O primeiro passo para a utilização do programa é a conferência da data/hora. Sempre que o programa for iniciado, todos os componentes \
da janela estarão bloqueados, exceto o botão de confirmação do horário e a caixa de log.<br>\
Se você ajustar o horário (ou se ele já estiver correto) e clicar em <b>Ok</b> sem marcar a opção de gerar log, uma janela popup será aberta para confirmar \
se não gerar log foi opcional ou por esquecimento. Essa ação é tomada devido à importância da geração de log durante a utilização do programa, cujo log tem \
diversos propósitos (ver documentação sobre logs).<br>\
Após essa etapa, os componentes que estarão disponíveis para pronta utilização serão liberados e os demais, conforme operação (descritos cada qual em seu \
respectivo capítulo).</p>");
    }
    else if (chapter == "2.0"){
        text  = tr("<p style='color:rgb(255, 153, 51);'><b><h3><b>Disposição dos componentes e recursos</b><br></p>");
        text += tr("<p align=justify>Os componentes da janela estão distribuidos em três conjuntos principais:<br>");
        text += tr("<b>1</b> - A caixa de opções e ajustes, à esquerda.<br>");
        text += tr("Esse menu está relacionado a detalhes de operação (descritos em seu respectivo capítulo). Ao gerar uma base de strings por exemplo, ");
        text += tr("o menu <b>Base de Strings</b> se tornará disponível e você terá a opção de visualizar o fluxo dos dados sendo capturados, assim como ");
        text += tr("definir parâmetros previamente ao início da operação.<br>");
        text += tr("<br><b>2</b> - O conjunto de opções globais, chamados 'ítens de rodapé'.<br>");
        text += tr("Esse conjunto de opções é chamado de 'global' por ser aplicável em ambas as operações, geração de base de strings ou geração de imagem ");
        text += tr("raw para posterior análise. Alguns dos componentes tem ação de tempo real, podendo ser habilitado ou desabilitado durante qualquer ponto ");
        text += tr("da execução de um processo.<br>");
        text += tr("<br><b>3</b> - O menu de abas à direita, sobre os ícones de rodapé.<br>");
        text += tr("Esse menu separa o conjunto de componentes necessários para a operação descrita na aba, de forma a simplificar e agilizar o processo ");
        text += tr("inicial. Esse menu é reativo, de forma que os componentes dest aba são inicializados e reinicializados a partir do 'click' no nome da aba.");
        text += tr(" Por exemplo, ao clicar no menu <b>Geração de Imagem</b>, automaticamente a aba <b>Gerar Imagem Forense</b> será colocada em foco, mas seus ");
        text += tr("componentes não serão habilitados até que se clique sobre o nome desta aba.<br><br>");
        text += tr("A razão da mudança de aba automática é para relacionar visualmente os recursos disponíveis para determinada operação, enquanto manter a aba ");
        text += tr("desabilitada até que se clique sobre ela é uma garantia de que 'esse era o objetivo'.<br><br>");
        text += tr("<b>Trocar de aba previamente ao início de uma operação</b> forçará um reinicio ao padrão da aba. Isso significa que, se você entrou na aba <b>Gerar Imagem Forense</b> e escolheu origem, ");
        text += tr("destino e/ou ferramenta, mas <b style='color=red;'>não</b> clicou em <b>Coletar</b> e mudou de aba, ao retornar, todos os parâmetros voltarão ao");
        text += tr(" padrão. Se um processo foi finalizado, a troca de abas também eliminará qualquer dado, exceto os da aba <b>Monitor</b>, que estarão ");
        text += tr("disponíveis até o início de uma nova operação. Essa é a aba ideal para captura de tela a inserir no documento, mas as demais abas também ");
        text += tr("podem ser capturadas sem perda de dados se utilizar a opção de captura automática de tela, disponível na aba <b>Log</b> do menu lateral.<br><br>");
        text += tr("Clicando sobre o nome da aba do menu direito, mesmo que na aba selecionada, os parâmetros serão reinicializados. O objetivo é auxiliá-lo de ");
        text += tr("forma que não haja erros durante uma coleta, ou seja, se a operação não for definida linearmente, deve-se iniciar todo o processo novamente.<br>");
    }
    else if (chapter == "2.1"){
        text  = tr("<p align=justify>A responsividade interativa é a capacidade que o programa tem de perceber a operação pretendida. Dessa forma, os respectivos recursos da ");
        text += tr("interface são habilitados, desabilitados, reiniciados e alguns deles são executados em tempo real, como por exemplo o wipe.<br>");
        text += tr("É importante adaptar-se ao relacionamento com a inteligência do programa, que tem o propósito de guiá-lo afim de reduzir erros de operação.");
    }
    else if (chapter == "2.2"){
        text  = tr("<p align=justify>Como citado no documento anteriormente, existem duas ocasiões onde a interface retorna aos valores padrão. Uma delas é ao fim de um ");
        text += tr("processo, de forma que as variáveis do ambiente são reiniciadas, portanto a interface deixa de ser informativa nesse momento.<br>");
        text += tr("A segunda ocasião (e a mais comum) é o momento de uma troca de abas sem que algum processo tenha sido iniciado. Isso significa que, ");
        text += tr("se você modificou valores de componentes gráficos em uma aba e resolveu mudar de aba, os valores modificados voltarão ao padrão.<br>");
        text += tr("A única excessão é a aba <b>Monitor</b>, que preserva os dados da operação anterior até que qualquer componente seja modificado em outra ");
        text += tr("aba de processo.");
    }
    else if (chapter == "3.0"){
         text  = tr("<p align=justify>Os componentes da janela serão descritos a seguir conforme o conjunto que o representa, assim como suas relações com outros componentes,");
         text += tr(" de forma que algumas informações poderão parecer redundantes.<br>");
         text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Abas do menu esquerdo</h3></b><br></p>");
         text += tr("<b>Relógio</b><br>");
         text += tr("Ao iniciar o programa, toda a interface estará indisponível até que seja confirmada a data e hora, que pode ser ajustada previa ou ");
         text += tr("posteriormente. A melhor opção é ajustar o horário antes de iniciar a interface, porque se optar por gerar o log, isso fará diferença ");
         text += tr("nos registros.<br<br>");
         text += tr("Ainda que confirmado o horário e a data exibidos na interface inicial, o programa executará o comando de ajuste do relógio ");
         text += tr("ao clicar em <b>Ok</b><br><br>");
         text += tr("É fundamental que esse ajuste seja feito devido à necessidade de validação das provas durante a perícia. Certamente o horário ou data ");
         text += tr("errados invalidará as evidências, portanto evite deixar máculas.<br><br>");
         text += tr("<b>Log</b><br>");
         text += tr("Essa aba tem recursos extras em relação à geração de log, que fundamentalmente deverá estar marcado a utilização de alguns desses ");
         text += tr("recursos. Os componentes dessa aba são:<br>");
         text += tr("Visualização<br>");
         text += tr("Esse recurso lhe permitirá visualizar as linhas de log que estão sendo registradas, na aba <b>Ler Imagem</b>.<br>");
         text += tr("Esse recurso pode ser habilitado e desabilitado a qualquer momento, porém não atuará em conjunção à geração de base ");
         text += tr("de strings, descrita mais adiante.<br><br>");
         text += tr("Eu quero...<br>");
         text += tr("Essa é uma opção para visualização, cópia ou remoção dos arquivos de log, disponível durante todo o processo, porém não atualizado ");
         text += tr("em tempo real.<br>");
         text += tr("Extras<br>");
         text += tr("Aqui estão disponíveis as opções de gerar um relatório pré-formatado para aplicação de conteúdo no documento oficial, assim como ");
         text += tr("a opção de screenshot, tirado automaticamente da tela a cada operação entendida pelo programa como necessária. Um bom exemplo é ");
         text += tr("o final da operação de cópia. Por estar finalizado, uma alteração ou recarga da aba excluirão os dados exibidos, mas com o recurso ");
         text += tr("de screenshot habilitado, uma captura de tela é feita previamente à limpeza da aba correspondente.<br><br>");
         text += tr("<b>Geração de Imagem</b><br>");
         text += tr("Essa aba é auxiliar à aba <b>Gerar Imagem Forense</b>, do menu direito, e quando selecionada, automaticamente ativa essa aba ");
         text += tr("correspondente, apenas para firmar a relação visual entre ambas. Ainda que selecionada, os componentes da aba <b>Gerar Imagem Forense");
         text += tr("</b> só estarão disponíveis após a ativação por clique no nome dela própria, como descrito anteriormente. Esse comportamento certifica ");
         text += tr("a intenção de utilização do recurso, de forma que o programa entende a necessidade de gerar log das ações nessa aba a partir de então.");
         text += tr("<br>A opção Origem à Destino indica cópia direta de um a outro. Esse tipo de cópia é ideal para casos de mídias do mesmo tamanho.<br>");
         text += tr("A opção Origem a Arquivo é o modo comum de cópia e o modo de operação padrão da ferramenta.<br>");
         text += tr("A proteção da origem é o comportamento padrão da ferramenta, podendo ser desabilitado a critério.<br>");
         text += tr("A opção de Wipe só estará disponível a partir do momento que a origem e destino forem selecionados e válidos.<br>");
         text += tr("<br><br><b>HDDs/SSDs</b><br>");
         text += tr("É muito comum que surjam duvidas em relação à seleção do disco no momento da operação. Tomando por base que o serial da mídia de destino ");
         text += tr("tenha sido anotada previamente, identificá-la (a ela ou outra mídia, como a origem) pode ser fundamental nesse momento. A maneira mais ");
         text += tr("rápida de descobrir ou reconhecer as memórias disponíveis no ambiente é através desse menu, apenas clicando sobre um dos componentes, ");
         text += tr("não importanto se partição ou disco, pois o programa entenderá isso automaticamente.<br>");
         text += tr("O retorno do clique é uma janela com as informações sobre serial, modelo do disco, tamanho e alguns extras.<br><br>");
         text += tr("<b>Base de Strings</b><br>");
         text += tr("Essa aba só será habilitada após a escolha de origem, destino e a caixa de checagem <b>Gerar Base de Strings</b>. Esse comportamento ");
         text += tr("faz parte da lógica do programa; somente é possível gerar base de strings se um destino tiver sido selecionado e só é possível ");
         text += tr("selecionar um destino se uma origem tiver sido selecionada previamente.<br>");
         text += tr("Essa aba oferece recursos de tempo-real e recursos de pré-operação. Iniciando pela pré-operação, é fundamental definir previamente o ");
         text += tr("tamanho mínimo daquilo que deverá ser considerado como string (string é um conjunto de letras e/ou números que possam representar ");
         text += tr("algum valor para a perícia; um possível artefato). Após iniciada a operação, esse componente não surtirá mais efeito. 'Ver em detalhes' ");
         text += tr("exibirá os valores coletados na aba <b>Ler da Imagem</b>, 1 linha a cada X, onde X é um valor selecionado no slider, sendo 50 o valor");
         text += tr(" padrao mínimo e 999 como valor máximo. O campo 'Entradas' é o número máximo de linhas a exibir na tela. Isso porque certamente serão ");
         text += tr("encontradas centenas de milhares de linhas e não seria viável carregá-las todas para a memória, portanto esse seletor é um limitador ");
         text += tr("afim de preservar a memória.<br>");
         text += tr("<p style='color:rgb(255, 153, 51);'><b></b><h3>Abas do menu direito</h3><br></p>");
         text += tr("As abas desse menu são reativas, tornando aos valores padrão dos componentes quando clicado sobre o nome da aba, ainda que já estando ");
         text += tr("nela, exceto uma operação esteja em execução.<br><br>");
         text += tr("<b>Ler da Imagem</b><br>");
         text += tr("Existem algumas formas diferentes de executar uma perícia sobre o disco, das quais cita-se aqui a análise na superfície do sistema de ");
         text += tr("arquivos, utilizando-se de ferramentas que interajam com os respectivos sistemas de arquivos em busca de dados nos inodes do disco.<br>");
         text += tr("Quando não é possível a utilização dessa técnica, aprofunda-se em uma técnica mais arriscada, com menor índice de sucesso, chamado de ");
         text += tr("'data carving', onde  busca por arquivos vão além do sistema de arquivos, procurando por cabeçalhos e rodapés dos respectivos arquivos, ");
         text += tr("de forma que o arquivo deverá estar alocado linearmente na mídia alvo. Finalmente, uma técnica que pode trazer informação útil e ");
         text += tr("fundamental, porém sem formatação, sem definição de sua origem. Unindo o resultado a um conjunto de técnicas, certamente haverão outras ");
         text += tr("opções de análise, baseando-se em 'artefatos primitivos'. É para esse último tipo de análise que foi incluido o recurso de geração de base");
         text += tr("de strings.<br><br>");
         text += tr("O recurso está disponível para imagem pré-existente ou para imagem a ser gerada. Não é uma tarefa executada paralelamente à geração da imagem ");
         text += tr(" devendo então considerar que trata-se de uma segunda tarefa, iniciada somente ao término da geração de imagem, e nem sempre será possível ");
         text += tr(" executar essa tarefa no momento da coleta. Por isso a geração da base está disponível também para uma imagem pré-existente, assim você ");
         text += tr("poderá executá-la em seu laboratório.<br>");
         text += tr("Essa tarefa depende da checagem do ítem <b>Gerar Base de Strings</b>, dos componentes do rodapé.<br><br>");
         text += tr("<b>Gerar Imagem Forense</b><br>");
         text += tr("Essa aba contém todos os recursos necessários para a coleta. A seleção mínima de componentes necessário para uma operação é:<br>");
         text += tr("- Origem<br>");
         text += tr("- Destino<br>");
         text += tr("- Ferramenta<br><br>");
         text += tr("Após essa seleção mínima, já pode-se iniciar a coleta, considerando que:<br>");
         text += tr("<b>a</b> - A origem não pode ser maior que o destino.<br>");
         text += tr("<b>b</b> - A origem não deve ser igual ao destino, exceto tenha sido selecionada cópia origem-a-destino no menu lateral esquerdo.<br>");
         text += tr("A origem será automaticamente protegida contra gravação, como pode-se notar no menu lateral esquerdo. Caso alguma anomalia não permita ");
         text += tr("a proteção da mídia, uma mensagem saltará na tela informando essa condição. Isso não impossibilita a cópia, porém é uma prática comum ");
         text += tr("ter a origem protegida previamente à geração da imagem.<br>");
         text += tr("O destino também é verificado, para saber previamente se é possível gravar. Em caso de anomalia, uma mensagem é exibida e a operação não ");
         text += tr("poderá ser continuada até que a mídia seja substituida.<br>");
         text += tr("<br>A opção Formatar<br>");
         text += tr("Mesmo havendo partição previamente, ao selecionar a formatação toda a tabela será excluida e recriada no esquema GPT e formatada com o ");
         text += tr("respectivo sistema de arquivos. Cada sistema de arquivos oferece um nível de verbosidade diferente, isto é, a informação visual variará ");
         text += tr("conforme o sistema escolhido. Se o processo estiver consideravelmente lento e não informativo, mude para a aba <b>Monitor</b>. Essa aba ");
         text += tr("faz uma atualização a cada 5 segundos, apesar de todas as tarefas dinâmicas executadas no sistema de arquivos.<br>");
         text += tr("Se o novo dispositivo não aparecer após a formatação, apenas faça o recarregamento da aba de imagens, clicando novamente sobre seu nome ");
         text += tr(" no rodapé.<br>");
         text += tr("<br>Serial e Modelo<br>");
         text += tr("Quando a mídia é selecionada, a informação correspondente ao serial e modelo/marca são carregados nos respectivos campos. Caso não seja ");
         text += tr("possível obter tal informação, uma mensagem de substituição deixará claro o ocorrido. Se o dispositivo for USB, seu endereço no barramento");
         text += tr(" do sistema será utilizado para preencher ambos os campos. Clicando sobre o campo, uma janela com o texto maior e caixa de detalhes será ");
         text += tr("exibida, e é uma ótima opção aos que primam pelo detalhes informativos.<br>");
         text += tr("<br>Mensagens de Operação<br>");
         text += tr("Logo abaixo das caixas de origem e destino, todas as mensagens de background das ferramentas utilizadas e algumas mensagens de status do ");
         text += tr("programa são exibidos, servindo como indicador de progresso (em ocasiões, exibindo inclusive a porcentagem do processo) e condição de ");
         text += tr("operação do programa.<br>");
         text += tr("<br>Ferramentas de Cópia<br>");
         text += tr("As ferramentas de cópia disponíveis são DCFLDD, DC3DD e DD. Todas são ferramentas de qualidade, confiáveis e cada qual com suas ");
         text += tr("características. Não é mérito da questão indicar melhor ou pior. Apenas deve-se considerar a cópia raw integra e confiável para a ");
         text += tr("perícia e, tendo como propósito principal a integridade dos dados na sua mais pura forma, todas as 3 ferramentas são aplicáveis na ");
         text += tr(" área forense.<br>");
         text += tr("Essa caixa de ferramentas só estará disponível após escolher origem e destino, obedecendo aos critérios supracitados.<br>");
         text += tr("<br><b>Monitor</b><br>");
         text += tr("Essa aba contém o máximo de informações possíveis, não apenas de operação, mas também da saúde do sistema quanto a consumo de memória, ");
         text += tr("processamento e cache.<br>");
         text += tr("Na caixa da esquerda, uma prévia de origem e destino são exibidos, como o tamanho e área consumida destas mídias, além do serial.<br>");
         text += tr("Acima das caixas, data e hora de início e fim de operação são marcados em seus respectivos momentos. À direita, as tarefas de segundo ");
         text += tr("plano, executadas pelo programa. Esse recurso é bastante importante para diagnósticos de anomalía não previstas, por exemplo, na ");
         text += tr("interrupção de um processo de segundo plano como cópia.<br>");
         text += tr("Abaixo das caixas citadas, um gráfico que exibe entrada e saída da operação baseando-se em um conjunto de condições:<br>");
         text += tr("- Dados lidos da origem dentro do ciclo de processamento.<br>");
         text += tr("- Dados lidos do cache iniciado pela ferramenta de background.<br>");
         text += tr("- Dados lidos no barramento de sistema na intercomunicação entre o processo de background e a interface do programa.<br>");
         text += tr("- Tempo de processamento da informação até sua exibição em video, considerando resposta da interface gráfica e placa de video.<br>");
         text += tr("Considerando o conjunto de variáveis, não é viável parametrizar desempenho de operação utilizando-se desse gráfico, cujo propósito ");
         text += tr("é exclusivamente manter-lhe informado quanto a momentos de gargalo, defasagem na operação, anomalias que venham a causar congelamentos, ");
         text += tr("momento de determinado problema.<br>");
         text += tr("Esse gráfico pode indicar por exemplo, um problema físico em endereçamentos da mídia de origem (ou destino). Uma falha no mesmo ponto em ");
         text += tr("uma repetição do processo poderia indicar badblocks, devendo-se então adotar diferentes procedimentos para a cópia.<br>");
         text += tr("Por fim, esse gráfico não representa tempo, mas dados lidos no intervalo de tempo dentro de seu ciclo.");
         text += tr("Uma das ferramentas lê em blocos de tamanho fixo, gerando um padrão estável no gráfico. Nesse caso, sua constância também vale como ");
         text += tr("indicador de estabilidade.<br>");
         text += tr(" É muito comum também que quando lendo de memórias flash, haja um padrão linear, característico de leitura digital.<br>");


    }
    else if(chapter == "3.1"){
        text  = tr("<p align=justify>Os componentes de rodapé estão sempre dispostos porque podem ser utilizados em duas condições diferentes; ");
        text += tr(" na geração de base de strings (a partir de seleção de imagem na primeira aba do menu direito) ou conjuntamente ao processo ");
        text += tr("de geração de imagem forense.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Gerar Hash</h3></b></p>");
        text += tr("<p align=justify>Aplica-se a uma imagem pré-existente, de forma que sua assinatura digital já não sofrerá mais modificações.</p> ");
        text += tr("<p align=justify>Quando selecionado conjuntamente ao processo de geração de imagem forense, primeiramente a imagem é gerada e posteriormente seu hash.</p>");
        text += tr("<p align=justify>Algumas das ferramentas oferecem a geração de hash, que poderia ser ativado através de parâmetro de comando. Porém não é uma operação ");
        text += tr("mágica nem de adivinhação. É necessário que o arquivo esteja devidamente registrado em disco para que seja possível compor sua assinatura, ");
        text += tr("portanto não há ganho em passá-lo como parâmetro ou sequência de tarefa.</p>");
        text += tr("<br><b>Tipos de hash</b>");
        text += tr("<p align=justify>A opção padrão é o sha1, com opções até sha512, a ser escolhido no combo de opções ao lado esquerdo do tipo de hash.");
        text += tr("O MD5 está disponivel por ser bastante rápido e efetivo. Já foi comprovado que há colisões do hash gerado por MD5, mas");
        text += tr("não é aplicável à cadeia de custódia de apenas 1 mídia, pois não haverá outra imagem no mesmo processo para que haja colisão. ");
        text += tr("Porém existe a probabilidade dessa ocorrência quando da geração de imagem forense de mais midias, por exemplo, em callcenters,");
        text += tr(" onde imagens de sistema podem estar sendo replicadas em mídias virgens, e daí por diante.</p>");
        text += tr("<p align=justify>Ao término da geração do hash, uma janela se abrirá exibindo o resultado, que poderá ser capturado via screenshot.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Gerar Hash em Arquivo</h3></b></p>");
        text += tr("<p align=justify>Após selecionada a opção de gerar hash, a caixa de checagem para geração de arquivo contendo o hash é liberada. ");
        text += tr("Marcando-a, o hash será gravado na mídia de destino junto à imagem em um arquivo com o mesmo nome, diferindo apenas na extensão, ");
        text += tr("que passa a ser o nome do respectivo tipo hash selecionado.</p>");
        text += tr("<p align=justify>A geração do hash na mídia de destino não maculará a evidência, tratando-se de um arquivo de suporte, de forma que ainda que o ");
        text += tr("perito se esqueça de anotar o hash gerado, haverá como recuperá-lo de forma válida judicialmente, validando-se através dos metadados");
        text += tr(" do arquivo contendo o hash (considerando seu timestamp). Ou seja, sua coleta não será perdida.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Gerar Base de Strings</h3></b></p>");
        text += tr("<p align=justify>A base de strings (explicada no capítulo 3.0/Base de Strings) é um processo de execução sequencial, ou seja, só é ");
        text += tr("iniciado após o termino da geração de imagem forense. Se tiver um curto espaço de tempo para sua coleta, você pode adiar esta tarefa ");
        text += tr("executando-a posteriormente sobre a imagem pré-existente, a partir da aba 'Ler da Imagem', selecionando-a no botão superior direito, ");
        text += tr("seguidamente marcando a caixa de checagem de base de strings e então clicando em 'Coletar'. Será aberta uma janela para a escolha do ");
        text += tr("destino, que poderá seguramente ser guardada na mídia contendo a imagem forense desde que haja espaço para tal.</p>");
        text += tr("<p align=justify>Se for uma coleta para apresentação judicial da imagem, recomenda-se que seja gerada apenas a imagem forense na midia ");
        text += tr("de destino e no máximo o arquivo de hash, pois a geração da base de strings já se trata de um produto da manipulação da evidência.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Gerar Log</h3></b></p>");
        text += tr("<p align=justify>Ao optar pela geração de log, todos os eventos possíveis são registrados em arquivo, o qual poderá ser visualizado, ");
        text += tr("copiado ou removido posteriormente através da aba 'Log' do menu vertical à esquerda.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Barra de Progresso</h3></b></p>");
        text += tr("<p align=justify>Logo abaixo do botão 'Coletar' há uma área reservada para a barra de progresso. Essa barra de progresso só é exibida ");
        text += tr("durante uma operação na qual ela seja utilizada, como geração de base de strings ou geração de imagem forense. Toda a vez que uma operação ");
        text += tr("terminar, a recarga de qualquer aba ocultará a barra de progresso, assim como tornará os demais componentes das janelas aos seus valores ");
        text += tr("padrão, com excessão da aba <b>Monitor</b>.");
    }
    else if (chapter == "3.2"){
        text  = tr("<p align=justify>A aba <b>Monitor</b> contém informações suficientes para mantê-lo informado da operação em execução. Todos os dados exibidos");
        text += tr("nessa aba mantem-se permanentes até o inicio de uma nova operação, enquanto as demais abas são limpas ao final de cada processo, seja ele qual for.");
        text += tr("Essa aba separa as informações em grupos lógicos específicos, descritos a seguir.");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Operações e Tarefas</h3></b></p>");
        text += tr("<p align=justify>Ao topo da aba, note as descrições de início e fim de tarefa. Quando uma tarefa é iniciada, imediatamente sua data/hora ");
        text += tr("é exibida, enquanto <b>Final de Operação</b> é um campo que aguarda pelo retorno de final de tarefa, ou seja, você não precisará anotar ");
        text += tr("início e fim de operação para controle de tempo, apenas colete as informações a partir da interface do programa. Tais informações também ");
        text += tr("se encontrarão disponíveis no arquivo e log, mas visualmente para apresentação documental seria mais adequado um screenshot. Esse recurso ");
        text += tr("será descrito em detalhes no respectivo capítulo.</p>");
        text += tr("<p align=justify>À direita, logo abaixo de <b>Tarefas em Execução</b> é exibido cada um dos processos correntes. A tarefa <b>Monitor</b> ");
        text += tr("sempre deverá estar em exibição, caso contrário as informações desta janela não poderão ser mais consideradas válidas, pois essa tarefa ");
        text += tr("se encarrega do controle de diversos estados de operação.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Operação com mídias</h3></b></p>");
        text += tr("<p align=justify>Esse grupo contém informações de nome de dispositivos, espaços de utilização na respectiva mídia e número serial (quando disponível).");
        text += tr("Não há proporcionalidade entre as mídias de origem e destino para a medição disposta abaixo da identificação do dispositivo, portanto não ");
        text += tr("baseie-se nesses indicadores para mensurar ocupação de área da mídia de origem no destino. Para tal, seus respectivos tamanhos são exibidos ");
        text += tr("na aba <b>Gerar Imagem Forense</b> e estes dados estarão disponíveis até o final da operação ou se você já estiver nessa aba, até que seja ");
        text += tr("recarregada.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Estado do Sistema</h3></b></p>");
        text += tr("<p align=justify>Esse grupo contém informações pertinentes ao estado do sistema operacional, relantando utilização de memória para determinada ");
        text += tr("operação, além de exibir o número de núcleos e a utilização deles. Essas informações são importantes para saber o limite de utilização ");
        text += tr("dos recursos disponíveis, compreender gargalos na operação ou anomalias, causadas por exemplo, por mídias defeituosas.</p>");
        text += tr("<p align=justify>O gráfico disposto abaixo dos grupos anteriormente descritos, exibe informações de tempo-real de algumas operações.");
        text += tr(" O tipo de exibição variará conforme o tipo de cópia. Quando copiando de memória flash, é provável um padrão similar ao ");
        text += tr("de uma leitura digital, parecido com uma onda quadrada.</p>");
        text += tr("<p align=justify>Uma das ferramentas de cópia aloca os dados na memória em um tamanho fixo, depois descarrega para a mídia de destino. ");
        text += tr("Casos como wipe podem (e normalmente devem) manter uma linha estável na velocidade máxima do barramento, exceto alguma interferência no ");
        text += tr("barramento de dados desestabilize o processamento do programa.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Gráfico</h3></b></p>");
        text += tr("<p align=justify>O gráfico mais comum, onde grandes variações são percebidas, pode eventualmente iniciar com um pico, tratando-se da primeira ");
        text += tr("alocação em memória, carga do buffer da mídia de destino ou wakeup da mídia de origem. ");
        text += tr("<p align=justify>A leitura de pico foi mantida ainda que percebida, tendo o objetivo de alguma representação do buffer, porém em alguns casos pode ");
        text += tr("ser prejudicial à exibição dos valores na matriz de dados do gráfico, por exceder a resolução do limite vertical.</p>");
        text += tr("<p align=justify>Isso significa que o tamanho da linha de exibição poderá se tornar tão fina que a resolução da tela não terá capacidade ");
        text += tr("de exibí-la, gerando falhas na impressão da linha, o que <b>não</b> representa danos nos dados, gargalo ou intermitência na operação.</p>");
        text += tr("<p align=justify>O gráfico é gerado a partir de leitura de dados repassados pelo núcleo do sistema operacional, sendo essa a forma mais efetiva");
        text += tr("obter dados reais e válidos. Tais valores são exibidos em MBps (Mega Bytes Por Segundo), com raras ocorrências de repetição da amostragem.</p>");
        text += tr("<p align=justify>O maior objetivo do gráfico é fornecer alguma percepção do andamento do processo, tanto por anomalias quanto intermitências");
        text += tr(" em momentos que as próprias ferramentas não trazem algum feedback. Um bom exemplo para essa condição está na formatação com EXT3, onde logo");
        text += tr(" ao término das mensagens de formatação, alguma parte do sistema interage com a mídia formatada (seja kernel, seja firmware da mídia).");
        text += tr("Observando o gráfico pode-se notar comunicação de background com o disco e somente quando finalizada essa atividade a formatação devolve a");
        text += tr("mensagem de conclusão. É reconfortante que durante esse tempo de espera haja ainda algum feedback do andamento do processo e esse é um dos ");
        text += tr("objetivos.</p>");
    }
    if (chapter == "4.0"){
        text  = tr("<p style='color:rgb(255, 153, 51);'><b><h3>Screenshot do Programa</h3></b></p>");
        text += tr("<p align=justify>O recurso de screenshot do programa permite que automaticamente sejam coletadas telas da operação sempre que clicado em determinados componentes da janela.");
        text += tr(" Esse recurso pode ser habilitado a qualquer momento, porém é recomendado que seja feito logo ao início da utilização do programa.<br>");
        text += tr("A captura de tela, quando feita pelo programa DWatson rodando sobre Wayland (DWOS), executará a captura apenas da janela do aplicativo.<br>");
        text += tr("Quando executado na versão Desktop (ainda não disponível), captura toda a interface, decoração, janelas e menus.<br>");
        text += tr("Há também a opções de captura manual através do programa de coleta, utilizando a combinação <b>Ctrl+S</b></p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Screenshot do Sistema</h3></b></p>");
        text += tr("<p align=justify>Para utilizar o recurso de captura de tela do sistema, utilize a combinação de teclas <b>Super+S</b>. A tecla <b>Super</b> ");
        text += tr("é a tecla que normalmente contém uma bandeira ou janela, representando uma tecla de sistema operacional.");
        text += tr("<p align=justify>A captura deverá ser feita manualmente e sempre que executada, ");
        text += tr("estará disponível na raiz do sistema, acessível através do terminal. Seguidamente a cada captura do sistema, será necessário renomear o ");
        text += tr("arquivo gerado ou se preferir, marque a opção <b>Auto renomear</b> na aba <b>Log</b> do menu esquerdo. Optando por auto renomear no grupo <b>Wayland</b>, ");
        text += tr(" o arquivo será renomeado com data e hora e movido para /var/log/screenshots. Ao final dos trabalhos você pode recolher todo o material ");
        text += tr(" gerado clicando em <b>Copiar Operação</p>, no mesmo menu.");
    }
    if (chapter == "5.0"){
        text  = tr("<p align=justify>A gravação de tela não é um recurso do programa, mas do sistema operacional.</p>");
        text += tr("<p align=justify>Para iniciar e parar a gravação, utilize a combinação de teclas <b>Super+R</b>. Não é recomendado (e provavelmente nem é ");
        text += tr("possível) gravar toda uma operação, devido ao tamanho final do arquivo, que será gravado totalmente em memória RAM, para posteriormente ser ");
        text += tr("movido manualmente para uma mídia de armazenamento externa.</p>");
        text += tr("<p align=justify>Cada vez que iniciada a gravação, o conteúdo é adicionado ao final da gravação anterior, de forma a concatenar esses dados.");
        text += tr(" Esse recurso é conhecido por <b>Screencast</b>, que captura o vídeo em uma sequência de quadros, necessitando posteriormente conversão. O arquivo ");
        text += tr(" gerado estará disponível na raiz do sistema com o nome <b>capture.wcap</b>");
        text += tr("<p align=justify>Para fazer a conversão, clique em <b>Converter</b> na aba <b>Log</b> do menu esquerdo, no grupo <b>Wayland</b>. Você ");
        text += tr("Pode opcionalmente gravar pequenos trechos de operação e então converter cada parte invés de gerar um arquivo único, o que pode ser útil para ");
        text += tr("distiguir determinado momento do processo de coleta.</p>");
    }
    if (chapter == "6.0"){
        text  = tr("<p style='color:rgb(255, 153, 51);'><b><h3>Como identificar dispositivos e partições?</h3></b></p>");
        text += tr("<p align=justify>Os dispositivos são identificados de uma forma bastante clara, normalmente um padrão de 3 letras ou 3 letras seguidas por ");
        text += tr("números. Por exemplo: <br>");
        text += tr("sda1<br>");
        text += tr("<b>sd</b> representa um disco SATA, IDE ou SCSI.");
        text += tr("<b>a</b> representa a ordem do disco, alfabeticamente. Portanto, <b>a</b> representa a mídia (HD/SSD) no primeiro barramento SATA.<br>");
        text += tr("<b>1</b> representa a primeira partição da mídia <b>sda</b>.</p>");
        text += tr("<p align=justify>Quando selecionando a origem, apenas os dispositivos serão exibidos. Partições são exibidas apenas na seleção de destino, ");
        text += tr("porque é o procedimento padrão gravar uma imagem em arquivo.<br>");
        text += tr("Quando pretende-se gravar origem a destino, isto é, clonagem da mídia, então seleciona-se apenas o dispositivo (<b>sdX</b>) na caixa de ");
        text += tr("destino. Se a intenção é gravar a imagem em arquivo e a caixa de destino exibe apenas dispositivo, o mais provável é que a mídia seja virgem ");
        text += tr(" ou passou por wipe. Nesse caso, escolha formatar a mídia na caixa ao lado do dispositivo de destino, selecionando o sistema de arquivos ");
        text += tr("pretendido.<br>");
        text += tr("Recomenda-se que antes de fazer uma cópia dispositivo-a-dispositivo aplique-se o processo de wipe afim de excluir quaisquer dados que ");
        text += tr("porventura possam existir.<br>");
        text += tr("Dispositivos podem ser clonados diretamente ao destino desde que a origem seja menor ou igual ao tamanho de destino.<br>");
        text += tr("A superfície do sistema de arquivos (seus inodes) podem ser verificados através da aba <b>Monitor</b>. clicando em <b>Exibir Dados</b></p>");
    }
    if (chapter == "7.0"){
        text  = tr("<p align=justify>Você pode obter suporte e exclarecer suas dúvidas através do nosso blog em <b style='color:blue'>http://dwos.com.br</b><br>");
        text += tr("Também é possível contratar suporte especializado e personalização visual ou funcional do sistema através de nosso website.<br></p>");
    }
    if (chapter == "8.0"){
        text  = tr("<p style='color:rgb(255, 153, 51);'><b><h3>Sistema Operacional</h3></b></p>");
        text += tr("<p align=justify>O sistema operacional é baseado no Kernel Linux, disponível em <b style='color:blue'>http://kernel.org</b><br> sob a licença GPL.");
        text += tr("Nenhum código foi adicionado ou removido do kernel original, tendo sido compilado nativamente sobre uma plataforma X86_64, utilizando ");
        text += tr("o sistema Linux Mint como sistema nativo.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Raiz do sistema</h3></b></p>");
        text += tr("<p align=justify>A raíz do sistema foi compilada a partir de binários nativos do sistema operacional nativo, através da cópia dos binários ");
        text += tr(" e bibliotecas necessários. O sistema nativo está disponível para download através do website <b style='color:blue'>http://www.linuxmint.com/download.php</b>.</p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Servidor Gráfico</h3></b></p>");
        text += tr("<p align=justify>A renderização gráfica é servida pela utilização do Wayland, compilado totalmente a partir dos fontes, conforme descrição ");
        text += tr(" do link <b style='color:blue'>http://wayland.freedesktop.org</b> (não houve modificação no código fonte do Wayland ou de suas dependências).");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Os aplicativos da suite Djames Watson</h3></b></p>");
        text += tr("<p align=justify>Os módulos da suite forense Djames Watson estão disponíveis apenas como software de código fechado, através da aquisição do sistema integrado ");
        text += tr("exceto o módulo de Clonagem (ainda não disponível), cujo código manter-se-á público sob a licença GPL, cujo módulo será mantido pela comunidade, ");
        text += tr("não havendo qualquer garantia sobre seu funcionamento, modo de operação e/ou recursos.<br>");
        text += tr("O propósito do módulo de clonagem é a substituição do sistema de clonagem Phantom, palestrado na 8ª edição do Fórum Internacional do ");
        text += tr(" Software Livre, FLISOL 2011 na Universidade Federal de Ciências e Tecnologia em Salto, e com artigo publicado na IBM Developer Works, ");
        text += tr("no link <b style='color:blue'>https://www.ibm.com/developerworks/community/blogs/752a690f-8e93-4948-b7a3-c060117e8665/entry/clonagem_de_hd?lang=en</b></p>");
        text += tr("<p style='color:rgb(255, 153, 51);'><b><h3>Propriedade de Hardware, Hardware de Terceiros e Open Hardware</h3></b></p>");
        text += tr("Os hardwares proprietários estão todos sob patente requerida, enquanto hardwares OEM são equipamentos compostos com especialização de software ");
        text += tr("para exercer determinada função, onde a propriedade intelectual limita-se ao software especialista embarcado.");
    }

    emit signalSelectedSection(text);
}
