#ifndef _PWM_H_
#define _PWM_H_
#include "hal.h"

//#define MHZ 1000000
#define PWM_APB_TIME 40*MHz
#define PWM_EXT_TIME 32*KHz
#define RTC_PWM_NUM	 5
#define RTC_PWM_PIN_NUM	 8
#define RTC_PWM_NUM_INVALD	 5
#define PWM_PIN_MASK  (BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(20)|BIT(21)|BIT(22)|BIT(23))
/////////////////////////////////////////////////
#define HAL_PWM1_START(_tmr_)                                  \
        outw(TM1_CHNEN, inw(TM1_CHNEN) | (0x8 << (4 * (_tmr_))))

#define HAL_PWM1_STOP(_tmr_)                                   \
         outw(TM1_CHNEN, inw(TM1_CHNEN) & ~(0x8 << (4 * (_tmr_))))


#define HAL_PWM1_INIT(_tmr_)                                   \
		do {																	\
				HAL_PWM1_STOP(_tmr_);											\
				outw(TM1_CTRL(_tmr_),TM_CTRL_MODE_PWM | TM_CTRL_PCLK); 			\
		} while(0)


#define HAL_PWM1_SET_PERIOD(_tmr_, _period_)                  \
				outw(TM1_LOAD(_tmr_), (_period_))

#define HAL_PWM1_PARK(_tmr_,_level) 	outw(TM1_CTRL(_tmr_),TM_CTRL_MODE_PWM |( _level))


/////////////////////////////////////////////////

#define HAL_PWM2_START(_tmr_)                                  \
        outw(TM2_CHNEN, inw(TM2_CHNEN) | (0x8 << (4 * (_tmr_))))

#define HAL_PWM2_STOP(_tmr_)                                   \
         outw(TM2_CHNEN, inw(TM2_CHNEN) & ~(0x8 << (4 * (_tmr_))))


#define HAL_PWM2_INIT(_tmr_)                                   \
		do {																	\
				HAL_PWM2_STOP(_tmr_);											\
				outw(TM2_CTRL(_tmr_),TM_CTRL_MODE_PWM | TM_CTRL_PCLK); 			\
		} while(0)


#define HAL_PWM2_SET_PERIOD(_tmr_, _period_)                  \
				outw(TM2_LOAD(_tmr_), (_period_))
				
#define HAL_PWM2_PARK(_tmr_,_level)   outw(TM2_CTRL(_tmr_),TM_CTRL_MODE_PWM |(_level)) 


/////////////////////////////////////////////////

#define HAL_PWM3_START(_tmr_)                                  \
        outw(TM3_CHNEN, inw(TM3_CHNEN) | (0x8 << (4 * (_tmr_))))

#define HAL_PWM3_STOP(_tmr_)                                   \
         outw(TM3_CHNEN, inw(TM3_CHNEN) & ~(0x8 << (4 * (_tmr_))))


#define HAL_PWM3_INIT(_tmr_)                                   \
		do {																	\
				HAL_PWM3_STOP(_tmr_);											\
				outw(TM3_CTRL(_tmr_),TM_CTRL_MODE_PWM | TM_CTRL_PCLK); 			\
		} while(0)


#define HAL_PWM3_SET_PERIOD(_tmr_, _period_)                  \
				outw(TM3_LOAD(_tmr_), (_period_))

#define HAL_PWM3_PARK(_tmr_,_level)   outw(TM3_CTRL(_tmr_),TM_CTRL_MODE_PWM |(_level) )


#define HAL_GET_PWM_CLOCK(_num_,_clocksource_)                                     \
        do{                                                                        \
            if(_num_ <= 3)                                                         \
			{                                                                      \
				_clocksource_ = inw(TM1_CTRL(_num_));                          \
			}                                                                      \
																				   \
			if((_num_ >=16)&&(_num_ <=19))                                         \
			{                                                                      \
                _num_ -= 16;                                                       \
				_clocksource_ = inw(TM2_CTRL(_num_));                          \
			}                                                                      \
																				   \
			if((_num_ >=20)&&(_num_ <=23))                                         \
			{                                                                      \
                _num_ -= 20;                                                       \
				_clocksource_ = inw(TM3_CTRL(_num_));                          \
			}                                                                      \
                                                                                   \
        }while(0)



#define HAL_SET_PWM_CLOCK(_num_,_clockmode_)                                           \
		do {	                                                                       \
                if(_num_ <= 3)                                                         \
                {                                                                      \
                	if(_clockmode_ == TRUE)                                            \
                	{                                                                  \
                		 outw(TM1_CTRL(_num_),TM_CTRL_MODE_PWM | TM_CTRL_PCLK);        \
                    }                                                                  \
                    else                                                               \
                    {                                                                  \
                    	 outw(TM1_CTRL(_num_),TM_CTRL_MODE_PWM | TM_CTRL_EXTCLK);      \
                    }                                                                  \
                }                                                                      \
                                                                                       \
                if((_num_ >=16)&&(_num_ <=19))                                         \
                {                                                                      \
                	_num_ -= 16;                                                       \
                	if(_clockmode_ == TRUE)                                            \
                	{                                                                  \
                		 outw(TM2_CTRL(_num_),TM_CTRL_MODE_PWM | TM_CTRL_PCLK);        \
                    }                                                                  \
                    else                                                               \
                    {                                                                  \
                    	 outw(TM2_CTRL(_num_),TM_CTRL_MODE_PWM | TM_CTRL_EXTCLK);      \
                    }                                                                  \
                }                                                                      \
                                                                                       \
                if((_num_ >=20)&&(_num_ <=23))                                         \
                {                                                                      \
                	_num_ -= 20;                                                       \
                	if(_clockmode_ == TRUE)                                            \
                	{                                                                  \
                		 outw(TM3_CTRL(_num_),TM_CTRL_MODE_PWM | TM_CTRL_PCLK);        \
                    }                                                                  \
                    else                                                               \
                    {                                                                  \
                    	 outw(TM3_CTRL(_num_),TM_CTRL_MODE_PWM | TM_CTRL_EXTCLK);      \
                    }                                                                  \
                }                                                                      \
		   } while(0)

/////////////////////////////////////////////////////////
void pwm_set_clock_source(u16 pin, BOOL clock_source);
u32 pwm_get_clock_source(u16 pin);
void pwm_set_freq(u16 pin, u32 freq);
u32 pwm_get_freq(u16 pin);
void pwm_set_duty(u16 pin,u32 duty);
void pwm_enable(u16 pin);
void pwm_disable(u16 pin);

void pwm_set_prequency(u32 time);
void pwm_start(u16 pin,u16 PWM16_Hi,u16 PWM16_Lo);
void pwm_stop(u16 pin);
#endif /* _PWM_H_ */

