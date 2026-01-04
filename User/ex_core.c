#include "ex_core.h"

#include <stdlib.h>
#include <string.h>

#include "ex_adc.h"
#include "ex_gpio.h"
#include "ex_protocol.h"



#define _BIT_VALUE(byte, bit) ((((byte) >> (bit)) & 0x01))
#define _CMP_BITS(a, b, bit) ((((a) >> (bit)) & 0x01) == (((b) >> (bit)) & 0x01))



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
	bool have_read_adc;
} _registers_t;



static _registers_t _regs = {
	.have_read_adc = true,
};



static void _adc_cb(uint16_t val){
	_regs.adc_data = val;
	_regs.adc_status = EX_PROTOCOL_STATUS_READY;
	// printf("ADC: %u\r\n", val);
}



static void _write_io_reg(uint8_t reg, uint8_t port_value){
	for(int bit = 0; bit < 8; bit++){
		const int pin = bit + (reg == EX_PROTOCOL_REG_IODIR_A ? 0 : 8);
		//only if they differ from current mode
		// if(!_CMP_BITS(reg == EX_PROTOCOL_REG_IODIR_A ? _regs.io_a : _regs.io_b, port_value, bit)){
			// // if this bit is ADC then skip it
			// if(ex_gpio_is_adc(reg == EX_PROTOCOL_REG_IODIR_A ? EX_GPIO_PORT_0 : EX_GPIO_PORT_1, bit)){
			// 	continue;
			// }

			// only if not forced by adc state
			const int adc_idx = ex_gpio_map_to_adc(pin);
			if(adc_idx != EX_GPIO_ILLEGAL_PIN && _BIT_VALUE(_regs.adc_io, adc_idx)){
				//skip
				continue;
			}
			// ex_adc_map_to_gpio()
			ex_gpio_set_mode(pin, _BIT_VALUE(port_value, bit));
		// }
	}

	if(reg == EX_PROTOCOL_REG_IODIR_A){
		_regs.io_a = port_value;
	}else{
		_regs.io_b = port_value;
	}
}

static void _write_gpio_reg(uint8_t reg, uint8_t port_value){
	for(int pin = 0; pin < 8; pin++){
		//only if configured as output
		// if(_BIT_VALUE(reg == EX_PROTOCOL_REG_GPIO_A ? _regs.io_a : _regs.io_b, pin)){
			// if this pin is ADC then skip it
			// if(ex_gpio_is_adc(reg == EX_PROTOCOL_REG_IODIR_A ? EX_GPIO_PORT_0 : EX_GPIO_PORT_1, pin)){
			// 	continue;
			// }
			ex_gpio_output_pin_set(pin + (reg == EX_PROTOCOL_REG_GPIO_A ? 0 : 8), _BIT_VALUE(port_value, pin));
		// }
	}

	if(reg == EX_PROTOCOL_REG_GPIO_A){
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
			ex_adc_enable_pin(pin);
			// }else{
				//TODO: configure from current settings
				//need map from adc pin to gpio pin
			// }
		}
	}

	_regs.adc_io = port_value;
}

static void _trigger_adc_call(int pin)
{
	//only if the pin is ADC, and adc was read before
	if(_regs.have_read_adc && _BIT_VALUE(_regs.adc_io, pin)){
		_regs.adc_status = EX_PROTOCOL_STATUS_BUSY;
		_regs.have_read_adc = false;
		ex_adc_read_irq(pin, &_adc_cb);
	}
}



void ex_core_write(uint8_t reg, uint8_t value)
{
	switch(reg)
	{
		case EX_PROTOCOL_REG_IODIR_A:
		case EX_PROTOCOL_REG_IODIR_B:
			_write_io_reg(reg, value);
			break;

		case EX_PROTOCOL_REG_GPIO_A:
		case EX_PROTOCOL_REG_GPIO_B:
			_write_gpio_reg(reg, value);
			break;
		
		case EX_PROTOCOL_REG_ADC_CFG:
			_write_adc_cfg_reg(value);
			break;
		
		case EX_PROTOCOL_REG_ADC_START:
			_trigger_adc_call(value);
			break;
	}
}

void ex_core_set_read_reg(uint8_t reg)
{
	_regs.read_reg_val = reg;
}

uint8_t ex_core_read()
{
	switch(_regs.read_reg_val)
	{
		case EX_PROTOCOL_REG_IODIR_A:
			return _regs.io_a;
		case EX_PROTOCOL_REG_IODIR_B:
			return _regs.io_b;

		case EX_PROTOCOL_REG_GPIO_A:
			return ex_gpio_input_read(EX_GPIO_PORT_0);

		case EX_PROTOCOL_REG_GPIO_B:
			return ex_gpio_input_read(EX_GPIO_PORT_1);
		
		case EX_PROTOCOL_REG_ADC_VAL_H:
			_regs.have_read_adc = true;
			return (_regs.adc_data >> 8) & 0xFF;
		
		case EX_PROTOCOL_REG_ADC_VAL_L:
			return _regs.adc_data & 0xFF;

		case EX_PROTOCOL_REG_ADC_CFG:
			return _regs.adc_io;
		
		case EX_PROTOCOL_REG_ADC_STATUS:
			return _regs.adc_status;
		
		default:
			return 0;
	}
}