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
