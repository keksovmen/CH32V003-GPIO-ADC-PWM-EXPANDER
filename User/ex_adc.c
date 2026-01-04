#include "ex_adc.h"

#include "debug.h"
#include "ch32v00x_adc.h"



static GPIO_TypeDef* const _TABLE_ADC_IDX_TO_PORT[EX_ADC_MAX_PIN] = {
	GPIOD, GPIOD, GPIOD, GPIOA, GPIOA, GPIOD, GPIOD, GPIOC,
};

static const int _TABLE_ADC_IDX_TO_PIN[EX_ADC_MAX_PIN] = {
	GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4,
};

static const uint8_t _TABLE_ADC_IDX_TO_CHANNEL[EX_ADC_MAX_PIN] = {
	ADC_Channel_7, ADC_Channel_5, ADC_Channel_6, ADC_Channel_1,
	ADC_Channel_0, ADC_Channel_3, ADC_Channel_4, ADC_Channel_2
};

static const int _TABLE_ADC_IDX_TO_GPIO_IDX[EX_ADC_MAX_PIN] = {
	4, 5, 6, 8, 9, 2, 3, 12
};



static ex_adc_cb_t _adc_val_cb = NULL;






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



//interrupts can't be static due to weak references and we need to override it
void __attribute__((interrupt("WCH-Interrupt-fast"))) ADC1_IRQHandler(void)
{
	if(ADC_GetITStatus(ADC1, ADC_IT_EOC))
	{
		if(_adc_val_cb != NULL){
			_adc_val_cb(ADC_GetConversionValue(ADC1));
		}
	}

	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}



void exp_adc_init()
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

void ex_adc_enable_pin(int pin)
{
	//UART pins for debug
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

uint16_t ex_adc_read(int pin)
{
	//UART pins for debug
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

void ex_adc_read_irq(int pin, ex_adc_cb_t cb)
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

int ex_adc_map_to_gpio(int pin)
{
	return _TABLE_ADC_IDX_TO_GPIO_IDX[pin];
}