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

#include "vbs.h"
#include "LD_vbstext.h"
extern struct ExecBase  *SysBase;

#define FILLCOLOR    	4
#define EXCLUDECOLOR 	0
#define INCLUDECOLOR 	1
#define DIRTREECOLOR 	5
#define BCOLOR       	2
#define CURDIRCOLOR  	7
#define BOXCOLOR     	7
#define BOXBORDERCOLOR  1
#define BOXTEXTCOLOR1	1
#define BOXTEXTCOLOR2	2

#define CBOXTOP      3
#define CBOXHEIGHT   26
#define MAXCOL       40
#define MAXDIRROWS   100

#define SPACEBETWEEN 16
#define SPACELEFTFILE 4
#define SPACELEFTDIR  8

#define INSPRING     16

#define COLUMNS      1
#define LONGLIST     0

enum {   BACKUP, RESTORE, VERIFY };

#include "hdb_ui.h"
/*#include "VD_hdb_ui.c"*/

enum { NOTHING, RETURN_QUIT, RETURN_START, RETURN_CANCEL};

enum { ADDGAD, MODIFYGAD, DELETEGAD, STARTGAD, CANCELGAD, TIMEGAD,
       LISTVIEWGAD, DIRGAD, DIRHOR, DIRVER, FILEGAD, FILEHOR, FILEVER,
       INCLUDEALLGAD, EXCLUDEALLGAD, TOGGLEALLGAD, INCLUDEARCHGAD,
       EXCLUDEARCHGAD, LONGFILELISTGAD, DONEGAD, NAMEGAD, 
       SELCANCELGAD, BACKUPTYPEGAD};

struct HDB_BackupTree
{
   struct Node    bt_Node;
   VBS_DirNode_t *bt_DirTree;
   UBYTE	  bt_BackupType;
   UBYTE          bt_BackupName[79];
   UBYTE          bt_Path[256];
   UBYTE          bt_ListViewName[256];
};

typedef struct HDB_BackupTree HDB_BackupTree_t;

static VBS_error_t       HDB_AskTree(UBYTE *, VBS_DirNode_t **, BOOL, BOOL, BOOL);
static void              HDB_ChangeCurDir(VBS_DirEntry_t *);
VOID                     HDBG_Cleanup(VOID);
static void              HDB_ColumnizeFiles(VBS_DirNode_t *);
static void              HDB_CShowFiles(VBS_DirNode_t *);
static int               HDB_HandleGadget(USHORT, ULONG, UWORD, int, int);
static int               HDB_HandleVanillaKey(UWORD, BOOL);
static BOOL              HDB_IncludeArch(VBS_DirNode_t *, BOOL);
static void              HDB_IncludeDirNode(VBS_DirNode_t *, BOOL);
static void              HDB_IncludeFile(VBS_FileNode_t *, BOOL);
static BOOL              HDB_IncludeFiles(VBS_DirNode_t *, BOOL);
static void              HDB_IncludeParents(VBS_DirNode_t *);
static void              HDB_IncludeTree(VBS_DirNode_t *, BOOL);
BOOL                     HDBG_Init(VOID);
static void              HDB_LongListFiles(VBS_DirNode_t *);
static UWORD             HDB_NodeNum(struct MinList *, HDB_BackupTree_t *);
static HDB_BackupTree_t *HDB_RemoveNode(HDB_BackupTree_t *);
static VBS_error_t       HDB_ReportDirectory(FILE *, VBS_DirNode_t *);
static void              HDB_SetFlags(VBS_DirNode_t *, UBYTE, UBYTE);
static VBS_error_t       HDB_SelectFiles(VOID);
static void		 HDB_SelectInit(VOID);
static void              HDB_ShowDirectories(VBS_DirNode_t *, int, int, int, int);
static void              HDB_ShowFile(VBS_FileNode_t *, int, int);
static void              HDB_ShowFiles(VBS_DirNode_t *, int, int);
static void              HDB_ShowStats(void);
static void              HDB_ShowTree(VBS_DirNode_t *, int, int);
static VBS_FileNode_t   *HDB_SkipFiles(VBS_FileNode_t *, int);
static HDB_BackupTree_t *HDB_SkipNodes(struct MinList *, UWORD);
static BOOL              HDB_ToggleFiles(VBS_DirNode_t *);
static UBYTE            *HDB_TreeErrorMsg(UBYTE);
static VBS_error_t       HDB_WriteReport(int, UBYTE *, VBS_DirNode_t *, UBYTE *,\
                             UBYTE *, UBYTE *, BOOL, ULONG, ULONG,\
                             long, long, long);

static   long              HDB_numdirs, HDB_totalbytes, HDB_numfiles;

static   struct Window    *HDB_window = NULL;
static   struct Window    *HDB_treewindow = NULL;
static   struct RastPort  *HDB_dirrp, *HDB_filerp, *HDB_winrp;

static struct Process     *HDB_thisprocess;
static struct Window      *HDB_oldwindow;

static struct Gadget      *HDB_glist = NULL;
static void               *HDB_visinfo;

static   int               HDB_fileshowmode;
static   int               HDB_diry, HDB_maxy, HDB_Wwidth;
static   UWORD             HDB_colwidth[MAXCOL];
static   VBS_FileNode_t   *HDB_filecolumns[MAXCOL];
static   VBS_DirEntry_t   *HDB_direntries=0/*[MAXDIRROWS]*/, *HDB_de, *HDB_curde;
static   VBS_DirNode_t    *HDB_tree, *HDB_curdir;
static   UBYTE             HDB_starttime[80] = STARTTIMEFORMATTX;
static   UBYTE             HDB_BackupName[80];
static   struct MinList    HDB_BackupsList;
static   int               HDB_NumBackups = 0;
static   HDB_BackupTree_t *HDB_CurrentNode;

static   VBS_ListData_t    HDB_dirlist;
static   VBS_ListData_t    HDB_filelist;
static   int		   HDB_wincurx, HDB_wincury;
static   UBYTE		   HDB_backuptype;
static struct Gadget *HDB_timegad, *HDB_listviewgad, *HDB_longlistgad,
                     *HDB_dirgad,  *HDB_dirhor,    *HDB_dirver,
                     *HDB_filegad, *HDB_filehor,   *HDB_filever,
                     *HDB_namegad, *HDB_modifygad, *HDB_deletegad,
                     *HDB_startgad;

extern BOOL	      VBSG_is68000;
extern BOOL           VBSG_novideocheck;
extern struct Screen *VMG_screen;
extern UBYTE          VMG_logname[], VBSG_reportdrawer[];

extern struct TextAttr   VBSG_text_ta,
                         VBSG_pad_ta;

static struct TextFont  *HDB_textfont,
                        *HDB_padfont;

extern BOOL WTG_TimerRequest(LONG, UBYTE *);
extern BOOL WTG_StringToTime(UBYTE *, LONG *);

extern VBS_screentype_t  VBSG_screentype;
extern VBS_countertype_t VBSG_countertype;
extern UBYTE             VBSG_errorinfo[];
extern BOOL		 VBSG_beepafter;

VBS_error_t   HDBG_FSBackup(keep_log)
BOOL             keep_log;
{
   LONG                 startpos, starttime, endtime, duration;
   struct DateStamp     date;
   UBYTE                datestring[20], string[80];
   UBYTE               *timestring;
   BOOL                 archive, timergotten = FALSE;
   BOOL                 timed = FALSE, scrsetup = FALSE;
   VBS_error_t          error = EOK;
   UBYTE                report[80], *afterreport;
   struct Gadget       *gad;
   struct IntuiMessage *imsg;
   ULONG                class;
   UWORD                code, gid;
   int                  to_do;
   HDB_BackupTree_t    *node, *next;
   LONG                 time;
   int			i, backupnr;
   
   NewList((struct List *)&HDB_BackupsList);

   HDB_textfont = UIG_OpenFont(&VBSG_text_ta);
   if(!HDB_textfont)
      HDB_textfont = UIG_OpenDefaultFont();
   
   if(HDBG_Init())
   {
      HDBG_Cleanup();
      error = EMEMORY;
      goto quit;
   }
lus:
   while(!(imsg = GT_GetIMsg(HDB_window->UserPort)))
      WaitPort(HDB_window->UserPort);

   code = imsg->Code; class = imsg->Class;
   gad = NULL;
   if(class==IDCMP_GADGETDOWN || class==IDCMP_GADGETUP)
   {
      gad = (struct Gadget *)(imsg->IAddress);
      gid = gad->GadgetID;
   }

   GT_ReplyIMsg(imsg);
   switch(class)
   {
      case IDCMP_GADGETUP:
      case IDCMP_GADGETDOWN:
         to_do = HDB_HandleGadget(gid, class, code, 0, 0);
         break;
      case IDCMP_VANILLAKEY:
         to_do = HDB_HandleVanillaKey(code, FALSE);
         break;
   }
   switch(to_do)
   {
      case RETURN_START:
         break;
      case RETURN_QUIT:
         goto quit;
      default:
         goto lus;
   }

   if(!NTG_AskNameTime(HDB_window, keep_log ? &startpos : NULL,
                               &archive, report, BACKUPOT))
      goto lus;

   timestring = ((struct StringInfo *)HDB_timegad->SpecialInfo)->Buffer;
   if(*timestring && strcmp(timestring, STARTTIMEFORMATTX))
   {
      if(!WTG_StringToTime(timestring, &time))
      {
         strcpy(VBSG_errorinfo, STARTTIMEFORMATTX);
         ERRG_Message(HDB_window, EENTERTIME);
         goto lus;
      }
      else
      {
         if(!WTG_TimerRequest(time, timestring))
            goto lus;
         else
            timed = TRUE;
      }
   }
   
/*   if(!(buffer = AllocMem(MEMSIZE, 0))) {
      error = EMEMORY;
      goto quit;
   }*/

   if(!(BAG_CheckConnection(VMG_screen, HDB_window)))
      goto lus;
   BAG_SetupVideoScreen(NULL);
   scrsetup = TRUE;
   if(!timed)
      VBS_WaitFire();
   VBS_GetTimer();timergotten = TRUE;
   DateStamp(&date);

   backupnr = 0; afterreport = report+strlen(report);
   node = (HDB_BackupTree_t *)HDB_BackupsList.mlh_Head;  

   next = (HDB_BackupTree_t *)node->bt_Node.ln_Succ;
   if(!next)
   	goto quit;
nextbackup:
   HDB_tree = node->bt_DirTree;

   starttime = VBS_ReadTimer();

   error = BAG_WriteHeader(node->bt_BackupType, node->bt_BackupName, node->bt_Path, &date);
   if(error) goto breek;
   if(!(error=BAG_Backup(node->bt_BackupType, HDB_tree, node->bt_BackupName, node->bt_Path, 
   	archive)))
   	/*, buffer, MEMSIZE, NULL, 0, NULL)))*/
   {
      endtime  = VBS_ReadTimer();
      duration = (endtime - starttime) & 0x00FFFFFF;
      UIG_Date(datestring, date.ds_Days);
      sprintf(string, "FS  %s "ONTX" %s", node->bt_BackupName, datestring);
      if(keep_log)
         VMG_AddLogEntry(startpos, startpos+duration, string);
      if(report[0])
      {
         if(backupnr) {
         	sprintf(afterreport, ".%d", backupnr);
         }
         error = HDB_WriteReport(BACKUP, report, HDB_tree, node->bt_Path, datestring,
                         node->bt_BackupName,
                         keep_log, startpos, startpos+duration,
                         HDB_totalbytes, HDB_numfiles, HDB_numdirs);
      }
   } else {
breek:   
	if(scrsetup)
      		BAG_RestoreVideoScreen();
      	(void)ERRG_Message(HDB_window, error);
      	error = EOK;
   	goto lus;
   }
   node = next;
   
   next = (HDB_BackupTree_t *)next->bt_Node.ln_Succ;

   if(next) {
      endtime  = VBS_ReadTimer();
      duration = (endtime - starttime) & 0x00FFFFFF;
      startpos += duration;     
      backupnr++;
      goto nextbackup;
   }
   if(VBSG_beepafter) {
   	i = 0;
   	for(;;) {
   		UIG_Beep(1000);
   		i = UIG_DelayLMB(50/2);
   		UIG_Beep(0);
   		if(!i || !UIG_DelayLMB(50*2))
   			break;
   	}	
   }   
quit:
   node = (struct HDB_BackupTree *)HDB_BackupsList.mlh_Head;
   while(node && node->bt_Node.ln_Succ)
      node = HDB_RemoveNode(node);
   if(HDB_window)
      HDBG_Cleanup();
   if(timergotten)
      VBS_FreeTimer();
   if(scrsetup)
      BAG_RestoreVideoScreen();
/*   if(buffer)
      FreeMem(buffer, MEMSIZE);*/
   CloseFont(HDB_textfont);      
   return(error);
}

