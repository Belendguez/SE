/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define PRODUCER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define CONSUMER_TASK_PRIORITY (configMAX_PRIORITIES - 2)

/* Data structure for messages */
typedef struct
{
    uint32_t data;
} Message_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void producer_task(void *pvParameters);
static void consumer_task(void *pvParameters);

extern char _end;
static char *heap_end = &_end;

/*******************************************************************************
 * Code
 ******************************************************************************/
void watchdog_ini(){
 SIM->COPC = 0;
}

void *_sbrk(ptrdiff_t increment){
	char *previous_heap_end = heap_end;
	heap_end += increment;
	return (void*)previous_heap_end;
}

int main(void)
{
    /*Disable the watchdog*/
    watchdog_ini();
    
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Create message queue */
    QueueHandle_t messageQueue = xQueueCreate(5, sizeof(Message_t));

    if (messageQueue == NULL)
    {
        PRINTF("Error creating message queue.\r\n");
        while (1)
            ;
    }

    /* Create producer task */
    if (xTaskCreate(producer_task, "Producer_task", configMINIMAL_STACK_SIZE + 10, (void *)&messageQueue, PRODUCER_TASK_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Producer task creation failed!.\r\n");
        while (1)
            ;
    }

    /* Create consumer task */
    if (xTaskCreate(consumer_task, "Consumer_task", configMINIMAL_STACK_SIZE + 10, (void *)&messageQueue, CONSUMER_TASK_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Consumer task creation failed!.\r\n");
        while (1)
            ;
    }

    /* Start scheduler */
    vTaskStartScheduler();

    for (;;)
        ;
}

//Task responsible for producing data and putting it into the message queue.

static void producer_task(void *pvParameters)
{
    QueueHandle_t *queue = (QueueHandle_t *)pvParameters;

    Message_t message;

    for (;;)
    {
        /* Produce data */
        message.data = rand() % 100; // Generate random data (you can replace this with your own data source)

        /* Put data into the queue */
        if (xQueueSend(*queue, &message, portMAX_DELAY) != pdPASS)
        {
            PRINTF("Failed to send message to queue.\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Small delay to simulate data generation time
    }
}

/*!
 * @brief Task responsible for consuming data from the message queue.
 */
static void consumer_task(void *pvParameters)
{
    QueueHandle_t *queue = (QueueHandle_t *)pvParameters;

    Message_t message;

    for (;;)
    {
        /* Receive data from the queue */
        if (xQueueReceive(*queue, &message, portMAX_DELAY) == pdPASS)
        {
            /* Process data */
            PRINTF("Consumed data: %u\r\n", message.data);

            /* Simulate processing time */
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}
