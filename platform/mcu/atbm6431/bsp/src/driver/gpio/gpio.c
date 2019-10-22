// Copyright (C) 2015, altobeam Technology Corp. Confidential Proprietary
#include "hal.h"
#include "os_api.h"
#include "gpio.h"
#include "app_AT_cmd.h"
#include "pwm/pwm.h"



PRIVATE atcgpio100_reg_p dev = (atcgpio100_reg_p)CPE_GPIO_BASE;
PRIVATE volatile UINT32 gpio_isr_flag = 0;

void Gpio_Default_Handler(UINT32 channel_no);
gpio_handler_p gpio_handler_tab[GPIO_CHANNEL_MAX]=
{								
		Gpio_Default_Handler,			// 0	  
		Gpio_Default_Handler,			// 1	  
		Gpio_Default_Handler,			// 2	
		Gpio_Default_Handler,			// 3	  
		Gpio_Default_Handler,			// 4	  
		Gpio_Default_Handler,			// 5	  
		Gpio_Default_Handler,			// 6	  
		Gpio_Default_Handler,			// 7	  
//		Gpio_Default_Handler,			// 8
//		Gpio_Default_Handler,			// 9
//		Gpio_Default_Handler,			// 10
//		Gpio_Default_Handler,			// 11
//		Gpio_Default_Handler,			// 12
//		Gpio_Default_Handler,			// 13
//		Gpio_Default_Handler,			// 14
//		Gpio_Default_Handler,			// 15
//		Gpio_Default_Handler,			// 16
//		Gpio_Default_Handler,			// 17
//		Gpio_Default_Handler,			// 18
//		Gpio_Default_Handler,			// 19
//		Gpio_Default_Handler,			// 20
//		Gpio_Default_Handler,			// 21
//		Gpio_Default_Handler,			// 22
//		Gpio_Default_Handler,			// 23
//		Gpio_Default_Handler,			// 24
//		Gpio_Default_Handler,			// 25
//		Gpio_Default_Handler,			// 26
//		Gpio_Default_Handler,			// 27
//		Gpio_Default_Handler,			// 28
//		Gpio_Default_Handler,			// 29
//		Gpio_Default_Handler,			// 30
//		Gpio_Default_Handler,			// 31
};


void Gpio_Default_Handler(UINT32 channel_no)
{
	DBG_Printf("Gpio_Default_Handler channel_no is %d\n",channel_no);
}

NORELOC PRIVATE void gpio_isr(void)
{
	UINT32 loop = 0,channel = 0;
	/* Disable GPIO interupt	  */
	disable_interrupt(IRQ_GPIO_VECTOR);	
	//dev->IntrEn = 0;
	gpio_isr_flag = dev->IntrSt;
	dev->IntrSt = gpio_isr_flag;
	
	loop = gpio_isr_flag;
	
	for(channel = 0; channel<GPIO_CHANNEL_MAX; channel++){
		if(loop&0x01){
			(*gpio_handler_tab[channel])(channel);
		}
		loop = loop >> 1;
	}
	
	enable_interrupt(IRQ_GPIO_VECTOR);
}

/*
 * @ channel_no
 * @ DBPreScale 31.25us ~ 8ms
 * @ uint um
 */
FLASH_FUNC void gpio_debounce_config(UINT32 channel_no, UINT32 DBPreScale)
{
	DBPreScale = (32*DBPreScale)/1000-1;
	dev->DBCtrl = ATCGPIO100_DB_EXTCLK | DBPreScale;
	dev->DBEn = 1<<channel_no;
}

FLASH_FUNC void gpio_input_config(UINT32 channel_no){
	dev->dir &= ~(GPIO_DIR_OUTPUT<<channel_no);
}

FLASH_FUNC void gpio_output_config(UINT32 channel_no){
	dev->dir |= GPIO_DIR_OUTPUT<<channel_no;
}


