#include "ex_gpio.h"


#include "debug.h"
#include "ch32v00x.h"
#include "ch32v00x_gpio.h"



static GPIO_TypeDef* const _TABLE_IDX_TO_PORT[EX_GPIO_MAX_PIN] = {
	GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD,
	GPIOA, GPIOA, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC
};

static const int _TABLE_IDX_TO_PIN[EX_GPIO_MAX_PIN] = {
	GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7,
	GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_1, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7
};

static const int _TABLE_IDX_TO_ADC_IDX[EX_GPIO_MAX_PIN] = {
	EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN, 5, 6, 0, 1, 2, EX_GPIO_ILLEGAL_PIN,
	3, 4, EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN, 7, EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN
};



static GPIO_TypeDef* _map_to_gpio_port(int pin)
{
	return _TABLE_IDX_TO_PORT[pin];
}

static int _map_to_gpio_pin(int pin)
{
	return _TABLE_IDX_TO_PIN[pin];
}



void ex_gpio_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	for(int pin = 0; pin < EX_GPIO_MAX_PIN; pin++)
	{
		ex_gpio_set_mode(pin, false);
	}
}

void ex_gpio_set_mode(int pin, bool is_output)
{
	if(pin == 1 || pin == 5 || pin == 6){
		// illegal SWIO and UTX GPIO for now only
		printf("Illegal pin: %d\r\n", pin);
		return;
	}

	GPIO_InitTypeDef cfg = {
		.GPIO_Pin = _map_to_gpio_pin(pin),
		.GPIO_Speed = GPIO_Speed_30MHz,
		.GPIO_Mode = is_output ? GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING,
	};

	
	printf("Set as out: %s[0x%X]\r\n", _map_to_gpio_port(pin) == GPIOD ? "GPIOD" : (_map_to_gpio_port(pin) == GPIOC ? "GPIOC" : "GPIOA"), _map_to_gpio_pin(pin));
	GPIO_Init(_map_to_gpio_port(pin), &cfg);
}

void ex_gpio_output_pin_set(int pin, bool state)
{
	if(pin == 1 || pin == 5 || pin == 6){
		// illegal SWIO and UTX GPIO for now only
		printf("Illegal pin: %d\r\n", pin);
		return;
	}
	
	GPIO_WriteBit(_map_to_gpio_port(pin), _map_to_gpio_pin(pin), state ? Bit_SET : Bit_RESET);
}

uint8_t ex_gpio_input_pin_read(int pin)
{
	return GPIO_ReadInputDataBit(_map_to_gpio_port(pin), _map_to_gpio_pin(pin));
}

uint8_t ex_gpio_input_read(ex_gpio_port_t port)
{
	uint8_t result = 0;
	for(int pin = 0; pin < 8; pin++){
		result += ex_gpio_input_pin_read(pin + (port == EX_GPIO_PORT_0 ? 0 : 8)) << pin;
	}

	return result;
}

int ex_gpio_map_to_adc(int pin)
{
	return _TABLE_IDX_TO_ADC_IDX[pin];
}