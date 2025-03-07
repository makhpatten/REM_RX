/* $Revision: 1.2 $ */
/* Copyright $Date: 2010/12/20 22:53:30 $ */
/*
 *******************************************************************************
 *
 * Copyright 1998-2010 NetBurner, Inc.  ALL RIGHTS RESERVED
 *
 *    Permission is hereby granted to purchasers of NetBurner Hardware to use or
 *    modify this computer program for any use as long as the resultant program
 *    is only executed on NetBurner provided hardware.
 *
 *    No other rights to use this program or its derivatives in part or in
 *    whole are granted.
 *
 *    It may be possible to license this or other NetBurner software for use on
 *    non-NetBurner Hardware. Contact sales@Netburner.com for more information.
 *
 *    NetBurner makes no representation or warranties with respect to the
 *    performance of this computer program, and specifically disclaims any
 *    responsibility for any damages, special or consequential, connected with
 *    the use of this program.
 *
 * NetBurner
 * 5405 Morehouse Dr.
 * San Diego, Ca. 92121
 * www.netburner.com
 *
 *******************************************************************************
 */

#include <snmp.h>

void CheckNVSettings();

extern const char FirmwareVersion[];

#ifndef SNMP
struct SysInfo {
	char SysContact[256];
	char SysName[256];
	char SysLocation[256];
	unsigned char ReadCommunity[40];
	unsigned char WriteCommunity[40];
	IPADDR trap_destination;
	DWORD trap_enable_flags;
	DWORD valid;
};
#endif

/* The structure that holds the configuration data */
struct NV_SettingsStruct {
	DWORD DataBaudRate;
	DWORD connect_idle_timeout;
	DWORD listen_idle_timeout;
	DWORD new_connection_timeout;
	DWORD connection_retry_timeout;
	char ConnectName[80];
	char DeviceName[40];
	IPADDR ConnectAddress;
	WORD ListenPort;
	WORD ConnectPort;
	BYTE Output_Bits;
	BYTE Output_Stop;
	BYTE Output_Parity;
	BYTE SerialMode;
	BYTE FlowMode;
	BYTE ConnectMode;
	WORD webpagePort;
	BYTE IP_Addr_mode;
	BYTE fill;
	char ConnectMessage[80];
	char ConnectLossMessage[80];
	SysInfo SysInfoData;
	WORD BreakInterval;
	BYTE BreakOnConnect;
	BYTE BreakKeyFlag;
	BYTE BreakKeyValue;
	char UserName[40];
	char Password[40];
	DWORD VerifyKey;
	DWORD txAddressVal;
	DWORD receivedTxMonoStereoMode;
	DWORD txPower;
	int txId[8];
	char RxName[100];
	long long unsigned int freq[8];
	WORD netburnerMakesRequests;
	DWORD PasswordTimeout;
	DWORD noReceiverDataTimeout;
};

/* Constants that go with Serial mode */
#define SERIAL_MODE_RS232_DATA_ON_PORT0    ( 1 )
#define SERIAL_MODE_RS232_DATA_ON_PORT1    ( 2 )
#define SERIAL_MODE_RS485H_DATA_ON_PORT1   ( 3 )
#define SERIAL_MODE_RS485F_DATA_ON_PORT1   ( 4 )

/* Constants that go with Serial FlowMode */
#define SERIAL_FLOW_MODE_NONE            ( 1 )
#define SERIAL_FLOW_MODE_XON_OFF         ( 2 )
#define SERIAL_FLOW_MODE_RTS_CTS         ( 3 )

/* Constants that go with ConnectMode */
#define SERIAL_CONNECT_CONNECT_NEVER (1)
#define SERIAL_CONNECT_CONNECT_AT_POWERUP (2)
#define SERIAL_CONNECT_CONNECT_WHEN_DATARX (3)

/* Constants that go with IP_Addr_mode */
#define IP_ADDR_MODE_DHCP (1)
#define IP_ADDR_MODE_STATIC (2)

extern NV_SettingsStruct NV_Settings;
extern volatile BOOL Settings_Changed;

void RegisterPost();
void CheckNVSettings();

