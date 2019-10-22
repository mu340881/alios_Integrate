/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#ifndef APP_WIFI_CONFIG_H
#define APP_WIFI_CONFIG_H 1

#if CONFIG_ATBM_PRODUCT_TEST
#define DEFAULT_CHANNEL_VALUE    	7
#else
#define DEFAULT_CHANNEL_VALUE    	1
#endif

#define DEFAULT_AP_SSID 		"wifi_test_ap11"
#define DEFAULT_AP_PWD 			"1234567890"
#define DEFAULT_AP_KEYMGM 		KEY_WPA2
#define DEFAULT_COUNTRY			country_chinese
#endif //APP_WIFI_CONFIG_H

