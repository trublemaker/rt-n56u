/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Abstract:


 */


#ifndef __AUTOCHSELECT_CMM_H__
#define __AUTOCHSELECT_CMM_H__

#define RSSI_TO_DBM_OFFSET 120 /* RSSI-115 = dBm */

/*Define for auto-channel state machine*/
#define AUTO_CH_SEL_MACHINE_BASE	                0
#define AUTO_CH_SEL_SCAN_IDLE                 		0
#define AUTO_CH_SEL_SCAN_LISTEN			            1
#define AUTO_CH_SEL_SCAN_MAX_STATE      	        2

#define AUTO_CH_SEL_SCAN_REQ				        0
#define AUTO_CH_SEL_SCAN_TIMEOUT			        1
#define AUTO_CH_SEL_SCAN_MAX_MSG			        2

#define AUTO_CH_SEL_SCAN_FUNC_SIZE                  (AUTO_CH_SEL_SCAN_MAX_STATE * AUTO_CH_SEL_SCAN_MAX_MSG)

typedef struct {
	ULONG dirtyness[MAX_NUM_OF_CHANNELS+1];
	ULONG ApCnt[MAX_NUM_OF_CHANNELS+1];
	UINT32 FalseCCA[MAX_NUM_OF_CHANNELS+1];
    BOOLEAN SkipList[MAX_NUM_OF_CHANNELS+1];
//#ifdef AP_QLOAD_SUPPORT
	UINT32 chanbusytime[MAX_NUM_OF_CHANNELS+1]; /* QLOAD ALARM */
//#endif /* AP_QLOAD_SUPPORT */
        BOOLEAN IsABand;
} CHANNELINFO, *PCHANNELINFO;

typedef struct {
	UCHAR Bssid[MAC_ADDR_LEN];
	UCHAR SsidLen;
	CHAR Ssid[MAX_LEN_OF_SSID];
	UCHAR Channel;
	UCHAR ExtChOffset;
	CHAR Rssi;
} BSSENTRY, *PBSSENTRY;

typedef struct {
	UCHAR BssNr;
	BSSENTRY BssEntry[MAX_LEN_OF_BSS_TABLE];	
} BSSINFO, *PBSSINFO;


typedef struct _AUTOCH_SEL_CH_LIST {

	UCHAR Channel;
	UCHAR Flags;
	UCHAR CentralChannel ;
	UCHAR Bw;
	UCHAR PhyMode;
	BOOLEAN BwCap;
	BOOLEAN SkipChannel;

/*
	Channel property:

	CHANNEL_DISABLED: The channel is disabled.
	CHANNEL_PASSIVE_SCAN: Only passive scanning is allowed.
	CHANNEL_NO_IBSS: IBSS is not allowed.
	CHANNEL_RADAR: Radar detection is required.
	CHANNEL_NO_FAT_ABOVE: Extension channel above this channel is not allowed.
	CHANNEL_NO_FAT_BELOW: Extension channel below this channel is not allowed.
	CHANNEL_40M_CAP: 40 BW channel group
	CHANNEL_80M_CAP: 80 BW channel group
 */
#define CHANNEL_DEFAULT_PROP		0x00
#define CHANNEL_DISABLED			0x01	/* no use */
#define CHANNEL_PASSIVE_SCAN		0x02
#define CHANNEL_NO_IBSS			0x04
#define CHANNEL_RADAR				0x08
#define CHANNEL_NO_FAT_ABOVE		0x10
#define CHANNEL_NO_FAT_BELOW		0x20
#define CHANNEL_40M_CAP			0x40
#define CHANNEL_80M_CAP			0x80
} AUTOCH_SEL_CH_LIST, *PAUTOCH_SEL_CH_LIST;

typedef struct _AUTOCH_SEL_CTRL{  
	struct wifi_dev *pScanReqwdev;
	CHAR ScanChIdx;    
	UCHAR ChannelListNum2G;
	UCHAR ChannelListNum5G;
	UCHAR ChannelListNum;
	UCHAR IsABand;
	UCHAR PhyMode;
	UINT32 AutoChannelFlag; /* Flag for auto-channel selection */
	RALINK_TIMER_STRUCT AutoChScanTimer;
	STATE_MACHINE AutoChScanStatMachine;
	STATE_MACHINE_FUNC AutoChScanFunc[AUTO_CH_SEL_SCAN_FUNC_SIZE];
	AUTOCH_SEL_CH_LIST AutoChSel2GChList[MAX_NUM_OF_CHANNELS+1];	/* List all supported channels for auto-channel selection in G-band*/
	AUTOCH_SEL_CH_LIST AutoChSel5GChList[MAX_NUM_OF_CHANNELS+1];	/* List all supported channels for auto-channel selection in A-band*/ 
	AUTOCH_SEL_CH_LIST AutoChSelChList[MAX_NUM_OF_CHANNELS+1];
}AUTOCH_SEL_CTRL, *PAUTOCH_SEL_CTRL;

#endif /* __AUTOCHSELECT_CMM_H__ */

