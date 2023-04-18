//
// Created by Dmitriy on 27.03.2023.
//

#include "led_lib.h"

//static GPIO_TypeDef *port_led;
//static uint16_t pin_led;
//static TIM_HandleTypeDef *timer;
//static uint8_t timer_flag = 1;
//
//void led_init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef *tim)
//{
//    port_led = GPIOx;
//    pin_led = GPIO_Pin;
//    timer = tim;
//}
//
//void led_blink(void){
//    HAL_GPIO_WritePin(port_led, pin_led,0);
//    timer->Instance->ARR = 3000;
//    HAL_TIM_Base_Start_IT(timer);
//    while (timer_flag);
//    timer_flag = 1;
//    HAL_GPIO_WritePin(port_led, pin_led,1);
//    HAL_TIM_Base_Stop(timer);
//}
//
//void led_on()
//{
//    HAL_GPIO_WritePin(port_led, pin_led,0);
//}
//void led_off()
//{
//    HAL_GPIO_WritePin(port_led, pin_led,1);
//}
//
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
//    if (htim->Instance == timer->Instance)
//    {
//        timer_flag = 0;
//    }
//}