/*
value is in GPIO_PULL_UP ~ GPIO_PULL_DOWN
*/
FLASH_FUNC void gpio_pull_value(UINT32 channel_no,bool value)
{
	u32 regaddr;
#if(TARGET_CHIP == ARES_B)
	if(channel_no == 22 || channel_no == 23)
	{
		HW_WRITE_REG_BIT(0x17400000,8,8,1);
	}

	regaddr = 0x17400000+(channel_no/2)*4+4;
	if(value&0x01)
	{
		if(channel_no&1)
		{
			HW_WRITE_REG_BIT(regaddr,27,27,1);      //GPIO23
			HW_WRITE_REG_BIT(regaddr,28,28,0);      //GPIO23
		}
		else
		{
			HW_WRITE_REG_BIT(regaddr,11,11,1);      //GPIO22
			HW_WRITE_REG_BIT(regaddr,12,12,0);      //GPIO22
		}
	}
	else
	{
		if(channel_no&1)
		{
			HW_WRITE_REG_BIT(regaddr,27,27,0);      //GPIO23
			HW_WRITE_REG_BIT(regaddr,28,28,1);      //GPIO23
		}
		else
		{
			HW_WRITE_REG_BIT(regaddr,11,11,0);      //GPIO22
			HW_WRITE_REG_BIT(regaddr,12,12,1);      //GPIO22
		}
	}
#else
	if(value&0x01)
		dev->PullMode|= (value&0x01)<<channel_no;
	else
		dev->PullMode &= ~(0x01 << channel_no);

	dev->PullEn |= 0x01 << channel_no;
#endif//(TARGET_CHIP == ARES_B)
}

FLASH_FUNC bool gpio_input_value(UINT32 channel_no){
	UINT32 din = dev->din;
	return (din>>channel_no & 0x01);
}

FLASH_FUNC void gpio_output_value(UINT32 channel_no,bool value){
	if(value&0x01)
		dev->dout |= 0x01 << channel_no;
	else
		dev->dout &= ~(0x01 << channel_no);

}

FLASH_FUNC void gpio_toggle_value(UINT32 channel_no)
{
    dev->dout ^= (0x01 << channel_no);
}

FLASH_FUNC unsigned char  get_gpio_output_value(unsigned char channel_no)
{
    unsigned char u8level = 0;
    u8level = (dev->dout&(0x01 << channel_no));
	if(u8level)
	{
		u8level = 1;
	}
	return u8level;
}

FLASH_FUNC unsigned char  get_gpio_dir_value(unsigned char channel_no)
{
	u32 u8dir = 0;
	u8dir = (dev->dir&(0x01 << channel_no));
	if(u8dir)
	{
		  u8dir = 1;
	}
	 return u8dir;
}


/*
	int mode in enum gpio_int_mode
*/
FLASH_FUNC  void gpio_interrupt_config(UINT32 channel_no,enum gpio_int_mode int_mode,void *pCallback)
{
	volatile UINT32 *mode_ptr = &dev->IntrMode0;
	uint32	old_mod;
	uint32 	new_mod;
	
	/*find the interrupt register of current channel no*/
	mode_ptr += channel_no / 8;
	
	old_mod = *mode_ptr;
	new_mod = int_mode;
	new_mod = new_mod << ((channel_no % 8) * 4);

	*mode_ptr = (old_mod | new_mod);
	/*set interrupt mode of cuttent channel no*/
//	*mode_ptr = int_mode << (channel_no % 8) * 4;

	/*register callback func*/
	gpio_handler_tab[channel_no] = pCallback;
	_gpio_udelay(10);
	/*channel Interrupt enable*/
	dev->IntrEn |= ((UINT32)0x1) << channel_no;
}

FLASH_FUNC  void gpio_interrupt_disable(UINT32 channel_no)
{
	/*channel Interrupt disable*/
	dev->IntrEn &= ((UINT32)0x0) << channel_no;
}

FLASH_FUNC void gpio_init(void)
{
	/* Register LISR */
	IRQ_RegisterHandler(IRQ_GPIO_VECTOR, (void *)gpio_isr);
	enable_interrupt(IRQ_GPIO_VECTOR);
}