VBS_error_t  HDBG_FSRestore(win, name)
struct Window   *win;
UBYTE           *name;
{
   BOOL         scrsetup = FALSE;
   UBYTE       *path, *fullname, *filename;
   UBYTE        presetpath[80], *bnp;
   VBS_error_t  error = EOK;
   char		type;
   UBYTE	oldnamelen;
   
   HDB_textfont = UIG_OpenFont(&VBSG_text_ta);
   if(!HDB_textfont)
      HDB_textfont = UIG_OpenDefaultFont();
   
   HDB_window = win;
   HDB_tree = NULL;
   
   BAG_SetupScreen(VMG_screen);
   scrsetup = TRUE;

   error = BAG_Search(BT_FS, name, HDB_BackupName, &type);
   if(error)
      goto quit;
   	
   error = BAG_RestoreTree(type, &HDB_tree);
   if(error)
      goto quit;
#if 0
	{ ULONG totalbytes, numfiles, numdirs;
	  VBS_ListData_t dirlist;
      BAG_PrepareTree(HDB_tree, win->RPort,
          &totalbytes, &numfiles, &numdirs,
          &dirlist, SPACELEFTDIR, INSPRING);
          printf("Hallo readdir %ul %ul %ul %ul\n",
          	totalbytes, numfiles, numdirs, dirlist);
        }
#endif
   path 	= HDB_tree->Name;
   oldnamelen 	= HDB_tree->NameLen;
   strcpy(presetpath, path);
   if(*path) {
   	HDB_tree->NameLen = 1;
   	path[0] = '/'; path[1] = 0;
   }
   
   BAG_RestoreScreen();
   scrsetup = FALSE;
   
   if(ERRG_Message(HDB_window, ETREEREAD)==ERROR_CANCEL)
      goto quit;

   if(HDB_AskTree(NULL, &HDB_tree, FALSE, FALSE, TRUE))
   	goto quit;
   path = FileRequest(HDB_window, ENTERPATHRESTTX, presetpath, NULL, NULL, DIRRT);
   if(!path)
      goto quit;
   strcpy(presetpath, path);

   BAG_SetupScreen(VMG_screen); scrsetup = TRUE;

   error    = BAG_RestoreFilesTo(type, HDB_tree, path, HDB_BackupName);
   if(error)
      goto quit;
   bnp      = HDB_BackupName+strlen(HDB_BackupName);
   strcpy(bnp, ".RReport");
   BAG_RestoreScreen(); scrsetup = FALSE;
   fullname = FileRequest(HDB_window, RESTREPORTFNAMETX,
                          VBSG_reportdrawer, &filename, HDB_BackupName, SAVERT);
   *bnp     = '\0';
   if(fullname && fullname[0])
      HDB_WriteReport(RESTORE, fullname, HDB_tree, presetpath, NULL,
                      HDB_BackupName, FALSE, 0, 0,
                      HDB_totalbytes, HDB_numfiles, HDB_numdirs);

quit:
   if(scrsetup)
      BAG_RestoreScreen();
   if(HDB_tree) {
   	HDB_tree->NameLen = oldnamelen;
      	BAG_FreeTree(HDB_tree);
   }
   CloseFont(HDB_textfont);      
   return(error);
}


VBS_error_t  HDBG_FSVerify(win, name)
struct Window   *win;
UBYTE           *name;
{
   BOOL         scrsetup = FALSE;
   UBYTE       *path, *fullname, *filename;
   UBYTE        presetpath[80], *bnp;
   VBS_error_t  error = EOK;
   char		type;
   UBYTE	oldnamelen;
   
   HDB_textfont = UIG_OpenFont(&VBSG_text_ta);
   if(!HDB_textfont)
      HDB_textfont = UIG_OpenDefaultFont();
   
   HDB_window = win;
   HDB_tree = NULL;
   
   BAG_SetupScreen(VMG_screen);
   scrsetup = TRUE;

   error = BAG_Search(BT_FS, name, HDB_BackupName, &type);
   if(error)
      goto quit;
   	
   error = BAG_RestoreTree(type, &HDB_tree);
   if(error)
      goto quit;
      
   path 	= HDB_tree->Name;
   oldnamelen 	= HDB_tree->NameLen;
   strcpy(presetpath, path);
   if(*path) {
   	HDB_tree->NameLen = 1;
   	path[0] = '/'; path[1] = 0;
   }
   
   BAG_RestoreScreen();
   scrsetup = FALSE;
   
   if(ERRG_Message(HDB_window, ETREEREADV)==ERROR_CANCEL)
      goto quit;

   if(HDB_AskTree(NULL, &HDB_tree, FALSE, FALSE, TRUE))
   	goto quit;
   path = FileRequest(HDB_window, ENTERPATHVERTX, presetpath, NULL, NULL, DIRRT);
   if(!path)
      goto quit;
   strcpy(presetpath, path);

   BAG_SetupScreen(VMG_screen); scrsetup = TRUE;

   error    = BAG_VerifyFilesFrom(type, HDB_tree, path, HDB_BackupName);
   if(error)
      goto quit;
   bnp      = HDB_BackupName+strlen(HDB_BackupName);
   strcpy(bnp, ".VReport");
   BAG_RestoreScreen(); scrsetup = FALSE;
   fullname = FileRequest(HDB_window, VERREPORTFNAMETX,
                          VBSG_reportdrawer, &filename, HDB_BackupName, SAVERT);
   *bnp     = '\0';
   if(fullname && fullname[0])
      HDB_WriteReport(VERIFY, fullname, HDB_tree, presetpath, NULL,
                      HDB_BackupName, FALSE, 0, 0,
                      HDB_totalbytes, HDB_numfiles, HDB_numdirs);

quit:
   if(scrsetup)
      BAG_RestoreScreen();
   if(HDB_tree) {
   	HDB_tree->NameLen = oldnamelen;
      	BAG_FreeTree(HDB_tree);
   }
   CloseFont(HDB_textfont);      
   return(error);
}







static void HDB_SelectInit()
{
   HDB_curdir = HDB_tree;

   HDB_ShowTree(HDB_tree, 0, 0);
   UIG_AdjustProps(HDB_treewindow, &HDB_dirlist, 0, 0);
   HDB_CShowFiles(HDB_tree);
   HDB_ShowStats();
}

static VBS_error_t HDB_SelectFiles()
{

   struct IntuiMessage *message;
   struct Gadget       *g;
   USHORT               code, gid;
   ULONG                class;
   int                  mouse_x, mouse_y;
   int                  to_do;

lus:
   while(!(message = GT_GetIMsg(HDB_treewindow->UserPort)))
      WaitPort(HDB_treewindow->UserPort);

   code = message->Code;class = message->Class;
   g = NULL;
   if(class==IDCMP_GADGETDOWN || class==IDCMP_GADGETUP || class==IDCMP_MOUSEMOVE)
   {
      g = (struct Gadget *)(message->IAddress);
      gid = g->GadgetID;
   }
   mouse_x = message->MouseX; mouse_y = message->MouseY;

   GT_ReplyIMsg(message);

   to_do = NOTHING;
   switch(class)
   {
      case IDCMP_GADGETUP:
      case IDCMP_GADGETDOWN:
      case IDCMP_MOUSEMOVE:
         to_do = HDB_HandleGadget(gid, class, code, mouse_x, mouse_y);
         break;
      case IDCMP_VANILLAKEY:
         to_do = HDB_HandleVanillaKey(code, TRUE);
         break;
      case IDCMP_REFRESHWINDOW:
         GT_BeginRefresh(HDB_treewindow);
         HDB_ShowFiles(HDB_curdir, -1, -1);
         HDB_ShowTree(HDB_tree, -1, -1);
         GT_EndRefresh(HDB_treewindow, TRUE);
         break;
   }
   if(to_do==RETURN_QUIT)
      return(EOK);
   if(to_do==RETURN_CANCEL)
      return(EBREAK);
   goto lus;
}

