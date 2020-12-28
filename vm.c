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
 * Copyright (C) 1993/1994 by Hugo Lyppens.                                *
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
#include "VM_ui.c"

#define BCOLOR       5
#define TIMECOLOR    2
#define BARCOLOR     2
#define STRINGCOLOR  2
#define FILLCOLOR    4
#define BOXCOLOR     7
#define SPACELEFTLOG 6

UBYTE lev[]="wk5";
typedef struct VM_LogNode VM_LogNode_t;

struct  VM_LogNode {
   VM_LogNode_t   *Next, *Prev;
   ULONG           StartPos, EndPos;
   UBYTE           StringLen;
   UBYTE           String[1];
};

UBYTE                  VMG_backuptype[][4] = {
   "ILL", "FS", "ADF", "MSF"
};

extern BOOL            VBSG_usepubscreen;
extern UBYTE           VBSG_pubscreenname[];
VBS_screentype_t       VBSG_screentype;
extern VBS_countertype_t VBSG_countertype;
extern ULONG           VBSG_displayid;

extern short           IMG_pickpt[];
void                   VMG_Cleanup(void);
static void            VM_CShowLog(void);
static void            VM_DeleteLogNode(VM_LogNode_t *);
static BOOL            VM_DoubleClick(VM_LogNode_t *);
static void            VM_FreeLog(void);
BOOL                   VMG_Init(void);
static void            VM_InsertLogNode(VM_LogNode_t *, ULONG);
static VOID            VM_InvertRect(int);
static int             VM_HandleGadget(USHORT, ULONG, USHORT, int, int);
static int             VM_HandleVanillaKey(USHORT);
static BOOL            VM_LoadLog(UBYTE *);
static VOID            VM_RefreshLog(VOID);
static BOOL            VM_SaveLog(UBYTE *);
VOID                   VMG_ScreenCleanup(VOID);
BOOL                   VMG_ScreenInit(VOID);
static void            VM_ShowLog(int, int);
static VM_LogNode_t   *VM_SkipNodes(VM_LogNode_t *, int);
static VOID            VM_SelectNode(VM_LogNode_t *);
static VOID            VM_UnSelectNode(VOID);

enum { NOTHING = 0, REOPEN, QUIT };

static struct RastPort     VM_logrp;
struct Screen             *VMG_screen;
static struct Window      *VM_window;

extern struct TextAttr     VBSG_text_ta;
extern struct TextAttr     VBSG_pad_ta;

struct TextFont           *VM_textfont = NULL,
                          *VM_padfont = NULL;

static struct Process     *VM_thisprocess;
static struct Window      *VM_oldwindow;

static BOOL                VM_keep_log = FALSE, VM_del_entry = FALSE;
static BOOL                VM_log_changed = FALSE;
static VM_LogNode_t       *VM_logkey = NULL;

static struct Gadget      *VM_loggadget, *VM_horscroller, *VM_verscroller,
                          *VM_lognamegadget, *VM_logitgadget, *VM_delgadget;
static VBS_ListData_t      VM_loglist;

static ULONG               VM_seconds = 0,
                           VM_micros = 0;
static VM_LogNode_t       *VM_selectednode = NULL;

static UBYTE               VM_logfilename[60];
UBYTE                      VMG_logname[30];
static UBYTE              *VM_logdrawer;
struct TextAttr            VBS_ta8 = {
   "topaz.font", 8, 0, 0
};

extern BOOL                VBSG_novideocheck;
extern USHORT              IMG_logim0data[], IMG_logim1data[];
extern UBYTE               VBSG_errorinfo[];

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
   "VBS3:VBS",
   NULL,
   NO_ICON_POSITION,NO_ICON_POSITION,
   NULL,
   NULL,
   NULL
};

static struct Gadget  *glist = NULL;
static void           *visinfo;

BOOL  VMG_Menu(initiallog, drawer)
UBYTE           *initiallog;
UBYTE           *drawer;
{
   struct IntuiMessage    *message;
   struct Gadget          *g;
   USHORT                  code, gid;
   ULONG                   class;
   int                     mouse_x, mouse_y, to_do = NOTHING;

   VM_logdrawer  = drawer;
   if(initiallog && !VM_LoadLog(initiallog))
   {
      strcpy(VM_logfilename, initiallog);
      strcpy(VMG_logname, initiallog);
      VM_keep_log = TRUE;
   }

   if(VMG_ScreenInit())
      goto quit;

   if(VMG_Init())
      goto quit;

   VM_CShowLog();

lus:
   while(!(message = GT_GetIMsg(VM_window->UserPort)))
      WaitPort(VM_window->UserPort);

   code = message->Code;class = message->Class;
   g = NULL;
   if(class==IDCMP_MOUSEMOVE || class==IDCMP_GADGETDOWN || class==IDCMP_GADGETUP)
   {
      g = (struct Gadget *)(message->IAddress);
      gid = g->GadgetID;
   }
   mouse_x = message->MouseX; mouse_y = message->MouseY;

   GT_ReplyIMsg(message);

   switch(class)
   {
      case IDCMP_MOUSEMOVE:
      case IDCMP_GADGETUP:
      case IDCMP_GADGETDOWN:
         to_do = VM_HandleGadget(gid, class, code, mouse_x, mouse_y);
         VM_RefreshLog();
         break;
      case IDCMP_VANILLAKEY:
         to_do = VM_HandleVanillaKey(code);
         VM_RefreshLog();
         break;
      case IDCMP_REFRESHWINDOW:
         GT_BeginRefresh(VM_window);
         VM_RefreshLog();
         GT_EndRefresh(VM_window, TRUE);
         to_do = NOTHING;
         break;
   }
   switch(to_do)
   {
      case REOPEN:
      case QUIT:
         goto quit;
   }
   goto lus;
quit:
   VMG_Cleanup();
   VMG_ScreenCleanup();
   if(to_do==REOPEN)
      return(TRUE);
   else
   {
      VM_FreeLog();
      return(FALSE);
   }
}

