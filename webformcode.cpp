#include <stdio.h>
#include <string.h>
#include <constants.h>
#include <htmlfiles.h>
#include <iosys.h>
#include <http.h>
#include <utils.h>

#include "webformcode.h"
#include "serialburnerdata.h"

extern char bufForVuAndRssi[];
extern int rssiA, rssiB, vuL, vuR;
extern int lockA, lockB;

//==============================================================================
// This page holds the C++ code that is exposed to the HTML pages for use
// as callbacks. We minimize the callbacks by using javascript when possible.
//==============================================================================

//==============================================================================
// Simple sample code to show how to implement a function callback from
// webpage
//==============================================================================
extern "C" {
void rssiShow(char *x, int y, int z);
void vuShow(char *x, int y);
void txIdShow(char *buf);
void txVoltageShow(char *buf);
void txRSSIShow(char *buf);
void controlTxLEDShow(char *buf);
void secsTimeoutShow(char *buf);

//==============================================================================
// Example function that can be called via AJAX. This one just writes out the
// the current timetick value but of course this could be much more elaborate.
//==============================================================================
void AjaxCallback(int sock, const char* url) {
	char java_script[100];
	sprintf(java_script, " System Tick count:%lu", TimeTick);

	writestring(sock, java_script);
}

void AjaxCallbackReload(int sock, const char* url) {

	siprintf(
			bufForVuAndRssi,
			"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\" width=\"400\">");
	strcat(
			bufForVuAndRssi,
			"<tr><td width=\"10%\" align=\"right\"><font size=\"2\"><b> RSSI A: </b></font> </td>");
	rssiShow(bufForVuAndRssi, rssiA, lockA);
	strcat(
			bufForVuAndRssi,
			"</tr><tr><td width=\"10%\" align=\"right\"><font size=\"2\"><b> RSSI B: </b></font></td>");
	rssiShow(bufForVuAndRssi, rssiB, lockB);
	strcat(
			bufForVuAndRssi,
			"</tr><tr><td></td><td></td><td></td><td><font size=\"2\"><b>Lock</b></font></td></tr>");
	strcat(
			bufForVuAndRssi,
			"<tr><td width=\"10%\" align=\"right\"><font size=\"2\"><b>Level L: </b></font></td>");
	vuShow(bufForVuAndRssi, vuL);
	strcat(
			bufForVuAndRssi,
			"</tr><tr><td width=\"10%\" align=\"right\"><font size=\"2\"><b>Level R: </b></font> </td>");
	vuShow(bufForVuAndRssi, vuR);

	controlTxLEDShow(bufForVuAndRssi);
	strcat(
			bufForVuAndRssi,
			"</font></td></tr><tr><td></td><td></td><td></td><td></td></tr><td colspan=\"4\">");
	strcat(
			bufForVuAndRssi,
			"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\" width=\"600\">");
	strcat(
			bufForVuAndRssi,
			"<tr><td width=\"13%\" align=\"right\"><font size=\"2\"><b>TxID: </b></font> </td><td width=\"30%\" align=\"left\">");
	txIdShow(bufForVuAndRssi);
	strcat(bufForVuAndRssi, "</td><td></td><td></td></tr>");
	strcat(
			bufForVuAndRssi,
			"<tr><td width=\"13%\" align=\"right\"><font size=\"2\"><b>Tx Voltage: </b></font></td><td width=\"30%\" align=\"left\">");
	txVoltageShow(bufForVuAndRssi);
	strcat(bufForVuAndRssi, "</td><td></td><td></td></tr>");

	strcat(
			bufForVuAndRssi,
			"<tr><td width=\"13%\" align=\"right\"><font size=\"2\"><b>Cmd RSSI: </b></font></td><td width=\"30%\" align=\"left\">");
	txRSSIShow(bufForVuAndRssi);
	strcat(bufForVuAndRssi, "</td><td></td><td></td></tr>");



	strcat(bufForVuAndRssi,"<tr><td width=\"13%\" align=\"right\"><font size=\"2\"><b>Secs until timeout: </b></font> </td><td width=\"30%\" align=\"left\">");
	secsTimeoutShow(bufForVuAndRssi);
	strcat(bufForVuAndRssi,"<tr><td width=\"13%\" align=\"right\"></td><td width=\"30%\" align=\"left\">");
	strcat(bufForVuAndRssi, "</td><td></td><td></td></tr></tr></table>");

	writestring(sock, bufForVuAndRssi);

}

int chanLock[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
extern int requestedChannelNumber;
extern int channelScanFlag;
extern int receivedTxStatus;
extern int receivedChannelNumber;

void AjaxCallbackChanLock(int sock, const char* url) {
	int i;
//	chanLock[7]=chanLock[0];
//	for (i=0;i<7;i++) {
//		chanLock[i]=chanLock[i+1];
//	}
	siprintf(
			bufForVuAndRssi,
			"<table><tr><td align=\"center\"><b>Lock</b></td><td align=\"center\"><b>Select</b></td></tr>");
	for (i = 7; i >= 0; i--) {
		strcat(bufForVuAndRssi, "<tr><td align=\"center\">");
		if (chanLock[i] == 1) {
			strcat(bufForVuAndRssi, "<image src=\"LEDGray.PNG");
			strcat(bufForVuAndRssi, "\" width=\"20\" height=\"20\">");
		} else if (chanLock[i] == 2) {
			strcat(bufForVuAndRssi, "<image src=\"LEDOn.PNG");
			strcat(bufForVuAndRssi, "\" width=\"20\" height=\"20\">");
		} else {
			strcat(bufForVuAndRssi, "<image src=\"LEDBlank.PNG");
			strcat(bufForVuAndRssi, "\" width=\"20\" height=\"20\">");
		}
		strcat(bufForVuAndRssi, "</td><td>");
		if (receivedChannelNumber == i + 1) {
//			if (receivedTxStatus == 1) {
//				strcat(bufForVuAndRssi,"<image src=\"arrow.jpg\" width=\"30\" height=\"20\" align=\"right\"");
//			} else {
			strcat(
					bufForVuAndRssi,
					"<image src=\"arrow_green.jpg\" width=\"30\" height=\"20\" align=\"right\"");
//			}
		}
		strcat(bufForVuAndRssi, "</td></tr>");
	}
	strcat(bufForVuAndRssi, "</table>");
	writestring(sock, bufForVuAndRssi);
}

void AjaxCallbackTxStatus2(int sock, const char* url) {
	int i;
//	chanLock[7]=chanLock[0];
//	for (i=0;i<7;i++) {
//		chanLock[i]=chanLock[i+1];
//	}
	siprintf(bufForVuAndRssi," ");
	if (receivedTxStatus == 1) {
		strcat(bufForVuAndRssi, "<img src=\"");
		strcat(bufForVuAndRssi, "LEDOnRed.png");
//		strcat(bufForVuAndRssi, "waves.gif");
//		strcat(bufForVuAndRssi, "Rectangle_Transmitting.png");
		strcat(bufForVuAndRssi, "\" width=\"80\" height=\"40\">");
	} else {
		strcat(bufForVuAndRssi, "<img src=\"");
		strcat(bufForVuAndRssi, "LEDBlank.png");
//		strcat(bufForVuAndRssi, "waves.gif");
//		strcat(bufForVuAndRssi, "Rectangle_Transmitting.png");
		strcat(bufForVuAndRssi, "\" width=\"80\" height=\"40\">");

	}
	writestring(sock, bufForVuAndRssi);
}

int callbackCount = 0;
extern int allChannelsFlag[];
void AjaxCallbackChanStatus(int sock, const char* url) {
	int i;
	char buf[5];
//	chanLock[7]=chanLock[0];
//	for (i=0;i<7;i++) {
//		chanLock[i]=chanLock[i+1];
//	}

	siprintf(bufForVuAndRssi, "");
	/*
	 if (receivedTxStatus == 1) {
	 strcat(bufForVuAndRssi, "<td width=\"15%%\"><img src=\"");
	 strcat(bufForVuAndRssi, "LEDOnRed.png");
	 strcat(bufForVuAndRssi, "\" width=\"20\" height=\"20\"></td>");
	 } else {
	 strcat(bufForVuAndRssi, "<td width=\"15%%\"></td>");
	 }
	 if (receivedTxStatus == 1) {
	 strcat(bufForVuAndRssi, "<td><b>Txmttng</b></td>");
	 } else {
	 strcat(bufForVuAndRssi, "<td></td>");
	 }
	 strcat(bufForVuAndRssi, "</tr></table><br>");
	 */
	if (channelScanFlag == 1) {
#if 1==1
		strcat(bufForVuAndRssi, "<br>Scanning... ");
#endif
	}
//	strcat(bufForVuAndRssi, "Current channel: ");
//	siprintf(buf,"%d ",receivedChannelNumber);
//	strcat(bufForVuAndRssi, buf);
//	for (i=0;i<8;i++) {
//		siprintf(buf," %d ",allChannelsFlag[i]);
//		strcat(bufForVuAndRssi, buf);
//	}
//	strcat(bufForVuAndRssi, "]");
	writestring(sock, bufForVuAndRssi);
}

}

extern int dataReceivedFlag;

void AjaxCallbackTxStatus(int sock, const char* url) {
	int i;
	char buf[5];
//	chanLock[7]=chanLock[0];
//	for (i=0;i<7;i++) {
//		chanLock[i]=chanLock[i+1];
//	}


	siprintf(bufForVuAndRssi, " ");

 	siprintf(bufForVuAndRssi, "<table><tr>");
//	if (dataReceivedFlag == 0) {
//		strcat(buf, "<td colspan=\"3\">No Received Data</td>");
//	} else {
		if (receivedTxStatus == 1) {
			strcat(
					bufForVuAndRssi,
					"<td width=\"100\"></td><td width=\"10\"></td><td width=\"130\" >");
			strcat(bufForVuAndRssi, "<img src=\"");
			strcat(bufForVuAndRssi, "LEDOnRed.png");
//			strcat(bufForVuAndRssi, "waves.gif");
//			strcat(bufForVuAndRssi, "Rectangle_Transmitting.png");
			strcat(bufForVuAndRssi, "\" width=\"80\" height=\"40\"></TD>");
		} else {
			strcat(
					bufForVuAndRssi,
					"<td width=\"100\"></td><td width=\"15\"></td><td width=\"25\"></td><td width=\"25\"></td>");
			strcat(bufForVuAndRssi, "<td width=\"120\"></TD>");
		}
//	}
	strcat(bufForVuAndRssi, "</tr></table>");

	writestring(sock, bufForVuAndRssi);

}

extern int dataReceivedFlag;

void rssiAshow(int sock, PCSTR url) {
	rssiShow(bufForVuAndRssi, rssiA, lockA);
	writestring(sock, bufForVuAndRssi);
}

void rssiBshow(int sock, PCSTR url) {
	rssiShow(bufForVuAndRssi, rssiB, lockB);
	writestring(sock, bufForVuAndRssi);
}

char tempBuf[300];
void rssiShow(char *buf, int rssi, int lock) {
	int x;
	if (dataReceivedFlag == 0) {
		strcat(buf, "<td colspan=\"2\">No Received Data  </td>");
	} else {
		x = -100;
//		printf("\n\n Rssi=%d\n\n", rssi);
		siprintf(tempBuf, "<TD width=\"60\">%d dBm </TD>", rssi);
		strcat(buf, tempBuf);
		strcat(
				buf,
				"<td width=\"100\"><TABLE class=\"vu_meter\" id=\"vuLI\"><tbody>");
		int i;
		for (i = 0; i < 30; i++) {
			strcat(buf, "<TD style=\"background-color:");
			if (rssi > x) {
				strcat(buf, "lime");
			} else {
				strcat(buf, "lightgrey");
			}
			strcat(buf, "\"></TD>");
			x += 2;
		}
		strcat(buf, "</TBODY></TABLE></td>");
		strcat(buf, "<td width=\"160\" align=\"left\"><img src=\"");
		if (lock == 1) {
			strcat(buf, "LEDOn.png");
		} else {
			strcat(buf, "LEDGray.png");
		}
		strcat(buf, "\" width=\"20\" height=\"20\"></td>");

	}

}

void vuShow(char *buf, int vu) {
	int x;
	if (dataReceivedFlag == 0) {
		strcat(buf, "<td colspan=\"2\">No Received Data  </td>");
	} else {
		x = 0;
		siprintf(tempBuf, "<TD width=\"15%%\">%d </TD>", vu);
		strcat(buf, tempBuf);
		strcat(buf, "<td><TABLE class=\"vu_meter\" id=\"vuLI\"><tbody>");
		int i;
		for (i = 0; i < 32; i++) {
			strcat(buf, "<TD style=\"background-color:");
			if (vu > x) {
				strcat(buf, "lime");
			} else {
				strcat(buf, "lightgrey");
			}
			strcat(buf, "\"></TD>");
			x += 8;
		}
		strcat(buf, "</TBODY></TABLE>");
	}
}

void controlTxLEDShow(char *buf) {

//	if (receivedTxStatus == 1) {
//		strcat(buf, "<td width=\"15%%\"><img src=\"");
//		strcat(buf, "LEDOnRed.png");
//		strcat(buf, "\" width=\"80\" height=\"40\"></td>");
//	} else {
		strcat(buf, "<td width=\"15%%\"></td>");
//	}
	strcat(buf, "</tr><tr><td></td><td></td><td></td><td><font size=\"2\">");

	/*
	 if (receivedTxStatus == 1) {
	 strcat(buf, "<b>Txmttng</b>");
	 } else {
	 strcat(buf, " ");
	 }
	 */

}

extern long unsigned int txID;
extern long unsigned int txVoltage;
extern long unsigned int txRSSI;



void txIdShow(char *buf) {
	if (dataReceivedFlag == 0) {
		strcat(buf, "No Received Data  ");
	} else {
		if (txID==0) {
			siprintf(tempBuf, " - ", txID);
			strcat(buf, tempBuf);
		} else {
			siprintf(tempBuf, "%ld", txID);
			strcat(buf, tempBuf);
		}
	}
}

void txVoltageShow(char *buf) {
	if (dataReceivedFlag == 0) {
		siprintf(buf, "No Received Data  ");
	} else {
		if (txVoltage==0) {
			sprintf(tempBuf, "  -  V", (float) txVoltage / 10.0);
			strcat(buf, tempBuf);
		} else {
			sprintf(tempBuf, "%4.1f V", (float) txVoltage / 10.0);
			strcat(buf, tempBuf);
		}
	}
}

void txRSSIShow(char *buf) {
	if (dataReceivedFlag == 0) {
		siprintf(buf, "No Received Data  ");
	} else {
		if (txRSSI==0) {
			sprintf(tempBuf, "  -  dBm", (float) txRSSI / -10.0);
			strcat(buf, tempBuf);
		} else {
			sprintf(tempBuf, "%4.1f dBm", (float) txRSSI / -10.0);
			strcat(buf, tempBuf);
		}
	}
}

extern NV_SettingsStruct NV_Settings;
extern DWORD LoggedInTimer;
void secsTimeoutShow(char *buf) {
	float secsRemaining;
	secsRemaining = ((float) NV_Settings.PasswordTimeout * 1000000.0
			- ((float) GetPreciseTime() - (float) LoggedInTimer)) / 1000000.0;
	if (secsRemaining < 0.0)
		secsRemaining = 0.0;
	if (NV_Settings.Password[0] == 0) {
       sprintf(tempBuf, "OFF (no password entered)");
	} else {
	   sprintf(tempBuf, "%16.0f secs", secsRemaining);
	}
	strcat(buf, tempBuf);
}

/*
 //==============================================================================
 // Post Handler - When the user submits a form this method is called
 // Set up using SetNewPostHandler() via the RegisterPost() method
 //==============================================================================
 int MyDoPost( int sock, char *url, char *pData, char *rxBuffer )
 {
 const char REDIRECT_PAGE[]   = "index.htm";

 //We have to respond to the post with a new HTML page...
 //In this case we will redirect so the browser will go to that URL for the response...
 RedirectResponse( sock, REDIRECT_PAGE );

 return 0;
 }
 */

/*
 //==============================================================================
 // Install the method we want to have handle form submissions
 //==============================================================================
 void RegisterPost()
 {
 SetNewPostHandler( MyDoPost );
 }
 */
