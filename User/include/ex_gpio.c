#include "ex_gpio.h"


#include "debug.h"
#include "ch32v00x.h"
#include "ch32v00x_gpio.h"
#include "ch32v00x_adc.h"



static GPIO_TypeDef* _map_to_gpio_port(ex_gpio_port_t port, int pin)
{
	if(port == EX_GPIO_PORT_0){
		return GPIOD;
	}

	if(pin == 0 || pin == 1){
		return GPIOA;
	}

	return GPIOC;
}

static int _map_to_gpio_pin(ex_gpio_port_t port, int pin)
{
	//GPIOD
	if(port == EX_GPIO_PORT_0){
		return 1 << pin;
	}

	switch (pin)
	{
		//GPIOA
		case 0: case 1:
			return 1 << (pin + 1);
		
		//GPIOC
		case 2:
			return 1;
		
		default:
			return 1 << pin;
	}
}

static GPIO_TypeDef* _map_adc_pin_to_gpio_port(int pin)
{
	switch(pin)
	{
		case 0: case 1: case 2: case 5: case 6:
			return GPIOD;
		
		case 3: case 4:
			return GPIOA;

		default:		
			return GPIOC;
	}
}

static int _map_adc_pin_to_gpio_pin(int pin)
{
	switch (pin)
	{
		//GPIOD
		case 0: case 1: case 2:
			return 1 << (pin + 4);
		
		//GPIOA
		case 3: case 4:
			return 1 << (pin - 2);
		
		//GPIOD
		case 5: case 6:
			return 1 << (pin - 3);
		
		//GPIOC
		default:
			return 1 << (pin - 3);
	}
}

static int _map_adc_pin_to_channel(int pin)
{
	switch (pin)
	{
		//GPIOD
		case 0:
			return ADC_Channel_7;
		
		case 1:
			return ADC_Channel_5;

		case 2:
			return ADC_Channel_6;
		
		case 3:
			return ADC_Channel_1;

		case 4:
			return ADC_Channel_0;
		
		case 5:
			return ADC_Channel_3;
		
		case 6:
			return ADC_Channel_4;
		
		case 7:
			 return ADC_Channel_2;
		//GPIOC
		default:
			return ADC_Channel_2;
	}
}



void ex_gpio_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	for(ex_gpio_port_t port = 0; port < EX_GPIO_PORT_MAX; port++)
	{
		for(int pin = 0; pin < 8; pin++)
		{
			ex_gpio_set_mode(port, pin, false);
		}
	}
}

void ex_gpio_set_mode(ex_gpio_port_t port, int pin, bool is_output)
{
	if(port == EX_GPIO_PORT_0 && (pin == 1 || pin == 5 || pin == 6)){
		// illegal SWIO and UTX GPIO for now only
		printf("Illegal port and pin: %d[%d]\r\n", port, pin);
		return;
	}

	GPIO_InitTypeDef cfg = {
		.GPIO_Pin = _map_to_gpio_pin(port, pin),
		.GPIO_Speed = GPIO_Speed_30MHz,
		.GPIO_Mode = is_output ? GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING,
	};

	
	printf("Set as out: %s[0x%X]\r\n", _map_to_gpio_port(port, pin) == GPIOD ? "GPIOD" : (_map_to_gpio_port(port, pin) == GPIOC ? "GPIOC" : "GPIOA"), _map_to_gpio_pin(port, pin));
	GPIO_Init(_map_to_gpio_port(port, pin), &cfg);
}

void ex_gpio_output_pin_set(ex_gpio_port_t port, int pin, bool state)
{
	if(port == EX_GPIO_PORT_0 && (pin == 1 || pin == 5 || pin == 6)){
		// illegal SWIO and UTX GPIO for now only
		printf("Illegal port and pin: %d[%d]\r\n", port, pin);
		return;
	}
	
	GPIO_WriteBit(_map_to_gpio_port(port, pin), _map_to_gpio_pin(port, pin), state ? Bit_SET : Bit_RESET);
}

uint8_t ex_gpio_input_pin_read(ex_gpio_port_t port, int pin)
{
	return GPIO_ReadInputDataBit(_map_to_gpio_port(port, pin), _map_to_gpio_pin(port, pin));
}

uint8_t ex_gpio_input_read(ex_gpio_port_t port)
{
	uint8_t result = 0;
	for(int pin = 0; pin < 8; pin++){
		result += ex_gpio_input_pin_read(port, pin) << pin;
	}

	return result;
}



void exp_gpio_adc_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	ADC_InitTypeDef cfg = {
		.ADC_Mode = ADC_Mode_Independent,
		.ADC_ScanConvMode = DISABLE,
		.ADC_ContinuousConvMode = DISABLE,
		.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
		.ADC_DataAlign = ADC_DataAlign_Right,
		.ADC_NbrOfChannel = 1,
	};
	ADC_Init(ADC1, &cfg);
	ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

void ex_gpio_set_mode_adc(int pin)
{
	if(pin == 1 || pin == 2){
		// illegal SWIO and UTX GPIO for now only
		printf("Illegal pin: [%d]\r\n", pin);
		return;
	}

	GPIO_InitTypeDef cfg = {
		.GPIO_Pin = _map_adc_pin_to_gpio_pin(pin),
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_Mode = GPIO_Mode_AIN,
	};

	GPIO_Init(_map_adc_pin_to_gpio_port(pin), &cfg);
}

uint16_t ex_gpio_adc_read(int pin)
{
	if(pin == 1 || pin == 2){
		// illegal SWIO and UTX GPIO for now only
		printf("Illegal pin: [%d]\r\n", pin);
		return -1;
	}

	ADC_RegularChannelConfig(ADC1, _map_adc_pin_to_channel(pin), 1, ADC_SampleTime_241Cycles);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	const uint16_t result = ADC_GetConversionValue(ADC1);

	ADC_SoftwareStartConvCmd(ADC1, DISABLE);

	return result;
}