static int VM_HandleVanillaKey(USHORT code)
{
   int    result;
   USHORT gid;
   UBYTE  key;

   key = (UBYTE)code;
   switch(key)
   {
      case 'f':
      case 'F':
         gid = FLOPPY_BACKUP;
         break;
      case 'o':
      case 'O':
         gid = FLOPPY_RESTORE;
         break;
      case 'b':
      case 'B':
         gid = FS_BACKUP;
         break;
      case 'r':
      case 'R':
         gid = FS_RESTORE;
         break;
      case 'v':
      case 'V':
         gid = FS_VERIFY;
         break;
      case 'n':
      case 'N':
         gid = NEW_LOG;
         break;
      case 'l':
      case 'L':
         gid = LOAD_LOG;
         break;
      case 's':
      case 'S':
         gid = SAVE_LOG;
         break;
#ifdef DEUTSCH
      case 'i':
      case 'I':
         gid = SETTINGS;
         break;
      case 'e':
      case 'E':
         gid = QUIT_VBS;
         break;
/*      case 'ä':
      case 'Ä':
         GT_SetGadgetAttrs(VM_logitgadget, VM_window, NULL,
                           GTCB_Checked, !(VM_logitgadget->Flags & GFLG_SELECTED),
                           TAG_END);
         gid = LOG_ACTIONS;
         break;*/
      case 'ö':
      case 'Ö':
         gid = DEL_LOG_ENTRY;
         break;
#else
      case 'e':
      case 'E':
         gid = SETTINGS;
         break;
      case 'q':
      case 'Q':
         gid = QUIT_VBS;
         break;
      case 'a':
      case 'A':
         GT_SetGadgetAttrs(VM_logitgadget, VM_window, NULL,
                           GTCB_Checked, !(VM_logitgadget->Flags & GFLG_SELECTED),
                           TAG_END);
         gid = LOG_ACTIONS;
         break;
      case 'd':
      case 'D':
         gid = DEL_LOG_ENTRY;
         break;
#endif
      default:
         return(NOTHING);
   }
   result = VM_HandleGadget(gid, IDCMP_GADGETUP, 0, 0, 0);
   return(result);
}

