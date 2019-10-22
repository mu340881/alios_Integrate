#ifndef __MAC80211_H__
#define __MAC80211_H__

enum nl80211_iftype {
	NL80211_IFTYPE_STATION,
	NL80211_IFTYPE_AP,
	NL80211_IFTYPE_ADHOC,
	NL80211_IFTYPE_MONITOR,
	NL80211_IFTYPE_P2P_CLIENT,
	NL80211_IFTYPE_P2P_GO,

	/* keep last */
	NUM_NL80211_IFTYPES,
	NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};

/**
 * enum nl80211_band - Frequency band
 * @NL80211_BAND_2GHZ: 2.4 GHz ISM band
 * @NL80211_BAND_5GHZ: around 5 GHz band (4.9 - 5.7 GHz)
 */
enum nl80211_band {
	NL80211_BAND_2GHZ,
	NL80211_BAND_5GHZ,
};

/**
 * enum atbmwifi_ieee80211_band - supported frequency bands
 *
 * The bands are assigned this way because the supported
 * bitrates differ in these bands.
 *
 * @IEEE80211_BAND_2GHZ: 2.4GHz ISM band
 * @IEEE80211_BAND_5GHZ: around 5GHz band (4.9-5.7)
 * @IEEE80211_NUM_BANDS: number of defined bands
 */
enum atbmwifi_ieee80211_band {
	IEEE80211_BAND_2GHZ = NL80211_BAND_2GHZ,
	IEEE80211_BAND_5GHZ = NL80211_BAND_5GHZ,
	/* keep last */
	IEEE80211_NUM_BANDS
};
/**
 * enum atbmwifi_ieee80211_ac_numbers - AC numbers as used in mac80211
 * @IEEE80211_AC_VO: voice
 * @IEEE80211_AC_VI: video
 * @IEEE80211_AC_BE: best effort
 * @IEEE80211_AC_BK: background
 */
enum atbmwifi_ieee80211_ac_numbers {
	IEEE80211_AC_VO		= 0,
	IEEE80211_AC_VI		= 1,
	IEEE80211_AC_BE		= 2,
	IEEE80211_AC_BK		= 3,
};
#define IEEE80211_NUM_ACS	4



struct atbmwifi_ieee80211_tx_info{
	u16   preamble_long:1,
		  greenfield:1,
		  short_gi:1,
		  b_40M:1,
		  b_HT:1,
		  b_eapol:1,
		  b_net:1;
	u8	  reserve;
	u8	  link_id;
	int   hw_rate_id;
	int   rate_policy;
	int   txpower;
	u32   flags;//enum mac80211_tx_control_flags
};

struct atbmwifi_ieee80211_rx_status {
	//u64 mactime;
	enum atbmwifi_ieee80211_band band;
	u8 antenna;
	u16 freq;
	s8 signal;
	u8 rate_idx;
//	u8 reserve[2];
	u8 if_id;
	u8 link_id;
	int flag;
	//unsigned int rx_flags;
};

enum mac80211_rx_flags {
	RX_FLAG_MMIC_ERROR	= 1<<0,
	RX_FLAG_DECRYPTED	= 1<<1,
	RX_FLAG_MMIC_STRIPPED	= 1<<3,
	RX_FLAG_IV_STRIPPED	= 1<<4,
	RX_FLAG_FAILED_FCS_CRC	= 1<<5,
	RX_FLAG_FAILED_PLCP_CRC = 1<<6,
	RX_FLAG_MACTIME_MPDU	= 1<<7,
	RX_FLAG_SHORTPRE	= 1<<8,
	RX_FLAG_HT		= 1<<9,
	RX_FLAG_40MHZ		= 1<<10,
	RX_FLAG_SHORT_GI	= 1<<11,
};



#define BASIC_RATE_MASK 0x80

