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

#include "vbs.h"
#include "LD_vbstext.h"

#define EXCLUDECOLOR 0
#define FILLCOLOR    1
#define INCLUDECOLOR 4
#define DIRTREECOLOR 5
#define BCOLOR       2
#define CURDIRCOLOR  6
#define BOXCOLOR     5

#define CBOXTOP      3
#define CBOXHEIGHT   26
#define MAXCOL       40
#define MAXDIRROWS   30

#define SPACEBETWEEN 16
#define SPACELEFTFILE 4
#define SPACELEFTDIR  8

#define INSPRING     16

#define COLUMNS      1
#define LONGLIST     0

enum {   BACKUP, RESTORE, VERIFY };

#include "hdb_ui.h"
#include "VD_hdb_ui.c"

#define  STARTTEXT   IText8
enum { NOTHING, RETURN_QUIT, RETURN_START };

static void             HDB_ChangeCurDir(VBS_DirEntry_t *);
static void             HDB_Cleanup(void);
static void             HDB_ColumnizeFiles(VBS_DirNode_t *);
static void             HDB_CShowFiles(VBS_DirNode_t *);
static int              HDB_HandleGadget(USHORT, ULONG, int, int);
static int              HDB_HandleMenu(int, int);
static BOOL             HDB_IncludeArch(VBS_DirNode_t *, BOOL);
static void             HDB_IncludeDirNode(VBS_DirNode_t *, BOOL);
static void             HDB_IncludeFile(VBS_FileNode_t *, BOOL);
static BOOL             HDB_IncludeFiles(VBS_DirNode_t *, BOOL);
static void             HDB_IncludeParents(VBS_DirNode_t *);
static void             HDB_IncludeTree(VBS_DirNode_t *, BOOL);
static BOOL             HDB_Init(struct Screen *, char *, int);
static void             HDB_LongListFiles(VBS_DirNode_t *);
static VBS_error_t      HDB_ReportDirectory(FILE *, VBS_DirNode_t *);
static void             HDB_SetFlags(VBS_DirNode_t *, UBYTE, UBYTE);
static int              HDB_SelectFiles(void);
static void             HDB_SelectInit(char *);
static void             HDB_ShowDirectories(VBS_DirNode_t *, int, int, int, int);
static void             HDB_ShowFile(VBS_FileNode_t *, int, int);
static void             HDB_ShowFiles(VBS_DirNode_t *, int, int);
static void             HDB_ShowStats(void);
static void             HDB_ShowTree(VBS_DirNode_t *, int, int);
static VBS_FileNode_t  *HDB_SkipFiles(VBS_FileNode_t *, int);
static BOOL             HDB_ToggleFiles(VBS_DirNode_t *);
static char            *HDB_TreeErrorMsg(UBYTE);
static VBS_error_t      HDB_WriteReport(int, char *, VBS_DirNode_t *, char *,\
                            char *, char *, BOOL, ULONG, ULONG,\
                            long, long, long);

static   long              HDB_numdirs, HDB_totalbytes, HDB_numfiles;

static   struct Screen    *HDB_screen = NULL;
static   struct Window    *HDB_window = NULL;
static   struct RastPort  *HDB_dirrp, *HDB_filerp, *HDB_winrp;

static   int               HDB_fileshowmode = COLUMNS;
static   int               HDB_diry, HDB_maxy, HDB_Wwidth;
static   UWORD             HDB_colwidth[MAXCOL];
static   VBS_FileNode_t   *HDB_filecolumns[MAXCOL];
static   VBS_DirEntry_t    HDB_direntries[MAXDIRROWS], *HDB_de, *HDB_curde;
static   VBS_DirNode_t    *HDB_tree, *HDB_curdir;

static   VBS_ListData_t    HDB_dirlist = {
   &VBS_dirgadget, &VBS_dirhoriz, &VBS_dirvert,
   &VBS_dirarrowleft, &VBS_dirarrowright,  &VBS_dirarrowup, &VBS_dirarrowdown
};
static   VBS_ListData_t    HDB_filelist = {
   &VBS_filegadget, &VBS_filehoriz, &VBS_filevert,
   &VBS_filearrowleft, &VBS_filearrowright,  &VBS_filearrowup, &VBS_filearrowdown
};
extern BOOL       VBSG_novideocheck;
extern char       VMG_logname[], VBSG_reportdrawer[];



