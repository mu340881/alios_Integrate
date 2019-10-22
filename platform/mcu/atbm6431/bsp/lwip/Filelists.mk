#
# Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
# All rights reserved. 
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission. 
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
# SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
# OF SUCH DAMAGE.
#
# This file is part of the lwIP TCP/IP stack.
# 
# Author: Adam Dunkels <adam@sics.se>
#

# COREFILES, CORE4FILES: The minimum set of files needed for lwIP.
COREFILES=bsp/lwip/core/init.c \
	bsp/lwip/core/def.c \
	bsp/lwip/core/dns.c \
	bsp/lwip/core/inet_chksum.c \
	bsp/lwip/core/ip.c \
	bsp/lwip/core/mem.c \
	bsp/lwip/core/memp.c \
	bsp/lwip/core/netif.c \
	bsp/lwip/core/pbuf.c \
	bsp/lwip/core/raw.c \
	bsp/lwip/core/stats.c \
	bsp/lwip/core/sys.c \
	bsp/lwip/core/tcp.c \
	bsp/lwip/core/tcp_in.c \
	bsp/lwip/core/tcp_out.c \
	bsp/lwip/core/timeouts.c \
	bsp/lwip/core/udp.c

CORE4FILES=bsp/lwip/core/ipv4/autoip.c \
	bsp/lwip/core/ipv4/dhcp.c \
	bsp/lwip/core/ipv4/etharp.c \
	bsp/lwip/core/ipv4/icmp.c \
	bsp/lwip/core/ipv4/igmp.c \
	bsp/lwip/core/ipv4/ip4_frag.c \
	bsp/lwip/core/ipv4/ip4.c \
	bsp/lwip/core/ipv4/ip4_addr.c

CORE6FILES=bsp/lwip/core/ipv6/dhcp6.c \
	bsp/lwip/core/ipv6/ethip6.c \
	bsp/lwip/core/ipv6/icmp6.c \
	bsp/lwip/core/ipv6/inet6.c \
	bsp/lwip/core/ipv6/ip6.c \
	bsp/lwip/core/ipv6/ip6_addr.c \
	bsp/lwip/core/ipv6/ip6_frag.c \
	bsp/lwip/core/ipv6/mld6.c \
	bsp/lwip/core/ipv6/nd6.c

# APIFILES: The files which implement the sequential and socket APIs.
APIFILES=bsp/lwip/api/api_lib.c \
	bsp/lwip/api/api_msg.c \
	bsp/lwip/api/err.c \
	bsp/lwip/api/netbuf.c \
	bsp/lwip/api/netdb.c \
	bsp/lwip/api/netifapi.c \
	bsp/lwip/api/sockets.c \
	bsp/lwip/api/tcpip.c

# NETIFFILES: Files implementing various generic network interface functions
NETIFFILES=bsp/lwip/netif/ethernet.c \
	bsp/lwip/netif/slipif.c

# SIXLOWPAN: 6LoWPAN
SIXLOWPAN=bsp/lwip/netif/lowpan6.c \

# PPPFILES: PPP
PPPFILES=bsp/lwip/netif/ppp/auth.c \
	bsp/lwip/netif/ppp/ccp.c \
	bsp/lwip/netif/ppp/chap-md5.c \
	bsp/lwip/netif/ppp/chap_ms.c \
	bsp/lwip/netif/ppp/chap-new.c \
	bsp/lwip/netif/ppp/demand.c \
	bsp/lwip/netif/ppp/eap.c \
	bsp/lwip/netif/ppp/ecp.c \
	bsp/lwip/netif/ppp/eui64.c \
	bsp/lwip/netif/ppp/fsm.c \
	bsp/lwip/netif/ppp/ipcp.c \
	bsp/lwip/netif/ppp/ipv6cp.c \
	bsp/lwip/netif/ppp/lcp.c \
	bsp/lwip/netif/ppp/magic.c \
	bsp/lwip/netif/ppp/mppe.c \
	bsp/lwip/netif/ppp/multilink.c \
	bsp/lwip/netif/ppp/ppp.c \
	bsp/lwip/netif/ppp/pppapi.c \
	bsp/lwip/netif/ppp/pppcrypt.c \
	bsp/lwip/netif/ppp/pppoe.c \
	bsp/lwip/netif/ppp/pppol2tp.c \
	bsp/lwip/netif/ppp/pppos.c \
	bsp/lwip/netif/ppp/upap.c \
	bsp/lwip/netif/ppp/utils.c \
	bsp/lwip/netif/ppp/vj.c \
	bsp/lwip/netif/ppp/polarssl/arc4.c \
	bsp/lwip/netif/ppp/polarssl/des.c \
	bsp/lwip/netif/ppp/polarssl/md4.c \
	bsp/lwip/netif/ppp/polarssl/md5.c \
	bsp/lwip/netif/ppp/polarssl/sha1.c

