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

#include  <stdio.h>
#include  <devices/input.h>
#include  <graphics/displayinfo.h>
#include  <hardware/custom.h>
#include  "hardware/dmabits.h"
#include  "vbs.h"
#include  "LD_vbstext.h"
#include  "rs.h"
#include  "datacomp.h"
#include  "simpleview.h"

#define LINESATTOP	 3
extern struct ExecBase  *SysBase;
extern struct Custom     custom;

typedef enum {
	BA_BACKUP, BA_RESTORE, BA_VERIFY, BA_RESTORETREE
}	BA_Reason_t;
	
static VBS_error_t	 BA_AllocBuffers(BA_Reason_t);
static VBS_error_t	 BA_GetFileData(VBS_DirNode_t *, BOOL);
static void 		 BA_FreeBuffers(void);
static VBS_error_t	 BA_Mem2Full(void);
static int               BA_NumDirsIncluded(VBS_DirNode_t *);
static int               BA_NumFilesIncluded(VBS_FileNode_t *);
static VBS_error_t       BA_BackupFiles(VBS_DirNode_t  *);
static void              BA_BackupTree(VBS_DirNode_t  *);
static void              BA_PrepareDirectory(VBS_DirNode_t *,\
                            struct RastPort *, VBS_ListData_t *, int, int);
static VBS_error_t       BA_RestoreDirectory(VBS_DirNode_t **,
                            VBS_DirNode_t *, int);
static void              BA_PrintMsgPathFile(UBYTE *);
static VBS_error_t       BA_RestoreFiles(VBS_DirNode_t *, BOOL);
static void		 BA_RestWindow(char, char *);
static VBS_error_t       BA_ScanDirectory(VBS_DirNode_t *, UBYTE *,
                    struct RastPort *, VBS_ListData_t *, int, int);
static void              BA_SetDateProt(BPTR, UBYTE *, UBYTE,
                           struct DateStamp *, ULONG);
static BOOL              BA_SetFileDateInit(UBYTE *);
static void              BA_SetFileDateCleanup(void);
static long              BA_Verify(BPTR, UBYTE *, long);
static struct InputEvent *BA_eraseinput(void);
#define HBMWIDTH     640
#define HBMHEIGHT    150
#define DELAYPERFILE 8     /*(TICKS)*/
#define ROOTNAME     "/"


static ULONG		      VWG_DataOffset, VWG_DataLength;
static struct FileInfoBlock  *BA_fib;
static UBYTE                 *BA_mem, *BA_mem2, *BA_cmpbuf, *BA_decrptr;
static long                   BA_memsize, BA_memsize2;
static UBYTE                 *BA_memptr, *BA_memlimit;
static UBYTE		     *BA_check;
static	long 		      BA_chunknr, BA_DataOffset, BA_DataLength;
static  long 		      BA_memoff, BA_memoff2;
static  long		      BA_llduration;

static long                   BA_nopen, BA_nerr, BA_filedirdelay;
static long                   BA_duration, BA_starttime, BA_wantedduration;

static UBYTE                  BA_msg[150], BA_path[150];

static BOOL                   BA_archive, BA_verifying;
static struct StandardPacket *BA_packet     = NULL, BA_packetst;
static UBYTE                 *BA_bstrpt     = NULL;
static struct MsgPort        *BA_deviceproc = NULL;
static struct Process        *BA_tsk;
static BPTR		      BA_tmplock;
static BOOL		      BA_ok;

USHORT            	      BAG_rdisplayflg = 0;
struct View                  *BAG_oldviewptr;


static SimpleView	      BA_sv0, BA_sv1, BA_hrsv;

static struct Layer_Info     *BA_li = NULL;
static struct Layer          *BA_layer = NULL;
static struct RastPort       *BA_vhrp;

static struct Screen         *BA_as;
static struct Window         *BA_aw;

extern UWORD  VBS_empty_pointer[];

extern int                    VBSG_writefactor, VBSG_perfile, VBSG_perdir;
extern BOOL                   VBSG_novideocheck, VBSG_readdisplayblank;
extern UBYTE		      VBSG_extradelay;
extern BOOL		      VBSG_is68000;
extern UBYTE                  VBSG_errorinfo[];
UBYTE				BA_type;
static	long BA_chunknr, BA_DataOffset, BA_DataLength;
static  long BA_memoff, BA_memoff2;

static VBS_error_t	BA_ReadInit(void);
static VBS_error_t	BA_WriteInit(void);
static VBS_error_t	rb(void);
static VBS_error_t	wb(void);

#define clock() VBS_ReadTimer()
#define MAXDIRSIZE 4000000


static long  BA_tb, BA_nf, BA_nd, BA_realnd;


VBS_error_t    BAG_ScanDirectories(dnp, path, rp, tb, nf, nd, fl, offset, delt)
VBS_DirNode_t  **dnp;
UBYTE           *path;
struct RastPort *rp;
long            *tb, *nf, *nd;
VBS_ListData_t  *fl;/*Past aan: NumEntries, MaxTotalWidth, Cols, Rows */
int              offset, delt;
{
   BPTR           lock = NULL;
   int            l, tl;
   VBS_error_t    error = EOK;

   fl->NumEntries = 0;
   fl->MaxTotalWidth = 0;
   BA_fib = (struct FileInfoBlock *)AllocMem(sizeof(struct FileInfoBlock),
                                              MEMF_CLEAR);
   if(!BA_fib) {
      error = EMEMORY;
      goto quit;
   }

   lock = Lock(path, ACCESS_READ);
   if(!lock) {
      error = EREAD;
      strcpy(VBSG_errorinfo, path);
      goto quit;
   }

   Examine(lock, BA_fib);
   UnLock(lock);
   if(BA_fib->fib_EntryType <= 0) {
      error = EREAD;
      strcpy(VBSG_errorinfo, BA_fib->fib_FileName);
      goto quit;
   }

   l = strlen(path);
   if(!(*dnp = (VBS_DirNode_t *)AllocMem(sizeof(VBS_DirNode_t)+l, MEMF_CLEAR))) {
      error = EMEMORY;
      goto quit;
   }
   tl = TextLength(rp, path, l)+offset;

   fl->MaxTotalWidth = tl;
   fl->NumEntries    = 1;

   (*dnp)->Flags      = ERRF_INCLUDE;
   (*dnp)->ParentDir  = NULL;
   (*dnp)->Protection = BA_fib->fib_Protection;
   (*dnp)->Date       = BA_fib->fib_Date;
   (*dnp)->NameLen    = l;
   strcpy((*dnp)->Name, path);

   BA_tb = 0; BA_nd = 1; BA_nf = 0;

   error = BA_ScanDirectory(*dnp, path, rp, fl, offset+delt, delt);
   fl->NumCols = 1;
   fl->NumRows = fl->NumEntries;
   *tb = BA_tb; *nd = BA_nd; *nf = BA_nf;
quit:
   if(BA_fib)
      FreeMem((UBYTE *)BA_fib, sizeof(struct FileInfoBlock));
   return(error);
}

static VBS_error_t BA_ScanDirectory(dn, dirname, rp, fl, offset, delt)
VBS_DirNode_t   *dn;
UBYTE           *dirname;
struct RastPort *rp;
VBS_ListData_t  *fl;
int              offset, delt;
{
   BPTR             lock, oldlock;
   VBS_DirNode_t   *dnode, *prevdnode = NULL;
   VBS_FileNode_t  *fnode, *prevfnode = NULL;
   int              l, tl, clen, memlen;
   VBS_error_t      error = EOK;

   lock = Lock(dirname, ACCESS_READ);

   if(!lock || !Examine(lock, BA_fib) || BA_fib->fib_EntryType <= 0) {
      error = EREAD;
      strcpy(VBSG_errorinfo, dirname);
      goto quit;
   }

   while(ExNext(lock, BA_fib)) {
      l    = strlen(BA_fib->fib_FileName);
      clen = strlen(BA_fib->fib_Comment);
      tl   = TextLength(rp, BA_fib->fib_FileName, l);
      memlen = clen?l+clen+1:l;
      if(BA_fib->fib_EntryType < 0) {
      /* file */
         fnode = (VBS_FileNode_t *)AllocMem(sizeof(VBS_FileNode_t)+memlen, MEMF_CLEAR);
         if(!fnode) {
            error = EMEMORY;
            goto quit;
         }
         if(!prevfnode) {
            dn->FirstFile = fnode;
         } else {
            prevfnode->Next = fnode;
         }
         prevfnode = fnode;

         fnode->Flags      = 0;
	
         if(!(BA_fib->fib_Protection & 8)) {
         	fnode->Flags      = ERRF_INCLUDE;
	        BA_tb+=BA_fib->fib_Size;
        	BA_nf++;
         }
         fnode->TextLength = tl;
         fnode->Size       = BA_fib->fib_Size;
         fnode->Protection = BA_fib->fib_Protection;
         fnode->Date       = BA_fib->fib_Date;
         fnode->NameLen    = l;
         strcpy(fnode->Name, BA_fib->fib_FileName);
         if(clen) {
         	fnode->Flags |= ERRF_COMMENT;
         	strcpy(fnode->Name+l+1, BA_fib->fib_Comment);
         }
      } else {
      /* directory */
         dnode = (VBS_DirNode_t *)AllocMem(sizeof(VBS_DirNode_t)+memlen, MEMF_CLEAR);
         if(!dnode) {
            error = EMEMORY;
            goto quit;
         }
         fl->NumEntries++;
         if(!prevdnode) {
            dn->FirstDir = dnode;
         } else {
            prevdnode->Next = dnode;
         }
         prevdnode = dnode;

         dnode->Flags      = ERRF_INCLUDE;
         dnode->ParentDir  = dn;
         dnode->TextLength = tl;
         dnode->Protection = BA_fib->fib_Protection;
         dnode->Date       = BA_fib->fib_Date;
         dnode->NameLen    = l;
         strcpy(dnode->Name, BA_fib->fib_FileName);
         if(clen) {
         	dnode->Flags |= ERRF_COMMENT;
         	strcpy(dnode->Name+l+1, BA_fib->fib_Comment);
         }

         tl+=offset;
         if(tl > fl->MaxTotalWidth)
            fl->MaxTotalWidth = tl;
         BA_nd++;
      }
   }

   oldlock = CurrentDir(lock);
   dnode = dn->FirstDir;
   while(dnode && !error) {
      error = BA_ScanDirectory(dnode, dnode->Name, rp, fl,
                               offset+delt, delt);
      dnode = dnode->Next;
   }
   CurrentDir(oldlock);

quit:
   if(lock)
      UnLock(lock);

   return(error);
}
/*

void BAG_PrintTree(dn)
VBS_DirNode_t     *dn;
{
   VBS_DirNode_t     *childn;
   VBS_FileNode_t    *fn;

   printf("*Directory* %s\n", dn->Name);
   fn = dn->FirstFile;
   while(fn) {
      printf("---%s\n", fn->Name);
      fn = fn->Next;
   }
   childn = dn->FirstDir;

   while(childn) {
      BAG_PrintTree(childn);
      childn = childn->Next;
   }
}
*/
void BAG_FreeTree(dn)
VBS_DirNode_t  *dn;
{
   VBS_DirNode_t  *dn2;
   VBS_FileNode_t *fn, *fn2;
   int		   memlen;

   while(dn) {
      fn = dn->FirstFile;
      while(fn) {
         fn2 = fn->Next;
         memlen = sizeof(VBS_FileNode_t)+fn->NameLen;
         if(fn->Flags & ERRF_COMMENT) {
         	memlen += strlen(fn->Name+fn->NameLen+1)+1;
         }
         FreeMem((UBYTE *)fn, memlen);
         fn = fn2;
      }
      BAG_FreeTree(dn->FirstDir);
      dn2 = dn->Next;
      memlen = sizeof(VBS_DirNode_t)+dn->NameLen;
      if(dn->Flags & ERRF_COMMENT) {
         memlen += strlen(dn->Name+dn->NameLen+1)+1;
      }
      FreeMem((UBYTE *)dn, memlen);
      dn  = dn2;
   }
}