VBS_error_t   HDBG_FSBackup(screen, path, fontname, height, keep_log)
struct Screen  *screen;
char           *path, *fontname;
int             height;
BOOL            keep_log;
{
   char            *name;
   LONG             startpos, starttime, endtime, duration;
   struct DateStamp date;
   char             datestring[20], string[80];
   char            *buffer = NULL;
   BOOL             hdbinited = FALSE, archive, timergotten = FALSE;
   BOOL             scrsetup = FALSE, backupwopened = FALSE;
   VBS_error_t      error = EOK;
   char             report[80];

   HDB_tree = NULL;
   if(!(buffer = AllocMem(MEMSIZE, 0))) {
      error = EMEMORY;
      goto quit;
   }
   if(HDB_Init(screen, fontname, height)) {
      error = EMEMORY;
      goto quit;
   }
   hdbinited = TRUE;
   SetAPen(HDB_winrp, 0);
   SetDrMd(HDB_winrp, JAM1);
   Move(HDB_winrp, 25, CBOXTOP+9);
   Text(HDB_winrp, BACKUPFROMTX, LEN(BACKUPFROMTX));
   SetAPen(HDB_winrp, 3);
   Move(HDB_winrp, 130, CBOXTOP+9);
   Text(HDB_winrp, path, strlen(path));

   HDB_Wwidth             = HDB_filelist.Wwidth;

   if(HDB_dirlist.WinRows > MAXDIRROWS)
      HDB_dirlist.WinRows = MAXDIRROWS;
   if(error = BAG_ScanDirectories(&HDB_tree, path, HDB_dirrp,
          &HDB_totalbytes, &HDB_numfiles, &HDB_numdirs,
          &HDB_dirlist,SPACELEFTDIR,INSPRING))
            goto quit;

   SRTG_SortTree(&HDB_tree);
   HDB_SelectInit(STARTBACKUPTX);
   do {
      if(HDB_SelectFiles()==QUIT)
         goto quit;
      name = NTG_AskNameTime(HDB_window, keep_log ? &startpos : NULL,
                             &archive, report);
   } while(!name);
   if(!(BAG_CheckConnection(screen, HDB_window)))
      goto quit;
   HDB_Cleanup();hdbinited = FALSE;
   BAG_SetupVideoScreen(screen);
   scrsetup = TRUE;
   VBS_WaitFire();
   VBS_GetTimer();timergotten = TRUE;
   starttime = VBS_ReadTimer();
   DateStamp(&date);
   error = BAG_WriteHeader(BT_FS, name, path, &date);
   if(error) goto quit;
   if(!(error=BAG_Backup(HDB_tree, name, path, archive, buffer, MEMSIZE))) {
      scrsetup = FALSE;
      endtime  = VBS_ReadTimer();
      duration = (endtime - starttime) & 0x00FFFFFF;
      UIG_Date(datestring, date.ds_Days);
      sprintf(string, "FS  %s "ONTX" %s", name, datestring);
      if(keep_log)
         VMG_AddLogEntry(startpos, startpos+duration, string);
      if(report[0]) {
         error = HDB_WriteReport(BACKUP, report, HDB_tree, path, datestring,
                         name,
                         keep_log, startpos, startpos+duration,
                         HDB_totalbytes, HDB_numfiles, HDB_numdirs);
      }
   }

quit:
   if(backupwopened) {
      CloseWindow(HDB_window);
      HDB_window = NULL;
   }
   if(timergotten)
      VBS_FreeTimer();
   if(scrsetup)
      BAG_RestoreVideoScreen();
   if(HDB_tree)
      BAG_FreeTree(HDB_tree);
   if(hdbinited)
      HDB_Cleanup();
   if(buffer)
      FreeMem(buffer, MEMSIZE);
   return(error);
}

