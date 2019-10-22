/* -*-C-*-
*******************************************************************************
* ALTOBEAM
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by ALTOBEAM
******************************************************************************/
#include "pwm.h" 
#include "os_api.h" 
#include "gpio.h"
#include "app_AT_cmd.h"

static u32 period=PWM_APB_TIME/10000;//default is 10K hz
/*frequency dividing ratio */
static u32 freq_ratio[12] = {0};

/*
UART1_RXD_GPIO0_inout_pin	PIT		pit1_ch0_pwm
UART1_TXD_GPIO1_inout_pin	PIT		pit1_ch1_pwm
UART1_CTS_GPIO2_inout_pin	PIT		pit1_ch2_pwm
UART1_RTS_GPIO3_inout_pin	PIT		pit1_ch3_pwm
CLOCK_SEL_GPIO16_inout_pin	PIT		pit2_ch0_pwm
I2S_CLK_GPIO17_inout_pin	PIT		pit2_ch1_pwm
I2S_FS_GPIO18_inout_pin	PIT		    pit2_ch2_pwm
I2S_TXRXD_GPIO19_inout_pin	PIT		pit2_ch3_pwm
UART2_RXD_GPIO20_inout_pin	PIT		pit3_ch0_pwm
UART2_TXD_GPIO21_inout_pin	PIT		pit3_ch1_pwm
UART2_CTS_GPIO22_inout_pin	PIT		pit3_ch2_pwm
UART2_RTS_GPIO23_inout_pin	PIT		pit3_ch3_pwm
*/

/*
 * select the PWM clocke source, APB clock 40MHz or external clock 32KHz.
 * @ u8 pwm_num PWM 0,1,2
 * @ u8 channel 0,1,2,3
 * @ clock_source == TRUE  APB clock 40MHz
 * @ clock_source == FLASE external clock 32KHz
 */
void pwm_set_clock_source(u16 pin, BOOL clock_source)
{
	 HAL_SET_PWM_CLOCK(pin,clock_source);
}


/*
 * get clock source Hz
 */

u32 pwm_get_clock_source(u16 pin)
{
	u32 clock;

	HAL_GET_PWM_CLOCK(pin,clock);
	clock &= 0x4;
    if(clock == 0x4)
    {
    	clock = 40000000;
    }
    else
    {
    	clock = 32000;
    }
    return clock;
}

//pwm_set_prequency<--->pwm_start
//pwm_set_freq<--->pwm_set_duty


// pwm prequency 300Hz - 20MHz
// The output frequency of PWM is preferably an integer multiple of 40MHz
void pwm_set_prequency(u32 time)
{

	if(time == 300)
	{
		u32 preq = 131072;
		period = preq;
	}
	else if(time < 300 || time > 20*MHz)
	{
		 iot_printf("pwm prequency error \n");
		 return;
	}
	else
	{
		period = PWM_APB_TIME/time;
	}
}
/*
 *  pwm prequency 650Hz - 20MHz
 *  @ u32 freq  The output frequency of PWM HZ
 */
void pwm_set_freq(u16 pin, u32 freq)
{
	 u32 input_clock;

	 input_clock = pwm_get_clock_source(pin);

	 if(freq < 650 || freq > 20*MHz)
	 {
		 iot_printf("pwm prequency error \n");
		 return;
	 }
	 else
	 {
		 if(pin>=16 && pin<=23)
		 {
			 pin -= 12;
		 }
		 freq_ratio[pin] = input_clock/freq;
		 iot_printf("freq_ratio[%d] = %d\n",pin,freq_ratio[pin]);
	 }
}

u32 pwm_get_freq(u16 pin)
{
     u32 freq;
     u32 input_clock;

	 input_clock = pwm_get_clock_source(pin);

	 if(pin>=16 && pin<=23)
	 {
		 pin -= 12;
	 }
     freq = input_clock/freq_ratio[pin];
     return freq;
}

/*
 *   @ u8 pwm_num PWM 0,1,2
 *   @ u8 channel 0,1,2,3
 *   @ u32 duty  The output frequency of duty HZ
 */
void pwm_set_duty(u16 pin,u32 duty)
{
	u8 num = pin;
	u16 hight,low;
	u32 pwm_freq = pwm_get_freq(pin);

	if(duty > pwm_freq)
	{
		iot_printf("pwm duty cycle error duty[%d]>%d \n",duty,pwm_freq);
		return;
	}
	 if(num>=16 && num<=23)
	 {
		 num -= 12;
	 }

	if(duty == 0)
	{
		hight = duty;
	}
	else
	{
		hight = (duty*freq_ratio[num]/pwm_freq) - 1;
	}
    if(duty == pwm_freq)
    {
    	low = 0;
    }
    else
    {
    	low = (((pwm_freq - duty)*freq_ratio[num])/pwm_freq) - 1;
    }



    if(pin <=3)
    {
		HAL_PWM1_SET_PERIOD(pin,hight<<16|low);

    }
    else if((pin >=16)&&(pin <=19))
	{
		pin -= 16;
		HAL_PWM2_SET_PERIOD(pin,hight<<16|low);

	}
	else if((pin >=20)&&(pin <=23))
	{
		pin -= 20;
		HAL_PWM3_SET_PERIOD(pin,hight<<16|low);
	}
    pwm_enable(pin);
}

