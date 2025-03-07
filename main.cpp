/* $Revision: 1.0 $ */
/* Copyright $Date: 2013/4/5 22:53:30 $ */

/* -----------------------------------------------------------------------------
 * DNR-100 (formerly RD-1100) Controller Program
 * Mark Patten
 * Tektron Micro Electronics
 *
 *
 * This program creates a web page GUI for controlling the DNR-1200
 * UART1 connects to the DNR-1200
 * The webpage sends RS-232 messages to the DNR-1200 and displays the status of the DNR-1200
 * The Tektron standard GUI can also control the DNR-1200 through a Telnet connection
 * UART0 initially sends debug information, but will also relay RS-232 commands to the DNR-1200
 * Therefore, control messages from three sources (Web page, Telnet, and UART0) are "mixed" together and sent to the RS-1200
 * network.  It basically opens a serial port and an Ethernet port, and then
 * connects them.  This allows any data sent to the Ethernet port to be
 * forwarded to the serial port and vice versa.
 *
 *
 * Revisions:
 *
 * 04-05-13		v0.90	Getting close to the final version
 * 01-16-15     v2.02   Changed name to DNR-1100  A2100492
 * 03-23-15     v2.03   Added web page port setting
 * 10-02-17     v2.04   Finished web page port setting, also changed frequencies and re-added tx voltage and tx ID on receiver page
 * 02-06-18     v2.06   Added ability to set frequencies manually, changed name to DNR-1200
 * 03-26-18     v2.07   Fixed bug in "All Off" button
 * 06-27-18     v2.08   Fixed bug in "Tone No Lock" which was causing "Cal Tone" to also be selected
 * 07-10-18     v2.09   Added message saying web page port requires power cycle; added receiver firmware version display
 * 10-02-19     v2.10   Fixing stinger transmitter bug where it displays "Stereo" but actually transmits command for mono
 * 02-09-20     v2.11   Make password always required to see channels or receiver pages; added TX RSSI display to receiver page
 * 02-17-20     v2.12   Fixed the way information is updated for txid voltage and RSSI
 * 02-29-20     v2.13   Fixed "select channel 6, get channel 3" problem
 * 03-05-20     v2.14   Fixed non-blinking command RSSI display, timeout page redirect error, non-working timeout time set
 * 03-09-20     v2.15   Set minimum timeout value to 30 seconds
 * 03-14-20     v2.16   Made option to disable AJAX update of receiver page so no message if receiver disconnected
 *
 * -------------------------------------------------------------------------- */
#define TEST_GPIO_STUFF_FROM_MARK 1
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
#include <string.h>
#include <constants.h>
#include <basictypes.h>
#include <buffers.h>
#include <dhcpclient.h>
#include <dns.h>
#include <iointernal.h>
#include <serinternal.h>
#include <snmp.h>
#include <arp.h>
#include <taskmon.h>
#include "serialburnerdata.h"
#include "ReceiverControl.h"
#include "formtools.h"
#if TEST_GPIO_STUFF_FROM_MARK==1
#include <pins.h>
#endif

//#define NODEBUG_PORT   ( 1 )

#ifdef NB_VERSION_1_9
#define OS_VER "1.9"
#else
#ifdef NB_VERSION_1_8
#define OS_VER "1.8"
#else
#define OS_VER "1.7"
#endif
#endif

const char FirmwareVersion[] =
		"DNR-1200 WebGUI v2.16 A2100525 " __DATE__ " OS:" NB_VERSION_TEXT" ";

//----- Function Prototypes -----
extern "C" {
void UserMain(void *pd);
void DisplayWebPage(int sock, PCSTR url);
void fOSDumpTasks(FILE *fp);
void DiagPage(int sock, PCSTR url);

#ifdef NODEBUG_PORT
int XOpenDBGSerial( int portnum, unsigned int baudrate );
#endif
}
int FD_SerialData; /* The file descriptor for the data socket */
int FD_SerialDebug; /* The file descriptor for the debugging serial port. (The other port ) */
int FD_ListeningSocket; /* The socket to listen on if we are listening */
int FD_ConnectedSocket; /* The connected data socket */
int FD_shutdown; /* The file descriptor used to communciate between tasks */

BOOL bWeInitiatedConnection; /* True if we established the connection */
DWORD LastNetWorkDataRxed; /* The time when we last received network data  */
DWORD LastNetWorkDataTxed; /* The time when we last transmitted data */
DWORD LastConnectTry; /* The time when we last tried to make an outgoing connection */

BOOL bSerialBoundDataBlocked;
BOOL bNetWorkBoundDataBlocked;
BOOL bShowDebug;
volatile DWORD tcp_write;
volatile DWORD tcp_read;
volatile DWORD ser_write;
volatile DWORD ser_read;
#ifdef SERIAL_DEBUG_DATA
extern volatile DWORD s1rx;
extern volatile DWORD s1rxa;
extern volatile DWORD s2rx;
extern volatile DWORD s2rxa;
extern volatile DWORD s2rxerr;
extern volatile DWORD s1rxerr;
#endif
fd_set waiting_fd_r;
fd_set waiting_fd_w;
fd_set waiting_fd_err;

int rssiA, rssiB, vuL, vuR;
int dataOnSerialPortFlag = 0;
long int dataOnSerialPortTime;
int ipSetupFlag = 0;

#define dbprintf if (bShowDebug) iprintf

void SendBreak(int port, DWORD time);

char GetHexByte(const char *cp) {
	BYTE tv = 0;
	long lv = strtol(cp, NULL, 16);
	tv = (lv & 0xFF );
	char bui[20];
	bui[0] = *cp;
	bui[1] = *(cp + 1 );
	bui[2] = 0;
	return tv;
}