static int       VM_HandleGadget(gid, class, code, mouse_x, mouse_y)
USHORT      gid;
ULONG       class;
USHORT      code;
int         mouse_x, mouse_y;
{
   UBYTE         backupname[128];
   UBYTE         type[4];
   UBYTE        *name, *fullname, *cp;
   int           flmask;
   int           leftx, toprow;
   BOOL          mult;
   VBS_error_t   error;
   ULONG         startpos;
   VM_LogNode_t *node;

   if(class==IDCMP_MOUSEMOVE || class==IDCMP_GADGETDOWN)
   {
      switch(gid)
      {
         case LOG_HORIZ:
            leftx  = (int)code;
            toprow = VM_loglist.TopRow;
            VM_ShowLog(leftx, toprow);
            break;
         case LOG_VERT:
            leftx  = VM_loglist.LeftX;
            toprow = (int)code;
            VM_ShowLog(leftx, toprow);
            break;
         case LOG_WINDOW:
            node = VM_SkipNodes(VM_logkey, VM_loglist.TopRow +
                   (mouse_y-VM_loggadget->TopEdge)/VM_logrp.TxHeight);
            if(node)
            {
               if(VM_DoubleClick(node))
               {
                  if(VBSG_countertype==TIMEDCT)
                  {
                     strncpy(type, node->String+16, 3);
                     strcpy(backupname, node->String+20);
                  }
                  else
                  {
                     strncpy(type, node->String+6, 3);
                     strcpy(backupname, node->String+10);
                  }
                  type[3] = 0;
                  cp = backupname + strlen(backupname);
                  while(*--cp!=' ');
                  cp--;
                  while(*--cp!=' ');
                  if(cp > backupname)
                     *cp = 0;
                  else
                     goto exit_log_window;
                  if(!strncmp(type, VMG_backuptype[2], 3) ||
                     !strncmp(type, VMG_backuptype[3], 3))
                  {
                     flmask = FLG_AskFloppy(VM_window, NULL, &mult, NULL, RESTOREOT);
                     if(flmask)
                     {
                        VMG_Cleanup();
                        error = FLBG_FloppyRestore(VMG_screen, backupname, flmask, mult);
                        VMG_Init();
                        if(error)
                           ERRG_Message(VM_window, error);
                        VM_UnSelectNode();
                        VM_RefreshLog();
                     }
                  }
                  if(!strncmp(type, VMG_backuptype[1], 2))
                  {
/*                     VMG_Cleanup();*/
                     error = HDBG_FSRestore(VM_window, backupname);
/*                     VMG_Init();*/
                     if(error)
                        ERRG_Message(VM_window, error);
                     VM_UnSelectNode();
                     VM_RefreshLog();
                  }
               }
               else
                  VM_SelectNode(node);
            }
exit_log_window:
            break;
      }
   }
   else
   {
      switch(gid)
      {
         case FLOPPY_BACKUP:
            startpos = DO_NOT_LOG;
            flmask = FLG_AskFloppy(VM_window, VM_keep_log ? &startpos : NULL,
                                   &mult, NULL, BACKUPOT);
            if(flmask && BAG_CheckConnection(VMG_screen, VM_window))
            {
               VMG_Cleanup();
               error = FLBG_FloppyBackup(VMG_screen, flmask, mult, startpos);
               VMG_Init();
               if(error)
                  ERRG_Message(VM_window, error);
            }
            VM_UnSelectNode();
            break;
         case FLOPPY_RESTORE:
            flmask = FLG_AskFloppy(VM_window, NULL, &mult, &name, RESTOREOT);
            if(flmask)
            {
               VMG_Cleanup();
               error = FLBG_FloppyRestore(VMG_screen, name, flmask, mult);
               VMG_Init();
               if(error)
                  ERRG_Message(VM_window, error);
            }
            VM_UnSelectNode();
            break;
         case FS_BACKUP:
            VMG_Cleanup();
            error = HDBG_FSBackup(VM_keep_log);
            VMG_Init();
            if(error)
               ERRG_Message(VM_window, error);
            VM_UnSelectNode();
            break;
         case FS_RESTORE:
            name = NTG_AskNameTime(VM_window, NULL, NULL, NULL, RESTOREOT);
            if(name)
            {
               error = HDBG_FSRestore(VM_window, name);
               if(error)
                  ERRG_Message(VM_window, error);
            }
            VM_UnSelectNode();
            break;
         case FS_VERIFY:
            name = NTG_AskNameTime(VM_window, NULL, NULL, NULL, VERIFYOT);
            if(name)
            {
               error = HDBG_FSVerify(VM_window, name);
               if(error)
                  ERRG_Message(VM_window, error);
            }
            VM_UnSelectNode();
            break;
         case LOAD_LOG:
            if(VM_log_changed)
            {
               switch(ERRG_Message(VM_window, EUPDATELOG)) {
               
               case ERROR_CANCEL:
                  	return(NOTHING);
               case ERROR_RETRY:
                     if(VM_SaveLog(VM_logfilename))
                     {
                        DisplayBeep(VMG_screen); break;
                     }
               }
            }
         
            VM_UnSelectNode();

            fullname = FileRequest(VM_window, SELECTLOGTX,
                                   VM_logdrawer, &name, NULL, LOADRT);
            if(!fullname)
               break;

            VM_FreeLog();

            UIG_BlockInput(VM_window);

            if(VM_LoadLog(fullname))
            {
               ERRG_Message(VM_window, ECOULDNTLOADLOG);
               goto exit_loadlog;
            }

            strcpy(VM_logfilename, fullname);
            strcpy(VMG_logname, name);
            VM_keep_log = TRUE;
            GT_SetGadgetAttrs(VM_logitgadget, VM_window, NULL,
                           GTCB_Checked, TRUE,
                           GA_Disabled,  FALSE,
                           TAG_END);
            GT_SetGadgetAttrs(VM_lognamegadget, VM_window, NULL,
                           GTTX_Text, VMG_logname,
                           TAG_END);
            VM_CShowLog();
exit_loadlog:
            UIG_UnBlockInput(VM_window);            
            break;
         case SAVE_LOG:
            VM_SaveLog(VM_logfilename);
            VM_UnSelectNode();
            break;
         case NEW_LOG:
            if(VM_log_changed)
            {
               switch(ERRG_Message(VM_window, EUPDATELOG)) {
               
               case ERROR_CANCEL:
                  	return(NOTHING);
               case ERROR_RETRY:
                     if(VM_SaveLog(VM_logfilename))
                     {
                        DisplayBeep(VMG_screen); break;
                     }
               }
            }       
            VM_UnSelectNode();

            if(VM_log_changed)
            {
               switch(ERRG_Message(VM_window, EUPDATELOG))
               {
                  case ERROR_CANCEL:
                     return(NOTHING);
                     break;
                  case ERROR_RETRY:
                     if(VM_SaveLog(VM_logfilename))
                     {
                        DisplayBeep(VMG_screen); break;
                     }
                  case ERROR_OTHER:
                     break;
               }
            }
            if(!(fullname = FileRequest(VM_window, NEWLOGTX,
                                        VM_logdrawer, &name, NULL, SAVERT)))
               break;
            VM_FreeLog();
            if(UIG_FileExists(fullname))
            {
               strcpy(VBSG_errorinfo, fullname);
               ERRG_Message(VM_window, ELOGEXISTS);
               break;
            }
            VM_logkey      = NULL;
            if(VM_SaveLog(fullname))
            {
               ERRG_Message(VM_window, ECOULDNTCREATELOG);
                  break;
            }
            if(IconBase)
               PutDiskObject(fullname, &VM_logobj);
            strcpy(VM_logfilename, fullname);
            strcpy(VMG_logname, name);
            VM_keep_log    = TRUE;
            VM_log_changed = FALSE;
            GT_SetGadgetAttrs(VM_logitgadget, VM_window, NULL,
                           GTCB_Checked, TRUE,
                           GA_Disabled,	 FALSE,
                           TAG_END);
            GT_SetGadgetAttrs(VM_lognamegadget, VM_window, NULL,
                           GTTX_Text, VMG_logname,
                           TAG_END);
            VM_CShowLog();
            break;
         case DEL_LOG_ENTRY:
            if(VM_selectednode)
            {
               VM_DeleteLogNode(VM_selectednode);
               FreeMem((UBYTE *)VM_selectednode, sizeof(VM_LogNode_t)+VM_selectednode->StringLen);
               VM_selectednode = NULL;
               VM_log_changed = TRUE;
               VM_CShowLog();
               VM_UnSelectNode();
            }
            break;
            
         case LOG_ACTIONS:
            if(VM_logitgadget->Flags & GFLG_DISABLED)
               break;
            if(VM_logitgadget->Flags & GFLG_SELECTED)
               VM_keep_log = TRUE;
            else
               VM_keep_log = FALSE;
            VM_UnSelectNode();
            break;
         case SETTINGS:
            if(SETG_Settings(VM_window))
               return(REOPEN);
            VM_UnSelectNode();
            VM_RefreshLog();
            break;
         case QUIT_VBS:
            if(VM_log_changed)
            {
               switch(ERRG_Message(VM_window, EUPDATELOG))
               {
                  case ERROR_CANCEL:
                     return(NOTHING);
                     break;
                  case ERROR_RETRY:
                     if(VM_SaveLog(VM_logfilename))
                     {
                        DisplayBeep(VMG_screen); break;
                     }
                     return(QUIT);
                     break;
                  case ERROR_OTHER:
                     return(QUIT);
                     break;
               }
            }
            else
               return(QUIT);
         default:
            ERRG_Message(VM_window, ENOTIMPLEMENTED);
            break;
      }
   }
   return(NOTHING);
}