void pwm_enable(u16 pin)
{
	if(pin <=3)
	{
		HAL_PWM1_START(pin);
	}
	else if((pin >=16)&&(pin <=19))
	{
		pin -= 16;
		HAL_PWM2_START(pin);
	}
	else if((pin >=20)&&(pin <=23))
	{
		pin -= 20;
		HAL_PWM3_START(pin);
	}
}

void pwm_disable(u16 pin)
{
	if(pin <=3)
	{
		HAL_PWM1_STOP(pin);
	}
	else if((pin >=16)&&(pin <=19))
	{
		pin -= 16;
		HAL_PWM2_STOP(pin);
	}
	else if((pin >=20)&&(pin <=23))
	{
			pin -= 20;
			HAL_PWM3_STOP(pin);
	}
}





/*
UART1_RXD_GPIO0_inout_pin	PIT		pit1_ch0_pwm
UART1_TXD_GPIO1_inout_pin	PIT		pit1_ch1_pwm
UART1_CTS_GPIO2_inout_pin	PIT		pit1_ch2_pwm
UART1_RTS_GPIO3_inout_pin	PIT		pit1_ch3_pwm
CLOCK_SEL_GPIO16_inout_pin	PIT		pit2_ch0_pwm
I2S_CLK_GPIO17_inout_pin	PIT		pit2_ch1_pwm
I2S_FS_GPIO18_inout_pin	PIT		    pit2_ch2_pwm
I2S_TXRXD_GPIO19_inout_pin	PIT		pit2_ch3_pwm
UART2_RXD_GPIO20_inout_pin	PIT		pit3_ch0_pwm
UART2_TXD_GPIO21_inout_pin	PIT		pit3_ch1_pwm
UART2_CTS_GPIO22_inout_pin	PIT		pit3_ch2_pwm
UART2_RTS_GPIO23_inout_pin	PIT		pit3_ch3_pwm




*/
/* @ u16 pin PWM output pin
 * @ u16 PWM16_Hi Hight duty cycle
 * @ u16 PWM16_Lo Low   duty cycle
 * @ PWM16_Hi + PWM16_Lo > time error
 */
void pwm_start(u16 pin,u16 PWM16_Hi,u16 PWM16_Lo)
{
	if((PWM16_Hi+ PWM16_Lo) > (PWM_APB_TIME/period))
	{
		iot_printf("pwm duty cycle error \n");
		return;
	}
	u16 hight,low;
	//iot_printf("period = %d\n",period);
	//iot_printf("PWM_Hi = 0x%x ,PWM16_Lo = 0x%x\n",PWM16_Hi,PWM16_Lo);
	if(PWM16_Hi == 0)
	{
		hight = PWM16_Hi;
	}

	else
	{
		hight = (PWM16_Hi*period/(PWM16_Hi + PWM16_Lo)) - 1;
	}

	if(PWM16_Lo == 0)
	{
		low = PWM16_Lo;
	}
	else
	{
		low = (PWM16_Lo*period/(PWM16_Hi + PWM16_Lo)) - 1 ;
	}


	//iot_printf("PWM_Hi = 0x%x ,PWM16_Lo = 0x%x ++++\n",hight,low);

	if(pin <=3){
		if(hight==0){
			HAL_PWM1_PARK(pin,0);
			HAL_PWM1_STOP(pin);
		}
		else if(low==0){			
			HAL_PWM1_PARK(pin,TM_CTRL_PWMPARK);
			HAL_PWM1_STOP(pin);
		}
		else{
			HAL_PWM1_INIT(pin);	
			HAL_PWM1_SET_PERIOD(pin,hight<<16|low);
			HAL_PWM1_START(pin);
		}
		
	}
	else if((pin >=16)&&(pin <=19)){
		pin -= 16;
		if(hight==0){
			HAL_PWM2_PARK(pin,0);
			HAL_PWM2_STOP(pin);
		}
		else if(low==0){			
			HAL_PWM2_PARK(pin,TM_CTRL_PWMPARK);
			HAL_PWM2_STOP(pin);
		}
		else{
			HAL_PWM2_INIT(pin);	
			HAL_PWM2_SET_PERIOD(pin,hight<<16|low);
			HAL_PWM2_START(pin);
		}
	}
	else if((pin >=20)&&(pin <=23)){
		pin -= 20;
		if(hight==0){
			HAL_PWM3_PARK(pin,0);
			HAL_PWM3_STOP(pin);
		}
		else if(low==0){			
			HAL_PWM3_PARK(pin,TM_CTRL_PWMPARK);
			HAL_PWM3_STOP(pin);
		}
		else{
			HAL_PWM3_INIT(pin);	
			HAL_PWM3_SET_PERIOD(pin,hight<<16|low);
			HAL_PWM3_START(pin);
		}
	}
}


