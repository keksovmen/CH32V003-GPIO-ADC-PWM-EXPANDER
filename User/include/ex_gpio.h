#pragma once



#include "stdint.h"
#include "stdbool.h"



typedef enum
{
	EX_GPIO_PORT_0,
	EX_GPIO_PORT_1,
	EX_GPIO_PORT_MAX,
} ex_gpio_port_t;



//

//reads gpio states
// uint8_t ex_gpio_read_port(ex_gpio_port_t port);
//set gpio to given states
// uint8_t ex_gpio_write_port(ex_gpio_port_t port);


//Low level
// init buses and whatever else you need, as default input floating state
void ex_gpio_init();
void ex_gpio_set_mode(ex_gpio_port_t port, int pin, bool is_output);
void ex_gpio_output_pin_set(ex_gpio_port_t port, int pin, bool state);
uint8_t ex_gpio_input_pin_read(ex_gpio_port_t port, int pin);
uint8_t ex_gpio_input_read(ex_gpio_port_t port);

void exp_gpio_adc_init();
void ex_gpio_set_mode_adc(int pin);
uint16_t ex_gpio_adc_read(int pin);