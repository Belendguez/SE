#include "MKL46Z4.h"

// LED_GREEN = PTD5
void led_green_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOD->PSOR = (1 << 5);
}

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
}

// LED_RED = PTE29
void led_red_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);
}

void led_red_toggle()
{
  GPIOE->PTOR = (1 << 29);
}

void led_both_toggle()
{
  GPIOD->PTOR = (1 << 5);
  GPIOE->PTOR = (1 << 29);
}

void delay()
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

void boton_init(){
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) |PORT_PCR_PS(1) | PORT_PCR_IRQC(10); // Configura IRQC para flanco de bajada
  GPIOC->PDDR &= ~(1 << 3);
}

void boton2_init(){
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) |PORT_PCR_PS(1) | PORT_PCR_IRQC(10); // Configura IRQC para flanco de bajada
  GPIOC->PDDR &= ~(1 << 12);
}

void PORTC_PORTD_IRQHandler()
{
    if (PORTC->ISFR & (1 << 3))
    {
        // Handle SW1 interrupt
        // Your code here
        PORTC->ISFR |= (1 << 3); // Clear the interrupt flag
        if (boton_pulsado_D()) {
            led_green_toggle();
        }
    }

    if (PORTC->ISFR & (1 << 12))
    {
        // Handle SW3 interrupt
        // Your code here
        PORTC->ISFR |= (1 << 12); // Clear the interrupt flag
        if (boton_pulsado_I()) {
            led_red_toggle();
        }
    }
}

int boton_pulsado_D() {
    return !(GPIOC->PDIR & (1 << 3));
}

int boton_pulsado_I() {
    return !(GPIOC->PDIR & (1 << 12));
}

int main() {
    NVIC_EnableIRQ(PORTC_PORTD_IRQn); // Habilita las interrupciones del Puerto C y D en el NVIC
    NVIC_SetPriority(PORTC_PORTD_IRQn, 2); // Establece la prioridad de la interrupción

    boton_init();
    boton2_init();

    led_green_init();
    led_red_init();

    while(1) {
        // Aquí puedes agregar cualquier otra lógica necesaria para tu aplicación
    }
    return 0;
}