static void BA_BackupTree(dn)
VBS_DirNode_t  *dn;
{
   VBS_FileNode_t *fn;
   VBS_DirNode_t  *dn2;
   int             l;

   while(dn) {
      if(!(dn->Flags & ERRF_INCLUDE))
         goto nextdir;
      l = sizeof(VBS_DirNode_t)+dn->NameLen;
      if(dn->Flags & ERRF_COMMENT) {
         l += strlen(dn->Name+dn->NameLen+1)+1;
      }
      
      if(BA_memptr + l >= BA_mem + BA_memsize) {
         *BA_memptr = 0x00;
         BA_DataLength = BA_memsize; 
         wb();
         Delay(BA_nopen/10); BA_nopen = 0;
         BA_memptr = BA_mem;
      }
      fn  = dn->FirstFile;
      dn2 = dn->FirstDir;
      dn->FirstDir  = (VBS_DirNode_t  *)BA_NumDirsIncluded(dn2);
      dn->FirstFile = (VBS_FileNode_t *)BA_NumFilesIncluded(fn);

      memcpy(BA_memptr, (UBYTE *)dn, l); BA_nopen++;
      dn->FirstDir  = dn2;
      dn->FirstFile = fn;
      *BA_memptr = l;
      BA_memptr += l;
      while(fn) {
         if(!(fn->Flags & ERRF_INCLUDE))
            goto nextfile;
            
         l = sizeof(VBS_FileNode_t)+fn->NameLen;
      	 if(fn->Flags & ERRF_COMMENT) {
            l += strlen(fn->Name+fn->NameLen+1)+1;
         }
         
         if(BA_memptr + l >= BA_mem + BA_memsize) {
            *BA_memptr = 0x00;
            BA_DataLength = BA_memsize; 
            wb();
             
            Delay(BA_nopen/10); BA_nopen = 0;
            BA_memptr = BA_mem;
         }
         memcpy(BA_memptr, (UBYTE *)fn, l); BA_nopen++;
         *BA_memptr = l;
         BA_memptr += l;
nextfile:
         fn = fn->Next;
      }
      BA_BackupTree(dn->FirstDir);
nextdir:
      dn = dn->Next;
   }
}

VBS_error_t   BAG_RestoreTree(type, dnp)
char		type;
VBS_DirNode_t **dnp;
{
   VBS_error_t    error = EOK;

   BA_type = type;
   if(error = BA_AllocBuffers(BA_RESTORETREE))
   	goto quit;	
   BA_memptr  = BA_mem;

   BAG_PrintMsg(READINGTREETX);
   if(error = BA_ReadInit())
	goto quit;

   if(error = rb())
   	goto quit;
   if(BA_type == BT_FS) {
	   BA_memlimit = BA_mem+VBS_FindMemLimit();
   } else {
   	   BA_memlimit = BA_mem+BA_memsize;
   }
   if(BA_mem >= BA_memlimit) {
      error = EREAD;
      strcpy(VBSG_errorinfo, "\0");
      goto quit;
   }
   error = BA_RestoreDirectory(dnp, NULL, 1);
quit:
   BA_FreeBuffers();
   return(error);
}
/* vooraf errval en memptr !*/
static VBS_error_t   BA_RestoreDirectory(dnpp, parent, num)
VBS_DirNode_t **     dnpp;
VBS_DirNode_t       *parent;
int                  num;
{
   VBS_DirNode_t  *dnp = NULL, *dn, *dnfirst = NULL;
   VBS_FileNode_t *fnp = NULL, *fn;
   int             l;
   VBS_error_t     error = EOK;
   int             dir, file, numfiles;

   for(dir = 0; dir<num; dir++) {
      l = *BA_memptr;
      if(!l) {
         BA_DataOffset += BA_memsize;
         error = rb();
         if(error)
         	break;
	 if(BA_type == BT_FS) {
	   	BA_memlimit = BA_mem+VBS_FindMemLimit();
   	 } else {
   	   	BA_memlimit = BA_mem+BA_memsize;
   	 }
         	
         if(BA_mem >= BA_memlimit) {
            error = EREAD;
            strcpy(VBSG_errorinfo, "\0");
            break;
         }
         BA_memptr = BA_mem; l = *BA_memptr;
      }
      if(BA_memptr+l >= BA_memlimit) {
         error = EREAD;
         strcpy(VBSG_errorinfo, "\0");
         break;
      }

      dn = AllocMem(l, 0);
      if(dnp)
         dnp->Next = dn;
      else
         dnfirst   = dn;
      if(!dn) {
         error = EMEMORY;
         break;
      }
/*      *BA_memptr = 0;          */
      memcpy((UBYTE *)dn, BA_memptr, l);  BA_memptr+=l;
      dn->Next      = NULL;
      dn->ParentDir = parent;
      numfiles = (int)dn->FirstFile;
      fnp = NULL;
      for(file = 0; file<numfiles; file++) {
            l = *BA_memptr;
            if(!l) {
	       	BA_DataOffset += BA_memsize;
               	error = rb();

               	if(error)
               		goto error;
	 	if(BA_type == BT_FS) {
	   		BA_memlimit = BA_mem+VBS_FindMemLimit();
   	 	} else {
   	   		BA_memlimit = BA_mem+BA_memsize;
   	 	}
               		
               	if(BA_mem >= BA_memlimit) {
                  	error = EREAD;
                  	strcpy(VBSG_errorinfo, "\0");
                  	goto error;
               	}
               	BA_memptr = BA_mem; l = *BA_memptr;
            }
            if(BA_memptr+l >= BA_memlimit) {
               error = EREAD;
               strcpy(VBSG_errorinfo, "\0");
               goto error;
            }

            fn = AllocMem(l, 0);
            if(fnp)
               fnp->Next     = fn;
            else
               dn->FirstFile = fn;
            if(!dn) {
               error = EMEMORY;
               goto error;
            }
          /*  *BA_memptr = 0; */
            memcpy((UBYTE *)fn, BA_memptr, l);  BA_memptr+=l;
            fn->Next = NULL;
            fnp = fn;
      }
      error = BA_RestoreDirectory(&dn->FirstDir, dn, (int)dn->FirstDir);
      if(error)
         break;

      dnp = dn;
   }
error:
   *dnpp = dnfirst;
   return(error);
}


void   BAG_PrepareTree(dn, rp, tb, nf, nd, dl, offset, delt)
VBS_DirNode_t   *dn;
struct RastPort *rp;
long            *tb, *nf, *nd;
VBS_ListData_t  *dl;/*Past aan: NumEntries, MaxTotalWidth, Cols, Rows */
int              offset, delt;
{
   dl->NumEntries = 0;
   dl->MaxTotalWidth = 0;
   BA_tb = 0; BA_nd = 0; BA_nf = 0; BA_realnd = 0;
   BA_PrepareDirectory(dn, rp, dl, offset, delt);
   dl->NumCols = 1;
   dl->NumRows = dl->NumEntries = BA_realnd;
   *tb = BA_tb; *nd = BA_nd; *nf = BA_nf;
}
/* vooraf errval en memptr !*/
static void      BA_PrepareDirectory(dn, rp, dl, offset, delt)
VBS_DirNode_t       *dn;
struct RastPort     *rp;
VBS_ListData_t      *dl;
int                  offset, delt;
{
   VBS_FileNode_t *fnode;
   int             tl;

   while(dn) {
      dn->TextLength = TextLength(rp, dn->Name, dn->NameLen);
      dn->Flags      &= ~ERRF_ERRMASK;
      tl = dn->TextLength+offset;
      if(tl > dl->MaxTotalWidth)
         dl->MaxTotalWidth = tl;
      if(dn->Flags & ERRF_INCLUDE)   
	      BA_nd++;
      BA_realnd++;	      
      fnode = dn->FirstFile;
      while(fnode) {
            fnode->TextLength  = TextLength(rp, fnode->Name, fnode->NameLen);
	    fnode->Flags      &= ~ERRF_ERRMASK;
            if(fnode->Flags & ERRF_INCLUDE) {
	            BA_tb+= fnode->Size;
        	    BA_nf++;
       	    }
            fnode = fnode->Next;
      }
      BA_PrepareDirectory(dn->FirstDir, rp, dl, offset+delt, delt);
      dn = dn->Next;
   }
}

static struct TextFont  *BA_largefont = NULL, *BA_smallfont = NULL,
                        *BA_textfont = NULL;

extern struct TextAttr   VBSG_large_ta, VBSG_small_ta, VBSG_text_ta;

VBS_error_t BAG_WriteHeader(type, name, path, date)
UBYTE              type;
UBYTE             *name, *path;
struct DateStamp  *date;
{
   VBS_error_t       error = EMEMORY;
   UBYTE             headerblock[64], datestring[20];
   struct RastPort  *rp;
   struct DateStamp *ds;
   static UWORD	     colors[] = { 0x000, 0xFFF };
   
   BA_type = type;
   rp = BA_vhrp;
   SetRast(rp, 0); SetFont(rp, BA_largefont);

