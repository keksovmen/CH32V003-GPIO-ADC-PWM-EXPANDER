#include "debug.h"

#include "ex_gpio.h"



/* Global define */
#define RXAdderss   0x20
#define TXAdderss   0x20




void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2C_InitTypeDef I2C_InitTSturcture={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture );

    I2C_Cmd( I2C1, ENABLE );
}

// static void _gpio_config()
// {
// 	GPIO_InitTypeDef GPIO_InitStructure = {0};

//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//     GPIO_Init(GPIOD, &GPIO_InitStructure);

// 	GPIO_Write(GPIOD, 0);
// }

// static void _set_port(uint8_t state)
// {
// 	GPIO_Write(GPIOD, state);
// }
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
	ex_gpio_init();
	exp_gpio_adc_init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
	IIC_Init(100000, RXAdderss << 1);

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	// GPIO_InitTypeDef cfg = {
	// 	.GPIO_Pin = 0,
	// 	.GPIO_Speed = GPIO_Speed_30MHz,
	// 	.GPIO_Mode = GPIO_Mode_Out_PP,
	// };
    // GPIO_Init(GPIOD, &cfg);
	// GPIO_Write(GPIOD, 1);
	
	// Delay_Ms(50);
	// ex_gpio_set_mode_adc(3);
	// GPIO_InitTypeDef cfg = {
	// 	.GPIO_Pin = GPIO_Pin_2,
	// 	.GPIO_Speed = 0,
	// 	.GPIO_Mode = GPIO_Mode_AIN,
	// };

	// GPIO_Init(GPIOD, &cfg);

	// ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_241Cycles);
	// ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	// ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	// while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	// const uint16_t result = ADC_GetConversionValue(ADC1);

	// ADC_SoftwareStartConvCmd(ADC1, DISABLE);

	// printf("ADC: %d\r\n", result);



	// int count = 0;

    while(1)
    {
		// I2C_SoftwareResetCmd(I2C1, ENABLE);
        // I2C_SoftwareResetCmd(I2C1, DISABLE);

		//working loop do not break it!!!
		/*
		while( !I2C_CheckEvent( I2C1, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED ) ){
			if (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))
			{
				I2C_AcknowledgeConfig(I2C1, ENABLE);
				I2C_ClearFlag(I2C1, I2C_FLAG_STOPF);
			}

			if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR))
			{
				I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
			}

			if (I2C_GetFlagStatus(I2C1, I2C_FLAG_ARLO))
			{
				I2C_ClearFlag(I2C1, I2C_FLAG_ARLO);
			}

			if (I2C_GetFlagStatus(I2C1, I2C_FLAG_OVR))
			{
				I2C_ClearFlag(I2C1, I2C_FLAG_OVR);
			}
		}

		I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR);

		uint8_t data[2] = {0};
		int i = 0;
		while(i < 2){
			if(I2C_GetFlagStatus( I2C1, I2C_FLAG_RXNE ) !=  RESET){
				data[i] = I2C_ReceiveData( I2C1 );
				i++;
			}
		}

		

		_set_port(data[0]);
		*/
		printf( "Entered main cycle:\r\n");
		for(ex_gpio_port_t port = 0; port < EX_GPIO_PORT_MAX; port++)
		{
			for(int pin = 0; pin < 8; pin++)
			{
				printf("Waiting for UART msg...\r\n");
				while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == 0){

				}

				// 0 = out, 1 = in, 2 = analog 
				char data = USART_ReceiveData(USART1);
				printf("Read: %c\r\n", data);
				if(data == 'i'){
					ex_gpio_set_mode(port, pin, false);
					printf("Enabled %d[%d] as input: %d\r\n", port, pin, ex_gpio_input_pin_read(port, pin));
				}else if(data == 'a'){
					ex_gpio_set_mode_adc(pin);
					printf("Enabled [%d] as ADC input: %d\r\n", pin, ex_gpio_adc_read(pin));
				}else{
					printf("Enabling %d[%d] as output high:\r\n", port, pin);
					ex_gpio_output_pin_set(port, pin, true);
				}

			}
		}

		printf( "Finished main cycle:\r\n");
		// Delay_Ms(1000);

		// for(int i = 0; i < sizeof(data); i++){
		// 	printf("Rx: 0x%X\r\n", data[i]);
		// }

		// printf("Count: %d\r\n", count++);
    }
}
