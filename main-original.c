#include "MKL46Z4.h"
#include "lcd.h"
#include <stdio.h>

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc

//volatile bool rtcIsrFlag = false;
volatile int tiempo_restante = 0;

void interruptRTC_init(){
NVIC_EnableIRQ(RTC_Seconds_IRQn);
NVIC_SetPriority(RTC_Seconds_IRQn ,0);
}

void RTC_Init(void)
{
    // Habilitar el reloj para el RTC
    SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;

    // Establecer la fuente de reloj para el RTC como el reloj interno de 32 kHz
    SIM->SOPT1 &= ~SIM_SOPT1_OSC32KSEL_MASK;
    SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(0b10); // Selección del reloj interno de 32 kHz
    SIM->SOPT2 &= ~SIM_SOPT2_RTCCLKOUTSEL_MASK;

    // Habilitar el acceso de escritura a los registros del RTC
    RTC->CR |= RTC_CR_SC16P_MASK | RTC_CR_SC2P_MASK; // Habilite el acceso a los registros del RTC

    // Habilitar el temporizador RTC
    RTC->CR |= RTC_CR_OSCE_MASK; // Habilitar el oscilador de 32 kHz del RTC

    // Configurar el reloj de entrada para el RTC
    RTC->CR |= RTC_CR_CLKO_MASK; // Habilitar la salida del reloj en pin PTC3

    RTC->SR |= RTC_SR_TCE_MASK;
    // Esperar a que el oscilador interno del RTC se estabilice
    while (!(RTC->SR & RTC_SR_TIF_MASK))
    {
    }

    interruptRTC_init();

    // Habilitar la interrupción del segundo del RTC
    RTC->IER |= RTC_IER_TSIE_MASK;
}

void RTCSIntHandler(void)
{
	
    // Limpiar la bandera de interrupción
    RTC->SR &= ~RTC_SR_TIF_MASK;
    
    if (tiempo_restante >0){
    lcd_display_time(tiempo_restante/60, tiempo_restante%60);
    tiempo_restante--;
    }
    
}

void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay_Second(void)
{
  while(!(RTC->SR & RTC_SR_TIF_MASK))
  {
  }
  RTC->SR &= ~RTC_SR_TIF_MASK;
  

}

/*void iniciar_cronometro(int minutos, int segundos){

  //Convertir el tiempo inicial a segundos
  tiempo_restante = minutos*60 + segundos;
  
  //Mostrar el tiempo inicial en el LCD
  
  //Esperar un segundo antes de comenzar la cuenta atras

  
  //Habilitar el cronómetro
  RTC->SR |= RTC_SR_TCE_MASK;
  
  lcd_display_time(minutos, segundos);
  
  while (tiempo_restante>0){
  
  lcd_display_time(tiempo_restante/60, tiempo_restante%60);

  
  tiempo_restante--;
  }
  lcd_display_time(tiempo_restante/60, tiempo_restante%60);
  //Desabilitar el cronometro al finalizar la cuenta atras
  RTC->SR &= ~RTC_SR_TCE_MASK;
}*/



/*void sw1_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 3);
}


void sw2_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 12);
}

int sw1_check()
{
  return( !(GPIOC->PDIR & (1 << 3)) );
}

int sw2_check()
{
  return( !(GPIOC->PDIR & (1 << 12)) );
}


void sws_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 3 | 1 << 12);
}
*/




int main(void)
{
    // Inicializar el RTC
    RTC_Init();
    
    //__enable_irq();
    // Habilitar el reloj interno para el LCD
    irclk_ini();

    // Inicializar el LCD
    lcd_ini();

    tiempo_restante=10;
	

  while (1) {

  }

  return 0;
}
