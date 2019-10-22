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
HAL_ALARM_TIME_TYPE��
 	�� HAL_ALARM_PEROID_TYPE = HAL_ALARM_NO_PEROIDʱ������
���ڱ�ʾʱ����ʵ������������ʱ�䣬
��Ҫ����ʱ�����ʱʹ�ã�
����ԭ��ϵͳʱ����2018/10/1 �����ڸ���ʱ��Ϊ2018/10/10 ,��ʱ��Ҫ��������alarmʱ�䣻
���������ʱ����ô��������õ�HAL_ALARM_TIME_TYPE����ʱ��,������(���졢���졢���ܡ��¸���)
 	���¹�������:
HAL_ALARM_REALTIME ʵ��ʱ�䣬ϵͳʱ����²�Ӱ��alarm ʱ�䣬���ϵͳʱ�����֮��alarmʱ�����ֱ�Ӷ���
HAL_ALARM_DAY_TIME ����ʱ��Ϊ��һ�ε�tm_hour/tm_min/tm_sec ���ڣ���������ʱ����12:00:00 ,
[1].alarmʱ����12:00:01, ��ô���ӽ��ڽ����12:00:01����
[2].alarmʱ����11:00:01����ô���ӽ�������11:00:01 ����
HAL_ALARM_WEEK_TIME ����ʱ��Ϊ��һ�ε�week/tm_hour/tm_min/tm_sec ���ڣ���������ʱ������һ12:00:00 
[1].alarmʱ������һ12:00:01 ����ô��ʱ�����ڽ���12:00:01������
[2].alarmʱ�����ܶ�12:00:01 ����ô��ʱ���������ܶ�12:00:01������
[3].alarmʱ������һ11:00:01 ����ô��ʱ����������һ11:00:01 ����
HAL_ALARM_MONTH_TIME ����ʱ��Ϊ��һ�ε�tm_mday/tm_hour/tm_min/tm_sec ���ڣ���������ʱ����2019/1/1 12:00:00 ��
[1].alarmʱ����2018/2/1  12:00:01 ��ô��ʱ��2019/1/1 12:00:01 ����
[2].alarmʱ����2018/12/31 12:00:01 ��ô��ʱ��2019/1/31 12:00:01 ����
��������ʱ����2019/2/1 12:00:00 ��
[1].alarmʱ����2018/12/31 12:00:01 ��ô��ʱ��2019/3/31 12:00:01 ��������Ϊ2�·�û��31�����ԣ�������
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
/*��ʾ��������*/
void hal_alarm_show();
/*ɾ��*/
bool  hal_alarm_delete(int timeid);
/*
��������
success return  timerid [0~HAL_ALARM_MAX_NUM-1]
else return -1
*/
int hal_set_alarm(struct hal_alarm_tm *time,enum HAL_ALARM_PEROID_TYPE type,callback func,void * arg);
struct hal_alarm_tm *  hal_alarm_get_next_week_date(struct hal_alarm_tm * nexweek/*OUT*/,unsigned int weekday/*IN*/,int hour/*IN*/, int min/*IN*/, int sec/*IN*/);

/*����ϵͳʱ�䣻ͨ��sntp��ȡ*/
bool hal_sync_sntp(void);
/*�ֶ�����ϵͳʱ�䣬��ͨ��sntp��ȡ*/
void hal_set_current_real_date(struct hal_alarm_tm *time);

#endif //_OS_ALARM_H_
