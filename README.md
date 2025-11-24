# Construção de Um Mecanismo de Indexação de Arquivos

## O programa tem dois modos:

``indice construir <caminho_do_diretorio>``
``indice buscar <termo1> <termo2> ... <termoN>``

## Blocos principais de responsabilidade são:

- Index - guarda o indice invertido + mapeamento ID <> arquivo
- Text Processor - limpa texto, tokeniza, tira pontuação/stopwords
- Indexer - percorre diretorio, le arquivos, alimenta o Index 
- Serializer - salva/carrega o Index no arquivo binário index.dat
- Query Processor - recebe termos de busca e devolve arquivos
- Command Line Interface - interpreta argc/argv e chama o resto

processo de pensamento:
como representar o indice invertido em c++ usando stl
tipo da chave tipo do valor 
como mapearia nome_arquivo > id e id > nome_arquivo

construir ambiente basico e makefile compilar "hello from index world"

mecanismo_indexacao_arquivos/
  src/
    main.cpp
    index.cpp
    indexer.cpp
    text_processor.cpp
    serializer.cpp
    query_processor.cpp
    cli.cpp
  include/
    index.h
    indexer.h
    text_processor.h
    serializer.h
    query_processor.h
    cli.h
  machado/
  Makefile
  stopwords.txt

  - commit 1 ("estrutura do projeto")

  
Nesse projeto, o main eh basicamente um roteador de comandos:
le os argumentos da linha de comando
decide qual modo o usuario quer

modo indexacao: indice construir <caminho_do_diretorio>
modo busca: indice buscar <termo1> <termo2> ...

verifica se o comando esta correto (ler e validar parametros)
chama as partes do sistema que fazem o trabalho
tratar erros basicos (comandos errados, falta de arquivo index.dat)

primeiro passo: mapear todos os cenarios que o main precisa tratar
---
usuario roda sem nenhum argumento
err: "erro: argumentos invalidos (modelo: indice modo(indexacao/busca) <dir>/<termo>)"
---
usuario roda com modo sem parametros suficientes - indexacao
err: "erro: argumentos insuficientes (<dir>/<termo>)"
---
usuario roda indexacao corretamente
ex: ./indice construir ./docs
---
validar que o comando tem os argumentos certos
criar os objetos responsaveis por indexar
mandar construir o indice e salvar em index.dat

sucesso ou erro
---
usuario roda busca sem termos
ex: ./indice buscar
err: "erro: argumentos insuficientes (<termo>)"
---
usuario roda busca com termos
ex: ./indice buscar redes computadores
---
verificar se existe index.dat no diretório atual
se nao existir, avisar: “rode a indexacao primeiro”
se existir, carregar o indice
mandar buscar os termos e exibir os arquivos encontrados
---
usuario digita um modo invalido
ex: ./indice destruir ./docs
err: "erro: argumentos invalidos (modelo: indice modo(indexacao/busca) <dir>/<termo>)"
---

segundo passo: transformar isso num fluxograma mental do main

O programa começa >

- tem argumentos suficientes?

 - nao > mostra mensagem de uso > encerra
 - sim > continua

le o modo >

- se o modo for construir

 - verifica se foi passado dir
  - se nao > mostra mensagem de erro especifica > encerra
  - se foi > continua

 - varre o diretorio
 - constroi o indice
 - salva em index.dat
 - sucesso ou erro

- se o modo for buscar

 - verifica se tem ao menos um termo apos a palavra buscar
  - se nao tiver > mostra erro > encerra

 - verifica se o arquivo index.dat existe
  - se nao existir > diz “indice nao encontrado, rode a indexacao primeiro” > encerra
  - se existir > carrega indice, processa a busca, mostra os arquivos encontrados

 - se o modo nao for nenhum dos dois 
  - modo invalido > erro > encerra

terceiro passo: planejar a versao minima viavel do main

ter um main que apenas reconhece o modo e imprime mensagens de teste.
modo construir “entrando no modo de indexacao com diretorio: X”
modo buscar “entrando no modo de busca com termos: ...”

 - commit 2 (main minimo viavel)

 agora para construir os headers

 - index
1. escolher a estrutura de dados
2. definir a interface publica

---

index eh como um banco de dados em memoria
1. indice invertido
2. mapeamento de documentos

estrutura de dados:
  hashmap para caminho > id
  hashmap para id > caminho

mapeamento:
  1. saber a partir de filepath, o id e vice-versa
  
---
classe index
 - mapa de indice inverso
 - mapa de id > path
 - mapa de path > id

doc_set_id

add_word()
  
getters
  get_filepath(int doc_id);
    
  get_docs_from_word();
    
  get_inverted_index();  
    
  get_filepath_to_id();
    
  get_id_to_filepath();

setters
set_inverted_index()
set_doc_maps()
---

commit 3 (index headers e implementacao)

TextProcessor (pre-processamento)

- receber uma string com texto
- converter para minusculas
- remover pontuação
- quebrar em palavras
- eliminar stopwords

build header
implement

TextProcessor: pre processamento de texto

- ter um componente que recebe texto bruto e devolve um vetor de palavras normalizadas sem stopwords

responsabilidades:

- carregar lista de stopwords de um arquivo
- normalizar texto:

  - converter para minusculas
  - remover pontuacao e simbolos, trocando por espaco
