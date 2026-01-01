#include "debug.h"

#include "ex_adc.h"
#include "ex_core.h"
#include "ex_gpio.h"
#include "ex_i2c.h"



/* Global define */
#define RXAdderss   0x40
#define TXAdderss   0x40




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
	exp_adc_init();
	ex_i2c_slave_init(100000, RXAdderss);
	// IIC_Init(100000, RXAdderss << 1);

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	// ex_core_write(0x00, 0x0C);
	// ex_core_write(0x12, 0x0C);

	// ex_core_set_read_reg(0x00);
	// printf("0x0C == 0x%X\r\n", ex_core_read());
	// ex_core_set_read_reg(0x12);
	// printf("0x0C == 0x%X\r\n", ex_core_read());

	// ex_core_write(0x00, 0x04);
	// ex_core_write(0x12, 0x04);

	// ex_core_set_read_reg(0x00);
	// printf("0x04 == 0x%X\r\n", ex_core_read());
	// ex_core_set_read_reg(0x12);
	// printf("0x04 == 0x%X\r\n", ex_core_read());

	// ex_core_write(0x30, 0x60);
	// ex_core_set_read_reg(0x30);
	// printf("0x60 == 0x%X\r\n", ex_core_read());
	
	// ex_core_set_read_reg(0x2A);
	// ex_core_set_read_reg(0x32);
	// while(ex_core_read() == 0x01){
	// 	printf(".");
	// };
	// printf("\r\n");
	// ex_core_set_read_reg(0x2A);
	// uint16_t val = ex_core_read() << 8;
	// ex_core_set_read_reg(0x2B);
	// val += ex_core_read();

	// printf("val = %d\r\n", val);


	int nop_reads = 0;
	uint8_t reg = 0;
	bool is_second_read = false;
    while(1)
    {
		const ex_i2c_event_t event =  ex_i2c_pull();
		if(event == EX_I2C_EVENT_NONE){
			nop_reads++;
			continue;
		}

		printf("NONE = %d\r\n", nop_reads);
		nop_reads = 0;

		if(event == EX_I2C_EVENT_WRITE_REQ){
			//is this second read or not?
			if(is_second_read){
				is_second_read = false;
				const uint8_t data = ex_i2c_slave_read();
				ex_core_write(reg, data);

				printf("READ_2 = %d\r\n", data);
			}else{
				reg = ex_i2c_slave_read();
				is_second_read = true;

				printf("READ_1 = %d\r\n", reg);
			}
		}else if (event == EX_I2C_EVENT_READ_REQ){
			is_second_read = false;
			const uint8_t data = ex_core_read();
			ex_i2c_slave_write(data, true);
			printf("WRITE = %d\r\n", data);
		}

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
				ex_adc_enable_pin(pin % EX_ADC_MAX_PIN);
				printf("Enabled [%d] as ADC input: %d\r\n", pin % EX_ADC_MAX_PIN, ex_adc_read(pin % EX_ADC_MAX_PIN));
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
