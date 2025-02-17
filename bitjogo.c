#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

#define BUTTON_A 5  // GPIO do botão A (0)
#define BUTTON_B 6  // GPIO do botão B (1)
#define LED_GREEN 11 // GPIO do LED verde (acerto)
#define LED_RED 13   // GPIO do LED vermelho (erro)
#define SEQ_LENGTH 7 // Tamanho da sequência binária

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;


/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin) {
    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
  
    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
      np_pio = pio1;
      sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }
  
    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
  
    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i) {
      leds[i].R = 0;
      leds[i].G = 0;
      leds[i].B = 0;
    }
  }
  
  /**
   * Atribui uma cor RGB a um LED.
   */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
  }
  
  /**
   * Limpa o buffer de pixels.
   */
  void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
      npSetLED(i, 0, 0, 0);
  }

  /**
   * Escreve os dados do buffer nos LEDs.
   */
void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
      pio_sm_put_blocking(np_pio, sm, leds[i].G);
      pio_sm_put_blocking(np_pio, sm, leds[i].R);
      pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

/**
 * Converte coordenadas de linha e coluna para índice no buffer.
 */
int getIndex(int row, int col) {
    return row * 5 + col; // Converte coordenadas (linha, coluna) em índice linear.
}
  
// Exibe um texto no display OLED
void display_text(const char *text1, const char *text2, const char *text3) {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Zera o display antes de exibir novos textos
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, sizeof(ssd));  

    ssd1306_draw_string(ssd, 5, 8, text1);
    ssd1306_draw_string(ssd, 5, 28, text2);
    ssd1306_draw_string(ssd, 5, 48, text3);

    render_on_display(ssd, &frame_area);
    
    sleep_ms(100); // Pequeno atraso para garantir atualização
}

// Gera uma sequência aleatória de 0s e 1s
void generate_sequence(char *sequence) {
    for (int i = 0; i < SEQ_LENGTH; i++) {
        sequence[i] = (rand() % 2) ? '1' : '0';
    }
    sequence[SEQ_LENGTH] = '\0';
}

// Configuração inicial dos periféricos
void setup() {
    stdio_init_all();
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    // Inicializa o display
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
 
    ssd1306_init();

    // Inicializa matriz de LEDs NeoPixel.
    npInit(LED_PIN);
    npClear();
}

/**
 * Desenha na matriz um x de errou.
 */
void errou(uint8_t r, uint8_t g, uint8_t b) {
    npClear();
    // desenha sinal de mais +
    // Mapeamento corrigido do coração na matriz 5x5.
    int heart[5][5] = {
      {1, 0, 0, 0, 1},
      {0, 1, 0, 1, 0},
      {0, 0, 1, 0, 0},
      {0, 1, 0, 1, 0},
      {1, 0, 0, 0, 1}
    };
  
    // Define os LEDs que formam o desenho.
    for (int row = 0; row < 5; row++) {
      for (int col = 0; col < 5; col++) {
        if (heart[row][col]) {
          int index = getIndex(row, col);
          npSetLED(index, r, g, b);
        }
      }
    }
    npWrite();
  }
  
// Indica sucesso desenhando um quadrado  
void ganhou(uint8_t r, uint8_t g, uint8_t b) {
    npClear();
    // desenha um quadrado
    // Mapeamento corrigido do coração na matriz 5x5.
    int heart[5][5] = {
      {1, 1, 1, 1, 1},
      {1, 0, 0, 0, 1},
      {1, 0, 1, 0, 1},
      {1, 0, 0, 0, 1},
      {1, 1, 1, 1, 1}
    };
  
    // Define os LEDs que formam o desenho.
    for (int row = 0; row < 5; row++) {
      for (int col = 0; col < 5; col++) {
        if (heart[row][col]) {
          int index = getIndex(row, col);
          npSetLED(index, r, g, b);
        }
      }
    }
    npWrite();
}

// apaga todos os leds
void apaga(uint8_t r, uint8_t g, uint8_t b) {
    npClear();
    // desenha um quadrado
    // Mapeamento corrigido do coração na matriz 5x5.
    int heart[5][5] = {
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0}
    };
  
    // Define os LEDs que formam o desenho.
    for (int row = 0; row < 5; row++) {
      for (int col = 0; col < 5; col++) {
        if (heart[row][col]) {
          int index = getIndex(row, col);
          npSetLED(index, r, g, b);
        }
      }
    }
    npWrite();
}

// Verifica se o botão foi pressionado com debounce
bool debounce(int button) {
    if (!gpio_get(button)) {
        sleep_ms(20); // Debounce delay
        if (!gpio_get(button)) {
            return true;
        }
    }
    return false;
}

// jogo principal
void play_game() {
    apaga(0,0,0); // limpa a matriz de led

    char sequence[SEQ_LENGTH + 1];
    char user_sequence[SEQ_LENGTH + 1] = "";
    generate_sequence(sequence);
    display_text("Memorize:", sequence, "5 seg...");
    sleep_ms(5000);
    display_text("Digite a", "sequencia", "  A 0   B 1");
    
    int index = 0;
    while (index < SEQ_LENGTH) {
        if (debounce(BUTTON_A)) { // SE BUTTON A FOR PRECIONADO VALOR 0
            user_sequence[index] = '0';
            user_sequence[index + 1] = '\0';
            char display_buffer[32];
            snprintf(display_buffer, sizeof(display_buffer), "%s", user_sequence);
            display_text("Digite a", "sequencia", display_buffer);
            printf("Sequencia digitada: %s\n", user_sequence);
            gpio_put(LED_GREEN, sequence[index] == '0');
            gpio_put(LED_RED, sequence[index] != '0');
            sleep_ms(500);
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_RED, 0);
            index++;
            while (debounce(BUTTON_A));
        }
        if (debounce(BUTTON_B)) {
            user_sequence[index] = '1';
            user_sequence[index + 1] = '\0';
            char display_buffer[32];
            snprintf(display_buffer, sizeof(display_buffer), "%s", user_sequence);
            display_text("Digite a", "sequencia", display_buffer);
            printf("Sequencia digitada: %s\n", user_sequence);
            gpio_put(LED_GREEN, sequence[index] == '1');
            gpio_put(LED_RED, sequence[index] != '1');
            sleep_ms(500);
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_RED, 0);
            index++;
            while (debounce(BUTTON_B));
        }
    }
    user_sequence[SEQ_LENGTH] = '\0';
    printf("Sequencia final digitada: %s\n", user_sequence);
    
    if (strcmp(sequence, user_sequence) == 0) {
        display_text("Parabens!", "Sequencia", "correta!");
        ganhou(0, 160, 0); // aparece na matriz de led - Verde.
    } else {
        display_text("Errou", "Tente", "novamente.");
        errou(160, 0, 0); // aparece na matriz de led -  Vermelho.
    }
    sleep_ms(3000);
    npClear(); // limpa a matriz de led

}

int main() {
    setup();
    display_text("JOGO", "MEMORIA", "BINARIO"); // APRESENTACAO
    sleep_ms(8000);
    while (1) {
        play_game();
        sleep_ms(5000);
    }
}