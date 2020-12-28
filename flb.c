/***************************************************************************
 *                                                                         *
 * TRADE SECRETS NOTICE: ALL RIGHTS RESERVED. This material contains       *
 * valuable properties and trade secrets of Hugo Lyppens, embodying        *
 * substantial creative efforts and confidential information, ideas and    *
 * expressions, no part of which may be reproduced or transmitted in any   *
 * form or by any means, electronic, mechanical, or otherwise including    *
 * photographic and recording or in connection with any information        *
 * storage or retrieval system without written permission from             *
 * Hugo Lyppens.                                                           *
 *                                                                         *
 * COPYRIGHT NOTICE:                                                       *
 *                                                                         *
 * Copyright (C) 1993/1994 by Hugo Lyppens.                                     *
 * All rights reserved. An unpublished work.                               *
 *                                                                         *
 * Hugo Lyppens,                                                           *
 * Generaal Coenderslaan 25,                                               *
 * NL-5623 LT  Eindhoven.                                                  *
 * Telephone 31-40-461403                                                  *
 *                                                                         *
 ***************************************************************************/

#include <resources/disk.h>
#include <devices/trackdisk.h>
#include "vbs.h"
#include "LD_vbstext.h"

#define IOR(k) ((struct IORequest *)(k))

#define TD_READ         CMD_READ
#define BLOCKSIZE       TD_SECTOR
#define NUMHEADS        2
#define TRACKSINCACHE   4
#define GAPSIZE         0x500
#define BLOCKSPERTRACK  94
#define TRACKBUFSIZE    GAPSIZE+128*BLOCKSPERTRACK+8


static BOOL FLB_AllocTrackBuffers(int);
static LONG FLB_ChangeNum(int);
static int  FLB_ChangeState(int);
static void FLB_CleanupDrives(void);
static void FLB_CloseDrive(int);
static void FLB_DiskBusy(int, int);
static void FLB_FreeDrive(void);
static void FLB_FreeTrackBuffers(void);
static void FLB_GetDrive(int);
static void FLB_GotoTrack0(int);
static BOOL FLB_InitDrives(int);
static void FLB_MotorOn(int, BOOL);
static BOOL FLB_OpenDrive(int);
static BOOL FLB_ReadCylSec(int, UBYTE *, int, int, int);
static BOOL FLB_ReadNameDate(int, UBYTE *, struct DateStamp *);
static LONG FLB_StatusCmd(int, UWORD);


static UBYTE             FLB_drivename[][5] = {
      "DF0:", "DF1:", "DF2:", "DF3:"
};
static struct MsgPort   *FLB_diskport[4];
static struct IOExtTD   *FLB_diskreq[4];
static UBYTE             FLB_drives[4], FLB_numdrives;
static LONG              FLB_changenum[4];

extern BOOL              VBSG_nochange;

