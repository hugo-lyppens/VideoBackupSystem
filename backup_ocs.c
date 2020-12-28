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

#include  "vbs.h"
#include  "LD_vbstext.h"

static int               BA_NumDirsIncluded(VBS_DirNode_t *);
static int               BA_NumFilesIncluded(VBS_FileNode_t *);
static VBS_error_t       BA_BackupFiles(VBS_DirNode_t  *, char *);
static void              BA_BackupTree(VBS_DirNode_t  *);
static void              BA_PrepareDirectory(VBS_DirNode_t *,\
                            struct Rastport *, VBS_ListData_t *, int, int);
static VBS_error_t       BA_RestoreDirectory(VBS_DirNode_t **,
                            VBS_DirNode_t *, int);
static void              BA_PrintMsgPathFile(char *, char *);
static VBS_error_t       BA_RestoreFiles(VBS_DirNode_t *, BPTR);
static VBS_error_t       BA_ScanDirectory(VBS_DirNode_t *, char *,
                    struct RastPort *, VBS_ListData_t *, int, int);
static void              BA_SetDateProt(BPTR, char *, UBYTE,
                           struct DateStamp *, ULONG);
static BOOL              BA_SetFileDateInit(char *);
static void              BA_SetFileDateCleanup(void);
static VBS_error_t       BA_VerifyFiles(VBS_DirNode_t *, BPTR);
static long              BA_Verify(BPTR, char *, long);

#define HBMWIDTH     640
#define HBMHEIGHT    150
#define DELAYPERFILE 8     /*(TICKS)*/
#define ROOTNAME     "/"

static struct FileInfoBlock  *BA_fib;
static char                  *BA_mem, *BA_cmpbuf;
static long                   BA_memsize;
static char                  *BA_memptr, *BA_memlimit;
static long                   BA_nopen, BA_nerr, BA_filedirdelay;
static long                   BA_duration, BA_starttime, BA_wantedduration;

static char                   BA_msg[150], BA_path[150], *BA_appendpath;

static BOOL                   BA_archive;
static struct StandardPacket *BA_packet     = NULL, BA_packetst;
static char                  *BA_bstrpt     = NULL;
static struct MsgPort        *BA_deviceproc = NULL;
static struct Process        *BA_tsk;

extern int                    VBSG_writefactor, VBSG_perfile, VBSG_perdir;
extern BOOL                   VBSG_novideocheck;

VBS_error_t  BAG_Backup(dn, name, path, archive, buffer, bl)
VBS_DirNode_t  *dn;
char           *path, *name;
BOOL            archive;
char           *buffer;
long            bl;
{
   VBS_error_t    error = EOK;

   BA_mem     = buffer;
   BA_memsize = bl;

   Delay(10);

   BA_memptr = BA_mem;
   BA_nopen  = 0; /* n-AllocMem */
   BA_BackupTree(dn);
   if(BA_mem != BA_memptr) {
      Delay(10);
      Forbid();
      VBS_WriteMem(BA_mem, BA_memptr-BA_mem, NUM_REPEAT);
      Delay(BA_nopen/10);  /* ongeveer .002 seconden per AllocMem */
      Permit();
   }
   Delay(400);
   VBS_WriteFirstBlockSync();
   BA_memptr  = BA_mem;
   BA_archive = archive;

   BA_wantedduration = 0;
   BA_filedirdelay   = 0;
   VBS_GetTimer();
   BA_starttime = VBS_ReadTimer();
   if(error = BA_BackupFiles(dn, path)) {
      VBS_WriteErrorIndicator();
      goto quit;
   }
   if(BA_mem != BA_memptr) {
      BA_duration = (VBS_ReadTimer() - BA_starttime) & 0x00FFFFFF;

      BA_wantedduration -= BA_duration;
      if(BA_wantedduration < 0)
         BA_wantedduration = 0;
      BAG_DelayJiffies(1+BA_wantedduration); Forbid();
      VBS_WriteMem(BA_mem, BA_memptr-BA_mem, NUM_REPEAT);
      Permit();
   }
   Delay(25);
   VBS_ShowBlack();
   UIG_DelayLMB(150);
   VBS_RestoreScreen();
   Delay(2);
   BA_memptr = BA_mem;

quit:
   VBS_FreeTimer();
   BA_mem = NULL; BA_memsize = NULL;
   return(error);
}

static long  BA_tb, BA_nf, BA_nd;