/*
GPIO0_BOOTSEL_pin
　
	WLAN_IRQ			0x1610_00cc	[2:0]	0
	GPIO0			0x1610_00cc	[2:0]	2
	BOOT_SEL			0x1610_00cc	[2:0]	3
GPIO1 _pin
　
　	I2C_DEB_SCL		0x1610_00cc	[18:16]	0
	GPIO1			0x1610_00cc	[18:16]	2
	UART1_TXD		0x1610_00cc	[18:16]	3
GPIO2_ pin
　	I2C_peripheral_SDA	0x1610_00ac	[18:16]	1
	GPIO2			0x1610_00ac	[18:16]	2
GPIO3 _pin
　
　	I2C_DEB_SDA		0x1610_00d0	[2:0]	0
	GPIO3			0x1610_00d0	[2:0]	2
	UART1_RXD		0x1610_00d0	[2:0]	3
*/

FLASH_FUNC int gpio_set_pin_func(int pin,enum io_func func)
{
#if(TARGET_CHIP == ARES_B)
	int gpio_id= pin;
	if(pin > MAX_GPIO_PIN_NUM){
		return ;
	}
	if(pin == 22 || pin == 23)
	{
		HW_WRITE_REG_BIT(0x17400000,8,8,1);
	}
	//rtc pwm iomux clear
	if(PWM_PIN_MASK & BIT(pin))
	{
		if(pin >=20)	{
			gpio_id = pin-16;
		}
		HW_WRITE_REG_BIT(0x16101084,gpio_id,gpio_id,0);
	}
#endif//(TARGET_CHIP == ARES_B)

#if (CHIP_TYPE == CHIP_ATHENAB)
	if(pin==0){
		if(func == GPIO_FUNC){
			//GPIO0			0x1610_00cc	[2:0]	2
			HW_WRITE_REG_BIT(0x161000cc,2,0,2);
			return HAL_SUCCESS;
		}
		else if(func == I2C_FUNC){
			
		}
		else if(func == UART_FUNC){
			
		}
	}
	else if(pin==1){
		if(func == GPIO_FUNC){
			//GPIO1			0x1610_00cc [18:16] 2
			HW_WRITE_REG_BIT(0x161000cc,18,16,2);
			return HAL_SUCCESS;
		}
		else if(func == UART_FUNC){
			//UART1_TXD		0x1610_00cc [18:16] 3
			HW_WRITE_REG_BIT(0x161000cc,18,16,3);
			return HAL_SUCCESS;
		}
		else if(func == I2C_FUNC){
			//I2C_DEB_SCL 	0x1610_00cc [18:16] 0
			HW_WRITE_REG_BIT(0x161000cc,18,16,0);
			return HAL_SUCCESS;
		}
	}
	else if(pin==2){
		if(func == GPIO_FUNC){
			//GPIO2			0x1610_00ac [18:16] 2
			HW_WRITE_REG_BIT(0x161000ac,18,16,2);
			return HAL_SUCCESS;
		}
		else if(func == UART_FUNC){
		}
		else if(func == I2C_FUNC){
			//I2C_peripheral_SDA	0x1610_00ac [18:16] 1
			HW_WRITE_REG_BIT(0x161000ac,18,16,1);
			return HAL_SUCCESS;
		}
	}
	else if(pin==3){
	//I2C_DEB_SDA		0x1610_00d0	[2:0]	0
	//GPIO3			0x1610_00d0	[2:0]	2
	//UART1_RXD		0x1610_00d0	[2:0]	3
		if(func == I2C_FUNC ){
			HW_WRITE_REG_BIT(0x161000d0,2,0,0);
			return HAL_SUCCESS;
		}
		else if(func == GPIO_FUNC){
			HW_WRITE_REG_BIT(0x161000d0,2,0,2);
			return HAL_SUCCESS;
		}
		else if(func == UART_FUNC ){
			HW_WRITE_REG_BIT(0x161000d0,2,0,3);
			return HAL_SUCCESS;
		}
	}
#elif (CHIP_TYPE == CHIP_ARES_B)
	
	u32 regaddr;
	func = func&0x7;
	regaddr = 0x17400000+(pin/2)*4+4;
	if(pin&1){
		HW_WRITE_REG_BIT(regaddr,19,16,func);
	}
	else {
		HW_WRITE_REG_BIT(regaddr,3,0,func);		
	}

#else //#if (CHIP_TYPE == CHIP_ARES_A)
	u32 regaddr;
	func = func&0x7;
	regaddr = 0x17400000+(pin/2)*4+4;  
	if(pin&1){
		HW_WRITE_REG_BIT(regaddr,19,16,func);
	}
	else {
		HW_WRITE_REG_BIT(regaddr,3,0,func);		
	}
#endif //#if (CHIP_TYPE == CHIP_ATHENAB)
	return HAL_FALSE;
}


