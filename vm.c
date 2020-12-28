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
 * Copyright (C) 1993 by Hugo Lyppens.                                     *
 * All rights reserved. An unpublished work.                               *
 *                                                                         *
 * Hugo Lyppens,                                                           *
 * Generaal Coenderslaan 25,                                               *
 * NL-5623 LT  Eindhoven.                                                  *
 * Telephone 31-40-461403                                                  *
 *                                                                         *
 ***************************************************************************/

#include <string.h>

#include "vbs.h"
#include "LD_vbstext.h"
#include "vm_ui.h"
#include "VD_vm_ui.c"

#define BCOLOR       2
#define TIMECOLOR    0
#define BARCOLOR     5
#define STRINGCOLOR  4
#define SPACELEFTLOG 6

char lev[]="wk5";
typedef struct VM_LogNode VM_LogNode_t;

struct  VM_LogNode {
   VM_LogNode_t   *Next, *Prev;
   ULONG           StartPos, EndPos;
   UBYTE           StringLen;
   char            String[1];
};

char                   VMG_backuptype[][4] = {
   "ILL", "FS", "ADF", "MSF"
};

extern short           IMG_pickpt[];
static void            VM_Cleanup(void);
static void            VM_CShowLog(void);
static void            VM_DeleteLogNode(VM_LogNode_t *);
static void            VM_FreeLog(void);
static BOOL            VM_Init(void);
static void            VM_InsertLogNode(VM_LogNode_t *, ULONG);
static int             VM_HandleGadget(USHORT, ULONG, int, int);
static BOOL            VM_LoadLog(char *);
static BOOL            VM_SaveLog(char *);
static void            VM_ShowLog(int, int);
static VM_LogNode_t   *VM_SkipNodes(VM_LogNode_t *, int);

enum { NOTHING = 0, QUIT };

static struct RastPort     VM_logrp;
static struct Screen      *VM_screen;
static struct Window      *VM_window;
static char               *VM_fontname;
static int                 VM_fontheight;

static BOOL                VM_keep_log = FALSE, VM_del_entry = FALSE;
static BOOL                VM_log_changed = FALSE;
static VM_LogNode_t       *VM_logkey = NULL;
static   VBS_ListData_t    VM_loglist = {
   &VM_loggadget, &VM_loghoriz, &VM_logvert,
   &VM_logarrowleft, &VM_logarrowright,  &VM_logarrowup, &VM_logarrowdown
};
static char                VM_logfilename[60];
char                       VMG_logname[30];
static char               *VM_logdrawer;
struct TextAttr            VBS_ta8 = {
   "topaz.font", 8, 0, 0
};

extern BOOL                VBSG_novideocheck;
extern USHORT              IMG_logim0data[], IMG_logim1data[];

#define LOGIMWIDTH         70
#define LOGIMHEIGHT        36

static struct Image        VM_logiconim0={
   0, 0, LOGIMWIDTH, LOGIMHEIGHT, 2,
   &IMG_logim0data[0],
   3, 0, NULL
};

static struct Image        VM_logiconim1={
   0, 0, LOGIMWIDTH, LOGIMHEIGHT, 2,
   &IMG_logim1data[0],
   3, 0, NULL
};

static struct DiskObject   VM_logobj={
   WB_DISKMAGIC,
   WB_DISKVERSION,
   {
      NULL, 0, 0,
      LOGIMWIDTH, LOGIMHEIGHT,
      GADGIMAGE|GADGHIMAGE,
      RELVERIFY|GADGIMMEDIATE,
      BOOLGADGET,
      (APTR)&VM_logiconim0,
      (APTR)&VM_logiconim1,
   },
   WBPROJECT,
   "VBS:VBS",
   NULL,
   NO_ICON_POSITION,NO_ICON_POSITION,
   NULL,
   NULL,
   NULL
};