VBS_error_t FLBG_FloppyBackup(screen, flmask, mult, startpos)
struct Screen    *screen;
int               flmask;
BOOL              mult;
ULONG             startpos;
{
   struct DateStamp  date;
   UBYTE             name[31];
   UBYTE             datestring[20];
   UBYTE             string[60];
   BOOL              scrsetup = FALSE,driveopened = FALSE, tballoc = FALSE;
   LONG              starttime, starttime2, endtime, duration;
   LONG              cn;
   int               fl, curdr;
   VBS_error_t       error = EOK;
   BOOL              timergotten = FALSE;
   UBYTE            *decodetab  = NULL;
   register UBYTE   *decodetabp = NULL;
   register ULONG    i;

   if(FLB_InitDrives(flmask)) {
      error = EREAD;
      goto quit;
   }
   driveopened = TRUE;
   if(FLB_AllocTrackBuffers(4)) {
      error = EMEMORY; goto quit;
   }
   tballoc = TRUE;

   BAG_SetupVideoScreen(screen);
   scrsetup = TRUE;
   VBS_WaitFire();
   if(!(decodetab = (UBYTE *)AllocMem(65536, 0)))
      goto quit;
   decodetabp = decodetab;
   for(i = 0; i<65536; i++) {
/*      v = i-32768; maakt niets uit */
      *decodetabp++ = ((i >> 7) & 0xAA) | (i&0x55);
   }

   curdr = 0;
   starttime2 = -1;
   VBS_GetTimer(); timergotten = TRUE;
nextdisk:
   fl = FLB_drives[curdr];
   if(startpos!=DO_NOT_LOG) {
      starttime = VBS_ReadTimer();
      if(starttime2 != -1) {
         startpos += (starttime - starttime2) & 0x00FFFFFF;
      }
      starttime2 = starttime;
   }

   if(FLB_ReadNameDate(fl, name, &date)) {
      error = EREAD;
      goto quit;
   }
#ifdef FISH
   if(name[12] == '_') {
      movmem(name+13, name+12, 10);
   }
#endif
   FLB_GotoTrack0(fl);
   if(error = BAG_WriteHeader(BT_AMIGADOS, name, NULL, &date))
      goto quit;
   FLB_GetDrive(fl);

   scrsetup = TRUE;
   Forbid();
   if(!(error = VFG_BackupFloppy(fl, decodetab)) && startpos != DO_NOT_LOG) {
      endtime = VBS_ReadTimer();
      duration = (endtime - starttime) & 0x00FFFFFF;
#ifndef FISH
      UIG_Date(datestring, date.ds_Days);
      sprintf(string, "ADF %s "OFTX" %s", name, datestring);
#else
      sprintf(string, "ADF %s ", name);
#endif
      VMG_AddLogEntry(startpos, startpos+duration, string);
   }
   if(error)
      VBS_WriteErrorIndicator();
   Permit();
   FLB_FreeDrive();
   if(!error && mult) {
      FLB_changenum[fl] = FLB_ChangeNum(fl);
      if(!UIG_DelayLMB(30))
         goto quit;
      curdr++; if(curdr>=FLB_numdrives) curdr = 0;
      fl = FLB_drives[curdr];
      cn = FLB_changenum[fl];
      if(!VBSG_nochange && FLB_ChangeNum(fl)<=cn) {
         sprintf(string, REMDISKTX " %s", FLB_drivename[fl]);
         BAG_ShowMessage(screen, string, LMBABORTTX, 0x47A, 0x772);
         while(FLB_ChangeNum(fl)<=cn) {
            if(!UIG_DelayLMB(6)) goto quit;
         }
      }
      if(FLB_ChangeState(fl) & 1) {
         sprintf(string, INSERTDISKTX" %s", FLB_drivename[fl]);
         BAG_ShowMessage(screen, string, LMBABORTTX, 0x47A, 0x772);
         while(FLB_ChangeState(fl)&1) {
            if(!UIG_DelayLMB(30)) goto quit;
         }
      }
      VBS_ShowBlack();
      goto nextdisk;
   }
quit:
   if(decodetab)
      FreeMem(decodetab, 65536);
   if(timergotten)
      VBS_FreeTimer();
   if(scrsetup)
      BAG_RestoreVideoScreen();
   if(driveopened)
      FLB_CleanupDrives();
   if(tballoc)
      FLB_FreeTrackBuffers();

   return(error);
}
static BOOL FLB_InitDrives(flmask)
int      flmask;
{
   int   fl, mask;


   FLB_numdrives = 0;
   for(fl = 0, mask = 1; fl<4; fl++,mask <<= 1) {
      FLB_changenum[fl] = -1;
      if(flmask & mask) {
         FLB_drives[FLB_numdrives++] = fl;

         FLB_DiskBusy(fl, TRUE);

         if(FLB_OpenDrive(fl)) {
            FLB_CleanupDrives();
            return(TRUE);
         }
      }
   }
   return(FALSE);
}
static void FLB_CleanupDrives()
{
   int   i, fl;

   for(i = 0; i<FLB_numdrives; i++ ) {
      fl = FLB_drives[i];

      if(FLB_diskreq[fl])
         FLB_CloseDrive(fl);
      FLB_DiskBusy(fl, FALSE);
   }
   FLB_numdrives = 0;
}

static void FLB_DiskBusy(fl, onflag)
int   fl, onflag;
{
   struct StandardPacket   *pk;
   struct Process          *tsk;

   tsk = (struct Process *)FindTask(NULL);
   if(pk = AllocMem(sizeof(struct StandardPacket), MEMF_PUBLIC|MEMF_CLEAR)){
      pk->sp_Msg.mn_Node.ln_Name = (UBYTE *)&(pk->sp_Pkt);

      pk->sp_Pkt.dp_Link = &(pk->sp_Msg);
      pk->sp_Pkt.dp_Port = &(tsk->pr_MsgPort);
      pk->sp_Pkt.dp_Type = ACTION_INHIBIT;
      pk->sp_Pkt.dp_Arg1 = onflag ? -1L : 0L;

      PutMsg(DeviceProc(FLB_drivename[fl]), (struct Message *)pk);
      WaitPort(&(tsk->pr_MsgPort));
      GetMsg(&(tsk->pr_MsgPort));
      FreeMem(pk, sizeof(*pk));
   }
}