char serial_msg_buffer[80];
void SendSerialMessage(const char *msg) {
	if (FD_SerialData == 0) {
		return;
	}
	const char *cp = msg;
	char *co = serial_msg_buffer;
	while (*cp) {
		if (*cp == '%') {
			switch (cp[1]) {
			case '%':
				*(co++) = '%';
				cp += 2;
				break;
			case 'R':
			case 'r':
				*(co++) = '\r';
				cp += 2;
				break;
			case 'N':
			case 'n':
				*(co++) = '\n';
				cp += 2;
				break;
			case 'x':
			case 'X': {
				BYTE bv = GetHexByte(cp + 2);
				*(co++) = bv;
				cp += 4;
			}
				break;
			default:
				*(co++) = *(cp++);
				*(co++) = *(cp++);
			}
		} else {
			*(co++) = *(cp++);
		}
	}
	write(FD_SerialData, serial_msg_buffer, co - serial_msg_buffer);
}

int SerialBreakWrite(int fd, char *start, int len) {
	PBYTE pb = (PBYTE) start;
	int nw = 0;
	int rv = 0;

	for (int i = 0; i < len; i++) {
		if (pb[i] == NV_Settings.BreakKeyValue) {
			if ((i - nw )) {
				rv = write(fd, start + nw, i - nw);
				if (rv < 0) {
					return rv;
				}
				nw += rv;
				OSTimeDly(10);
				{
					WORD bi;
					bi = NV_Settings.BreakInterval * 2;
					if (bi == 0) {
						bi = 20;
					}
					SendBreak(FD_SerialData, bi);
				}
				nw++;
			}
		}
	}
	if (nw != len) {
		rv = write(fd, start + nw, len - nw);
		if (rv < 0) {
			return rv;
		}
		nw += rv;
	}
	return nw;
}

void OpenSerialPorts();

/* Function called to shut down every thing */
void CloseEverything() {
	dbprintf("Closing every thing");

	if ((FD_ConnectedSocket) && (NV_Settings.ConnectLossMessage[0])) {
		SendSerialMessage(NV_Settings.ConnectLossMessage);
		if (FD_SerialData) {
			UartDataRec *prec =
					&(UartData[FD_SerialData - SERIAL_SOCKET_OFFSET]);
			volatile BOOL bv;
			int n = 0;
			bv = prec->m_FifoRead.Empty();
			while ((!(bv)) && (n++ < 100)) {
				OSTimeDly(2);
				bv = prec->m_FifoRead.Empty();
			}
			OSTimeDly(20);
		}
	}
	close(FD_SerialData);
	close(FD_ListeningSocket);
	close(FD_ConnectedSocket);
	FD_SerialData = 0;
	FD_ListeningSocket = 0;
	FD_ConnectedSocket = 0;
	SetHaveError(FD_shutdown);
	dbprintf("We are closed\r\n");
}
/*-------------------------------------------------------------------

 Network connection establishment functions.


 -------------------------------------------------------------------*/

/* Open a listening port */
void OpenListeningPort() {
	dbprintf("Opened listining port\r\n");
	if (FD_ListeningSocket) {
		close(FD_ListeningSocket);
	}

	FD_ListeningSocket = listen(INADDR_ANY, NV_Settings.ListenPort, 3);
	iprintf("Listening on port %d\r\n", NV_Settings.ListenPort);
}
/* Close listening port */
void CloseListeningPort() {
	dbprintf("Closed listining port\r\n");
	close(FD_ListeningSocket);
	FD_ListeningSocket = 0;
}
/*Called to initiate a connection */
void MakeConnection() {
	if (FD_ConnectedSocket != 0) {
		close(FD_ConnectedSocket);
		if (NV_Settings.ConnectLossMessage[0]) {
			SendSerialMessage(NV_Settings.ConnectLossMessage);
		}
	}

	FD_ConnectedSocket = connect(NV_Settings.ConnectAddress, 0,
			NV_Settings.ConnectPort, TICKS_PER_SECOND * 10);
	if (FD_ConnectedSocket > 0) {
		SetSocketRxBuffers(FD_ConnectedSocket, 15);
		setsocketackbuffers(FD_ConnectedSocket, 15);
		dbprintf("Successful outgoing connection\r\n");
		bWeInitiatedConnection = TRUE;
		if (NV_Settings.ConnectMessage[0]) {
			SendSerialMessage(NV_Settings.ConnectMessage);
		}
	} else {
		dbprintf("Failed to make an outgoing connection\r\n");
		bWeInitiatedConnection = FALSE;
		close(FD_ConnectedSocket);
		FD_ConnectedSocket = 0;
		LastConnectTry = Secs;
	}
	if (NV_Settings.new_connection_timeout) {
		CloseListeningPort();
	}

	LastNetWorkDataRxed = Secs;
	LastNetWorkDataTxed = Secs;
}
BOOL OkToListen() {
	if (FD_ListeningSocket != 0) {
		return FALSE;
	}

	if (((LastNetWorkDataRxed + NV_Settings.new_connection_timeout) > Secs)
			&& (FD_ConnectedSocket != 0)) {
		return FALSE;
	}

	if (NV_Settings.ListenPort) {
		return TRUE;
	}

	return FALSE;
}
/* Test all of the communication time outs */
void ProcessTimeOuts() {
	if (FD_ConnectedSocket > 0) {
		/*We have a current connection */
		DWORD most_current = LastNetWorkDataRxed;
		DWORD limit;

		if (LastNetWorkDataTxed > most_current) {
			most_current = LastNetWorkDataTxed;
		}

		if (bWeInitiatedConnection) {
			limit = NV_Settings.connect_idle_timeout;
		} else {
			limit = NV_Settings.listen_idle_timeout;
		}

		if (limit != 0) {
			if ((most_current + limit) < Secs) {
				close(FD_ConnectedSocket);
				if (NV_Settings.ConnectLossMessage[0]) {
					SendSerialMessage(NV_Settings.ConnectLossMessage);
				}
				FD_ConnectedSocket = 0;
				LastConnectTry = 0;/* Force an immediate retry */
			}
		}
	} else {
		/* Not currently connected */
		if ((LastConnectTry == 0)
				|| ((LastConnectTry + NV_Settings.connection_retry_timeout)
						< Secs)) {
			if ((NV_Settings.ConnectMode != SERIAL_CONNECT_CONNECT_NEVER)
					&& (NV_Settings.ConnectAddress == 0)) {
				IPADDR ipa = 0;
				int rv = GetHostByName(NV_Settings.ConnectName, &ipa, 0,
						TICKS_PER_SECOND * 10);

				iprintf("\n\n Hey mark, IP Address 2 is ");
				ShowIP(ipa);
				iprintf("\n");

				if (rv == DNS_OK) {
					NV_Settings.ConnectAddress = ipa;
					LastConnectTry = 0;/* Try a connection */
				} else {
					NV_Settings.ConnectAddress = 0;
					LastConnectTry = Secs;
				}
			}
			//DNS processing
			if ((NV_Settings.ConnectMode == SERIAL_CONNECT_CONNECT_AT_POWERUP)
					&& (NV_Settings.ConnectAddress)) {
				MakeConnection();
			}
		}
		//Should be trying to connect
	}
	//Not currently connected

	/* Test to see if we should be listening */
	if (OkToListen()) {
		OpenListeningPort();
	}
}

