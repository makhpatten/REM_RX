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

#include "predef.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <startnet.h>
#include <ip.h>
#include <tcp.h>
#include <htmlfiles.h>
#include <http.h>
#include <string.h>
#include "formtools.h"

void FormOutputSelect(int sock, const char *name, int selnum, const char **list,
		int submitNow) {
	writestring(sock, "<SELECT NAME=\"");
	writestring(sock, name);
	writestring(sock, "\" SIZE=1");
	if (submitNow != 0) {
		writestring(sock, " onchange=\"this.form.submit()\"");
	}
	writestring(sock, ">");
	const char **lp = list;
	int n = 0;
	while (lp[n][0]) {
		if ((n + 1) == selnum) {
			writestring(sock, "<OPTION SELECTED>");
		} else {
			writestring(sock, "<OPTION>");
		}
		writesafestring(sock, lp[n]);
		n++;
	}
	writestring(sock, "</SELECT>\n");
}

char tbuf[40];
void ShowIP2Sock(int sock, IPADDR ip) {
	PBYTE ipb = (PBYTE) &ip;
	siprintf(tbuf, "(%d.%d.%d.%d)", (int) ipb[0], (int) ipb[1], (int) ipb[2],
			(int) ipb[3]);
	writestring(sock, tbuf);
}

void FormOutputCheckbox(int sock, const char *name, BOOL checked,
		int submitNow) {
	writestring(sock, "<INPUT TYPE=\"checkbox\"");
	if (submitNow != 0) {
		writestring(sock, " onclick=\"this.form.submit()\"");
	}
	writestring(sock, " NAME=\"");
	writestring(sock, name);
	if (checked) {
		writestring(sock, "\" VALUE=\"checked\" CHECKED>\n");
	} else {
		writestring(sock, "\" VALUE=\"checked\">\n");
	}
}

void FormOutputInput(int sock, const char *name, int siz, const char *val) {
	char buf[10];

	writestring(sock, "<INPUT NAME=\"");
	writestring(sock, name);
	writestring(sock, "\" TYPE=\"text\" SIZE=\"");
	siprintf(buf, "%d", siz);
	writestring(sock, buf);
	writestring(sock, "\" VALUE=\"");
	writesafestring(sock, val);
	writestring(sock, "\" >\n");
}

void FormOutputNumInput(int sock, const char *name, int siz, int val) {
	char buf[20];
	siprintf(buf, "%d", val);
	FormOutputInput(sock, name, siz, buf);
}

void FormOutputLongNumInput(int sock, const char *name, int siz, long long unsigned int val) {
	char buf[20];
	siprintf(buf, "%lld", val);
	FormOutputInput(sock, name, 20, buf);
}
void FormOutputIPInput(int sock, const char *name, IPADDR ip) {
	char buf[20];
	PBYTE ipb = (PBYTE) &ip;
	siprintf(buf, "%d.%d.%d.%d", (int) ipb[0], (int) ipb[1], (int) ipb[2],
			(int) ipb[3]);
	FormOutputInput(sock, name, 20, buf);
}

IPADDR FormExtractIP(const char *name, char *pData, IPADDR def_val) {
	char rx_buf[40];
	if (ExtractPostData(name, pData, rx_buf, 40)) {
//		iprintf("\nFormExtractIP name=%s\n",name);
		return AsciiToIp(rx_buf);
	}
	return def_val;
}

/*
 void ShowIP(IPADDR ia) {
	PBYTE ipb = (PBYTE) &ia;
	iprintf("%d.%d.%d.%d", (int) ipb[0], (int) ipb[1], (int) ipb[2],
			(int) ipb[3]);
}
*/

long FormExtractNum(const char *name, char *pData, long def_val) {
	char rx_buf[40];
	if (ExtractPostData(name, pData, rx_buf, 40)) {
		return atol(rx_buf);
	}
	return def_val;
}

BOOL FormExtractCheck(const char *name, char *pData, BOOL def_val) {
	char rx_buf[40];
	int i;
	if (ExtractPostData(name, pData, rx_buf, 40)) {
		if ((rx_buf[0] == 'C') || (rx_buf[0] == 'c')) {
			return TRUE;
		} else {
			return FALSE;
		}
	}
	return def_val;
}

int FormExtractSel(const char *name, char *pData, const char **pList,
		int defsel) {
	char rx_buf[100];
	if (ExtractPostData(name, pData, rx_buf, 40)) {
		int n = 0;
		while (pList[n][0]) {
			if (strcmp(pList[n], rx_buf) == 0) {
				return n + 1;
			}
			n++;
		}
	}
	return defsel;
}