static BOOL FLB_OpenDrive(fl)
int      fl;
{
   FLB_diskport[fl] = NULL;
   FLB_diskreq[fl]  = NULL;

   if(!(FLB_diskport[fl] = CreatePort(0, 0)))
      goto error;
   if(!(FLB_diskreq[fl] = (struct IOExtTD *)CreateExtIO(FLB_diskport[fl],
      sizeof(struct IOExtTD))))
         goto error;
   if(OpenDevice("trackdisk.device", fl, IOR(FLB_diskreq[fl]), 0))
      goto error;
   return(FALSE); /*OK*/
error:
   if(FLB_diskreq[fl]) {
      DeleteExtIO(IOR(FLB_diskreq[fl])/*, sizeof(struct IOExtTD)*/);
   }
   if(FLB_diskport[fl]) {
      DeletePort(FLB_diskport[fl]);
   }
   return(TRUE);
}

static void
FLB_GetDrive(fl)
int      fl;
{
   register struct DiscResourceUnit *drunit;
   register void                    *LastDriver;

   drunit = (struct DiscResourceUnit *)(FLB_diskreq[fl]->iotd_Req.io_Unit);

   for (;;) {
      drunit->dru_Message.mn_Node.ln_Type = NT_MESSAGE;
      LastDriver = GetUnit(drunit);

      if (LastDriver) {
          break;
      } else {
          while (drunit->dru_Message.mn_Node.ln_Type != NT_REPLYMSG)
          Wait(1L << drunit->dru_Message.mn_ReplyPort->mp_SigBit);
          Remove(&drunit->dru_Message.mn_Node);
      }
   }
}

static void
FLB_FreeDrive()
{
   GiveUnit();
}
static void FLB_CloseDrive(fl)
int      fl;
{
   CloseDevice(IOR(FLB_diskreq[fl]));
   DeleteExtIO(IOR(FLB_diskreq[fl])/*, sizeof(struct IOExtTD)*/);
   DeletePort(FLB_diskport[fl]);
   FLB_diskreq[fl]  = NULL;
   FLB_diskport[fl] = NULL;
}

static void FLB_MotorOn(fl, on)
int         fl;
BOOL        on;
{
   FLB_diskreq[fl]->iotd_Req.io_Length  = on;
   FLB_diskreq[fl]->iotd_Req.io_Command = TD_MOTOR;
   DoIO(IOR(FLB_diskreq[fl]));
}
static void FLB_GotoTrack0(fl)
int         fl;
{
   FLB_diskreq[fl]->iotd_Req.io_Offset  = 0;
   FLB_diskreq[fl]->iotd_Req.io_Command = TD_SEEK;
   DoIO(IOR(FLB_diskreq[fl]));
}

static BOOL FLB_ReadCylSec(fl, buf, cyl, sec, hd)
int      fl;
UBYTE   *buf;
int      cyl, sec, hd;
{
   FLB_StatusCmd(fl, CMD_CLEAR);

   FLB_diskreq[fl]->iotd_Req.io_Length  = BLOCKSIZE;
   FLB_diskreq[fl]->iotd_Req.io_Data    = (APTR)buf;
   FLB_diskreq[fl]->iotd_Req.io_Command = TD_READ;

   FLB_diskreq[fl]->iotd_Req.io_Offset  = TD_SECTOR*(sec + NUMSECS*hd +
                                          (NUMSECS*NUMHEADS)*cyl);
   DoIO(IOR(FLB_diskreq[fl]));
   return((BOOL)(FLB_diskreq[fl]->iotd_Req.io_Error));
}
static BOOL FLB_ReadNameDate(fl, name, date)
int                fl;
UBYTE             *name;
struct DateStamp  *date;
{
   UBYTE    *buffer = NULL;
   int       len;
   BOOL      error = TRUE;

   *name = 0;
   if(!(buffer = AllocMem(BLOCKSIZE, MEMF_CHIP)))
      goto quit;

   FLB_MotorOn(fl, TRUE);
   if(FLB_ReadCylSec(fl, buffer, 40, 0, 0))
      goto quit;
   len = buffer[512-20*4];
   if(len>0 && len<=30) {
      memcpy(name, &buffer[512-20*4+1], len);
      name[len] = 0;
   } else {
      strcpy(name, "NDOS");
   }
   memcpy((UBYTE *)date, &buffer[512-7*4], 3*4);
   error = FALSE;
quit:
   FLB_MotorOn(fl, FALSE);
   if(buffer)
      FreeMem(buffer, BLOCKSIZE);
   return(error);
}