   Move(rp, 40, rp->TxHeight); Text(rp, name, strlen(name));

   SetFont(rp, BA_smallfont); Move(rp, 40, rp->cp_y+rp->TxHeight+16);
   ds = (struct DateStamp *)(headerblock+(64-sizeof(struct DateStamp)));
   *ds = *date;
   switch(type) {
   case BT_AMIGADOS:
#ifndef FISH
      Text(rp, CREATIONDATETX, LEN(CREATIONDATETX));
#endif
      break;
   case BT_FS:
      Text(rp, BACKUPDATETX, LEN(BACKUPDATETX));
      break;
   }


#ifndef FISH
   Text(rp, datestring, UIG_Date(datestring, date->ds_Days));
#endif
   if(path) {
      Move(rp, 40, rp->cp_y+rp->TxHeight);
      Text(rp, FROMTX, LEN(FROMTX));
      Text(rp, path, strlen(path));
   }
   Rect(rp, 0, 0, HBMWIDTH-1, HBMHEIGHT-1, 1);
   Rect(rp, 2, 2, HBMWIDTH-5, HBMHEIGHT-5, 1);
#ifdef FISH
   Rect(rp, 4, 4, HBMWIDTH-9, HBMHEIGHT-9, 1);
#endif

   ScreenToFront(BA_as); ActivateWindow(BA_aw);
   LoadRGB4(&BA_hrsv.ViewPort, &colors[0], 2);
   Delay(2);
   LoadView(&BA_hrsv.View);
   
   Delay(TICKS_PER_SECOND*4);
   memset(headerblock, 0, sizeof headerblock);
   headerblock[0] = type; strcpy(&headerblock[1], name);

   ScreenToFront(BA_as); ActivateWindow(BA_aw);
   Delay(2);
   WaitTOF();
   Forbid();
   VBS_WriteBackupHeader(headerblock);
   Permit();
   Delay(TICKS_PER_SECOND/2);
   
   error =  EOK;
quit:
   return(error);
}


#define  COP1LCH  0xDFF080
#define  COP1LCL  0xDFF082
#define  COPJMP1  0xDFF088
#define  COP2LCH  0xDFF084
#define  COP2LCL  0xDFF086
#define  COPJMP2  0xDFF08A
#define  COLOR0   0xDFF180

VBS_error_t BAG_Init(void)
{
   VBS_error_t        error = EMEMORY;
   int                o, bpr;
   static UWORD	      colortable[] = {0x000, 0xFFF};
   
   BA_largefont = NULL; BA_smallfont = NULL; BA_textfont = NULL;

   if(!(BA_largefont = UIG_OpenFont(&VBSG_large_ta)))
      BA_largefont = UIG_OpenDefaultFont();

   if(!(BA_smallfont = UIG_OpenFont(&VBSG_small_ta)))
      BA_smallfont = UIG_OpenDefaultFont();

   if(!(BA_textfont = UIG_OpenFont(&VBSG_text_ta)))
      BA_textfont = UIG_OpenDefaultFont();

   if(SVG_CreateSimpleView(&BA_sv0, HSTART, VSTART-LINESATTOP, 
   	VIDEOWIDTH, VIDEOHEIGHT+LINESATTOP, 1, 0))
   		goto quit;
   if(SVG_CreateSimpleView(&BA_sv1, HSTART, VSTART-LINESATTOP, 
   	VIDEOWIDTH, VIDEOHEIGHT+LINESATTOP, 1, 0))
   		goto quit;
   if(SVG_CreateSimpleView(&BA_hrsv, 107+4+8, 60,
	HBMWIDTH, HBMHEIGHT, 1, HIRES))
   		goto quit;
   LoadRGB4(&BA_sv0.ViewPort,  colortable, 2);
   LoadRGB4(&BA_sv1.ViewPort,  colortable, 2);
   LoadRGB4(&BA_hrsv.ViewPort, colortable, 2);

   memset(BA_sv0.BitMap.Planes[0], 0, RASSIZE(VIDEOWIDTH, (VIDEOHEIGHT+LINESATTOP)));
   memset(BA_sv1.BitMap.Planes[0], 0, RASSIZE(VIDEOWIDTH, (VIDEOHEIGHT+LINESATTOP)));
   bpr = BA_sv0.BitMap.BytesPerRow;
   for(o = 0; o<(bpr*LINESATTOP); o+=bpr) {
      memset(BA_sv0.BitMap.Planes[0]+o, 0xFF, 13);
      memset(BA_sv1.BitMap.Planes[0]+o, 0xFF, 13);
   }

   BA_li = NewLayerInfo();
   if(!BA_li)
      goto quit;
   if(!(BA_layer = CreateUpfrontLayer(BA_li, &BA_hrsv.BitMap,
      0, 0, HBMWIDTH-1, HBMHEIGHT-1,
      LAYERSIMPLE,NULL)))   /*layersimple*/
         goto quit;
   BA_vhrp = BA_layer->rp;
   VBS_VIInit(BA_sv0.BitMap.Planes[0]+bpr*LINESATTOP, 
   	      BA_sv1.BitMap.Planes[0]+bpr*LINESATTOP,
   	      &BA_sv0.View, &BA_sv1.View);
   error = EOK;
quit:
   if(error) {
   	SVG_DestroySimpleView(&BA_sv0);
   	SVG_DestroySimpleView(&BA_sv1);
   	SVG_DestroySimpleView(&BA_hrsv);
   }
   return(error);
}
void  BAG_Cleanup()
{
   if(BA_largefont) {
      CloseFont(BA_largefont);
   }
   if(BA_smallfont) {
      CloseFont(BA_smallfont);
   }
   if(BA_textfont)
      CloseFont(BA_textfont);
   if(BA_layer) {
      DeleteLayer(BA_li, BA_layer);
      DisposeLayerInfo(BA_li);
   }
   BA_layer = 0; BA_li = 0;
   SVG_DestroySimpleView(&BA_sv0);
   SVG_DestroySimpleView(&BA_sv1);
   SVG_DestroySimpleView(&BA_hrsv);
}

struct MsgPort    *BA_inputport;
struct IOStdReq   *BA_irb;
struct Interrupt   BA_handler;

static struct InputEvent *BA_eraseinput()
{
   return(NULL);
}
#define BACKUPSCRW   320
#define BACKUPSCRH   100
 
static struct NewScreen BA_backupns = {
	0, 0, BACKUPSCRW, BACKUPSCRH, 1,
	0, 0,
	0,
	CUSTOMSCREEN
};

static struct NewWindow BA_backupnw = {
   0, 0, BACKUPSCRW, BACKUPSCRH, 1,0,0,
   ACTIVATE|SIMPLE_REFRESH|BORDERLESS|RMBTRAP|NOCAREREFRESH,
   NULL, NULL, NULL, NULL, NULL,
   0,0,0,0,CUSTOMSCREEN
};
extern UWORD WaitPointer[];
extern int VBS_hz, VBS_sd;
void  BAG_SetupVideoScreen(screen)
struct Screen *screen;
{
   struct Screen *s;
   int		  o;
      
   s = OpenScreen(&BA_backupns);
   WaitTOF();
   Delay(2);
	
   
   if(s) {
      BA_backupnw.Screen = s;
      BA_backupnw.Width  = s->Width;
      BA_backupnw.Height = s->Height;

      if(!(BA_aw = OpenWindow(&BA_backupnw)))
         goto error;

      if(!VBS_hz)
      	SetPointer(BA_aw, &VBS_empty_pointer[0], 16, 16, 0, 0);
      WaitTOF();
      Delay(2);
      WaitTOF();

      if(!VBS_sd)
	      OFF_SPRITE;


      BA_as = s;
   }
   BAG_oldviewptr = GfxBase->ActiView;

   memset(BA_sv0.BitMap.Planes[0], 0, RASSIZE(VIDEOWIDTH, (VIDEOHEIGHT+LINESATTOP)));
   memset(BA_sv1.BitMap.Planes[0], 0, RASSIZE(VIDEOWIDTH, (VIDEOHEIGHT+LINESATTOP)));
   for(o = 0; o<(44*LINESATTOP); o+=44) {
      memset(BA_sv0.BitMap.Planes[0]+o, 0xFF, 13);
      memset(BA_sv1.BitMap.Planes[0]+o, 0xFF, 13);
   }
error:
   return;   
}


/*   BA_irb = NULL;
   if(!(BA_inputport         = CreatePort(0, 0)))
      goto error;
   if(!(BA_irb               = CreateStdIO(BA_inputport)))
      goto error;
   memset((UBYTE *)&BA_handler, 0, sizeof(BA_handler));
   BA_handler.is_Code        = BA_eraseinput;
   BA_handler.is_Node.ln_Pri = 101;
   if(OpenDevice("input.device", 0, (struct IORequest *)BA_irb, 0))
      goto error;
   BA_irb->io_Command = IND_ADDHANDLER;
   BA_irb->io_Data    = (APTR)&BA_handler;
   DoIO((struct IORequest *)BA_irb);*/
/*   if(BA_irb)
      DeleteStdIO(BA_irb);
   if(BA_inputport)
      DeletePort(BA_inputport);
   BA_irb = NULL; BA_inputport = NULL;*/
   
   
   
void  BAG_RestoreVideoScreen()
{
   LoadView(BAG_oldviewptr);
   WaitTOF();
   Delay(2);    WaitTOF();
   if(BA_aw)
      	CloseWindow(BA_aw);
      	
   BA_aw = NULL;
   if(BA_as) {
   	WaitTOF();
   	ON_SPRITE;
   	CloseScreen(BA_as);
   }
   BA_as = NULL;
}


/*      BA_irb->io_Command = IND_REMHANDLER;
      BA_irb->io_Data    = (APTR)&BA_handler;
      DoIO((struct IORequest *)BA_irb);
      CloseDevice((struct IORequest *)BA_irb);
      DeleteStdIO(BA_irb);
      DeletePort(BA_inputport);
   }
*/



BOOL     BAG_CheckConnection(s, w)
struct Screen  *s;
struct Window  *w;
{
   BOOL     v = TRUE;

   if(!VBSG_novideocheck) {
      do {
         BAG_SetupVideoScreen(s);
         v = VBS_CheckLoopback();
         BAG_RestoreVideoScreen();
      } while(!v && ERRG_Message(w, EVIDEOCONNECTION)!=ERROR_CANCEL);
   }
   return(v);
}

