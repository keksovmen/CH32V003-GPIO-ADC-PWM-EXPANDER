#include "ex_i2c.h"

#include <stdbool.h>

#include "ch32v00x_i2c.h"



void ex_i2c_slave_init(int clock_hz, uint8_t address)
{
	GPIO_InitTypeDef gpio_cfg ={0};
    I2C_InitTypeDef i2c_cfg ={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );

    gpio_cfg .GPIO_Pin = GPIO_Pin_2;
    gpio_cfg .GPIO_Mode = GPIO_Mode_AF_OD;
    gpio_cfg .GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &gpio_cfg );

    gpio_cfg .GPIO_Pin = GPIO_Pin_1;
    gpio_cfg .GPIO_Mode = GPIO_Mode_AF_OD;
    gpio_cfg .GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &gpio_cfg );

    i2c_cfg .I2C_ClockSpeed = clock_hz;
    i2c_cfg .I2C_Mode = I2C_Mode_I2C;
    i2c_cfg .I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_cfg .I2C_OwnAddress1 = address;
    i2c_cfg .I2C_Ack = I2C_Ack_Enable;
    i2c_cfg .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &i2c_cfg );

    I2C_Cmd(I2C1, ENABLE);
}

ex_i2c_event_t ex_i2c_pull()
{
	//it reads both status registers, so if you need to send data you must hurry to put it there
	const uint32_t last_event = I2C_GetLastEvent(I2C1);
	// const bool address_match = I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == SET;
	if((last_event & I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED) == I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED || 
		(last_event & I2C_EVENT_SLAVE_BYTE_TRANSMITTED) == I2C_EVENT_SLAVE_BYTE_TRANSMITTED)
	{
		//receiver mode, or data is ready to be written
		return EX_I2C_EVENT_WRITE_REQ;

	}else if((last_event & I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED) == I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED || 
		(last_event & I2C_EVENT_SLAVE_BYTE_RECEIVED) == I2C_EVENT_SLAVE_BYTE_RECEIVED)
	{
		//transmitter mode, or data is ready to be read
		return EX_I2C_EVENT_READ_REQ;

	}else{
		//nothing, just exit, and fix errors
		//when in slave transceiver mode, and master didn't send ACK on received byte
		if((last_event & I2C_EVENT_SLAVE_ACK_FAILURE) == I2C_EVENT_SLAVE_ACK_FAILURE){
			// need to clear AF flag by writing 0 to SR1 register
			I2C_ClearFlag(I2C1, I2C_FLAG_AF);
		}

		// slave read STOP bit
		if((last_event & I2C_EVENT_SLAVE_STOP_DETECTED) == I2C_EVENT_SLAVE_STOP_DETECTED){
			//read and then write to CTRL1 register to clear the flag
			((void)I2C1->STAR1);
			I2C_Cmd(I2C1, ENABLE);
		}

		//when in slave transceiver mode, and master did some unexpected start stop addr sequences
		// use mask 0x00FFFFFF to remove garbage from actual flag
		uint32_t bit_flag = I2C_FLAG_BERR & 0x00FFFFFF;
		if((last_event & bit_flag) == bit_flag){
			I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
		}

		return EX_I2C_EVENT_NONE;
	}
}

void ex_i2c_slave_write(uint8_t data, bool generate_stop)
{
	//must read SR1 register to clear bit BTF if such exists
	((void) I2C_ReadRegister(I2C1, I2C_Register_STAR1));
	I2C_SendData(I2C1, data);
	if(generate_stop){
		I2C_GenerateSTOP(I2C1, SET);
	}
	//wait for data send finish?
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == RESET)
	{
		//wait for transfer finished
	}
}

uint8_t ex_i2c_slave_read()
{
	return I2C_ReceiveData(I2C1);
}
