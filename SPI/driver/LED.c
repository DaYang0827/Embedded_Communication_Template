#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "LED.h"

//typedef struct  
//{
//    GPIO_TypeDef* port;
//    uint16_t pin;
//    uint32_t clock;
//}LED_TypeDef;

LED_TypeDef LED0 = {GPIOB, GPIO_Pin_0, RCC_AHB1Periph_GPIOB};
LED_TypeDef LED1 = {GPIOB, GPIO_Pin_1, RCC_AHB1Periph_GPIOB};
LED_TypeDef LED2 = {GPIOE, GPIO_Pin_9, RCC_AHB1Periph_GPIOE};

void led_init(LED_TypeDef* led)
{
    RCC_AHB1PeriphClockCmd(led->clock, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = led->pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Medium_Speed;
    GPIO_Init(led->port,&GPIO_InitStruct);
}

void led_on(LED_TypeDef* led)
{
    GPIO_WriteBit(led->port, led->pin, Bit_SET);
}

void led_off(LED_TypeDef* led)
{
    GPIO_WriteBit(led->port, led->pin, Bit_RESET);
}