static struct RastPort  *BA_msgrp;
static int               BA_msgy;
static struct NewScreen	 BA_rns = {
	0, 0, 640, RSCR68000HEIGHT, 1,
	1, 1, HIRES, CUSTOMSCREEN
};
static struct Screen	*BA_rscreen;
static struct Window	*BA_rwindow;
static struct TextFont	*BA_textfont;
static struct Rectangle  BA_taperect, BA_msgrect, BA_filerect;
static int		 BA_filemaxx;

#define BOXCOLOR	2
#define BOXBORDERCOLOR	1
#define BOXTEXTCOLOR1	1
#define BOXTEXTCOLOR2	3

void BAG_SetupScreen(kk/*type, backupname, path*/)
struct Screen *kk;
/*UBYTE	 type;
char	*backupname;
char	*path;*/
{
   struct Screen       *DefPubScreen;
   ULONG                ModeID;
   UWORD                pens[] = { ~0 };
   struct ColorSpec     colors[] =
   {
      { 0, 15, 14, 13},
      { 1,  0,  0,  0},
      { 2,  6,  8,  9},
      { 3, 15, 15, 15},
      {-1,  0,  0,  0}
   };
   int			xsize, ysize, ysize2, ysize4;

   int			winleft, wintop;
               
   int			winwidth, winheight, i;


   
   
   if(!VBSG_is68000) {

      if(DefPubScreen = LockPubScreen(NULL))
      {
         ModeID       = GetVPModeID(&DefPubScreen->ViewPort);
         UnlockPubScreen(NULL, DefPubScreen);
      }
      else
         ModeID       = HIRES_KEY;
      if(ModeID==INVALID_ID)
         ModeID       = HIRES_KEY;
      BA_rscreen      = OpenScreenTags(NULL,
                                 SA_Pens,      pens,
                                 SA_Depth,     2,
                                 SA_DisplayID, ModeID,
                                 SA_Colors,    (ULONG)colors,
                                 SA_SysFont,   1,
/*                                 SA_Title,     "hugo is gek",*/
                                 TAG_DONE);
      BAG_rdisplayflg = 0;
   } else {
      BA_rscreen	= OpenScreen(&BA_rns);
      if(BA_rscreen) {
         VBS_ShowPlane(BA_rscreen->BitMap.Planes[0],
            BA_rscreen->Width, BA_rscreen->Height,
            GfxBase->ActiView->DxOffset, GfxBase->ActiView->DyOffset,
            /*s->ViewPort.Modes & 0x9FFF*/HIRES|0x1000 /*eenplanig*/,
            GetRGB4(BA_rscreen->ViewPort.ColorMap, 0),
            GetRGB4(BA_rscreen->ViewPort.ColorMap, 1));
      }
      BAG_rdisplayflg = 1;
   }

   if(!BA_rscreen)
      goto quit;
   BA_rwindow = OpenWindowTags(NULL,
                                  WA_Left,         0,
                                  WA_Top,          0,
                                  WA_Width,        BA_rscreen->Width,
                                  WA_Height,       BA_rscreen->Height,
                                  WA_CustomScreen, BA_rscreen,
                                  WA_Borderless,   TRUE,
/*                                  WA_Backdrop,     TRUE,*/
                                  WA_Activate,     TRUE,
                                  WA_SmartRefresh, TRUE,
                                  TAG_DONE);
   if(!BA_rwindow)
      goto quit;
   BA_msgrp 	= BA_rwindow->RPort;
/*   BA_msgwidth  = BA_rscreen->Width;
   BA_msgheight = BA_rscreen->Height;
   BA_msgy	= 19;*/

   BA_textfont  = UIG_OpenFont(&VBSG_text_ta);
   if(!BA_textfont)
       BA_textfont = UIG_OpenDefaultFont();
   
   SetFont(BA_msgrp, BA_textfont);
   SetRast(BA_msgrp, 0);
/*   
   vi = GetVisualInfo(BA_rscreen, TAG_END);
   DrawBevelBox(BA_msgrp, 20, 150, 300, 70, GT_VisualInfo, vi,
   	GTBB_Recessed, FALSE, TAG_END);
   FreeVisualInfo(vi);
*/

   xsize     = TextLength(BA_msgrp, "h", 1);
   ysize     = VBSG_text_ta.ta_YSize;
   ysize2    = ysize >> 1;
   ysize4    = ysize >> 2;

   winleft   = 0;
   wintop    = 0;
               
   winwidth  = BA_rwindow->Width;
   winheight = BA_rwindow->Height;


   SetDrMd(BA_msgrp, JAM2);
   SetAPen(BA_msgrp, BOXCOLOR);
   SetOPen(BA_msgrp, BOXBORDERCOLOR);
   RectFill(BA_msgrp, winleft + xsize,            wintop + ysize4,
                winleft + winwidth - xsize,
                wintop + ysize*3 + ysize2*3 + ysize4);
   BNDRYOFF(BA_msgrp);
   
   i = wintop + ysize*4 + ysize2*3 + ysize4;
   Rect(BA_msgrp, xsize, i,
                  winwidth - xsize*2,
                  winheight-ysize4-i, BOXBORDERCOLOR);
   BA_msgrect.MinX  = xsize+1;
   BA_msgrect.MinY  = i+1;   
   BA_msgrect.MaxX  = winwidth-xsize-2;
   BA_msgrect.MaxY  = winheight-ysize4-2;
   BA_msgy = BA_msgrect.MinY+BA_msgrp->TxBaseline;
quit:
   BA_taperect.MinX = 0; /*signal rb() */
}

static void BA_RestWindow(type, backupname)
char	 type;
char	*backupname;
{  
   int xsize,ysize,ysize2,ysize4;
	
   xsize     = TextLength(BA_msgrp, "h", 1);
   ysize     = VBSG_text_ta.ta_YSize;
   ysize2    = ysize >> 1;
   ysize4    = ysize >> 2;
   
   SetDrMd(BA_msgrp, JAM1);

   SetAPen(BA_msgrp, BOXTEXTCOLOR2);
   Move(BA_msgrp, xsize*2, ysize*2+ysize2*4 + ysize4);
   Text(BA_msgrp, "Tape: ", 6);
   BA_taperect.MinX = BA_msgrp->cp_x;
   BA_taperect.MinY = BA_msgrp->cp_y-BA_msgrp->TxBaseline;
   BA_taperect.MaxX = BA_rwindow->Width - xsize-3;
   BA_taperect.MaxY = BA_taperect.MinY+ysize;
   SetAPen(BA_msgrp, BOXTEXTCOLOR1);
   Text(BA_msgrp, "Press PLAY on VCR", 17); 
   BA_ok = FALSE;
   
   SetAPen(BA_msgrp, BOXTEXTCOLOR2);
   Move(BA_msgrp, xsize*2, ysize2*2 + ysize4);   
   Text(BA_msgrp, BA_verifying ? "Verifying from: ":"Restoring to:   ", 16);
   SetAPen(BA_msgrp, BOXTEXTCOLOR1); 
   Text(BA_msgrp, BA_path, strlen(BA_path));

   SetAPen(BA_msgrp, BOXTEXTCOLOR2);
   Move(BA_msgrp, xsize*2, ysize*1+ysize2*3 + ysize4);   
   Text(BA_msgrp, "File: ", 6);

   BA_filerect.MinX = BA_filemaxx = BA_msgrp->cp_x;
   BA_filerect.MinY = BA_msgrp->cp_y-BA_msgrp->TxBaseline;
   BA_filerect.MaxX = BA_rwindow->Width - xsize-3;
   BA_filerect.MaxY = BA_filerect.MinY+ysize;
   
   Move(BA_msgrp, BA_rwindow->Width*2/3, ysize2*2 + ysize4);
   Text(BA_msgrp, "Name: ", 6); SetAPen(BA_msgrp, BOXTEXTCOLOR1);
   Text(BA_msgrp, backupname, strlen(backupname));
}


void BAG_RestoreScreen()
{
   if(BAG_rdisplayflg & 1)
         VBS_RestoreScreen();
   CloseWindow(BA_rwindow);
   CloseScreen(BA_rscreen);
   CloseFont(BA_textfont);
   BAG_rdisplayflg = 0;
}


void BAG_PrintMsg(msg)
UBYTE *msg;
{
   int ystep;

   SetAPen(BA_msgrp, BOXTEXTCOLOR1);
   SetBPen(BA_msgrp, 0);
   SetDrMd(BA_msgrp, JAM1);
   ystep = BA_msgrp->Font->tf_YSize;
   if(BA_msgy > BA_msgrect.MaxY)
   {
      ScrollRaster(BA_msgrp, 0, ystep, 
	 		     BA_msgrect.MinX, BA_msgrect.MinY, 
      			     BA_msgrect.MaxX, BA_msgrect.MaxY);
      BA_msgy-=ystep;
   }
   Move(BA_msgrp, BA_msgrect.MinX, BA_msgy);
   Text(BA_msgrp, msg, strlen(msg));
   BA_msgy+=ystep;
}

void BAG_ShowMessage(s, message1, message2, c1, c2)
struct Screen  *s;
UBYTE          *message1, *message2;
UWORD           c1,c2;
{
   struct RastPort   *rp;
   struct TextFont   *font;
   UWORD              colors[2];

   colors[0] = c1; colors[1] = c2;
   rp = BA_vhrp;
   SetRast(rp, 0);
   SetAPen(rp, 1);
   font = rp->Font;
   SetFont(rp, BA_smallfont);
   if(message1) {
      Move(rp, 20, 70);
      Text(rp, message1, strlen(message1));
   }
   if(message2) {
      Move(rp, 20, 130);
      Text(rp, message2, strlen(message2));
   }
   SetFont(rp, font);
   LoadRGB4(&BA_hrsv.ViewPort, &colors[0], 2);
   Delay(2);
   LoadView(&BA_hrsv.View);
}

