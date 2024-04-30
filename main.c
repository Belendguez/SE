#include "MKL46Z4.h"
#include "lcd.h"
#include <stdio.h>
#include <stdbool.h>

// LED (RG)
// LED_GREEN = PTD5 (pin 98)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc

volatile bool RTCFlag = false;
volatile int tiempo_restante = 0;
volatile int mode = 0;

void interruptRTC_init(){
NVIC_EnableIRQ(RTC_Seconds_IRQn);
NVIC_SetPriority(RTC_Seconds_IRQn ,0);
}

void watchdog_ini(){
 SIM->COPC = 0;
}

void RTC_Init(void)
{
    SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;
    SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(3);
    //RTC->SR &= ~RTC_SR_TCE_MASK;//Disable time counter
      RTC->SR &= RTC_SR_TCE(0);

    RTC->TSR = 1;
    RTC->TPR = 32768U-1000U;
    RTC->IER |= RTC_IER_TSIE(1);

    NVIC_EnableIRQ(RTC_Seconds_IRQn);
    
    RTC->SR |= RTC_SR_TCE(1);
    
    

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

void RTCSIntHandler(void)
{
	
    RTC->SR &= RTC_SR_TCE(0);
    RTC->TPR = 32768U-1000U;
    RTC->SR |= RTC_SR_TCE(1);
    
    if (RTCFlag == true){
    	if (tiempo_restante>-1){
    	lcd_display_time(tiempo_restante/60, tiempo_restante%60);
    	tiempo_restante--;
    
    	}
    }
    //led_red_toggle();
}

void delay()
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
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



void sw1_ini()
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

int sw1_check(){
    return !(GPIOC->PDIR & (1 << 3));
}

int sw2_check()
{
    return !(GPIOC->PDIR & (1 << 12));
}


void sws_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 3 | 1 << 12);
}

void Change_Minutes(){
  tiempo_restante +=60;
  lcd_display_time(tiempo_restante/60, tiempo_restante%60);
}

void Change_Seconds(){
  tiempo_restante +=1;
  lcd_display_time(tiempo_restante/60, tiempo_restante%60);
}


int main(void)
{
    watchdog_ini();
    //__enable_irq();
    // Habilitar el reloj interno para el LCD
    irclk_ini();

    // Inicializar el LCD
    lcd_ini();
    
    // Inicializar los botones
    sws_ini();
    
    led_red_init();
    
    // Inicializar el RTC
    RTC_Init();

    tiempo_restante=10;
    lcd_display_time(tiempo_restante/60, tiempo_restante%60);
    

  while (1) {
  	if (mode == 0) {
  	  if(sw1_check()){
  	  	if (RTCFlag == true) RTCFlag = false;
  	  	else RTCFlag = true;
  	  	while (sw1_check()); 
          	delay();
  	  }else if(sw2_check()){
  	  RTCFlag = false;
  	  mode = 1;
  	  while (sw2_check()); 
          delay();
  	  }
  	}
	if (mode == 1){
	  if (sw1_check()) {
	  Change_Minutes();
	  while (sw1_check()); 
          delay();
	  }else if (sw2_check()){
	  mode = 2;
	  while (sw2_check()); 
          delay();
	  }
	}
	if (mode == 2){
	  if(sw1_check()){
	  Change_Seconds();
	  while (sw1_check()); 
          delay();
	  }else if (sw2_check()){
	  mode = 0;
	  while (sw2_check()); 
          delay();
	  }
	}
  }

  return 0;
}
