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

# Análise Detalhada do Projeto 1 - Estrutura de Dados II

Vou decompor este projeto de forma estruturada para você entender todos os requisitos.

## 🎯 **O que é o projeto?**

Um **Sistema de Informação Geográfica (SIG) simplificado** que gerencia uma cidade fictícia chamada **Bitnópolis**. O sistema deve:
- Armazenar e manipular dados sobre a **geografia da cidade** (quadras/blocos)
- Gerenciar **pessoas e moradores** da cidade
- Processar **consultas e atualizações** sobre esse universo geográfico-populacional

---

## 🏙️ **Estrutura de Bitnópolis**

### **A Cidade**
- Composta por **16 quadras** (blocos/retângulos) organizados em uma grade
- Cada quadra é identificada por um **CEP alfanumérico** (cep1, cep2, ..., cep16)
- Cada quadra tem **4 faces/lados**: **N** (norte), **S** (sul), **L** (leste), **O** (oeste)
- A cidade tem um sistema de coordenadas com origem em **(0,0)** no canto superior esquerdo

### **Sistema de Endereçamento**
Um endereço em Bitnópolis tem 3 componentes:
- **CEP**: identifica a quadra (ex: cep15)
- **Face**: qual lado da quadra (N, S, L, O)
- **Número**: distância da frente da casa até o **ponto de ancoragem** (canto sudeste da quadra)

**Exemplo de endereço completo**: `cep15/S/45`

---

## 📋 **Dados da Cidade (arquivo .geo)**

O arquivo `.geo` descreve a cidade usando **comandos**:

| Comando | Parâmetros | O que faz |
|---------|-----------|----------|
| **q** | `cep x y w h` | Insere uma quadra (retângulo). `cep` = identificador; `x, y` = coordenadas do canto superior esquerdo; `w` = largura; `h` = altura |
| **cq** | `sw cfill cstrk` | Define cores e estilo para **todas as quadras seguintes**: `sw` = espessura da borda; `cfill` = cor de preenchimento; `cstrk` = cor da borda |

**Exemplo de arquivo .geo**:
```
cq 2 #FF9966 #8B4513
q cep1 0 0 100 100
q cep2 100 0 100 100
q cep3 200 0 100 100
...
```

---

## 👥 **Dados de Pessoas e Moradores (arquivo .pm)**

O arquivo `.pm` descreve os habitantes de Bitnópolis. Há **dois tipos**:
- **Habitante**: qualquer pessoa registrada na cidade (pode ter ou não endereço)
- **Morador**: um habitante que **mora em um endereço específico**
- **Sem-teto**: um habitante que **não é morador** (não tem endereço)

### **Comandos do arquivo .pm**

| Comando | Parâmetros | O que faz |
|---------|-----------|----------|
| **p** | `cpf nome sobrenome sexo nasc` | Insere um habitante. `cpf` = identificador; `sexo` = M ou F; `nasc` = data nascimento (dd/mm/aaaa) |
| **m** | `cpf cep face num compl` | Registra que um habitante mora em um endereço. `cpf` = habitante; resto = endereço |

**Exemplo**:
```
p 12345678900 João Silva M 15/03/1990
p 98765432100 Maria Santos F 22/07/1985
m 12345678900 cep5 N 42
m 98765432100 cep10 L 87
```

Aqui:
- João nasce/é registrado
- Maria nasce/é registrada
- João passa a morar em cep5, face norte, número 42
- Maria passa a morar em cep10, face leste, número 87

---

## 🔍 **Consultas e Atualizações (arquivo .qry)**

O arquivo `.qry` contém comandos de **atualização e consulta**. Produz saída em **texto (TXT)** e **visual (SVG/graphics - VG)**:

| Comando | Parâmetros | O que faz |
|---------|-----------|----------|
| **rq** | `cep` | **Remove uma quadra**. Moradores dessa quadra viram sem-tetos. **TXT**: informar CPF e nome dos moradores removidos. **VG**: marcar com X vermelho o ponto de ancoragem |
| **pq** | `cep` | **Conta moradores** por face e total. **TXT**: reportar números. **VG**: escrever números ao lado de cada face e no centro |
| **censo** | (sem parâmetros) | **Estatísticas gerais** da cidade. **TXT**: número total de habitantes, moradores, proporção, contagem por sexo (homens/mulheres em %), sem-tetos (total e %), etc. |
| **h?** | `cpf` | **Dados de uma pessoa**: todos os dados. Se morador, também informar endereço. **TXT** apenas |
| **nasc** | `cpf nome sobrenome sexo nasc` | **Pessoa nasce** (nova pessoa entra no sistema). **TXT**: confirmar inserção |
| **rip** | `cpf` | **Pessoa falece** (remove do sistema). **TXT**: reportar dados. Se morador, reportar endereço. **VG**: cruz vermelha no endereço (se houver) |
| **mud** | `cpf cep face num cmpl` | **Morador muda de endereço**. **VG**: marcar novo endereço com quadrado vermelho + CPF dentro (fonte pequena) |
| **dspj** | `cpf` | **Morador é despejado** (perde o endereço, vira sem-teto). **TXT**: reportar dados e endereço. **VG**: círculo preto no local do despejo |

