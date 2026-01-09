#include "debug.h"

#include "ex_adc.h"
#include "ex_core.h"
#include "ex_gpio.h"
#include "ex_i2c.h"



#define RXAdderss   0x40
#define TXAdderss   0x40



int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

	#ifdef ENABLE_DEBUG
		USART_Printf_Init(115200);
	#endif

	ex_gpio_init();
	exp_adc_init();
	ex_i2c_slave_init(100000, RXAdderss);

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	uint8_t reg = 0;
	bool is_second_read = false;
    while(1)
    {
		const ex_i2c_event_t event =  ex_i2c_pull();
		if(event == EX_I2C_EVENT_NONE){
			continue;
		}

		if(event == EX_I2C_EVENT_WRITE_REQ){
			uint8_t tmp = 0;
			while(ex_i2c_slave_read(&tmp)){
				//is this second read or not?
				if(is_second_read){
					is_second_read = false;
					const uint8_t data = tmp;
					ex_core_write(reg, data);

					printf("READ_2 = %d\r\n", data);
				}else{
					reg = tmp;
					is_second_read = true;
					ex_core_set_read_reg(reg);

					printf("READ_1 = %d\r\n", reg);
				}
			}
		}else if (event == EX_I2C_EVENT_READ_REQ){
			is_second_read = false;
			const uint8_t data = ex_core_read();
			ex_i2c_slave_write(data, true);

			printf("WRITE = [%d]%d\r\n", reg, data);
		}
    }
}