enum rate_flags {
	IEEE80211_RT_11B		= BIT(0),
	IEEE80211_RT_11G		= BIT(1),
	IEEE80211_RT_11N		= BIT(2),
	IEEE80211_RT_BASIC		= BASIC_RATE_MASK,/*must be BIT7*/
};

/**
 * struct atbmwifi_ieee80211_rate - bitrate definition
 *
 * This structure describes a bitrate that an 802.11 PHY can
 * operate with. The two values @hw_value and @hw_value_short
 * are only for driver use when pointers to this structure are
 * passed around.
 *
 * @flags: rate-specific flags
 * @bitrate: bitrate in units of 100 Kbps
 * @hw_value: driver/hardware value for this rate
 * @hw_value_short: driver/hardware value for this rate when
 *	short preamble is used
 */
struct atbmwifi_ieee80211_rate {
	//u32 flags;
	u16 bitrate;
	u8 hw_value;
	u8 rate_flag;
}__packed;

enum mac80211_tx_control_flags {
	IEEE80211_TX_CTL_NO_ACK			= BIT(0),
	IEEE80211_TX_CTL_NO_CCK_RATE		= BIT(1),
	//IEEE80211_TX_CTL_NO_CCK_RATE		= BIT(2),
	IEEE80211_TX_CTL_USE_MINRATE		= BIT(3),
	IEEE80211_TX_CTL_USE_FIXRATE		= BIT(4),	
	IEEE80211_TX_INTFL_TKIP_MIC_FAILURE	= BIT(5),
	IEEE80211_TX_STATUS_EOSP		= BIT(6),
	IEEE80211_TX_STAT_ACK			= BIT(9),
	IEEE80211_TX_INTFL_DONT_ENCRYPT 	= BIT(16),
};



#define HT_CAP_INFO_LDPC_CODING_CAP		((u16) BIT(0))
#define HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET	((u16) BIT(1))
#define HT_CAP_INFO_SMPS_MASK			((u16) (BIT(2) | BIT(3)))
#define HT_CAP_INFO_SMPS_STATIC			((u16) 0)
#define HT_CAP_INFO_SMPS_DYNAMIC		((u16) BIT(2))
#define HT_CAP_INFO_SMPS_DISABLED		((u16) (BIT(2) | BIT(3)))
#define HT_CAP_INFO_GREEN_FIELD			((u16) BIT(4))
#define HT_CAP_INFO_SHORT_GI20MHZ		((u16) BIT(5))
#define HT_CAP_INFO_SHORT_GI40MHZ		((u16) BIT(6))
#define HT_CAP_INFO_TX_STBC			((u16) BIT(7))
#define HT_CAP_INFO_RX_STBC_MASK		((u16) (BIT(8) | BIT(9)))
#define HT_CAP_INFO_RX_STBC_1			((u16) BIT(8))
#define HT_CAP_INFO_RX_STBC_12			((u16) BIT(9))
#define HT_CAP_INFO_RX_STBC_123			((u16) (BIT(8) | BIT(9)))
#define HT_CAP_INFO_DELAYED_BA			((u16) BIT(10))
#define HT_CAP_INFO_MAX_AMSDU_SIZE		((u16) BIT(11))
#define HT_CAP_INFO_DSSS_CCK40MHZ		((u16) BIT(12))
#define HT_CAP_INFO_PSMP_SUPP			((u16) BIT(13))
#define HT_CAP_INFO_40MHZ_INTOLERANT		((u16) BIT(14))
#define HT_CAP_INFO_LSIG_TXOP_PROTECT_SUPPORT	((u16) BIT(15))


#define EXT_HT_CAP_INFO_PCO			((u16) BIT(0))
#define EXT_HT_CAP_INFO_TRANS_TIME_OFFSET	1
#define EXT_HT_CAP_INFO_MCS_FEEDBACK_OFFSET	8
#define EXT_HT_CAP_INFO_HTC_SUPPORTED		((u16) BIT(10))
#define EXT_HT_CAP_INFO_RD_RESPONDER		((u16) BIT(11))