---

## 💻 **Implementação Técnica**

### **Estrutura de Dados: Hash Files Dinâmicos**

O projeto exige implementação de **Hash Files Dinâmicos em disco** (arquivo binário):

- **Extensão**: `.hf` (arquivo principal) + opcionalmente `.hfc` (cabeçalho)
- **Saída adicional**: arquivo `.hfd` (texto esquemático) listando conteúdo e **registrando expansões de buckets**
- **Dados**: armazenar em **2 hash files distintos**:
  - Um para **quadras**
  - Um para **habitantes/moradores**

### **Organização em Módulos**

- Cada módulo deve ter:
  - Um arquivo `.h` (header) bem documentado com a **struct definida**
  - Um arquivo `.c` (implementação)
  - Um arquivo de **testes unitários** (usando a biblioteca **unity**)

### **Compilação**

Usar **Makefile** com:
- Opção `-fstack-protector-all` (proteção de stack)
- Padrão C99 (`-std=c99`)
- Targets para compilação e execução de testes unitários


## 🚀 **Parâmetros de Execução do Programa**

```bash
./ted -e <BED> -f <arquivo.geo> -o <BSD> [-pm <arquivo.pm>] [-q <arquivo.qry>]
```

| Parâmetro | Obrigatório | Significado |
|-----------|------------|-------------|
| `-e path` | Sim (S) | Diretório-base de **entrada** (BED) |
| `-f arquivo.geo` | Não (S) | Arquivo com descrição da cidade (deve estar em BED) |
| `-o path` | Não (N) | Diretório-base de **saída** (BSD) |
| `-pm arquivo.pm` | Sim (S) | Arquivo de pessoas e moradores (deve estar em BED) |
| `-q arquivo.qry` | Sim (S) | Arquivo de consultas (deve estar em BED) |

---

## 📊 **Avaliação (Critérios de Nota)**

A nota é dividida em **2 partes**:

### **Parte 1: 1.0 ponto**
- Entregar arquivo `.h` com struct do hash file dinâmico
- Testes unitários do hash file funcionando

### **Parte 2: 9.0 pontos**
- Proporcional ao **número de testes bem-sucedidos**
- Aplicar descontos conforme tabela abaixo

### **Descontos (abaixam a nota)**:

| Problema | Desconto |
|----------|----------|
| Struct não definida em `.h` | -2.5 |
| Modularização pobre (`.h` mal projetado/documentado) | até -1.5 |
| Não implementado conforme especificado ou não funcional | até -4.5 |
| Procedimentos extensos/complicados | até -1.0 |
| Makefile não funciona ou não produz executável | até -1.5 |
| Poucos commits ou concentrados no final | até -3.0 |
| Implementação ineficiente | até -1.5 |
| Módulos sem testes unitários ou testes ruins | até -2.5 |
| Makefile sem targets para testar | até -1.5 |
| **Erro de compilação** | **NOTA ZERO** |
| **Detecção de fraude** | **NOTA ZERO para todos envolvidos** |

---

## ⚠️ **Pontos Críticos**

1. **Hash Files Dinâmicos em disco são OBRIGATÓRIOS** — não é opcional
2. **Modularização bem-feita é essencial** — mesmo que o resto esteja perfeito, modularização ruim derruba muito a nota
3. **Testes unitários em cada módulo** — sem isso, perde pontos
4. **Makefile funcional** — compilação é obrigatória com as flags `-fstack-protector-all -std=c99`
5. **Git com commits bem distribuídos** — muitos commits no final é suspeito
6. **Frau acusa nota ZERO para todos** — não brinca com isso

---

## 🎬 **Fluxo Esperado do Programa**

1. Ler arquivo `.geo` e criar/populate hash file de quadras
2. Ler arquivo `.pm` e criar/populate hash file de pessoas
3. Ler arquivo `.qry` e processar cada comando:
   - Atualizar estado (pessoas nascem, morrem, mudam, são despejadas)
   - Consultar estado (dados de pessoa, contagem de moradores, censo)
   - Gerar saídas: `.txt` (resultados textuais) e visualização SVG (representação gráfica)
4. Ao final, gerar arquivos `.hfd` mostrando estado dos hash files

---

Ficou claro? Qual parte você quer que eu aprofunde mais?