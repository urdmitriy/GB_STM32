//
// Created by Dmitriy on 27.03.2023.
//

#ifndef GB_STM32_LED_LIB_H
#define GB_STM32_LED_LIB_H

#include "stm32f1xx_hal.h"

void led_blink(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef timer);

#endif //GB_STM32_LED_LIB_H
