//
// Created by urdmi on 01.05.2023.
//

#include "prog.h"
#include "stdlib.h"

static uint8_t *rx_buffer;
static uint16_t buffer_size;
static uint32_t address_data_ext = 0;
static uint8_t flash_is_clear = 0;
static data_list data_for_write;
static UART_HandleTypeDef *huart_log;

void parsing_init(uint8_t *_rx_buffer, uint16_t _buffer_size, UART_HandleTypeDef *_huart_log)
{
    rx_buffer = _rx_buffer;
    buffer_size = _buffer_size;
    huart_log = _huart_log;
}

uint16_t parse_value(uint8_t * data, uint16_t start_position, uint8_t length)
{
    char data_string[5] = {'\0',};
    uint8_t write_position = 0;
    for (int i = 0; i < length * 2; ++i) {
        data_string[write_position++] = *(data + start_position + i);
    }
    uint32_t result = strtol(data_string, NULL, 16);
    return result;
}

uint16_t parse_data(uint8_t * data, uint8_t * data_out, uint16_t start_position, uint8_t length)
{
    char data_string[3] = {'\0',};
    uint8_t crc_received_packet = 0;
    uint16_t write_position = 0;
    for (int i = 0; i < length; ++i)
    {
        for (int j = 0; j < 2; ++j) {
            data_string[j] = *(data + start_position + DATA_POSITION + i * 2 + j);
        }

        uint8_t current_decode_byte = strtol(data_string, NULL, 16);
        crc_received_packet += current_decode_byte;
        *(data_out + write_position++) = current_decode_byte;
    }
    return crc_received_packet;
}

void rx_packet_handler(void )
{
    uint16_t current_byte = 0;
    uint16_t count_string = 0;
    uint8_t stop_parsing = 0;
    uint8_t error = 0;
    while (!stop_parsing && current_byte <= buffer_size)
    {
        if (rx_buffer[current_byte] == ':')
        {
            uint8_t data_count_byte =
                    parse_value(rx_buffer, current_byte + COUNT_POSITION, 1);
            uint32_t data_address =
                    parse_value(rx_buffer, current_byte + ADDRESS_POSITION, 2);
            uint8_t data_type =
                    parse_value(rx_buffer, current_byte + TYPE_STRING_POSITION, 1);
            uint8_t crc_send_packet =
                    parse_value(rx_buffer, current_byte + DATA_POSITION + data_count_byte * 2, 1);
            uint16_t crc_calculate = 0;
            crc_calculate += data_count_byte;
            crc_calculate += (data_address & 0xFF00)>>8;
            crc_calculate += data_address & 0x00FF;
            crc_calculate += data_type;
            data_address += address_data_ext;
            switch (data_type) {
                case hex_ext_address: //если расширенный адрес
                {
                    uint16_t address_data =
                            parse_value(rx_buffer, current_byte + DATA_POSITION, 2);
                    address_data_ext = address_data << 16;
                    crc_calculate += (address_data & 0xFF00) >> 8;
                    crc_calculate += address_data & 0x00FF;
                    uint8_t result_crc_addr = crc_calculate + crc_send_packet;
                    if (result_crc_addr != 0)
                    {
                        error++;
                    }
                    break;
                }
                case hex_end_file: //если конец файла
                {
                    stop_parsing = 1;
                    break;
                }
                case hex_data:  //если данные
                {
                    uint8_t data[32] = {'\0',};
                    crc_calculate += parse_data(rx_buffer, data, current_byte,
                                                data_count_byte);
                    uint8_t result_crc = crc_calculate + crc_send_packet;
                    if (result_crc != 0) //если данные с ошибкой
                    {
                        error++;
                    }
                    else //если данные верны добавляем их в структуру данных
                    {
                        data_for_write.list[count_string].address_to_write = data_address;
                        data_for_write.list[count_string].count_byte = data_count_byte;
                        data_for_write.list[count_string].string_begin_address = current_byte;
                        data_for_write.count = count_string++;
                    }
                    break;
                }
                default:
                    break;
            }

            while (rx_buffer[++current_byte] != ':' && current_byte < buffer_size); // перемещаем каретку до начала строки
        }
    }
    if (!error) //если данные не содержат ошибок, пишем их в мк
    {
        char messageCRC[] = {"CRC OK\n\r\0"};
        print_log(messageCRC);

        int count_write_bytes = 0;
        for (int i = 0; i < data_for_write.count; ++i) {
            error += write_data_to_flash(data_for_write.list[i].address_to_write,
                                         data_for_write.list[i].string_begin_address,
                                         data_for_write.list[i].count_byte);
            if (error == HAL_OK) count_write_bytes += data_for_write.list[i].count_byte;
        }
        if (error == HAL_OK)
        {
            char message[] = {"Write complete successful\n\r\0"};
            print_log(message);
        }
        else
        {
            char message[] = {"ERROR!\n\r\0"};
            print_log(message);
        }
    }
    else
    {
        char messageCRC[] = {"CRC ERROR\n\r\0"};
        print_log(messageCRC);
    }
}

uint8_t write_data_to_flash(uint32_t address, uint16_t data_begin, uint16_t length)
{
    uint8_t data[32] = {0,};
    parse_data(rx_buffer,data,data_begin,length);
    uint8_t result = 0;
    if (!flash_is_clear) result = clear_flash(FIRMWARE_START_ADDRESS);
    if (result == HAL_OK)
    {
        HAL_FLASH_Unlock();
        for (int i = 0; i < length; i+=2) {
            uint32_t address_to_write = address + i;
            __IO uint64_t data_to_write  = data[i+1];
                          data_to_write <<= 8;
                          data_to_write |= data[i];
            result += HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address_to_write,data_to_write);
        }
        HAL_FLASH_Lock();
    }

    return result;
}

uint8_t clear_flash(uint32_t address)
{
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase_settings;
    erase_settings.PageAddress = address;
    erase_settings.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_settings.NbPages = 20;
    uint32_t page_error[256];
    HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&erase_settings, page_error);
    HAL_FLASH_Lock();
    if (result == HAL_OK)
    {
        flash_is_clear = 1;
    }
    return result;
}

void print_log(char * data)
{
    uint16_t count = get_count_symbols(data);
    char end_string[] = {"\n\r"};
    HAL_UART_Transmit(huart_log,(uint8_t*)data,count,1000);
    HAL_UART_Transmit(huart_log,(uint8_t*)end_string, get_count_symbols(end_string),1000);
}

uint16_t get_count_symbols(char *data)
{
    uint16_t current_symbol_position = 0;
    while (*(data + current_symbol_position) != '\0')
    {
        current_symbol_position++;
    }
    return current_symbol_position;
}