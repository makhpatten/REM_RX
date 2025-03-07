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

#ifndef FORM_TOOL_H
#define FORM_TOOL_H

/* Functiosn to manage HTML form creation and data extraction */

/* Output a select */
/* Item 1 = first selection item !!! */
void FormOutputSelect(int sock, const char *name, int selnum, const char **list,
		int submitNowFlag);

/* Outout a Check box */
void FormOutputCheckbox(int sock, const char *name, BOOL checked,
		int submitNow);

/* Output an input box */
void FormOutputInput(int sock, const char *name, int siz, const char *val);

/* Output an input box for numbers */
void FormOutputNumInput(int sock, const char *name, int siz, int val);

/* Output an input box for long numbers */
void FormOutputLongNumInput(int sock, const char *name, int siz, long long unsigned int val);

/*Output an input box for IP addresses */
void FormOutputIPInput(int sock, const char *name, IPADDR ip);

/* Extract an IP address for the post data */
IPADDR FormExtractIP(const char *name, char *pData, IPADDR def_val);

/*Extract a number from the post data */
long FormExtractNum(const char *name, char *pData, long def_val);

/* Extract a Check box state from the post data */
BOOL FormExtractCheck(const char *name, char *pData, BOOL def_val);

/* extract a selection from a select box */
/* Item 1 = first selection item !!! */
int FormExtractSel(const char *name, char *pdata, const char **pList,
		int defsel);

void ShowIP2Sock(int sock, IPADDR ip);
//void ShowIP(IPADDR ia);

#endif

#define CONNECTED_LIST_STR_LEN 500