/*********************************************************************
*below is test code. if no need , remove it.
*********************************************************************/
NO_FLASH_FUNC void _gpio_udelay(int us)
{
	volatile UINT32 loop = 100*us;
	while(loop--){
		;
	}
}
#if 0

void pull_test()
{
	volatile int tmp = 2000;
	int i=0;
	while(1){
		
		for(i=0;i<GPIO_CHANNEL_MAX; i++)
		{
			gpio_pull_value(i,GPIO_PULL_UP);
			_gpio_udelay(tmp);
			gpio_pull_value(i,GPIO_PULL_DOWN);
			_gpio_udelay(tmp);
		}
		
		for(i=0;i<GPIO_CHANNEL_MAX; i++)
		{
			gpio_pull_value(GPIO_CHANNEL_MAX-i,GPIO_PULL_UP);
			_gpio_udelay(tmp);
			gpio_pull_value(GPIO_CHANNEL_MAX-i,GPIO_PULL_DOWN);
			_gpio_udelay(tmp);
		}

		if(tmp <= 100)
			tmp = 1000;
		else if (tmp>100 && tmp <=500)
			tmp -=100;
		else
			tmp -= 200;
	}
}

void output_test()
{
	volatile int tmp = 2000;

	int i=0;
	for(i=0;i<GPIO_CHANNEL_MAX; i++)
	{
		gpio_output_config(i);
	}
	
	while(1){
		
		for(i=0;i<GPIO_CHANNEL_MAX; i++)
		{
			gpio_output_value(i,1);
			_gpio_udelay(tmp);
			gpio_output_value(i,0);
			_gpio_udelay(tmp);
		}
		
		for(i=0;i<GPIO_CHANNEL_MAX; i++)
		{
			gpio_output_value(GPIO_CHANNEL_MAX-i,1);
			_gpio_udelay(tmp);
			gpio_output_value(GPIO_CHANNEL_MAX-i,0);
			_gpio_udelay(tmp);
		}

		if(tmp <= 100)
			tmp = 1000;
		else if (tmp>100 && tmp <=500)
			tmp -=100;
		else
			tmp -= 200;
	}
}

void input_test()
{
	int i=0;
	int inputnew[8];
	int inputold[8];
	
	for(i=0;i<GPIO_CHANNEL_MAX; i++)
	{
		gpio_input_config(i);
		inputold[i] = 0;
		inputnew[i] = 0;
	}
	
	while(1)
	{
		for(i=0;i<GPIO_CHANNEL_MAX; i++)
		{
			if(inputold[i] != inputnew[i]){
				DBG_Printf("channel %d input value changed from %d to %d\n",i,inputold[i],inputnew[i]);
				inputold[i] = inputnew[i];
			}
		}

		for(i=0;i<GPIO_CHANNEL_MAX; i++)
		{
			inputnew[i] = gpio_input_value(i);
		}
	}
	
}

void interrupt_test()
{
	int i=0;
	for(i=0;i<GPIO_CHANNEL_MAX; i++)
	{
		gpio_interrupt_config(i,GPIO_INT_HIGH_LEVEL,Gpio_Default_Handler);
	}

}
//#include "lmac_os.h"
#define GPIO_FUNC_CONF_ADDR_BASE		(0x16100000+0xa0)
#define GPIO_FUNC_ENABLE				(BIT(1)|BIT(17)|BIT(11)|BIT(27))
#define GPIO_FUNC_RST					(~(BIT(0)|BIT(1)|BIT(2)|BIT(16)|BIT(17)|BIT(18))|BIT(12)|BIT(28))
#define GPIO_CLK_ADDR					(0x16100000+0x14)
#define GPIO_CLK_ENABLE					(BIT(9))