extern char connectedListStr[];

/* Called when connections are waiting to be accepted */
void ProcessAccept() {
	IPADDR ipa;
	WORD port;
	int new_fd = accept(FD_ListeningSocket, &ipa, &port, 2);
	int i, j, k;
	int flag;
	char tbuf[50];
	iprintf("\nConnection from IP Address ");
	ShowIP(ipa);
	iprintf(" Port %ld", port);
	iprintf("\n");

	if (strlen(connectedListStr) + 40 < CONNECTED_LIST_STR_LEN) {
		j = 0;
		for (i = CONNECTED_LIST_STR_LEN / 2; i < CONNECTED_LIST_STR_LEN; i++) {
			connectedListStr[j] = connectedListStr[i];
			j++;

		}
		connectedListStr[j] = 0;
	}
	PBYTE ipb = (PBYTE) &ipa;
	sprintf(tbuf, "Connection from (%d.%d.%d.%d) Port %ld \n", (int) ipb[0],
			(int) ipb[1], (int) ipb[2], (int) ipb[3], (long int)port);
	strcat(connectedListStr, tbuf);

	if (new_fd > 0) {
		if (FD_ConnectedSocket != 0) {
			close(FD_ConnectedSocket);
		}
		if (NV_Settings.BreakOnConnect) {
			WORD bi;
			bi = NV_Settings.BreakInterval * 2;
			if (bi == 0) {
				bi = 20;
			}
			SendBreak(FD_SerialData, bi);
		}

		if (NV_Settings.ConnectMessage[0]) {
			SendSerialMessage(NV_Settings.ConnectMessage);
		}

		FD_ConnectedSocket = new_fd;
		SetSocketRxBuffers(FD_ConnectedSocket, 15);
		setsocketackbuffers(FD_ConnectedSocket, 15);
		LastNetWorkDataRxed = Secs;
		LastNetWorkDataTxed = Secs;
		dbprintf("Accepted new connection \r\n");
		bWeInitiatedConnection = FALSE;
		if (NV_Settings.new_connection_timeout) {
			CloseListeningPort();
		}
	}
}

/* Special SB70LC serial functions */
void Serial485HalfDupMode(int port, int enab);
void SerialEnableHwTxFlow(int port, int enab);
void SerialEnableHwRxFlow(int port, int enab);
void fShowSerialData(FILE *fp);
extern fd_set write_file_sets;
extern fd_set read_file_sets;
extern fd_set error_file_sets;

#define BUFFER_SIZE 20000
char Buffer_From_S2N[BUFFER_SIZE];
int Buf_S2N_Start;
int Buf_S2N_End;
char Buffer_From_N2S[BUFFER_SIZE];
int Buf_N2S_Start;
int Buf_N2S_End;
char CommandBuffer[256];
int db_pos;
void DumpTree(int fd);
IPADDR rawGetDNS(const char *cmd, FILE *fp) {
	const char *cp = cmd;

	//Trim leading white space
	while ((*cp) && (isspace(*cp))) {
		cp++;
	}

	if (*cp) {
		IPADDR ip;
		int rv = GetHostByName(cp, &ip, 0, 20 * TICKS_PER_SECOND);

		switch (rv) {
		case DNS_OK:
			return ip;
			break;
		case DNS_TIMEOUT:
			fiprintf(fp, "\r\nDNS timed out\r\n");
			break;
		case DNS_NOSUCHNAME:
			fiprintf(fp, "\r\nDNS reported no such name\r\n");
			break;
		default:
			fiprintf(fp, "\r\nUnknown DNS error %d\r\n", rv);
		}
	} else {
		fiprintf(fp, "No Name\r\n");
	}

	return 0;
}
void DoPing(const char *cmd, FILE *fp) {
	IPADDR addr_to_ping = 0;
	const char *cp = cmd;

	//Trim leading white space
	while ((*cp) && (isspace(*cp))) {
		cp++;
	}

	fiprintf(fp, "About to ping [%s]\r\n", cp);
	//Get the address or use the default
	if (cp[0]) {
		addr_to_ping = rawGetDNS(cp, fp);
	} else {
		addr_to_ping = EthernetIpGate;
	}

	if (addr_to_ping == 0) {
		fiprintf(fp, "\r\n");
		return;
	}
	fiprintf(fp, "\nPinging :");
	fShowIP(fp, addr_to_ping);
	fiprintf(fp, "\r\n");
	int rv = Ping(addr_to_ping, 1/*Id */, 1 /*Seq */, 100 /*Max Ticks*/);

	if (rv == -1) {
		fiprintf(fp, " Failed! \r\n");
	} else {
		fiprintf(fp, " Response Took %d ticks\r\n", rv);
	}
}