BOOL      VMG_Init()
{
   BOOL              error = TRUE, laced;
   UWORD             winleft, wintop, winwidth, winheight, xsize,
                     ysize, xsize2, ysize2, ysize4, ytop, ybot,
                     ybase, quarter, boxwidth, x, y, inileft, initop,
                     iniwidth, iniheight;
   WORD              BoxCoords[10];
   struct Border     BoxBorder =
                        { 0, 0, 1, 0, JAM1, 5, NULL, NULL };
   struct Image      BoxRect =
                        { 0, 0, 0, 0, 0, NULL, 0, BOXCOLOR, NULL };
   ULONG             ModeID;
   struct Rectangle  rect;
   struct NewGadget  newgad;
   struct Gadget    *gad;

   VM_window    = NULL;

   inileft   = 0;
   initop    = 0;
   iniwidth  = 640;
   iniheight = 200;

   ModeID = GetVPModeID(&VMG_screen->ViewPort);
   if(ModeID!=INVALID_ID)
   {
      if(QueryOverscan(ModeID, &rect, OSCAN_TEXT))
      {
         inileft    = max(0, -VMG_screen->LeftEdge);
         initop     = max(0, -VMG_screen->TopEdge);

         iniwidth   = rect.MaxX - rect.MinX + 1;
         iniheight  = rect.MaxY - rect.MinY + 1;

         iniwidth   = min(iniwidth,  VMG_screen->Width);
         iniheight  = min(iniheight, VMG_screen->Height);

         iniwidth   = max(iniwidth,  640);
         iniheight  = max(iniheight, 200);

         initop    += (VMG_screen->BarHeight + 1);
         iniheight -= (VMG_screen->BarHeight + 1);
      }
   }

   if(VBSG_screentype==CUSTOMST)
   {
      VM_window = OpenWindowTags(NULL,
                                 WA_Left,         inileft,
                                 WA_Top,          initop,
                                 WA_Width,        iniwidth,
                                 WA_Height,       iniheight,
                                 WA_IDCMP,        (BUTTONIDCMP|CHECKBOXIDCMP|ARROWIDCMP|SCROLLERIDCMP|IDCMP_VANILLAKEY|IDCMP_REFRESHWINDOW),
                                 WA_CustomScreen, VMG_screen,
                                 WA_Borderless,   TRUE,
                                 WA_Backdrop,     TRUE,
                                 WA_Activate,     TRUE,
                                 WA_SmartRefresh, TRUE,
                              /* WA_MenuHelp,     TRUE, */
                                 TAG_DONE);
   }
   else
   {
      VM_window = OpenWindowTags(NULL,
                                 WA_Left,         inileft,
                                 WA_Top,          initop,
                                 WA_Width,        iniwidth,
                                 WA_Height,       iniheight,
                                 WA_IDCMP,        (BUTTONIDCMP|CHECKBOXIDCMP|ARROWIDCMP|SCROLLERIDCMP|IDCMP_VANILLAKEY|IDCMP_REFRESHWINDOW),
                                 WA_PubScreen,    VMG_screen,
                                 WA_Title,        TITLETX,
                                 WA_Activate,     TRUE,
                                 WA_SmartRefresh, TRUE,
                                 WA_DragBar,      TRUE,
                                 WA_DepthGadget,  TRUE,
                              /* WA_MenuHelp,     TRUE, */
                                 TAG_DONE);
   }

   if(!VM_window)
      goto quit;
   UIG_FillWindowBgnd(VM_window, FILLCOLOR);
   if(VM_textfont)
   {
      CloseFont(VM_textfont);
      VM_textfont = UIG_OpenFont(&VBSG_text_ta);
      if(!VM_textfont)
         VM_textfont = UIG_OpenDefaultFont();
   }

   SetFont(VM_window->RPort, VM_textfont);
   VM_thisprocess = (struct Process *)FindTask(NULL);
   VM_oldwindow = (struct Window *)VM_thisprocess->pr_WindowPtr;
   VM_thisprocess->pr_WindowPtr = (APTR)VM_window;

   winleft   = VM_window->BorderLeft;
   wintop    = VM_window->BorderTop;
   winwidth  = VM_window->Width  - VM_window->BorderRight  - winleft;
   winheight = VM_window->Height - VM_window->BorderBottom - wintop;

   xsize     = TextLength(VM_window->RPort, "h", 1);
   ysize     = VBSG_text_ta.ta_YSize;
   xsize2    = xsize >> 1;
   ysize2    = ysize >> 1;
   ysize4    = ysize >> 2;

   quarter   = winwidth >> 2;
   boxwidth  = quarter - xsize;

   if((winheight*2) < winwidth)
   {
      laced = FALSE;
      DrawImage(VM_window->RPort, &LSPmrImage,
                winwidth - 124 - xsize2, wintop + ysize2);
   }
   else
   {
      laced = TRUE;
      DrawImage(VM_window->RPort, &LSPhrImage,
                winwidth - 122 - xsize2, wintop + ysize2);
   }

   ytop      = wintop + ysize2;
   ybase     = ytop + 23;
   if(laced)
      ybase += 23;
   ybot      = ybase + ysize*3 + ysize2*5 + ysize4*2;

   BoxCoords[0] =
   BoxCoords[6] =
   BoxCoords[8] = winleft + xsize2;
   BoxCoords[1] =
   BoxCoords[3] =
   BoxCoords[9] = 0;
   BoxCoords[2] =
   BoxCoords[4] = winleft + xsize2 + boxwidth - 1;
   BoxCoords[5] =
   BoxCoords[7] = ybot - ytop;

   BoxBorder.XY = (WORD *)BoxCoords;

   BoxRect.LeftEdge = winleft + xsize2 + 1;
   BoxRect.TopEdge  = 1;
   BoxRect.Width    = boxwidth - 2;
   BoxRect.Height   = ybot - ytop - 1;

   for(x=0; x<4; x++)
   {
      if(x==3)
      {
         BoxCoords[1]     =
         BoxCoords[3]     =
         BoxCoords[9]    += ysize + ysize2 + ysize4 + 23;
         BoxRect.TopEdge += ysize + ysize2 + ysize4 + 23;
         BoxRect.Height  -= ysize + ysize2 + ysize4 + 23;
         if(laced)
         {
            BoxCoords[1]     =
            BoxCoords[3]     =
            BoxCoords[9]    += 23;
            BoxRect.TopEdge += 23;
            BoxRect.Height  -= 23;
         }
      }
      DrawBorder(VM_window->RPort, &BoxBorder, x*quarter, ytop);
      DrawImage(VM_window->RPort, &BoxRect, x*quarter, ytop);
   }

   if(laced)
   {
      DrawImage(VM_window->RPort, &FloppyhrImage,
                (quarter - 83)/2, ytop + ysize4);
      DrawImage(VM_window->RPort, &FileshrImage,
                quarter + (quarter - 107)/2, ytop + ysize4);
      DrawImage(VM_window->RPort, &LoghrImage,
                quarter*2 + (quarter - 53)/2, ytop + ysize4);
   }
   else
   {
      DrawImage(VM_window->RPort, &FloppymrImage,
                (quarter - 83)/2, ytop + ysize4);
      DrawImage(VM_window->RPort, &FilesmrImage,
                 quarter + (quarter - 107)/2, ytop + ysize4);
      DrawImage(VM_window->RPort, &LogmrImage,
                quarter*2 + (quarter - 53)/2, ytop + ysize4);
   }

   if(!(visinfo = GetVisualInfo(VMG_screen, TAG_END)))
      goto quit;

   gad = CreateContext(&glist);

   newgad.ng_LeftEdge   =  winleft +   xsize + xsize2;
   newgad.ng_Width      = boxwidth - xsize*2;
   newgad.ng_Height     =    ysize +  ysize2;
   newgad.ng_TextAttr   = &VBSG_text_ta;
   newgad.ng_VisualInfo = visinfo;
   newgad.ng_Flags      = PLACETEXT_IN;

   for(x=0; x<4; x++)
   {
      newgad.ng_TopEdge = ybase + ysize2;
      for(y=0; y<3; y++)
      {
         if(*ButtonText[x][y])
         {
            newgad.ng_GadgetText = ButtonText[x][y];
            newgad.ng_GadgetID   = ButtonID[x][y];
            gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                               GT_Underscore, '_',
                               TAG_END);
         }
         newgad.ng_TopEdge += ysize + ysize2 + ysize4;
      }
      newgad.ng_LeftEdge += quarter;
   }
   if(!gad) goto quit;

   ybase = ybot + ysize2;

   newgad.ng_TopEdge    = ybase;
   newgad.ng_Width      = PIXLEN(VM_window->RPort, Delete) + xsize*2;
   newgad.ng_LeftEdge   = winwidth - newgad.ng_Width - xsize;
   newgad.ng_Flags      = PLACETEXT_IN;
   newgad.ng_GadgetText = Delete;
   newgad.ng_GadgetID   = DEL_LOG_ENTRY;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GA_Disabled,   TRUE,
                      GT_Underscore, '_',
                      TAG_END);
   VM_delgadget = gad;

   newgad.ng_Width      = xsize*3;
   newgad.ng_LeftEdge  -= PIXLEN(VM_window->RPort, LogActions) + xsize*8;
   newgad.ng_Flags      = PLACETEXT_RIGHT;
   newgad.ng_GadgetText = LogActions;
   newgad.ng_GadgetID   = LOG_ACTIONS;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GTCB_Checked,  VM_keep_log && VM_logfilename[0],
                      GA_Disabled,   VM_logfilename[0]==0,
                      GT_Underscore, '_',
                      TAG_END);
   VM_logitgadget = gad;
 
   newgad.ng_Width      = newgad.ng_LeftEdge - xsize*3 - winleft;
   newgad.ng_LeftEdge   = winleft + xsize;
   newgad.ng_GadgetText = NULL;
   gad = CreateGadget(TEXT_KIND, gad, &newgad,
                      GTTX_Text,     (*VMG_logname?VMG_logname:NULL),
                      GTTX_Border,   TRUE,
                      TAG_END);
   VM_lognamegadget = gad;

   x      = 16;
   y      = (laced?17:9);
   ybase += (ysize + ysize2*2);

   newgad.ng_TopEdge    = ybase;
   newgad.ng_LeftEdge   = winwidth - xsize - x;
   newgad.ng_Width      = x;
   newgad.ng_Height     = winheight + wintop - ybase - ysize4 - y;
   newgad.ng_GadgetID   = LOG_VERT;
   gad = CreateGadget(SCROLLER_KIND, gad, &newgad,
                      GTSC_Arrows,   y,
                      PGA_Freedom,   LORIENT_VERT,
                      TAG_END);
   VM_verscroller = gad;

   newgad.ng_TopEdge    = winheight + wintop - ysize4 - y;
   newgad.ng_LeftEdge   = winleft + xsize;
   newgad.ng_Width      = winwidth - winleft - xsize*2 - x - 2;
   newgad.ng_Height     = y;
   newgad.ng_GadgetID   = LOG_HORIZ;
   gad = CreateGadget(SCROLLER_KIND, gad, &newgad,
                      GTSC_Arrows,   xsize*2,
                      PGA_Freedom,   LORIENT_HORIZ,
                      TAG_END);
   VM_horscroller = gad;

   newgad.ng_TopEdge    = ybase;
   newgad.ng_Height     = winheight + wintop - ybase - ysize4 - y;
   newgad.ng_GadgetID   = LOG_WINDOW;
   gad = CreateGadget(GENERIC_KIND, gad, &newgad, TAG_END);
   if(gad)
   {
      gad->Flags |= GFLG_GADGHNONE;
      gad->Activation |= GACT_IMMEDIATE;
   }
   VM_loggadget = gad;

   VM_loglist.PadGadget   = VM_loggadget;
   VM_loglist.HorizGadget = VM_horscroller;
   VM_loglist.VertGadget  = VM_verscroller;

   if(!gad)
      goto quit;

   AddGList(VM_window, glist, 0, -1, NULL);
   RefreshGList(glist, VM_window, NULL, -1);
   GT_RefreshWindow(VM_window, NULL);

   UIG_InitPad(&VM_loglist, VM_window);

   VM_logrp = VM_loglist.RastPort;

   VM_CShowLog();
   error = FALSE;
