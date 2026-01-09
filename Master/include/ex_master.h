#pragma once



#include "ex_i2c.h"



typedef enum 
{
	EX_MASTER_PORT_0,	//GPIO [0; 7]
	EX_MASTER_PORT_1,	//GPIO [8; 15]
} ex_master_port_t;

typedef enum
{
	EX_MASTER_PIN_0,
	EX_MASTER_PIN_1,
	EX_MASTER_PIN_2,
	EX_MASTER_PIN_3,
	EX_MASTER_PIN_4,
	EX_MASTER_PIN_5,
	EX_MASTER_PIN_6,
	EX_MASTER_PIN_7,
	EX_MASTER_PIN_8,
	EX_MASTER_PIN_9,
	EX_MASTER_PIN_10,
	EX_MASTER_PIN_11,
	EX_MASTER_PIN_12,
	EX_MASTER_PIN_13,
	EX_MASTER_PIN_14,
	EX_MASTER_PIN_15,
} ex_master_pin_t;

typedef enum
{
	EX_MASTER_ADC_PIN_0,
	EX_MASTER_ADC_PIN_1,
	EX_MASTER_ADC_PIN_2,
	EX_MASTER_ADC_PIN_3,
	EX_MASTER_ADC_PIN_4,
	EX_MASTER_ADC_PIN_5,
	EX_MASTER_ADC_PIN_6,
	EX_MASTER_ADC_PIN_7,
} ex_master_adc_pin_t;



typedef struct
{
	ex_i2c_t* i2c;
	uint8_t dirA;
	uint8_t dirB;
	uint8_t outA;
	uint8_t outB;
	uint8_t adc;
} ex_master_t;



/**
 * @brief init structure to work with
 * 
 * @param master out
 * @param i2c must be valid and not NULL, with set fields
 */
bool ex_master_init(ex_master_t* master, ex_i2c_t* i2c);

/**
 * @brief changes GPIO direction for a port
 * 
 * @param port
 * @param val if bit is 1 = INPUT, 0 = OUTPUT 
 * @return true 
 * @return false 
 */
bool ex_master_set_port_dir(ex_master_t* master, ex_master_port_t port, uint8_t val);

/**
 * @brief changes GPIO output value for a port
 * 
 * @param port 
 * @param val if bit is 1 = HIGH, 0 = LOW
 * @return true 
 * @return false 
 */
bool ex_master_set_port_val(ex_master_t* master, ex_master_port_t port, uint8_t val);

/**
 * @brief changes GPIO direction for a pin
 * 
 * @param pin 
 * @param is_out true = OUTPUT, false = INPUT
 * @return true 
 * @return false 
 */
bool ex_master_set_pin_dir(ex_master_t* master, ex_master_pin_t pin, bool is_out);

/**
 * @brief changes GPIO output value
 * 
 * @param pin 
 * @param state true = HIGH, false = LOW 
 * @return true 
 * @return false 
 */
bool ex_master_set_pin_val(ex_master_t* master, ex_master_pin_t pin, bool state);

/**
 * @brief configures ADC port
 * 
 * @param val if bit is 1 = ADC ON, 0 = ADC OFF
 * @return true 
 * @return false 
 */
bool ex_master_set_port_adc_mode(ex_master_t* master, uint8_t val);

/**
 * @brief configures ADC for a given pin
 * 
 * @param pin 
 * @param is_enable true = ADC ON, 0 = ADC OFF
 * @return true 
 * @return false 
 */
bool ex_master_set_pin_adc_mode(ex_master_t* master, ex_master_adc_pin_t pin, bool is_enable);

/**
 * @brief reads ADC pin
 * 
 * @param pin 
 * @param out must be NOT NULL, will store the result
 * @return true 
 * @return false 
 */
bool ex_master_adc_read(ex_master_t* master, ex_master_adc_pin_t pin, uint16_t* out);