static int HDB_HandleVanillaKey(UWORD code, BOOL select)
{
   int   result;
   UWORD gid;

   switch(code)
   {
      case 's':
      case 'S':
         if(!select && HDB_NumBackups>0) gid = STARTGAD;
         break;
      case 'b':
         if(!select)
         {
            if(HDB_NumBackups<2)
               break;
            if(((struct Node *)HDB_CurrentNode)->ln_Succ->ln_Succ!=NULL)
            {
               HDB_CurrentNode = (HDB_BackupTree_t *)HDB_CurrentNode->bt_Node.ln_Succ;
               GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
                                 GTLV_Selected, HDB_NodeNum(&HDB_BackupsList, HDB_CurrentNode),
                                 TAG_END);
               return(NOTHING);
            }
         }
         break;
      case 'B':
         if(!select)
         {
            if(HDB_NumBackups<2)
               break;
            if(((struct Node *)HDB_CurrentNode)->ln_Pred->ln_Pred!=NULL)
            {
               HDB_CurrentNode = (struct HDB_BackupTree *)HDB_CurrentNode->bt_Node.ln_Pred;
               GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
                                 GTLV_Selected, HDB_NodeNum(&HDB_BackupsList, HDB_CurrentNode),
                                 TAG_END);
               return(NOTHING);
            }
         }
         break;
#ifdef DEUTSCH
      case 'a':
      case 'A':
         gid = (select?EXCLUDEALLGAD:CANCELGAD);
         break;
      case 'h':
      case 'H':
         if(!select) gid = ADDGAD;
         break;
      case 'ä':
      case 'Ä':
         if(!select && HDB_NumBackups>0) gid = MODIFYGAD;
         break;
      case 'l':
      case 'L':
         if(select)
         {
            GT_SetGadgetAttrs(HDB_longlistgad, HDB_treewindow, NULL,
                              GTCB_Checked, !(HDB_longlistgad->Flags & GFLG_SELECTED),
                              TAG_END);
            gid = LONGFILELISTGAD;
         }
         else
            if(HDB_NumBackups>0) gid = DELETEGAD;
         break;
      case 'z':
      case 'Z':
         if(!select)
         {
            ActivateGadget(HDB_timegad, HDB_window, NULL);
            return(NOTHING);
         }
         break;
      case 'f':
      case 'F':
         if(select) gid = DONEGAD;
         break;
      case 'e':
      case 'E':
         if(select) gid = INCLUDEALLGAD;
         break;
      case 'i':
      case 'I':
         if(select) gid = INCLUDEARCHGAD;
         break;
      case 'u':
      case 'U':
         if(select) gid = EXCLUDEARCHGAD;
         break;
      case 'n':
      case 'N':
         if(select && HDB_namegad)
         {
            ActivateGadget(HDB_namegad, HDB_treewindow, NULL);
            return(NOTHING);
         }
         break;
#else
      case 'z':
      case 'Z':
         if(select) gid = INCLUDEARCHGAD;
         break;
      case 'c':
      case 'C':
         gid = (select?SELCANCELGAD:CANCELGAD);
         break;
      case 'a':
      case 'A':
         if(!select) gid = ADDGAD;
         break;
      case 'm':
      case 'M':
         if(!select) gid = MODIFYGAD;
         break;
      case 'd':
      case 'D':
         gid = (select?DONEGAD:DELETEGAD);
         break;
      case 't':
      case 'T':
         if(!select)
         {
            ActivateGadget(HDB_timegad, HDB_window, NULL);
            return(NOTHING);
         }
         else
            gid = TOGGLEALLGAD;
         break;
      case 'i':
      case 'I':
         if(select) gid = INCLUDEALLGAD;
         break;
      case 'e':
      case 'E':
         if(select) gid = EXCLUDEALLGAD;
         break;
      case 'n':
      case 'N':
         if(select && HDB_namegad)
         {
            ActivateGadget(HDB_namegad, HDB_treewindow, NULL);
            return(NOTHING);
         }
         break;
      case 'x':
      case 'X':
         if(select) gid = EXCLUDEARCHGAD;
         break;
      case 'l':
      case 'L':
         if(select)
         {
            GT_SetGadgetAttrs(HDB_longlistgad, HDB_treewindow, NULL,
                              GTCB_Checked, !(HDB_longlistgad->Flags & GFLG_SELECTED),
                              TAG_END);
            gid = LONGFILELISTGAD;
         }
         break;
#endif
      default:
         return(NOTHING);
   }
   result = HDB_HandleGadget(gid, IDCMP_GADGETUP, 0, 0, 0);
   return(result);
}

static int HDB_HandleGadget(gid, class, code, mouse_x, mouse_y)
USHORT     gid;
ULONG      class;
UWORD      code;
int        mouse_x, mouse_y;
{
   BOOL              error = TRUE;

   int               x, y, row, col, leftx, toprow;
   BOOL              f, showtree = FALSE;
   VBS_FileNode_t   *fnode;
   VBS_DirEntry_t   *de;
   int               xc;
   UBYTE            *pathptr, string[256];
   HDB_BackupTree_t *tree;
   UWORD             numnode;
   struct Region    *oldregion;
   static UBYTE	     backuptype[] = {
   	BT_FS, BT_FSFAST, BT_FSFASTCOMPR
   };

   switch(class)
   {
      case GADGETUP:
         switch(gid)
         {
            case BACKUPTYPEGAD:
            	HDB_backuptype = backuptype[code];
            	break;	
            case ADDGAD:
               pathptr = FileRequest(HDB_window, ENTERPATHBACKTX, NULL, NULL, NULL, DIRRT);
               if(pathptr)
               {
                  tree = (HDB_BackupTree_t *)AllocVec(sizeof(HDB_BackupTree_t), MEMF_CLEAR);
                  if(!tree)
                  {
                     ERRG_Message(HDB_window, EMEMORY);
                     return(NOTHING);
                  }

/*                  HDB_tree = NULL;*/
		  HDB_BackupName[0] = 0;
		  HDB_backuptype = VBSG_is68000 ? BT_FS : BT_FSFAST;
                  error = HDB_AskTree(pathptr, &tree->bt_DirTree, TRUE, TRUE, TRUE);
                  if(error)
                  {
                     if(tree->bt_DirTree) 
                     	BAG_FreeTree(tree->bt_DirTree);
                     FreeVec(tree);
                     goto quit;
                  }

/*                  tree->bt_DirTree = HDB_tree;
                  HDB_tree = NULL;*/
                  tree->bt_BackupType = HDB_backuptype;
                  strcpy(tree->bt_BackupName, HDB_BackupName);
                  strcpy(tree->bt_Path, pathptr);
                  sprintf(string, "%s from %s", HDB_BackupName, pathptr);
                  strcpy(tree->bt_ListViewName, string);
                  tree->bt_Node.ln_Name = tree->bt_ListViewName;

                  GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
                                    GTLV_Labels, ~0,
                                    TAG_END);
                  HDB_CurrentNode = tree;
                  HDB_NumBackups++;
                  AddTail((struct List *)&HDB_BackupsList, (struct Node *)HDB_CurrentNode);
                  GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
                                    GTLV_Selected, HDB_NumBackups,
                                    GTLV_Labels, &HDB_BackupsList,
                                    TAG_END);

                  if(HDB_NumBackups==1)
                  {
                     GT_SetGadgetAttrs(HDB_modifygad, HDB_window, NULL,
                                       GA_Disabled, FALSE,
                                       TAG_END);
                     GT_SetGadgetAttrs(HDB_deletegad, HDB_window, NULL,
                                       GA_Disabled, FALSE,
                                       TAG_END);
                     GT_SetGadgetAttrs(HDB_timegad, HDB_window, NULL,
                                       GA_Disabled, FALSE,
                                       TAG_END);
                     GT_SetGadgetAttrs(HDB_startgad, HDB_window, NULL,
                                       GA_Disabled, FALSE,
                                       TAG_END);
                  }
               }
               break;
            case MODIFYGAD:
               strcpy(HDB_BackupName, HDB_CurrentNode->bt_BackupName);
               HDB_backuptype = HDB_CurrentNode->bt_BackupType;
               HDB_AskTree(HDB_CurrentNode->bt_Path,
                           &HDB_CurrentNode->bt_DirTree, FALSE, TRUE, FALSE);
               GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
                                 GTLV_Labels, ~0,
                                 TAG_END);

               sprintf(string, "%s from %s", HDB_BackupName, HDB_CurrentNode->bt_Path);
               strcpy(HDB_CurrentNode->bt_ListViewName, string);
               strcpy(HDB_CurrentNode->bt_BackupName, HDB_BackupName);
               HDB_CurrentNode->bt_BackupType = HDB_backuptype;
               
               HDB_CurrentNode->bt_Node.ln_Name = HDB_CurrentNode->bt_ListViewName;

               GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
