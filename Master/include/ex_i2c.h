#pragma once



#include <stdbool.h>
#include <stdint.h>



/**
 * @brief write HAL callback
 * Implementer must write I2C WRITE request
 * 
 * @par address is I2C address
 * @par data is pointer to data to be send
 * @par length is data length, amount of bytes to send
 * 
 * @return true if succeed, false otherwise
 */
typedef bool (*ex_i2c_write_ft)(int32_t address, uint8_t* data, int32_t length);

/**
 * @brief read HAL callback
 * Implementer must write I2C READ request
 * 
 * @par address is I2C address
 * @par data is pointer to where put data
 * @par length is data length, amount of bytes to receive
 * 
 * @return true if succeed, false otherwise
 */
typedef bool (*ex_i2c_read_ft)(int32_t address, uint8_t* data, int32_t length);



typedef struct
{
	ex_i2c_write_ft write_cb;
	ex_i2c_read_ft read_cb;
} ex_i2c_t;