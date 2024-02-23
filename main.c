#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29
// SW1 = PTC3
// SW3 = PTC12

void delay()
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

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

void boton_init(){
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) |PORT_PCR_PS(1);
  GPIOC->PDDR &= ~(1 << 3);
}


void boton2_init(){
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) |PORT_PCR_PS(1);
  GPIOC->PDDR &= ~(1 << 12);
}

int boton_pulsado_D() {
    return !(GPIOC->PDIR & (1 << 3));
}

int boton_pulsado_I() {
    return !(GPIOC->PDIR & (1 << 12));
}


		



int nuevo_estado1(int estado){
	if (estado == 3){
	estado=0;
	}else{
	estado=estado+1;
	}
	return estado;
}

int nuevo_estado2(int estado){
	if (estado == 0){
	estado++;
	}else{
	estado=0;
	}
	return estado;
}



int main()
{
  led_green_init();
  led_red_init();
  boton_init();
  boton2_init();
  
  enum{ LED_ROJO_VERDE, LED_VERDE, LED_ROJO, LED_APAGADO} estado = LED_VERDE;
  int estado2 = 0;
  
  while (1) {
  //cambiamos los estados
  	if (boton_pulsado_I()) {
            if (estado2==0) estado2=1;
            else estado2=0;
            
 //Cambiamos estados
            switch(estado){
            case LED_ROJO_VERDE:
            estado = LED_APAGADO;
            break;
            case LED_APAGADO:
            estado = LED_ROJO_VERDE;
            break;
            case LED_VERDE:
            estado = LED_ROJO;
            break;
            case LED_ROJO:
	    estado = LED_VERDE;
            break;
            }
            while (boton_pulsado_I()); // Esperar a que se suelte el botón
            delay();
            }
        if (boton_pulsado_D()) {
            switch(estado){
            case LED_ROJO_VERDE:
            if (estado2==0)estado = LED_APAGADO;
            if (estado2==1)estado = LED_VERDE;
            break;
            case LED_APAGADO:
            if (estado2==0)estado = LED_ROJO;
            if (estado2==1)estado = LED_ROJO_VERDE;
            break;
            case LED_VERDE:
            if (estado2==0)estado = LED_ROJO_VERDE;
            if (estado2==1)estado = LED_ROJO;
            break;
            case LED_ROJO:
            if (estado2==0)estado = LED_VERDE;
            if (estado2==1)estado = LED_APAGADO;
            break;
            }
            while (boton_pulsado_D()); // Esperar a que se suelte el botón
            delay();
            }
            
            
            switch(estado){
            case LED_ROJO_VERDE:
            	led_both_toggle();
            	delay();
            	break;
            case LED_APAGADO:
            	led_green_init();
  		led_red_init();
  		delay();
  		break;
  	    case LED_VERDE:
  	    	led_green_toggle();
  	    	led_red_init();
  	    	break;
  	    case LED_ROJO:
  	    	led_red_toggle();
  	    	led_green_init();
  	    	
  	    	break;
            }
            
        }
    
 


  return 0;
}