/*                                 GTLV_Selected, HDB_NumBackups,*/
                                 GTLV_Labels, &HDB_BackupsList,
                                 TAG_END);

               break;
            case DELETEGAD:
               if(HDB_NumBackups>0)
               {
                  GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
                                    GTLV_Labels, ~0,
                                    TAG_END);

                  HDB_CurrentNode = HDB_RemoveNode(HDB_CurrentNode);
                  numnode = HDB_NodeNum(&HDB_BackupsList, HDB_CurrentNode);
                  
                  GT_SetGadgetAttrs(HDB_listviewgad, HDB_window, NULL,
                                    GTLV_Labels,   &HDB_BackupsList,
                                    GTLV_Selected, numnode,
                                    TAG_END);

                  if(HDB_NumBackups==0)
                  {
                     GT_SetGadgetAttrs(HDB_modifygad, HDB_window, NULL,
                                       GA_Disabled, TRUE,
                                       TAG_END);
                     GT_SetGadgetAttrs(HDB_deletegad, HDB_window, NULL,
                                       GA_Disabled, TRUE,
                                       TAG_END);
                     GT_SetGadgetAttrs(HDB_timegad, HDB_window, NULL,
                                       GA_Disabled, TRUE,
                                       TAG_END);
                     GT_SetGadgetAttrs(HDB_startgad, HDB_window, NULL,
                                       GA_Disabled, TRUE,
                                       TAG_END);
                  }
               }
               break;
            case LISTVIEWGAD:
               HDB_CurrentNode = HDB_SkipNodes(&HDB_BackupsList, code);
               break;
            case TIMEGAD:
               strcpy(HDB_starttime, ((struct StringInfo *)HDB_timegad->SpecialInfo)->Buffer);
               break;
            case STARTGAD:
               return(RETURN_START);
               break;
            case CANCELGAD:
            case DONEGAD:
               return(RETURN_QUIT);
            case SELCANCELGAD:
               return(RETURN_CANCEL);
            case INCLUDEALLGAD:
               showtree = HDB_IncludeFiles(HDB_curdir, TRUE);
               break;
            case EXCLUDEALLGAD:
               showtree = HDB_IncludeFiles(HDB_curdir, FALSE);
               break;
            case TOGGLEALLGAD:
               showtree = HDB_ToggleFiles(HDB_curdir);
               break;
            case EXCLUDEARCHGAD:
               showtree = HDB_IncludeArch(HDB_tree, FALSE);
               break;
            case INCLUDEARCHGAD:
               showtree = HDB_IncludeArch(HDB_tree, TRUE);
               break;
            case LONGFILELISTGAD:
               HDB_fileshowmode = (HDB_longlistgad->Flags & GFLG_SELECTED) ?
                  LONGLIST : COLUMNS;
               HDB_CShowFiles(HDB_curdir);
               break;
         }
      case IDCMP_GADGETDOWN:
      case IDCMP_MOUSEMOVE:
         switch(gid)
         {
            case DIRHOR:
               leftx  = (int)code;
               toprow = HDB_dirlist.TopRow;
               HDB_ShowTree(HDB_tree, leftx, toprow);
               break;
            case DIRVER:
               leftx  = HDB_dirlist.LeftX;
               toprow = (int)code;
               HDB_ShowTree(HDB_tree, leftx, toprow);
               break;
            case FILEHOR:
               leftx  = (int)code;
               toprow = HDB_filelist.TopRow;
               HDB_ShowFiles(HDB_curdir, leftx, toprow);
               break;
            case FILEVER:
               leftx  = HDB_filelist.LeftX;
               toprow = (int)code;
               HDB_ShowFiles(HDB_curdir, leftx, toprow);
               break;
            case DIRGAD:
               x = mouse_x - HDB_dirgad->LeftEdge;
               y = mouse_y - HDB_dirgad->TopEdge;
               if(y<HDB_dirlist.WinHeight)
               {
                  row = y/HDB_dirrp->TxHeight;
                  if(row + HDB_dirlist.TopRow < HDB_dirlist.NumRows &&
                     x >= HDB_direntries[row].x)
                  {
                     de = &HDB_direntries[row];
                     if(de != HDB_curde)
                     {
                        HDB_ChangeCurDir(&HDB_direntries[row]);
                        HDB_CShowFiles(HDB_curdir);
                     }
                     else
                     {
                        f = (HDB_curdir->Flags&ERRF_INCLUDE) == 0;
                        HDB_IncludeTree(HDB_curdir, f);
                        if(f)
                           HDB_IncludeParents(HDB_curdir);

                        HDB_ShowTree(HDB_tree, -1, -1);
                        HDB_ShowFiles(HDB_curdir, -1, -1);
                        HDB_ShowStats();
                     }
                  }
               }
               break;
            case FILEGAD:
               x = mouse_x - HDB_filegad->LeftEdge;
               y = mouse_y - HDB_filegad->TopEdge;
               xc = SPACELEFTFILE-HDB_filelist.LeftX;
               for(col = 0; col < HDB_filelist.NumCols; col++ )
               {
                  row = xc + HDB_colwidth[col];
                  if(x < row)
                     goto foundcol;
                  xc = row+SPACEBETWEEN;
               }
               break;
foundcol:
               row = y/HDB_filerp->TxHeight;
               if(y<HDB_filelist.WinHeight && row<HDB_filelist.NumRows) {
                  fnode = HDB_filecolumns[col];
                  fnode = HDB_SkipFiles(fnode, row);
                  if(fnode)
                  {
                     HDB_IncludeFile(fnode, (BOOL)((fnode->Flags&ERRF_INCLUDE)==0));
		     oldregion = InstallClipRegion(HDB_treewindow->WLayer,
   				 HDB_filelist.Region);

                     HDB_ShowFile(fnode, xc, 
                        row*HDB_filerp->TxHeight + HDB_filerp->TxBaseline);
		     InstallClipRegion(HDB_treewindow->WLayer, oldregion);
                        
                     if(!(HDB_curdir->Flags & ERRF_INCLUDE))
                     {
                        HDB_IncludeParents(HDB_curdir);
                        HDB_ShowTree(HDB_tree, -1, -1);
                     }
                     HDB_ShowStats();
                  }
               }
               break;
         }
   }
   if(class==IDCMP_GADGETUP &&
      (gid==INCLUDEALLGAD || gid==EXCLUDEALLGAD || gid==TOGGLEALLGAD ||
       gid==INCLUDEARCHGAD || gid==EXCLUDEARCHGAD))
   {
      HDB_ShowFiles(HDB_curdir, -1, -1);
      if(showtree)
         HDB_ShowTree(HDB_tree, -1, -1);
      HDB_ShowStats();
   }
quit:
   return(NOTHING);
}

static void HDB_ChangeCurDir(de)
VBS_DirEntry_t    *de;
{
   int le, te;
   struct Region *oldregion;
 
   oldregion = InstallClipRegion(HDB_treewindow->WLayer, HDB_dirlist.Region);
   le = HDB_dirlist.LeftEdge;
   te = HDB_dirlist.TopEdge;
   SetDrMd(HDB_dirrp, JAM2);

   if(HDB_curde) {
      SetAPen(HDB_dirrp, HDB_curde->dn->Flags&ERRF_INCLUDE ?
         INCLUDECOLOR : EXCLUDECOLOR );
      SetBPen(HDB_dirrp, BCOLOR);
      Move(HDB_dirrp, le+HDB_curde->x, te+HDB_curde->y);
      Text(HDB_dirrp, HDB_curde->dn->Name, HDB_curde->dn->NameLen);
   }
   SetAPen(HDB_dirrp, de->dn->Flags&ERRF_INCLUDE ?
      INCLUDECOLOR : EXCLUDECOLOR );
   SetBPen(HDB_dirrp, CURDIRCOLOR);
   Move(HDB_dirrp, le+de->x, te+de->y);
   Text(HDB_dirrp, de->dn->Name, de->dn->NameLen);
   HDB_curde  = de;
   HDB_curdir = de->dn;
   InstallClipRegion(HDB_treewindow->WLayer, oldregion); 
}

static void HDB_CShowFiles(dn)
VBS_DirNode_t  *dn;
{
   if(HDB_fileshowmode == COLUMNS)
      HDB_ColumnizeFiles(dn);
   else
      HDB_LongListFiles(dn);
   UIG_AdjustProps(HDB_treewindow, &HDB_filelist, 0, 0);
   UIG_FillPadBgnd(&HDB_filelist, BCOLOR);
   HDB_ShowFiles(dn, 0, 0);
}

BOOL HDBG_Init()
{
   BOOL              error = TRUE;
   UWORD             winleft, wintop, winwidth, winheight, xsize,
                     ysize, ysize2, ysize4, third, inileft, initop,
                     iniwidth, iniheight, tmp;
   ULONG             ModeID;
   struct RastPort  *rp;
   struct Rectangle  rect;
   struct NewGadget  newgad;
   struct Gadget    *gad;

   HDB_window    = NULL;

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
      HDB_window = OpenWindowTags(NULL,
                                  WA_Left,         inileft,
                                  WA_Top,          initop,
                                  WA_Width,        iniwidth,
                                  WA_Height,       iniheight,
                                  WA_IDCMP,        (BUTTONIDCMP|SCROLLERIDCMP|IDCMP_VANILLAKEY|IDCMP_REFRESHWINDOW),
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
      HDB_window = OpenWindowTags(NULL,
                                  WA_Left,         inileft,
                                  WA_Top,          initop,
                                  WA_Width,        iniwidth,
                                  WA_Height,       iniheight,
                                  WA_IDCMP,        (BUTTONIDCMP|SCROLLERIDCMP|IDCMP_VANILLAKEY|IDCMP_REFRESHWINDOW),
                                  WA_PubScreen,    VMG_screen,
                                  WA_Activate,     TRUE,
                                  WA_SmartRefresh, TRUE,
                                  WA_DragBar,      TRUE,
                                  WA_DepthGadget,  TRUE,
                               /* WA_MenuHelp,     TRUE, */
                                  TAG_DONE);
   }

   if(!HDB_window)
      goto quit;
   rp = HDB_window->RPort;


   SetFont(HDB_window->RPort, HDB_textfont);
   HDB_thisprocess = (struct Process *)FindTask(NULL);
   HDB_oldwindow = (struct Window *)HDB_thisprocess->pr_WindowPtr;
   HDB_thisprocess->pr_WindowPtr = (APTR)HDB_window;

   winleft   = HDB_window->BorderLeft;
   wintop    = HDB_window->BorderTop;
   winwidth  = HDB_window->Width  - HDB_window->BorderRight  - winleft;
   winheight = HDB_window->Height - HDB_window->BorderBottom - wintop;

   xsize     = TextLength(HDB_window->RPort, "h", 1);
   ysize     = VBSG_text_ta.ta_YSize;
   ysize2    = ysize >> 1;
   ysize4    = ysize >> 2;

   third     = winwidth/3;

   if(!(HDB_visinfo = GetVisualInfo(VMG_screen, TAG_END)))
      goto quit;

   gad = CreateContext(&HDB_glist);

   tmp = PIXLEN(rp, CANCELTX) + xsize*2;
   newgad.ng_Width      = tmp;
   newgad.ng_Height     = ysize + ysize2;
   newgad.ng_TextAttr   = &VBSG_text_ta;
   newgad.ng_VisualInfo = HDB_visinfo;
   newgad.ng_Flags      = PLACETEXT_IN;

   newgad.ng_LeftEdge   = winleft + xsize;
   newgad.ng_TopEdge    = wintop + winheight - ysize*2 - ysize2*5;
   newgad.ng_GadgetText = ADDTX;
   newgad.ng_GadgetID   = ADDGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge   = winleft + (winwidth - tmp)>>1;
   newgad.ng_GadgetText = MODIFYTX;
   newgad.ng_GadgetID   = MODIFYGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GA_Disabled,   !HDB_NumBackups,
                      GT_Underscore, '_',
                      TAG_END);
   HDB_modifygad = gad;

   newgad.ng_LeftEdge   = winleft + winwidth - tmp - xsize;
   newgad.ng_GadgetText = DELETETX;
   newgad.ng_GadgetID   = DELETEGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GA_Disabled,   !HDB_NumBackups,
                      GT_Underscore, '_',
                      TAG_END);
   HDB_deletegad = gad;

   newgad.ng_TopEdge    = wintop + winheight - ysize - ysize2*2;
   newgad.ng_GadgetText = CANCELTX;
   newgad.ng_GadgetID   = CANCELGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   tmp                  = PIXLEN(rp, STARTTIMEFORMATTX) + xsize*2;
   newgad.ng_LeftEdge   = winleft + ((winwidth - tmp)>>1);
   newgad.ng_Width      = tmp;
   newgad.ng_GadgetText = STARTTIMETX;
   newgad.ng_GadgetID   = TIMEGAD;
   newgad.ng_Flags      = PLACETEXT_LEFT;
   gad = CreateGadget(STRING_KIND, gad, &newgad,
                      GTST_String,   HDB_starttime,
                      GTST_MaxChars, 8,
                      GA_Disabled,   !HDB_NumBackups,
                      GT_Underscore, '_',
                      TAG_END);
   HDB_timegad = gad;

   newgad.ng_LeftEdge   = winleft + xsize;
   newgad.ng_Width      = PIXLEN(rp, STARTBACKUPSTX) + xsize*2;
   newgad.ng_GadgetText = STARTBACKUPSTX;
   newgad.ng_GadgetID   = STARTGAD;
   newgad.ng_Flags      = PLACETEXT_IN;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GA_Disabled,   !HDB_NumBackups,
                      GT_Underscore, '_',
                      TAG_END);
   HDB_startgad = gad;

   newgad.ng_TopEdge    = wintop + ysize + ysize2*2;
   newgad.ng_Width      = winwidth - xsize*2;
   newgad.ng_Height     = winheight - ysize*4 - ysize2*6;
   newgad.ng_Flags      = PLACETEXT_ABOVE;
   newgad.ng_GadgetText = LISTVIEWTX;
   newgad.ng_GadgetID   = LISTVIEWGAD;
   gad = CreateGadget(LISTVIEW_KIND, gad, &newgad,
                      GTLV_ShowSelected, NULL,
                      GT_Underscore,     '_',
/*                      GTLV_Labels,       &HDB_BackupsList,*/
                      TAG_END);
   HDB_listviewgad = gad;

   if(!gad)
      goto quit;

   AddGList(HDB_window, HDB_glist, 0, -1, NULL);
   RefreshGList(HDB_glist, HDB_window, NULL, -1);
   GT_RefreshWindow(HDB_window, NULL);

   error = FALSE;