void  VMG_Menu(initiallog, fontname, fontheight, drawer)
char     *initiallog;
char     *fontname;
int       fontheight;
char     *drawer;
{
   struct IntuiMessage  *message;
   struct Gadget        *g;
   USHORT                code, gid;
   ULONG                 class;
   int                   mouse_x, mouse_y, to_do;

   VM_logdrawer  = drawer;
   NewScreenStructure.Font = &VBS_ta8;
   VM_screen = OpenScreen(&NewScreenStructure);
   if(!VM_screen)
      goto quit;
   LoadRGB4(&VM_screen->ViewPort, &Palette[0], PaletteColorCount);
   VM_fontname   = fontname;
   VM_fontheight = fontheight;

   VM_Init();
   if(initiallog && !VM_LoadLog(initiallog)) {
      strcpy(VM_logfilename, initiallog);
      strcpy(VMG_logname, initiallog);
      VM_keep_log = TRUE;
      UIG_ChangeGadgetText(VM_window, &VM_logfilegadget, DISABLELOGTX, 0);
      UIG_ChangeGadgetText(VM_window, &VM_lognamegadget, VMG_logname, 1);
   }
   VM_CShowLog();

lus:
   while(!(message = (struct IntuiMessage *)GetMsg(VM_window->UserPort)))
      WaitPort(VM_window->UserPort);

   code = message->Code;class = message->Class;
   g = NULL;
   if(class==GADGETDOWN || class==GADGETUP){
      g = (struct Gadget *)(message->IAddress);
      gid = g->GadgetID;
   }
   mouse_x = message->MouseX; mouse_y = message->MouseY;

   ReplyMsg((struct Message *)message);
   if(VM_del_entry && (!g || gid != LOG_WINDOW)) {
      VM_del_entry = FALSE;
      ClearPointer(VM_window);
   }
   switch(class) {
   case GADGETUP:
   case GADGETDOWN:
      to_do = VM_HandleGadget(gid, class, mouse_x, mouse_y);
      break;
   }
   switch(to_do) {
   case QUIT:
      goto quit;
   }
   goto lus;
quit:
   VM_Cleanup();
   if(VM_screen)
      CloseScreen(VM_screen);
   VM_FreeLog();
   VM_screen    = NULL;
   VM_window    = NULL;
}

