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

/*-------------------------------------------------------------------
 PROGRAM DESCRIPTION:
 This program is designed to connect an an  RS-232 device to the network.
 Data sent to the serial port will be sent out the network port; data
 sent in the network port will be sent out the serial port. All
 configuration is done via the web page interface.
 -------------------------------------------------------------------*/
#include "predef.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <startnet.h>
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include <ip.h>
#include <tcp.h>
#include <htmlfiles.h>
#include <http.h>
#include <autoupdate.h>
#include <system.h>
#include <dhcpclient.h>
#include <utils.h>
#include <string.h>
#include <dns.h>
#include <httppass.h>
#include "formtools.h"
#include "serialburnerdata.h"
#include "ReceiverControl.h"
#include "webformcode.h"
//#include "socket.h"

extern BOOL bShowDebug;

const char * ResetPassword = "RESETPASSWORD";
int singleScanFlag = 0;

#define dbprintf if (bShowDebug) iprintf

extern DhcpObject *pSB70LCDhcpObj;

/* Used to verify contents of non volatile storage */
#define VERIFY_KEY 0x4e545054
#define HARDCODE_RS232_SETTINGS 1 // set this to 1 to keep rs232 on desired port for ladybug control configuration
//----- Function Prototypes -----
extern "C" {
void IPMode(int sock, PCSTR url);
void DeviceAddress(int sock, PCSTR url);
void DisplayDeviceAddress(int sock, PCSTR url);
void DeviceMask(int sock, PCSTR url);
void DisplayDeviceMask(int sock, PCSTR url);
void DeviceGate(int sock, PCSTR url);
void DisplayDeviceGate(int sock, PCSTR url);
void DeviceDns(int sock, PCSTR url);
void DisplayDeviceDns(int sock, PCSTR url);
void DeviceName(int sock, PCSTR url);
void ListenCheck(int sock, PCSTR url);
void ListenPort(int sock, PCSTR url);
void ListenTimeout(int sock, PCSTR url);
void NetburnerStatusMsgs(int sock, PCSTR url);
void txAddressValue(int sock, PCSTR url);
void NewTimeout(int sock, PCSTR url);
void ConnMode(int sock, PCSTR url);
void ConnectPort(int sock, PCSTR url);
void ConnectName(int sock, PCSTR url);
void ConnectTimeout(int sock, PCSTR url);
void OutgoingTimeout(int sock, PCSTR url);
void PasswordTimeout(int sock, PCSTR url);
void noReceiverDataTimeout(int sock, PCSTR url);
void WebpagePort(int sock, PCSTR url);
void SerialPort(int sock, PCSTR url);
void DataBaudRate(int sock, PCSTR url);
void DataBits(int sock, PCSTR url);
void StopBits(int sock, PCSTR url);
void DataParity(int sock, PCSTR url);
void FlowMode(int sock, PCSTR url);
void SendMsgCheck(int sock, PCSTR url);
void SendMsgMsg(int sock, PCSTR url);
void SendLossMsgCheck(int sock, PCSTR url);
void SendLossMsgMsg(int sock, PCSTR url);
void SendBreakConnect(int sock, PCSTR url);
void SendBreakInterval(int sock, PCSTR url);
void SendBreakSerial(int sock, PCSTR url);
void HexBreakKey(int sock, PCSTR url);
void UserName(int sock, PCSTR url);
void Password(int sock, PCSTR url);
void RPassword(int sock, PCSTR url);
void DisplayFirmwareVersion(int sock, PCSTR url);
void DisplayReceiverFirmwareVersion(int sock, PCSTR url);

void DisplayWebPage(int sock, PCSTR url);
void JumperPage(int seek, PCSTR url);
void ChannelNum(int sock, PCSTR url);
void FilterNum(int sock, PCSTR url);
void Squelch(int sock, PCSTR url);
void MonoStereoMode(int sock, PCSTR url);
void TxMonoStereoMode(int sock, PCSTR url);
void chanOnOffRadioButtons(int tempChanNum);
void chan1OnOffRadioButtons(int sock, PCSTR url);
void chan2OnOffRadioButtons(int sock, PCSTR url);
void chan3OnOffRadioButtons(int sock, PCSTR url);
void chan4OnOffRadioButtons(int sock, PCSTR url);
void chan5OnOffRadioButtons(int sock, PCSTR url);
void chan6OnOffRadioButtons(int sock, PCSTR url);
void chan7OnOffRadioButtons(int sock, PCSTR url);
void chan8OnOffRadioButtons(int sock, PCSTR url);
void displaySerialNum(int sock, PCSTR url);
void receiverNameShow(int sock, PCSTR url);
void receiverNameEdit(int sock, PCSTR url);
void TxPower(int sock, PCSTR url);
void ControlPower(int sock, PCSTR url);
void rssiAshow(int sock, PCSTR url);
void rssiBshow(int sock, PCSTR url);
void vuLshow(int sock, PCSTR url);
void vuRshow(int sock, PCSTR url);
void TNLCheck(int sock, PCSTR url);
void CalToneCheck(int sock, PCSTR url);
void freqModeShow(int sock, PCSTR url);
void chan1TxIdShow(int sock, PCSTR url);
void chan2TxIdShow(int sock, PCSTR url);
void chan3TxIdShow(int sock, PCSTR url);
void chan4TxIdShow(int sock, PCSTR url);
void chan5TxIdShow(int sock, PCSTR url);
void chan6TxIdShow(int sock, PCSTR url);
void chan7TxIdShow(int sock, PCSTR url);
void chan8TxIdShow(int sock, PCSTR url);
void chan1TxIdEdit(int sock, PCSTR url);
void chan2TxIdEdit(int sock, PCSTR url);
void chan3TxIdEdit(int sock, PCSTR url);
void chan4TxIdEdit(int sock, PCSTR url);
void chan5TxIdEdit(int sock, PCSTR url);
void chan6TxIdEdit(int sock, PCSTR url);
void chan7TxIdEdit(int sock, PCSTR url);
void chan8TxIdEdit(int sock, PCSTR url);
void scanChannelsCheckbox(int sock, PCSTR url);
void displayConnectList(int sock, PCSTR url);
void chanTxIdShow(int sock, int chanNum);
void chanTxIdEdit(int sock, int chanNum);
void colorRowChan1(int sock, PCSTR url);
void colorRowChan2(int sock, PCSTR url);
void colorRowChan3(int sock, PCSTR url);
void colorRowChan4(int sock, PCSTR url);
void colorRowChan5(int sock, PCSTR url);
void colorRowChan6(int sock, PCSTR url);
void colorRowChan7(int sock, PCSTR url);
void colorRowChan8(int sock, PCSTR url);
void colorRowChan(int x);
void DisplayFreq1(int sock, PCSTR url);
void DisplayFreq2(int sock, PCSTR url);
void DisplayFreq3(int sock, PCSTR url);
void DisplayFreq4(int sock, PCSTR url);
void DisplayFreq5(int sock, PCSTR url);
void DisplayFreq6(int sock, PCSTR url);
void DisplayFreq7(int sock, PCSTR url);
void DisplayFreq8(int sock, PCSTR url);
}

/* Declare global variable */
NV_SettingsStruct NV_Settings;
volatile BOOL Settings_Changed;
int channelScanFlag;
char connectedListStr[CONNECTED_LIST_STR_LEN];
void fixPassword(char pass[]);

/*-------------------------------------------------------------------
 Check NV Settings.  Assign default values if VerifyKey is not
 valid.
 ------------------------------------------------------------------*/
void CheckNVSettings() {
	NV_SettingsStruct *pData = (NV_SettingsStruct *) GetUserParameters();
	NV_Settings = *pData;
	if (NV_Settings.VerifyKey != VERIFY_KEY) {
		NV_Settings.VerifyKey = VERIFY_KEY;
		NV_Settings.DataBaudRate = 115200;
		NV_Settings.Output_Bits = 8;
		NV_Settings.Output_Stop = 1;
		NV_Settings.Output_Parity = 1;
		NV_Settings.connect_idle_timeout = 60;
		NV_Settings.listen_idle_timeout = 60;
		NV_Settings.new_connection_timeout = 30;
		NV_Settings.connection_retry_timeout = 10;
		NV_Settings.webpagePort = 80;
		NV_Settings.ConnectName[0] = 0;
		siprintf(NV_Settings.DeviceName, "SB70LC-%02X%02X",
				gConfigRec.mac_address[4], gConfigRec.mac_address[5]);
		NV_Settings.ListenPort = 23;
		NV_Settings.ConnectPort = 0;
		NV_Settings.SerialMode = SERIAL_MODE_RS232_DATA_ON_PORT1;
		NV_Settings.FlowMode = SERIAL_FLOW_MODE_NONE;
		NV_Settings.ConnectMode = SERIAL_CONNECT_CONNECT_NEVER;
		if (gConfigRec.ip_Addr == 0) {
			NV_Settings.IP_Addr_mode = IP_ADDR_MODE_DHCP;
		} else {
			NV_Settings.IP_Addr_mode = IP_ADDR_MODE_STATIC;
		}

		NV_Settings.ConnectAddress = 0;
		NV_Settings.SysInfoData.SysContact[0] = 0;
		NV_Settings.SysInfoData.SysName[0] = 0;
		NV_Settings.SysInfoData.SysLocation[0] = 0;
		strcpy((char *) NV_Settings.SysInfoData.ReadCommunity, "public");
		strcpy((char *) NV_Settings.SysInfoData.WriteCommunity, "public");
		NV_Settings.SysInfoData.trap_destination = AsciiToIp("0.0.0.0");
		NV_Settings.SysInfoData.trap_enable_flags = 0;
		NV_Settings.SysInfoData.valid = 0x12345678;
		NV_Settings.ConnectMessage[0] = 0;
		NV_Settings.ConnectLossMessage[0] = 0;
		NV_Settings.BreakInterval = 20;
		NV_Settings.BreakOnConnect = 0;
		NV_Settings.BreakKeyFlag = 0;
		NV_Settings.BreakKeyValue = 0x02;
		NV_Settings.Password[0] = 0;
		NV_Settings.UserName[0] = 0;
		NV_Settings.txId[0] = 111;
		NV_Settings.txId[1] = 222;
		NV_Settings.txId[2] = 333;
		NV_Settings.txId[3] = 444;
		NV_Settings.txId[4] = 555;
		NV_Settings.txId[5] = 666;
		NV_Settings.txId[6] = 777;
		NV_Settings.txId[7] = 888;
		NV_Settings.freq[0] = (long long unsigned int) 0x054D6C;
		NV_Settings.freq[1] = (long long unsigned int) 0x05553C;
		NV_Settings.freq[2] = (long long unsigned int) 0x055D0C;
		NV_Settings.freq[3] = (long long unsigned int) 0x056800;
		NV_Settings.freq[4] = (long long unsigned int) 0x057800;
		NV_Settings.freq[5] = (long long unsigned int) 0x058800;
		NV_Settings.freq[6] = (long long unsigned int) 0x059800;
		NV_Settings.freq[7] = (long long unsigned int) 0x05A800;
		NV_Settings.receivedTxMonoStereoMode = 1; // initialize to stereo
		NV_Settings.netburnerMakesRequests = 1; // initialize to netburner makes requests
		NV_Settings.PasswordTimeout = 300;
		NV_Settings.noReceiverDataTimeout=4000;
		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
		dbprintf("Setting up new default storage info\r\n");
	}
	/*BACKDOOR*/

	if (strncmp(ResetPassword, (char *) gConfigRec.m_FileName, 13) == 0) {
		NV_Settings.Password[0] = 0;
		NV_Settings.UserName[0] = 0;
		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
		iprintf("\n\nPassword Reset\n");
	}
	iprintf("\nreceivedTxMonoStereoMode set to %d\n",
			NV_Settings.receivedTxMonoStereoMode);

	/* End of BACKDOOR*/

}