quit:
   return(error);
}

VOID HDBG_Cleanup()
{
   if(HDB_window)
   {
      HDB_thisprocess->pr_WindowPtr = (APTR)HDB_oldwindow;
      CloseWindow(HDB_window);
   }
   FreeGadgets(HDB_glist);
   FreeVisualInfo(HDB_visinfo);

   HDB_window    = NULL;
}
#define STANDARDTX	"68000 Std."
#define FASTTX		"68020 Fast"
#define FASTCOMPRTX	"68020 Compr."

static  VBS_error_t HDB_AskTree(UBYTE *pathptr, VBS_DirNode_t **tree,\
		 BOOL readtree, BOOL placebtypegad, BOOL placecancelgad)
{
   VBS_error_t              error = EMEMORY;

   struct RastPort         *rp;
   struct Gadget           *gad, *btypegad = NULL, *glist = NULL;
   struct NewGadget         newgad;
   void                    *visinfo = NULL;
   UWORD                    winleft, wintop, winwidth, winheight, xsize,
                            ysize, ysize2, ysize4, third, ybase, x, y, tmp;
   BOOL                     laced;
   static char		   *BackupTypes[] = {
   	STANDARDTX, FASTTX, FASTCOMPRTX, NULL
   };
   
   int			    a;
      	
   UIG_BlockInput(HDB_window);

   xsize     = TextLength(HDB_window->RPort, "h", 1);
   ysize     = VBSG_text_ta.ta_YSize;
   ysize2    = ysize >> 1;
   ysize4    = ysize >> 2;

   winleft   = HDB_window->WScreen->WBorLeft;
   wintop    = HDB_window->WScreen->WBorTop+
               HDB_window->WScreen->Font->ta_YSize + 1;
   winwidth  = HDB_window->Width  - HDB_window->WScreen->WBorRight  - winleft - xsize*4;
   winheight = HDB_window->Height - HDB_window->WScreen->WBorBottom - wintop - ysize;

   third     = winwidth/3;

   if((winheight*2) < winwidth)
      laced = FALSE;
   else
      laced = TRUE;

   if(!(visinfo = GetVisualInfo(HDB_window->WScreen, TAG_END)))
      goto quit;

   newgad.ng_Height     = ysize * 3/2;
   newgad.ng_TextAttr   = &VBSG_text_ta;
   newgad.ng_VisualInfo = visinfo;
   newgad.ng_Flags      = PLACETEXT_IN;

   gad = CreateContext(&glist);

   ybase = wintop + ysize*2 + ysize2*3 + ysize4;
   tmp   = PIXLEN(HDB_window->RPort, EXCLUDEARCHTX) + xsize;
   newgad.ng_TopEdge    = ybase;
   newgad.ng_LeftEdge   = winleft + xsize;
   newgad.ng_Width      = tmp;
   newgad.ng_GadgetText = INCLUDEALLTX;
   newgad.ng_GadgetID   = INCLUDEALLGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge   = winleft + (winwidth - tmp)>>1;
   newgad.ng_GadgetText = EXCLUDEALLTX;
   newgad.ng_GadgetID   = EXCLUDEALLGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge   = winleft + winwidth - tmp - xsize;
   newgad.ng_GadgetText = TOGGLEALLTX;
   newgad.ng_GadgetID   = TOGGLEALLGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   ybase += ysize + ysize2 + ysize4;
   newgad.ng_TopEdge    = ybase;
   newgad.ng_LeftEdge   = winleft + xsize;
   newgad.ng_GadgetText = INCLUDEARCHTX;
   newgad.ng_GadgetID   = INCLUDEARCHGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge   = winleft + (winwidth - tmp)>>1;
   newgad.ng_GadgetText = EXCLUDEARCHTX;
   newgad.ng_GadgetID   = EXCLUDEARCHGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   tmp = PIXLEN(HDB_window->RPort, LONGFILELISTTX);
   newgad.ng_Width      = xsize*3;
   newgad.ng_LeftEdge   = winleft + winwidth - tmp - xsize*6;
   newgad.ng_GadgetText = LONGFILELISTTX;
   newgad.ng_GadgetID   = LONGFILELISTGAD;
   newgad.ng_Flags      = PLACETEXT_RIGHT;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);
   HDB_longlistgad  = gad;
   HDB_fileshowmode = COLUMNS;

   x = winleft + winwidth - xsize;

   if(placecancelgad) {
	   tmp = PIXLEN(HDB_window->RPort, CANCELTX) + xsize;
	   newgad.ng_Width      = tmp;
	   x-=tmp+xsize;
	   newgad.ng_LeftEdge   = x+xsize;
	   newgad.ng_TopEdge    = wintop + winheight - ysize - ysize2*2;
	   newgad.ng_GadgetText = CANCELTX;
	   newgad.ng_GadgetID   = SELCANCELGAD;
	   newgad.ng_Flags      = PLACETEXT_IN;
	   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);
           
   }

   tmp = PIXLEN(HDB_window->RPort, DONETX) + xsize;
   newgad.ng_Width      = tmp;
   x -= tmp+xsize;
   newgad.ng_LeftEdge   = x+xsize;
   newgad.ng_TopEdge    = wintop + winheight - ysize - ysize2*2;
   newgad.ng_GadgetText = DONETX;
   newgad.ng_GadgetID   = DONEGAD;
   newgad.ng_Flags      = PLACETEXT_IN;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   if(placebtypegad) {
   tmp			= PIXLEN(HDB_window->RPort, FASTCOMPRTX) + xsize*5;
   newgad.ng_Width      = tmp;
   x -= tmp+xsize;
   newgad.ng_LeftEdge   = x+xsize;
   newgad.ng_GadgetText = NULL;
   newgad.ng_GadgetID   = BACKUPTYPEGAD;
   if(VBSG_is68000)
   	BackupTypes[1] = NULL;
   switch(HDB_backuptype) {
   case BT_FS:
   	a = 0; break;
   case BT_FSFAST:
   	a = 1; break;
   case BT_FSFASTCOMPR:
   	a = 2;
   }
   gad = CreateGadget(CYCLE_KIND, gad, &newgad,
                      GTCY_Labels,	BackupTypes,
                      GTCY_Active,   	a,
                      GT_Underscore, 	'_',
                      TAG_END);
   btypegad = gad;
   }
   
      tmp = x - xsize*2 - PIXLEN(HDB_window->RPort, BACKUPNAMETX)-winleft;
      newgad.ng_Width      = tmp;
      newgad.ng_LeftEdge   = winleft + PIXLEN(HDB_window->RPort, BACKUPNAMETX) + xsize*2;
      newgad.ng_GadgetText = BACKUPNAMETX;
      newgad.ng_GadgetID   = NAMEGAD;
      newgad.ng_Flags      = PLACETEXT_LEFT;
      gad = CreateGadget(STRING_KIND, gad, &newgad,
                         GTST_String,   HDB_BackupName,
                         GTST_MaxChars, 80,
                         GT_Underscore, '_',
                         TAG_END);
      HDB_namegad = gad;

   x      = 16;
   y      = (laced?17:9);
   ybase += ysize + ysize2 + ysize4;

   newgad.ng_GadgetText = NULL;
   newgad.ng_TopEdge    = ybase;
   newgad.ng_LeftEdge   = winleft + third - x;
   newgad.ng_Width      = x;
   newgad.ng_Height     = wintop + winheight - ybase - ysize - ysize2*3 - y;
   newgad.ng_GadgetID   = DIRVER;
   gad = CreateGadget(SCROLLER_KIND, gad, &newgad,
                      GTSC_Arrows,   y,
                      PGA_Freedom,   LORIENT_VERT,
                      TAG_END);
   HDB_dirver = gad;

   newgad.ng_TopEdge    = winheight + wintop - ysize - ysize2*3 - y;
   newgad.ng_LeftEdge   = winleft + xsize;
   newgad.ng_Width      = third - xsize - x;
   newgad.ng_Height     = y;
   newgad.ng_GadgetID   = DIRHOR;
   gad = CreateGadget(SCROLLER_KIND, gad, &newgad,
                      GTSC_Arrows,   xsize*2,
                      PGA_Freedom,   LORIENT_HORIZ,
                      TAG_END);
   HDB_dirhor = gad;

   newgad.ng_TopEdge    = ybase;
   newgad.ng_Height     = wintop + winheight - ybase - ysize - ysize2*3 - y;
   newgad.ng_GadgetID   = DIRGAD;
   gad = CreateGadget(GENERIC_KIND, gad, &newgad, TAG_END);
   if(gad)
   {
      gad->Flags |= GFLG_GADGHNONE;
      gad->Activation |= GACT_IMMEDIATE;
   }
   HDB_dirgad = gad;

   HDB_dirlist.PadGadget   = HDB_dirgad;
   HDB_dirlist.HorizGadget = HDB_dirhor;
   HDB_dirlist.VertGadget  = HDB_dirver;

   newgad.ng_TopEdge    = ybase;
   newgad.ng_LeftEdge   = winleft + winwidth - x - xsize;
   newgad.ng_Width      = x;
   newgad.ng_Height     = wintop + winheight - ybase - ysize - ysize2*3 - y;
   newgad.ng_GadgetID   = FILEVER;
   gad = CreateGadget(SCROLLER_KIND, gad, &newgad,
                      GTSC_Arrows,   y,
                      PGA_Freedom,   LORIENT_VERT,
                      TAG_END);
   HDB_filever = gad;

   newgad.ng_TopEdge    = winheight + wintop - ysize - ysize2*3 - y;
   newgad.ng_LeftEdge   = winleft + third + xsize;
   newgad.ng_Width      = winwidth - third - xsize*2 - x;
   newgad.ng_Height     = y;
   newgad.ng_GadgetID   = FILEHOR;
   gad = CreateGadget(SCROLLER_KIND, gad, &newgad,
                      GTSC_Arrows,   xsize*2,
                      PGA_Freedom,   LORIENT_HORIZ,
                      TAG_END);
   HDB_filehor = gad;

   newgad.ng_TopEdge    = ybase;
   newgad.ng_Height     = wintop + winheight - ybase - ysize - ysize2*3 - y;
   newgad.ng_GadgetID   = FILEGAD;
   gad = CreateGadget(GENERIC_KIND, gad, &newgad, TAG_END);
   if(gad)
   {
      gad->Flags |= GFLG_GADGHNONE;
      gad->Activation |= GACT_IMMEDIATE;
   }
   HDB_filegad = gad;

   HDB_filelist.PadGadget   = HDB_filegad;
   HDB_filelist.HorizGadget = HDB_filehor;
   HDB_filelist.VertGadget  = HDB_filever;

   if(!gad)
      goto quit;

   HDB_treewindow = OpenWindowTags(NULL,
                           (VBSG_screentype==PUBLICST?WA_PubScreen:WA_CustomScreen),
                                           HDB_window->WScreen,
                           WA_Activate,    TRUE,
                           WA_Left,        ((HDB_window->Width  - winwidth)  >> 1),
                           WA_Top,         ((HDB_window->Height - winheight) >> 1),
                           WA_InnerWidth,  winwidth,
                           WA_InnerHeight, winheight,
                           WA_AutoAdjust,  TRUE,
                           WA_DragBar,     TRUE,
                           WA_DepthGadget, TRUE,
                           WA_Title,       FILESDIRSSELTX,
                           WA_IDCMP,       (IDCMP_REFRESHWINDOW | IDCMP_VANILLAKEY | IDCMP_GADGETDOWN | BUTTONIDCMP | CHECKBOXIDCMP | STRINGIDCMP | ARROWIDCMP | SCROLLERIDCMP),
                           WA_Gadgets,     glist,
                           TAG_END);

   if(!HDB_treewindow)
      goto quit;
   UIG_FillWindowBgnd(HDB_treewindow, FILLCOLOR);   
   RefreshGList(glist, HDB_treewindow, NULL, -1);  
   GT_RefreshWindow(HDB_treewindow, NULL);

   HDB_winrp = HDB_treewindow->RPort;
   SetFont(HDB_winrp, HDB_textfont);

   rp = HDB_treewindow->RPort;

   SetDrMd(rp, JAM2);
   SetAPen(rp, BOXCOLOR);
   SetOPen(rp, BOXBORDERCOLOR);
   RectFill(rp, winleft + xsize,            wintop + ysize4,
                winleft + winwidth - xsize, wintop + ysize*2 + ysize2*2 + ysize4);
   BNDRYOFF(rp);