VBS_error_t  BAG_Search(type, nameprefix, fullname, foundtypep)
UBYTE     type;
UBYTE    *nameprefix;
UBYTE    *fullname;
UBYTE	 *foundtypep;
{
   UBYTE   *headerblock=NULL, *p, c, foundtype;
   int      len;
   BOOL     ok = FALSE;
   static UBYTE *backuptype[] = {
      "Unknown", "File System", "AmigaDOS floppy", "MS-DOS floppy",
      "Testpattern",
      "FileSystem FAST", "FileSystem FAST Compressed"
   };

   len = strlen(nameprefix);
   sprintf(BA_msg, SEARCHINGFORTX " %s: %s", backuptype[type], nameprefix);
   BAG_PrintMsg(BA_msg);
   BAG_PrintMsg(PRESSPLAYTX);
   do {
      headerblock = VBS_Search();
      if(!headerblock) {
         BAG_PrintMsg(SEARCHERRTX);
         return(ESEARCH);
      }
      c = headerblock[1+len];
      headerblock[1+len] = '\0';
      foundtype = headerblock[0];
      if((type == foundtype || 
      	 (type==BT_FS && foundtype>=BT_FSFAST && foundtype<=BT_FSFASTCOMPR))
      	 && !StrCmpNoCase(headerblock+1, nameprefix)) {
         p = FOUNDTX; ok = TRUE;
      } else {
         p = ENCOUNTEREDTX;
      }
      headerblock[1+len] = c;
      sprintf(BA_msg, "%s %s: %s", p, backuptype[foundtype],
              headerblock+1);
      if(fullname)
         strcpy(fullname, headerblock+1);
      BAG_PrintMsg(BA_msg);
   } while(!ok);
   if(foundtypep)
   	*foundtypep = foundtype;
   return(EOK);
}

static BOOL BA_SetFileDateInit(path)
UBYTE    *path;
{

   BA_deviceproc = DeviceProc(path);
   if(!BA_deviceproc)
      goto error;
   BA_tsk = (struct Process *)FindTask(NULL);
   BA_bstrpt = AllocMem(35, 0);
   if(!BA_bstrpt)
      goto error;

   if(!(BA_packet = (struct StandardPacket *)
      AllocMem(sizeof(struct StandardPacket), MEMF_CLEAR|MEMF_PUBLIC)))
         goto error;
   memset((UBYTE *)&BA_packetst, 0, sizeof(struct StandardPacket));
   BA_packetst.sp_Msg.mn_Node.ln_Name = (UBYTE *)&BA_packet->sp_Pkt;
   BA_packetst.sp_Pkt.dp_Link         = &(BA_packet->sp_Msg);
   BA_packetst.sp_Pkt.dp_Type         = ACTION_SET_DATE;

   BA_packetst.sp_Pkt.dp_Arg1         = NULL;
   BA_packetst.sp_Pkt.dp_Arg3         = ((long)BA_bstrpt)>>2;

   BA_packetst.sp_Pkt.dp_Port         = &(BA_tsk->pr_MsgPort);

   return(FALSE);    /* OK */
error:
   BA_SetFileDateCleanup();

   return(TRUE);
}
static void     BA_SetFileDateCleanup()
{
   if(BA_bstrpt)
      FreeMem(BA_bstrpt, 35);
   if(BA_packet)
      FreeMem((UBYTE *)BA_packet, sizeof(struct StandardPacket));
   BA_bstrpt = NULL;
   BA_packet = NULL;
}
static void    BA_SetDateProt(dlock, name, namelen, date, prot)
BPTR               dlock;
UBYTE             *name;
UBYTE              namelen;
struct DateStamp  *date;
ULONG              prot;
{
         *BA_packet = BA_packetst;
         BA_packet->sp_Pkt.dp_Arg2 = dlock;
         BA_bstrpt[0] = namelen;
         strcpy(BA_bstrpt+1, name);
         BA_packet->sp_Pkt.dp_Arg4 = (BPTR)(date);
         PutMsg(BA_deviceproc, (struct Message *)BA_packet);
         WaitPort(&(BA_tsk->pr_MsgPort));
         GetMsg(&(BA_tsk->pr_MsgPort));

         *BA_packet = BA_packetst;
         BA_packet->sp_Pkt.dp_Type = ACTION_SET_PROTECT;
         BA_packet->sp_Pkt.dp_Arg2 = dlock;
         BA_packet->sp_Pkt.dp_Arg4 = (BPTR)(prot);
         PutMsg(BA_deviceproc, (struct Message *)BA_packet);
         WaitPort(&(BA_tsk->pr_MsgPort));
         GetMsg(&(BA_tsk->pr_MsgPort));
}

static long BA_Verify(fn, buffer, len)
BPTR      fn;
UBYTE    *buffer;
register long      len;
{
   register long     reallen,d;

   if(!len)
   	return(0);
   /* len > 0 */
   d = BA_type == BT_FS ? (long)buffer&3 : 0;

   *(long *)(BA_cmpbuf) = *(long *)(buffer-d);

   reallen = Read(fn, BA_cmpbuf+d, len);
   if(reallen < len)
      return(reallen);
   return(VBS_MemCmpL(BA_cmpbuf, buffer-d, d+len) ? 0 : len);
}
static int BA_NumDirsIncluded(dn)
VBS_DirNode_t  *dn;
{
   int   included;

   included = 0;
   while(dn) {
      if(dn->Flags & ERRF_INCLUDE) {
         included ++;
      }
      dn = dn->Next;
   }
   return(included);
}

static int BA_NumFilesIncluded(fn)
VBS_FileNode_t *fn;
{
   int   included;

   included = 0;
   while(fn) {
      if(fn->Flags & ERRF_INCLUDE) {
         included ++;
      }
      fn = fn->Next;
   }
   return(included);
}

static void  BA_PrintMsgPathFile(s)
UBYTE    *s;
{
   sprintf(BA_msg, s, BA_path);
   BAG_PrintMsg(BA_msg);
}
void BAG_DelayJiffies(jiffies)
long     jiffies;
{
   jiffies += VBS_ReadTimer();
   while(VBS_ReadTimer() < jiffies)
      ;

/*   while(jiffies>0) {
      WaitTOF();
      jiffies--;
   }*/
}

static VBS_error_t	BA_ReadInit()
{
	BA_chunknr = 0; /*BA_rp = 0;*/
	BA_DataOffset = 0; VWG_DataOffset = 0; VWG_DataLength = 0;
	return(EOK);
}

static VBS_error_t	BA_WriteInit()
{
	BA_chunknr = 0;
	BA_DataOffset = 0;BA_DataLength = BA_memsize;
	return(EOK);
}
long BA_totallen, BA_totalorig, BA_numt;


static VBS_error_t	wb()
{
	Delay(10); Forbid();
	if(BA_type == BT_FSFAST || BA_type == BT_FSFASTCOMPR) {
		VBS_WriteMemRS(BA_DataOffset, BA_DataLength, BA_mem, 10, 
			       RSG_shiftregtab, BA_check);
	}
	if(BA_type == BT_FS) {
		VBS_WriteMem(BA_mem, BA_DataLength, NUM_REPEAT);
	}
	Permit();
	if(VBSG_extradelay)
		Delay(VBSG_extradelay*5);
	BA_DataOffset+=BA_DataLength;
	BA_totallen+=BA_DataLength; BA_totalorig+=BA_memsize; BA_numt++;
	return(LPRESSED?EBREAK:EOK);
}


static VBS_error_t	rb()
{
	VBS_error_t	error = EOK;
	int		i;
	char		s[20];
	

	if(LPRESSED)
		return(EBREAK);


#ifdef FILEEMULk
	return(farread(wfn, BA_mem, BA_memsize)!=BA_memsize ? EREAD:EOK);
#else
	for(;;) {
		if(BA_type == BT_FS) {
			while(BA_DataOffset>=VWG_DataOffset+VWG_DataLength) {

				Delay(8);Forbid();
				VBS_ReadMem(BA_mem, BA_memsize);
				Permit();
				if(LPRESSED) {
					return(EBREAK);
				}
				VWG_DataOffset += VWG_DataLength;
				VWG_DataLength  = BA_memsize;
			}
			goto ok;
		} else {
			VWG_DataOffset = BA_DataOffset; VWG_DataLength = 0;
	
			Delay(8);Forbid();
			error = VBS_ReadMemRS(&VWG_DataOffset, &VWG_DataLength,
				BA_mem, 10, RSG_shiftregtab, BA_check);
			Permit();
			if(error == EBREAK)
				return(EBREAK);
			if(error == EOK)
				goto ok;
		}
		if(BA_taperect.MinX) {
		     SetAPen(BA_msgrp, BOXCOLOR);
		     RectFill(BA_msgrp, BA_taperect.MinX, BA_taperect.MinY,
		     			BA_taperect.MaxX, BA_taperect.MaxY);
		     SetAPen(BA_msgrp, BOXTEXTCOLOR1);
		     Move(BA_msgrp, BA_taperect.MinX, 
		     		    BA_taperect.MinY+BA_msgrp->TxBaseline);
		     		    
		     BA_ok = FALSE; 
		     if(VWG_DataOffset > BA_DataOffset) {
			Text(BA_msgrp, "Please rewind Tape", 13);
			DisplayBeep(BA_rscreen);
/*			sound(3000);
			delay(500);nosound();*/
		     }
		     else if((VWG_DataOffset+VWG_DataLength+1048576L) < BA_DataOffset) {
			Text(BA_msgrp, "Please Fast Forward Tape: ", 26);
			sprintf(s, "%13ld", BA_DataOffset-VWG_DataOffset-VWG_DataLength);
			Text(BA_msgrp, s, 13);
		     }

		}
	}
#endif
ok:	Delay(2);
	if(BA_taperect.MinX && !BA_ok) {
		BA_ok = TRUE;
		SetAPen(BA_msgrp, BOXCOLOR);
		RectFill(BA_msgrp, BA_taperect.MinX, BA_taperect.MinY,
		    		   BA_taperect.MaxX, BA_taperect.MaxY);
		SetAPen(BA_msgrp, BOXTEXTCOLOR1);
		Move(BA_msgrp, 	BA_taperect.MinX, 
				BA_taperect.MinY+BA_msgrp->TxBaseline);
		
		Text(BA_msgrp, "Running OK", 10);
	}
	if(BA_type == BT_FS)
		return(EOK);
	else {

		i = VWG_CorrectErrs(BA_mem, 10, BA_check);

		if(i<0)
			return(EVIDEOREAD);
		else
			return(EOK);
	}
}



	
static VBS_error_t	BA_AllocBuffers(reason)
BA_Reason_t	reason;
{
   BA_mem = NULL; BA_mem2 = NULL; BA_check = NULL; BA_cmpbuf = NULL;
   if(BA_type==BT_FS) {
   	BA_memsize = MEMSIZE;
   	BA_mem = AllocVec(BA_memsize+4, 0);
   	if(!BA_mem)
   		goto error;

   }
   if(BA_type==BT_FSFASTCOMPR && 
     (reason==BA_RESTORE || reason==BA_BACKUP)) {  
   	BA_memsize2 = 500000;
   	BA_mem2 = AllocVec(BA_memsize2+4, 0);
   	if(!BA_mem2)
   		goto error;
   } 
   
   if(BA_type==BT_FSFAST || BA_type==BT_FSFASTCOMPR) {
/*   	BA_memsize = reason==BA_BACKUP ? K*1024 : N*1024;*/

   	BA_mem = AllocVec(N*1024+4, 0);
   	if(!BA_mem)
   		goto error;
   	BA_memsize = K*1024;
   	BA_check = AllocVec((N-K)*1024, 0);
   	if(!BA_check)
   		goto error;
   }
   if(!BA_mem2) {
   	BA_mem2 = BA_mem; BA_memsize2 = BA_memsize;
   }
   if(BA_type!=BT_FSFASTCOMPR && reason == BA_VERIFY) {
   	BA_cmpbuf = AllocVec(BA_memsize2+4, 0);
   	if(!BA_cmpbuf)
   		goto error;
   }
   return(EOK);
error:
   BA_FreeBuffers();
   return(EMEMORY);
}