VBS_error_t  HDBG_FSRestore(screen, name, fontname, height)
struct Screen  *screen;
char           *name, *fontname;
int             height;
{
   BOOL         scrsetup = FALSE, hdbinited = FALSE;
   char        *path, *fullname, *filename;
   char         backupname[64], *bnp;
   char        *buffer = NULL;
   VBS_error_t  error = EOK;

   HDB_tree = NULL;
   if(!(buffer = AllocMem(MEMSIZE, 0))) {
      error = EMEMORY;
      goto quit;
   }
   BAG_SetupScreen(screen);
   scrsetup = TRUE;
   error = BAG_Search(BT_FS, name, backupname);
   if(error)
      goto quit;

   error = BAG_RestoreTree(&HDB_tree, buffer, MEMSIZE);
   if(error)
      goto quit;

   BAG_RestoreScreen();
   scrsetup = FALSE;
   if(HDB_Init(screen, fontname, height)) {
      error = EMEMORY;
      goto quit;
   }
   hdbinited = TRUE;
   if(ERRG_Message(HDB_window, ETREEREAD)==ERROR_CANCEL)
      goto quit;
   BAG_PrepareTree(HDB_tree, HDB_winrp,
          &HDB_totalbytes, &HDB_numfiles, &HDB_numdirs,
          &HDB_dirlist, SPACELEFTDIR, INSPRING);

   HDB_Wwidth             = HDB_filelist.Wwidth;

   if(HDB_dirlist.WinRows > MAXDIRROWS)
      HDB_dirlist.WinRows = MAXDIRROWS;

   HDB_SelectInit(STARTRESTORETX);

   do {
      if(HDB_SelectFiles()==QUIT)
         goto quit;
      path  = PAG_AskPath(HDB_window, ENTERPATHRESTTX);
   } while(!path);
   HDB_Cleanup();hdbinited = FALSE;
   BAG_SetupScreen(screen); scrsetup = TRUE;

   error    = BAG_RestoreFilesTo(HDB_tree, path, buffer, MEMSIZE);
   if(error)
      goto quit;
   bnp      = backupname+strlen(backupname);
   strcpy(bnp, ".RReport");
   BAG_RestoreScreen(); scrsetup = FALSE;
   fullname = FileRequest(screen, NULL, RESTREPORTFNAMETX,
                          VBSG_reportdrawer, &filename, backupname);
   *bnp     = '\0';
   if(fullname && fullname[0])
      HDB_WriteReport(RESTORE, fullname, HDB_tree, path, NULL,
                      backupname, FALSE, 0, 0,
                      HDB_totalbytes, HDB_numfiles, HDB_numdirs);

quit:
   if(scrsetup)
      BAG_RestoreScreen();
   if(hdbinited)
      HDB_Cleanup();
   if(HDB_tree)
      BAG_FreeTree(HDB_tree);
   if(buffer)
      FreeMem(buffer, MEMSIZE);
   return(error);
}

