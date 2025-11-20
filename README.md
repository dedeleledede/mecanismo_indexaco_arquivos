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

commit 4 (text_processor)