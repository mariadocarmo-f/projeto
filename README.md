# Projeto BitJogo
O projeto "BitJogo" é um jogo de memória binária desenvolvido para a plataforma Raspberry Pi Pico, utilizando uma matriz de LEDs NeoPixel e um display OLED SSD1306 para interações visuais, além de botões físicos para entrada do usuário. O objetivo principal é auxiliar no aprendizado e na prática da memorização de sequências binárias de forma lúdica e interativa.
## Componentes Principais:
* Raspberry Pi Pico: Microcontrolador central do projeto, responsável pelo processamento e controle dos periféricos.
* Matriz de LEDs NeoPixel: Composta por 25 LEDs RGB dispostos em uma configuração 5x5, utilizada para exibir padrões visuais que indicam acertos ou erros durante o jogo.
* Display OLED SSD1306: Tela utilizada para apresentar instruções, sequências a serem memorizadas e feedback ao usuário.
* Botões de Entrada: Dois botões físicos conectados aos pinos GPIO do Raspberry Pi Pico, permitindo que o usuário insira sequências binárias correspondentes aos dígitos '0' e '1'.
## Funcionamento do Jogo:
1. Inicialização: Ao iniciar, o sistema configura os pinos GPIO para os botões e LEDs, inicializa a comunicação I2C para o display OLED e prepara a matriz de LEDs NeoPixel.
2. Apresentação: Uma mensagem de boas-vindas é exibida no display OLED, introduzindo o jogo ao usuário.
3. Geração da Sequência: O sistema gera aleatoriamente uma sequência binária de 7 dígitos, composta por '0's e '1's.
4. Memorização: A sequência gerada é exibida no display OLED por um período determinado (por exemplo, 5 segundos), permitindo que o usuário a memorize.
5. Entrada do Usuário: Após a exibição, o usuário é solicitado a reproduzir a sequência pressionando os botões correspondentes ('0' e '1'). Cada entrada é exibida em tempo real no display OLED para confirmação.
6. Verificação: Após a entrada completa da sequência pelo usuário, o sistema compara a sequência inserida com a sequência gerada:
   
   _o Acerto_: Se as sequências coincidirem, uma mensagem de parabéns é exibida no display OLED, e a matriz de LEDs NeoPixel exibe um padrão visual indicando sucesso (por exemplo, um quadrado verde).
   
   _o Erro_: Se houver discrepâncias, uma mensagem de erro é exibida, e a matriz de LEDs mostra um padrão indicando falha (por exemplo, um 'X' vermelho).
7. Reinício: Após um breve intervalo para que o usuário veja o feedback, o jogo reinicia, permitindo novas tentativas.
## Aspectos Técnicos:
* Debounce dos Botões: Implementado para garantir que cada pressão de botão seja registrada apenas uma vez, evitando múltiplas entradas devido a ruídos mecânicos.
* Controle da Matriz de LEDs NeoPixel: Utiliza a interface PIO (Programmable Input/Output) do Raspberry Pi Pico para comunicação eficiente com os LEDs, permitindo a exibição de padrões coloridos personalizados.
* Interface I2C para o Display OLED: Configura a comunicação I2C para enviar comandos e dados ao display SSD1306, facilitando a exibição de textos e instruções ao usuário.
## Objetivo Educacional:
O "BitJogo" serve como uma ferramenta educacional para ensinar conceitos de números binários e aprimorar habilidades de memória. A interação prática com hardware e software proporciona uma experiência de aprendizado envolvente, combinando elementos de eletrônica, programação e lógica. Este projeto é ideal para entusiastas de eletrônica e programação que desejam explorar o desenvolvimento de
jogos interativos em plataformas de microcontroladores.

Link para vídeo de apresentação do projeto: https://drive.google.com/file/d/1CB6kcohozYgRgLUu_4GXZkfAjzy87fyq/view?usp=sharing