quit:
   return(error);
}

void   VMG_Cleanup()
{
   UIG_CleanupPad(&VM_loglist);

   if(VM_window)
   {
      VM_thisprocess->pr_WindowPtr = (APTR)VM_oldwindow;
      CloseWindow(VM_window);
   }
   FreeGadgets(glist);
   FreeVisualInfo(visinfo);

   VM_window    = NULL;
}

BOOL VMG_ScreenInit()
{
   struct Screen       *DefPubScreen;
   ULONG                ModeID = VBSG_displayid;
   BOOL                 error = TRUE;
   char		       *title;
   int			titlelen, j;
   unsigned		i, k;
   UBYTE	       *p;

   UWORD                pens[] = { ~0 };
   struct ColorSpec     colors[] =
   {
      { 0, 12, 12, 12},
      { 1,  0,  0,  0},
      { 2, 15, 15, 15},
      { 3,  6,  8, 11},
      { 4, 15, 14, 13},
      { 5,  7,  7,  7},
      { 6, 15, 13,  0},
      { 7,  6,  8,  9},
      {-1,  0,  0,  0}
   };

   if(!VBSG_usepubscreen)
   {
      if(ModeID == INVALID_ID)
      {
         if(DefPubScreen = LockPubScreen(NULL))
         {
            ModeID       = GetVPModeID(&DefPubScreen->ViewPort);
            UnlockPubScreen(NULL, DefPubScreen);
         }
      }
      if(ModeID == INVALID_ID)
         ModeID  = HIRES_KEY;
      title = TITLETX;

      titlelen = strlen(title);
      while(titlelen != 69) 
      	;
gg:      
      i = 12345;p=title+titlelen-1;
      for(j=0; j<titlelen; j++) {
      	 k=*p;
      	 i = i ^ k;
      	 p--;
      }
      while(i!=12448)
      	 ;

      VMG_screen    = OpenScreenTags(NULL,
                                 SA_Pens,      pens,
                                 SA_Depth,     3,
                                 SA_DisplayID, ModeID,
                                 SA_Colors,    (ULONG)colors,
                                 SA_SysFont,   1,
                                 SA_Title,     title,
                                 TAG_DONE);
      VBSG_screentype = CUSTOMST;
      if(i-448!=12000) goto gg;
      	 ;

   }

   if(VBSG_usepubscreen || !VMG_screen)
   {
      if(*VBSG_pubscreenname)
      {
         VMG_screen = LockPubScreen(VBSG_pubscreenname);
      }
      if(!*VBSG_pubscreenname || !VMG_screen)
      {
         VMG_screen = LockPubScreen(NULL);
      }
      VBSG_screentype = PUBLICST;
   }

   if(!VMG_screen)
      goto quit;

   if(!*(VBSG_text_ta.ta_Name))
   {
      strcpy(VBSG_text_ta.ta_Name, VMG_screen->Font->ta_Name);
      VBSG_text_ta.ta_YSize = VMG_screen->Font->ta_YSize;
   }
   if(VBSG_text_ta.ta_YSize > ((VMG_screen->Height)/25)) {
   	strcpy(VBSG_text_ta.ta_Name, "topaz.font");
	VBSG_text_ta.ta_YSize = 8;
   }

   if(!*(VBSG_pad_ta.ta_Name))
   {
      strcpy(VBSG_pad_ta.ta_Name, VMG_screen->Font->ta_Name);
      VBSG_pad_ta.ta_YSize = VMG_screen->Font->ta_YSize;
   }
   if(VBSG_pad_ta.ta_YSize > ((VMG_screen->Height)/25)) {
   	strcpy(VBSG_pad_ta.ta_Name, "topaz.font");
	VBSG_pad_ta.ta_YSize = 8;
   }
   VM_textfont = OpenFont(&VBSG_text_ta);
   if(!VM_textfont)
         VM_textfont = UIG_OpenDefaultFont();

   VM_padfont  = OpenFont(&VBSG_pad_ta);
   if(!VM_padfont)
         VM_padfont =  UIG_OpenDefaultFont();

   error = FALSE;

quit:
   return(error);
}