/* Declare the selection lists for the web page configuration */
const char *BaudList[] = { "115200", "57600", "38400", "19200", "9600", "4800",
		"2400", "1200", "\0" };
const char *StopList[] = { "1", "2", "\0" };
const char *BitList[] = { "8", "7", "6", "5", "\0" };
const char *ParityList[] = { "None", "Odd", "Even", "\0" };
const char *IPModeList[] = { "DHCP", "Static settings", "\0" };
const char *FreqDisplayList[] = { "DHCP", "Static settings", "\0" };

const char *SerialModeList[] = { "RS-232 on Port 0", "RS-232 on Port 1",
		"RS-485 Half-Duplex on Port 1", "RS-485 Full-Duplex on Port 1", "\0" };

const char *FlowModeList[] = { "None", "XON/XOFF", "RTS/CTS", "\0" };

const char *ConnectMode[] = { "Don't initiate connections",
		"Connect on powerup", "Connect when receiving serial data", "\0" };
/*
 const char *ChannelNumber[] = { "Channel 1: 362.496 MHz",
 "Channel 2: 366.592 MHz", "Channel 3: 370.688 MHz",
 "Channel 4: 374.784 MHz", "Channel 5: 378.880 MHz",
 "Channel 6: 382.976 MHz", "Channel 7: 387.072 MHz",
 "Channel 8: 391.168 MHz", "\0" };
 */
const char *ChannelNumber[] = { "Channel 1", "Channel 2", "Channel 3",
		"Channel 4", "Channel 5", "Channel 6", "Channel 7", "Channel 8", "\0" };

const char *FilterNumber[] =
		{ "Flat (No Filter)", "100 Hz", "200 Hz", "200 Hz Sharp",
				"200 Hz Sharper", "400 Hz Sharp", "400 Hz Sharper", "\0" };

const char *SquelchVal[] = { "Noise Reduce", "Extended Range", "\0" };

const char *MonoStereoModeVal[] = { "Stereo", "Mono", "\0" };

const char *TxMonoStereoModeVal[] =
		{ "Stereo", "Mono Left", "Mono Right", "\0" };

const char *TxPowerVal[] = { "Low", "High", "\0" };
const char *ControlPowerVal[] = { "Low", "High", "\0" };

char GetHexByte(const char *cp);

void IPMode(int sock, PCSTR url) {
	FormOutputSelect(sock, "IPMode", NV_Settings.IP_Addr_mode, IPModeList, 0);
}

void DeviceAddress(int sock, PCSTR url) {
	FormOutputIPInput(sock, "DeviceAddress", gConfigRec.ip_Addr);
}

void DisplayDeviceAddress(int sock, PCSTR url) {
	if (NV_Settings.IP_Addr_mode == IP_ADDR_MODE_DHCP
	)
		ShowIP2Sock(sock, EthernetIP);
}

void DeviceMask(int sock, PCSTR url) {
	FormOutputIPInput(sock, "DeviceMask", gConfigRec.ip_Mask);
}

void DisplayDeviceMask(int sock, PCSTR url) {
	if (NV_Settings.IP_Addr_mode == IP_ADDR_MODE_DHCP
	)
		ShowIP2Sock(sock, EthernetIpMask);
}

void DeviceGate(int sock, PCSTR url) {
	FormOutputIPInput(sock, "DeviceGate", gConfigRec.ip_GateWay);
}

void DisplayDeviceGate(int sock, PCSTR url) {
	if (NV_Settings.IP_Addr_mode == IP_ADDR_MODE_DHCP
	)
		ShowIP2Sock(sock, EthernetIpGate);
}

void DeviceDns(int sock, PCSTR url) {
	FormOutputIPInput(sock, "DeviceDns", gConfigRec.ip_DNS_server);
}
void DisplayDeviceDns(int sock, PCSTR url) {
	if (NV_Settings.IP_Addr_mode == IP_ADDR_MODE_DHCP
	)
		ShowIP2Sock(sock, EthernetDNS);
}

void DeviceName(int sock, PCSTR url) {
	FormOutputInput(sock, "DeviceName", 14, NV_Settings.DeviceName);
}

void ListenCheck(int sock, PCSTR url) {
	FormOutputCheckbox(sock, "ListenCheck", NV_Settings.ListenPort != 0, 0);
}

void DisplayFreq1(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq1Value", 10, NV_Settings.freq[0]);
}
void DisplayFreq2(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq2Value", 10, NV_Settings.freq[1]);
}
void DisplayFreq3(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq3Value", 10, NV_Settings.freq[2]);
}
void DisplayFreq4(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq4Value", 10, NV_Settings.freq[3]);
}
void DisplayFreq5(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq5Value", 10, NV_Settings.freq[4]);
}
void DisplayFreq6(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq6Value", 10, NV_Settings.freq[5]);
}
void DisplayFreq7(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq7Value", 10, NV_Settings.freq[6]);
}
void DisplayFreq8(int sock, PCSTR url) {
	FormOutputLongNumInput(sock, "freq8Value", 10, NV_Settings.freq[7]);
}

extern int receivedChannelNumber;
extern int receivedFilterNumber;
extern int receivedSquelchMode;
extern int receivedMonoStereoMode;
extern int receivedToneNoLock;
extern int receivedCalTone;

extern int controlPowerValue;

void TNLCheck(int sock, PCSTR url) {
//	FormOutputCheckbox(sock, "toneNoLock", receivedToneNoLock != 0, 1);
	writestring(sock, "<INPUT TYPE=\"checkbox\"");
	writestring(sock, " onclick=\"this.form.submit()\"");
	writestring(sock, " NAME=\"toneNoLock");
	if (receivedToneNoLock != 0) {
		writestring(sock, "\" VALUE=\"checked\" CHECKED>\n");
	} else {
		writestring(sock, "\" VALUE=\"checked\">\n");
	}

}

void CalToneCheck(int sock, PCSTR url) {
	writestring(sock, "<INPUT TYPE=\"checkbox\"");
	writestring(sock, " onclick=\"this.form.submit()\"");
	writestring(sock, " NAME=\"FScalTone");
	if (receivedCalTone != 0) {
		writestring(sock, "\" VALUE=\"checked\" CHECKED>\n");
	} else {
		writestring(sock, "\" VALUE=\"checked\">\n");
	}
}

void ListenPort(int sock, PCSTR url) {
	FormOutputNumInput(sock, "ListenPort", 5, NV_Settings.ListenPort);
}

void NetburnerStatusMsgs(int sock, PCSTR url) {
	FormOutputCheckbox(sock, "NetburnerStatusMsgs", NV_Settings.netburnerMakesRequests != 0, 0);
}

void WebpagePort(int sock, PCSTR url) {
	FormOutputNumInput(sock, "WebpagePort", 5, NV_Settings.webpagePort);
}

void ListenTimeout(int sock, PCSTR url) {
	FormOutputNumInput(sock, "ListenTimeout", 10,
			NV_Settings.listen_idle_timeout);
}

void txAddressValue(int sock, PCSTR url) {
	FormOutputNumInput(sock, "txAddressValue", 10, NV_Settings.txAddressVal);
}

void NewTimeout(int sock, PCSTR url) {
	FormOutputNumInput(sock, "NewTimeout", 10,
			NV_Settings.new_connection_timeout);
}

void ConnMode(int sock, PCSTR url) {
	FormOutputSelect(sock, "ConnMode", NV_Settings.ConnectMode, ConnectMode, 0);
}

void ChannelNum(int sock, PCSTR url) {
//	if (receivedChannelNumber == 0) {
//		   writestring( sock, "No received data" );
//	} else {
	FormOutputSelect(sock, "ChannelNum", receivedChannelNumber, //NV_Settings.ChannelNumber
			ChannelNumber, 1);
//	}
}

