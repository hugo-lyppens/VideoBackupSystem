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

#include  <resources/disk.h>
#include  <hardware/intbits.h>
#include  "vbs.h"
#include  "LD_vbstext.h"

#define DEFWRITEFACTOR     150
#define DEFPERFILE         8
#define DEFPERDIR          25


static void              VBS_Cleanup(void);
static BOOL              VBS_Init(int, char **);
static void              VBS_SeparateNameSize(char *, char *, int *);

static char              VBS_logdrawer[50] = DEFLOGDIR;

static char              VBS_largefont[40] = "melba.font",
                         VBS_smallfont[40] = "melba.font",
                         VBS_padfont[40]   = "topaz.font";
static int               VBS_largesize = 61, VBS_smallsize = 29,
                         VBS_padsize = 8;

static struct Interrupt  VBS_vbi = {
   {
      NULL, NULL, NT_INTERRUPT, -60, NULL
   },
   NULL, VBS_VertBServer
};


BOOL                     VBSG_nochange         = FALSE;
BOOL                     VBSG_novideocheck     = FALSE;
BOOL                     VBSG_readdisplayblank = FALSE;
int                      VBSG_writefactor      = DEFWRITEFACTOR;
int                      VBSG_perfile          = DEFPERFILE;
int                      VBSG_perdir           = DEFPERDIR;
char                     VBSG_reportdrawer[50] = DEFREPORTDIR;

extern struct WBStartup *_WBenchMsg;

extern USHORT VBS_cprbetween[];

extern struct View    BAG_view0,BAG_view1;
extern PLANEPTR       BAG_planes[];

long        main(argc, argv)
int     argc;
char   *argv[];
{
   struct DiskObject *myobj;
   char              *cp;
   char              *initiallog = NULL;
   BPTR               olddir = NULL, newdir;
   int                i;
   BOOL               intserveradded = FALSE;

   if(VBS_Init(argc, argv))
      goto quit;
   VBSG_novideocheck = (argc == 2) && (!strcmp(argv[1], "nvc"));
   if(!argc) {
      myobj=GetDiskObject(_WBenchMsg->sm_ArgList->wa_Name);
      cp = FindToolType(myobj->do_ToolTypes, LOGDIRTX);
      if(cp && *cp)
         strcpy( VBS_logdrawer, cp );

      cp = FindToolType(myobj->do_ToolTypes, REPORTDIRTX);
      if(cp && *cp)
         strcpy( VBSG_reportdrawer, cp );

      cp = FindToolType(myobj->do_ToolTypes, LARGEFONTTX);
      if(cp && *cp) {
         VBS_SeparateNameSize(cp, VBS_largefont, &VBS_largesize);
      }
      cp = FindToolType(myobj->do_ToolTypes, SMALLFONTTX);
      if(cp && *cp) {
         VBS_SeparateNameSize(cp, VBS_smallfont, &VBS_smallsize);
      }
      cp = FindToolType(myobj->do_ToolTypes, PADFONTTX);
      if(cp && *cp) {
         VBS_SeparateNameSize(cp, VBS_padfont, &VBS_padsize);
      }
      cp = FindToolType(myobj->do_ToolTypes, WORKBENCHTX);
      if(cp && *cp && !strcmp(cp, "OFF")) {
         CloseWorkBench();
      }

      cp = FindToolType(myobj->do_ToolTypes, WRITEFACTORTX);
      if(cp && (i = atoi(cp))>DEFWRITEFACTOR)
         VBSG_writefactor = i;

      cp = FindToolType(myobj->do_ToolTypes, PERFILETX);
      if(cp && (i = atoi(cp))>DEFPERFILE)
         VBSG_perfile = i;

      cp = FindToolType(myobj->do_ToolTypes, PERDIRTX);
      if(cp && (i = atoi(cp))>DEFPERDIR)
         VBSG_perdir = i;

      cp = FindToolType(myobj->do_ToolTypes, DISKCHANGETX);
      VBSG_nochange         = cp && *cp && !strcmp(cp, OFFTX);

      cp = FindToolType(myobj->do_ToolTypes, VIDEOCHECKTX);
      VBSG_novideocheck     = cp && *cp && !strcmp(cp, OFFTX);

      cp = FindToolType(myobj->do_ToolTypes, READDISPLAYBLANKTX);
      VBSG_readdisplayblank = cp && *cp && !strcmp(cp, TTONTX);

      if(_WBenchMsg->sm_NumArgs >= 2 &&
         (newdir = _WBenchMsg->sm_ArgList[1].wa_Lock)) {
             olddir     = CurrentDir(newdir);
             initiallog = _WBenchMsg->sm_ArgList[1].wa_Name;
      }

      FreeDiskObject(myobj);
   }
   AddIntServer(INTB_VERTB, &VBS_vbi); intserveradded = TRUE;

   if(BAG_Init(VBS_largefont, VBS_largesize, VBS_smallfont, VBS_smallsize))
      goto quit;
/*   VBS_VIInit(BAG_planes[0], BAG_planes[1], &BAG_view0, &BAG_view1);
   wordt nu vanuit BAG_Init aangeroepen */

   VMG_Menu(initiallog, VBS_padfont, VBS_padsize, VBS_logdrawer);
   if(olddir) {
      CurrentDir(olddir);
   }
quit:
   if(intserveradded)
      RemIntServer(INTB_VERTB, &VBS_vbi);
   BAG_Cleanup();
   OpenWorkBench();
   VBS_Cleanup();
   return(0L);
}

static void VBS_SeparateNameSize(char *s, char *name, int *size)
{
   char     *cp;

   strcpy(name, s);
   cp = stpchr(name, '-');
   if(cp) {
      *cp = 0; *size = atoi(cp+1);
   } else {
      *size = 10;
   }
}

/*struct GfxBase       *GfxBase;
struct LayersBase    *LayersBase;
struct IntuitionBase *IntuitionBase;
struct Library       *DiskfontBase, *IconBase;*/
struct DiskResource  *DRResource;

static BOOL   VBS_Init(argc, argv)
int   argc;
char *argv[];
{
   GfxBase       = (struct GfxBase *)OpenLibrary("graphics.library", 0);
   LayersBase    = (struct LayersBase *)OpenLibrary("layers.library", 0);
   IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
   DRResource    = (struct DiskResource *)OpenResource(DISKNAME);
   if(!argc)
      IconBase   = OpenLibrary("icon.library",0);
   return((BOOL)(!GfxBase || !IntuitionBase || !LayersBase || !DRResource));
}

static void   VBS_Cleanup()
{
   if(GfxBase)
      CloseLibrary((struct Library *)GfxBase);
   if(LayersBase)
      CloseLibrary((struct Library *)LayersBase);
   if(IntuitionBase)
      CloseLibrary((struct Library *)IntuitionBase);
   if(DiskfontBase)
      CloseLibrary((struct Library *)DiskfontBase);
   if(IconBase)
      CloseLibrary((struct Library *)IconBase);
}

/*void  XCEXIT()
{
}*/

void  stub()
{
}
       /*
lul(s)
char *s;
{
   BPTR  fn;

   fn = Open("AUX:", MODE_NEWFILE);
   if(fn) {
      Write(fn, s, strlen(s));
      Close(fn);
   }
}
      */