#if 0
   SetAPen(rp, BOXTEXTCOLOR1);
   SetBPen(rp, BOXCOLOR);
   Move(rp, winleft + xsize*2, wintop + ysize + ysize2*3 + ysize4);
   Text(rp, DIRECTORIESTX, LEN(DIRECTORIESTX));
   Move(rp, winleft + third + xsize, wintop + ysize + ysize2*3 + ysize4);
   Text(rp, FILESTX, LEN(FILESTX));
   Move(rp, winleft + third*2 + xsize, wintop + ysize + ysize2*3 + ysize4);
   Text(rp, BYTESTX, LEN(BYTESTX));
#endif

   UIG_InitPad(&HDB_dirlist, HDB_treewindow);
   UIG_InitPad(&HDB_filelist, HDB_treewindow);

   HDB_dirrp  = &HDB_dirlist.RastPort;
   HDB_filerp = &HDB_filelist.RastPort;

   UIG_FillPadBgnd(&HDB_dirlist, BCOLOR);

   SetDrMd(HDB_dirrp,  JAM1);
   SetDrMd(HDB_filerp, JAM1);
   SetAPen(HDB_dirrp,  1);
   SetAPen(HDB_filerp, 1);

   SetAPen(rp, 1);

   if(readtree)
   {
      HDB_tree = NULL;
      if(error = BAG_ScanDirectories(&HDB_tree, pathptr, HDB_dirrp,
                    &HDB_totalbytes, &HDB_numfiles, &HDB_numdirs,
                    &HDB_dirlist,SPACELEFTDIR,INSPRING))
      {
         ERRG_Message(HDB_window, EMEMORY);
         goto quit;
      }
      SRTG_SortTree(&HDB_tree);
      *tree = HDB_tree;
   }
   else {
      HDB_tree = *tree;
      BAG_PrepareTree(HDB_tree, HDB_dirrp,
          &HDB_totalbytes, &HDB_numfiles, &HDB_numdirs,
          &HDB_dirlist, SPACELEFTDIR, INSPRING);
   }
      

   if(pathptr) {
      SetAPen(rp, BOXTEXTCOLOR1);
      SetDrMd(rp, JAM1);
      Move(rp, winleft + xsize*2, wintop + ysize2*2 + ysize4*2);
      Text(rp, BACKUPFROMTX, LEN(BACKUPFROMTX));
      SetAPen(rp, BOXTEXTCOLOR2);
      Move(rp, winleft + PIXLEN(rp, BACKUPFROMTX) + xsize*4, wintop + ysize2*2 + ysize4*2);
      Text(rp, HDB_tree->Name, HDB_tree->NameLen);
   }
   HDB_Wwidth     = HDB_filelist.Wwidth;
   HDB_direntries = (VBS_DirEntry_t *)AllocVec(HDB_dirlist.WinRows*
   					       sizeof(VBS_DirEntry_t),
   					       MEMF_CLEAR);
   					       
   if(!HDB_direntries)
   	goto quit;

/*   if(HDB_dirlist.WinRows > MAXDIRROWS)
      HDB_dirlist.WinRows = MAXDIRROWS;
*/
   HDB_SelectInit();

   error = HDB_SelectFiles();

   strcpy(HDB_BackupName, ((struct StringInfo *)HDB_namegad->SpecialInfo)->Buffer);

   UIG_CleanupPad(&HDB_dirlist);
   UIG_CleanupPad(&HDB_filelist);

quit:
   if(HDB_direntries) {
   	FreeVec((char *)HDB_direntries);
   	HDB_direntries = NULL;
   }
   if(HDB_treewindow)
      CloseWindow(HDB_treewindow);
   FreeGadgets(glist);
   FreeVisualInfo(visinfo);
   UIG_UnBlockInput(HDB_window);
   return(error);
}

static HDB_BackupTree_t *HDB_RemoveNode(HDB_BackupTree_t *node)
{
   HDB_BackupTree_t *prev, *next, *temp;

   prev = (HDB_BackupTree_t *)node->bt_Node.ln_Pred;
   next = (HDB_BackupTree_t *)node->bt_Node.ln_Succ;
   if(next->bt_Node.ln_Succ)
      temp = next;
   else
   {
      if(prev->bt_Node.ln_Pred)
         temp = prev;
      else
         temp = NULL;
   }

   Remove((struct Node *)node);
   BAG_FreeTree(node->bt_DirTree);
   FreeVec(node);
   HDB_NumBackups--;

   return(temp);
}

static HDB_BackupTree_t *HDB_SkipNodes(struct MinList *list, UWORD num)
{
   struct Node *node;
   UWORD        count;

   count = 0;
   node = (struct Node *)list->mlh_Head;

   while(count!=num && node->ln_Succ!=NULL)
   {
      node = node->ln_Succ;
      count++;
   }

   if(node->ln_Succ==NULL)
      return(NULL);
   else
      return((HDB_BackupTree_t *)node);
}

static UWORD HDB_NodeNum(struct MinList *list, HDB_BackupTree_t *tree)
{
   UWORD        result;
   struct Node *node;

   result = 0;
   node = (struct Node *)list->mlh_Head;
   while(node->ln_Succ!=NULL)
   {
      if(node == (struct Node *)tree)
         break;
      result++;
      node = node->ln_Succ;
   }
   if(node->ln_Succ==NULL)
      result = ~0;
   return(result);
}

static void HDB_ShowTree(dn, leftx, toprow)
VBS_DirNode_t  *dn;
int             leftx, toprow;
{
   int	 	  le, te;
   struct Region *oldregion;
   
   le = HDB_dirlist.LeftEdge;
   te = HDB_dirlist.TopEdge;
 
   oldregion = InstallClipRegion(HDB_treewindow->WLayer, HDB_dirlist.Region);
   
   
   if(leftx<0)
      leftx = HDB_dirlist.LeftX;
   if(toprow<0)
      toprow = HDB_dirlist.TopRow;
   HDB_diry = -toprow*HDB_dirrp->TxHeight;
   HDB_maxy = HDB_dirlist.WinHeight;
   HDB_de   = &HDB_direntries[0];

   SetBPen(HDB_dirrp, BCOLOR);
   ScrollRaster(HDB_dirrp,
                leftx-HDB_dirlist.LeftX,
                (toprow-HDB_dirlist.TopRow)*HDB_dirrp->TxHeight,
                le, te,
                le+HDB_dirlist.WinWidth-1, te+HDB_dirlist.WinHeight-1);
   HDB_curde = NULL;
   HDB_ShowDirectories(dn, SPACELEFTDIR-leftx, SPACEBETWEEN, -leftx, HDB_diry);
/*   SetAPen(HDB_dirrp, BCOLOR);
   y = HDB_dirlist.WinHeight;
   if(y!=HDB_dirlist.PadGadget->Height) {
      RectFill(HDB_dirrp, 0, y, HDB_dirlist.WinWidth-1,
               HDB_dirlist.PadGadget->Height-1);
   }*/
   HDB_dirlist.LeftX  = leftx;
   HDB_dirlist.TopRow = toprow;
   InstallClipRegion(HDB_treewindow->WLayer, oldregion);
}

