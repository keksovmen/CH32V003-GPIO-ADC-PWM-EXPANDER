#include "ex_gpio.h"


#include "debug.h"
#include "ch32v00x.h"
#include "ch32v00x_gpio.h"
#include "ch32v00x_adc.h"



static GPIO_TypeDef* _TABLE_IDX_TO_PORT[EX_GPIO_MAX_PIN] = {
	GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD,
	GPIOA, GPIOA, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC
};

static const int _TABLE_IDX_TO_PIN[EX_GPIO_MAX_PIN] = {
	GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7,
	GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_1, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7
};



static GPIO_TypeDef* _TABLE_ADC_IDX_TO_PORT[EX_GPIO_ADC_MAX_PIN] = {
	GPIOD, GPIOD, GPIOD, GPIOA, GPIOA, GPIOD, GPIOD, GPIOC,
};

static const int _TABLE_ADC_IDX_TO_PIN[EX_GPIO_ADC_MAX_PIN] = {
	GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4,
};

static const uint8_t _TABLE_ADC_IDX_TO_CHANNEL[EX_GPIO_ADC_MAX_PIN] = {
	ADC_Channel_7, ADC_Channel_5, ADC_Channel_6, ADC_Channel_1,
	ADC_Channel_0, ADC_Channel_3, ADC_Channel_4, ADC_Channel_2
};




static int _TABLE_IDX_TO_ADC_IDX[EX_GPIO_MAX_PIN] = {
	EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN, 5, 6, 0, 1, 2, EX_GPIO_ILLEGAL_PIN,
	3, 4, EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN, 7, EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN, EX_GPIO_ILLEGAL_PIN
};

static int _TABLE_ADC_IDX_TO_IDX[EX_GPIO_ADC_MAX_PIN] = {
	4, 5, 6, 8, 9, 2, 3, 12
};



static ex_gpio_adc_cb_t _adc_val_cb = NULL;




static GPIO_TypeDef* _map_to_gpio_port(int pin)
{
	return _TABLE_IDX_TO_PORT[pin];
}

static int _map_to_gpio_pin(int pin)
{
	return _TABLE_IDX_TO_PIN[pin];
}

static GPIO_TypeDef* _map_adc_pin_to_gpio_port(int pin)
{
	return _TABLE_ADC_IDX_TO_PORT[pin];
}

static int _map_adc_pin_to_gpio_pin(int pin)
{
	return _TABLE_ADC_IDX_TO_PIN[pin];
}

static int _map_adc_pin_to_channel(int pin)
{
	return _TABLE_ADC_IDX_TO_CHANNEL[pin];
}



void ex_gpio_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	// for(ex_gpio_port_t port = 0; port < EX_GPIO_PORT_MAX; port++)
	// {
		for(int pin = 0; pin < EX_GPIO_MAX_PIN; pin++)
		{
			ex_gpio_set_mode(pin, false);
		}
	// }
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



static void __attribute__((interrupt("WCH-Interrupt-fast"))) ADC1_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC))
    {
		if(_adc_val_cb != NULL){
			_adc_val_cb(ADC_GetConversionValue(ADC1));
		}
    }

    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
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

    NVIC_InitTypeDef irq_cfg = {
		.NVIC_IRQChannel = ADC_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 0,
		.NVIC_IRQChannelSubPriority = 1,
		.NVIC_IRQChannelCmd = ENABLE,
	};
    NVIC_Init(&irq_cfg);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
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

	return ADC_GetConversionValue(ADC1);
}

void ex_gpio_adc_read_irq(int pin, ex_gpio_adc_cb_t cb)
{
	_adc_val_cb = cb;

	if(pin == 1 || pin == 2){
		// illegal SWIO and UTX GPIO for now only
		printf("Illegal pin: [%d]\r\n", pin);
		return;
	}

	ADC_RegularChannelConfig(ADC1, _map_adc_pin_to_channel(pin), 1, ADC_SampleTime_241Cycles);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

int ex_gpio_adc_map_to_gpio(int pin)
{
	return _TABLE_ADC_IDX_TO_IDX[pin];
}

// bool ex_gpio_is_adc(ex_gpio_port_t port, int pin)
// {
// 	uint32_t state = _map_to_gpio_port(port, pin)->CFGLR;
// 	int pin = _map_to_gpio_pin(port, pin);
// 	uint32_t shift_count = 2;
// 	for(int bit = 0; bit < 8; bit++){
// 		if(pin & (1 << bit)){
// 			shift_count += bit * 4;
// 			break;
// 		}
// 	}

// 	//check if in INPUT mode and mode is ADC
// 	return ((state >> (shift_count - 2)) & 0x03) == 0 &&
// 		((state >> shift_count) & 0x03) == 0;

// }