void ProcessDebugCommand(const char *cmd, int fd) {
	FILE *fp = fdopen(fd, "w");
	switch (toupper(cmd[0])) {
	case 'A':
		fShowArp(fp);
		break;
	case 'C':
		fShowCounters(fp);
		fiprintf(fp, "Serial Sent = %ld\r\n", ser_write);
		fiprintf(fp, "Serial Read = %ld\r\n", ser_read);
		fiprintf(fp, "TCP Sent = %ld\r\n", tcp_write);
		fiprintf(fp, "TCP read = %ld\r\n", tcp_read);
#ifdef SERIAL_DEBUG_DATA
		fiprintf( fp, "s1rx read = %ld\r\n", s1rx );
		fiprintf( fp, "s2rx read = %ld\r\n", s2rx );
		fiprintf( fp, "s1rxa read = %ld\r\n", s1rxa );
		fiprintf( fp, "s2rxa read = %ld\r\n", s2rxa );
		fiprintf( fp, "s1rxerr read = %ld\r\n", s1rxerr );
		fiprintf( fp, "s2rxerr read = %ld\r\n", s2rxerr );
#endif
		fiprintf(fp, "Waiting rd_r=[");
		if (FD_ISSET(FD_ConnectedSocket, &waiting_fd_r)) {
			fiprintf(fp, "Conneected socket,");
		}

		if (FD_ISSET(FD_SerialData, &waiting_fd_r)) {
			fiprintf(fp, "Serial Data");
		}

		fiprintf(fp, "] Set =[");
		if (FD_ISSET(FD_ConnectedSocket, &read_file_sets)) {
			fiprintf(fp, "Conneected socket,");
		}

		if (FD_ISSET(FD_SerialData, &read_file_sets)) {
			fiprintf(fp, "Serial Data");
		}

		fiprintf(fp, "]\r\n");
		fiprintf(fp, "Waiting rd_w=[");
		if (FD_ISSET(FD_ConnectedSocket, &waiting_fd_w)) {
			fiprintf(fp, "Conneected socket,");
		}

		if (FD_ISSET(FD_SerialData, &waiting_fd_w)) {
			fiprintf(fp, "Serial Data");
		}

		fiprintf(fp, "]  Set =[");
		if (FD_ISSET(FD_ConnectedSocket, &write_file_sets)) {
			fiprintf(fp, "Conneected socket,");
		}

		if (FD_ISSET(FD_SerialData, &write_file_sets)) {
			fiprintf(fp, "Serial Data");
		}

		fiprintf(fp, "]\r\n");
		fiprintf(fp, "Waiting rd_err=[");
		if (FD_ISSET(FD_ConnectedSocket, &waiting_fd_err)) {
			fiprintf(fp, "Conneected socket,");
		}

		if (FD_ISSET(FD_SerialData, &waiting_fd_err)) {
			fiprintf(fp, "Serial Data");
		}

		fiprintf(fp, "]  Set =[");
		if (FD_ISSET(FD_ConnectedSocket, &error_file_sets)) {
			fiprintf(fp, "Conneected socket,");
		}

		if (FD_ISSET(FD_SerialData, &error_file_sets)) {
			fiprintf(fp, "Serial Data");
		}

		fiprintf(fp, "]\r\n");
		break;
	case 'U':
		fOSDumpTasks(fp);
		break;
#ifdef SNMP
		case 'S':
		DumpTree( fd );
		break;
#endif
	case 'W':
		fShowSerialData(fp);
		break;
	case 'P':
	case '=':
		DoPing(cmd + 1, fp);
		break;
	case '1':
		bShowDebug = TRUE;
		fiprintf(fp, "Debug is on\r\n");
		break;
	case '0':
		bShowDebug = FALSE;
		fiprintf(fp, "Debug is off\r\n");
		break;
	case '?':
		writestring(
				fd,
				"Debug commands :\r\n1 Debug messages on\r\n0 Debug messages off\r\nA show arp\r\nC show counter\r\nP x.x.x.x ping\r\nU ucos tasks\r\nS dump snmp tree\r\n");
		break;
	default:
		writestring(fd, "Unknown command ? for help\r\n");
	}
	fflush(fp);
	fp->_file = 99;
	fclose(fp);
}
void DiagPage(int sock, PCSTR url) {
	if (url[8] == '?') {
		writestring(sock, "<pre>\r\n");
		if ((url[9] == 'P') && (url[10] == 'i')) {
			ProcessDebugCommand(url + 17, sock);
		} else {
			ProcessDebugCommand(url + 9, sock);
		}

		writestring(sock, "</pre>\r\n");
	}

	writestring(
			sock,
			"<font face=\"Arial,sans-serif\" size=\"2\"><a href=\"diag.htm?A\">ARP Cache</a><br>");
	writestring(sock, "<a href=\"diag.htm?C\">Data Counters</a><br>");
	writestring(sock, "<a href=\"diag.htm?U\">Show Processor Tasks</a><br>");
	writestring(sock, "<a href=\"diag.htm?S\">Dump SNMP Tree</a><br>");
	writestring(sock, "<a href=\"diag.htm?W\">Dump UART State</a></font><br>");
	writestring(
			sock,
			"<form action=\"diag.htm?I\" method=get><input type=\"Submit\" value=\"Ping\"><input name=\"PingName\" type=\"text\" size=\"40\" value=\"");

	if ((url[8] == '?') && (url[9] == 'P') && (url[10] == 'i')) {
		writestring(sock, url + 18);
	} else {
		writestring(sock, NV_Settings.ConnectName);
	}

	writestring(sock, "\"></form>\n");
}

