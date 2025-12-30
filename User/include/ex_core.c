#include "ex_core.h"

#include <stdlib.h>
#include <string.h>

#include "ex_gpio.h"



#define _STATUS_READY 0x00
#define _STATUS_BUSY 0x01

#define _REG_IODIR_A 0x00
#define _REG_IODIR_B 0x01
#define _REG_GPIO_A 0x12
#define _REG_GPIO_B 0x13

#define _REG_ADC_DATA_0_H 0x20
#define _REG_ADC_DATA_0_L 0x21
#define _REG_ADC_DATA_1_H 0x22
#define _REG_ADC_DATA_1_L 0x23
#define _REG_ADC_DATA_2_H 0x24
#define _REG_ADC_DATA_2_L 0x25
#define _REG_ADC_DATA_3_H 0x26
#define _REG_ADC_DATA_3_L 0x27
#define _REG_ADC_DATA_4_H 0x28
#define _REG_ADC_DATA_4_L 0x29
#define _REG_ADC_DATA_5_H 0x2A
#define _REG_ADC_DATA_5_L 0x2B
#define _REG_ADC_DATA_6_H 0x2C
#define _REG_ADC_DATA_6_L 0x2D
#define _REG_ADC_DATA_7_H 0x2E
#define _REG_ADC_DATA_7_L 0x2F
#define _REG_ADC_CFG 0x30
#define _REG_ADC_STATUS 0x31



typedef struct
{
	uint8_t io_a;
	uint8_t io_b;
	uint8_t gpio_a;
	uint8_t gpio_b;
	uint8_t adc_io;
	uint16_t adc_data;
	uint16_t adc_status;
	uint8_t read_reg_val;
} _registers_t;



static _registers_t _regs = {0};
// static uint8_t _data_reg = _STATUS_BUSY;



#define _FOR_ALL_PINS(port_val) {	\
	for(int pin = 0; pin < 8; pin++){	\
		const int val = (port_val >> pin) & 0x01;

#define _END_ALL_PINS }}

#define _BIT_VALUE(byte, bit) ((((byte) >> (bit)) & 0x01))
#define _CMP_BITS(a, b, bit) ((((a) >> (bit)) & 0x01) == (((b) >> (bit)) & 0x01))



static void _adc_cb(uint16_t val){
	_regs.adc_data = val;
	_regs.adc_status = _STATUS_READY;
}



static void _write_io_reg(uint8_t reg, uint8_t port_value){
	for(int bit = 0; bit < 8; bit++){
		const int pin = bit + (reg == _REG_IODIR_A ? 0 : 8);
		//only if they differ from current mode
		// if(!_CMP_BITS(reg == _REG_IODIR_A ? _regs.io_a : _regs.io_b, port_value, bit)){
			// // if this bit is ADC then skip it
			// if(ex_gpio_is_adc(reg == _REG_IODIR_A ? EX_GPIO_PORT_0 : EX_GPIO_PORT_1, bit)){
			// 	continue;
			// }

			// only if not forced by adc state
			const int adc_idx = ex_gpio_map_to_adc(pin);
			if(adc_idx != EX_GPIO_ILLEGAL_PIN && _BIT_VALUE(_regs.adc_io, adc_idx)){
				//skip
				continue;
			}
			// ex_gpio_adc_map_to_gpio()
			ex_gpio_set_mode(pin, _BIT_VALUE(port_value, bit));
		// }
	}

	if(reg == _REG_IODIR_A){
		_regs.io_a = port_value;
	}else{
		_regs.io_b = port_value;
	}
}

static void _write_gpio_reg(uint8_t reg, uint8_t port_value){
	for(int pin = 0; pin < 8; pin++){
		//only if configured as output
		// if(_BIT_VALUE(reg == _REG_GPIO_A ? _regs.io_a : _regs.io_b, pin)){
			// if this pin is ADC then skip it
			// if(ex_gpio_is_adc(reg == _REG_IODIR_A ? EX_GPIO_PORT_0 : EX_GPIO_PORT_1, pin)){
			// 	continue;
			// }
			ex_gpio_output_pin_set(pin + (reg == _REG_GPIO_A ? 0 : 8), _BIT_VALUE(port_value, pin));
		// }
	}

	if(reg == _REG_GPIO_A){
		_regs.gpio_a = port_value;
	}else{
		_regs.gpio_a = port_value;
	}
}

static void _write_adc_cfg_reg(uint8_t port_value)
{
	for(int pin = 0; pin < 8; pin++){
		// if(!_CMP_BITS(_regs.adc_io, port_value, pin)){
		if(_BIT_VALUE(port_value, pin)){
			ex_gpio_set_mode_adc(pin);
			// }else{
				//TODO: configure from current settings
				//need map from adc pin to gpio pin
			// }
		}
	}

	_regs.adc_io = port_value;
}

// static void _write_adc_data(uint8_t pin_value)
// {
// 	ex_gpio_adc_read(pin_value);
// }



void ex_core_write(uint8_t reg, uint8_t value)
{
	switch(reg)
	{
		case _REG_IODIR_A:
		case _REG_IODIR_B:
			_write_io_reg(reg, value);
			break;

		case _REG_GPIO_A:
		case _REG_GPIO_B:
			_write_gpio_reg(reg, value);
			break;
		
		case _REG_ADC_CFG:
			_write_adc_cfg_reg(value);
			break;
	}
}

void ex_core_set_read_reg(uint8_t reg)
{
	//do stuff like adc calls
	switch (reg)
	{
		case _REG_ADC_DATA_0_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 0)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(0, &_adc_cb);
			}
			break;

		case _REG_ADC_DATA_1_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 1)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(1, &_adc_cb);
			}
			break;

		case _REG_ADC_DATA_2_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 2)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(2, &_adc_cb);
			}
			break;

		case _REG_ADC_DATA_3_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 3)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(3, &_adc_cb);
			}
			break;

		case _REG_ADC_DATA_4_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 4)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(4, &_adc_cb);
			}
			break;

		case _REG_ADC_DATA_5_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 5)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(5, &_adc_cb);
			}
			break;

		case _REG_ADC_DATA_6_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 6)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(6, &_adc_cb);
			}
			break;

		case _REG_ADC_DATA_7_H:
			if(_regs.read_reg_val != _REG_ADC_STATUS && _BIT_VALUE(_regs.adc_io, 7)){
				_regs.adc_status = _STATUS_BUSY;
				ex_gpio_adc_read_irq(7, &_adc_cb);
			}
			break;
	
		default:
			break;
	}

	_regs.read_reg_val = reg;
}

