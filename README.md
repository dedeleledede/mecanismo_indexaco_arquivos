### Como compilar

Requisitos:

* Compilador C++ com suporte a C++17 (ex.: `g++` no Ubuntu 20.04+).
* `make` instalado.
* Nenhuma biblioteca externa além da STL.

No diretório raiz do projeto (`mecanismo_indexacao_arquivos/`), rode:

```bash 
make
```

Isso gera o executável:

```bash
./indice
```

Para limpar os `.o` e o binário:

```bash
make clean
```

---

### Como executar

O programa tem dois modos de operação, ambos chamados a partir do binário `indice`.

#### 1) Modo indexação

```bash
./indice construir <caminho_do_diretorio>
```

Exemplo:

```bash
./indice construir data/machado
```

O que acontece aqui:

* O programa varre recursivamente o diretório informado (`data/machado`, no exemplo).
* Para cada arquivo de texto, lê o conteúdo, normaliza e tokeniza.
* Remove stopwords usando o arquivo `data/stopwords.txt`.
* Constrói o índice invertido em memória.
* Salva tudo (índice + mapeamento de documentos) em **binário** no arquivo:

```txt
data/index.dat
```

Se algo der errado (diretório inválido, erro de leitura, falha ao salvar), uma mensagem de erro é exibida no terminal e o programa encerra com código diferente de zero.

#### 2) Modo busca

```bash
./indice buscar <termo1> <termo2> ... <termoN>
```

Exemplos:

```bash
./indice buscar rede
./indice buscar vida morte
```

O que acontece aqui:

* O programa verifica se o arquivo `data/index.dat` existe.

  * Se não existir, avisa para rodar a indexação primeiro.
* Carrega o índice invertido e o mapeamento de documentos a partir de `data/index.dat`.
* Normaliza os termos de busca com o `TextProcessor` (minúsculas, sem pontuação, sem stopwords).
* Usa o `QueryProcessor` para buscar:

  * Se for **uma palavra**, retorna todos os documentos onde o termo aparece.
  * Se forem **múltiplas palavras**, faz uma busca do tipo **AND** (retorna só arquivos que contêm **todas** as palavras).
* Imprime na tela os **caminhos completos** dos arquivos encontrados.

Saída típica:

```bash
./indice buscar rede
Entrando em modo busca com argumentos: rede 
Documentos encontrados:
/caminho/completo/.../data/machado/conto/contosFluminenses.txt
...
```

Se nenhum documento for encontrado, o programa informa isso em vez de imprimir uma lista vazia.

---

## Estrutura de diretórios

A organização esperada do projeto é algo nessa linha:

```txt
mecanismo_indexacao_arquivos/
  src/
    main.cpp
    cli.cpp
    index.cpp
    indexer.cpp
    text_processor.cpp
    serializer.cpp
    query_processor.cpp
  include/
    cli.h
    index.h
    indexer.h
    text_processor.h
    serializer.h
    query_processor.h
  data/
    stopwords.txt
    index.dat        (gerado em tempo de execução)
    machado/         (coleção de textos para teste)
  Makefile
  README.md
```

---

## Fluxo de construção do projeto (histórico mental / “commits”)

Aqui vai o fluxo que fui seguindo, atualizado até o estado atual.

---

### Push 1 – Estrutura do projeto

* Criar a “skeleton” do projeto:

  * pasta `src/` para os `.cpp`;
  * pasta `include/` para os `.h`;
  * pasta `data/` para `stopwords.txt`, `index.dat` e o diretório `machado/`;
  * `Makefile` básico para compilar tudo em um binário chamado `indice`.

* Ideia central: o programa será uma CLI que oferece **dois modos**:

  ```txt
  indice construir <caminho_do_diretorio>
  indice buscar <termo1> <termo2> ... <termoN>
  ```

---

### Push 2 – main / CLI mínimo viável

* Definir o contrato da linha de comando:

  * nenhum argumento → mensagem de erro de uso;
  * primeiro argumento é o **modo** (`construir` ou `buscar`);
  * validar quantidade mínima de parâmetros para cada modo.

* Nesta fase, o main/CLI só fazia:

  * identificar o modo;
  * imprimir mensagens do tipo:

    * “entrando em modo de indexacao com diretorio: X”
    * “entrando em modo de busca com termos: ...”

* Sem “trabalho real” ainda, só a casca de orquestração.

---

### Push 3 – Index (headers e implementação)

* Projeto do **Index** como “banco de dados em memória”:

  * índice invertido:

    ```cpp
    std::unordered_map<std::string, std::set<int>> // palavra -> conjunto de docIds
    ```
  * mapeamento de documentos:

    * `std::unordered_map<std::string, int> path_to_id;`  // caminho → id
    * `std::unordered_map<int, std::string> id_to_path;`  // id → caminho

* Métodos principais:

  * `add_word(palavra, filepath)`:

    * garante um docId para o caminho (criando se não existir),
    * registra o docId no conjunto da palavra.

  * `get_docs_from_word(palavra)`:

    * devolve (por referência constante) o `std::set<int>` com os ids dos documentos.

  * `get_filepath(doc_id)`:

    * devolve a string com o caminho completo do arquivo (ou string vazia se não existir).

  * getters/setters “crus” para o `Serializer`:

    * `get_inverted_index()`
    * `get_filepath_to_id()`
    * `get_id_to_filepath()`
    * `set_inverted_index(...)`
    * `set_doc_maps(...)`

