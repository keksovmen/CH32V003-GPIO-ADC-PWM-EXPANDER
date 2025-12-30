#pragma once



#include "stdint.h"
#include "stdbool.h"



#define EX_GPIO_MAX_PIN 16
#define EX_GPIO_ADC_MAX_PIN 8
#define EX_GPIO_ILLEGAL_PIN -1



typedef enum
{
	EX_GPIO_PORT_0,
	EX_GPIO_PORT_1,
	EX_GPIO_PORT_MAX,
} ex_gpio_port_t;



typedef void(*ex_gpio_adc_cb_t)(uint16_t val);
//

//reads gpio states
// uint8_t ex_gpio_read_port(ex_gpio_port_t port);
//set gpio to given states
// uint8_t ex_gpio_write_port(ex_gpio_port_t port);


//Low level
// init buses and whatever else you need, as default input floating state
void ex_gpio_init();
void ex_gpio_set_mode(int pin, bool is_output);
void ex_gpio_output_pin_set(int pin, bool state);
uint8_t ex_gpio_input_pin_read(int pin);
uint8_t ex_gpio_input_read(ex_gpio_port_t port);
int ex_gpio_map_to_adc(int pin);

void exp_gpio_adc_init();
void ex_gpio_set_mode_adc(int pin);
uint16_t ex_gpio_adc_read(int pin);
void ex_gpio_adc_read_irq(int pin, ex_gpio_adc_cb_t cb);
int ex_gpio_adc_map_to_gpio(int pin);
// bool ex_gpio_is_adc(ex_gpio_port_t port, int pin);