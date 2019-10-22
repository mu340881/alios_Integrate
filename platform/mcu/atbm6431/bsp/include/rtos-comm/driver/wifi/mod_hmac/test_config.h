#ifndef __TEST_CONFIG_H__
#define __TEST_CONFIG_H__

/*
send mpdu_ACK	0
send mpdu_NOACK	1
send ampdu	2
*/
#define NOACK_MASK  BIT(7)
enum{
TX_STOP_JUST_RX =0,
UDP_MPDU_ACK_TYPE =1,
UDP_MPDU_NOACK_TYPE = UDP_MPDU_ACK_TYPE|NOACK_MASK ,
TCP_MPDU_ACK_TYPE =2,
TCP_MPDU_NOACK_TYPE = TCP_MPDU_ACK_TYPE|NOACK_MASK ,
ICMP_MPDU_ACK_TYPE=3,
ICMP_MPDU_NOACK_TYPE= ICMP_MPDU_ACK_TYPE|NOACK_MASK,
AMPDU_BA_TYPE=4,
};

/*
#define WSM_KEY_TYPE_WEP_DEFAULT	(0)
#define WSM_KEY_TYPE_WEP_PAIRWISE	(1)
#define WSM_KEY_TYPE_TKIP_GROUP		(2)
#define WSM_KEY_TYPE_TKIP_PAIRWISE	(3)
#define WSM_KEY_TYPE_AES_GROUP		(4)
#define WSM_KEY_TYPE_AES_PAIRWISE	(5)
#define WSM_KEY_TYPE_WAPI_GROUP	(6)
#define WSM_KEY_TYPE_WAPI_PAIRWISE	(7)
*/
#define TEST_KEY_TYPE		0

/*
wep_40  len = 5
wep_104  len = 13
tkip 16+8+8
aes  32
wapi 32
*/
#define TEST_KEY_LEN		0
/*
*/
#define TEST_KEY_DATA		0x01,0x02

/*
#define RATE_INDEX_B_1M           0
#define RATE_INDEX_B_2M           1
#define RATE_INDEX_B_5_5M         2
#define RATE_INDEX_B_11M          3
#define RATE_INDEX_PBCC_22M       4     // not supported/unused
#define RATE_INDEX_PBCC_33M       5     // not supported/unused
#define RATE_INDEX_A_6M           6
#define RATE_INDEX_A_9M           7
#define RATE_INDEX_A_12M          8
#define RATE_INDEX_A_18M          9
#define RATE_INDEX_A_24M          10
#define RATE_INDEX_A_36M          11
#define RATE_INDEX_A_48M          12
#define RATE_INDEX_A_54M          13
#define RATE_INDEX_N_6_5M         14
#define RATE_INDEX_N_13M          15
#define RATE_INDEX_N_19_5M        16
#define RATE_INDEX_N_26M          17
#define RATE_INDEX_N_39M          18
#define RATE_INDEX_N_52M          19
#define RATE_INDEX_N_58_5M        20
#define RATE_INDEX_N_65M          21
*/
#define TEST_RATE_AUTOCTRL 0xff

#define TEST_SEND_RATE_1    TEST_RATE_AUTOCTRL
#define TEST_SEND_RATE_2    1
#define TEST_SEND_RATE_3    1
#define TEST_SEND_RATE_4    1
#define TEST_SEND_RATE_5    1

/*max num is 0xf*/
#define TEST_SEND_PRERATE_RERTY_NUM    2


#define TEST_SEND_DATA_LEN  1500

#define TEST_SEND_DATA_CNT  1000000


#define TEST_BASIC_RATE		(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(6)|BIT(8)|BIT(10)|BIT(11))

#define TEST_LONG_RETRY_NUM		7
#define TEST_SHORT_RETRY_NUM	7


/*short 1:long 0*/
#define TEST_LONG_PREAMBLE		0
#define TEST_SHORT_PREAMBLE		1

/*0 - Mixed, 1 - Greenfield*/
#define MODE_11N_MIXED			0
#define MODE_11N_GREENFIELD		1

#define	TEST_RX_NO_COUNT		0
#define TEST_RX_COUNT			1


/*set 1 ,if need send this AC queue data frame*/
#define TEST_SEND_AC_0			1
#define TEST_SEND_AC_1			0
#define TEST_SEND_AC_2			0
#define TEST_SEND_AC_3			0





#define TEST_BEACON_INTV    	100
#define TEST_DTIM_INTV    		1





#define DEFAULT_BEACON_LOSS_CNT      80

#define DCCM_LEN 	(4*1024)
#define TX_PACKET_NUM 10

struct test_config_txrx_s
{
	uint8	Send_Type;
	uint8	Rate;
	uint8   Preamble_Type;
	uint8	Mode_11n;
	uint8	Tx_Num;
	uint8	Rx_Count;
};



#endif /*__TEST_CONFIG_H__*/
