#ifndef __LED_H__
#define __LED_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

typedef struct  
{
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t clock;
}LED_TypeDef;

extern LED_TypeDef LED0;
extern LED_TypeDef LED1;
extern LED_TypeDef LED2;


void led_init(LED_TypeDef* led);
void led_on(LED_TypeDef* led);
void led_off(LED_TypeDef* led);

#endif