VBS_error_t    BAG_ScanDirectories(dnp, path, rp, tb, nf, nd, fl, offset, delt)
VBS_DirNode_t  **dnp;
char            *path;
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
      goto quit;
   }

   Examine(lock, BA_fib);
   UnLock(lock);
   if(BA_fib->fib_EntryType <= 0) {
      error = EREAD;
      goto quit;
   }

   l = strlen(ROOTNAME);
   if(!(*dnp = (VBS_DirNode_t *)AllocMem(sizeof(VBS_DirNode_t)+l, MEMF_CLEAR))) {
      error = EMEMORY;
      goto quit;
   }
   tl = TextLength(rp, ROOTNAME, l)+offset;

   fl->MaxTotalWidth = tl;
   fl->NumEntries    = 1;

   (*dnp)->Flags      = ERRF_INCLUDE;
   (*dnp)->ParentDir  = NULL;
   (*dnp)->Protection = BA_fib->fib_Protection;
   (*dnp)->Date       = BA_fib->fib_Date;
   (*dnp)->NameLen    = l;
   strcpy((*dnp)->Name, ROOTNAME);

   BA_tb = 0; BA_nd = 1; BA_nf = 0;

   error = BA_ScanDirectory(*dnp, path, rp, fl, offset+delt, delt);
   fl->NumCols = 1;
   fl->NumRows = fl->NumEntries;
   *tb = BA_tb; *nd = BA_nd; *nf = BA_nf;
quit:
   if(BA_fib)
      FreeMem((char *)BA_fib, sizeof(struct FileInfoBlock));
   return(error);
}

