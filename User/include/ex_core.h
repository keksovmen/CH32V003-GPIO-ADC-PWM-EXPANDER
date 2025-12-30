#pragma once



#include <stdint.h>



void ex_core_write(uint8_t reg, uint8_t value);
void ex_core_set_read_reg(uint8_t reg);
uint8_t ex_core_read();
// void ex_core_set_data(uint8_t* data, int length);
// void ex_core_set_adc_value(uint16_t adc);
// void ex_core_clear_busy();
// void ex_core_set_busy();