void gpio_func_config(void)
{
	
	uint32 reg_val = 0;
	uint8_t io_conut = 0;
	/***enable gpio clk*****/
	DBG_Printf("gpio_func_config \n");
	lmac_Wait(1);
	DBG_Printf("set clk \n");
	lmac_Wait(10);
	reg_val = HW_READ_REG(GPIO_CLK_ADDR);
	HW_WRITE_REG(GPIO_CLK_ADDR,reg_val|GPIO_CLK_ENABLE);
	DBG_Printf("set gpio \n");
	lmac_Wait(10);
	for(io_conut = 0;io_conut<6;io_conut++)
	{
		
		#if 0
		reg_val &= GPIO_FUNC_RST;
		HW_WRITE_REG(GPIO_FUNC_CONF_ADDR_BASE+4*io_conut,reg_val|GPIO_FUNC_ENABLE);
		
		#endif
		HW_WRITE_REG(GPIO_FUNC_CONF_ADDR_BASE+4*io_conut,0x0b020b02);
		reg_val = HW_READ_REG(GPIO_FUNC_CONF_ADDR_BASE+4*io_conut);
		DBG_Printf("set gpio addr (%x)=(%x)\n",GPIO_FUNC_CONF_ADDR_BASE+4*io_conut,reg_val);
		lmac_Wait(10);
	}

	
}
static uint8_t io_inited = 0;
static uint8_t isr_mod = 0xFF;
void gpio_test(uint8_t irq_io,uint8 io_index,uint8 out_put)
{
	if(irq_io == 0)
	{
		disable_interrupt(IRQ_GPIO_VECTOR);
		dev->IntrEn = 0;
		if(io_inited == 0)
		{
			gpio_func_config();
			io_inited = 1;
		}

		gpio_pull_value(io_index,1);
		gpio_output_config(io_index);
		gpio_output_value(io_index,out_put);
		isr_mod = 0xFF;
		
	}
	else
	{
		io_inited = 0;
		
		if(isr_mod != out_put)
		{
			uint32 reg_val = 0;
			uint8_t io_conut = 0;
			volatile UINT32 *mode_ptr = &dev->IntrMode0;
			DBG_Printf("gpio isr mod(%d)\n",out_put);
//			gpio_func_config();
			disable_interrupt(IRQ_GPIO_VECTOR);
			for(io_conut = 0;io_conut<6;io_conut++)
			{
				
				#if 0
				reg_val &= GPIO_FUNC_RST;
				HW_WRITE_REG(GPIO_FUNC_CONF_ADDR_BASE+4*io_conut,reg_val|GPIO_FUNC_ENABLE);
				
				#endif
				HW_WRITE_REG(GPIO_FUNC_CONF_ADDR_BASE+4*io_conut,0x00020002);
				reg_val = HW_READ_REG(GPIO_FUNC_CONF_ADDR_BASE+4*io_conut);
				DBG_Printf("set gpio addr (%x)=(%x)\n",GPIO_FUNC_CONF_ADDR_BASE+4*io_conut,reg_val);
				lmac_Wait(10);
			}
			
			dev->IntrEn = 0;
			dev->dir = 0;
			*mode_ptr = 0;
			mode_ptr++;
			*mode_ptr = 0;
			gpio_interrupt_config(7,out_put,Gpio_Default_Handler);
			gpio_interrupt_config(8,out_put,Gpio_Default_Handler);
			gpio_interrupt_config(10,out_put,Gpio_Default_Handler);
			isr_mod = out_put;
			gpio_init();
		}
		
	}
//	gpio_init();
	
	//pull_test();
//	output_test();
	//input_test();
	//interrupt_test();
//	while(1);
}
#endif  //0
