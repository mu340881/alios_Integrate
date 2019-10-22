/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#ifndef _OS_ALARM_H_
#define  _OS_ALARM_H_


#define HAL_ALARM_MAX_NUM 8

enum HAL_ALARM_PEROID_TYPE{
 HAL_ALARM_NO_PEROID,
 HAL_ALARM_DAY_PEROID,
 HAL_ALARM_WEEK_PEROID,
 HAL_ALARM_MONTH_PEROID,
 HAL_ALARM_PEROID_TYPE_MAX,
};

/*
HAL_ALARM_TIME_TYPE，
 	在 HAL_ALARM_PEROID_TYPE = HAL_ALARM_NO_PEROID时起作用
用于标示时间是实数，还是虚数时间，
主要用于时间更新时使用，
比如原来系统时间是2018/10/1 ，现在更新时间为2018/10/10 ,这时需要更新所有alarm时间；
如果是虚数时间那么会根据设置的HAL_ALARM_TIME_TYPE更新时间,类似于(今天、明天、下周、下个月)
 	更新规则如下:
HAL_ALARM_REALTIME 实数时间，系统时间更新不影响alarm 时间，如果系统时间更新之后，alarm时间过期直接丢弃
HAL_ALARM_DAY_TIME 更新时间为下一次的tm_hour/tm_min/tm_sec 日期，比如现在时间是12:00:00 ,
[1].alarm时间是12:00:01, 那么闹钟将在今天的12:00:01产生
[2].alarm时间是11:00:01，那么闹钟将在明天11:00:01 产生
HAL_ALARM_WEEK_TIME 更新时间为下一次的week/tm_hour/tm_min/tm_sec 日期，比如现在时间是周一12:00:00 
[1].alarm时间是周一12:00:01 ，那么定时器将在今天12:00:01产生，
[2].alarm时间是周二12:00:01 ，那么定时器将在这周二12:00:01产生，
[3].alarm时间是周一11:00:01 ，那么定时器将在下周一11:00:01 产生
HAL_ALARM_MONTH_TIME 更新时间为下一次的tm_mday/tm_hour/tm_min/tm_sec 日期，比如现在时间是2019/1/1 12:00:00 ，
[1].alarm时间是2018/2/1  12:00:01 那么定时器2019/1/1 12:00:01 产生
[2].alarm时间是2018/12/31 12:00:01 那么定时器2019/1/31 12:00:01 产生
比如现在时间是2019/2/1 12:00:00 ，
[1].alarm时间是2018/12/31 12:00:01 那么定时器2019/3/31 12:00:01 产生，因为2月份没有31号所以，被跳过
*/

enum HAL_ALARM_TIME_TYPE{
 HAL_ALARM_REALTIME,
 HAL_ALARM_DAY_TIME,
 HAL_ALARM_WEEK_TIME,
 HAL_ALARM_MONTH_TIME,
 HAL_ALARM_TIME_TYPE_MAX,
};

struct hal_alarm_tm{
  u16	tm_year;
  u8	tm_mon;
  u8	tm_mday;
  
  u8	tm_hour;
  u8	tm_min;
  u8	tm_sec;
  u8 	time_type;
};

struct hal_alarm_struct {
	struct  list_head list;
	callback func;
	void * arg;
	struct 	hal_alarm_tm time;
	enum 	HAL_ALARM_PEROID_TYPE type;
	u8	 	valid;
	u8	 	add;
	struct system_timer timer;
};
/*显示所有闹钟*/
void hal_alarm_show();
/*删除*/
bool  hal_alarm_delete(int timeid);
/*
设置闹钟
success return  timerid [0~HAL_ALARM_MAX_NUM-1]
else return -1
*/
int hal_set_alarm(struct hal_alarm_tm *time,enum HAL_ALARM_PEROID_TYPE type,callback func,void * arg);
struct hal_alarm_tm *  hal_alarm_get_next_week_date(struct hal_alarm_tm * nexweek/*OUT*/,unsigned int weekday/*IN*/,int hour/*IN*/, int min/*IN*/, int sec/*IN*/);

/*设置系统时间；通过sntp获取*/
bool hal_sync_sntp(void);
/*手动设置系统时间，不通过sntp获取*/
void hal_set_current_real_date(struct hal_alarm_tm *time);

#endif //_OS_ALARM_H_
