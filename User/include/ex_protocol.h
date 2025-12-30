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
 * @brief ADC custom register
 * Algorithm to read ADC values:
 * - send write request (I2C write with corresponding _ADC_DATA_X_H register as value) to initiate ADC calculation
 * - now reed @def EX_PROTOCOL_REG_ADC_STATUS (I2C read = I2C write with register as value then read request)
 * 		if you get @ref EX_PROTOCOL_STATUS_BUSY then result is not ready, it is ready when @ref EX_PROTOCOL_STATUS_BUSY
 * - now read (I2C read -> write with arg and then read)
 * 		_ADC_DATA_X_H register and then _ADC_DATA_X_L register,
 * 		H is higher 8 bits and L is lower: uint16_t val = (H << 8) + L
 * - or you just can wait some time say 1ms (TODO: calculate 241 cycle at 24MHZ) and then read again
 */
#define EX_PROTOCOL_REG_ADC_DATA_0_H 0x20
#define EX_PROTOCOL_REG_ADC_DATA_0_L 0x21
#define EX_PROTOCOL_REG_ADC_DATA_1_H 0x22
#define EX_PROTOCOL_REG_ADC_DATA_1_L 0x23
#define EX_PROTOCOL_REG_ADC_DATA_2_H 0x24
#define EX_PROTOCOL_REG_ADC_DATA_2_L 0x25
#define EX_PROTOCOL_REG_ADC_DATA_3_H 0x26
#define EX_PROTOCOL_REG_ADC_DATA_3_L 0x27
#define EX_PROTOCOL_REG_ADC_DATA_4_H 0x28
#define EX_PROTOCOL_REG_ADC_DATA_4_L 0x29
#define EX_PROTOCOL_REG_ADC_DATA_5_H 0x2A
#define EX_PROTOCOL_REG_ADC_DATA_5_L 0x2B
#define EX_PROTOCOL_REG_ADC_DATA_6_H 0x2C
#define EX_PROTOCOL_REG_ADC_DATA_6_L 0x2D
#define EX_PROTOCOL_REG_ADC_DATA_7_H 0x2E
#define EX_PROTOCOL_REG_ADC_DATA_7_L 0x2F

/**
 * @brief Works as IODIR register but 1 overrides the IODIR reg value and sets pin to ADC mode
 * writing 0 to previously 1, doesn't change anything
 * only drops the protection for further IODIR configuration
 */
#define EX_PROTOCOL_REG_ADC_CFG 0x30
/**
 * @brief Used to get ADC measurement state
 * 
 */
#define EX_PROTOCOL_REG_ADC_STATUS 0x31