static void HDB_ShowDirectories(dn, xoffset, xdelt, xp, yp)
VBS_DirNode_t     *dn;
int                xoffset, xdelt;
int                xp, yp;
{
   int      y;
   int	 le, te;
   
   le = HDB_dirlist.LeftEdge;
   te = HDB_dirlist.TopEdge;
 
   while(dn) {
      SetAPen(HDB_dirrp, DIRTREECOLOR);
      y = HDB_diry+(HDB_dirrp->TxHeight>>1);
      RectFill(HDB_dirrp, le+xp, te+yp,  le+xp+3, te+y);
      RectFill(HDB_dirrp, le+xp, te+y-1, le+xoffset-2, te+y);
      if(HDB_diry >=0 && HDB_diry < HDB_maxy) {
         if(HDB_curdir == dn) {
            SetBPen(HDB_dirrp, CURDIRCOLOR);
            SetDrMd(HDB_dirrp, JAM2);
            HDB_curde = HDB_de;
         }
         SetAPen(HDB_dirrp, dn->Flags&ERRF_INCLUDE ?
            INCLUDECOLOR : EXCLUDECOLOR);
         y = HDB_diry;
         Move(HDB_dirrp, le+xoffset, te+HDB_diry+HDB_dirrp->TxBaseline);
         HDB_de->dn = dn;
         HDB_de->x  = xoffset;
         HDB_de->y  = HDB_diry+HDB_dirrp->TxBaseline;
         HDB_de++;
         Text(HDB_dirrp, dn->Name, dn->NameLen);
         if(HDB_curdir == dn) {
            SetDrMd(HDB_dirrp, JAM1);
         }
      }
      HDB_diry += HDB_dirrp->TxHeight;
      HDB_ShowDirectories(dn->FirstDir, xoffset+xdelt, xdelt, xoffset+2,
                          HDB_diry);
      dn = dn->Next;
   }
}

static void HDB_ColumnizeFiles(dn)
VBS_DirNode_t  *dn;
{
   VBS_FileNode_t    *fnode;

   int                totlen = 0, maxnamlen = 0;
   int                avglen = 0;
   int                maxcol = 0, maxrow = 0;
   int                colcnt, rowcnt;
   int                tlen;
   int                winwidth;


   fnode = dn->FirstFile;
   HDB_filelist.NumEntries = 0;

   while(fnode) {
      tlen = fnode->TextLength;
      totlen += tlen;
      if(tlen > maxnamlen)
         maxnamlen = tlen;
      HDB_filelist.NumEntries++;
      fnode = fnode->Next;
   }
   if(!HDB_filelist.NumEntries)
      goto quit;

   avglen = (totlen+HDB_filelist.NumEntries-1)/HDB_filelist.NumEntries;
   winwidth = HDB_filelist.WinWidth;
   if(maxnamlen >= winwidth) {
      maxcol = 1;
   } else {
      maxcol = winwidth / (avglen + SPACEBETWEEN);
      if(maxcol > MAXCOL)
         maxcol = MAXCOL;
   }

  for (;;)
  {
  /* Clear out previous HDB_colwidth */
    memset(HDB_colwidth, 0, sizeof(HDB_colwidth));

  /* Number of rows = total entries / entries per row */
    maxrow = (HDB_filelist.NumEntries+maxcol-1) / maxcol;

    for (colcnt = 0, fnode = dn->FirstFile; fnode; colcnt++)
    {
      for (rowcnt = 0; rowcnt < maxrow && fnode; rowcnt++)
      {
        tlen = fnode->TextLength;
        if (tlen > HDB_colwidth[colcnt])
          HDB_colwidth[colcnt] = tlen;
        fnode = fnode->Next;
      }
   }
   maxcol = colcnt;
   maxrow = (HDB_filelist.NumEntries+maxcol-1) / maxcol;

  /* Calculate actual total width by adding up width of all columns */
    for (colcnt = totlen = 0; (colcnt + 1) < maxcol; colcnt++)
    {
      totlen += (LONG)HDB_colwidth[colcnt] + SPACEBETWEEN;
    }
    totlen += (LONG)HDB_colwidth[colcnt];

    if (maxcol > 1 && totlen > winwidth)
    {
      maxcol--;
    }
    else
      break;
  }
quit:
  HDB_filelist.NumRows       = maxrow;
  HDB_filelist.NumCols       = maxcol;
  HDB_filelist.MaxEltWidth   = maxnamlen;
  HDB_filelist.MaxTotalWidth = totlen+SPACELEFTFILE;

}

static void HDB_LongListFiles(dn)
VBS_DirNode_t  *dn;
{
   VBS_FileNode_t    *fnode;

   int                maxnamlen = 1;
   int                tlen;


   fnode = dn->FirstFile;
   HDB_filelist.NumEntries = 0;

   while(fnode) {
      tlen = fnode->TextLength;
      if(tlen > maxnamlen)
         maxnamlen = tlen;
      HDB_filelist.NumEntries++;
      fnode = fnode->Next;
   }
   HDB_colwidth[0]            = maxnamlen + HDB_Wwidth*36;
   HDB_filelist.NumRows       = HDB_filelist.NumEntries;
   HDB_filelist.NumCols       = 1;
   HDB_filelist.MaxEltWidth   = maxnamlen;
   HDB_filelist.MaxTotalWidth = HDB_colwidth[0] + SPACELEFTFILE;
}

static VBS_FileNode_t   *HDB_SkipFiles(fn, n)
VBS_FileNode_t    *fn;
int                n;
{
   while(n && fn) {
      fn = fn->Next;
      n--;
   }
   return(fn);
}


static void HDB_ShowFiles(dn, leftx, toprow)
VBS_DirNode_t  *dn;
int             leftx, toprow;
{
   VBS_FileNode_t    *fnode;
   int                row, col;
   int                x, y;
   int                winrows;
   int	 le, te;
   struct Region *oldregion;
   
   le = HDB_filelist.LeftEdge;
   te = HDB_filelist.TopEdge;
 
   oldregion = InstallClipRegion(HDB_treewindow->WLayer,
   				 HDB_filelist.Region);

   winrows = HDB_filelist.WinRows;
   SetBPen(HDB_filerp, BCOLOR);
   if(leftx  < 0) leftx  = HDB_filelist.LeftX;
   if(toprow < 0) toprow = HDB_filelist.TopRow;
   ScrollRaster(HDB_filerp,
                leftx - HDB_filelist.LeftX,
                (toprow - HDB_filelist.TopRow)*HDB_filerp->TxHeight,
                le, te,
                le+HDB_filelist.WinWidth-1, te+HDB_filelist.WinHeight-1);

   if(HDB_filelist.NumRows < winrows) {
      winrows  = HDB_filelist.NumRows;
      toprow = 0;
   }
   fnode = dn->FirstFile;
   x = SPACELEFTFILE-leftx;
      for(col = 0; col<HDB_filelist.NumCols; col++) {
         fnode = HDB_SkipFiles(fnode, toprow);
         y = HDB_filerp->TxBaseline;
         HDB_filecolumns[col] = fnode;
         for(row = 0; fnode && row<winrows; row++) {
            HDB_ShowFile(fnode, x, y);
            y += HDB_filerp->TxHeight;
            fnode = fnode->Next;
         }
         fnode = HDB_SkipFiles(fnode, HDB_filelist.NumRows - toprow - winrows);
         x += HDB_colwidth[col]+SPACEBETWEEN;
      }

   HDB_filelist.TopRow = toprow;
   HDB_filelist.LeftX  = leftx;
   InstallClipRegion(HDB_treewindow->WLayer, oldregion);
}

static void HDB_ShowFile(fnode, x, y)
VBS_FileNode_t    *fnode;
int                x, y;
{
   int                cx;
   int                l;
   static UBYTE       string[12];

   int	 le, te;
   
   le = HDB_filelist.LeftEdge;
   te = HDB_filelist.TopEdge;
 
   Move(HDB_filerp, le+x, te+y);
   SetAPen(HDB_filerp, fnode->Flags & ERRF_INCLUDE ?
               INCLUDECOLOR : EXCLUDECOLOR);
   Text(HDB_filerp, fnode->Name, fnode->NameLen);

   if(HDB_fileshowmode == LONGLIST) {

      cx = x+HDB_filelist.MaxEltWidth+HDB_Wwidth*8;
      l = stci_d(string, fnode->Size);
      Move(HDB_filerp, le+cx- TextLength(HDB_filerp, string, l), te+y);
      Text(HDB_filerp, string, l);
      cx += HDB_Wwidth;
      Move(HDB_filerp, le+cx, te+y);

      Text(HDB_filerp, UIG_MakeProtString(fnode->Protection), 8);
      cx += HDB_Wwidth*9;
      Move(HDB_filerp, le+cx, te+y);
      l = UIG_Date(string, fnode->Date.ds_Days);
      Text(HDB_filerp, string, l);
      cx += HDB_Wwidth*(l+1);
      Move(HDB_filerp, le+cx, te+y);
      l = UIG_Time(string, fnode->Date.ds_Minute, fnode->Date.ds_Tick);
      Text(HDB_filerp, string, l);
                                 /*36*/
   }
}

static void HDB_IncludeTree(dn, include)
VBS_DirNode_t  *dn;
BOOL            include;
{
   HDB_IncludeDirNode(dn, include);
   HDB_IncludeFiles(dn, include);
   dn = dn->FirstDir;
   while(dn) {
      HDB_IncludeTree(dn, include);
      dn = dn->Next;
   }
}

static BOOL HDB_ToggleFiles(dn)
VBS_DirNode_t  *dn;
{
   VBS_FileNode_t  *fnode;
   BOOL             include, oinclude = FALSE;

   fnode = dn->FirstFile;
   while(fnode) {
      include = ((fnode->Flags & ERRF_INCLUDE) == 0);
      HDB_IncludeFile(fnode, include);
      oinclude |= include;
      fnode = fnode->Next;
   }
   if(oinclude && !(dn->Flags & ERRF_INCLUDE)) {
      HDB_IncludeParents(dn); return(TRUE);
   }
   return(FALSE);
}