VBS_error_t  HDBG_FSVerify(screen, name, fontname, height)
struct Screen  *screen;
char           *name, *fontname;
int             height;
{
   BOOL         scrsetup = FALSE, hdbinited = FALSE;
   char        *path, *fullname, *filename, *bnp;
   char         backupname[64];
   VBS_error_t  error = EOK;
   char        *buffer1 = NULL, *buffer2 = NULL;

   HDB_tree = NULL;
   if(!(buffer1 = AllocMem(MEMSIZE, 0)) ||
      !(buffer2 = AllocMem(MEMSIZE, 0))) {
         error = EMEMORY;
         goto quit;
   }
   BAG_SetupScreen(screen);
   scrsetup = TRUE;
   error = BAG_Search(BT_FS, name, backupname);
   if(error)
      goto quit;

   error = BAG_RestoreTree(&HDB_tree, buffer1, MEMSIZE);
   if(error)
      goto quit;

   if(HDB_Init(screen, fontname, height)) {
      error = EMEMORY;
      goto quit;
   }
   hdbinited = TRUE;
   BAG_RestoreScreen();
   scrsetup = FALSE;
   if(ERRG_Message(HDB_window, ETREEREADV)==ERROR_CANCEL)
      goto quit;
   BAG_PrepareTree(HDB_tree, HDB_winrp,
          &HDB_totalbytes, &HDB_numfiles, &HDB_numdirs,
          &HDB_dirlist, SPACELEFTDIR, INSPRING);

   HDB_Wwidth             = HDB_filelist.Wwidth;

   if(HDB_dirlist.WinRows > MAXDIRROWS)
      HDB_dirlist.WinRows = MAXDIRROWS;

   HDB_SelectInit(STARTVERIFYTX);

   do {
      if(HDB_SelectFiles()==QUIT)
         goto quit;
      path = PAG_AskPath(HDB_window, ENTERPATHVERTX);
   } while(!path);
   HDB_Cleanup();hdbinited = FALSE;
   BAG_SetupScreen(screen); scrsetup = TRUE;
   error    = BAG_VerifyFilesFrom(HDB_tree, path, buffer1, buffer2, MEMSIZE);
   if(error)
      goto quit;
   bnp      = backupname+strlen(backupname);
   strcpy(bnp, ".VReport");
   BAG_RestoreScreen(); scrsetup = FALSE;
   fullname = FileRequest(screen, NULL, RESTREPORTFNAMETX,
                          VBSG_reportdrawer, &filename, backupname);
   *bnp     = '\0';
   if(fullname && fullname[0])
      error = HDB_WriteReport(VERIFY, fullname, HDB_tree, path, NULL,
                      backupname, FALSE, 0, 0,
                      HDB_totalbytes, HDB_numfiles, HDB_numdirs);

quit:
   if(scrsetup)
      BAG_RestoreScreen();
   if(hdbinited)
      HDB_Cleanup();
   if(HDB_tree)
      BAG_FreeTree(HDB_tree);
   if(buffer1)
      FreeMem(buffer1, MEMSIZE);
   if(buffer2)
      FreeMem(buffer2, MEMSIZE);
   return(error);
}
static void HDB_SelectInit(menustring)
char     *menustring;
{
   STARTTEXT.IText = menustring;

   HDB_curdir = HDB_tree;

   HDB_ShowTree(HDB_tree, 0, 0);
   UIG_AdjustProps(HDB_window, &HDB_dirlist, 0, 0);
   HDB_CShowFiles(HDB_tree);
   HDB_ShowStats();
}
static int HDB_SelectFiles()
{

   struct IntuiMessage *message;
   struct Gadget       *g;
   USHORT               code, gid;
   ULONG                class;
   int                  mouse_x, mouse_y;
   int                  to_do;

lus:
   while(!(message = (struct IntuiMessage *)GetMsg(HDB_window->UserPort)))
      WaitPort(HDB_window->UserPort);

   code = message->Code;class = message->Class;
   g = NULL;
   if(class==GADGETDOWN || class==GADGETUP){
      g = (struct Gadget *)(message->IAddress);
      gid = g->GadgetID;
   }
   mouse_x = message->MouseX; mouse_y = message->MouseY;

   ReplyMsg((struct Message *)message);
   switch(class) {
   case GADGETUP:
   case GADGETDOWN:
      to_do = HDB_HandleGadget(gid, class, mouse_x, mouse_y);
      break;
   case MENUPICK:
      if(code != MENUNULL) {
         to_do = HDB_HandleMenu(MENUNUM(code), ITEMNUM(code));
      }
      break;
   }
   switch(to_do) {
   case RETURN_START:
      return(START);
   case RETURN_QUIT:
      return(QUIT);
   default:
      goto lus;
   }
}
static int  HDB_HandleMenu(menu, item)
int      menu, item;
{
   int     ret = NOTHING;
   BOOL    showtree;

   switch(menu) {
   case PROJECTMENU:
      switch(item) {
      case START:
         ret = RETURN_START; break;
      case QUIT:
         ret = RETURN_QUIT;  break;
      }
      break;
   case INCLEXCLMENU:
      if(item >=INCLUDEFILES && item <= INCARCH) {
         switch(item) {
         case INCLUDEFILES:
            showtree = HDB_IncludeFiles(HDB_curdir, TRUE); break;
         case EXCLUDEFILES:
            showtree = HDB_IncludeFiles(HDB_curdir, FALSE); break;
         case TOGGLEFILES:
            showtree = HDB_ToggleFiles(HDB_curdir); break;
         case EXARCH:
            showtree = HDB_IncludeArch(HDB_tree, FALSE); break;
         case INCARCH:
            showtree = HDB_IncludeArch(HDB_tree, TRUE); break;
         }
         HDB_ShowFiles(HDB_curdir, -1, -1);
         if(showtree)
            HDB_ShowTree(HDB_tree, -1, -1);
         HDB_ShowStats();
      } else {
         switch(item) {
         case SHOWMODE:
            HDB_fileshowmode = LONGLISTITEM.Flags & CHECKED ?
               LONGLIST : COLUMNS;
            HDB_CShowFiles(HDB_curdir);
            break;
         }
      }
      break;
   }
   return(ret);
 }