/* Rx MCS bitmask is in the first 77 bits of supported_mcs_set */
#define IEEE80211_HT_MCS_MASK_LEN 10

struct atbmwifi_ieee80211_ht_capabilities {
	le16 ht_capabilities_info;
	u8 a_mpdu_params;
	u8 supported_mcs_set[16];
	le16 ht_extended_capabilities;
	le32 tx_bf_capability_info;
	u8 asel_capabilities;
} __packed;


struct atbmwifi_ieee80211_ht_operation {
	u8 control_chan;
	u8 ht_param;
	le16 operation_mode;
	le16 stbc_param;
	u8 basic_set[16];
}__packed;

/**
 * struct atbmwifi_ieee80211_channel - channel definition
 *
 * This structure describes a single channel for use
 * with cfg80211.
 *
 * @center_freq: center frequency in MHz
 * @hw_value: hardware-specific value for the channel
 * @flags: channel flags from &enum atbmwifi_ieee80211_channel_flags.
 * @orig_flags: channel flags at registration time, used by regulatory
 *	code to support devices with additional restrictions
 * @band: band this channel belongs to.
 * @max_antenna_gain: maximum antenna gain in dBi
 * @max_power: maximum transmission power (in dBm)
 * @beacon_found: helper to regulatory code to indicate when a beacon
 *	has been found on this channel. Use regulatory_hint_found_beacon()
 *	to enable this, this is useful only on 5 GHz band.
 * @orig_mag: internal use
 * @orig_mpwr: internal use
 */
struct atbmwifi_ieee80211_channel {
	//enum atbmwifi_ieee80211_band band;
	s8 max_power;
	u8 hw_value;
	//bool beacon_found;
	//u16 center_freq;
	u16 flags;
	//int max_antenna_gain;
	//u32 orig_flags;
	//int orig_mag, orig_mpwr;
};
/**
 * struct atbmwifi_ieee80211_sta_ht_cap - STA's HT capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11n HT capabilities for an STA.
 *
 * @ht_supported: is HT supported by the STA
 * @cap: HT capabilities map as described in 802.11n spec
 * @ampdu_factor: Maximum A-MPDU length factor
 * @ampdu_density: Minimum A-MPDU spacing
 * @mcs: Supported MCS rates
 */
struct atbmwifi_ieee80211_sta_ht_cap {
	u16 cap; /* use IEEE80211_HT_CAP_ */
	bool ht_supported;
	u8 ampdu_factor;
	u8 ampdu_density;
	struct atbmwifi_ieee80211_mcs_info mcs;
};

/**
 * struct atbmwifi_ieee80211_supported_band - frequency band definition
 *
 * This structure describes a frequency band a wiphy
 * is able to operate in.
 *
 * @channels: Array of channels the hardware can operate in
 *	in this band.
 * @band: the band this structure represents
 * @n_channels: Number of channels in @channels
 * @bitrates: Array of bitrates the hardware can operate with
 *	in this band. Must be sorted to give a valid "supported
 *	rates" IE, i.e. CCK rates first, then OFDM.
 * @n_bitrates: Number of bitrates in @bitrates
 * @ht_cap: HT capabilities in this band
 */
struct atbmwifi_ieee80211_supported_band {
	struct atbmwifi_ieee80211_channel *channels;
	struct atbmwifi_ieee80211_rate *bitrates;
	enum atbmwifi_ieee80211_band band;
	int n_channels;
	int n_bitrates;
	struct atbmwifi_ieee80211_sta_ht_cap ht_cap;
};

struct cfg80211_connect_params {
	u32 crypto_pairwise;
	u32 crypto_group;
	u8 *bssid;
	u8 key[32];
	u8 key_len;
	u8 key_idx;
	u8 encrype;
	u8 update;
};

#endif   /*__MAC80211_H__*/

