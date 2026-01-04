#pragma once



#define EX_PROTOCOL_STATUS_READY 0x00
#define EX_PROTOCOL_STATUS_BUSY 0x01

/**
 * @brief same as MCP23017, 0 set pin as input, 1 as output,
 * works for pins [0; 7]
 */
#define EX_PROTOCOL_REG_IODIR_A 0x00

/**
 * @brief same as MCP23017, 0 set pin as input, 1 as output,
 * works for pins [8; 15]
 */
#define EX_PROTOCOL_REG_IODIR_B 0x01

/**
 * @brief same as MCP23017, if input you read value, if output you set logic level 1 H, 0 L
 * works for pins [0; 7]
 */
#define EX_PROTOCOL_REG_GPIO_A 0x12

/**
 * @brief same as MCP23017, if input you read value, if output you set logic level 1 H, 0 L
 * works for pins [8; 15]
 */
#define EX_PROTOCOL_REG_GPIO_B 0x13



/**
 * @brief ADC custom registers
 * Algorithm to read ADC values:
 * - write to reg @ref EX_PROTOCOL_REG_ADC_START (where value is pin to measure) to initiate ADC calculation
 * - now reed @ref EX_PROTOCOL_REG_ADC_STATUS (I2C read = I2C write with register as value then read request)
 * 		if you get @ref EX_PROTOCOL_STATUS_BUSY then result is not ready, it is ready when @ref EX_PROTOCOL_STATUS_BUSY
 * - now read (I2C read -> write with arg and then read)
 * 		@ref EX_PROTOCOL_REG_ADC_VAL_H register and then @ref EX_PROTOCOL_REG_ADC_VAL_L register,
 * 		H is higher 8 bits and L is lower: uint16_t val = (H << 8) + L
 * - or you just can wait some time say 1ms (TODO: calculate 241 cycle at 24MHZ) and then read again
 */

/**
 * @brief Works as IODIR register but 1 overrides the IODIR reg value and sets pin to ADC mode
 * writing 0 to previously 1, doesn't change anything
 * only drops the protection for further IODIR configuration
 */
#define EX_PROTOCOL_REG_ADC_CFG 0x20

/**
 * @brief Used to get ADC measurement state
 * @ref EX_PROTOCOL_STATUS_READY
 * @ref EX_PROTOCOL_STATUS_BUSY
 */
#define EX_PROTOCOL_REG_ADC_STATUS 0x21

/**
 * @brief high 8 bits of value
 */
#define EX_PROTOCOL_REG_ADC_VAL_H 0x22

/**
 * @brief low 8 bits of value
 */
#define EX_PROTOCOL_REG_ADC_VAL_L 0x23

/**
 * @brief Write to this register desired pin to measure ADC,
 * if pin is enabled as ADC then measurement will start
 * 
 * Data is: pin [0; 7], 
 * 
 */
#define EX_PROTOCOL_REG_ADC_START 0x24