static void BA_FreeBuffers()
{   
   if(BA_mem2 == BA_mem) {
   	BA_mem2 = NULL;     BA_memsize2 = 0;
   }
   if(BA_mem) {
   	FreeVec(BA_mem);    BA_mem    = NULL; BA_memsize  = 0;
   }
   if(BA_mem2) {
   	FreeVec(BA_mem2);   BA_mem2   = NULL; BA_memsize2 = 0;
   }
   if(BA_cmpbuf) {
   	FreeVec(BA_cmpbuf); BA_cmpbuf = NULL;
   }
   if(BA_check) {
   	FreeVec(BA_check);  BA_check  = NULL;
   }
}
int BA_tdelay, BA_twrite, BA_tvwrite, BA_tread;

static BPTR BA_tmpfn;
VBS_error_t  BAG_Backup(type, dn, name, path, archive)
char		type;
VBS_DirNode_t  *dn;
UBYTE          *path, *name;
BOOL            archive;
{
   struct Process *thisprocess;
   struct Window  *oldwindow;
   VBS_error_t    error = EMEMORY;
   BPTR		  oldlock;
	long a,b;
   register	  int i;
	
   BA_type 	= type;

   BA_tmpfn	= NULL;

   if(error = BA_AllocBuffers(BA_BACKUP))
   	goto quit;
   
   error = BA_WriteInit();
   if(error)
	goto quit;
   BA_memptr = BA_mem;
   BA_BackupTree(dn);
   if(BA_mem!=BA_memptr) {
   	BA_DataLength = BA_memptr-BA_mem;
	if(error = wb())
		goto quit;
   }
   srand((unsigned)BA_mem);
   for(i = 0; i<BA_memsize; i++) {
   	BA_mem[i] = rand()>>8;
   }

   Delay(5*50);
   if(BA_type == BT_FS)
   	VBS_WriteFirstBlockSync();
   else 
   	BA_DataOffset = MAXDIRSIZE;	

   BA_totallen=0; BA_totalorig=0; BA_numt=0;


   BA_memoff  = 0; BA_memoff2 = 0;
   BA_archive = archive;

   BA_filedirdelay   = 0;
   BA_tmplock = Lock(path, ACCESS_READ);
   if(!BA_tmplock) {
   	error = EREAD; strcpy(VBSG_errorinfo, path);
   	goto quit;
   }
   oldlock = CurrentDir(BA_tmplock);

   BA_wantedduration = 0;
   if(BA_type == BT_FSFAST || BA_type == BT_FSFASTCOMPR) {
   	thisprocess = (struct Process *)FindTask(NULL);
  	oldwindow = (struct Window *)thisprocess->pr_WindowPtr;
   	thisprocess->pr_WindowPtr = (APTR)-1;
   
   	a = clock();
	BA_tmpfn = Open("_VBSTMP_.TMP", MODE_NEWFILE);
   	thisprocess->pr_WindowPtr = (APTR)oldwindow;

   	if(!BA_tmpfn) {
   		BA_wantedduration = JIFFIES_PER_SECOND/2;
   	} else if(BA_type == BT_FSFASTCOMPR) {
		if(Write(BA_tmpfn, BA_mem, BA_memsize)!=BA_memsize) {
	   		error = EWRITE; strcpy(VBSG_errorinfo, "tmpfile");
		   	goto quit;
		}
		Close(BA_tmpfn);
		BA_tmpfn = NULL;
		BA_wantedduration = 8+(((clock()-a)*12)/10);
   		DeleteFile("_VBSTMP_.TMP");
   	}
   }

BA_tdelay = 0; BA_twrite = 0; BA_tvwrite = 0;
BA_tread = 0;
   BA_chunknr 	 = 0;
   BA_DataLength = 0;
   BA_starttime  = clock();
   a=clock();
   BA_duration   = 0; 
   BA_llduration = 0; BA_nopen = 0;
   if(error = BA_BackupFiles(dn)) {
   	if(BA_type == BT_FS)
		VBS_WriteErrorIndicator();
	goto quit;
   }
   b=clock();
   while(BA_memoff2) {
   	BA_Mem2Full();
   }
   if(BA_memoff) {
	if(BA_wantedduration >= BA_duration) {
		BAG_DelayJiffies(1+BA_wantedduration-BA_duration);
	}
	if(error = wb())goto quit;
	BA_memoff = 0;
   }
   b=clock();

   Delay(2*50);
   BA_memoff2 = 0;
quit:
   if(BA_tmplock) {
   	if(BA_tmpfn) {
	   	Close(BA_tmpfn); BA_tmpfn = NULL;
   		DeleteFile("_VBSTMP_.TMP");
   	}
   	CurrentDir(oldlock);
   	UnLock(BA_tmplock);
   	BA_tmplock = NULL;
   }
   BA_FreeBuffers();
   return(error);
}


static VBS_error_t BA_Mem2Full()
{
	long 		 bytescomp;
	char		*p, *p2;
	ULONG		 a, b;
	VBS_error_t	 error = EOK;

	
 	BA_tread      += clock()-BA_starttime;
	if(BA_type==BT_FSFASTCOMPR) {
		a=clock();
		Forbid();
		DCG_Compress(BA_mem2, BA_memoff2, &p2,
			     BA_mem+BA_memoff, BA_memsize-BA_memoff, &p,
		      	     BA_check);
		bytescomp  = p2-BA_mem2;
		Permit();
		BA_memoff  = p-BA_mem;
		BA_DataLength += bytescomp;
		BA_memoff2 = BA_memoff2-bytescomp;
		if(BA_memoff2){
			 memcpy(BA_mem2, p2, BA_memoff2);
		}
	} else {
		BA_DataLength += BA_memoff2;
		BA_memoff  = BA_memsize;
		BA_memoff2 = 0;
	}
	BA_duration   += clock()-BA_starttime;

	if(!(BA_type==BT_FSFASTCOMPR) || BA_memoff2>0) {
		/*blijkbaar istie vol!*/
		 BAG_DelayJiffies(1); BA_tdelay++;
		 if(BA_wantedduration > BA_duration) {
			 BAG_DelayJiffies(BA_wantedduration-BA_duration);
			 BA_tdelay += BA_wantedduration-BA_duration;
		 }
		 a=clock();
		 if(error = wb())
		 	goto quit;
		 BA_tvwrite += clock()-a;

 		 switch(BA_type) {
 		 case BT_FSFAST:
 		    if(BA_tmpfn) {
			a = clock();
		    	if(Seek(BA_tmpfn, 0, OFFSET_BEGINNING)<0 ||
		    	   Write(BA_tmpfn, BA_mem, BA_memsize)!=BA_memsize) {
		        	strcpy(VBSG_errorinfo, "tmpfile");
				error = EWRITE; goto quit;
			}
			b = clock()-a;
		    	BA_twrite+=b;
		    	BA_wantedduration = ((BA_duration+1)*12L)/10 - (b>>1);
		    }
		    break;				
		 case BT_FS:		   				
		    BA_wantedduration = (BA_duration*VBSG_writefactor)/100+
		   			 BA_filedirdelay;
		 }
		 BA_DataLength = 0;
		 
		 BA_duration = 0; BA_filedirdelay = 0;
		 if(BA_wantedduration<0)
		 	BA_wantedduration  = 0;
		 BA_memoff = 0;		 
	}
quit:	
	BA_starttime = clock();
	return(error);
}


static VBS_error_t      BA_BackupFiles(dn)
VBS_DirNode_t  *dn;
{
   VBS_DirNode_t   *dnode;
   VBS_FileNode_t  *fnode;
   ULONG            fs, rlen, mo2;
   VBS_error_t      error = EOK;
   BPTR		    fn;
   BPTR		    lock, oldlock;
   
   BA_filedirdelay += VBSG_perdir;
   fnode = dn->FirstFile;
   while(fnode) {
      	if(LPRESSED) {
      		error = EBREAK; goto quit;
      	}
      	if(!(fnode->Flags & ERRF_INCLUDE))
	 	goto nextfile;
      	BA_filedirdelay += VBSG_perfile;
	 
      	fn = Open(fnode->Name, MODE_OLDFILE);
      	if(!fn) {
		error = EREAD;
      		strcpy(VBSG_errorinfo, fnode->Name);
      		goto quit;
      	}
      
      	fs   = fnode->Size;
      
	while(fs) {
      		rlen = BA_memsize2 - BA_memoff2;
      		if(rlen>fs)
      			rlen = fs;
		if(rlen) {
			mo2 = BA_type!=BT_FS?(BA_memoff2+3)&~3:BA_memoff2;
		 	if(Read(fn, BA_mem2+mo2, rlen)!=rlen) {
		      		error = EREAD;
      				strcpy(VBSG_errorinfo, fnode->Name);
      				goto quit;
      			}
      			if(mo2 != BA_memoff2) {
      				DCG_CopyMemLong(BA_mem2+mo2, BA_mem2+BA_memoff2, rlen);
      			}
		}
		if(LPRESSED) {
			error = EBREAK; goto quit;
	 	}
	 	BA_memoff2 += rlen;
		if(BA_memoff2 >= BA_memsize2) {
			if(error = BA_Mem2Full()) {
				strcpy(VBSG_errorinfo, fnode->Name);
	 			goto quit;
	 		}
	 	}
		fs -= rlen;
      	}
      	Close(fn); fn = NULL;
      
      	if(BA_archive)
      		SetProtection(fnode->Name, fnode->Protection|FIBF_ARCHIVE);
nextfile:
      	fnode = fnode->Next;
   }
   dnode = dn->FirstDir;

   while(dnode && !error) {
   	lock = Lock(dnode->Name, ACCESS_READ);
   	if(!lock) {
   		strcpy(VBSG_errorinfo, dnode->Name);
		error = EREAD; goto quit;
	}
	oldlock = CurrentDir(lock);
      
      	error = BA_BackupFiles(dnode);
/*      if(BA_archive)
	  _chmod(".", 1, (dnode->Attribute&ERRF_ATTRMASK)&~FA_ARCH);*/
	CurrentDir(oldlock);
	UnLock(lock);
	dnode = dnode->Next;
   }
quit:
   return(error);
}



