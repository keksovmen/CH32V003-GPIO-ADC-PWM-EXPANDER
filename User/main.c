#include "debug.h"

#include "ex_gpio.h"
#include "ex_core.h"



/* Global define */
#define RXAdderss   0x20
#define TXAdderss   0x20




static void IIC_Init(u32 bound, u16 address)
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
    USART_Printf_Init(115200);
	ex_gpio_init();
	exp_gpio_adc_init();
	// IIC_Init(100000, RXAdderss << 1);

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	ex_core_write(0x00, 0x0C);
	ex_core_write(0x12, 0x0C);

	ex_core_set_read_reg(0x00);
	printf("0x0C == 0x%X\r\n", ex_core_read());
	ex_core_set_read_reg(0x12);
	printf("0x0C == 0x%X\r\n", ex_core_read());

	ex_core_write(0x00, 0x04);
	ex_core_write(0x12, 0x04);

	ex_core_set_read_reg(0x00);
	printf("0x04 == 0x%X\r\n", ex_core_read());
	ex_core_set_read_reg(0x12);
	printf("0x04 == 0x%X\r\n", ex_core_read());

	ex_core_write(0x30, 0x60);
	ex_core_set_read_reg(0x30);
	printf("0x60 == 0x%X\r\n", ex_core_read());
	
	ex_core_set_read_reg(0x2A);
	ex_core_set_read_reg(0x32);
	while(ex_core_read() == 0x01){
		printf(".");
	};
	printf("\r\n");
	ex_core_set_read_reg(0x2A);
	uint16_t val = ex_core_read() << 8;
	ex_core_set_read_reg(0x2B);
	val += ex_core_read();

	printf("val = %d\r\n", val);


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
		*/

		

		/*
		printf( "Entered main cycle:\r\n");
		for(int pin = 0; pin < EX_GPIO_MAX_PIN; pin++)
		{
			printf("Waiting for UART msg...\r\n");
			while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == 0){

			}

			// 0 = out, 1 = in, 2 = analog 
			char data = USART_ReceiveData(USART1);
			printf("Read: %c\r\n", data);
			if(data == 'i'){
				ex_gpio_set_mode(pin, false);
				printf("Enabled %d as input: %d\r\n", pin, ex_gpio_input_pin_read(pin));
			}else if(data == 'a'){
				ex_gpio_set_mode_adc(pin % EX_GPIO_ADC_MAX_PIN);
				printf("Enabled [%d] as ADC input: %d\r\n", pin % EX_GPIO_ADC_MAX_PIN, ex_gpio_adc_read(pin % EX_GPIO_ADC_MAX_PIN));
			}else{
				printf("Enabling [%d] as output high:\r\n", pin);
				ex_gpio_set_mode(pin, true);
				ex_gpio_output_pin_set(pin, true);
			}

		}

		printf( "Finished main cycle:\r\n");
		*/


		

    }
}