static VBS_error_t BA_ScanDirectory(dn, dirname, rp, fl, offset, delt)
VBS_DirNode_t   *dn;
char            *dirname;
struct RastPort *rp;
VBS_ListData_t  *fl;
int              offset, delt;
{
   BPTR             lock, oldlock;
   VBS_DirNode_t   *dnode, *prevdnode = NULL;
   VBS_FileNode_t  *fnode, *prevfnode = NULL;
   int              l, tl;
   VBS_error_t      error = EOK;

   lock = Lock(dirname, ACCESS_READ);

   if(!lock || !Examine(lock, BA_fib) || BA_fib->fib_EntryType <= 0) {
      error = EREAD;
      goto quit;
   }

   while(ExNext(lock, BA_fib)) {
      l  = strlen(BA_fib->fib_FileName);
      tl = TextLength(rp, BA_fib->fib_FileName, l);
      if(BA_fib->fib_EntryType < 0) {
      /* file */
         fnode = (VBS_FileNode_t *)AllocMem(sizeof(VBS_FileNode_t)+l, MEMF_CLEAR);
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

         fnode->Flags      = (BA_fib->fib_Protection & 8) ? 0:ERRF_INCLUDE;
         fnode->TextLength = tl;
         fnode->Size       = BA_fib->fib_Size;
         fnode->Protection = BA_fib->fib_Protection;
         fnode->Date       = BA_fib->fib_Date;
         fnode->NameLen    = l;
         strcpy(fnode->Name, BA_fib->fib_FileName);
         BA_tb+=BA_fib->fib_Size;
         BA_nf++;
      } else {
      /* directory */
         dnode = (VBS_DirNode_t *)AllocMem(sizeof(VBS_DirNode_t)+l, MEMF_CLEAR);
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
         dnode->ParentDir = dn;
         dnode->TextLength = tl;
         dnode->Protection = BA_fib->fib_Protection;
         dnode->Date       = BA_fib->fib_Date;
         dnode->NameLen    = l;
         strcpy(dnode->Name, BA_fib->fib_FileName);

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

   while(dn) {
      fn = dn->FirstFile;
      while(fn) {
         fn2 = fn->Next;
         FreeMem((char *)fn, sizeof(VBS_FileNode_t)+fn->NameLen);
         fn = fn2;
      }
      BAG_FreeTree(dn->FirstDir);
      dn2 = dn->Next;
      FreeMem((char *)dn, sizeof(VBS_DirNode_t)+dn->NameLen);
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
      if(BA_memptr + l >= BA_mem + BA_memsize) {
         *BA_memptr = 0x00;
         Delay(1);Forbid();
         VBS_WriteMem(BA_mem, BA_memsize, NUM_REPEAT);
         Permit();
         Delay(BA_nopen/10); BA_nopen = 0;
         BA_memptr = BA_mem;
      }
      fn  = dn->FirstFile;
      dn2 = dn->FirstDir;
      dn->FirstDir  = (VBS_DirNode_t  *)BA_NumDirsIncluded(dn2);
      dn->FirstFile = (VBS_FileNode_t *)BA_NumFilesIncluded(fn);

      memcpy(BA_memptr, (char *)dn, l); BA_nopen++;
      dn->FirstDir  = dn2;
      dn->FirstFile = fn;
      *BA_memptr = l;
      BA_memptr += l;
      while(fn) {
         if(!(fn->Flags & ERRF_INCLUDE))
            goto nextfile;
         l = sizeof(VBS_FileNode_t)+fn->NameLen;
         if(BA_memptr + l >= BA_mem + BA_memsize) {
            *BA_memptr = 0x00;
            Delay(1);Forbid();
            VBS_WriteMem(BA_mem, BA_memsize, NUM_REPEAT);
            Permit();
            Delay(BA_nopen/10); BA_nopen = 0;
            BA_memptr = BA_mem;
         }
         memcpy(BA_memptr, (char *)fn, l); BA_nopen++;
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

static VBS_error_t      BA_BackupFiles(dn, dirname)
VBS_DirNode_t  *dn;
char           *dirname;
{
   BPTR             lock, oldlock, fn;
   VBS_DirNode_t   *dnode;
   VBS_FileNode_t  *fnode;
   long             fs, rest;
   VBS_error_t      error = EOK;

   lock = Lock(dirname, ACCESS_READ);

   if(!lock) {
      error = EREAD;
      goto quit;
   }
   oldlock = CurrentDir(lock);
   BA_filedirdelay += VBSG_perdir;


   fnode = dn->FirstFile;
   while(fnode) {
      if(!(fnode->Flags & ERRF_INCLUDE))
         goto nextfile;
      if(LPRESSED) {
         error = EBREAK; goto quit;
      }
      fn = Open(fnode->Name, MODE_OLDFILE);
      BA_filedirdelay += VBSG_perfile;
      if(!fn) {
         error = EREAD;
         goto quit;
      }
      fs = fnode->Size;
      rest = BA_mem + BA_memsize - BA_memptr;
      while(fs > rest) {
         if(rest) {
            if(Read(fn, BA_memptr, rest) != rest) {
               error = EREAD; goto quit;
            }
         }
         fs-=rest; rest = BA_memsize; BA_memptr = BA_mem;

         BA_duration = (VBS_ReadTimer() - BA_starttime) & 0x00FFFFFF;

         BA_wantedduration -= BA_duration;
         if(BA_wantedduration < 0)
            BA_wantedduration = 0;
         BAG_DelayJiffies(1+BA_wantedduration);
         if(LPRESSED) {
            Close(fn);
            error = EBREAK; goto quit;
         }
         Forbid();
         VBS_WriteMem(BA_mem, BA_memsize, NUM_REPEAT);
         BA_wantedduration = (BA_duration*VBSG_writefactor)/100 +
                              BA_filedirdelay;
         BA_filedirdelay   = 0;
         Permit();
         BA_starttime = VBS_ReadTimer();
      }
      if(fs) {

         if(Read(fn, BA_memptr, fs)!=fs) {
            error = EREAD; goto quit;
         }
         BA_memptr+=fs;
      }
      Close(fn);
      if(BA_archive)
         SetProtection(fnode->Name, fnode->Protection|FIBF_ARCHIVE);
nextfile:

      fnode = fnode->Next;
   }
   dnode = dn->FirstDir;

   while(dnode && !error) {
      error = BA_BackupFiles(dnode, dnode->Name);
      dnode = dnode->Next;
   }
quit:
   if(lock) {
      CurrentDir(oldlock);
      UnLock(lock);
   }
   return(error);
}
VBS_error_t     BAG_RestoreFilesTo(dn, path, buffer, bl)
VBS_DirNode_t  *dn;
char           *path;
char           *buffer;
long            bl;
{
   BPTR        lock = NULL;
   VBS_error_t error = EOK;
   int         l;

   BA_mem = buffer;
   if(BA_SetFileDateInit(path)) {
      error = EMEMORY;
      goto quit;
   }
   BA_memsize = bl;
   BA_memptr = BA_mem;
   lock = Lock(path, ACCESS_READ);
   if(!lock) {
      error = EREAD;
      goto quit;
   }
   sprintf(BA_msg, RESTORINGTOTX " %s", path);
   BAG_PrintMsg(BA_msg);
   BAG_PrintMsg(PRESSPLAYTX);
   strcpy(BA_path, path); l = strlen(BA_path);
   if(l && BA_path[l-1]!=':') {
      strcpy(BA_path+l, "/");
   }
   error = VBS_SearchFirstBlockSync();
   if(error)
      goto quit;
   BA_memptr = BA_mem+bl; BA_nerr = 0;
   error = BA_RestoreFiles(dn, lock);
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
   BAG_PrintMsg(PRESSSTOPTX);
   BAG_PrintMsg(PRESSLMBTX);
   while(!LPRESSED) {
      Delay(2);
   }
quit:
   BA_SetFileDateCleanup();
   BA_mem = NULL;
   return(error);
}

static VBS_error_t    BA_RestoreFiles(dn, dlock)
VBS_DirNode_t  *dn;
BPTR            dlock;
{
   BPTR             lock, oldlock, fn;
   VBS_DirNode_t   *dnode;
   VBS_FileNode_t  *fnode;
   long             fs, rest;
   VBS_error_t      error = EOK;
   BOOL             created_dir;
   char            *appendpath;

   appendpath    = BA_path+strlen(BA_path);
   BA_appendpath = appendpath;
/*   if(!dlock) {
      error = EREAD;
      goto quit;
   }*/
   if(dlock >0)
      oldlock = CurrentDir(dlock);

   fnode = dn->FirstFile;
   while(fnode) {
      rest = BA_mem + BA_memsize - BA_memptr;
      fs   = fnode->Size;
      lock = NULL;
      if(dlock <= 0 || !(fnode->Flags & ERRF_INCLUDE) ||
            ((lock = Lock(fnode->Name, ACCESS_READ))>0)) {
         if(lock>0) {
            BA_PrintMsgPathFile(FILEEXISTSTX, fnode->Name);
            UnLock(lock);
            fnode->Flags = ERRF_INCLUDE|ERRF_FILEEXISTSERROR;
         }
         while(fs > rest) {
            fs-=rest; rest = BA_memsize; BA_memptr = BA_mem;
            Delay(8);
            if(LPRESSED) {
               error = EBREAK; goto quit;
            }
            Forbid();
            VBS_ReadMem(BA_mem, BA_memsize);
            Permit();
         }
         BA_memptr+=fs;
      } else {
         if(LPRESSED) {
            error = EBREAK; goto quit;
         }
         fn = Open(fnode->Name, MODE_NEWFILE);
         if(!fn) {
            BA_PrintMsgPathFile(COULDNTOPENTX" %s", fnode->Name);
            error = EWRITE;
            goto quit;
         }
         while(fs > rest) {
            if(rest) {
               if(fn && fnode->Flags == ERRF_INCLUDE) {
                  if(VBS_CheckInterval(BA_memptr-BA_mem, rest)) {
                     BA_PrintMsgPathFile(VIDEOERRTX" %s", fnode->Name);

                     fnode->Flags |= ERRF_READERROR; BA_nerr++;
/*                     Close(fn); fn = NULL;
                     DeleteFile(fnode->Name);*/
                  }
               }
               if(fn && Write(fn, BA_memptr, rest) != rest) {
                  BA_PrintMsgPathFile(WRITEERRTX " %s!", fnode->Name);
                  error = EWRITE; goto quit;
               }
            }
            fs-=rest; rest = BA_memsize; BA_memptr = BA_mem;
            Delay(8);Forbid();
            VBS_ReadMem(BA_mem, BA_memsize);
            Permit();
         }
         if(fs) {
            if(fn && fnode->Flags == ERRF_INCLUDE) {
               if(VBS_CheckInterval(BA_memptr-BA_mem, fs)) {
                  BA_PrintMsgPathFile(VIDEOERRTX " %s", fnode->Name);

                  fnode->Flags |= ERRF_READERROR; BA_nerr++;
/*                  Close(fn); fn = NULL;
                  DeleteFile(fnode->Name);*/
               }
            }
            if(fn && Write(fn, BA_memptr, fs)!=fs) {
               BA_PrintMsgPathFile(WRITEERRTX " %s!", fnode->Name);
               error = EWRITE; goto quit;
            }
            BA_memptr+=fs;
         }
         if(fn) {
            Close(fn);
            BA_SetDateProt(dlock, fnode->Name, fnode->NameLen, &fnode->Date,
                           fnode->Protection);
            fn = NULL;
         }
      }
      fnode = fnode->Next;
   }
   dnode = dn->FirstDir;

   while(dnode &&!error) {
      lock = NULL; created_dir = FALSE;
      if(dlock>0 && dnode->Flags & ERRF_INCLUDE) {
         lock = CreateDir(dnode->Name);
         if(lock<=0) {
            lock = Lock(dnode->Name, ACCESS_READ);
            if(lock<=0) {
               BA_PrintMsgPathFile(COULDNTCREATETX" %s!", dnode->Name);
               error = EWRITE; goto quit;
            }
         } else {
            created_dir = TRUE;
         }
      }
      strcpy(appendpath, dnode->Name); strcat(appendpath, "/");
      error = BA_RestoreFiles(dnode, lock);
      *appendpath = 0;
      BA_appendpath = appendpath;
      if(created_dir)
         BA_SetDateProt(dlock, dnode->Name, dnode->NameLen, &dnode->Date,
                        dnode->Protection);
      if(error)
         goto quit;
      dnode = dnode->Next;
   }
quit:
   if(dlock > 0) {
      CurrentDir(oldlock);
      UnLock(dlock);
   }
   return(error);
}
VBS_error_t     BAG_VerifyFilesFrom(dn, path, buffer1, buffer2, bl)
VBS_DirNode_t  *dn;
char           *path;
char           *buffer1, *buffer2;
long            bl;
{
   BPTR        lock = NULL;
   VBS_error_t error = EOK;
   int         l;

   BA_mem     = buffer1;
   BA_cmpbuf  = buffer2;
   BA_memsize = bl;
   BA_memptr  = BA_mem;
   lock = Lock(path, ACCESS_READ);
   if(!lock) {
      error = EREAD;
      goto quit;
   }
   sprintf(BA_msg, VERIFYINGFROMTX " %s", path);
   BAG_PrintMsg(BA_msg);
   BAG_PrintMsg(PRESSPLAYTX);
   strcpy(BA_path, path); l = strlen(BA_path);
   if(l && BA_path[l-1]!=':') {
      strcpy(BA_path+l, "/");
   }
   BA_memptr = BA_mem+bl; BA_nerr = 0;
   error = VBS_SearchFirstBlockSync();
   if(error)
      goto quit;
   error = BA_VerifyFiles(dn, lock);
   if(error) {
      BAG_PrintMsg(FATALERRTX);
   } else {
      if(BA_nerr) {
         sprintf(BA_msg, "%ld "VERIFYERRSTX, BA_nerr);
         BAG_PrintMsg(BA_msg);
      } else {
         BAG_PrintMsg(VERIFYOKTX);
      }
   }
   BAG_PrintMsg(PRESSSTOPTX);
   BAG_PrintMsg(PRESSLMBTX);
   while(!LPRESSED) {
      Delay(2);
   }
quit:
   BA_mem = NULL; BA_cmpbuf = NULL;
   return(error);
}

static VBS_error_t    BA_VerifyFiles(dn, dlock)
VBS_DirNode_t  *dn;
BPTR            dlock;
{
   BPTR             lock, oldlock, fn;
   VBS_DirNode_t   *dnode;
   VBS_FileNode_t  *fnode;
   long             fs, rest;
   VBS_error_t      error = EOK;
   BOOL             created_dir;
   char            *appendpath;

   appendpath    = BA_path+strlen(BA_path);
   BA_appendpath = appendpath;

   if(dlock >0)
      oldlock = CurrentDir(dlock);

   fnode = dn->FirstFile;
   while(fnode) {
      rest = BA_mem + BA_memsize - BA_memptr;
      fs   = fnode->Size;
      fn   = -1;
      if(LPRESSED) {
         error = EBREAK;
         goto quit;
      }
      if(dlock <= 0 || !(fnode->Flags & ERRF_INCLUDE) ||
            (!(fn = Open(fnode->Name, MODE_OLDFILE)))) {
         if(!fn) {
            fnode->Flags = ERRF_INCLUDE|ERRF_FILENOTEXISTSERROR;
            BA_PrintMsgPathFile(FILENOTEXISTSTX, fnode->Name);
         }
         while(fs > rest) {
            fs-=rest; rest = BA_memsize; BA_memptr = BA_mem;
            Delay(8);
            if(LPRESSED) {
               error = EBREAK;
               goto quit;
            }
            Forbid();
            VBS_ReadMem(BA_mem, BA_memsize);
            Permit();
         }
         BA_memptr+=fs;
      } else {
         /* dlock >0 && fnode->Flags & ERRF_INCLUDE && fn */
         while(fs > rest) {
            if(rest) {
               if(fnode->Flags == ERRF_INCLUDE) {
                  if(VBS_CheckInterval(BA_memptr-BA_mem, rest)) {
                     BA_PrintMsgPathFile(VIDEOERRTX " %s", fnode->Name);

                     fnode->Flags |= ERRF_READERROR; BA_nerr++;
                  }
               }
               if(fnode->Flags == ERRF_INCLUDE) {
                  if(BA_Verify(fn, BA_memptr, rest) != rest) {
                     BA_PrintMsgPathFile(VERIFYERRTX " %s!", fnode->Name);
                     fnode->Flags |= ERRF_VERIFYERROR; BA_nerr ++;
                  }
               }
            }
            fs-=rest; rest = BA_memsize; BA_memptr = BA_mem;
            Delay(8);Forbid();
            VBS_ReadMem(BA_mem, BA_memsize);
            Permit();
         }
         if(fs) {
            if(fnode->Flags == ERRF_INCLUDE) {
               if(VBS_CheckInterval(BA_memptr-BA_mem, fs)) {
                  BA_PrintMsgPathFile(VIDEOERRTX " %s", fnode->Name);

                  fnode->Flags |= ERRF_READERROR; BA_nerr++;
               }
            }
            if(fnode->Flags == ERRF_INCLUDE) {
               if(BA_Verify(fn, BA_memptr, fs) != fs) {
                  BA_PrintMsgPathFile(VERIFYERRTX " %s!", fnode->Name);
                  fnode->Flags |= ERRF_VERIFYERROR; BA_nerr++;
               }
            }
            BA_memptr+=fs;
         }
         Close(fn);
      }
      fnode = fnode->Next;
   }
   dnode = dn->FirstDir;

   while(dnode &&!error) {
      lock = NULL; created_dir = FALSE;
      if(dlock>0 && dnode->Flags & ERRF_INCLUDE) {
         lock = Lock(dnode->Name, ACCESS_READ);
         if(!lock) {
            BA_PrintMsgPathFile(DIRNOTEXISTS, dnode->Name);
            dnode->Flags = ERRF_INCLUDE|ERRF_FILENOTEXISTSERROR;
         }
      }
      strcpy(appendpath, dnode->Name);strcat(appendpath, "/");
      error = BA_VerifyFiles(dnode, lock);
      *appendpath   = 0;
      BA_appendpath = appendpath;
      if(error)
         goto quit;
      dnode = dnode->Next;
   }
quit:
   if(dlock > 0) {
      CurrentDir(oldlock);
      UnLock(dlock);
   }
   return(error);
}

VBS_error_t   BAG_RestoreTree(dnp, buffer, bl)
VBS_DirNode_t **dnp;
char           *buffer;
long            bl;
{
   VBS_error_t    error = EOK;

   BA_mem     = buffer;
   BA_memsize = bl;
   BA_memptr  = BA_mem;

   BAG_PrintMsg(READINGTREETX);
   Delay(8);Forbid();
   VBS_ReadMem(BA_mem, bl);
   Permit();
   BA_memlimit = BA_mem+VBS_FindMemLimit();
   if(BA_mem >= BA_memlimit) {
      error = EREAD; goto quit;
   }
   error = BA_RestoreDirectory(dnp, NULL, 1);
quit:
   BA_mem = NULL;
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
         Delay(8);Forbid();
         VBS_ReadMem(BA_mem, BA_memsize);
         Permit();
         BA_memlimit = BA_mem + VBS_FindMemLimit();
         if(BA_mem >= BA_memlimit) {
            error = EREAD;
            break;
         }
         BA_memptr = BA_mem; l = *BA_memptr;
      }
      if(BA_memptr+l >= BA_memlimit) {
         error = EREAD;
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
      memcpy((char *)dn, BA_memptr, l);  BA_memptr+=l;
      dn->Next      = NULL;
      dn->ParentDir = parent;
      numfiles = (int)dn->FirstFile;
      fnp = NULL;
      for(file = 0; file<numfiles; file++) {
            l = *BA_memptr;
            if(!l) {
               Delay(8);Forbid();
               VBS_ReadMem(BA_mem, BA_memsize);
               Permit();
               BA_memlimit = BA_mem + VBS_FindMemLimit();
               if(BA_mem >= BA_memlimit) {
                  error = EREAD;
                  goto error;
               }
               BA_memptr = BA_mem; l = *BA_memptr;
            }
            if(BA_memptr+l >= BA_memlimit) {
               error = EREAD;
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
            memcpy((char *)fn, BA_memptr, l);  BA_memptr+=l;
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
   BA_tb = 0; BA_nd = 0; BA_nf = 0;
   BA_PrepareDirectory(dn, rp, dl, offset, delt);
   dl->NumCols = 1;
   dl->NumRows = dl->NumEntries = BA_nd;
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
      tl = dn->TextLength+offset;
      if(tl > dl->MaxTotalWidth)
         dl->MaxTotalWidth = tl;
      BA_nd++;
      fnode = dn->FirstFile;
      while(fnode) {
            fnode->TextLength = TextLength(rp, fnode->Name, fnode->NameLen);
            BA_tb+= fnode->Size;
            BA_nf++;
            fnode = fnode->Next;
      }
      BA_PrepareDirectory(dn->FirstDir, rp, dl, offset+delt, delt);
      dn = dn->Next;
   }
}
static   struct TextFont      *BA_largefont = NULL, *BA_smallfont = NULL;


#ifdef MOOIHEADER

VBS_error_t BAG_WriteHeader(type, name, path, date)
char               type;
char              *name, *path;
struct DateStamp  *date;
{
   struct BitMap     bm;
   VBS_error_t       error = EMEMORY;
   struct LayerInfo *li = NULL;
   struct Layer     *layer = NULL;
   struct RastPort  *rp;
   char              headerblock[64], datestring[20];
   int               i;
   struct TextFont  *oldfont = NULL;

   memset((char *)&bm, 0, sizeof bm);
   InitBitMap(&bm, 1, HBMWIDTH, HBMHEIGHT);
   if(!(bm.Planes[0] = AllocRaster(HBMWIDTH, HBMHEIGHT)))
      goto quit;

   li = NewLayerInfo();
   if(!li)
      goto quit;
   if(!(layer = CreateUpfrontLayer(li, &bm,
      0, 0, HBMWIDTH-1, HBMHEIGHT-1,
      LAYERSIMPLE,NULL)))   /*layersimple*/
         goto quit;
   rp = layer->rp;
   oldfont = rp->Font;
   SetRast(rp, 0);
   SetFont(rp, BA_largefont);
   Move(rp, 40, rp->TxHeight);

   Text(rp, name, strlen(name));

   Delay(20);i = 7;

   SetFont(rp, BA_smallfont);
   Move(rp, 40, rp->cp_y+rp->TxHeight+16);
/*   ds = (struct DateStamp *)(headerblock+(64-sizeof(struct DateStamp)));
   *ds = *date; */
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

   VBS_ShowPlane(bm.Planes[0], HBMWIDTH, HBMHEIGHT, 107+4+8, 60, HIRES|0x1000,
      0x000, 0xFFF);

   Delay(TICKS_PER_SECOND*4);
   memset(headerblock, 0, sizeof headerblock);
   strcpy(&headerblock[1], name);
   headerblock[0] = type;

   Forbid();
   VBS_WriteBackupHeader(headerblock);
   Permit();
   error =  EOK;
quit:
   if(oldfont)
      SetFont(rp, oldfont);
   if(layer) {
      DeleteLayer(li, layer);
      DisposeLayerInfo(li);
   }

   if(bm.Planes[0])
      FreeRaster(bm.Planes[0], HBMWIDTH, HBMHEIGHT);
   bm.Planes[0] = NULL;
   return(error);
}
#else


VBS_error_t BAG_WriteHeader(type, name, path, date)
char               type;
char              *name, *path;
struct DateStamp  *date;
{
   struct BitMap     bm;
   VBS_error_t       error = EMEMORY;
   struct LayerInfo *li = NULL;
   struct Layer     *layer = NULL;
   struct RastPort   rps, *rp;
   char              headerblock[64], datestring[20];
   int               i;
   struct TextFont  *oldfont = NULL;

   InitBitMap(&bm, 1, HBMWIDTH, HBMHEIGHT);
   if(!(bm.Planes[0] = AllocRaster(HBMWIDTH, HBMHEIGHT)))
      goto quit;
   rp = &rps;
   InitRastPort(rp); rp->BitMap = &bm;
   SetAPen(rp, 1);

   SetRast(rp, 0);
   SetFont(rp, BA_largefont);
   Move(rp, 40, rp->TxHeight);

   i = strlen(name);
   while(TextLength(rp, name, i) > (HBMWIDTH-60))
      i--;
   Text(rp, name, i);

   Delay(20);i = 7;

   SetFont(rp, BA_smallfont);
   Move(rp, 40, rp->cp_y+rp->TxHeight+16);
/*   ds = (struct DateStamp *)(headerblock+(64-sizeof(struct DateStamp)));
   *ds = *date; */
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

   VBS_ShowPlane(bm.Planes[0], HBMWIDTH, HBMHEIGHT, 107+4+8, 60, HIRES|0x1000,
      0x000, 0xFFF);

   Delay(TICKS_PER_SECOND*4);
   memset(headerblock, 0, sizeof headerblock);
   strcpy(&headerblock[1], name);
   headerblock[0] = type;

   Forbid();
   VBS_WriteBackupHeader(headerblock);
   Permit();
   error =  EOK;
quit:
   if(bm.Planes[0])
      FreeRaster(bm.Planes[0], HBMWIDTH, HBMHEIGHT);
   bm.Planes[0] = NULL;
   return(error);
}
#endif

VBS_error_t BAG_Init(largename, largesize, smallname, smallsize)
char     *largename, *smallname;
int       largesize,  smallsize;
{
   if(!BA_largefont) {
      BA_largefont = UIG_OpenFont(largename, largesize);
      BA_smallfont = UIG_OpenFont(smallname, smallsize);
   }
   VBS_VIInit(NULL, NULL, NULL, NULL);
   return(EOK);
}
void  BAG_Cleanup()
{
   if(BA_largefont) {
      CloseFont(BA_largefont);
      CloseFont(BA_smallfont);
   }
}
static struct RastPort  *BA_msgrp;
static int               BA_msgy, BA_msgwidth, BA_msgheight;

void  BAG_SetupScreen(s)
struct Screen  *s;
{
   BA_msgrp = &s->RastPort;
   SetAPen(BA_msgrp, 1);
   RectFill(BA_msgrp, 0, 11, s->Width-1, s->Height-1);
   SetAPen(BA_msgrp, 0);
   SetBPen(BA_msgrp, 1);
   SetDrMd(BA_msgrp, JAM1);
   BA_msgy = 19; BA_msgheight = s->Height; BA_msgwidth = s->Width;

   VBS_ShowPlane(s->BitMap.Planes[0],
      s->Width, s->Height,
      GfxBase->ActiView->DxOffset, GfxBase->ActiView->DyOffset,
      /*s->ViewPort.Modes & 0x9FFF*/HIRES|0x1000 /*eenplanig*/,
      GetRGB4(s->ViewPort.ColorMap, 0),
      GetRGB4(s->ViewPort.ColorMap, 1));
}
void BAG_SetupVideoScreen(void)
{
}
void BAG_RestoreScreen(void)
{
   VBS_RestoreScreen();
}

BOOL     BAG_CheckConnection(w)
struct Window  *w;
{
   BOOL     v = TRUE;

   if(!VBSG_novideocheck) {
      do {
         BAG_SetupVideoScreen();
         v = VBS_CheckLoopback();
         BAG_RestoreScreen();
      } while(!v && ERRG_Message(w, EVIDEOCONNECTION)!=ERROR_CANCEL);
   }
   return(v);
}

void BAG_PrintMsg(msg)
char  *msg;
{
   if(BA_msgy > BA_msgheight) {
      ScrollRaster(BA_msgrp, 0, 8, 0, 11, BA_msgwidth-1, BA_msgheight-1);
      BA_msgy-=8;
   }
   Move(BA_msgrp, 4, BA_msgy); Text(BA_msgrp, msg, strlen(msg));
   BA_msgy+=8;
}

void BAG_ShowMessage(s, message1, message2, c1, c2)
struct Screen  *s;
char           *message1, *message2;
UWORD           c1,c2;
{
   struct RastPort   *rp;
   struct TextFont   *font;

   rp = &s->RastPort;
   SetRast(rp, 0);
   SetAPen(rp, 1);
   font = rp->Font;
   SetFont(rp, BA_smallfont);
   if(message1) {
      Move(rp, 20, 100);
      Text(rp, message1, strlen(message1));
   }
   if(message2) {
      Move(rp, 20, 160);
      Text(rp, message2, strlen(message2));
   }
   SetFont(rp, font);
   VBS_ShowPlane(s->BitMap.Planes[0],
      s->Width, s->Height,
      GfxBase->ActiView->DxOffset, GfxBase->ActiView->DyOffset,
      HIRES|0x1000 /*eenplanig*/,
      c1, c2);

}

VBS_error_t  BAG_Search(type, nameprefix, fullname)
char      type;
char     *nameprefix;
char     *fullname;
{
   char    *headerblock=NULL, *p, c;
   int      len;
   BOOL     ok = FALSE;
   static char *backuptype[] = {
      "Unknown", "FileSystem", "AmigaDOS floppy", "MS-DOS floppy"
   };

   len = strlen(nameprefix);
   sprintf(BA_msg, SEARCHINGFORTX " %s: %s", backuptype[type], nameprefix);
   BAG_PrintMsg(BA_msg);
   BAG_PrintMsg("Press PLAY");
   do {
      headerblock = VBS_Search();
      if(!headerblock) {
         BAG_PrintMsg(SEARCHERRTX);
         return(ESEARCH);
      }
      c = headerblock[1+len];
      headerblock[1+len] = '\0';
      if(headerblock[0]!=type || StrCmpNoCase(headerblock+1, nameprefix)) {
         p = ENCOUNTEREDTX;
      } else {
         p = FOUNDTX; ok = TRUE;
      }
      headerblock[1+len] = c;
      sprintf(BA_msg, "%s %s: %s", p, backuptype[headerblock[0]],
              headerblock+1);
      if(fullname)
         strcpy(fullname, headerblock+1);
      BAG_PrintMsg(BA_msg);
   } while(!ok);
   return(EOK);
}

static BOOL BA_SetFileDateInit(path)
char     *path;
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
   memset((char *)&BA_packetst, 0, sizeof(struct StandardPacket));
   BA_packetst.sp_Msg.mn_Node.ln_Name = (char *)&BA_packet->sp_Pkt;
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
      FreeMem((char *)BA_packet, sizeof(struct StandardPacket));
   BA_bstrpt = NULL;
   BA_packet = NULL;
}
static void    BA_SetDateProt(dlock, name, namelen, date, prot)
BPTR               dlock;
char              *name;
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
char     *buffer;
register long      len;
{
   register long     reallen,d;

   /* len > 0 */
   d = (long)buffer&3;

   *(long *)(BA_cmpbuf+((len+d-1) & 0xFFFC)) = *(long *)((long)(buffer+len-1) & 0xFFFFFFFC);
   *(long *)(BA_cmpbuf) = *(long *)(buffer-d);

   reallen = Read(fn, BA_cmpbuf+d, len);
   if(reallen < len)
      return(reallen);
   return(VBS_MemCmpL(BA_cmpbuf, buffer-d, (d+len+3)>>2) ? 0 : len);
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

static void  BA_PrintMsgPathFile(s, name)
char     *s, *name;
{
   strcpy(BA_appendpath, name);
   sprintf(BA_msg, s, BA_path);
   BAG_PrintMsg(BA_msg);
   *BA_appendpath = '\0';
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