static BOOL HDB_IncludeFiles(dn, include)
VBS_DirNode_t  *dn;
BOOL            include;
{
   VBS_FileNode_t  *fnode;
   BOOL             oinclude = FALSE;

   fnode = dn->FirstFile;
   while(fnode) {
      oinclude = include;
      HDB_IncludeFile(fnode, include);
      fnode = fnode->Next;
   }
   if(oinclude && !(dn->Flags & ERRF_INCLUDE)) {
      HDB_IncludeParents(dn); return(TRUE);
   }
   return(FALSE);
}
static void HDB_IncludeDirNode(dn, include)
VBS_DirNode_t  *dn;
BOOL            include;
{
   UBYTE            f;

   f = include ? ERRF_INCLUDE : 0;
   if((dn->Flags&ERRF_INCLUDE) != f) {
      dn->Flags^=ERRF_INCLUDE;
      HDB_numdirs += include ? 1 : -1;
   }
}
static void HDB_IncludeFile(fn, include)
VBS_FileNode_t *fn;
BOOL            include;
{
   UBYTE           f;

   f = include ? ERRF_INCLUDE : 0;

   if((fn->Flags&ERRF_INCLUDE) != f) {
      fn->Flags^=ERRF_INCLUDE;
      if(include) {
         HDB_totalbytes += fn->Size;
         HDB_numfiles++;
      } else {
         HDB_totalbytes -= fn->Size;
         HDB_numfiles--;
      }
   }
}

static void HDB_SetFlags(dn, flags, mask)
VBS_DirNode_t  *dn;
register UBYTE           flags, mask;
{
   VBS_FileNode_t *fnode;

   dn->Flags = (dn->Flags&~mask) | flags;
   fnode = dn->FirstFile;
   while(fnode) {
      fnode->Flags = (fnode->Flags&~mask) | flags;
      fnode = fnode->Next;
   }
   dn = dn->FirstDir;
   while(dn) {
      HDB_SetFlags(dn, flags, mask);
      dn = dn->Next;
   }
}
static void HDB_IncludeParents(dn)
VBS_DirNode_t  *dn;
{
   while(dn) {
      HDB_IncludeDirNode(dn, TRUE);
      dn = dn->ParentDir;
   }
}
static BOOL HDB_IncludeArch(dn, include)
VBS_DirNode_t  *dn;
BOOL            include;
{
   VBS_FileNode_t   *fnode;
   BOOL              oinclude = FALSE, ret = FALSE;

   fnode = dn->FirstFile;
   while(fnode) {
      if(fnode->Protection & ARCHIVEBIT) {
         HDB_IncludeFile(fnode, include); oinclude |= include;
      }
      fnode = fnode->Next;
   }
   if(oinclude && !(dn->Flags & ERRF_INCLUDE)) {
      HDB_IncludeParents(dn); ret = TRUE;
   }
   dn = dn->FirstDir;
   while(dn) {
      ret |= HDB_IncludeArch(dn, include);
      dn = dn->Next;
   }
   return(ret);
}

static void HDB_ShowStats()
{
   UBYTE    s[25];
   UWORD    xsize, ysize, ysize2, ysize4, third, winleft, wintop, winwidth, winheight;

   struct RastPort *rp = HDB_treewindow->RPort;
   
   
   xsize     = TextLength(rp, "h", 1);
   ysize     = VBSG_text_ta.ta_YSize;
   ysize2    = ysize >> 1;
   ysize4    = ysize >> 2;

   winleft   = HDB_window->WScreen->WBorLeft;
   wintop    = HDB_window->WScreen->WBorTop+
               HDB_window->WScreen->Font->ta_YSize + 1;
   winwidth  = HDB_window->Width  - HDB_window->WScreen->WBorRight  - winleft - xsize*4;
   winheight = HDB_window->Height - HDB_window->WScreen->WBorBottom - wintop - ysize;

   third     = winwidth/3;
   

   SetDrMd(rp, JAM2);
   SetAPen(rp, BOXTEXTCOLOR1);
   SetBPen(rp, BOXCOLOR);
   Move(rp, winleft + xsize*2, wintop + ysize + ysize2*3 + ysize4);
   Text(rp, DIRECTORIESTX, LEN(DIRECTORIESTX));
   Move(rp, winleft + third + xsize, wintop + ysize + ysize2*3 + ysize4);
   Text(rp, FILESTX, LEN(FILESTX));
   Move(rp, winleft + third*2 + xsize, wintop + ysize + ysize2*3 + ysize4);
   Text(rp, BYTESTX, LEN(BYTESTX));

   SetAPen(rp, BOXTEXTCOLOR2);
   SetBPen(rp, BOXCOLOR);
   SetDrMd(rp, JAM2);

   sprintf(s, "%-8ld", HDB_numdirs);
   Move(rp, winleft + PIXLEN(rp, DIRECTORIESTX) + xsize*4,
                   wintop + ysize + ysize2*3 + ysize4);
   Text(rp, s, 8);

   sprintf(s, "%-10ld  ", HDB_numfiles);
   Move(rp, winleft + third + PIXLEN(rp, FILESTX) + xsize*3,
                   wintop + ysize + ysize2*3 + ysize4);
   Text(rp, s, 10);

   sprintf(s, "%-18ld  ", HDB_totalbytes);
   Move(rp, winleft + third*2 + PIXLEN(rp, BYTESTX) + xsize*3,
                   wintop + ysize + ysize2*3 + ysize4);
   Text(rp, s, 18);
}

static UBYTE   HDB_path[150];


static VBS_error_t   HDB_WriteReport(operation, reportname, tree, path, datestring,
                         name, keep_log,
                         startpos, endpos,
                         tb, nf, nd)
int             operation;
UBYTE          *reportname, *path, *datestring, *name;
VBS_DirNode_t  *tree;
BOOL            keep_log;
ULONG           startpos, endpos;
long            tb, nf, nd;
{
   VBS_error_t    error = EWRITEREPORT;
   FILE          *fp = NULL;
   UBYTE          startstring[9], endstring[9];


   fp = fopen(reportname, "w");

   if(!fp)
      goto quit;
   switch(operation) {
   case BACKUP:
      if(fprintf(fp, REPORTHDRTX)==EOF)
         goto quit;

      if(fprintf(fp, BACKUPONTX " %s\n\n\n", datestring)==EOF)
         goto quit;

      if(fprintf(fp, BACKUPNAMETX " %s\n", name)==EOF)
         goto quit;

      if(fprintf(fp, BACKUPFROMTX " %s\n", path)==EOF)
         goto quit;

      if(keep_log) {
         VMG_TimeToString(startstring, startpos);
   	 if(VBSG_countertype==TIMEDCT)
   	 {
      	    VMG_TimeToString(endstring, endpos);
            if(fprintf(fp, VIDEOTAPETX "%s %s-%s.\n\n",
               VMG_logname, startstring, endstring)==EOF)
                	goto quit;
	 } else {
            if(fprintf(fp, VIDEOTAPETX "%s %s.\n\n",
               VMG_logname, startstring)==EOF)
                	goto quit;
         }
      }
      break;
   case RESTORE:
      if(fprintf(fp, RESTREPORTHDRTX)==EOF)
         goto quit;

      if(fprintf(fp, BACKUPNAMETX " %s\n", name)==EOF)
         goto quit;

      if(fprintf(fp, RESTORETOTX " %s\n\n", path)==EOF)
         goto quit;
      break;
   case VERIFY:
      if(fprintf(fp, VERREPORTHDRTX)==EOF)
         goto quit;

      if(fprintf(fp, BACKUPNAMETX " %s\n", name)==EOF)
         goto quit;

      if(fprintf(fp, VERIFYTOTX " %s\n\n", path)==EOF)
         goto quit;
      break;
   }
   if(fprintf(fp,
      TOTALDIRTX "%9ld\n" TOTALFILESTX "%9ld\n" TOTALBYTESTX "%9ld\n\n\n\n",
      nd, nf, tb)<=0)
          goto quit;


   if(fprintf(fp, LISTHDRTX)==EOF)
      goto quit;

   strcpy(HDB_path, path);

   error = HDB_ReportDirectory(fp, tree);

quit:
   if(fp)
      fclose(fp);
   return(error);
}

static VBS_error_t    HDB_ReportDirectory(fp, dnode)
FILE          *fp;
VBS_DirNode_t *dnode;
{
   VBS_error_t     error = EWRITEREPORT;
   VBS_FileNode_t *fnode;
   short           l;
   static UBYTE    datestring[20], timestring[9];
   VBS_DirNode_t  *dn2;

   if(!(dnode->Flags & ERRF_INCLUDE))
      return(EOK);
   if(fprintf(fp, DIRECTORYTX " %s %s\n", HDB_path,
      HDB_TreeErrorMsg(dnode->Flags))==EOF)
         goto quit;

   fnode = dnode->FirstFile;
   while(fnode) {
      if(fnode->Flags == ERRF_INCLUDE) {
         UIG_Date(datestring, fnode->Date.ds_Days);
         UIG_Time(timestring, fnode->Date.ds_Minute, fnode->Date.ds_Tick);

         if(fprintf(fp, "  %-30s %9ld %s %-9s %s\n",
            fnode->Name, fnode->Size, UIG_MakeProtString(fnode->Protection),
            datestring, timestring)==EOF)
                  goto quit;
      } else if(fnode->Flags & ERRF_INCLUDE) {
         if(fprintf(fp, "  %-30s %s\n",
            fnode->Name, HDB_TreeErrorMsg(fnode->Flags))==EOF)
                  goto quit;
      }
      fnode = fnode->Next;
   }
   fprintf(fp, "\n\n");
   l = strlen(HDB_path);

   if(l && HDB_path[l-1]!=':') {
      HDB_path[l++] = '/';
   }

   dn2 = dnode->FirstDir;

   error = EOK;
   while(!error && dn2) {
      strcpy(HDB_path+l, dn2->Name);
      error = HDB_ReportDirectory(fp, dn2);
      dn2 = dn2->Next;
   }
quit:
   return(error);
}

static UBYTE    *HDB_TreeErrorMsg(flags)
UBYTE     flags;
{
	static char	 errormsgstr[100];
	char		*errp;
	
	errp = errormsgstr;
	if(flags & ERRF_READERROR) {
		strcpy(errp, READERRORTX); 
		errp += strlen(errp); *errp++ = ' ';
	}
	if(flags & ERRF_VERIFYERROR) {
		strcpy(errp, VERIFYERRORTX);
		errp += strlen(errp); *errp++ = ' ';
	}
	if(flags & ERRF_FILEEXISTSERROR) {
		strcpy(errp, FILEEXISTSERRORTX);
		errp += strlen(errp); *errp++ = ' ';
	}
	if(flags & ERRF_FILENOTEXISTSERROR) {
		strcpy(errp, FILENOTEXISTSERRORTX);
		errp += strlen(errp); *errp++ = ' ';
	}
	*errp = 0;
	return errormsgstr;
}
