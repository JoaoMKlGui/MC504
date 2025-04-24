## INTEGRANTES

- Alexandre Leandro Seixas Adler (RA: 260355)  
- Gabriel Pereira Viterbo (RA: 217600)  
- João Vinicius Moreira K. Guimarães (RA: 260542)

---

## Projeto: Selvagens Famintos e o Cozinheiro

Este projeto tem como base o clássico problema de concorrência conhecido como "Selvagens Famintos", proposto por Andrews e Schneider. Ele foi implementado em linguagem C, utilizando os principais recursos de programação concorrente, como threads, mutexes, semáforos e variáveis de condição, por meio da biblioteca `pthread`. O objetivo é modelar uma situação onde múltiplas threads (representando selvagens) acessam um recurso compartilhado (um caldeirão com porções de comida), que é reabastecido por uma thread separada (o cozinheiro). O diferencial do projeto está na implementação de uma interface textual animada em ASCII, que permite visualizar em tempo real o comportamento dos selvagens, a atividade do cozinheiro e o número de porções disponíveis.

---

## Objetivo do Projeto

O projeto visa simular a dinâmica de um grupo de selvagens que compartilha um caldeirão limitado de porções. Quando o caldeirão se esvazia, é necessário acordar o cozinheiro para que ele o reabasteça. O foco está em resolver problemas clássicos de sincronização, como condições de corrida, deadlocks e starvation, garantindo que o sistema funcione corretamente mesmo com múltiplas threads atuando simultaneamente.

---

## Conceitos Utilizados

Foram aplicadas técnicas fundamentais de programação concorrente. O controle de acesso ao caldeirão é feito com mutexes e semáforos, assegurando exclusão mútua e coordenação entre threads. Variáveis de condição são usadas para que os selvagens aguardem adequadamente o reabastecimento e para que o cozinheiro só seja acordado quando necessário. Além disso, há controle para evitar que múltiplos selvagens acordem o cozinheiro simultaneamente, o que exige lógica refinada de sincronização.

---

## Lógica da Simulação

Cada selvagem é representado por uma thread que segue um ciclo contínuo: sair da tribo, caminhar até o caldeirão, tentar se servir de uma porção (caso ainda haja comida) e retornar à tribo. Se o caldeirão estiver vazio, apenas o primeiro selvagem a chegar solicita que o cozinheiro seja acordado. O cozinheiro então reabastece o caldeirão com um número fixo de porções e volta a dormir. Esse fluxo simula um ambiente realista de acesso concorrente a um recurso finito, com comportamento reativo e sincronizado entre as entidades.

---

## Visualização e Interface

A interface em modo texto foi projetada para facilitar a compreensão visual da simulação. Elementos gráficos simples em ASCII representam os selvagens, o caldeirão e o cozinheiro. O número de porções no caldeirão é exibido dinamicamente, e os personagens se movimentam na tela em tempo real, refletindo suas ações (ir até o caldeirão, servir-se, retornar à tribo, cozinhar, etc). Isso torna o comportamento do sistema observável e intuitivo, mesmo sem depuração passo a passo.

---

## Parâmetros Personalizáveis

Para facilitar testes e simulações com diferentes cenários, o código permite a personalização de parâmetros por meio de constantes definidas no início do arquivo. É possível alterar:

- A quantidade de selvagens (número de threads consumidoras)
- A capacidade do caldeirão (número máximo de porções)
- O intervalo de tempo entre atualizações da interface (tempo de animação e simulação)

Essas alterações não requerem mudanças na lógica do programa e permitem analisar o comportamento do sistema sob diferentes condições de carga e concorrência.

---

## Estrutura do Código

A aplicação foi modularizada em funções específicas para facilitar a leitura, manutenção e expansão do código. A seguir, um resumo das principais funções que compõem o sistema:

- *main*: É o ponto de entrada da aplicação. Inicializa todas as variáveis compartilhadas, mutexes, semáforos e variáveis de condição. Em seguida, cria as threads correspondentes aos selvagens, ao cozinheiro e à atualização da interface, e dá início à simulação. Também é responsável por tratar o encerramento do programa e liberar os recursos utilizados.

- *savage*: Função executada por cada thread de selvagem. Cada selvagem realiza um ciclo contínuo que envolve sair da tribo, tentar servir-se do caldeirão, e, se necessário, acordar o cozinheiro. Utiliza mutexes e variáveis de condição para coordenar o acesso ao recurso compartilhado e evitar problemas de concorrência.

- *cook_thread*: Representa a thread do cozinheiro. Ela permanece em estado de espera até ser acordada por um selvagem quando o caldeirão estiver vazio. Após ser acordado, o cozinheiro reabastece o caldeirão com novas porções e volta ao estado de espera. Sua lógica garante que ele só seja ativado uma vez por reabastecimento.

- *status_printer*: Responsável por atualizar dinamicamente a interface em modo texto. Exibe a posição e as ações dos personagens, bem como a quantidade atual de porções no caldeirão. Garante uma visualização intuitiva e clara da simulação em tempo real.

---

## Tecnologias Utilizadas

O projeto foi desenvolvido em linguagem *C*, utilizando as seguintes bibliotecas padrão:

- pthread.h — Criação e controle de múltiplas threads.
- semaphore.h — Semáforos POSIX para controle de acesso sincronizado.
- unistd.h — Funções utilitárias como sleep e usleep.
- time.h — Manipulação de tempo e geração de delays personalizados.

Todo o desenvolvimento e testes foram realizados em um ambiente *Linux*, aproveitando o suporte robusto a programação concorrente com pthreads e semáforos POSIX.

---

## Destaques do Projeto

- *Visualização animada em ASCII*: Permite acompanhar em tempo real a movimentação dos selvagens, o estado do caldeirão e a atividade do cozinheiro.
- *Organização do código*: As funções são bem segmentadas e a lógica de sincronização está clara e encapsulada.
- *Estabilidade na execução*: O projeto foi cuidadosamente testado para evitar condições de corrida, deadlocks e starvation, garantindo um comportamento previsível e correto mesmo sob diferentes cenários de carga.

---

## Instruções de Execução

Para compilar e rodar o projeto, utilize os seguintes comandos no terminal:

```bash
gcc selvagens.c -o selvagens -lpthread
./selvagens