static int       VM_HandleGadget(gid, class, mouse_x, mouse_y)
USHORT      gid;
ULONG       class;
int         mouse_x, mouse_y;
{
   char     *path, *name, *fullname;
   int       flmask;
   int       leftx, toprow;
   BOOL      mult;
   VBS_error_t error;
   ULONG     startpos;
   VM_LogNode_t   *delnode;


   if(class != GADGETUP)
      return(NOTHING);

   if(gid>=LOG_HORIZ && gid<=LOG_ARROW_DOWN) {
      UIG_NewView(VM_window, &VM_loglist,
                  gid-LOG_HORIZ, &leftx, &toprow);
      VM_ShowLog(leftx, toprow);
      return(NOTHING);
   }
   switch(gid) {
   case FLOPPY_BACKUP:
      startpos = DO_NOT_LOG;
      flmask = FLG_AskFloppy(VM_window, VM_keep_log ? &startpos : NULL, &mult);
      if(flmask && BAG_CheckConnection(VM_screen, VM_window)) {
         VM_Cleanup();
         error = FLBG_FloppyBackup(VM_screen, flmask, mult, startpos);
         VM_Init();
         if(error)
            ERRG_Message(VM_window, error);
      }
      break;
   case FLOPPY_RESTORE:
      name = NTG_AskNameTime(VM_window, NULL, NULL, NULL);
      if(name) {
         flmask = FLG_AskFloppy(VM_window, NULL, &mult);
         if(flmask) {
            VM_Cleanup();
            error = FLBG_FloppyRestore(VM_screen, name, flmask, mult);
            VM_Init();
            if(error)
               ERRG_Message(VM_window, error);
         }
      }
      break;
   case FS_BACKUP:
      path = PAG_AskPath(VM_window, ENTERPATHBACKTX);
      if(path) {
         VM_Cleanup();
         error = HDBG_FSBackup(VM_screen, path, VM_fontname, VM_fontheight,
                               VM_keep_log);
         VM_Init();
         if(error)
            ERRG_Message(VM_window, error);
      }
      break;
   case FS_RESTORE:
      name = NTG_AskNameTime(VM_window, NULL, NULL, NULL);
      if(name) {

         VM_Cleanup();
         error = HDBG_FSRestore(VM_screen, name, VM_fontname, VM_fontheight);
         VM_Init();
         if(error)
            ERRG_Message(VM_window, error);
      }
      break;
   case FS_VERIFY:
      name = NTG_AskNameTime(VM_window, NULL, NULL, NULL);
      if(name) {
         VM_Cleanup();
         error = HDBG_FSVerify(VM_screen, name, VM_fontname, VM_fontheight);
         VM_Init();
         if(error)
            ERRG_Message(VM_window, error);
      }
      break;
   case LOG_FILE:
      if(!VM_keep_log) {
         if(!(fullname = FileRequest(VM_screen, VM_window, SELECTLOGTX,
                                     VM_logdrawer, &name, NULL)))
            break;

         if(VM_LoadLog(fullname))
            break;

         strcpy(VM_logfilename, fullname);
         strcpy(VMG_logname, name);
         VM_keep_log = TRUE;
         UIG_ChangeGadgetText(VM_window, &VM_logfilegadget, DISABLELOGTX, 0);
         UIG_ChangeGadgetText(VM_window, &VM_lognamegadget, VMG_logname, 1);
      } else {
         if(VM_log_changed &&
           ERRG_Message(VM_window, EUPDATELOG)==ERROR_RETRY) {
            if(VM_SaveLog(VM_logfilename)) {
               DisplayBeep(VM_screen); break;
            }
         }
         VM_FreeLog();
         VM_keep_log = FALSE;
         UIG_ChangeGadgetText(VM_window, &VM_logfilegadget, LOGFILETX, 0);
         UIG_ChangeGadgetText(VM_window, &VM_lognamegadget, NONETX, 1);
      }
      VM_CShowLog();
      break;
   case UPDATE_LOG:
      if(VM_keep_log) {
         VM_SaveLog(VM_logfilename);
      }
      break;
   case CREATE_NEW_LOG:
      if(!VM_keep_log) {
         if(!(fullname = FileRequest(VM_screen, VM_window, NEWLOGTX,
                                     VM_logdrawer, &name, NULL)))
            break;
         if(UIG_FileExists(fullname)) {
            ERRG_Message(VM_window, ELOGEXISTS);
            break;
         }
         VM_logkey      = NULL;
         if(VM_SaveLog(fullname)) {
            ERRG_Message(VM_window, ECOULDNTCREATELOG);
            break;
         }
         if(IconBase)
            PutDiskObject(fullname, &VM_logobj);
         AddDirNode(name, 0);
         strcpy(VM_logfilename, fullname);
         strcpy(VMG_logname, name);
         VM_keep_log    = TRUE;
         VM_log_changed = FALSE;
         UIG_ChangeGadgetText(VM_window, &VM_logfilegadget, DISABLELOGTX, 0);
         UIG_ChangeGadgetText(VM_window, &VM_lognamegadget, VMG_logname, 1);
         VM_CShowLog();
      }
      break;
   case DEL_LOG_ENTRY:
      if(VM_keep_log) {
         SetPointer(VM_window, &IMG_pickpt[0], 16, 16, -2, -1);
         VM_del_entry = TRUE;
      }
      break;
   case LOG_WINDOW:
      if(VM_del_entry) {
         delnode = VM_SkipNodes(VM_logkey, VM_loglist.TopRow +
                   (mouse_y-VM_loggadget.TopEdge)/VM_logrp.TxHeight);
         if(delnode) {
            VM_DeleteLogNode(delnode);
            FreeMem((char *)delnode, sizeof(VM_LogNode_t)+delnode->StringLen);
            VM_CShowLog();
         }
         VM_del_entry = FALSE;
         ClearPointer(VM_window);
      }
      break;
   case QUIT_VBS:
      if(VM_keep_log && VM_log_changed) {
         if(ERRG_Message(VM_window, EUPDATELOG)==ERROR_RETRY) {
            if(VM_SaveLog(VM_logfilename)) {
               DisplayBeep(VM_screen); break;
            }
         }
      }
      return(QUIT);
   default:
      ERRG_Message(VM_window, ENOTIMPLEMENTED);
      break;
   }
   return(NOTHING);
}
static BOOL      VM_Init()
{
   BOOL             error = TRUE;

   VM_window    = NULL;

   VM_window = UIG_OpenWindow(VM_screen, &NewWindowStructure1, FILLCOLOR);
   if(!VM_window)
      goto quit;

   UIG_InitPad(&VM_loglist, VM_window, VM_fontname, VM_fontheight);

   RefreshGadgets(VM_window->FirstGadget, VM_window, NULL);

   VM_logrp = *VM_loglist.RastPort;

   VM_CShowLog();
   error = FALSE;
quit:
   return(error);
}

static void   VM_Cleanup()
{
   UIG_CleanupPad(&VM_loglist);

   FreeDir();
   if(VM_window) {
      CloseWindow(VM_window);
   }
   VM_window    = NULL;
}

