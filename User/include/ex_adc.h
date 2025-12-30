#pragma once



#include <stdint.h>



#define EX_ADC_MAX_PIN 8



/**
 * @brief IRQ callback, must be as small as possible,
 * just put your ADC data somewhere and mark a flag
 */
typedef void(*ex_adc_cb_t)(uint16_t val);



/**
 * @brief init hardware and anything else you need
 */
void exp_adc_init();

/**
 * @brief configures given pin in to ADC input mode
 * 
 * @param pin [0; @def EX_ADC_MAX_PIN)
 */
void ex_adc_enable_pin(int pin);

/**
 * @brief start ADC conversion and waits for it to finish
 * 
 * @param pin [0; @def EX_ADC_MAX_PIN)
 * @return uint16_t ADC value
 */
uint16_t ex_adc_read(int pin);

/**
 * @brief start ADC conversion and registers a callback to call
 * 
 * @param pin [0; @def EX_ADC_MAX_PIN)
 * @param cb if NULL nothing will be called, must exit as fast as possible
 */
void ex_adc_read_irq(int pin, ex_adc_cb_t cb);

/**
 * @brief maps ADC pin [0; @def EX_ADC_MAX_PIN) to GPIO pin if such
 * corelation is exists
 * 
 * @param pin 
 * @return int ADC pin number or @def EX_GPIO_ILLEGAL_PIN if pin is not ADC 
 */
int ex_adc_map_to_gpio(int pin);