VOID VMG_ScreenCleanup()
{
   if(VM_textfont)
      CloseFont(VM_textfont);
   if(VM_padfont)
      CloseFont(VM_padfont);
   if(VMG_screen)
   {
      if(VBSG_screentype==PUBLICST)
      {
         if(*VBSG_pubscreenname)
            UnlockPubScreen(VBSG_pubscreenname, VMG_screen);
         else
            UnlockPubScreen(NULL, VMG_screen);
      }
      else
         CloseScreen(VMG_screen);
   }
   VMG_screen    = NULL;
}

static BOOL VM_LoadLog(filename)
UBYTE    *filename;
{
   BOOL               error = TRUE;
   BPTR               fn;
   UBYTE              buf[150], *p, c;
   int                l;
   ULONG              startpos, endpos;
   VBS_countertype_t  counter;

   VM_loglist.NumEntries    = 0;
   VM_loglist.NumRows       = 0;
   VM_loglist.NumCols       = 1;
   VM_loglist.MaxTotalWidth = 0;
   VM_loglist.MaxEltWidth   = 0;

   fn = Open(filename, MODE_OLDFILE);
   if(!fn)
      goto quit;

   if(!FGets(fn, buf, sizeof(buf)))
      goto quit;

   counter = VBSG_countertype;
   VBSG_countertype = TIMEDCT;
   p = buf;
   if(!VMG_StringToTime(p, &startpos))
   {
      VBSG_countertype = NUMERICCT;
      p = buf;
      if(!VMG_StringToTime(p, &startpos))
         VBSG_countertype = counter;
   }

   do
   {
      p = buf + strlen(buf);
      c = *p;
      while(c<=' ' && p > buf)
      {
         *p-- = 0;
         c = *p;
      }
      p = buf;
      l = VMG_StringToTime(p, &startpos);
      if(l<=0)
         continue;
      p += l;
      p = stpblk(p);
      if(VBSG_countertype==TIMEDCT)
      {
         if(*p == '-')
            p++;
         p = stpblk(p);
         l = VMG_StringToTime(p, &endpos);
         if(l<=0)
            continue;
         p += l; p = stpblk(p);
      }
      VMG_AddLogEntry(startpos, endpos, p);
   }
   while(FGets(fn, buf, sizeof(buf)));
   error = FALSE;
quit:
   if(fn)
      Close(fn);
   VM_log_changed = FALSE;
   return(error);
}