static BOOL VM_LoadLog(filename)
char     *filename;
{
   BOOL  error = TRUE;
   BPTR  fn;
   char  line[150], buf[150], *p, *bufptr, *endptr, *d;
   int   l;
   ULONG startpos, endpos;
   char  c;

   VM_loglist.NumEntries    = 0;
   VM_loglist.NumRows       = 0;
   VM_loglist.NumCols       = 1;
   VM_loglist.MaxTotalWidth = 0;
   VM_loglist.MaxEltWidth   = 0;

   bufptr = &buf[0]; endptr = bufptr;
   fn = Open(filename, MODE_OLDFILE);
   if(!fn)
      goto quit;

   for(;;) {
      d = line;
kloot:
      while(bufptr < endptr) {
         if((c = *bufptr++) != '\n') {
            *d++=c;
         } else {
            *d = '\0'; goto lijnklaar;
         }
      }
      bufptr = &buf[0];
      l = Read(fn, &buf[0], sizeof(buf));
      if(l<0)
         goto quit;
      endptr = bufptr + l;
      if(!l) {
         if(d!=line) {
            *d++ = 0; goto lijnklaar;
         } else {
            goto klaar;
         }
      }
      goto kloot;
lijnklaar:
      p = line;
      l = VMG_StringToTime(p, &startpos);
      if(l<=0)
         continue;
      p += l;
      p = stpblk(p);
      if(*p == '-')
         p++;
      p = stpblk(p);
      l = VMG_StringToTime(p, &endpos);
      if(l<=0)
         continue;
      p += l; p = stpblk(p);
      VMG_AddLogEntry(startpos, endpos, p);
   }
klaar:
   error = FALSE;
quit:
   if(fn)
      Close(fn);
   VM_log_changed = FALSE;
   return(error);
}

int VMG_StringToTime(p, time)
register char     *p;
ULONG             *time;
{
   int           i, l, te;
   UBYTE         timeelements[3];
   char         *pold;
   static UBYTE  maxelements[3] = {
      24, 60, 60
   };
   pold = p;
   for(i = 0; i<3; i++) {
      if(i && *p++ != ':')
         return(0);
      l = stcd_i(p, &te);
      if(l<=0 || l>2 || te<0 || te >= maxelements[i])
         return(0);
      timeelements[i] = (UBYTE)te;
      p+=l;
   }
   *time = (((timeelements[0] * 60)+timeelements[1])*60+timeelements[2])*
           JIFFIES_PER_SECOND;
   return(p-pold);
}
int VMG_TimeToString(string, time)
char    *string;
ULONG    time;
{
   sprintf(string, "%01ld:%02ld:%02ld", time/(60*60*JIFFIES_PER_SECOND),
           (time / (60*JIFFIES_PER_SECOND))%60, (time/JIFFIES_PER_SECOND)%60);
   return(8);
}

BOOL  VMG_AddLogEntry(startpos, endpos, string)
ULONG  startpos, endpos;
char  *string;
{
   char     totalstring[80], startstring[9], endstring[9];
   BOOL     error = TRUE;
   int      w, l;
   VM_LogNode_t   *newnode;


   VMG_TimeToString(startstring, startpos);
   VMG_TimeToString(endstring, endpos);
   sprintf(totalstring, "%s-%s %s", startstring, endstring, string);
   l = strlen(totalstring);
   newnode  = (VM_LogNode_t *)AllocMem(sizeof(VM_LogNode_t)+l, MEMF_CLEAR);
   if(!newnode)
      goto error;
   VM_InsertLogNode(newnode, startpos);
   newnode->StartPos  = startpos;
   newnode->EndPos    = endpos;
   newnode->StringLen = l;
   strcpy(newnode->String, totalstring);
   w = TextLength(&VM_logrp, totalstring, l);
   if(w+SPACELEFTLOG > VM_loglist.MaxTotalWidth)
      VM_loglist.MaxTotalWidth = w+SPACELEFTLOG;
   if(w>VM_loglist.MaxEltWidth)
      VM_loglist.MaxEltWidth = w;
   error = FALSE; /*OK*/
error:
   return(error);
}

