//
// Created by urdmi on 01.05.2023.
//

#ifndef GB_STM32_PROG_H
#define GB_STM32_PROG_H

#include "stm32f1xx_hal.h"

enum type_hex_string{
    hex_data = 0,
    hex_end_file = 1,
    hex_ext_address = 4
};

typedef struct {
    uint32_t address_to_write;
    uint16_t string_begin_address;
    uint16_t count_byte;
}data_to_write;

typedef struct {
    data_to_write list[512];
    uint16_t count;
} data_list;

#define FIRMWARE_START_ADDRESS 0x08004000
#define COUNT_POSITION 1
#define ADDRESS_POSITION 3
#define TYPE_STRING_POSITION 7
#define DATA_POSITION 9

void parsing_init(uint8_t *_rx_buffer, uint16_t _buffer_size, UART_HandleTypeDef *_huart_log);
uint16_t parse_value(uint8_t * data, uint16_t start_position, uint8_t length);
uint16_t parse_data(uint8_t * data, uint8_t * data_out, uint16_t start_position, uint8_t length);
void rx_packet_handler(void);
uint8_t write_data_to_flash(uint32_t address, uint16_t data_begin, uint16_t length);
uint8_t clear_flash(uint32_t address);
void print_log(char * data);
uint16_t get_count_symbols(char *data);
#endif //GB_STM32_PROG_H