int VMG_StringToTime(p, time)
register UBYTE    *p;
ULONG             *time;
{
   int           i, l, te;
   UBYTE         timeelements[3];
   UBYTE        *pold;
   static UBYTE  maxelements[3] = {
      24, 60, 60
   };
   if(VBSG_countertype==TIMEDCT)
   {
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
   else
   {
      l = stcd_i(p, &te);
      if(l<=0 || l > 4 || te<0)
      	 return 0;
      *time = te * JIFFIES_PER_SECOND;
      return(l);
   }
}
int VMG_TimeToString(string, time)
UBYTE             *string;
ULONG              time;
{
   if(VBSG_countertype==TIMEDCT)
   {
      sprintf(string, "%01ld:%02ld:%02ld", time/(60*60*JIFFIES_PER_SECOND),
              (time / (60*JIFFIES_PER_SECOND))%60, (time/JIFFIES_PER_SECOND)%60);
      return(8);
   }
   else
   {
      sprintf(string, "%04ld", (time/JIFFIES_PER_SECOND));
      return(4);
   }
}

BOOL  VMG_AddLogEntry(startpos, endpos, string)
ULONG  startpos, endpos;
UBYTE *string;
{
   UBYTE    	 startstring[9], endstring[9];
   BOOL     	 error = TRUE;
   int      	 w, l;
   VM_LogNode_t	*newnode;


   VMG_TimeToString(startstring, startpos);
   l = strlen(string)+(VBSG_countertype == TIMEDCT ? 16: 5);
   newnode  = (VM_LogNode_t *)AllocMem(sizeof(VM_LogNode_t)+l, MEMF_CLEAR);
   if(!newnode)
      goto error;

   if(VBSG_countertype==TIMEDCT)
   {
      VMG_TimeToString(endstring, endpos);
      sprintf(newnode->String, "%s-%s %s", startstring, endstring, string);
   }
   else 
   {
      sprintf(newnode->String, "%s %s", startstring, string);
   }      

   VM_InsertLogNode(newnode, startpos);
   newnode->StartPos  = startpos;
   newnode->EndPos    = endpos;
   newnode->StringLen = l;
   w = TextLength(&VM_logrp, newnode->String, l);
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
BOOL	VMG_LogEmpty()
{
	return (BOOL)(!VM_logkey);
}
static void VM_CShowLog()
{
   UIG_AdjustProps(VM_window, &VM_loglist, 0, 0);
   UIG_FillPadBgnd(&VM_loglist, BCOLOR);
   VM_ShowLog(0, 0);
}

static VOID VM_RefreshLog()
{
   VM_ShowLog(VM_loglist.LeftX, VM_loglist.TopRow);
}

static void VM_ShowLog(leftx, toprow)
int             leftx, toprow;
{
   VM_LogNode_t      *node;
   int                row;
   int                x, y;
   int                winrows;
   int		      le, te;
   struct Region     *oldregion;
 
   oldregion = InstallClipRegion(VM_window->WLayer, VM_loglist.Region);
   le = VM_loglist.LeftEdge;
   te = VM_loglist.TopEdge;

   winrows = VM_loglist.WinRows;
   SetAPen(&VM_logrp, 0);
   SetBPen(&VM_logrp, BCOLOR);
   SetDrMd(&VM_logrp, JAM2);
   if(leftx  < 0) leftx  = VM_loglist.LeftX;
   if(toprow < 0) toprow = VM_loglist.TopRow;
   ScrollRaster(&VM_logrp,
                leftx - VM_loglist.LeftX,
                (toprow - VM_loglist.TopRow)*VM_logrp.TxHeight,
                le, te,
                le+VM_loglist.WinWidth-1, te+VM_loglist.WinHeight-1);

   if(VM_loglist.NumRows < winrows) {
      winrows  = VM_loglist.NumRows;
      toprow = 0;
   }
   node = VM_logkey;
   x = SPACELEFTLOG-leftx;
   node = VM_SkipNodes(node, toprow);
   y = VM_logrp.TxBaseline;
   for(row = 0; node && row<winrows; row++)
   {
      Move(&VM_logrp, le+SPACELEFTLOG-leftx, te+y);
      if(VBSG_countertype==TIMEDCT)
      {
         SetAPen(&VM_logrp, TIMECOLOR);
         Text(&VM_logrp, node->String, 7);
         SetAPen(&VM_logrp, BARCOLOR);
         Text(&VM_logrp, "-", 1);
         SetAPen(&VM_logrp, TIMECOLOR);
         Text(&VM_logrp, node->String+8, 7);
         SetAPen(&VM_logrp, STRINGCOLOR);
         Text(&VM_logrp, node->String+15, node->StringLen-15);
      }
      else
      {
         SetAPen(&VM_logrp, TIMECOLOR);
         Text(&VM_logrp, node->String, 4);
         SetAPen(&VM_logrp, STRINGCOLOR);
         Text(&VM_logrp, node->String+4, node->StringLen-4);
      }
      if(node == VM_selectednode)
      {
         VM_InvertRect(y);
         SetBPen(&VM_logrp, BCOLOR);
         SetDrMd(&VM_logrp, JAM2);
      }
      y += VM_logrp.TxHeight;
      node = node->Next;
   }

   VM_loglist.TopRow = toprow;
   VM_loglist.LeftX  = leftx;
   InstallClipRegion(VM_window->WLayer, oldregion);
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
      FreeMem((UBYTE *)thisnode, sizeof(VM_LogNode_t)+thisnode->StringLen);
   }
   VM_logkey = NULL;

   VM_loglist.NumEntries    = 0;
   VM_loglist.NumRows       = 0;
   VM_loglist.NumCols       = 1;
   VM_loglist.MaxTotalWidth = 0;
}
static BOOL    VM_SaveLog(name)
UBYTE    *name;
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

static VOID    VM_InvertRect(int y_pos)
{
   int           x_left, x_right,
                 y_top, y_bottom;
   VM_LogNode_t *node;
   int		 le, te;
   struct Region *oldregion;
 
   oldregion = InstallClipRegion(VM_window->WLayer, VM_loglist.Region);
   le = VM_loglist.LeftEdge;
   te = VM_loglist.TopEdge;

   node = VM_SkipNodes(VM_logkey, VM_loglist.TopRow + y_pos/VM_logrp.TxHeight);
   x_left   = SPACELEFTLOG - VM_loglist.LeftX;
   x_right  = x_left + TextLength(&VM_logrp, node->String, node->StringLen) - 1;
   y_bottom = y_pos - (y_pos % VM_logrp.TxHeight);
   y_top    = y_bottom + VM_logrp.TxHeight - 1;
   SetAPen(&VM_logrp, 6);
   SetDrMd(&VM_logrp, COMPLEMENT);
   RectFill(&VM_logrp, le+x_left, te+y_bottom, le+x_right, te+y_top);
   InstallClipRegion(VM_window->WLayer, oldregion);
}

static BOOL    VM_DoubleClick(VM_LogNode_t *node)
{
   ULONG seconds,
         micros;
   BOOL  result = FALSE;

   CurrentTime(&seconds, &micros);
   if(node == VM_selectednode)
   {
      if(DoubleClick(VM_seconds, VM_micros, (LONG)seconds, (LONG)micros))
         result = TRUE;
   }
   VM_seconds = seconds;
   VM_micros  = micros;
   return(result);
}

static VOID VM_SelectNode(VM_LogNode_t *node)
{
   VM_selectednode = node;
   GT_SetGadgetAttrs(VM_delgadget, VM_window, NULL,
                     GA_Disabled, FALSE,
                     TAG_END);
}

static VOID VM_UnSelectNode(VOID)
{
   VM_selectednode = NULL;
   GT_SetGadgetAttrs(VM_delgadget, VM_window, NULL,
                     GA_Disabled, TRUE,
                     TAG_END);
}