/*-------------------------------------------------------------------

 Functions to move data to/from serial and network ports.

 -------------------------------------------------------------------*/
void blinkLine17(void *);
void toggleLine17();
void scanComForIPsetCmd(char c);

/* Called when data is recieved on the debug port */
void ProcessDebugdata() {
	if (db_pos == 255) {
		db_pos -= 1;
	}

	int rv = read(FD_SerialDebug, CommandBuffer, 255);

	if (rv > 0) {
//		int ii;
//	   iprintf("Read %d bytes\n",rv);
//	   for (ii=0;ii<rv;ii++) {
//		   iprintf("Read byte %d\n",CommandBuffer[db_pos+ii]);
//		}

		int i;
		for (i = 0; i < rv; i++) {
			scanComForIPsetCmd(*(CommandBuffer + i));
			pushCharToComBuf2(*(CommandBuffer + i));

		}

		/*
		 write(FD_SerialDebug, CommandBuffer + db_pos, rv);
		 db_pos += rv;
		 if ((CommandBuffer[db_pos - 1] == '\r')
		 || (CommandBuffer[db_pos - 1] == '\n')) {
		 if (db_pos == 1) {
		 } else {
		 CommandBuffer[db_pos] = 0;
		 if ((db_pos) && (CommandBuffer[db_pos - 1] < 0x20)) {
		 db_pos--;
		 }

		 CommandBuffer[db_pos] = 0;
		 ProcessDebugCommand(CommandBuffer, 0FD_SerialDebug);
		 }
		 db_pos = 0;
		 }
		 */
	}
}

char IPCommand[10];
int IPCommandIndex = 0;
void scanComForIPsetCmd(char c) {
	c = toupper(c);

	switch (IPCommandIndex) {
	case 0:
		if (c == 'S')
			IPCommandIndex++;
		else
			IPCommandIndex = 0;
		break;
	case 1:
		if (c == 'E')
			IPCommandIndex++;
		else
			IPCommandIndex = 0;
		break;
	case 2:
		if (c == 'T')
			IPCommandIndex++;
		else
			IPCommandIndex = 0;
		break;
	case 3:
		if (c == 'I')
			IPCommandIndex++;
		else
			IPCommandIndex = 0;
		break;
	case 4:
		if (c == 'P') {
			IPCommandIndex++;
			iprintf(
					"\nGot SETIP command---Enter Static IP Address or D for DHCP:\n");
			ipSetupFlag = 1;
		} else
			IPCommandIndex = 0;
		break;
	case 5:
	default:
		if (c == '\r') {
			ipSetupFlag = 0;
			iprintf("\nEnd SETIP command\n");
			IPCommandIndex = 0;
		}
		if (c = ' ')
			if (c = 'D') {
				// go to DHCP mode
			}
//			if ()
		break;
	}

}