- quebrar em palavras (tokenizar)
- remover stopwords

estrutura interna:

- `unordered_set<string> stopwords;`

  - armazena todas as stopwords para consulta O(1)

interface (cabecalho):

- parte privada:

  - `string normalize(const string& text) const;`

    - percorre o texto caractere a caractere
    - converte para minusculas
    - mantem apenas caracteres alfanumericos
    - troca o resto por espaco
  - `vector<string> split_words(const string& normalized) const;`

    - percorre a string normalizada
    - monta palavras acumulando caracteres nao espaco
    - quando encontra espaco, fecha a palavra corrente
  - `bool is_stopword(const string& word) const;`

    - verifica se `word` esta em `stopwords`

- parte publica:

  - `TextProcessor() = default;`
  - `void load_stopwords(const string& filepath);`

    - abre o arquivo
    - le palavra por palavra
    - insere cada uma em `stopwords`
  - `vector<string> process(const string& text) const;`

    - `normalize(text)` -> `split_words(...)` -> filtrar:

      - so mantem palavras que nao sao stopwords

commit 4 (text_processor)

Indexer: construcao do indice a partir de arquivos

- ter um componente que:

  - percorre um diretorio
  - le arquivos de texto
  - passa o conteudo pelo `TextProcessor`
  - alimenta o `Index` com as palavras encontradas

dependencias:

- referencia para `Index`
- referencia para `TextProcessor`
- `std::filesystem` para varrer diretorios
- `ifstream` para ler arquivos

interface:

- parte privada:

  - `Index& index;`
  - `TextProcessor& text_processor;`
  - `void process_file(const string& filepath);`

    - abre o arquivo
    - le o conteudo completo para uma string
    - chama `text_processor.process(conteudo)`

      - recebe `vector<string>` de palavras normalizadas
    - para cada palavra:

      - `index.add_word(word, filepath);`

- parte publica:

  - construtor:

    - `Indexer(Index& idx, TextProcessor& tp);`

      - guarda referencias para `index` e `text_processor`
  - metodo principal:

    - `void build(const string& root_dir);`

      - usa `recursive_directory_iterator` para varrer `root_dir`
      - para cada arquivo regular:

        - obtem o caminho como string
        - chama `process_file(path);`

commit 5 (indexer)

main: integracao de Index, TextProcessor e Indexer no modo construir

estado do projeto ate aqui:

- `Index` guarda o indice em memoria
- `TextProcessor` gera tokens normalizados e sem stopwords
- `Indexer` construi o indice a partir de um diretorio

papel do main apos esses commits:

- interpretar `argc` / `argv`
- decidir modo:

  - `construir`
  - `buscar`
- validar argumentos
- instanciar e conectar os componentes certos
- reportar erros basicos ao usuario

fluxo no case construir

1. validar argumentos:

   - se argc < 3

     - imprimir mensagem de erro: argumentos insuficientes
     - encerrar com codigo de erro
2. obter diretorio:
3. criar componentes
4. construir indice

- ainda em versao preliminar
- apenas:

  - valida argumentos
  - monta string de consulta a partir de `argv[2..]`
  - usa `TextProcessor` para normalizar e remover stopwords
  - imprime os termos resultantes para debug

commit 6 (main: modo construir, fluxo geral)

Serializer: persistencia do indice

objetivo:

- permitir que o indice seja salvo em disco e recarregado depois
- arquivo alvo: `index.dat` (formato texto simples)

interface:

- `static bool save(const Index& index, const string& filename);`

  - abre arquivo para escrita
  - grava:

    - quantidade de documentos
    - linhas `id filepath`
    - quantidade de termos
    - linhas `palavra qtdIds id1 id2 ...`
- `static bool load(Index& index, const string& filename);`

  - abre arquivo para leitura
  - le:

    - numero de documentos
    - monta `filepath_to_id` e `id_to_filepath`
    - numero de termos
    - monta `inverted_index_map`
  - ao final:

    - chama `index.set_doc_maps(...)`
    - chama `index.set_inverted_index(...)`

formato de arquivo adotado:

- primeira parte: documentos

  - linha 1: `N` (numero de documentos)
  - proximas N linhas:

    - `docId filepath`
- segunda parte: termos

  - linha seguinte: `M` (numero de palavras distintas)
  - proximas M linhas:

    - `palavra qtdIds id1 id2 id3 ...`

limitacao assumida:

- caminhos de arquivo nao contem espacos
---

Makefile

estrutura de pastas alvo:

- `src/`

  - `main.cpp`
  - `index.cpp`
  - `text_processor.cpp`
  - `indexer.cpp`
  - `serializer.cpp`

- `include/`

  - `index.h`
  - `text_processor.h`
  - `indexer.h`
  - `serializer.h`

- `build/`

  - arquivos objeto `.o`
  - binario final
- `stopwords.txt` e diretorio com textos (ex: `machado/` ou `data/`)

principais regras do Makefile:

- `all`: regra padrao, chama a geracao do binario
- regra para `build/indice`:

  - linka todos os `.o`
- regra generica:

  - `build/%.o` depende de `src/%.cpp`
  - compila com `-std=c++17 -Iinclude`
- `clean`:

  - remove `build/` ou os `.o` e o binario

commit 7 serializer heaeder, makefile

commit 8 serializer

query processor h

qp cpp