VBS_error_t     BAG_RestoreFilesTo(type, dn, path, backupname)
char		type;
VBS_DirNode_t  *dn;
char           *path;
char	       *backupname;
{
   VBS_error_t   	 error = EOK;
   BOOL		 	 vbstempcreated = FALSE;
   BPTR		  	 origcdlock = NULL, lock = NULL;
   struct FileInfoBlock	*fib;
	int l;

   BA_type 	= type;
   BA_verifying = FALSE;

   if(error = BA_AllocBuffers(BA_RESTORE))
   	goto quit;
   BA_tmpfn	= NULL;
   BA_tmplock   = NULL;
   
   if(BA_SetFileDateInit(path)) {
      error = EMEMORY;
      goto quit;
   }
   BA_memptr = BA_mem;
   lock = Lock(path, ACCESS_READ);
   if(!lock) {
      error = EREAD;
      strcpy(VBSG_errorinfo, path);
      goto quit;
   }
   origcdlock = CurrentDir(lock);
   
/*   sprintf(BA_msg, RESTORINGTOTX " %s", path);
   BAG_PrintMsg(BA_msg);
   BAG_PrintMsg(PRESSPLAYTX);*/
   strcpy(BA_path, path); l = strlen(BA_path);
   if(l && BA_path[l-1]!=':' && BA_path[l-1]!='/') {
      strcpy(BA_path+l, "/");
   }
   BA_RestWindow(type, BA_path);
   BA_tmplock = NULL;
   if(BA_type==BT_FSFAST || BA_type==BT_FSFASTCOMPR) {
   	BA_tmplock = CreateDir("_VBSTMPS.DIR");
   	if(BA_tmplock<=0)
   		goto quit;
   	vbstempcreated = TRUE;
   	CurrentDir(BA_tmplock);
   	BA_memoff = 0; BA_memoff2 = 0;BA_nerr = 0;BA_chunknr = 0;
   	BA_DataOffset = MAXDIRSIZE;BA_DataLength = 0;
   	BAG_PrintMsg("Reading data from VCR...");
   	VWG_DataOffset = 0; VWG_DataLength = 0;
   	error = BA_GetFileData(dn, FALSE);
   	CurrentDir(lock);
   	if(error) {
   	   BAG_PrintMsg(FATALERRTX); goto stop;
   	}   	
   } 
   if(BA_type == BT_FS) {
	error = VBS_SearchFirstBlockSync();
	if(error)
      		goto quit;
      	BA_DataOffset  = VWG_DataOffset+VWG_DataLength;
   } else {
   	BA_DataOffset  = MAXDIRSIZE;
	VWG_DataOffset = 0; VWG_DataLength = 0;
   }   	
   BA_verifying = FALSE;
/*   BA_RestWindow(backupname, BT_FS);*/

   BA_memoff = 0; BA_memoff2 = 0;BA_nerr = 0;BA_chunknr = 0;

   BAG_PrintMsg("Creating files&directories...");
   
   error = BA_RestoreFiles(dn, FALSE);
   if(error) {
      BAG_PrintMsg(FATALERRTX);
   } else {
      if(BA_nerr) {
	 sprintf(BA_msg, "%ld " ERRONEOUSTX, BA_nerr);
	 BAG_PrintMsg(BA_msg);
      } else {
	 BAG_PrintMsg(RESTOREOKTX);
      }
   }
stop:
   BAG_PrintMsg(PRESSSTOPTX);
   BAG_PrintMsg(PRESSLMBTX);

/*   SetAPen(BA_rp, 10);
   RectFill(BA_rp, 80, 24+14+6+14+6, 620, 24+14+6+14+6+13);
   Move(BA_rp,  80, 24+14+6+14+6);
   SetAPen(BA_rp, 15); Text(BA_rp, "Press STOP on VCR, then press any key", 37);
*/
   while(!LPRESSED)
   	Delay(2);
quit:
   if(vbstempcreated) {/*changed*/
     	fib = (struct FileInfoBlock *)AllocMem(sizeof(struct FileInfoBlock),
                                              MEMF_CLEAR);
   	CurrentDir(BA_tmplock);
   	if(Examine(BA_tmplock, fib) && fib->fib_EntryType >= 0) {
   		while(ExNext(BA_tmplock, fib)) {
   			DeleteFile(fib->fib_FileName);
      		}
   	}
      	FreeMem((UBYTE *)fib, sizeof(struct FileInfoBlock));
   	
   	CurrentDir(lock);
   	if(BA_tmplock)
   		UnLock(BA_tmplock);
   	BA_tmplock = NULL;
   	DeleteFile("_VBSTMPS.DIR");
   }
   if(origcdlock) {
	   CurrentDir(origcdlock);
   }
   if(lock)
   	UnLock(lock);
   BA_SetFileDateCleanup();
   BA_FreeBuffers();
   return(error);
}
   



VBS_error_t     BAG_VerifyFilesFrom(type, dn, path, backupname)
char		 type;
VBS_DirNode_t   *dn;
char            *path, *backupname;
{
   VBS_error_t   	 error = EOK;
   BPTR		  	 origcdlock = NULL, lock = NULL;
	int l;

   BA_type 	= type;
   BA_verifying = TRUE;

   if(error = BA_AllocBuffers(BA_VERIFY))
   	goto quit;

   BA_memptr = BA_mem;
   lock = Lock(path, ACCESS_READ);
   if(!lock) {
      error = EREAD;
      strcpy(VBSG_errorinfo, path);
      goto quit;
   }
   origcdlock = CurrentDir(lock);
  
   if(BA_type == BT_FSFASTCOMPR) { 
      BAG_PrintMsg("Checking whether 68020 compressed backup is error-free and consistent.");
      BAG_PrintMsg("No file comparisons done.");
   }
/*   BAG_PrintMsg(PRESSPLAYTX);*/
   strcpy(BA_path, path); l = strlen(BA_path);
   if(l && BA_path[l-1]!=':' && BA_path[l-1]!='/') {
      strcpy(BA_path+l, "/");
   }
   BA_RestWindow(type, backupname);

   if(BA_type == BT_FS) {
	error = VBS_SearchFirstBlockSync();
	if(error)
      		goto quit;
      	BA_DataOffset  = VWG_DataOffset+VWG_DataLength;
   } else {
   	BA_DataOffset  = MAXDIRSIZE;
	VWG_DataOffset = 0; VWG_DataLength = 0;
   }   	
/*   BA_RestWindow(backupname, BT_FS);*/

   BA_memoff = 0; BA_memoff2 = 0;BA_nerr = 0;BA_chunknr = 0;

   error = BA_RestoreFiles(dn, FALSE);
   if(error) {
      BAG_PrintMsg(FATALERRTX);
   } else {
      if(BA_nerr) {
	 sprintf(BA_msg, "%ld " VERIFYERRSTX, BA_nerr);
	 BAG_PrintMsg(BA_msg);
      } else {
	 BAG_PrintMsg(VERIFYOKTX);
      }
   }
stop:
   BAG_PrintMsg(PRESSSTOPTX);
   BAG_PrintMsg(PRESSLMBTX);

/*   SetAPen(BA_rp, 10);
   RectFill(BA_rp, 80, 24+14+6+14+6, 620, 24+14+6+14+6+13);
   Move(BA_rp,  80, 24+14+6+14+6);
   SetAPen(BA_rp, 15); Text(BA_rp, "Press STOP on VCR, then press any key", 37);
*/
   while(!LPRESSED)
   	Delay(2);
quit:
   if(origcdlock) {
	   CurrentDir(origcdlock);
   }
   if(lock)
   	UnLock(lock);
   BA_FreeBuffers();
   return(error);
}





