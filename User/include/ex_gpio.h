#pragma once



#include <stdint.h>
#include <stdbool.h>



#define EX_GPIO_MAX_PIN 16
#define EX_GPIO_ILLEGAL_PIN -1



typedef enum
{
	EX_GPIO_PORT_0,
	EX_GPIO_PORT_1,
	EX_GPIO_PORT_MAX,
} ex_gpio_port_t;



/**
 * @brief Init whatever hardware you need
 */
void ex_gpio_init();

/**
 * @brief set input or output mode for given pin
 * INPUT must be FLOATING
 * OUTPUT must be PUSH PULL
 * 
 * @param pin [0; @def EX_GPIO_MAX_PIN)
 * @param is_output true is output false is input
 */
void ex_gpio_set_mode(int pin, bool is_output);

/**
 * @brief Sets logic level for given pin
 * 
 * @param pin [0; @def EX_GPIO_MAX_PIN)
 * @param state true equal logical one or HIGH voltage, false is 0
 */
void ex_gpio_output_pin_set(int pin, bool state);

/**
 * @brief reads logic level for single pin 
 * 
 * @param pin [0; @def EX_GPIO_MAX_PIN)
 * @return uint8_t 1 is HIGH, 0 is LOW
 */
uint8_t ex_gpio_input_pin_read(int pin);

/**
 * @brief reads the whole port in to 8 bits
 * bit 0 is pin 0 for the port,
 * bit 7 is pin 7 for the port
 * 
 * @param port [0; @def EX_GPIO_MAX_PIN)
 * @return uint8_t each bit position is one pin
 */
uint8_t ex_gpio_input_read(ex_gpio_port_t port);

/**
 * @brief convert GPIO pin to ADC pin if exists
 * 
 * @param pin [0; @def EX_GPIO_MAX_PIN)
 * @return int: ADC pin number or @def EX_GPIO_ILLEGAL_PIN if pin is not ADC 
 */
int ex_gpio_map_to_adc(int pin);