uint8_t ex_core_read()
{
	switch(_regs.read_reg_val)
	{
		case _REG_IODIR_A:
			return _regs.io_a;
		case _REG_IODIR_B:
			return _regs.io_b;

		case _REG_GPIO_A:
			return ex_gpio_input_read(EX_GPIO_PORT_0);

		case _REG_GPIO_B:
			return ex_gpio_input_read(EX_GPIO_PORT_1);
		
		
		case _REG_ADC_DATA_0_H:
		case _REG_ADC_DATA_1_H:
		case _REG_ADC_DATA_2_H:
		case _REG_ADC_DATA_3_H:
		case _REG_ADC_DATA_4_H:
		case _REG_ADC_DATA_5_H:
		case _REG_ADC_DATA_6_H:
		case _REG_ADC_DATA_7_H:
			return (_regs.adc_data >> 8) & 0xFF;
		
		case _REG_ADC_DATA_0_L:
		case _REG_ADC_DATA_1_L:
		case _REG_ADC_DATA_2_L:
		case _REG_ADC_DATA_3_L:
		case _REG_ADC_DATA_4_L:
		case _REG_ADC_DATA_5_L:
		case _REG_ADC_DATA_6_L:
		case _REG_ADC_DATA_7_L:
			return _regs.adc_data & 0xFF;

		case _REG_ADC_CFG:
			return _regs.adc_io;
		
		case _REG_ADC_STATUS:
			return _regs.adc_status;
		
		default:
			return 0;
	}
}

// void ex_core_set_data(uint8_t* data, int length)
// {
// 	memcpy(_buffer.data, data, length);
// 	_buffer.length = length;
// 	_buffer.index = 0;
// }

// void ex_core_set_busy()
// {
	// uint8_t data = _STATUS_BUSY;
	// ex_core_set_data(&data, 1);
	
// }

// void ex_core_clear_busy()
// {
// 	_regs.adc_status = _STATUS_READY;
// }