static VBS_error_t    BA_RestoreFiles(dn, skipall)
VBS_DirNode_t   *dn;
BOOL		 skipall;
{
   BPTR             fn, tfn, lock, oldlock;
   VBS_DirNode_t   *dnode;
   VBS_FileNode_t  *fnode;
   long             fs;
   ULONG	    bp, mo, mo2, l;
   VBS_error_t      error = EOK;
   char            *appendpath;
   BOOL		    newskipall, fileincluded,created_dir;
   static BOOL	    VideoError;
   static ULONG	    BA_LDataOffset;
   static ULONG	    BA_LDataLength;
   char		   *p2;
   char		    tmpname[10];	

   appendpath = BA_path+strlen(BA_path);

   fnode = dn->FirstFile;
   while(fnode) {
      fs   = fnode->Size; fn = NULL;
      lock = NULL;
      if(fileincluded = (!skipall && (fnode->Flags & ERRF_INCLUDE))) {
	 strcpy(appendpath, fnode->Name);
	 
	 if(!BA_verifying) {
	 	if(UIG_FileExists(fnode->Name)) {
		    	fnode->Flags    = (fnode->Flags & ~ERRF_ERRMASK)|
					   ERRF_FILEEXISTSERROR;
		    	BA_PrintMsgPathFile(FILEEXISTSTX);
		    	fileincluded = FALSE;
	 	} else {
			if(!(fn = Open(fnode->Name, MODE_NEWFILE))) {
		 		error = EWRITE;
			 	strcpy(VBSG_errorinfo, fnode->Name);
			 	goto quit;
			}
		}
	 } else {
	     if(BA_type != BT_FSFASTCOMPR) {
		 if(!(fn = Open(fnode->Name, MODE_OLDFILE))) {
		    fnode->Flags = (fnode->Flags & ~ERRF_ERRMASK)|
			ERRF_FILENOTEXISTSERROR;
		    BA_PrintMsgPathFile(FILENOTEXISTSTX);
		    fileincluded = FALSE; BA_nerr++;
		 }
	     }
	 }

      }
      if(fileincluded) {
	  while(fs) {
		if(BA_DataOffset>=VWG_DataOffset+VWG_DataLength) {
		     SetAPen(BA_msgrp, BOXTEXTCOLOR1);
	 	     SetBPen(BA_msgrp, BOXCOLOR); SetDrMd(BA_msgrp, JAM2);
	 	     Move(BA_msgrp, BA_filerect.MinX, 
	 		BA_filerect.MinY+BA_msgrp->TxBaseline);
	 	     Text(BA_msgrp, BA_path, strlen(BA_path));

	 	     SetAPen(BA_msgrp, BOXCOLOR); 
	 	     if(BA_msgrp->cp_x < BA_filemaxx) {
		 	RectFill(BA_msgrp, BA_msgrp->cp_x, BA_filerect.MinY,
	 			    BA_filemaxx,    BA_filerect.MaxY);
		     }
		     BA_filemaxx = BA_msgrp->cp_x+2;
		
		     if(!BA_verifying && BA_type!=BT_FS/*old*/) {
		     	   oldlock = CurrentDir(BA_tmplock);
			   sprintf(tmpname, "%04d.TMP", BA_chunknr++);
			   if(!(tfn = Open(tmpname, MODE_OLDFILE))) {
			   	sprintf(BA_msg, "Open error %s", tmpname);
			   	BAG_PrintMsg(BA_msg);
				error = EREAD; goto quit;
			   }
			   l = Read(tfn, BA_mem, BA_memsize+8);
			   Close(tfn);
			   if(l==BA_memsize+8) {
				VWG_DataOffset = *(ULONG *)(BA_mem+BA_memsize);
				VWG_DataLength = *(ULONG *)(BA_mem+BA_memsize+4);
				VideoError = FALSE;
			   } else if(l==8) {
				VWG_DataOffset = *(ULONG *)(BA_mem);
				VWG_DataLength = *(ULONG *)(BA_mem+4);
				if(fn>0) Close(fn);fn = NULL;
				VideoError = TRUE;
			   } else {
			   	sprintf(BA_msg, "Read error %s", tmpname);
			   	BAG_PrintMsg(BA_msg);
				error = EREAD; goto quit;
			   }
			   DeleteFile(tmpname);
			   CurrentDir(oldlock);
		     } else {
			   error = rb();
			   VideoError = FALSE;
			   if(error==EVIDEOREAD) {
			       VideoError = TRUE;
			       error = EOK;
			   }

			   if(error)
			       goto quit;
		     }
		     if(BA_type == BT_FSFASTCOMPR && !BA_verifying) {	
			     BA_LDataOffset = VWG_DataOffset;
			     BA_LDataLength = 0;
			     BA_decrptr = BA_mem;
		     } else {
			     BA_LDataOffset = VWG_DataOffset;
			     BA_LDataLength = VWG_DataLength;
		     }
		}
		if(VideoError) {
		     BA_PrintMsgPathFile(VIDEOERRTX " %s");
		     sprintf(tmpname, "%lu", VWG_DataOffset);
		     BAG_PrintMsg(tmpname);
		     BA_DataOffset+=fs; fs=0L;
		} else {
		     while(BA_DataOffset>=BA_LDataOffset+BA_LDataLength) {
			BA_LDataOffset+=BA_LDataLength;

			DCG_Uncompress(BA_decrptr, &BA_decrptr,
					BA_mem2,    &p2);
			BA_LDataLength = p2-(BA_mem2);
			if(BA_LDataLength) {
			     if(BA_LDataLength > BA_memsize2)
				BA_LDataLength = BA_memsize2;
			} else {
			     BA_LDataLength = 0;/*Should not occur!*/
			     BAG_PrintMsg("x");
			}
		     }
		     mo = BA_DataOffset-BA_LDataOffset;
		     bp = BA_LDataLength-mo;
		     if(fs<bp) 
		     	 bp = fs;
		     if(bp) {
			 BA_DataOffset +=bp; fs-=bp;
			 if(BA_verifying) {
			 /* fn=0 als verifying && type==BTFASTCOMPR*/
			    if(fn>0&&BA_Verify(fn, BA_mem2+mo, bp) != bp) {
				BA_PrintMsgPathFile(VERIFYERRTX " %s");
				fnode->Flags = (fnode->Flags & ~ERRF_ERRMASK)|
						ERRF_VERIFYERROR;
				BA_nerr++;
				BA_DataOffset+=fs; fs=0L;
			    }
			 } else {
			    mo2 = BA_type==BT_FS ? mo : mo&~3;
			    if(mo!=mo2) {
			    	DCG_CopyMemLong(BA_mem2+mo, BA_mem2+mo2, bp);
			    }
			    if(fn>0 && Write(fn, BA_mem2+mo2, bp)!=bp) {		
				BA_PrintMsgPathFile(WRITEERRTX " %s");
				BA_DataOffset+=fs; fs=0L;
			    }
			 }
		     }
		}
	  }
      } else
	  BA_DataOffset += fs;

      if(fn>0) {
	  Close(fn); fn = NULL;
	  if(!BA_verifying) {
	  	SetFileDate(fnode->Name,  &fnode->Date);
	  	SetProtection(fnode->Name, fnode->Protection);
	  	if(fnode->Flags & ERRF_COMMENT) {
	  		SetComment(fnode->Name, fnode->Name+fnode->NameLen+1);
	  	}
	  }
      }
      fnode = fnode->Next;
   }
   dnode = dn->FirstDir;

   while(dnode &&!error) {
      created_dir = FALSE;
      newskipall = TRUE; oldlock = NULL; lock = NULL;
      strcpy(appendpath, dnode->Name); strcat(appendpath, "/");
      if((!skipall) && (dnode->Flags & ERRF_INCLUDE)) {
	 newskipall = FALSE;
	 if(!BA_verifying) {
	 	if(!(lock = CreateDir(dnode->Name))) {
	       		BA_PrintMsgPathFile(COULDNTCREATETX " %s");
		} else {
			created_dir = TRUE;
		}
	 }
	 if(!BA_verifying || BA_type!=BT_FSFASTCOMPR) {
	     if(!lock)
	 	lock = Lock(dnode->Name, ACCESS_READ);
	     if(!lock) {
		if(BA_verifying) {
			BA_PrintMsgPathFile(DIRNOTEXISTS);
			dnode->Flags = (dnode->Flags & ~ERRF_ERRMASK) |
			               ERRF_INCLUDE|ERRF_FILENOTEXISTSERROR;
			newskipall = TRUE;
		} else {
			error = EWRITE; goto quit;
		}
	     }
	     oldlock = CurrentDir(lock);
	 }
      }
      error = BA_RestoreFiles(dnode, newskipall);

      if(!newskipall && (!BA_verifying || BA_type!=BT_FSFASTCOMPR)) {
      	   CurrentDir(oldlock);
      	   UnLock(lock);
      	   if(created_dir) {
	  	SetFileDate(dnode->Name,  &dnode->Date);
	  	SetProtection(dnode->Name, dnode->Protection);
	  	if(dnode->Flags & ERRF_COMMENT) {
	  		SetComment(dnode->Name, dnode->Name+dnode->NameLen+1);
	  	}
           }
      }	 
      if(error)
	   goto quit;
      dnode = dnode->Next;
   }
quit:
   *appendpath = 0;
   return(error);
}

static VBS_error_t    BA_GetFileData(dn, skipall)
VBS_DirNode_t   *dn;
BOOL		 skipall;
{
   BPTR             tfn;
   VBS_DirNode_t   *dnode;
   VBS_FileNode_t  *fnode;
   long             fs, bp;
   VBS_error_t      error = EOK;
   char            *appendpath;
   BOOL		    fileincluded;
   static VBS_error_t      rberror_s;
   ULONG	    l;
   char		    vbstmp[10];

   appendpath    = BA_path+strlen(BA_path);

   fnode = dn->FirstFile;
   while(fnode) {
      fs   = fnode->Size;
      if(fileincluded = (!skipall && (fnode->Flags & ERRF_INCLUDE))) {
	 strcpy(appendpath, fnode->Name);
/*	 SetAPen(BA_rp, 10); RectFill(BA_rp, 80, 24+20, 620, 24+33);
	 SetAPen(BA_rp, 15); Move(BA_rp, 80, 24+20);
	 Text(BA_rp, BA_path, strlen(BA_path));*/
      }
      if(fileincluded) {
	 while(fs) {
	     	if(BA_DataOffset<VWG_DataOffset ||
		   BA_DataOffset>=VWG_DataOffset+VWG_DataLength) {
			SetAPen(BA_msgrp, BOXTEXTCOLOR1);
			SetBPen(BA_msgrp, BOXCOLOR); SetDrMd(BA_msgrp, JAM2);
			Move(BA_msgrp, BA_filerect.MinX, 
	 			BA_filerect.MinY+BA_msgrp->TxBaseline);
			Text(BA_msgrp, BA_path, strlen(BA_path));

			SetAPen(BA_msgrp, BOXCOLOR); 
			if(BA_msgrp->cp_x < BA_filemaxx) {
			   RectFill(BA_msgrp, BA_msgrp->cp_x, BA_filerect.MinY,
	 			    BA_filemaxx,    BA_filerect.MaxY);
		 	}
		 	BA_filemaxx = BA_msgrp->cp_x+2;
		   
			error = rb();
			rberror_s = error;
			if(error == EVIDEOREAD) error = EOK;
			if(error)goto quit;
			sprintf(vbstmp, "%04d.TMP", BA_chunknr++);

			if(!(tfn = Open(vbstmp, MODE_NEWFILE))) {
				error = EWRITE; goto quit;
		 	}
		 	l = (rberror_s == EVIDEOREAD)?0:BA_memsize;
		 	*(ULONG *)(BA_mem+l)   = VWG_DataOffset;
		 	*(ULONG *)(BA_mem+l+4) = VWG_DataLength;
		 	l+=8;
		 	if(Write(tfn, BA_mem, l)!=l) {
				error = EWRITE; goto quit;
		 	}
		 	Close(tfn);/*delay(4);*/
/*		     _AX = 0x0D00;geninterrupt(0x21);*/
		}
		bp = VWG_DataOffset+VWG_DataLength-BA_DataOffset;
		if(fs<bp) bp= fs;
		BA_DataOffset+=bp; fs-=bp;
		if(rberror_s == EVIDEOREAD) {
			 BA_PrintMsgPathFile(VIDEOERRTX " %s");
		}
	 }
      } else
	    BA_DataOffset+=fs;
      fnode=fnode->Next;
   }
   dnode = dn->FirstDir;

   while(dnode &&!error) {
      strcpy(appendpath, dnode->Name); strcat(appendpath, "/");
      error = BA_GetFileData(dnode,
			     skipall || (!(dnode->Flags & ERRF_INCLUDE)));
      dnode = dnode->Next;
   }
quit:
   *appendpath = 0;
   return(error);
}

