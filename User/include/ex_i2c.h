#pragma once



#include <stdbool.h>
#include <stdint.h>



typedef enum
{
	EX_I2C_EVENT_NONE,		//nothing happened
	EX_I2C_EVENT_WRITE_REQ,	//slave read, master send
	EX_I2C_EVENT_READ_REQ,	//slave write, master read
} ex_i2c_event_t;



void ex_i2c_slave_init(int clock_hz, uint8_t address);

ex_i2c_event_t ex_i2c_pull();

void ex_i2c_slave_write(uint8_t data, bool generate_stop);
uint8_t ex_i2c_slave_read();