void pwm_stop(u16 pin)
{	
	if(pin <=3){
		HAL_PWM1_STOP(pin);	
	}
	else if((pin >=16)&&(pin <=19)){
		pin -= 16;
		HAL_PWM2_STOP(pin);
	}
	else if((pin >=20)&&(pin <=23)){
		pin -= 20;
		HAL_PWM3_STOP(pin);
	}
}

static u32 rtc_pwm_mask=0;
static u8 rtc_malloc_pwm_id[RTC_PWM_PIN_NUM];
/*
success return 0~(RTC_PWM_NUM-1)
fail return RTC_PWM_NUM_INVALD
*/
u8 rtc_pwm_alloc()
{
	u8 id=0;
	for(id=0;id<RTC_PWM_NUM;id++){
		if((rtc_pwm_mask & BIT(id))==0){
			rtc_pwm_mask |= BIT(id);
			break;
		}
	}
	return id;
}

void rtc_pwm_free(u8 pwm_id)
{
	rtc_pwm_mask &= ~BIT(pwm_id);
}

/*
//just support  gpio:
pwm clock is 32K,
//GPIO 0~3
//GPIO 20~23
rtc_pwm_id : 0~(RTC_PWM_NUM-1)
high: 0~255
low: 0~255
*/
bool rtc_pwm_start(u16 pin,u8 rtc_pwm_id,u32 high,u32 low)
{
	int gpio_id= pin;
	u32 regdata;
	if(rtc_pwm_id >= RTC_PWM_NUM_INVALD){
		return false;
	}
	
	if(pin > MAX_GPIO_PIN_NUM){
		return false;
	}
	if((PWM_PIN_MASK & BIT(pin))==0){
		return false;
	}
	if(pin >=20)	{
		gpio_id = pin-16;
	}
	//gpio is used to gpio wakeup
	regdata = HW_READ_REG(0x16101084);
	if(regdata&BIT(gpio_id+8)){
		return false;
	}
	
	rtc_malloc_pwm_id[gpio_id]=rtc_pwm_id;
    /*step 0 disable PWM 0,1,2,3,4 channel*/
  //  HW_WRITE_REG_BIT(0x16101060,rtc_pwm_id,rtc_pwm_id,0);
    //bit = HW_READ_REG_BIT(0x16101000,24,24);
    /*step 1 select PWM IO MASK*/
	//gpio_set_pin_func(pin,GPIO_FUNC);
	//gpio_output_config(pin);

    /*step 2 Enable clk PWM 0,1,2,3,4 channel*/
    HW_WRITE_REG_BIT(0x16101060,5+rtc_pwm_id,5+rtc_pwm_id,1);

    /*step 3 high low*/
	if((rtc_pwm_id & 1)==0){
	    /* PWM 0 */
	    HW_WRITE_REG_BIT(0x16101064+(rtc_pwm_id>>1)*4,15,8,high);
	    HW_WRITE_REG_BIT(0x16101064+(rtc_pwm_id>>1)*4,7,0,low);
	}
	else {
	    /* PWM 1 */
	    HW_WRITE_REG_BIT(0x16101064+(rtc_pwm_id>>1)*4,31,24,high);
	    HW_WRITE_REG_BIT(0x16101064+(rtc_pwm_id>>1)*4,23,16,low);
	}
    /*step 4 Enable PWM 0,1,2,3,4 channel*/
    //HW_WRITE_REG_BIT(0x16101088,27,24,1);  //GPIO 22
    HW_WRITE_REG_BIT(0x16101088,3+gpio_id*4,0+gpio_id*4,rtc_pwm_id);  //GPIO 23
    /*io mux寄存器，各bit由低到高分别表示io0-7用于pwm输出*/
    HW_WRITE_REG_BIT(0x16101084,gpio_id,gpio_id,1);
    HW_WRITE_REG_BIT(0x16101060,rtc_pwm_id,rtc_pwm_id,1);
    return true;
}
void rtc_pwm_stop(u8 pin)
{	
	int gpio_id= pin;
	if(pin > MAX_GPIO_PIN_NUM){
		return ;
	}
	if((PWM_PIN_MASK & BIT(pin))==0){
		return ;
	}
	if(rtc_malloc_pwm_id[pin]>=RTC_PWM_NUM_INVALD){
		return ;
	}
	if(pin >=20)	{
		gpio_id = pin-16;
	}
	rtc_pwm_free(rtc_malloc_pwm_id[pin]);
	//rtc_pwm_mask &= ~BIT(rtc_malloc_pwm_id[pin]);
    HW_WRITE_REG_BIT(0x16101060,rtc_malloc_pwm_id[pin],rtc_malloc_pwm_id[pin],0);
    /*io mux寄存器，各bit由低到高分别表示io0-7用于pwm输出*/
    HW_WRITE_REG_BIT(0x16101084,gpio_id,gpio_id,0);
	rtc_malloc_pwm_id[pin]=RTC_PWM_NUM_INVALD;
	
}

/*
void rtc_pwm_test()
{
	u8 id=rtc_pwm_alloc();
	iot_printf("rtc_pwm_alloc %d\n",id);
	rtc_pwm_start(22,id,244,122);
}
*/
