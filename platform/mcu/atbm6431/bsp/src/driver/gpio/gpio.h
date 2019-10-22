#ifndef __GPIO_H
#define __GPIO_H

//#include <type.h>
//#include "stddefs.h"
//#include "andes_ae210p.h"
//#include "sysconfig.h"
//#include "andes_init.h"


#define IRQ_GPIO_VECTOR		DEV_INT_NUM_GPIO
#ifndef GPIO_BASE_ADDRESS
#define CPE_GPIO_BASE		(APB_BRG_ADDRESS+0x800000)
#else
#define CPE_GPIO_BASE		GPIO_BASE_ADDRESS
#endif
#define GPIO_CHANNEL_MAX	24


/*GPIO channel direction.*/
#define GPIO_DIR_INPUT	0x00
#define GPIO_DIR_OUTPUT	0x01


/*GPIO pull control*/
#define GPIO_PULL_UP	0x00
#define GPIO_PULL_DOWN	0x01


/*GPIO de-bounce clock source selection. Select pclk (the faster clock) as de-bounce clock source would shorten the de-bounce latency.*/
#define ATCGPIO100_DB_EXTCLK	(0x0 << 31)
#define ATCGPIO100_DB_PCLK		(0x1 << 31)

/*Channel n interrupt mode.*/
#define ATCGPIO100_INTR_MODE_HIGH_LEVEL		((UINT32)0x2)
#define ATCGPIO100_INTR_MODE_LOW_LEVEL		((UINT32)0x3)
#define ATCGPIO100_INTR_MODE_FALLING_EDGE	((UINT32)0x5)
#define ATCGPIO100_INTR_MODE_RISING_EDGE	((UINT32)0x6)
#define ATCGPIO100_INTR_MODE_DUAL_EDGE		((UINT32)0x7)



enum gpio_int_mode{
	GPIO_INT_HIGH_LEVEL		= 0x02,
	GPIO_INT_LOW_LEVEL		= 0x03,
	GPIO_INT_NEGATIVE_EDGE	= 0x05,
	GPIO_INT_POSITIVE_EDGE 	= 0x06,
	GPIO_INT_DUAL_EDGE		= 0x07
};


typedef void (*gpio_handler_p)(UINT32 channel_no);

typedef struct atcgpio100_reg_t {
	volatile UINT32 ver;			// ID and revision register
	volatile UINT32 reserved_4;		// reserved register
	volatile UINT32 reserved_8;		// reserved register
	volatile UINT32 reserved_c;		// reserved register
	volatile UINT32 cfg;			// GPIO configuration register
	volatile UINT32 reserved_14;		// reserved register
	volatile UINT32 reserved_18;		// reserved register
	volatile UINT32 reserved_1c;		// reserved register
	volatile UINT32 din;			// GPIO Channel Data In register
	volatile UINT32 dout;			// GPIO Channel Data Out register
	volatile UINT32 dir;			// GPIO Channel Direction register
	volatile UINT32 do0;			// GPIO Channel Data Out Set 0 register
	volatile UINT32 do1;			// GPIO Channel Data Out Set 1 register
	volatile UINT32 reserved_34;		// reserved register
	volatile UINT32 reserved_38;		// reserved register
	volatile UINT32 reserved_3c;		// reserved register
	volatile UINT32 PullEn;			// GPIO Channel Pull Enable register
	volatile UINT32 PullMode;		// GPIO Channel Pull Mode register
	volatile UINT32 reserved_48;		// reserved register
	volatile UINT32 reserved_4c;		// reserved register
	volatile UINT32 IntrEn;			// GPIO Channel Interrupt Enable register
	volatile UINT32 IntrMode0;		// GPIO Channel Interrupt Mode register 0
	volatile UINT32 IntrMode1;		// GPIO Channel Interrupt Mode register 1
	volatile UINT32 IntrMode2;		// GPIO Channel Interrupt Mode register 2
	volatile UINT32 IntrMode3;		// GPIO Channel Interrupt Mode register 3
	volatile UINT32 IntrSt;			// GPIO Channel Interrupt Status register
	volatile UINT32 reserved_68;		// reserved register
	volatile UINT32 reserved_6c;		// reserved register
	volatile UINT32 DBEn;			// GPIO Channel Debounce Enable register
	volatile UINT32 DBCtrl;			// GPIO Channel Debounce Control register
} atcgpio100_reg_s, *atcgpio100_reg_p;

//extern void Gpio_Default_Handler();
extern void gpio_init(void);
extern void gpio_input_config(UINT32 channel_no);
extern void gpio_output_config(UINT32 channel_no);
extern void gpio_pull_value(UINT32 channel_no,bool value);
extern bool gpio_input_value(UINT32 channel_no);
extern void gpio_toggle_value(UINT32 channel_no);
extern void gpio_output_value(UINT32 channel_no,bool value);
extern void gpio_interrupt_config(UINT32 channel_no,enum gpio_int_mode int_mode,void *pCallback);
extern void gpio_debounce_config(UINT32 channel_no, UINT32 DBPreScale);



extern unsigned char  get_gpio_output_value(unsigned char channel_no);
extern unsigned char  get_gpio_dir_value(unsigned char channel_no);

#if(CHIP_TYPE == CHIP_ATHENAB)
enum io_func{
	GPIO_FUNC,
	I2C_FUNC,
	UART_FUNC,
	SPI_FUNC,
	PWM_FUNC,
};
#elif(CHIP_TYPE == CHIP_ARES)
/*can't be change,define by RTL*/
enum io_func{
	STRAP_PIN,//0
	UART_FUNC,
	I2C_FUNC,
	GPIO_FUNC,
	PWM_FUNC,
	JTAG_FUNC,
	SPI3_FUNC,//6
	DEBUG_BUS,//7
	SPI2_FUNC=10,//10 but real is 2
	
};
#elif(CHIP_TYPE == CHIP_ARES_B)
enum io_func{
	STRAP_PIN=0,//0
	UART_FUNC=1,
	I2C_DEBUG_FUNC=2,
	GPIO_FUNC=3,
	PWM_FUNC=4,
	I2C_PRIPHRAL=5,
	SPI3_FUNC=6,//6
	DEBUG_BUS=7,//7
	SPI2_FUNC=2|BIT(5),// real is 2
	JTAG_FUNC=5|BIT(5),
	
};
#endif  //#if(CHIP_TYPE == CHIP_ATHENAB)
//修改pin的功能类型
//正确返回HAL_SUCCESS ,错误返回HAL_FALSE
extern int gpio_set_pin_func(int pin,enum io_func func);

#endif // __GPIO_H