static int  HDB_HandleGadget(gid, class, mouse_x, mouse_y)
USHORT      gid;
ULONG       class;
int         mouse_x, mouse_y;
{
   int               leftx, toprow;
   int               x, y, row, col;
   BOOL              f;
   VBS_FileNode_t   *fnode;
   VBS_DirEntry_t   *de;
   int               xc;

   switch(class) {
   case GADGETUP:
      if(gid>=FILE_HORIZ && gid<=FILE_ARROW_DOWN) {
         UIG_NewView(HDB_window, &HDB_filelist,
                     gid-FILE_HORIZ, &leftx, &toprow);
         HDB_ShowFiles(HDB_curdir, leftx, toprow);
         break;
      }
      if(gid>=DIR_HORIZ && gid<=DIR_ARROW_DOWN) {
         UIG_NewView(HDB_window, &HDB_dirlist,
                     gid-DIR_HORIZ, &leftx, &toprow);
         HDB_ShowTree(HDB_tree, leftx, toprow);
         break;
      }
      switch(gid) {
      case DIR_WINDOW:
         x = mouse_x - VBS_dirgadget.LeftEdge;
         y = mouse_y - VBS_dirgadget.TopEdge;
         if(y<HDB_dirlist.WinHeight) {
            row = y/HDB_dirrp->TxHeight;
            if(row + HDB_dirlist.TopRow < HDB_dirlist.NumRows &&
               x >= HDB_direntries[row].x) {
               de = &HDB_direntries[row];
               if(de != HDB_curde) {
                  HDB_ChangeCurDir(&HDB_direntries[row]);
                  HDB_CShowFiles(HDB_curdir);
               } else {
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
      case FILE_WINDOW:
         x = mouse_x - VBS_filegadget.LeftEdge;
         y = mouse_y - VBS_filegadget.TopEdge;
         xc = SPACELEFTFILE-HDB_filelist.LeftX;
         for(col = 0; col < HDB_filelist.NumCols; col++ ) {
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
            if(fnode) {
               HDB_IncludeFile(fnode, (BOOL)((fnode->Flags&ERRF_INCLUDE)==0));
               HDB_ShowFile(fnode, xc,
                  row*HDB_filerp->TxHeight + HDB_filerp->TxBaseline);
               if(!(HDB_curdir->Flags & ERRF_INCLUDE)) {
                  HDB_IncludeParents(HDB_curdir);
                  HDB_ShowTree(HDB_tree, -1, -1);
               }
               HDB_ShowStats();
            }
         }
         break;
      }
   }
   return(NOTHING);
}
static void HDB_ChangeCurDir(de)
VBS_DirEntry_t    *de;
{
   SetDrMd(HDB_dirrp, JAM2);

   if(HDB_curde) {
      SetAPen(HDB_dirrp, HDB_curde->dn->Flags&ERRF_INCLUDE ?
         INCLUDECOLOR : EXCLUDECOLOR );
      SetBPen(HDB_dirrp, BCOLOR);
      Move(HDB_dirrp, HDB_curde->x, HDB_curde->y);
      Text(HDB_dirrp, HDB_curde->dn->Name, HDB_curde->dn->NameLen);
   }
   SetAPen(HDB_dirrp, de->dn->Flags&ERRF_INCLUDE ?
      INCLUDECOLOR : EXCLUDECOLOR );
   SetBPen(HDB_dirrp, CURDIRCOLOR);
   Move(HDB_dirrp, de->x, de->y);
   Text(HDB_dirrp, de->dn->Name, de->dn->NameLen);
   HDB_curde  = de;
   HDB_curdir = de->dn;
}
static void HDB_CShowFiles(dn)
VBS_DirNode_t  *dn;
{
   if(HDB_fileshowmode == COLUMNS)
      HDB_ColumnizeFiles(dn);
   else
      HDB_LongListFiles(dn);
   UIG_AdjustProps(HDB_window, &HDB_filelist, 0, 0);
   SetRast(HDB_filerp, BCOLOR);
   HDB_ShowFiles(dn, 0, 0);
}


static BOOL HDB_Init(screen, fontname, fontheight)
struct Screen     *screen;
char              *fontname;
int                fontheight;
{
   struct RastPort *rp;
   BOOL             error = TRUE;

   HDB_screen    = screen;
   HDB_window    = NULL;

   HDB_window = UIG_OpenWindow(HDB_screen, &NewWindowStructure1, FILLCOLOR);
   if(!HDB_window)
      goto quit;

   UIG_InitPad(&HDB_dirlist, HDB_window, fontname, fontheight);
   UIG_InitPad(&HDB_filelist, HDB_window, fontname, fontheight);
   rp = HDB_window->RPort;
   SetDrMd(rp, JAM2);
   SetAPen(rp, 5);
   SetOPen(rp, 3);
   RectFill(rp, VBS_dirgadget.LeftEdge, CBOXTOP,
                               VBS_filearrowup.LeftEdge+VBS_filearrowup.Width-1,
                               CBOXTOP + CBOXHEIGHT-1);
   BNDRYOFF(rp);
   SetAPen(rp, 0);
   SetBPen(rp, BOXCOLOR);
   Move(rp, 25, CBOXTOP+21);
   Text(rp, DIRECTORIESTX, LEN(DIRECTORIESTX));
   Move(rp, 270, CBOXTOP+21);
   Text(rp, FILESTX, LEN(FILESTX));
   Move(rp, 440, CBOXTOP+21);
   Text(rp, BYTESTX, LEN(BYTESTX));

   SetAPen(rp, 0);

   RefreshGadgets(HDB_window->FirstGadget, HDB_window, NULL);

   HDB_winrp  = rp;
   HDB_dirrp  = HDB_dirlist.RastPort;
   HDB_filerp = HDB_filelist.RastPort;

   SetRast(HDB_dirrp, BCOLOR);

   SetDrMd(HDB_dirrp,  JAM1);
   SetDrMd(HDB_filerp, JAM1);
   SetAPen(HDB_dirrp,  0);
   SetAPen(HDB_filerp, 0);
   SetMenuStrip(HDB_window, &Menu1);
   error = FALSE;
quit:
   return(error);
}

static void HDB_Cleanup()
{
   UIG_CleanupPad(&HDB_dirlist);
   UIG_CleanupPad(&HDB_filelist);

   if(HDB_window) {
      ClearMenuStrip(HDB_window);
      CloseWindow(HDB_window);
   }
/*   if(HDB_screen)
      CloseScreen(HDB_screen);*/

   HDB_screen    = NULL;
   HDB_window    = NULL;
}

static void HDB_ShowTree(dn, leftx, toprow)
VBS_DirNode_t  *dn;
int             leftx, toprow;
{
   int   y;

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
                0, 0,
                HDB_dirlist.WinWidth-1, HDB_dirlist.WinHeight-1);
   HDB_curde = NULL;
   HDB_ShowDirectories(dn, SPACELEFTDIR-leftx, SPACEBETWEEN, -leftx, HDB_diry);
   SetAPen(HDB_dirrp, BCOLOR);
   y = HDB_dirlist.WinHeight;
   if(y!=HDB_dirlist.PadGadget->Height) {
      RectFill(HDB_dirrp, 0, y, HDB_dirlist.WinWidth-1,
               HDB_dirlist.PadGadget->Height-1);
   }
   HDB_dirlist.LeftX  = leftx;
   HDB_dirlist.TopRow = toprow;
}

static void HDB_ShowDirectories(dn, xoffset, xdelt, xp, yp)
VBS_DirNode_t     *dn;
int                xoffset, xdelt;
int                xp, yp;
{
   int      y;

   while(dn) {
      SetAPen(HDB_dirrp, DIRTREECOLOR);
      y = HDB_diry+(HDB_dirrp->TxHeight>>1);
      RectFill(HDB_dirrp, xp, yp, xp+3, y);
      RectFill(HDB_dirrp, xp, y-1, xoffset-2,y);
      if(HDB_diry >=0 && HDB_diry < HDB_maxy) {
         if(HDB_curdir == dn) {
            SetBPen(HDB_dirrp, CURDIRCOLOR);
            SetDrMd(HDB_dirrp, JAM2);
            HDB_curde = HDB_de;
         }
         SetAPen(HDB_dirrp, dn->Flags&ERRF_INCLUDE ?
            INCLUDECOLOR : EXCLUDECOLOR);
         y = HDB_diry;
         Move(HDB_dirrp, xoffset, HDB_diry+HDB_dirrp->TxBaseline);
         HDB_de->dn = dn;
         HDB_de->x  = HDB_dirrp->cp_x;
         HDB_de->y  = HDB_dirrp->cp_y;
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

   winrows = HDB_filelist.WinRows;
   SetBPen(HDB_filerp, BCOLOR);
   if(leftx  < 0) leftx  = HDB_filelist.LeftX;
   if(toprow < 0) toprow = HDB_filelist.TopRow;
   ScrollRaster(HDB_filerp,
                leftx - HDB_filelist.LeftX,
                (toprow - HDB_filelist.TopRow)*HDB_filerp->TxHeight,
                0, 0,
                HDB_filelist.WinWidth-1, HDB_filelist.WinHeight-1);

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
}
static void HDB_ShowFile(fnode, x, y)
VBS_FileNode_t    *fnode;
int                x, y;
{
   int                cx;
   int                l;
   static char        string[12];

   Move(HDB_filerp, x, y);
   SetAPen(HDB_filerp, fnode->Flags & ERRF_INCLUDE ?
               INCLUDECOLOR : EXCLUDECOLOR);
   Text(HDB_filerp, fnode->Name, fnode->NameLen);

   if(HDB_fileshowmode == LONGLIST) {

      cx = x+HDB_filelist.MaxEltWidth+HDB_Wwidth*8;
      l = stci_d(string, fnode->Size);
      Move(HDB_filerp, cx- TextLength(HDB_filerp, string, l), y);
      Text(HDB_filerp, string, l);
      cx += HDB_Wwidth;
      Move(HDB_filerp, cx, y);

      Text(HDB_filerp, UIG_MakeProtString(fnode->Protection), 8);
      cx += HDB_Wwidth*9;
      Move(HDB_filerp, cx, y);
      l = UIG_Date(string, fnode->Date.ds_Days);
      Text(HDB_filerp, string, l);
      cx += HDB_Wwidth*(l+1);
      Move(HDB_filerp, cx, y);
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
   char     s[16];

   SetAPen(HDB_winrp, 3);
   SetBPen(HDB_winrp, BOXCOLOR);
   SetDrMd(HDB_winrp, JAM2);

   sprintf(s, "%4ld", HDB_numdirs);
   Move(HDB_winrp, 25+4+8*LEN(DIRECTORIESTX), CBOXTOP+21);
   Text(HDB_winrp, s, 4);

   sprintf(s, "%6ld", HDB_numfiles);
   Move(HDB_winrp, 270+4+8*LEN(FILESTX), CBOXTOP+21);
   Text(HDB_winrp, s, 6);

   sprintf(s, "%12ld", HDB_totalbytes);
   Move(HDB_winrp, 440+4+8*LEN(BYTESTX), CBOXTOP+21);
   Text(HDB_winrp, s, 12);
}

static char    HDB_path[150];


static VBS_error_t   HDB_WriteReport(operation, reportname, tree, path, datestring,
                         name, keep_log,
                         startpos, endpos,
                         tb, nf, nd)
int             operation;
char           *reportname, *path, *datestring, *name;
VBS_DirNode_t  *tree;
BOOL            keep_log;
ULONG           startpos, endpos;
long            tb, nf, nd;
{
   VBS_error_t    error = EWRITEREPORT;
   FILE          *fp = NULL;
   char           startstring[9], endstring[9];
 

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
         VMG_TimeToString(endstring, endpos);
         if(fprintf(fp, VIDEOTAPETX "%s %s-%s.\n\n",
             VMG_logname, startstring, endstring)==EOF)
                goto quit;
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
   static char     datestring[20], timestring[9];
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
static char     *HDB_TreeErrorMsg(flags)
UBYTE     flags;
{
   switch(flags) {
      case ERRF_INCLUDE|ERRF_READERROR:
         return(READERRORTX);
      case ERRF_INCLUDE|ERRF_VERIFYERROR:
         return(VERIFYERRORTX);
      case ERRF_INCLUDE|ERRF_FILEEXISTSERROR:
         return(FILEEXISTSERRORTX);
      case ERRF_INCLUDE|ERRF_FILENOTEXISTSERROR:
         return(FILENOTEXISTSERRORTX);
   }
   return("");
}