/* Called when data is recieved on the serial data port */
void ProcessReadSerialData() {
	int rv;
	rv = read(FD_SerialData, Buffer_From_S2N + Buf_S2N_End,
			BUFFER_SIZE - Buf_S2N_End);

//	iprintf("Read %d bytes from Ladybug\n",rv);

	if (rv > 0) {
		int i;
		for (i = 0; i < rv; i++) {
			pushCharToComBuf(*(Buffer_From_S2N + Buf_S2N_End + i));

		}
//		if (dataOnSerialPortFlag==1) {
		int nwr;
		if (ipSetupFlag != 1) {
			nwr = write(FD_SerialDebug, Buffer_From_S2N + Buf_S2N_End, rv);
		}

		ser_read += rv;
		Buf_S2N_End += rv;
		if ((FD_ConnectedSocket == 0)
				&& (NV_Settings.ConnectMode
						== SERIAL_CONNECT_CONNECT_WHEN_DATARX)) {
			MakeConnection();
		}

		if ((FD_ConnectedSocket != 0)
				&& (FD_ISSET(FD_ConnectedSocket, &write_file_sets))) {
			int nwr;
			//We are able to write

			nwr = write(FD_ConnectedSocket, Buffer_From_S2N + Buf_S2N_Start,
					(Buf_S2N_End - Buf_S2N_Start ));

			if (nwr > 0) {
				//We wrote something
				tcp_write += nwr;
				LastNetWorkDataTxed = Secs;
				Buf_S2N_Start += nwr;
				if (Buf_S2N_Start == Buf_S2N_End) {
					//We wrote everything
					Buf_S2N_Start = 0;
					Buf_S2N_End = 0;
				}
			} else if (nwr < 0) {
				close(FD_ConnectedSocket);
				if (NV_Settings.ConnectLossMessage[0]) {
					SendSerialMessage(NV_Settings.ConnectLossMessage);
				}
				FD_ConnectedSocket = 0;
				LastConnectTry = 0;/* Force an immediate retry */
			}
		}
		//We were able to write
		else {
			//We can't write
			if (FD_ConnectedSocket == 0) {
				//No place to send the data we already tried to connect earlier so
				//Toss all of the RX chars into the bit bucket!
				Buf_S2N_Start = 0;
				Buf_S2N_End = 0;
			}
		}
	}
	long int x;
	x = GetPreciseTime();
	if ((x - dataOnSerialPortTime) > 40000) {
//		dataOnSerialPortFlag=0;
	}
}
/* Called when data is recieved on the network port */
void ProcessReadNetworkData() {
	int rv = read(FD_ConnectedSocket, Buffer_From_N2S + Buf_N2S_End,
			BUFFER_SIZE - Buf_N2S_End);

	if (rv > 0) {
		tcp_read += rv;
		LastNetWorkDataRxed = Secs;
		Buf_N2S_End += rv;
		if (FD_ISSET(FD_SerialData, &write_file_sets)) {
			int nwr;
			/* Space to write some data */
			if (NV_Settings.BreakKeyFlag) {
				nwr = SerialBreakWrite(FD_SerialData,
						Buffer_From_N2S + Buf_N2S_Start,
						(Buf_N2S_End - Buf_N2S_Start ));
			} else {
				nwr = write(FD_SerialData, Buffer_From_N2S + Buf_N2S_Start,
						(Buf_N2S_End - Buf_N2S_Start ));
			}

			if (nwr > 0) {
				ser_write += nwr;
				Buf_N2S_Start += nwr;
				if (Buf_N2S_Start >= Buf_N2S_End) {
					//We wrote everything
					Buf_N2S_Start = 0;
					Buf_N2S_End = 0;
				}
			}
		}
	} else {
		close(FD_ConnectedSocket);

		if (NV_Settings.ConnectLossMessage[0]) {
			SendSerialMessage(NV_Settings.ConnectLossMessage);
		}

		Buf_N2S_Start = 0;
		Buf_N2S_End = 0;
		FD_ConnectedSocket = 0;
		dbprintf("Error on network socket, socket closed\r\n");
	}
}
/* Called when write space has just opened up on network port*/
void ProcessWriteNetworkData() {
	if (Buf_S2N_End == Buf_S2N_Start) {
		return;
	}
	int nwr;
	nwr = write(FD_ConnectedSocket, Buffer_From_S2N + Buf_S2N_Start,
			(Buf_S2N_End - Buf_S2N_Start ));

	if (nwr > 0) {
		tcp_write += nwr;
		Buf_S2N_Start += nwr;
		if (Buf_S2N_Start >= Buf_S2N_End) {
			//We wrote everything
			Buf_S2N_Start = 0;
			Buf_S2N_End = 0;
		}
	} else if (nwr < 0) {
		close(FD_ConnectedSocket);
		if (NV_Settings.ConnectLossMessage[0]) {
			SendSerialMessage(NV_Settings.ConnectLossMessage);
		}
		FD_ConnectedSocket = 0;
		LastConnectTry = 0; /* Force an immediate retry */
	}
}
/* Called when write space has just opened up serial port*/
void ProcessWriteSerialData() {
	if (Buf_N2S_End == Buf_N2S_Start) {
		return;
	}

	int nwr;

	if (NV_Settings.BreakKeyFlag) {
		nwr = SerialBreakWrite(FD_SerialData, Buffer_From_N2S + Buf_N2S_Start,
				(Buf_N2S_End - Buf_N2S_Start ));
	} else {
		nwr = write(FD_SerialData, Buffer_From_N2S + Buf_N2S_Start,
				(Buf_N2S_End - Buf_N2S_Start ));
	}

	if (nwr > 0) {
		ser_write += nwr;
		Buf_N2S_Start += nwr;
		if (Buf_N2S_Start >= Buf_N2S_End) {
			//We wrote everything
			Buf_N2S_Start = 0;
			Buf_N2S_End = 0;
		}
	}
}
void ProcessSerialError() {
	dbprintf("Error on the serial port\r\n");
	OSTimeDly(10);
	close(FD_SerialData);
#ifndef NODEBUG_PORT
	close(FD_SerialDebug);
#endif

	OpenSerialPorts();
}
void ProcessListenError() {
	dbprintf("Error on the listening port\r\n");
	CloseListeningPort();
	if (OkToListen()) {
		OpenListeningPort();
	}
}
void ProcessNetworkError() {
	dbprintf("Error on the connected port\r\n");
	while (dataavail(FD_ConnectedSocket)) {
		ProcessReadNetworkData();
	}

	close(FD_ConnectedSocket);
	if (NV_Settings.ConnectLossMessage[0]) {
		SendSerialMessage(NV_Settings.ConnectLossMessage);
	}

	FD_ConnectedSocket = 0;
	LastConnectTry = 0; /* Force an immediate retry */
}
const char *SYSDESC = "NetBurner SB70LC Tcp to Serial application";
const char *SYSOID = "1.3.6.1.4.1.8174.2.70";
SysInfo sys_tmp();

const SysInfo * GetSysInfo() {
	CheckNVSettings();
	return &NV_Settings.SysInfoData;
}
void SetSysInfo(SysInfo si) {
	CheckNVSettings();
	memcpy(&NV_Settings.SysInfoData, &si, sizeof(si));
	SaveUserParameters(&NV_Settings, sizeof(NV_Settings));
}

void OpenSerialPorts(void) {
	if (FD_SerialData) {
		close(FD_SerialData);
		FD_SerialData = 0;
	}

#ifndef NODEBUG_PORT
	if (FD_SerialDebug) {
		close(FD_SerialDebug);
		FD_SerialDebug = 0;
	}
#endif

	parity_mode ep;
	int port;

	switch (NV_Settings.Output_Parity) {
	case 2:
		ep = eParityOdd;
		break;
	case 3:
		ep = eParityEven;
		break;
	default:
		ep = eParityNone;
	}

	if (NV_Settings.SerialMode == SERIAL_MODE_RS232_DATA_ON_PORT0) {
		port = 0;
	} else {
		port = 1;
	}

// Hard code the data serial port settings
	FD_SerialData = OpenSerial(1, 9600, 1, 8, eParityNone);
//	FD_SerialData = OpenSerial(port, NV_Settings.DataBaudRate,
//			NV_Settings.Output_Stop, NV_Settings.Output_Bits, ep);

#ifdef NODEBUG_PORT
	ReplaceStdio( 0, -1 );
	ReplaceStdio( 1, -1 );
	ReplaceStdio( 2, -1 );
	FD_SerialDebug = 0;
	XOpenDBGSerial( 0, 115200 );
#else
// Hard code the debug serial port settings
	FD_SerialDebug = OpenSerial(0, 9600, 1, 8, eParityNone);
//	FD_SerialDebug = OpenSerial((port ^ 1 ), gConfigRec.baud_rate, 1, 8,
//	eParityNone );
	ReplaceStdio(0, FD_SerialDebug);
	ReplaceStdio(1, FD_SerialDebug);
	ReplaceStdio(2, FD_SerialDebug);
#endif

//	iprintf("Data port is %d\n",port);

	if (NV_Settings.SerialMode >= SERIAL_MODE_RS485H_DATA_ON_PORT1) {
		/* Set up the RS-485 stuff */
		if (NV_Settings.SerialMode == SERIAL_MODE_RS485H_DATA_ON_PORT1) {
			Serial485HalfDupMode(port, 1);
		} else {
			Serial485HalfDupMode(port, 0);
		}
	} else {
		switch (NV_Settings.FlowMode) {
		case SERIAL_FLOW_MODE_NONE:
			SerialEnableTxFlow(port, 0);
			SerialEnableRxFlow(port, 0);
			SerialEnableHwTxFlow(port, 0);
			SerialEnableHwRxFlow(port, 0);
			break;
		case SERIAL_FLOW_MODE_XON_OFF:
			SerialEnableTxFlow(port, 1);
			SerialEnableRxFlow(port, 1);
			SerialEnableHwTxFlow(port, 0);
			SerialEnableHwRxFlow(port, 0);
			break;
		case SERIAL_FLOW_MODE_RTS_CTS:
			SerialEnableHwTxFlow(port, 1);
			SerialEnableHwRxFlow(port, 1);
			SerialEnableTxFlow(port, 0);
			SerialEnableRxFlow(port, 0);
			break;
		}
	}
}

