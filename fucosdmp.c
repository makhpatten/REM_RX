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
#include "includes.h"
#include "cfinter.h"
#include "constants.h"
#include "iosys.h"
#include "bsp.h"
#include <stdio.h>


extern void UCOSWAITS_HERE( void );

void fDumpTCBState( OS_TCB *pTcb, FILE *fp )
{
   DWORD *pStack;
   DWORD newpc;
   DWORD olda6;

   switch ( pTcb->OSTCBPrio )
   {
     case 0x3F:
       fiprintf( fp, " Idle" ); break;
     case  MAIN_PRIO:
       fiprintf( fp, " Main" ); break;
     case  HTTP_PRIO:
       fiprintf( fp, " HTTP" ); break;
     case  PPP_PRIO:
       fiprintf( fp, " PPP " ); break;
     case  TCP_PRIO:
       fiprintf( fp, " TCP " ); break;
     case  IP_PRIO:
       fiprintf( fp, " IP  " ); break;
     case  ETHER_SEND_PRIO:
       fiprintf( fp, " Esnd" ); break;
     case  (MAIN_PRIO-2)  :
       fiprintf( fp, " SNMP" ); break;
     default:
       fiprintf( fp, "%2d ", pTcb->OSTCBPrio );
   }

   fiprintf( fp, " |" );

   if ( OSTCBCur->OSTCBPrio == pTcb->OSTCBPrio )
   {
      fiprintf( fp, "Running " );
   }
   else
   {
      switch ( pTcb->OSTCBStat )
      {
        case OS_STAT_RDY  :
          if ( pTcb->OSTCBDly == 0 )
          {
             fiprintf( fp, "Ready    |" );
          }
          else
          {
             fiprintf( fp, "Timer    |" );
          }
          break;
        case OS_STAT_MBOX :
          fiprintf( fp, "Mailbox  |" ); break;
        case OS_STAT_SEM  :
          fiprintf( fp, "Semaphore|" ); break;
        case OS_STAT_Q    :
          fiprintf( fp, "Queue    |" ); break;
        case OS_STAT_FIFO :
          fiprintf( fp, "Fifo     |" ); break;
        case OS_STAT_CRIT :
          fiprintf( fp, "Critical |" ); break;
        default:
          fiprintf( fp, "  OSTCBStat = ??%i\r\n", pTcb->OSTCBStat );
          break;
      }
      if ( pTcb->OSTCBDly )
      {
         fiprintf( fp, " %5i  |", pTcb->OSTCBDly );
      }
      else
      {
         fiprintf( fp, "forever |" );
      }
      pStack = pTcb->OSTCBStkPtr;
      olda6 = pStack[14];
      pStack += 15;
      //Now we walk up the stack....simulate the RTE....
      newpc = pStack[1];
      if ( newpc == ( DWORD ) & UCOSWAITS_HERE )
      {
         int n = 0;
         while ( n < 5 )
         {
            newpc = *( ( DWORD * ) ( olda6 + 4 ) );
            olda6 = *( ( DWORD * ) ( olda6 ) );
            if ( ( olda6 < ( DWORD ) pTcb->OSTCBStkPtr ) ||
                 ( newpc == ( DWORD ) OSTaskDelete ) )
            {
               fiprintf( fp, "<END>" );
               break;
            }
            fiprintf( fp, "%08lx->", newpc );
            n++;
         }
      }
      else
      {
         fiprintf( fp, "at: %08lx ", newpc );
      }
   }
}

void fOSDumpTasks( FILE *fp )
{
   int index;
   fiprintf( fp, "uc/OS Tasks\r\nPrio  State    Ticks    Call Stack   \r\n" );
   UCOS_ENTER_CRITICAL();
   for ( index = 0; index < OS_MAX_TASKS; index++ )
   {
      if ( OSTCBTbl[index].OSTCBPrio )
      {
         fDumpTCBState( OSTCBTbl + index, fp );
         fiprintf( fp, "\r\n" );
      }
   }
   UCOS_EXIT_CRITICAL();
}



