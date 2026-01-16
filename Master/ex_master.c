#include "ex_master.h"

#include <stddef.h>
#include "ex_protocol.h"



// Helper macros for pin to port/pin conversion
#define PIN_TO_PORT(pin) ((pin) < 8 ? EX_MASTER_PORT_0 : EX_MASTER_PORT_1)
#define PIN_TO_MASK(pin) (1 << ((pin) % 8))
#define ADC_PIN_TO_MASK(pin) (1 << (pin))



// Private helper functions
static bool write_register(ex_master_t* master, uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    return master->i2c.write_cb(EX_PROTOCOL_DEFAULT_I2C_ADDRESS, buffer, 2);
}

static bool read_register(ex_master_t* master, uint8_t reg, uint8_t* value) {
    // Write register address
    if (!master->i2c.write_cb(EX_PROTOCOL_DEFAULT_I2C_ADDRESS, &reg, 1)) {
        return false;
    }
    // Read register value
    return master->i2c.read_cb(EX_PROTOCOL_DEFAULT_I2C_ADDRESS, value, 1);
}



bool ex_master_init(ex_master_t* master, ex_i2c_t* i2c) {
    if (master == NULL || i2c == NULL || 
        i2c->write_cb == NULL || i2c->read_cb == NULL) {
        return false;
    }
    
    master->i2c = *i2c;
    master->dirA = 0xFF;  // Default all inputs (MCP23017: 1=input, 0=output)
    master->dirB = 0xFF;
    master->outA = 0x00;  // Default low output
    master->outB = 0x00;
    master->adc = 0x00;   // Default ADC off
    
    // Initialize all registers
    bool success = true;
    success &= write_register(master, EX_PROTOCOL_REG_IODIR_A, master->dirA);
    success &= write_register(master, EX_PROTOCOL_REG_IODIR_B, master->dirB);
    success &= write_register(master, EX_PROTOCOL_REG_GPIO_A, master->outA);
    success &= write_register(master, EX_PROTOCOL_REG_GPIO_B, master->outB);
    success &= write_register(master, EX_PROTOCOL_REG_ADC_CFG, master->adc);
    
    return success;
}

bool ex_master_set_port_dir(ex_master_t* master, ex_master_port_t port, uint8_t val) {
    if (master == NULL) return false;
    
    uint8_t reg = (port == EX_MASTER_PORT_0) ? 
                  EX_PROTOCOL_REG_IODIR_A : EX_PROTOCOL_REG_IODIR_B;
    
    // Update cache
    if (port == EX_MASTER_PORT_0) {
        master->dirA = val;
    } else {
        master->dirB = val;
    }
    
    return write_register(master, reg, val);
}

bool ex_master_set_port_val(ex_master_t* master, ex_master_port_t port, uint8_t val) {
    if (master == NULL) return false;
    
    uint8_t reg = (port == EX_MASTER_PORT_0) ? 
                  EX_PROTOCOL_REG_GPIO_A : EX_PROTOCOL_REG_GPIO_B;
    
    // Update cache
    if (port == EX_MASTER_PORT_0) {
        master->outA = val;
    } else {
        master->outB = val;
    }
    
    return write_register(master, reg, val);
}

bool ex_master_set_pin_dir(ex_master_t* master, ex_master_pin_t pin, bool is_out) {
    if (master == NULL) return false;
    
    ex_master_port_t port = PIN_TO_PORT(pin);
    uint8_t mask = PIN_TO_MASK(pin);
    uint8_t current_dir;
    
    // Get current direction register value
    if (port == EX_MASTER_PORT_0) {
        current_dir = master->dirA;
    } else {
        current_dir = master->dirB;
    }
    
    // MCP23017: 1 = input, 0 = output
    if (is_out) {
        current_dir &= ~mask;  // Clear bit for output
    } else {
        current_dir |= mask;   // Set bit for input
    }
    
    return ex_master_set_port_dir(master, port, current_dir);
}

bool ex_master_set_pin_val(ex_master_t* master, ex_master_pin_t pin, bool state) {
    if (master == NULL) return false;
    
    ex_master_port_t port = PIN_TO_PORT(pin);
    uint8_t mask = PIN_TO_MASK(pin);
    uint8_t current_val;
    
    // Get current output value
    if (port == EX_MASTER_PORT_0) {
        current_val = master->outA;
    } else {
        current_val = master->outB;
    }
    
    if (state) {
        current_val |= mask;   // Set bit high
    } else {
        current_val &= ~mask;  // Clear bit low
    }
    
    return ex_master_set_port_val(master, port, current_val);
}

bool ex_master_set_port_adc_mode(ex_master_t* master, uint8_t val) {
    if (master == NULL) return false;
    
    master->adc = val;
    return write_register(master, EX_PROTOCOL_REG_ADC_CFG, val);
}

bool ex_master_set_pin_adc_mode(ex_master_t* master, ex_master_adc_pin_t pin, bool is_enable) {
    if (master == NULL || pin > EX_MASTER_ADC_PIN_7) return false;
    
    uint8_t mask = ADC_PIN_TO_MASK(pin);
    uint8_t current_adc = master->adc;
    
    if (is_enable) {
        current_adc |= mask;
    } else {
        current_adc &= ~mask;
    }
    
    return ex_master_set_port_adc_mode(master, current_adc);
}

bool ex_master_adc_read(ex_master_t* master, ex_master_adc_pin_t pin, uint16_t* out) {
    if (master == NULL || out == NULL || pin > EX_MASTER_ADC_PIN_7) {
        return false;
    }
    
    // Check if ADC is enabled for this pin
    if (!(master->adc & ADC_PIN_TO_MASK(pin))) {
        return false;
    }
    
    // Start ADC conversion
    if (!write_register(master, EX_PROTOCOL_REG_ADC_START, (uint8_t)pin)) {
        return false;
    }
    
    // Poll status (implementation without delay)
    uint8_t status;
    int max_attempts = 1000;  // Simple timeout
    
    for (int i = 0; i < max_attempts; i++) {
        if (!read_register(master, EX_PROTOCOL_REG_ADC_STATUS, &status)) {
            return false;
        }
        
        if (status == EX_PROTOCOL_STATUS_READY) {
            break;
        } else if (status == EX_PROTOCOL_STATUS_ERROR) {
            return false;
        }
        // If busy, continue polling
    }
    
    if (status != EX_PROTOCOL_STATUS_READY) {
        return false;  // Timeout
    }
    
    // Read high byte
    uint8_t high_byte;
    if (!read_register(master, EX_PROTOCOL_REG_ADC_VAL_H, &high_byte)) {
        return false;
    }
    
    // Read low byte
    uint8_t low_byte;
    if (!read_register(master, EX_PROTOCOL_REG_ADC_VAL_L, &low_byte)) {
        return false;
    }
    
    // Combine bytes
    *out = (uint16_t)((high_byte << 8) | low_byte);
    return true;
}