void FilterNum(int sock, PCSTR url) {
//	if (receivedChannelNumber == 0) {
//		   writestring( sock, "No received data" );
//	} else {
	int x;
	x = receivedFilterNumber;
	if (x == 5)
		x = 3;
	else if (x == 3)
		x = 5;
	if (x == 5)
		x = 4;
	else if (x == 4)
		x = 5;

	FormOutputSelect(sock, "FilterNum", x, FilterNumber, 1);
//	}
}

void Squelch(int sock, PCSTR url) {
//	if (receivedChannelNumber == 0) {
//		   writestring( sock, "No received data" );
//	} else {
	FormOutputSelect(sock, "Squelch", receivedSquelchMode, SquelchVal, 1);
//	}
}

void MonoStereoMode(int sock, PCSTR url) {
//	if (receivedChannelNumber == 0) {
//		   writestring( sock, "No received data" );
//	} else {
	FormOutputSelect(sock, "MonoStereoMode", receivedMonoStereoMode,
			MonoStereoModeVal, 1);
//	}
}

void TxMonoStereoMode(int sock, PCSTR url) {
	FormOutputSelect(sock, "TxMonoStereoMode",
			NV_Settings.receivedTxMonoStereoMode, TxMonoStereoModeVal, 1);
}

extern long unsigned int receiverSerialNum;
char bufForVuAndRssi[8000];
int chOnOff[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
void chan1OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(1);
	writestring(sock, bufForVuAndRssi);
}

void chan2OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(2);
	writestring(sock, bufForVuAndRssi);
}

void chan3OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(3);
	writestring(sock, bufForVuAndRssi);
}

void chan4OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(4);
	writestring(sock, bufForVuAndRssi);
}

void chan5OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(5);
	writestring(sock, bufForVuAndRssi);
}

void chan6OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(6);
	writestring(sock, bufForVuAndRssi);
}

void chan7OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(7);
	writestring(sock, bufForVuAndRssi);
}

void chan8OnOffRadioButtons(int sock, PCSTR url) {
	chanOnOffRadioButtons(8);
	writestring(sock, bufForVuAndRssi);
}

void chanOnOffRadioButtons(int tempChanNum) {
	siprintf(
			bufForVuAndRssi,
			"<input type=\"radio\" name=\"chOff\" value=\"\" onclick=\"this.form.submit()\"");
	if (chOnOff[tempChanNum - 1] == 0) {
		strcat(bufForVuAndRssi, " checked=\"checked\"");
	}
	strcat(
			bufForVuAndRssi,
			"><input type=\"radio\" name=\"chOn\" value=\"\" onclick=\"this.form.submit()\"");
	if (chOnOff[tempChanNum - 1] != 0) {
		strcat(bufForVuAndRssi, " checked=\"checked\"");
	}
	strcat(bufForVuAndRssi, ">");
}

void displaySerialNum(int sock, PCSTR url) {
	if (receiverSerialNum == 0) {
		siprintf(bufForVuAndRssi, " ");
	} else {
		siprintf(bufForVuAndRssi, "S/N %ld", receiverSerialNum);
	}
	writestring(sock, bufForVuAndRssi);

}

void receiverNameEdit(int sock, PCSTR url) {
	siprintf(
			bufForVuAndRssi,
			"<form action=\"writeRxName.HTM\" method=\"post\"><input type=\"text\" name=\"rxName\" value=\"%s\" maxlength=\"90\" size=\"50\"><input type=\"submit\" value=\"Enter\"></form>",
			NV_Settings.RxName);
	writestring(sock, bufForVuAndRssi);

}

void receiverNameShow(int sock, PCSTR url) {
	siprintf(bufForVuAndRssi, "%s", NV_Settings.RxName);
	writestring(sock, bufForVuAndRssi);

}

void TxPower(int sock, PCSTR url) {
	FormOutputSelect(sock, "TxPower", NV_Settings.txPower, TxPowerVal, 1);
}

void ControlPower(int sock, PCSTR url) {
	FormOutputSelect(sock, "ControlPower", controlPowerValue, ControlPowerVal,
			1);
}
void colorRowChan1(int sock, PCSTR url) {
	colorRowChan(1);
	writestring(sock, bufForVuAndRssi);
}

void colorRowChan2(int sock, PCSTR url) {
	colorRowChan(2);
	writestring(sock, bufForVuAndRssi);
}

void colorRowChan3(int sock, PCSTR url) {
	colorRowChan(3);
	writestring(sock, bufForVuAndRssi);
}

void colorRowChan4(int sock, PCSTR url) {
	colorRowChan(4);
	writestring(sock, bufForVuAndRssi);
}

void colorRowChan5(int sock, PCSTR url) {
	colorRowChan(5);
	writestring(sock, bufForVuAndRssi);
}

void colorRowChan6(int sock, PCSTR url) {
	colorRowChan(6);
	writestring(sock, bufForVuAndRssi);
}

void colorRowChan7(int sock, PCSTR url) {
	colorRowChan(7);
	writestring(sock, bufForVuAndRssi);
}

void colorRowChan8(int sock, PCSTR url) {
	colorRowChan(8);
	writestring(sock, bufForVuAndRssi);
}

extern int requestedChannelNumber;
void colorRowChan(int chanNumTemp) {
	siprintf(bufForVuAndRssi, "<tr ");
//	if (requestedChannelNumber == chanNumTemp && channelScanFlag == 0) {
//		strcat(bufForVuAndRssi, " bgcolor=\"blue\""); //#eff1f4
//	}
	strcat(bufForVuAndRssi, ">");
}

void ConnectPort(int sock, PCSTR url) {
	FormOutputNumInput(sock, "ConnectPort", 5, NV_Settings.ConnectPort);
}

void ConnectName(int sock, PCSTR url) {
	if (NV_Settings.ConnectName[0] == 0)
		FormOutputInput(sock, "connectName", 40,
				"(Enter server name or IP address)");
	else {
		FormOutputInput(sock, "connectName", 40, NV_Settings.ConnectName);

		if (NV_Settings.ConnectAddress != 0)
			ShowIP2Sock(sock, NV_Settings.ConnectAddress);
	}
}

void ConnectTimeout(int sock, PCSTR url) {
	FormOutputNumInput(sock, "ConnectTimeout", 10,
			NV_Settings.connect_idle_timeout);
}

void OutgoingTimeout(int sock, PCSTR url) {
	FormOutputNumInput(sock, "OutgoingTimeout", 10,
			NV_Settings.connection_retry_timeout);
}

void PasswordTimeout(int sock, PCSTR url) {
	FormOutputNumInput(sock, "PasswordTimeout", 10,
			NV_Settings.PasswordTimeout);
	if (NV_Settings.PasswordTimeout<30) {
		NV_Settings.PasswordTimeout=30;
	}
}

void noReceiverDataTimeout(int sock, PCSTR url) {
	FormOutputNumInput(sock, "noReceiverDataTimeout", 10,
			NV_Settings.noReceiverDataTimeout);
}

void SerialPort(int sock, PCSTR url) {
	FormOutputSelect(sock, "SerialPort", NV_Settings.SerialMode, SerialModeList,
			0);
}

void DataBaudRate(int sock, PCSTR url) {
	int nsel;

	switch (NV_Settings.DataBaudRate) {
	case 57600:
		nsel = 2;
		break;
	case 38400:
		nsel = 3;
		break;
	case 19200:
		nsel = 4;
		break;
	case 9600:
		nsel = 5;
		break;
	case 4800:
		nsel = 6;
		break;
	case 2400:
		nsel = 7;
		break;
	case 1200:
		nsel = 8;
		break;
	default: // 115200
		nsel = 1;
	}

	FormOutputSelect(sock, "DataBaudRate", nsel, BaudList, 0);
}

void DataBits(int sock, PCSTR url) {
	FormOutputSelect(sock, "DataBits", 9 - NV_Settings.Output_Bits, BitList, 0);
}

void StopBits(int sock, PCSTR url) {
	FormOutputSelect(sock, "StopBits", NV_Settings.Output_Stop, StopList, 0);
}

void DataParity(int sock, PCSTR url) {
	FormOutputSelect(sock, "DataParity", NV_Settings.Output_Parity, ParityList,
			0);
}

void FlowMode(int sock, PCSTR url) {
	FormOutputSelect(sock, "FlowMode", NV_Settings.FlowMode, FlowModeList, 0);
}

void SendMsgCheck(int sock, PCSTR url) {
	FormOutputCheckbox(sock, "SendMsgCheck", NV_Settings.ConnectMessage[0] != 0,
			0);
}

void SendMsgMsg(int sock, PCSTR url) {
	FormOutputInput(sock, "SendMsgMsg", 80, NV_Settings.ConnectMessage);
}

void SendLossMsgCheck(int sock, PCSTR url) {
	FormOutputCheckbox(sock, "SendLossMsgCheck",
			NV_Settings.ConnectLossMessage[0] != 0, 0);
}

void SendLossMsgMsg(int sock, PCSTR url) {
	FormOutputInput(sock, "SendLossMsgMsg", 80, NV_Settings.ConnectLossMessage);
}

void SendBreakConnect(int sock, PCSTR url) {
	FormOutputCheckbox(sock, "SendBreakConnect", NV_Settings.BreakOnConnect, 0);
}

void SendBreakInterval(int sock, PCSTR url) {
	FormOutputNumInput(sock, "SendBreakInterval", 10,
			NV_Settings.BreakInterval);
}

void SendBreakSerial(int sock, PCSTR url) {
	FormOutputCheckbox(sock, "SendBreakSerial", NV_Settings.BreakKeyFlag, 0);
}