# LWIPNOAPPSFILES: All LWIP files without apps
LWIPNOAPPSFILES=$(COREFILES) \
	$(CORE4FILES) \
	$(CORE6FILES) \
	$(APIFILES) \
	$(NETIFFILES) \
	$(PPPFILES) \
	$(SIXLOWPAN)

# SNMPFILES: SNMPv2c agent
SNMPFILES=bsp/lwip/apps/snmp/snmp_asn1.c \
	bsp/lwip/apps/snmp/snmp_core.c \
	bsp/lwip/apps/snmp/snmp_mib2.c \
	bsp/lwip/apps/snmp/snmp_mib2_icmp.c \
	bsp/lwip/apps/snmp/snmp_mib2_interfaces.c \
	bsp/lwip/apps/snmp/snmp_mib2_ip.c \
	bsp/lwip/apps/snmp/snmp_mib2_snmp.c \
	bsp/lwip/apps/snmp/snmp_mib2_system.c \
	bsp/lwip/apps/snmp/snmp_mib2_tcp.c \
	bsp/lwip/apps/snmp/snmp_mib2_udp.c \
	bsp/lwip/apps/snmp/snmp_msg.c \
	bsp/lwip/apps/snmp/snmpv3.c \
	bsp/lwip/apps/snmp/snmp_netconn.c \
	bsp/lwip/apps/snmp/snmp_pbuf_stream.c \
	bsp/lwip/apps/snmp/snmp_raw.c \
	bsp/lwip/apps/snmp/snmp_scalar.c \
	bsp/lwip/apps/snmp/snmp_table.c \
	bsp/lwip/apps/snmp/snmp_threadsync.c \
	bsp/lwip/apps/snmp/snmp_traps.c \
	bsp/lwip/apps/snmp/snmpv3_mbedtls.c \
	bsp/lwip/apps/snmp/snmpv3_dummy.c

# HTTPDFILES: HTTP server
HTTPDFILES=bsp/lwip/apps/httpd/fs.c \
	bsp/lwip/apps/httpd/httpd.c

# LWIPERFFILES: IPERF server
LWIPERFFILES=bsp/lwip/apps/lwiperf/lwiperf.c

# SNTPFILES: SNTP client
SNTPFILES=bsp/lwip/apps/sntp/sntp.c

# MDNSFILES: MDNS responder
MDNSFILES=bsp/lwip/apps/mdns/mdns.c

# NETBIOSNSFILES: NetBIOS name server
NETBIOSNSFILES=bsp/lwip/apps/netbiosns/netbiosns.c

# TFTPFILES: TFTP server files
TFTPFILES=bsp/lwip/apps/tftp/tftp_server.c \
    bsp/lwip/apps/tftp/tftp_common.c \
    bsp/lwip/apps/tftp/tftp_client.c \
    bsp/lwip/apps/tftp/tftp_ota.c



# LWIPAPPFILES: All LWIP APPs
LWIPAPPFILES=$(SNMPFILES) \
	$(HTTPDFILES) \
	$(LWIPERFFILES) \
	$(SNTPFILES) \
	$(MDNSFILES) \
	$(NETBIOSNSFILES) \
	$(TFTPFILES) 