static void VM_InsertLogNode(newnode, startpos)
VM_LogNode_t  *newnode;
ULONG          startpos;
{
   VM_LogNode_t  *prevnode, *node;

   node = VM_logkey;
   if(!node) {
      VM_logkey = newnode;
   } else {
      prevnode = NULL;
      while(node && startpos > node->StartPos) {
         prevnode = node;
         node     = node->Next;
      }
      newnode->Prev = prevnode;
      if(prevnode) {
         prevnode->Next = newnode;
      } else
         VM_logkey = newnode;
      if(node)
         node->Prev = newnode;
      newnode->Next = node;
   }
   VM_log_changed = TRUE;
   VM_loglist.NumEntries ++;VM_loglist.NumRows ++;

}
static void  VM_DeleteLogNode(delnode)
VM_LogNode_t   *delnode;
{
   if(delnode->Prev)
      delnode->Prev->Next = delnode->Next;
   else
      VM_logkey = delnode->Next;

   if(delnode->Next)
      delnode->Next->Prev = delnode->Prev;

   VM_loglist.NumEntries --;VM_loglist.NumRows --;
   VM_log_changed = TRUE;
}

static void VM_CShowLog()
{
   UIG_AdjustProps(VM_window, &VM_loglist, 0, 0);
   SetRast(&VM_logrp, VM_keep_log ? BCOLOR : 0);
   if(VM_keep_log)
      VM_ShowLog(0, 0);
}


static void VM_ShowLog(leftx, toprow)
int             leftx, toprow;
{
   VM_LogNode_t      *node;
   int                row;
   int                x, y;
   int                winrows;

   winrows = VM_loglist.WinRows;
   SetAPen(&VM_logrp, 0);
   SetBPen(&VM_logrp, BCOLOR);
   if(leftx  < 0) leftx  = VM_loglist.LeftX;
   if(toprow < 0) toprow = VM_loglist.TopRow;
   ScrollRaster(&VM_logrp,
                leftx - VM_loglist.LeftX,
                (toprow - VM_loglist.TopRow)*VM_logrp.TxHeight,
                0, 0,
                VM_loglist.WinWidth-1, VM_loglist.WinHeight-1);

   if(VM_loglist.NumRows < winrows) {
      winrows  = VM_loglist.NumRows;
      toprow = 0;
   }
   node = VM_logkey;
   x = SPACELEFTLOG-leftx;
   node = VM_SkipNodes(node, toprow);
   y = VM_logrp.TxBaseline;
   for(row = 0; node && row<winrows; row++) {
      Move(&VM_logrp, SPACELEFTLOG-leftx, y);
      SetAPen(&VM_logrp, TIMECOLOR);
      Text(&VM_logrp, node->String, 7);
      SetAPen(&VM_logrp, BARCOLOR);
      Text(&VM_logrp, "-", 1);
      SetAPen(&VM_logrp, TIMECOLOR);
      Text(&VM_logrp, node->String+8, 7);
      SetAPen(&VM_logrp, STRINGCOLOR);
      Text(&VM_logrp, node->String+15, node->StringLen-15);
      y += VM_logrp.TxHeight;
      node = node->Next;
   }

   VM_loglist.TopRow = toprow;
   VM_loglist.LeftX  = leftx;
}

static VM_LogNode_t   *VM_SkipNodes(ln, n)
VM_LogNode_t      *ln;
int                n;
{
   while(n && ln) {
      ln = ln->Next;
      n--;
   }
   return(ln);
}
static void       VM_FreeLog()
{
   VM_LogNode_t   *node, *thisnode;

   node = VM_logkey;
   while(node) {
      thisnode = node;
      node = node->Next;
      FreeMem((char *)thisnode, sizeof(VM_LogNode_t)+thisnode->StringLen);
   }
   VM_logkey = NULL;

   VM_loglist.NumEntries    = 0;
   VM_loglist.NumRows       = 0;
   VM_loglist.NumCols       = 1;
   VM_loglist.MaxTotalWidth = 0;
}
static BOOL    VM_SaveLog(name)
char     *name;
{
   BPTR            fn;
   VM_LogNode_t   *node;
   BOOL            error = TRUE;

   fn = Open(name, MODE_NEWFILE);
   if(!fn)
      goto quit;
   node = VM_logkey;
   while(node) {
      if(Write2(fn, node->String, node->StringLen))
         goto quit;
      if(Write2(fn, "\n", 1))
         goto quit;
      node = node->Next;
   }
   if(Write2(fn, "\n", 1))
      goto quit;
   VM_log_changed = FALSE;
   error = FALSE;
quit:
   if(fn)
      Close(fn);
   return(error);
}


