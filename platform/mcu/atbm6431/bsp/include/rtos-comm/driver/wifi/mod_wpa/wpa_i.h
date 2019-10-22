
#ifndef WPA__I_H
#define WPA__I_H
/*
****************************************************************
*
*wpa_supplicant and hostapd extern function
*
****************************************************************
*/
//#include "wpa_auth_i.h"
extern int eapol_input(struct atbmwifi_vif *priv,struct sk_buff *skb);
//#ifndef HMAC_AP_MODE
extern void wpa_prepare_auth(struct atbmwifi_vif *priv);
extern struct wpa_supplicant * init_wpa_supplicant(struct atbmwifi_vif *priv);
extern void wpa_prepare_assciating(struct atbmwifi_vif *priv);
extern int atbmwifi_wpa_connect_ap(struct atbmwifi_vif *priv,u8 *essid,int essid_len,u8 *key,int key_len,int key_mgmt,int keyid);
extern void wpa_supplicant_event_assoc(struct atbmwifi_vif *priv,uint16 res);
//#else
extern int hostapd_start(struct atbmwifi_vif *priv,const char *ssid,int ssid_len,char *key,char key_len,int key_mgmt);
extern struct hostapd_data *init_hostapd(struct atbmwifi_vif *priv);
extern void hostapd_link_sta_sm(struct atbmwifi_vif *priv,
		  struct atbmwifi_sta_priv *sta_priv,u8* mac);

//extern void hostapd_run(struct atbmwifi_vif *priv,struct wpa_state_machine *sm);
extern int hostapd_rx_assoc_req(struct atbmwifi_vif *priv,struct sk_buff *skb,u16 type);
extern void hostapd_rx_auth(struct atbmwifi_vif *priv,struct sk_buff *skb);
//#endif //HMAC_AP_MODE


#endif /*WPA_SUPPLICANT_I_H*/