const char *AppName = "SB70LC Application";

DhcpObject *pSB70LCDhcpObj;

int TestPassword(const char *name, const char *passwd) {
	if (NV_Settings.Password[0]) {
		if ((strcmp(name, NV_Settings.UserName) == 0)
				&& (strcmp(passwd, NV_Settings.Password) == 0))
			return 1;
		else {
			if (FD_SerialDebug) {
				writestring(FD_SerialDebug, "Password test failed, tried");
				writestring(FD_SerialDebug, name);
				writestring(FD_SerialDebug, ",");
				writestring(FD_SerialDebug, passwd);
				writestring(FD_SerialDebug, " against");
				writestring(FD_SerialDebug, NV_Settings.UserName);
				writestring(FD_SerialDebug, ",");
				writestring(FD_SerialDebug, NV_Settings.Password);
				writestring(FD_SerialDebug, "\r\n");

			}

			return 0;
		}
	} else
		return 1;
}

#if TEST_GPIO_STUFF_FROM_MARK==1
int gpioToggle = 0;
#endif

#if TEST_GPIO_STUFF_FROM_MARK==1
void blinkLine17(void *) {
	while (1) {
		if (gpioToggle == 1) {
			gpioToggle = 0;
			Pins[17].set(); // Set pin high
		} else {
			gpioToggle = 1;
			Pins[17].clr(); // Set pin low
		}
		OSTimeDly(1 * TICKS_PER_SECOND); // Delay for 1 second
	}
}

void toggleLine17() {
	if (gpioToggle == 1) {
		gpioToggle = 0;
		Pins[17].set(); // Set pin high
	} else {
		gpioToggle = 1;
		Pins[17].clr(); // Set pin low
	}
}
#endif

/*-------------------------------------------------------------------
 UserMain
 ----------------------------------------- \-------------------------*/
