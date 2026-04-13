# AGENTS.md

## Objetivo do projeto

Este repositório implementa o **Projeto 1 de Estrutura de Dados II**: um SIG simplificado da cidade de **Bitnópolis**, com leitura de arquivos `.geo`, `.pm` e opcionalmente `.qry`, produzindo saídas gráficas em SVG e relatórios textuais.

O foco principal **não é apenas “fazer funcionar”**. O foco do projeto é:

1. **implementar corretamente a estrutura de dados exigida**;
2. **usar essa estrutura de forma consistente no sistema**;
3. **manter modularização boa e interfaces limpas**;
4. **ter testes unitários reais por módulo**.

A estrutura de dados central exigida neste projeto é **Hashfile Dinâmico em disco**.

---

## Regras mandatórias

Estas regras têm prioridade sobre preferências de implementação.

### 1. Linguagem e compilação

- Usar **C99**.
- Compilar com **`-std=c99`**.
- Compilar com **`-fstack-protector-all`**.
- O build deve funcionar com **GCC** e **GNU Make** em Linux.
- O executável principal deve se chamar **`ted`**.

### 2. Estrutura de dados obrigatória

- Implementar **Hashfiles Dinâmicos em disco**.
- O arquivo do hashfile deve usar extensão **`.hf`**.
- Pode existir um arquivo separado de cabeçalho/controle, como **`.hfc`**, se necessário.
- Ao final da execução, deve ser gerado um arquivo **`.hfd`** com uma representação legível do conteúdo do hashfile.
- O arquivo `.hfd` também deve registrar **expansões de buckets**.
- Manter os dados de **quadras** e **habitantes** em hashfiles, preferencialmente em **2 hashfiles distintos**.

### 3. Modularização

- **Nunca definir `struct` concreta em `.h` público** quando isso puder ser encapsulado.
- Preferir **tipos opacos** nos headers.
- Cada módulo deve ter responsabilidade clara.
- Evitar funções longas, monolíticas ou com múltiplas responsabilidades.
- Separar parsing, regras de negócio, acesso aos hashfiles e geração de saída.

### 4. Testes

- Cada módulo deve possuir **teste unitário correspondente**.
- Usar **Unity Framework**.
- O fluxo esperado é:
  1. definir a interface no `.h`;
  2. escrever os testes;
  3. implementar até os testes passarem.
- Testar caminho feliz e caminho infeliz.
- Testar erros de arquivo, argumentos inválidos, buscas sem resultado, colisões, splits de bucket, reabertura de arquivo e persistência.

### 5. Makefile

O `makefile` deve conter, no mínimo:

- target para gerar o executável **`ted`**;
- targets para compilar os objetos por módulo;
- targets para compilar e executar os testes unitários de cada módulo;
- target agregador como **`tstall`** para rodar todos os testes.

### 6. Compatibilidade com correção

- Não depender de IDE.
- Não depender de interface gráfica.
- Não depender de bibliotecas externas desnecessárias.
- O projeto deve ser operável por linha de comando.
- O professor poderá apagar arquivos que não sejam fontes e build scripts do diretório esperado antes da compilação.
- Não introduzir dependência em banco de dados externo, daemon, serviço web ou formato proprietário.

---

## Escopo funcional do sistema

## Entradas

### Arquivo `.geo`

Define a cidade e as quadras.

Comandos mínimos esperados:

- `cq sw cfill cstrk`
- `q cep x y w h`

### Arquivo `.pm`

Define habitantes e moradores.

Comandos mínimos esperados:

- `p cpf nome sobrenome sexo nasc`
- `m cpf cep face num compl`

### Arquivo `.qry`

Define consultas e atualizações.

Comandos mínimos esperados:

- `rq cep`
- `pq cep`
- `censo`
- `h? cpf`
- `nasc cpf nome sobrenome sexo nasc`
- `rip cpf`
- `mud cpf cep face num cmpl`
- `dspj cpf`

---

## Interface de linha de comando

O programa deve aceitar:

```bash
ted [-e path] -f arq.geo [-q arq.qry] -o path [-pm arq.pm]
```