BOOL  FLBG_DriveAvailable(fl)
int      fl;
{

   if(!FLB_OpenDrive(fl)) {
      FLB_CloseDrive(fl); return(TRUE);
   }
   return(FALSE);
}

VBS_error_t FLBG_FloppyRestore(screen, name, flmask, mult)
struct Screen    *screen;
UBYTE            *name;
int               flmask;
BOOL              mult;
{
   VBS_error_t    error = EOK;
   BOOL           scrsetup = FALSE, driveopened = FALSE, tballoc = FALSE;
   UBYTE          string[60];
   int            fl, curdr;
   LONG           cn;

   if(FLB_InitDrives(flmask)) {
      error = EWRITE; goto quit;
   }
   driveopened = TRUE;
   if(FLB_AllocTrackBuffers(2)) {
      error = EMEMORY; goto quit;
   }
   tballoc = TRUE;
   BAG_SetupScreen(screen);
   scrsetup = TRUE;
   curdr = 0;
nextdisk:
   fl = FLB_drives[curdr];
   error = BAG_Search(BT_AMIGADOS, name, NULL, NULL);
   if(error)
      goto quit;
   BAG_PrintMsg(NOWRESTORINGFLTX);
   FLB_GotoTrack0(fl);
   FLB_GetDrive(fl);
   Forbid();
   error = VFG_RestoreFloppy(fl);
   Permit();
   FLB_FreeDrive();
   if(!error && mult) {
      BAG_PrintMsg(RESTOREOKLMBTX);
      FLB_changenum[fl] = FLB_ChangeNum(fl);
      if(!UIG_DelayLMB(10)) goto quit;
      curdr++; if(curdr>=FLB_numdrives) curdr = 0;
      fl = FLB_drives[curdr];
      cn = FLB_changenum[fl];

      if(!VBSG_nochange && FLB_ChangeNum(fl)<=cn) {
         sprintf(string, REMDISKTX" %s", FLB_drivename[fl]);
         BAG_PrintMsg(string);
         while(FLB_ChangeNum(fl)<=cn) {
            if(!UIG_DelayLMB(2)) goto quit;
         }
      }

      if(FLB_ChangeState(fl)&1) {
         sprintf(string, INSERTWEDISKTX" %s", FLB_drivename[fl]);
         BAG_PrintMsg(string);
         while(FLB_ChangeState(fl)&1) {
            if(!UIG_DelayLMB(30)) goto quit;
         }
      }
      sprintf(string, "%s "LOADEDTX, FLB_drivename[fl]);
      BAG_PrintMsg(string);
      name[0] = '\0';
      goto nextdisk;
   }

quit:
   if(scrsetup)
      BAG_RestoreScreen();

   if(driveopened)
      FLB_CleanupDrives();

   if(tballoc)
      FLB_FreeTrackBuffers();

   FLB_DiskBusy(fl, FALSE);
   return(error);
}
static APTR    FLB_trackbuffer[TRACKSINCACHE];

static BOOL FLB_AllocTrackBuffers(n)
int      n;
{
   BOOL     error = FALSE;
   int      i;

   for(i = 0; i<n; i++) {
      FLB_trackbuffer[i] =
         AllocMem(TRACKBUFSIZE, MEMF_CHIP);
      error |= !FLB_trackbuffer[i];
   }
   if(error) {
      FLB_FreeTrackBuffers();
   }
   VFG_SetBuffers(FLB_trackbuffer[0], FLB_trackbuffer[1],
                  FLB_trackbuffer[2], FLB_trackbuffer[3]);
   return(error);
}

static void FLB_FreeTrackBuffers()
{
   int      i;

   for(i = 0; i<TRACKSINCACHE; i++) {
      if(FLB_trackbuffer[i]) {
         FreeMem(FLB_trackbuffer[i], TRACKBUFSIZE);
         FLB_trackbuffer[i] = NULL;
      }
   }
}
static LONG     FLB_StatusCmd(fl, cmd)
int      fl;
UWORD    cmd;
{
   FLB_diskreq[fl]->iotd_Req.io_Command = cmd;
   DoIO(IOR(FLB_diskreq[fl]));
   return((LONG)FLB_diskreq[fl]->iotd_Req.io_Actual);
}

static LONG     FLB_ChangeNum(fl)
int      fl;
{
   return(FLB_StatusCmd(fl, TD_CHANGENUM));
}
static int      FLB_ChangeState(fl)
int      fl;
{
   return((FLB_StatusCmd(fl, TD_CHANGESTATE)!=0) |
          ((FLB_StatusCmd(fl, TD_PROTSTATUS)!=0)<<1));
}