void HexBreakKey(int sock, PCSTR url) {
	char tbuf[4];
	siprintf(tbuf, "%02X", NV_Settings.BreakKeyValue);
	FormOutputInput(sock, "HexBreakKey", 2, tbuf);
}

void UserName(int sock, PCSTR url) {
	FormOutputInput(sock, "UserName", 14, NV_Settings.UserName);
}

void Password(int sock, PCSTR url) {
	if (NV_Settings.Password[0])
		FormOutputInput(sock, "Password", 14, "**********");
	else
		FormOutputInput(sock, "Password", 14, "");
}

void RPassword(int sock, PCSTR url) {
	if (NV_Settings.Password[0])
		FormOutputInput(sock, "RPassword", 14, "**********");
	else
		FormOutputInput(sock, "RPassword", 14, "");
}

void DisplayFirmwareVersion(int sock, PCSTR url) {
	char buf[200];
	siprintf(buf, "Firmware Version: %s \n", FirmwareVersion);
	writestring(sock, buf);
}

extern char receiverFirmwareVersion[];

void DisplayReceiverFirmwareVersion(int sock, PCSTR url) {
	char buf[200];
	siprintf(buf, "Receiver Firmware Version: %s \n", receiverFirmwareVersion);
	writestring(sock, buf);
}

extern int rssiA, rssiB, vuL, vuR;
int lockA, lockB;
extern int dataReceivedFlag;

//void vuRshow(int sock, PCSTR url) {
//	vuShow(bufForVuAndRssi, vuR);
//	writestring(sock, bufForVuAndRssi);
//}

//void vuLshow(int sock, PCSTR url) {
//	vuShow(bufForVuAndRssi, vuL);
//	writestring(sock, bufForVuAndRssi);
//}

extern int receivedChannelNumber;
extern char statusByte1, statusByte2;

void freqModeShow(int sock, PCSTR url) {
	switch (receivedChannelNumber) {
	case 1:
		siprintf(bufForVuAndRssi, "Channel 1");
		break;
	case 2:
		siprintf(bufForVuAndRssi, "Channel 2");
		break;
	case 3:
		siprintf(bufForVuAndRssi, "Channel 3");
		break;
	case 4:
		siprintf(bufForVuAndRssi, "Channel 4");
		break;
	case 5:
		siprintf(bufForVuAndRssi, "Channel 5");
		break;
	case 6:
		siprintf(bufForVuAndRssi, "Channel 6");
		break;
	case 7:
		siprintf(bufForVuAndRssi, "Channel 7");
		break;
	case 8:
		siprintf(bufForVuAndRssi, "Channel 8");
		break;
	case 0:
	default:
		siprintf(bufForVuAndRssi, "Channel ?");
		break;
	}
	if ((statusByte1 & 0x40) == 0) {
		siprintf(bufForVuAndRssi, "%s Mono,", bufForVuAndRssi);
	} else {
		siprintf(bufForVuAndRssi, "%s Stereo,", bufForVuAndRssi);
	}
	if ((statusByte1 & 0x20) == 0) {
		siprintf(bufForVuAndRssi, "%s Noise Reduce,", bufForVuAndRssi);
	} else {
		siprintf(bufForVuAndRssi, "%s Extended Range,", bufForVuAndRssi);
	}
	if ((statusByte2 & 0x02) == 0) {
		siprintf(bufForVuAndRssi, "%s Tone No Lock Off", bufForVuAndRssi);
	} else {
		siprintf(bufForVuAndRssi, "%s Tone No Lock On", bufForVuAndRssi);
	}
	writestring(sock, bufForVuAndRssi);
}

void chan1TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 0);
}
void chan2TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 1);
}
void chan3TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 2);
}
void chan4TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 3);
}
void chan5TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 4);
}
void chan6TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 5);
}
void chan7TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 6);
}
void chan8TxIdShow(int sock, PCSTR url) {
	chanTxIdShow(sock, 7);
}

void chanTxIdShow(int sock, int chanNum) {
	siprintf(bufForVuAndRssi, "%d", NV_Settings.txId[chanNum]);
	writestring(sock, bufForVuAndRssi);
}

void chan1TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 0);
}
void chan2TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 1);
}
void chan3TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 2);
}
void chan4TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 3);
}
void chan5TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 4);
}
void chan6TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 5);
}
void chan7TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 6);
}
void chan8TxIdEdit(int sock, PCSTR url) {
	chanTxIdEdit(sock, 7);
}

void chanTxIdEdit(int sock, int chanNum) {
	siprintf(
			bufForVuAndRssi,
			"<form action=\"writeTxIdCh%d.HTM\" method=\"post\"><input type=\"text\" name=\"txId\" value=\"%d\" maxlength=\"3\" size=\"3\"><input type=\"submit\" value=\"Enter\"></form>",
			chanNum, NV_Settings.txId[chanNum]);
	writestring(sock, bufForVuAndRssi);
}

void displayConnectList(int sock, PCSTR url) {
//	struct sockaddr *address;
//    int *address_len;

	/* socklen_t */
	siprintf(bufForVuAndRssi, "%s", connectedListStr);
//	connectedListStr[0] = 0;
//	iprintf("\nReset connectedListStr\n");
	writestring(sock, bufForVuAndRssi);
}

void scanChannelsCheckbox(int sock, PCSTR url) {
	siprintf(
			bufForVuAndRssi,
			"<br><table><tr><td><form  action=\"writeChanSingleScanButton.HTM\" method=\"post\"><button type=\"button\" onclick=\"this.form.submit()\">Scan Channels</button></form></td>");
	strcat(
			bufForVuAndRssi,
			"<td width=\"10\"></td><td><form  action=\"writeChannelsScanCheckbox.HTM\" method=\"post\"><input type=\"checkbox\" name=\"chanScan\" value=\"chanScanValue\" onclick=\"this.form.submit()\"");
#if 1==1
	if (channelScanFlag == 1 && singleScanFlag == 0) {
		strcat(bufForVuAndRssi, " checked");
	}
#endif
	strcat(bufForVuAndRssi,
			">Continuous Channel Scan </form></td></tr></table>");
	writestring(sock, bufForVuAndRssi);
}

#define PORT_RESOURCE_232_AUX_IN  (1)
#define PORT_RESOURCE_232_AUX_OUT (2)

/* Function declaration to close down system before updating configuration */
void CloseEverything();
int TestPassword(const char *name, const char *passwd);

extern int FD_SerialData; /* The connected data socket */
int justToggledChanelsScanFlag = 0;

/*-------------------------------------------------------------------
 Handle HTTP Post
 ------------------------------------------------------------------*/