void UserMain(void *pd) {

	// will it work to open ports here?  Looks like it does! MPatten 4-4-13
	OpenSerialPorts();

	iprintf("\n---------------------------------------------------------");
	iprintf("\nFirmware version: %s\n", FirmwareVersion);

	InitializeStack();
	OSChangePrio(MAIN_PRIO);
	EnableAutoUpdate();

	CheckNVSettings();
	StartHTTP(NV_Settings.webpagePort);
	iprintf("Starting HTTP on port %ld\n", NV_Settings.webpagePort);
//	StartHTTP((WORD)9003);
	EnableTaskMonitor();
	RegisterPost();
//	CheckNVSettings(); used to be here

	if (NV_Settings.Password[0]) {
		update_authenticate_func = TestPassword;
		ipsetup_authenticate_func = TestPassword;
	} else {
		update_authenticate_func = NULL;
		ipsetup_authenticate_func = NULL;
	}

	iprintf("WebPage Port: %ld", NV_Settings.webpagePort);
	iprintf("\nUser Name: %s", NV_Settings.UserName);
	iprintf("\nPassword: %s\n", NV_Settings.Password);

	Settings_Changed = TRUE;
	FD_shutdown = EXTRA_IO_OFFSET + 1;
	pSB70LCDhcpObj = new DhcpObject(0);

	if (EthernetIP == 0) {
		iprintf("IP address is 0.0.0.0, we are trying DHCP \n");
		pDHCPOfferName = NV_Settings.DeviceName;

		pSB70LCDhcpObj->StartDHCP(); //Start DHCP
		if (OSSemPend(&pSB70LCDhcpObj->NotifySem, 10 * TICKS_PER_SECOND)
				== OS_TIMEOUT)
				//Wait 10 seconds
				{
			//DHCP did not initialize handle the error here
			iprintf(
					"DHCP failed to initialize, system has IP address of Zero\r\n");
		} else {
			iprintf("DHCP assigned the IP address of :");
			ShowIP(EthernetIP);
			iprintf("\n");
		}
	} else {
		iprintf("Static IP address set to :");
		ShowIP(EthernetIP);
		iprintf("\r\n");
	}
#ifdef  SNMP
	dbprintf( "Starting SNMP" );
	StartSnmpProcessor( MAIN_PRIO - 2 );
	dbprintf( "SNMP done\r\n" );
#endif
//	iprintf("? for debug commands\r\n");
	OSTimeDly(10);

#if TEST_GPIO_STUFF_FROM_MARK==1
	Pins[17].function(PIN17_GPIO); // Configure JP1-17 for GPIO
//	OSSimpleTaskCreatewName( blinkLine17, MAIN_PRIO-2, "LED Blinker");
#endif
#if USE_OS_FOR_WRITE_UART1==1
	OSSimpleTaskCreatewName(writeUART1ComMsg, MAIN_PRIO-1, "Write to Ladybug");
	// send requests for data
#endif

	dataOnSerialPortTime = GetPreciseTime();

	while (1) {

		readUART1ComMsg(); // process any data from receiver on COM port

#if USE_OS_FOR_WRITE_UART1!=1
		writeUART1ComMsg(); // send requests for data
#endif

		readUART0ComMsg(); // process any data from external COM port on UART0
//		toggleLine17();

		if (Settings_Changed) {
			dbprintf("Starting new settings\r\n");
			if ((NV_Settings.ConnectMode != SERIAL_CONNECT_CONNECT_NEVER)
					&& (NV_Settings.ConnectAddress == 0)) {
				IPADDR ipa = 0;
				int rv = GetHostByName(NV_Settings.ConnectName, &ipa, 0,
						TICKS_PER_SECOND * 10);

				if (rv == DNS_OK) {
					NV_Settings.ConnectAddress = ipa;
				} else {
					NV_Settings.ConnectAddress = 0;
				}
			}
			ClrHaveError(FD_shutdown);
			OpenSerialPorts();
			if ((NV_Settings.ConnectMode == SERIAL_CONNECT_CONNECT_AT_POWERUP)
					&& (NV_Settings.ConnectAddress)) {
				MakeConnection();
			}
			if (OkToListen()) {
				OpenListeningPort();
			} else {
				CloseListeningPort();
			}

			dbprintf("New settings established \r\n");
			Settings_Changed = FALSE;
		} else {
			fd_set fd_err;
			fd_set fd_rd;
			fd_set fd_wr;

			bNetWorkBoundDataBlocked = FALSE;

			bSerialBoundDataBlocked = FALSE;
			FD_ZERO(&fd_err);
			FD_ZERO(&fd_rd);
			FD_ZERO(&fd_wr);
			FD_SET(FD_shutdown, &fd_err);/* Listen for shutdown commands */

			if (FD_ListeningSocket > 0)
			/* IF we are listening for connections do that */
			{
				FD_SET(FD_ListeningSocket, &fd_err);
				FD_SET(FD_ListeningSocket, &fd_rd);
			}
			FD_SET(FD_SerialData, &fd_err);/* Always listen for serial errors */

#ifndef NODEBUG_PORT
			FD_SET(FD_SerialDebug, &fd_rd);/* always listen for debug data requests */
#endif

			if (!FD_ISSET(FD_SerialData, &write_file_sets)) {
				bSerialBoundDataBlocked = TRUE;
			} else {
				bSerialBoundDataBlocked = FALSE;
			}

			if (FD_ConnectedSocket > 0) {
				FD_SET(FD_ConnectedSocket, &fd_err);
				if (!bSerialBoundDataBlocked) {
					FD_SET(FD_ConnectedSocket, &fd_rd);
				} else {
					FD_SET(FD_SerialData, &fd_wr);
				}

				if (!FD_ISSET(FD_ConnectedSocket, &write_file_sets)) {
					bNetWorkBoundDataBlocked = TRUE;
					FD_SET(FD_ConnectedSocket, &fd_wr);
				} else {
					bNetWorkBoundDataBlocked = FALSE;
					FD_SET(FD_SerialData, &fd_rd);
				}
			} else {
				bNetWorkBoundDataBlocked = FALSE;
				FD_SET(FD_SerialData, &fd_rd);
			}
			if (Buf_N2S_End != Buf_N2S_Start) {
				FD_SET(FD_SerialData, &fd_wr);
			}

			waiting_fd_r = fd_rd;
			waiting_fd_w = fd_wr;
			waiting_fd_err = fd_err;
			/* Clock on the select forever */
			if (select(FD_SETSIZE, &fd_rd, &fd_wr, &fd_err,
					2 * TICKS_PER_SECOND)) {
				/* Now process the results of the select */

				/* First check for data to read */
				if (FD_ISSET(FD_shutdown, &fd_err)) {
					OSTimeDly(10);
					dbprintf("We are in shutdown\r\n");
				} else {
					/* Not in shutdown and reconfig */

#ifndef NODEBUG_PORT
					if (FD_ISSET(FD_SerialDebug, &fd_rd)) {
//						   toggleLine17();
						ProcessDebugdata();
					}

#endif

					if (FD_ISSET(FD_SerialData, &fd_err)) {
						ProcessSerialError();
					}

					if (FD_ISSET(FD_SerialData, &fd_rd)) {
						ProcessReadSerialData();
					}

					if (FD_ConnectedSocket > 0) {
						if (FD_ISSET(FD_ConnectedSocket, &fd_rd)) {
							ProcessReadNetworkData();
						}

						if (FD_ISSET(FD_ConnectedSocket, &fd_err)) {
							ProcessNetworkError();
						}

						if (FD_ISSET(FD_ConnectedSocket, &fd_wr)) {
							ProcessWriteNetworkData();
						}
					}
					if (FD_ListeningSocket > 0) {
						if (FD_ISSET(FD_ListeningSocket, &fd_err)) {
							ProcessListenError();
						}

						if (FD_ISSET(FD_ListeningSocket, &fd_rd)) {
							ProcessAccept();
						}
					}
					if (FD_ISSET(FD_SerialData, &fd_wr)) {
						ProcessWriteSerialData();
					}
				}
			}
			//End of if select
			if (!Settings_Changed) {
				ProcessTimeOuts();
			}
		}
		//Else normal processing
	}
	//While
	dbprintf("ERROR! We exited the universe\r\n");
}
// UserMain

