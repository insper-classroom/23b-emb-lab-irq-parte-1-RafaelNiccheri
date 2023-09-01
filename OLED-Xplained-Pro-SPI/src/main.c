#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
#include <string.h>

#define LED1_PIO           PIOA
#define LED1_PIO_ID        ID_PIOA
#define LED1_IDX       0
#define LED1_IDX_MASK (1 << LED1_IDX)

#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        ID_PIOD
#define BUT1_IDX       28
#define BUT1_IDX_MASK (1u << BUT1_IDX)

#define BUT2_PIO            PIOC
#define BUT2_PIO_ID         ID_PIOC
#define BUT2_IDX        31
#define BUT2_IDX_MASK   (1u << BUT2_IDX)

#define BUT3_PIO            PIOA
#define BUT3_PIO_ID         ID_PIOA
#define BUT3_IDX        19
#define BUT3_IDX_MASK   (1u << BUT3_IDX)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
/* flag */
volatile char but1_flag = 0;
volatile char but1_2_flag = 0;
volatile char but2_flag = 0;
volatile char but3_flag = 0;

/************************************************************************/
/* prototype                                                            */
/************************************************************************/
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void frequency_handler(int pressed);
void io_init(void);
void pisca_led(int n, int t);

/************************************************************************/
/* handler / callbacks                                                  */
/************************************************************************/
void but1_callback(void) {
	but1_flag = !but1_flag;
}



void but2_callback(void) {
	but2_flag = !but2_flag;
}

void but3_callback(void) {
	but3_flag = !but3_flag;
}

/************************************************************************/
/* funções                                                              */
/************************************************************************/

// pisca led N vez no periodo T
void pisca_led(int n, int t){
	for (int i=0;i<n;i++){
		pio_clear(LED1_PIO, LED1_IDX_MASK);
		delay_ms(t);
		pio_set(LED1_PIO, LED1_IDX_MASK);
		delay_ms(t);
	}
}

// Inicializa botao SW0 do kit com interrupcao
void io_init(void)
{

	// Configura led
	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_IDX_MASK, PIO_DEBOUNCE);

	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_IDX_MASK, 60);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_IDX_MASK, 60);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_IDX_MASK, 60);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT1_PIO, BUT1_PIO_ID, BUT1_IDX_MASK, PIO_IT_FALL_EDGE, but1_callback);
	pio_handler_set(BUT2_PIO, BUT2_PIO_ID, BUT2_IDX_MASK, PIO_IT_FALL_EDGE, but2_callback);
	pio_handler_set(BUT3_PIO, BUT3_PIO_ID, BUT3_IDX_MASK, PIO_IT_FALL_EDGE, but3_callback);

	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	pio_enable_interrupt(BUT2_PIO, BUT2_IDX_MASK);
	pio_get_interrupt_status(BUT2_PIO);
	pio_enable_interrupt(BUT3_PIO, BUT3_IDX_MASK);
	pio_get_interrupt_status(BUT3_PIO);
	
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4); // Prioridade 4
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	io_init();

  // Init OLED
	gfx_mono_ssd1306_init();
  
  
// 	gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
//     gfx_mono_draw_string("mundo", 50,16, &sysfont);

  /* Insert application code here, after the board has been initialized. */
  int frequency = 1;
	while(1) {
		frequency = 1;		
		char str[14];
		sprintf(str, "%d", frequency);
			// Escreve na tela um circulo e um texto
			char str1[7];
			char str2[7];
			for(int i=1;i<=30;i++){
				if(but1_flag){
					delay_ms(100);
					if(pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK)) {
						if (frequency < 10) {
							frequency++;
						}
					}
					else if(frequency > 1) {
						frequency--;
					}
					but1_flag = 0;
					sprintf(str, "%d", frequency); // (2)
				}
				
				if(but3_flag) {
					if (frequency > 1) {
						frequency--;
					}
					but3_flag = 0;
					sprintf(str, "%d", frequency); // (2)
				}
				
				while(but2_flag){
					gfx_mono_draw_string("PAUSE", 70,16, &sysfont);
				}
				gfx_mono_draw_string("     ", 70,16, &sysfont);
				but2_flag = 0;
				
				float temp = i*6/30;
				int progress = (int) temp;
				strcpy (str1," ");
				strcpy (str2,"??????");
				strncat (str1, str2, progress);
				gfx_mono_draw_string("       ", 0,16, &sysfont);
				gfx_mono_draw_string(str1, 0,16, &sysfont);
				if(frequency == 10){
					gfx_mono_draw_string(str, 40,0, &sysfont);
				}
				else{
					gfx_mono_draw_string("0", 40,0, &sysfont);
					gfx_mono_draw_string(str, 50,0, &sysfont);
				}
				gfx_mono_draw_string("Hz", 60,0, &sysfont);
				int delay = 500 / frequency;
				pisca_led(1, delay);				
			}
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}