---

### Push 4 – TextProcessor (normalização de texto)

* Responsabilidade do `TextProcessor`:

  * receber uma `std::string` com texto bruto;
  * converter tudo para minúsculas;
  * remover pontuações (manter só `isalnum`);
  * trocar tudo que não for letra/dígito por espaço;
  * quebrar em palavras (tokens) com base em espaços;
  * remover:

    * strings vazias,
    * stopwords carregadas de `data/stopwords.txt`.

* Interface típica:

  * `void load_stopwords(const std::string& filepath);`
  * `std::string normalize(const std::string& text) const;`
  * `std::vector<std::string> split_words(const std::string& normalized) const;`
  * `std::vector<std::string> process(const std::string& text) const;`

    * que basicamente faz `normalize -> split_words -> filtra stopwords`.

---

### Push 5 – Indexer (varrer diretório e popular o índice)

* O `Indexer` é o “orquestrador” da indexação:

  * guarda **referências** para:

    * um `Index` (onde os dados serão guardados),
    * um `TextProcessor` (para limpar o conteúdo).

* Métodos principais:

  * `build(const std::string& root_dir)`:

    * usa `std::filesystem::recursive_directory_iterator` em cima de `fs::absolute(root_dir)`;
    * para cada `regular_file`, chama `process_file`.

  * `process_file(const std::string& filepath)`:

    * abre o arquivo (`std::ifstream`);
    * lê o conteúdo inteiro para uma string;
    * chama `text_processor.process(conteudo)` para obter as palavras;
    * para cada palavra, chama `index.add_word(palavra, filepath)`.

* A partir daqui, rodar `./indice construir data/machado` já preenche um índice em memória (sem ainda salvar no disco).

---

### Push 6 – Integrar modo construir (CLI + Indexer + TextProcessor + Index)

* No modo `construir`, o main/CLI passou a:

  * validar `argc` (precisa de `<dir>`);

  * criar:

    * `Index index;`
    * `TextProcessor text_processor;`
    * carregar `text_processor.load_stopwords("data/stopwords.txt");`
    * `Indexer indexer(index, text_processor);`

  * chamar:

    * `indexer.build(dir);`

  * em seguida, chamar o `Serializer::save(index, "data/index.dat")` para persistir o índice em disco (binário).

* Se qualquer etapa falha (diretório inválido, erro de leitura, erro de gravação), a CLI mostra uma mensagem clara e encerra.

---

### Push 7 – Serializer (binário) + reuso do índice

* Implementação do `Serializer` para salvar/carregar o `Index` em **formato binário**:

  * salvar, em ordem:

    * tamanho do mapa invertido,
    * para cada palavra:

      * comprimento da string,
      * bytes da palavra,
      * quantidade de docIds,
      * cada docId (int) em binário;
    * tamanho do mapa `filepath → id`,
    * pares (string do caminho completo, int id),
    * tamanho do mapa `id → filepath`,
    * pares (int id, string caminho).

  * carregar na ordem inversa, reconstruindo:

    * `inverted_index_map`,
    * `path_to_id`,
    * `id_to_path`.

* Detalhes importantes:

  * formato estritamente binário (sem `<<` ou `>>` de texto);
  * tamanho de strings guardado como inteiro fixo (`std::uint32_t`), seguido de `write`/`read` dos bytes;
  * o índice guarda **caminhos completos** dos arquivos, de forma que você pode:

    * indexar em um diretório,
    * depois rodar o programa a partir de outro diretório,
    * e mesmo assim a busca continua apontando para o caminho absoluto.

---

### Push 8 – QueryProcessor (busca AND com interseção manual)

* Criação do `QueryProcessor`, recebendo `const Index&` no construtor.

* Método principal:

  ```cpp
  Index::doc_set_id process_terms(const std::vector<std::string>& terms) const;
  ```

  * recebe um vetor de termos **já normalizados** (sem stopwords);
  * para cada termo:

    * pega o conjunto de docIds com `index.get_docs_from_word(term)`;
    * mantém um conjunto “resultado parcial”;
    * faz a **interseção** entre o conjunto acumulado e o conjunto da vez.

* A interseção é feita “na mão”, com um laço que percorre dois `std::set<int>` como em um merge de vetores ordenados, **sem** usar `std::set_intersection`, atendendo à exigência do PDF.

* Resultado final: conjunto de docIds de arquivos que contêm **todas** as palavras consultadas (AND).

---

### Push 9 – Modo buscar completo (CLI + TextProcessor + Serializer + QueryProcessor)

* No modo `buscar`, a CLI passou a:

  1. Validar se há pelo menos um termo após `buscar`.
  2. Construir a string da query a partir de `argv[2..]`.
  3. Criar `Index index;`.
  4. Criar `TextProcessor text_processor;` e carregar stopwords.
  5. Verificar se `data/index.dat` existe e é carregável via `Serializer::load`.

     * Se não, avisa: “rode a indexacao primeiro” / erro de carregamento.
  6. Normalizar os termos com o `TextProcessor` e obter o vetor de tokens.
  7. Criar `QueryProcessor query_processor(index);`.
  8. Chamar `process_terms(terms)` para obter o conjunto de docIds.
  9. Se o conjunto estiver vazio → informar que nenhum documento foi encontrado.
  10. Caso contrário, iterar nos docIds e imprimir `index.get_filepath(id)` — que hoje são caminhos completos (como visto na sua saída com "rede").

---