int allChannelsFlag[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // 0 means no data, 1 means not locked, 2 means locked
int savedChannel;
extern int LoggedIn;
DWORD LoggedInTimer;

int MyDoPost(int sock, char *url, char *pData, char *rxBuffer) {
	int msgLength;
	char outputMsg[10];
	int nwr;
	int i;
	int txChannel;
	long int tempAddr;
	char userName[40];
	char pass[40];
	char tempbuf[100];

//	iprintf("\nExecuting MyDoPost\n");
//    siprintf(tempbuf,"URL=%s\n",url);
//    iprintf(tempbuf);

	NV_SettingsStruct nvs;

//	NV_Settings.Password[0] = 0;

	/*

	 if (NV_Settings.Password[0]) {
	 if ((httpstricmp(url, "INDEX.HTM") == 1)||(httpstricmp(url, "SETUP.HTM") == 1)) {

	 char *pPass;
	 char *pUser;

	 if (!CheckAuthentication(url, &pPass, &pUser)) {
	 // No password was provided open
	 RequestAuthentication(sock, "Config Page");
	 return 1;
	 }
	 if (!TestPassword(pUser, pPass)) {
	 RequestAuthentication(sock, "Config Page");
	 return 1;
	 }
	 }
	 }
	 */

//	i = 0;
//	iprintf("\n\nStarting character string:");
//	while (pData[i] != 0) {
//			iprintf("%c", pData[i]);
//			i++;
//		}
//	iprintf(":End of character string\n");
//	iprintf("Calling URL: %s\n", url);
	//	if (strncmp("/writeTxOnMsgPost", url, 9) == 0) {
//		iprintf("Received ON mgs\n");
//		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
//		RedirectResponse(sock, "Index.HTM");
//	} else if (strncmp("/writeTxOffMsgPost", url, 9) == 0) {
//		iprintf("Received OFF mgs\n");
//		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
//		RedirectResponse(sock, "Index.HTM");
//	} else if (strncmp("/writeLBMsgPost", url, 5) == 0) {
//	iprintf("\n\nRequesting post %s\n",url);
	if (strncmp("/Logintimeout", url, 12) == 0) {
		LoggedIn = 0;
		RedirectResponse(sock, "login.htm");
		return 0;
	}
	if (strncmp("/Logout", url, 7) == 0) {
		LoggedIn = 0;
//		iprintf("\n\nRedirecting response...\n");
		RedirectResponse(sock, "login.htm");
		return 0;
	}
	if (strncmp("/writeNameP", url, 11) == 0) {

		pass[0] = 0;
		userName[0] = 0;
		ExtractPostData("UsrName", pData, userName, 40);
		ExtractPostData("Password", pData, pass, 40);

		fixPassword(pass);
		fixPassword(NV_Settings.Password);

//		iprintf("\n\ncomparing %s to %s\n",userName,NV_Settings.UserName);
		if (NV_Settings.Password[0] != 0
				&& (strncmp(userName, NV_Settings.UserName, 40) != 0)) {
			RedirectResponse(sock, "badpass.htm");
			return 0;
		}
//		iprintf("\n\ncomparing %s to %s\n",pass,NV_Settings.Password);
		if (NV_Settings.Password[0] != 0
				&& (strncmp(pass, NV_Settings.Password, 40) != 0)) {
			RedirectResponse(sock, "badpass.htm");
			return 0;
		}
		LoggedIn = 1;
		LoggedInTimer = GetPreciseTime(); // #0
//		iprintf("\nLoggedInTimer=%ld\n",LoggedInTimer);
		RedirectResponse(sock, "Channels_txid.HTM");
		return 0;
	}

	if (strncmp("/writeTxAddress", url, 9) == 0) {

		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				NV_Settings.txAddressVal = FormExtractNum("txAddressValue",
						pData, 123);
				//		iprintf("Tx address: %ld\n", NV_Settings.txAddressVal);
				//		NV_Settings.receivedTxMonoStereoMode = FormExtractSel(
				//				"TxMonoStereoMode", pData, TxMonoStereoModeVal, 1);
				//		iprintf("Tx Mode: %d\n", NV_Settings.receivedTxMonoStereoMode);
				//		NV_Settings.txPower = FormExtractSel("TxPower", pData, TxPowerVal, 1);
				//		iprintf("Tx Power: %d\n", NV_Settings.txPower);

				// temporarilly disabled save		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
				LoggedInTimer = GetPreciseTime(); // #0.5
//				iprintf("\nLoggedInTimer=%ld\n", LoggedInTimer);
				RedirectResponse(sock, "receiver.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "login.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);
	}
	if (strncmp("/writeChange", url, 12) == 0) {
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				channelScanFlag = 0;
				singleScanFlag = 0;
				justToggledChanelsScanFlag = 1;
				requestedChannelNumber = url[16] - '0';
				sendCSCommand();
				iprintf("\n*****************\nRequested channel: %d\n******************\n", requestedChannelNumber);
				LoggedInTimer = GetPreciseTime(); // #1
//				iprintf("\nLoggedInTimer=%ld\n", LoggedInTimer);
				RedirectResponse(sock, "channels_txid.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			channelScanFlag = 0;
			singleScanFlag = 0;
			justToggledChanelsScanFlag = 1;
//			requestedChannelNumber = url[16] - '0';
//			sendCSCommand();
			//		iprintf("\nRequested channel: %d\n", requestedChannelNumber);
			RedirectResponse(sock, "login.HTM");
		}
		return (0);

	}

	if (strncmp("/editTxid", url, 9) == 0) {
		RedirectResponse(sock, "channels_txid.htm");
		return (0);

	}

	if (strncmp("/allChannelsOff.HTM", url, 19) == 0
			|| strncmp("/allChannelsOffRx", url, 17) == 0) {
		outputMsg[0] = 'r';
		outputMsg[1] = 'x';\
// uaed for transmitter serial number 999
//		outputMsg[2] = 9;
//		outputMsg[2] <<= 4;
//		outputMsg[2] |= 9;
//		outputMsg[3] = 9;
//		outputMsg[3] <<= 4;

		outputMsg[2] = 0; // set serial number to 0
		outputMsg[3] = 0;

		outputMsg[4] = 0x09; // always 09
		outputMsg[5] = 0xbb; // set to stereo
		outputMsg[6] = 0xbb;
		outputMsg[7] = 0xbb;
		if (NV_Settings.txPower == 2) {
			outputMsg[7] &= 0xFC; // Mask out the lowest two bits to make it end with '8' for high power
		}
		outputMsg[8] = 0x0;
		outputMsg[9] = 0x0;
		if (controlPowerValue == 2) {
			outputMsg[9] |= 0x01;

		}
		msgLength = 12;
		int16 lCRC_value = Use_Crc((unsigned char*) outputMsg, (msgLength - 2));
		outputMsg[msgLength - 2] = (char) ((lCRC_value >> 8) & 0x0FF);
		outputMsg[msgLength - 1] = (char) (lCRC_value & 0x0FF);
		nwr = write(FD_SerialData, outputMsg, msgLength);
		// temporarilly disabled save		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));

		if (LoggedIn == 1) {
			if (strncmp("/allChannelsOffRx.HTM", url, 21) == 0) {
				RedirectResponse(sock, "receiver.htm");
			} else {
				RedirectResponse(sock, "channels_txid.htm");
			}
		} else {
			RedirectResponse(sock, "login.htm");
		}
		return (0);

	}

	if (strncmp("/showTxid", url, 9) == 0) {
		RedirectResponse(sock, "channels.htm");
		return (0);

	}
	if (strncmp("/writeChannelsScan", url, 12) == 0) {
#if 1==1
		if (channelScanFlag == 1) {
//			iprintf("\n\nChannels scan off\n\n");
			channelScanFlag = 0;
			singleScanFlag = 0;
		} else {
//			iprintf("\n\nChannels scan on\n\n");
			channelScanFlag = 1;
			singleScanFlag = 0;
		}
		justToggledChanelsScanFlag = 1;
		for (i = 0; i < 8; i++) {
			allChannelsFlag[i] = 0;
		}
#endif
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				RedirectResponse(sock, "channels_txid.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);

	}
	if (strncmp("/writeChanSingleSc", url, 12) == 0) {
#if 1==1
		savedChannel = receivedChannelNumber;
		singleScanFlag = 1;
		channelScanFlag = 1;

//		justToggledSingleScanFlag=1;
#endif
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				RedirectResponse(sock, "channels_txid.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);

	}
	if (strncmp("/writeControlPower", url, 12) == 0) {
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				controlPowerValue = FormExtractSel("ControlPower", pData,
						ControlPowerVal, 1);
				iprintf("\nControl power: %ld ", controlPowerValue);
				outputMsg[0] = 'r';
				outputMsg[1] = 'y';
				if (controlPowerValue == 2) {
					//			iprintf("high power\n");
					outputMsg[2] = 1; // high power
				} else {
					//			iprintf("low power\n");
					outputMsg[2] = 0; // low power
				}
				msgLength = 5;

				int16 lCRC_value = Use_Crc((unsigned char*) outputMsg,
						(msgLength - 2));
				outputMsg[msgLength - 2] = (char) ((lCRC_value >> 8) & 0x0FF);
				outputMsg[msgLength - 1] = (char) (lCRC_value & 0x0FF);
				nwr = write(FD_SerialData, outputMsg, msgLength);

				RedirectResponse(sock, "receiver.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);
	}

	if (strncmp("/writeTxMonoStereoMode", url, 12) == 0) {
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				NV_Settings.receivedTxMonoStereoMode = FormExtractSel(
						"TxMonoStereoMode", pData, TxMonoStereoModeVal, 1);
				iprintf("\nreceivedTxMonoStereoMode extracted as %d\n",
						NV_Settings.receivedTxMonoStereoMode);
				SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
				RedirectResponse(sock, "receiver.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);
	}
	if (strncmp("/writeTxPower", url, 12) == 0) {
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				NV_Settings.txPower = FormExtractSel("TxPower", pData,
						TxPowerVal, 1);
				//		siprintf(tempbuf,"\nTx power=%d\n",NV_Settings.txPower);
				//		iprintf(tempbuf);
				SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
				RedirectResponse(sock, "receiver.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);
	}

	if (strncmp("/writeLBMsgPost", url, 9) == 0) {
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				requestedChannelNumber = FormExtractSel("ChannelNum", pData,
						ChannelNumber, 1);
				receivedChannelNumber = requestedChannelNumber; // added this to keep receiver page from showing previous channel number (11-12-14 MPatten)
				receivedFilterNumber = FormExtractSel("FilterNum", pData,
						FilterNumber, 1);
				if (receivedFilterNumber == 5)
					receivedFilterNumber = 3;
				else if (receivedFilterNumber == 3)
					receivedFilterNumber = 5;

				if (receivedFilterNumber == 4)
					receivedFilterNumber = 3;
				else if (receivedFilterNumber == 3)
					receivedFilterNumber = 4;

				if (receivedFilterNumber == 1)
					receivedFilterNumber = 0;

				receivedSquelchMode = FormExtractSel("Squelch", pData,
						SquelchVal, 1);

				receivedMonoStereoMode = FormExtractSel("MonoStereoMode", pData,
						MonoStereoModeVal, 1);
				if (FormExtractCheck("toneNoLock", pData, TRUE) == FALSE) {
					receivedToneNoLock = 0;
					iprintf("\nDidnt find TNL check\n");
				} else {
					iprintf("\nFound TNL check\n");
					receivedToneNoLock = 1;
				}
				if (FormExtractCheck("FScalTone", pData, TRUE) == FALSE) {
					//			iprintf("\nDidnt find cal tone check\n");
					receivedCalTone = 0;
				} else {
					//			iprintf("\nFound cal tone check!\n");
					receivedCalTone = 1;
				}
				iprintf("\nsending tone no lock=%d", receivedToneNoLock);
				iprintf("\nsending cal tone=%d\n", receivedCalTone);

				sendCSCommand();

				RedirectResponse(sock, "receiver.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);
	}
	if ((strncmp("/writeTxId", url, 10) == 0)) {
		if (LoggedIn == 1) {
			if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) {
				i = url[12] - '0';
				NV_Settings.txId[i] = FormExtractNum("txId", pData, 246);
				SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
				LoggedInTimer = GetPreciseTime(); // #2
//				iprintf("\nLoggedInTimer=%ld\n", LoggedInTimer);
				RedirectResponse(sock, "channels_txid.HTM");
			} else {
				LoggedIn = 0;
				RedirectResponse(sock, "logintimeout.HTM");
			}
		} else {
			RedirectResponse(sock, "login.HTM");
		}
		return (0);
	}

	if ((strncmp("/writeRxNa", url, 10) == 0)) {

		i = 0;
		int stopFlag = 0;
		while (i < 90 && stopFlag == 0) {
			NV_Settings.RxName[i] = pData[i + 7];
			if (NV_Settings.RxName[i] == 0) {
				stopFlag = 1;
			}
			if (NV_Settings.RxName[i] == '+') {
				NV_Settings.RxName[i] = ' ';
			}
			i++;
		}

		i = 0;
// 		iprintf("\n\n");
//	while (i<20) {
//		iprintf("NV_Settings.RxName[%d]=%d , %c\n",i,pData[i],pData[i]);
//		i++;
//	}
//	iprintf("NV_Settings.RxName[%d]=%d , %c\n",i,pData[i],pData[i]);
//		iprintf("\n\n");

		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
		LoggedInTimer = GetPreciseTime();
		RedirectResponse(sock, "channels_txid.HTM");
		return (0);
	}

	if ((strncmp("/writeTxO", url, 9) == 0)) {
//		iprintf("\nGot write tx message...\n");
		outputMsg[0] = 'r';
		outputMsg[1] = 'x';
		if (strncmp("/writeTxOnOffRa", url, 15) == 0) {

			txChannel = url[22] - '0';
			tempAddr = NV_Settings.txId[txChannel - 1];
		} else {
			txChannel = receivedChannelNumber;
			tempAddr = NV_Settings.txAddressVal;
		}
//		iprintf("\n\ntempaddr=%ld\n",tempAddr);
//		iprintf("\n\ntempaddr=%ld\n",tempAddr);
		int tempAddr1 = (int) (tempAddr / 100);
		if (tempAddr1 > 9)
			tempAddr1 = 9;
		if (tempAddr1 < 0)
			tempAddr1 = 0;
//		iprintf("tempaddr1=%d\n",tempAddr1);
		int tempAddr2 = (int) ((tempAddr - (100 * tempAddr1)) / 10);
		if (tempAddr2 > 9)
			tempAddr2 = 9;
		if (tempAddr2 < 0)
			tempAddr2 = 0;
//		iprintf("tempaddr2=%d\n",tempAddr2);
		int tempAddr3 = (int) (tempAddr - (100 * (long int) tempAddr1)
				- (10 * tempAddr2));
		if (tempAddr3 > 9)
			tempAddr3 = 9;
		if (tempAddr3 < 0)
			tempAddr3 = 0;
//		iprintf("tempaddr3=%d\n",tempAddr3);
		outputMsg[2] = tempAddr3;
		outputMsg[2] <<= 4;
		outputMsg[2] |= tempAddr2;
		outputMsg[3] = tempAddr1;
		outputMsg[3] <<= 4;
		outputMsg[4] = 0x09; // always 09
		if (NV_Settings.receivedTxMonoStereoMode == 1) {
			outputMsg[5] = 0xbb; // set to stereo
			iprintf("\nTransmitting set to stereo\n");
		} else if (NV_Settings.receivedTxMonoStereoMode == 2) {
			outputMsg[5] = 0x8b; // set to mono left
			iprintf("\nTransmitting set to mono left\n");
		} else {
			outputMsg[5] = 0xb8; // set to mono right No Received Data
			iprintf("\nTransmitting set to mono right\n");
		}
		switch (txChannel) {
		case 1:
			outputMsg[6] = 0xbb;
			outputMsg[7] = 0xbb;
			break;
		case 2:
			outputMsg[6] = 0x8b;
			outputMsg[7] = 0xbb;
			break;
		case 3:
			outputMsg[6] = 0xb8;
			outputMsg[7] = 0xbb;
			break;
		case 4:
			outputMsg[6] = 0x88;
			outputMsg[7] = 0xbb;
			break;
		case 5:
			outputMsg[6] = 0xbb;
			outputMsg[7] = 0x8b;
			break;
		case 6:
			outputMsg[6] = 0x8b;
			outputMsg[7] = 0x8b;
			break;
		case 7:
			outputMsg[6] = 0xb8;
			outputMsg[7] = 0x8b;
			break;
		case 8:
		default:
			outputMsg[6] = 0x88;
			outputMsg[7] = 0x8b;
			break;
		}
		if (NV_Settings.txPower == 2) {
			outputMsg[7] &= 0xFC; // Mask out the lowest two bits to make it end with '8' for high power
		}
		outputMsg[8] = 0x0;
		outputMsg[9] = 0x0;
		if (strncmp("/writeTxOnMsgPost", url, 12) == 0) {
			outputMsg[9] = 0x08;
//			iprintf("\n\nSending on msg from receiver screen\n");
		} else if (strncmp("/writeTxOffMsgPost", url, 12) == 0) {
			outputMsg[9] = 0x00;
//			iprintf("\n\nSending off msg from receiver screen\n");
		} else if (strncmp("/writeTxOnOffRa", url, 15) == 0) {
//			i = 0;
//			iprintf("\n\nStarting character string:");
//			while (pData[i] != 0) {
//				iprintf("%c", pData[i]);
//				i++;
//			}
//			iprintf(":End of character string\n");
			if (strlen(pData) > 8) {
				if (chOnOff[txChannel - 1] == 0) {
					chOnOff[txChannel - 1] = 1;
					outputMsg[9] = 0x08;
//					iprintf("\n\nSending on msg from radio button\n");
				} else {
					chOnOff[txChannel - 1] = 0;
					outputMsg[9] = 0x00;
//					iprintf("\n\nSending off msg from radio button\n");
				}
			} else if (strncmp("chOn", pData, 4) == 0) {
				chOnOff[txChannel - 1] = 1;
				outputMsg[9] = 0x08;
//				iprintf("\n\nSending on msg from radio button\n");
			} else {
				chOnOff[txChannel - 1] = 0;
				outputMsg[9] = 0x00;
//				iprintf("\n\nSending off msg from radio button\n");
			}
		}
//		iprintf("transmitter power: %d\n", NV_Settings.txPower);
		if (controlPowerValue == 2) {
//			iprintf("Setting Tx to high power\n");
			outputMsg[9] |= 0x01;

		}
//		iprintf("OutputMsg[0] ended up as %d\n",outputMsg[0]);
//		iprintf("OutputMsg[1] ended up as %d\n",outputMsg[1]);
//		iprintf("OutputMsg[2] ended up as %d\n",outputMsg[2]);
//		iprintf("OutputMsg[3] ended up as %d\n",outputMsg[3]);
//		iprintf("OutputMsg[4] ended up as %d\n",outputMsg[4]);
//		iprintf("OutputMsg[5] ended up as %d\n",outputMsg[5]);
//		iprintf("OutputMsg[6] ended up as %d\n",outputMsg[6]);
//		iprintf("OutputMsg[7] ended up as %d\n",outputMsg[7]);
//		iprintf("OutputMsg[8] ended up as %d\n",outputMsg[8]);
//		iprintf("OutputMsg[9] ended up as %d\n",outputMsg[9]);
		msgLength = 12;
		int16 lCRC_value = Use_Crc((unsigned char*) outputMsg, (msgLength - 2));
		outputMsg[msgLength - 2] = (char) ((lCRC_value >> 8) & 0x0FF);
		outputMsg[msgLength - 1] = (char) (lCRC_value & 0x0FF);
		nwr = write(FD_SerialData, outputMsg, msgLength);
		// temporarilly disabled save		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
		if (strncmp("/writeTxOnOffRa", url, 15) == 0) {
			if (LoggedIn == 1) {
				if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
						|| NV_Settings.Password[0] == 0) {
					LoggedInTimer = GetPreciseTime(); // #3
//					iprintf("\nLoggedInTimer=%ld\n", LoggedInTimer);
					RedirectResponse(sock, "channels_txid.HTM");
// why were these following lines ever here?
//					LoggedIn = 0;
//					RedirectResponse(sock, "logintimeout.HTM");
				} else {
					LoggedIn = 0;
					RedirectResponse(sock, "logintimeout.HTM");
				}
			} else {
				RedirectResponse(sock, "channels.HTM");
			}
		} else {
			if (LoggedIn == 1) {
				if (GetPreciseTime() - LoggedInTimer < NV_Settings.PasswordTimeout * 1000000
						|| NV_Settings.Password[0] == 0) {
					LoggedInTimer = GetPreciseTime(); // #4
//					iprintf("\nLoggedInTimer=%ld\n", LoggedInTimer);

					RedirectResponse(sock, "receiver.HTM");
				} else {
					LoggedIn = 0;
					RedirectResponse(sock, "logintimeout.HTM");
				}
			} else {
				RedirectResponse(sock, "login.HTM");
			}
		}
		return (0);

	}
	if ((strncmp("/ajax", url, 5) == 0)) {
		return (0); //*oldhand)(sock, url, rxBuffer);
	}

	if ((strncmp("/formpost.htm", url, 13) == 0)) {
		iprintf("\n\nClose everything, %s\n", url);
		iprintf("Form: %s\n", pData);
		CloseEverything();
		Settings_Changed = TRUE;
		char pass1[40];
		char pass2[40];

		pass1[0] = 0;
		pass2[0] = 0;

		ExtractPostData("Password", pData, pass1, 40);
		ExtractPostData("RPassword", pData, pass2, 40);

		/* Make a copy of NV_Settings to make changes to */
		nvs = NV_Settings;

		nvs.VerifyKey = VERIFY_KEY;

//			ExtractPostData("Password", pData, nvs.Password, 40);
		ExtractPostData("UserName", pData, nvs.UserName, 40);

		if (strncmp(pass1, "**********", 40) != 0) {
			fixPassword(pass1);
			fixPassword(pass2);

			if (strncmp(pass1, pass2, 40) != 0) {
				RedirectResponse(sock, "badpass.htm");
				return 0;
			}
			iprintf("\nPasswords matched...setting user name and password\n");
			strcpy(nvs.Password, pass1);

		} else {
			iprintf("\nPassword not changed\n");
		}

		iprintf("\n\nUser name: <%s>\n", nvs.UserName);
		iprintf(" Password: <%s>\n", nvs.Password);

		/* Extract all of the data from the post */
#if HARDCODE_RS232_SETTINGS==0
		nvs.DataBaudRate = FormExtractNum("DataBaudRate", pData, 115200);
		nvs.Output_Bits = FormExtractNum("DataBits", pData, 8);
		nvs.Output_Stop = FormExtractNum("StopBits", pData, 1);
		nvs.Output_Parity = FormExtractSel("DataParity", pData, ParityList, 1);
#else
		nvs.DataBaudRate = 9600;
		nvs.Output_Bits = 8;
		nvs.Output_Stop = 1;
		nvs.Output_Parity = 1;
#endif

		if (FormExtractCheck("SendMsgCheck", pData, FALSE) == FALSE) {
			nvs.ConnectMessage[0] = 0;
		} else {
			if (ExtractPostData("SendMsgMsg", pData, nvs.ConnectMessage, 80)
					<= 0)
				dbprintf("Error in ExtractPostData() for SendMsgMsg\r\n");
		}

		if (FormExtractCheck("SendLossMsgCheck", pData, FALSE) == FALSE) {
			nvs.ConnectLossMessage[0] = 0;
		} else {
			if (ExtractPostData("SendLossMsgMsg", pData, nvs.ConnectLossMessage,
					80) <= 0)
				dbprintf("Error in ExtractPostData() for SendLossMsgMsg\r\n");
		}

		nvs.connect_idle_timeout = FormExtractNum("ConnectTimeout", pData, 60);
		nvs.listen_idle_timeout = FormExtractNum("ListenTimeout", pData, 60);
		nvs.new_connection_timeout = FormExtractNum("NewTimeout", pData, 30);
		nvs.connection_retry_timeout = FormExtractNum("OutgoingTimeout", pData,
				600);

		nvs.PasswordTimeout = FormExtractNum("PasswordTimeout", pData, 23);
		nvs.noReceiverDataTimeout = FormExtractNum("noReceiverDataTimeout", pData, 60);
		nvs.ListenPort = FormExtractNum("ListenPort", pData, 23);
		nvs.webpagePort = FormExtractNum("WebpagePort", pData, 23);
		nvs.ConnectPort = FormExtractNum("ConnectPort", pData, 23);
		nvs.ConnectMode = FormExtractSel("ConnMode", pData, ConnectMode, 1);
		nvs.IP_Addr_mode = FormExtractSel("IPMode", pData, IPModeList, 1);
#if HARDCODE_RS232_SETTINGS==0
		nvs.SerialMode = FormExtractSel("SerialPort", pData, SerialModeList, 1);
		nvs.FlowMode = FormExtractSel("FlowMode", pData, FlowModeList, 1);
#else
		nvs.SerialMode = 2;
		nvs.FlowMode = 1;
#endif

		if (FormExtractCheck("ListenCheck", pData, TRUE) == FALSE) {
			nvs.ListenPort = 0;
		}

		nvs.netburnerMakesRequests=1;
		if (FormExtractCheck("NetburnerStatusMsgs", pData, TRUE) == FALSE) {
			nvs.netburnerMakesRequests = 0;
		}

		if (ExtractPostData("connectName", pData, nvs.ConnectName, 80) <= 0) {
			dbprintf("Error in ExtractPostData() for connectName\r\n");
		}

		if (ExtractPostData("DeviceName", pData, nvs.DeviceName, 14) <= 0) {
			dbprintf("Error in ExtractPostData() for DeviceName\r\n");
		}

		nvs.ConnectAddress = 0;

		char tbuf[4];

		if (ExtractPostData("HexBreakKey", pData, tbuf, 3) <= 0) {
			dbprintf("Error in ExtractPostData() for HexBreakKey\r\n");
		}

		nvs.BreakKeyValue = GetHexByte(tbuf);
		nvs.BreakOnConnect = FormExtractCheck("SendBreakConnect", pData, FALSE);
		nvs.BreakKeyFlag = FormExtractCheck("SendBreakSerial", pData, FALSE);
		nvs.BreakInterval = FormExtractNum("SendBreakInterval", pData, 20);

		IPADDR new_addr = FormExtractIP("DeviceAddress", pData,
				gConfigRec.ip_Addr);
		IPADDR new_mask = FormExtractIP("DeviceMask", pData,
				gConfigRec.ip_Mask);
		IPADDR new_gate = FormExtractIP("DeviceGate", pData,
				gConfigRec.ip_GateWay);
		IPADDR new_dns = FormExtractIP("DeviceDns", pData,
				gConfigRec.ip_DNS_server);

		/* Save the old configuration mode */
		BYTE old_IP_Mode = NV_Settings.IP_Addr_mode;

		/* If the serial port changed, change it */
		if (nvs.SerialMode != NV_Settings.SerialMode) {
			BYTE port = 0;
#if HARDCODE_RS232_SETTINGS==0
			if (nvs.SerialMode == SERIAL_MODE_RS232_DATA_ON_PORT0) {
				port = 1;
			}
#endif

			if (gConfigRec.ser_boot != port) {
				ConfigRecord new_rec;
				memcpy(&new_rec, &gConfigRec, sizeof(new_rec));
				new_rec.ser_boot = port;
//				iprintf("\n\nupdate number 3");
				UpdateConfigRecord(&new_rec);
			}
		}

		/* Check to see ifwe need to renegotiate DHCP because name changed */
		BOOL name_Changed = FALSE;
		if ((nvs.IP_Addr_mode == IP_ADDR_MODE_DHCP)
				&& (strcmp(nvs.DeviceName, NV_Settings.DeviceName) != 0)) {
			name_Changed = TRUE;
		}

		/* Save the new settings */
		NV_Settings = nvs;
		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));

		if ((nvs.IP_Addr_mode == IP_ADDR_MODE_DHCP)
				&& (old_IP_Mode != IP_ADDR_MODE_DHCP)) {
			/* We have switched from static to DHCP mode */
			/* First save the old IP address information */
			IPADDR OldEthernetIP = EthernetIP;
			IPADDR OldEthernetIpMask = EthernetIpMask;
			IPADDR OldEthernetIpGate = EthernetIpGate;
			IPADDR OldEthernetDNS = EthernetDNS;

			/* Now negotiate a DHCP address */
			pDHCPOfferName = NV_Settings.DeviceName;
			pSB70LCDhcpObj->StartDHCP(); //Start DHCP
			if (OSSemPend(&pSB70LCDhcpObj->NotifySem, 10 * TICKS_PER_SECOND)
					== OS_TIMEOUT) //Wait 10 seconds
					{
				//DHCP did not initialize handle the error here
				dbprintf(
							"DHCP failed to initialize, system has IP address of Zero\r\n");
			} else {
				/* We have a new dhcp address */

				/* Save this new DHCP address */
				IPADDR DHCP_EthernetIP = EthernetIP;
				IPADDR DHCP_EthernetIpMask = EthernetIpMask;
				IPADDR DHCP_EthernetIpGate = EthernetIpGate;
				IPADDR DHCP_EthernetDNS = EthernetDNS;

				/* Set the address back to the old address */EthernetIP =
						OldEthernetIP;
				EthernetIpMask = OldEthernetIpMask;
				EthernetIpGate = OldEthernetIpGate;
				EthernetDNS = OldEthernetDNS;

				OSTimeDly(TICKS_PER_SECOND * 2); /* Let Stuff clear in Network land */

				/* Write out a web page redirect so the web browser will go to the new address */
				char buf[80];
				ConfigRecord new_rec;
				memcpy(&new_rec, &gConfigRec, sizeof(new_rec));

				/* Redirect to new address */
				PBYTE ipb = (PBYTE) &DHCP_EthernetIP;
				siprintf(buf, "http://%d.%d.%d.%d", (int) ipb[0], (int) ipb[1],
						(int) ipb[2], (int) ipb[3]);
				RedirectResponse(sock, buf);
				close(sock);

				OSTimeDly(TICKS_PER_SECOND * 2); /* Let Stuff clear in Network land */

				/* Now that the web page stuff is done reset the addresses to the new addresses */

				EthernetIP = DHCP_EthernetIP;
				EthernetIpMask = DHCP_EthernetIpMask;
				EthernetIpGate = DHCP_EthernetIpGate;
				EthernetDNS = DHCP_EthernetDNS;
				new_rec.ip_Addr = 0;
				new_rec.ip_Mask = 0;
				new_rec.ip_GateWay = 0;
				new_rec.ip_DNS_server = 0;
//				iprintf("\n\nupdate number 1\n\n");
				UpdateConfigRecord(&new_rec); /* Save the IP config record */

				EthernetIP = DHCP_EthernetIP;
				EthernetIpMask = DHCP_EthernetIpMask;
				EthernetIpGate = DHCP_EthernetIpGate;
				EthernetDNS = DHCP_EthernetDNS;

				return 0;
			}
		}/* End of switch from static to dhcp */
		else if (nvs.IP_Addr_mode == IP_ADDR_MODE_STATIC) {
			/* Just switched from DHCP to static address or changed satatic address */
			if ((new_addr != gConfigRec.ip_Addr)
					|| (new_mask != gConfigRec.ip_Mask)
					|| (new_gate != gConfigRec.ip_GateWay)
					|| (new_dns != gConfigRec.ip_DNS_server)
					|| (old_IP_Mode != IP_ADDR_MODE_STATIC)) {
				/* Redirect to new address */
				char buf[80];
				ConfigRecord new_rec;
				memcpy(&new_rec, &gConfigRec, sizeof(new_rec));
				PBYTE ipb = (PBYTE) &new_addr;
				siprintf(buf, "http://%d.%d.%d.%d", (int) ipb[0], (int) ipb[1],
						(int) ipb[2], (int) ipb[3]);
				RedirectResponse(sock, buf);
				close(sock);

				OSTimeDly(TICKS_PER_SECOND * 2); /* Let Stuff clear in Network land */

				/* Set up new address */EthernetIP = new_addr;
				EthernetIpMask = new_mask;
				EthernetIpGate = new_gate;
				EthernetDNS = new_dns;

				new_rec.ip_Addr = new_addr;
				new_rec.ip_Mask = new_mask;
				new_rec.ip_GateWay = new_gate;
				new_rec.ip_DNS_server = new_dns;

//				iprintf("\n\nupdate number 2\n\n");
				UpdateConfigRecord(&new_rec);
				if (old_IP_Mode != IP_ADDR_MODE_STATIC) {
					pSB70LCDhcpObj->StopDHCP();
					/* Must restore addresses after killing DHCP */EthernetIP =
							new_addr;
					EthernetIpMask = new_mask;
					EthernetIpGate = new_gate;
					EthernetDNS = new_dns;
				}
				return 0;
			}
		} else if (name_Changed) {
			/* We must redo dhcp to register new changed name */
			pSB70LCDhcpObj->StopDHCP();
			pDHCPOfferName = NV_Settings.DeviceName;
			pSB70LCDhcpObj->StartDHCP(); //Start DHCP
			OSSemPend(&pSB70LCDhcpObj->NotifySem, 10 * TICKS_PER_SECOND);
		}

		RedirectResponse(sock, "Setup.HTM");
		return 0;
	}
	if ((strncmp("/freqpost.htm", url, 13) == 0)) {
		iprintf("\n\nClose everything, %s\n", url);
		iprintf("Form: %s\n", pData);
		CloseEverything();
		Settings_Changed = TRUE;

		/* Make a copy of NV_Settings to make changes to */
		nvs = NV_Settings;

		nvs.VerifyKey = VERIFY_KEY;

		/* Extract all of the data from the post */

		nvs.freq[0] = FormExtractNum("freq1Value", pData, 60);
		nvs.freq[1] = FormExtractNum("freq2Value", pData, 60);
		nvs.freq[2] = FormExtractNum("freq3Value", pData, 60);
		nvs.freq[3] = FormExtractNum("freq4Value", pData, 60);
		nvs.freq[4] = FormExtractNum("freq5Value", pData, 60);
		nvs.freq[5] = FormExtractNum("freq6Value", pData, 60);
		nvs.freq[6] = FormExtractNum("freq7Value", pData, 60);
		nvs.freq[7] = FormExtractNum("freq8Value", pData, 60);
		NV_Settings = nvs;
		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
		RedirectResponse(sock, "freqSetup.HTM");
		return 0;
	}
	return 0;
}

char *upperCase(char x[]);
char *upperCase(char x[]) {
	int i;
	i=0;
	while (x[i]!=0) {
	   x[i]=toupper(x[i]);
	   i++;
	}
	return(x);
}

static http_gethandler *oldhand;
extern int chanLock[];
int LoggedIn = 0;

int MyDoGet(int sock, PSTR url, PSTR rxBuffer) {
	int i;

//	if (url[0]!='a' && url[0]!='L') {
//		iprintf("\n--------------\nURL=%s\n",url);
//		iprintf("\nLogged in=%d\n",LoggedIn);
//	}
//	if ((httpstricmp(url, "logout.htm") == 1)) {

//	int x = strlen(url);
//	int y = strlen(connectedListStr);
//	iprintf("\n  sock=%d, x=%d, y=%d\n", sock, x, y);

//	if (strlen(url) + strlen(connectedListStr) + 5 < 500) {
//		strcat(connectedListStr, url);
//		strcat(connectedListStr, "<br/>");
//	}

//	iprintf("\nRequested page %s",url);

	/* NEW */
	if (((httpstricmp(upperCase(url), "RECEIVER.HTM") == 1)
			|| (httpstricmp(upperCase(url), "SETUP.HTM") == 1)
			|| (httpstricmp(upperCase(url), "CHANNELS.HTM") == 1)
			|| (httpstricmp(upperCase(url), "CHANNELS_TXID.HTM") == 1))
			&& (strlen(url) > 0)) { // if one of the password protected screens
		if (NV_Settings.Password[0]) { // if password is in effect...
			if (LoggedIn == 0) { // if not logged in
				iprintf(
						"\nTried to access protected screen when not logged in...\n");
				iprintf("URL=<%s>\n", url);
				iprintf("length URL=<%d>\n", strlen(url));

				RedirectResponse(sock, "login.HTM"); // show password protect page
				return (*oldhand)(sock, url, rxBuffer);
			}
			if (GetPreciseTime() - LoggedInTimer > NV_Settings.PasswordTimeout * 1000000
					|| NV_Settings.Password[0] == 0) { //  if timed out
				RedirectResponse(sock, "logintimeout.HTM"); // show timeout page
				return (*oldhand)(sock, url, rxBuffer);
			}
		}
		LoggedInTimer = GetPreciseTime(); // reset timer if protected screen was successfully loaded
	}
	/* End new */

	if (httpstricmp(upperCase(url), "INDEX.HTM") == 1) {
//		iprintf("\nRequested page INDEX.HTM");
		RedirectResponse(sock, "login.HTM");
		return (*oldhand)(sock, url, rxBuffer); //reset password
	}

	if (httpstricmp(upperCase(url), "LOGOUT.HTM") == 1) {
//		iprintf("\n\nlogout\n");
		iprintf("/nRequested page logout.htm but logged out and redirected to login.htm");
		LoggedIn = 0;
//		iprintf("\n\nlogged out\n");
		RedirectResponse(sock, "login.HTM");
		return (*oldhand)(sock, url, rxBuffer); //reset password
	}
//	iprintf("\n\nnot logout\n");
//	iprintf("\nURL=%s\n",url);
	if ((strcmp(url, "RESETPASSWORD") == 0)) {
//		iprintf("/nRequested page RESETPASSWORD");
		NV_Settings.Password[0] = 0;
		NV_Settings.UserName[0] = 0;
		SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
		RedirectResponse(sock, "Channels.HTM");
		iprintf("\n\nReset password by secret means\n");
		return (*oldhand)(sock, url, rxBuffer); //reset password
	}

#if 1==0
	if (NV_Settings.Password[0]) { // if password is in effect...
		if (LoggedIn == 0 || (GetPreciseTime() - LoggedInTimer > NV_Settings.PasswordTimeout * 1000000)) { // if not logged in or timed out
			if (url[0] != 0) {
//				iprintf("\n\nnot logged in and password in effect\n");
				if ((httpstricmp(url, "RECEIVER.HTM") == 1)
						|| (httpstricmp(url, "SETUP.HTM") == 1)
						|| (httpstricmp(url, "CONNECTED.HTM") == 1)
						|| (httpstricmp(url, "CHANNELS_TXID.HTM") == 1)) {
					RedirectResponse(sock, "passwordRequired.HTM");
					return (*oldhand)(sock, url, rxBuffer); //reset password
//					return 1; // we are not logged in and password in effect

#if 1==0
					/* We need to do the password */

					char *pPass;
					char *pUser;
					iprintf("Requesting password");
					if (!CheckAuthentication(url, &pPass, &pUser)) {
						/* No password was provided */
						RequestAuthentication(sock, "Config Page");
						RedirectResponse(sock, "Channels1.HTM");
//						return (*oldhand)(sock, url, rxBuffer); //If we got  here the password was acceptable.
						return 1;
					}
					if (!TestPassword(pUser, pPass)) {
						RequestAuthentication(sock, "Config Page");
						strcpy(url,"Channels.HTM");
//						return (*oldhand)(sock, url, rxBuffer); //If we got  here the password was acceptable.
						return 1;
					}
#endif

				}
			}
//		} else if (GetPreciseTime() - LoggedInTimer > NV_Settings.PasswordTimeout * 1000000) {
//			RedirectResponse(sock, "logintimeout.HTM");
//		} else {
//			LoggedInTimer = GetPreciseTime(); // #5
//			iprintf("\nLoggedInTimer=%ld\n", LoggedInTimer);
		} else {
			LoggedInTimer = GetPreciseTime(); // #5
			iprintf("\n\nresetting time...\n");
		}
	}
#endif

	if (httpstricmp(url, "CHANNELS.HTM") == 1) {
		if (justToggledChanelsScanFlag == 1) {
			justToggledChanelsScanFlag = 0;
		} else {
//			channelScanFlag=1;
			for (i = 0; i < 8; i++) {
				chanLock[i] = 0;
			}
		}
//		iprintf("\n\nRequested channels page\n\n");
	}
	if (httpstricmp(url, "INDEX.HTM") == 1) {
		channelScanFlag = 0;
		singleScanFlag = 0;
//		iprintf("\n\nRequested another page\n\n");
	}

//	iprintf("\n\nRequested a reload from %s\n",url);
	return (*oldhand)(sock, url, rxBuffer); //If we got  here the password was acceptable.
}

/*-------------------------------------------------------------------
 Register our own custom post handler

 ------------------------------------------------------------------*/
void RegisterPost() {
	SetNewPostHandler(MyDoPost);
	oldhand = SetNewGetHandler(MyDoGet);
}

void fixPassword(char pass[]) {

	int passwordLen;
	int i;
	int j;
	passwordLen = strlen(pass);
//	iprintf("\n\nPassword before fix: <%s>, length=%d\n", pass, passwordLen);
	for (i = 0; i < passwordLen; i++) {
		while (pass[i] < '0' || pass[i] > 'z') { // if not a number or letter
			if (pass[0] == 0)
				break;
			for (j = i; j < passwordLen; j++) {
				pass[j] = pass[j + 1];
			}
			if (passwordLen > 0) {
				passwordLen--;
			}

		}
//		if (pass[i]>='a' && pass[i]<='z') { // if it's lowercase
//			pass[i]-=32;                    // make it uppercase
//		}
	}
//	iprintf("Password after fix: <%s>\n", pass);
}
