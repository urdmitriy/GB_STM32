//
// Created by Dmitriy on 27.03.2023.
//

#include "led_lib.h"

uint8_t timer_flag = 1;

void led_blink(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef timer){
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin,0);
    timer.Instance->ARR = 3000;
    HAL_TIM_Base_Start_IT(&timer);
    while (timer_flag);
    timer_flag = 1;
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin,1);
    HAL_TIM_Base_Stop(&timer);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    timer_flag = 0;
}