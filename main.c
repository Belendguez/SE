#include "MKL46Z4.h"

// Definiciones de pines para los LEDs y los botones
#define LED_GREEN_PIN   5   // PTD5
#define LED_RED_PIN     29  // PTE29
#define BUTTON1_PIN     3   // PTC3
#define BUTTON2_PIN     12  // PTC12

// Estados de los LEDs
enum LED_STATE {
    GREEN_ONLY,
    RED_ONLY,
    BOTH_ON,
    BOTH_OFF
};

// Variable para almacenar el estado actual de los LEDs
volatile enum LED_STATE current_state = GREEN_ONLY;

// Función para inicializar el LED verde
void led_green_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[LED_GREEN_PIN] = PORT_PCR_MUX(1);
    GPIOD->PDDR |= (1 << LED_GREEN_PIN);
    GPIOD->PSOR = (1 << LED_GREEN_PIN);
}

// Función para inicializar el LED rojo
void led_red_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[LED_RED_PIN] = PORT_PCR_MUX(1);
    GPIOE->PDDR |= (1 << LED_RED_PIN);
    GPIOE->PSOR = (1 << LED_RED_PIN);
}

// Función para inicializar el botón 1
void button1_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[BUTTON1_PIN] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IRQC(10); // Configura IRQC para flanco de bajada
    GPIOC->PDDR &= ~(1 << BUTTON1_PIN);
}

// Función para inicializar el botón 2
void button2_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[BUTTON2_PIN] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IRQC(10); // Configura IRQC para flanco de bajada
    GPIOC->PDDR &= ~(1 << BUTTON2_PIN);
}

void invert_leds() {
    switch (current_state) {
        case GREEN_ONLY:
            GPIOD->PCOR = (1 << LED_GREEN_PIN); // Apagar LED verde
            GPIOE->PSOR = (1 << LED_RED_PIN);   // Encender LED rojo
            current_state = RED_ONLY;
            break;
        case RED_ONLY:
            GPIOD->PSOR = (1 << LED_GREEN_PIN); // Encender LED verde
            GPIOE->PCOR = (1 << LED_RED_PIN);   // Apagar LED rojo
            current_state = GREEN_ONLY;
            break;
        case BOTH_ON:
            GPIOD->PCOR = (1 << LED_GREEN_PIN); // Apagar LED verde
            GPIOE->PCOR = (1 << LED_RED_PIN);   // Apagar LED rojo
            current_state = BOTH_OFF;
            break;
        case BOTH_OFF:
            GPIOD->PSOR = (1 << LED_GREEN_PIN); // Encender LED verde
            GPIOE->PSOR = (1 << LED_RED_PIN);   // Encender LED rojo
            current_state = BOTH_ON;
            break;
    }
}

// Función para manejar las interrupciones de los botones
void PORTDIntHandler() {
    if (PORTC->ISFR & (1 << BUTTON1_PIN)) {
        // Handle button 1 interrupt
        PORTC->ISFR |= (1 << BUTTON1_PIN); // Clear the interrupt flag
        current_state = (current_state + 1) % 4; // Cambiar al siguiente estado
    }

    if (PORTC->ISFR & (1 << BUTTON2_PIN)) {
        // Handle button 2 interrupt
        PORTC->ISFR |= (1 << BUTTON2_PIN); // Clear the interrupt flag
        invert_leds();
    }
}

// Función para actualizar los LEDs según el estado actual
void update_leds() {
    switch (current_state) {
        case GREEN_ONLY:
            GPIOD->PCOR = (1 << LED_GREEN_PIN); // Encender LED verde
            GPIOE->PSOR = (1 << LED_RED_PIN);   // Apagar LED rojo
            break;
        case RED_ONLY:
            GPIOD->PSOR = (1 << LED_GREEN_PIN); // Apagar LED verde
            GPIOE->PCOR = (1 << LED_RED_PIN);   // Encender LED rojo
            break;
        case BOTH_ON:
            GPIOD->PCOR = (1 << LED_GREEN_PIN); // Encender LED verde
            GPIOE->PCOR = (1 << LED_RED_PIN);   // Encender LED rojo
            break;
        case BOTH_OFF:
            GPIOD->PSOR = (1 << LED_GREEN_PIN); // Apagar LED verde
            GPIOE->PSOR = (1 << LED_RED_PIN);   // Apagar LED rojo
            break;
    }
}


int main() {
    //disable Watchdog
    SIM->COPC &= ~0b1100;
    // Configurar las interrupciones del Puerto C
    NVIC_EnableIRQ(PORTC_PORTD_IRQn); 
    NVIC_SetPriority(PORTC_PORTD_IRQn, 0); // Establecer la prioridad de la interrupción

    // Inicializar los LEDs y los botones
    led_green_init();
    led_red_init();
    button1_init();
    button2_init();

    while (1) {
        // Actualizar los LEDs según el estado actual
        update_leds();
    }

    return 0;
}
