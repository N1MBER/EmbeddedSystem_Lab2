#include <iostream>
#include "hal.h"

using namespace std;
unsigned int leds_num[] ={GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9,GPIO_PIN_11, GPIO_PIN_12};
unsigned int sw_num[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};
int delay;
int const T = 150;
int count;
int first_count = 7;
int second_count = 6;

void TIM6_IRQ_Handler();
void TIM7_IRQ_Handler();

int umain() {
    int prev_delay = 500;
    registerTIM6_IRQHandler(TIM6_IRQ_Handler);
    registerTIM7_IRQHandler(TIM7_IRQ_Handler);
    __enable_irq();
    while(1){
        ::count = 0;
        delay = 500;
        GPIO_PinState state1 = HAL_GPIO_ReadPin(GPIOE, sw_num[0]);
        GPIO_PinState state2 = HAL_GPIO_ReadPin(GPIOE, sw_num[1]);
        GPIO_PinState state3 = HAL_GPIO_ReadPin(GPIOE, sw_num[2]);
        GPIO_PinState state4 = HAL_GPIO_ReadPin(GPIOE, sw_num[3]);
        if (state1 == GPIO_PIN_SET)
            ::count += 8;
        if (state2 == GPIO_PIN_SET)
            ::count += 4;
        if (state3 == GPIO_PIN_SET)
            ::count += 2;
        if (state4 == GPIO_PIN_SET)
            ::count += 1;
        delay += ::count * T;
        WRITE_REG(TIM6_ARR, delay);
        WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
        WRITE_REG(TIM6_PSC, 0);
        WRITE_REG(TIM7_ARR, delay);
        WRITE_REG(TIM7_DIER, TIM_DIER_UIE);
        WRITE_REG(TIM7_PSC, 0);
        if(prev_delay != delay){
            __disable_irq();
            WRITE_REG(TIM6_ARR, delay);
            WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
            WRITE_REG(TIM6_PSC, 0);
            WRITE_REG(TIM7_ARR, delay);
            WRITE_REG(TIM7_DIER, TIM_DIER_UIE);
            WRITE_REG(TIM7_PSC, 0);
            __enable_irq();
        }
        WRITE_REG(TIM6_CR1, TIM_CR1_CEN);
        WRITE_REG(TIM7_CR1, TIM_CR1_CEN);
        HAL_GPIO_WritePin(GPIOD, leds_num[first_count], GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, leds_num[second_count], GPIO_PIN_SET);
        prev_delay = delay;
    }
    return 0;
}

void TIM6_IRQ_Handler(){
    HAL_GPIO_WritePin(GPIOD, leds_num[first_count], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, leds_num[second_count], GPIO_PIN_RESET);
}

void TIM7_IRQ_Handler(){
    first_count--;
    second_count--;
    if (first_count == -1)
        first_count = 7;
    if  (second_count == -1)
        second_count = 7;
    HAL_GPIO_WritePin(GPIOD, leds_num[first_count], GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, leds_num[second_count], GPIO_PIN_SET);
}