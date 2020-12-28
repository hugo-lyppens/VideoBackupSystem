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

#include  <resources/disk.h>
#include  <hardware/intbits.h>
#include  "vbs.h"
#include  "rs.h"
#include  "LD_vbstext.h"
#include  "simpleview.h"

#define DEFWRITEFACTOR     150
#define DEFPERFILE         6
#define DEFPERDIR          25

void			 stub(void);

static void              VBS_Cleanup(void);
static BOOL              VBS_Init(int, UBYTE **);

BOOL			 VBSG_beepafter	   = FALSE;
BOOL                     VBSG_usepubscreen = FALSE,
                         VBSG_closeworkbench = FALSE;
UBYTE                    VBSG_pubscreenname[64] = "\0";

UBYTE		         VBS_largefont[40] = "melba.font",
                         VBS_smallfont[40] = "melba.font",
                         VBS_padfont[40]   = "topaz.font",
                         VBS_textfont[40]  = "topaz.font";
int			             VBS_largesize = 61, VBS_smallsize = 29,
                         VBS_padsize = 8, VBS_textsize = 8;

struct TextAttr          VBSG_large_ta, VBSG_small_ta,
                         VBSG_pad_ta, VBSG_text_ta;

static UBYTE             VBS_template[] =
                            "PUBSCREEN/K,SETTINGS/K";

static UBYTE            *VBS_version = "$VER: VBS " VBSVER " (July 14, 1995)";

static struct Interrupt  VBS_vbi = {
   {
      NULL, NULL, NT_INTERRUPT, -60, NULL
   },
   NULL, VBS_VertBServer
};


BOOL                     VBSG_nochange          = FALSE;
BOOL                     VBSG_novideocheck      = FALSE;
BOOL                     VBSG_readdisplayblank  = FALSE;
int                      VBSG_writefactor       = DEFWRITEFACTOR;
int                      VBSG_perfile           = DEFPERFILE;
int                      VBSG_perdir            = DEFPERDIR;
UBYTE                    VBSG_logdrawer[256]    = DEFLOGDIR;
UBYTE                    VBSG_reportdrawer[256] = DEFREPORTDIR;
UBYTE                    VBSG_errorinfo[128];
UBYTE                    VBSG_prefsfile[256]    = "\0";
VBS_countertype_t        VBSG_countertype 		= TIMEDCT;
ULONG			 VBSG_displayid			= INVALID_ID;
ULONG			 VBSG_realdisplayid		= INVALID_ID;
UBYTE			 VBSG_extradelay		= 0;
BOOL			 VBSG_is68000			= FALSE;

extern struct WBStartup *_WBenchMsg;
extern struct ExecBase  *SysBase;
extern USHORT VBS_cprbetween[];

extern struct ExecBase *SysBase;
int VBS_hz, VBS_sd;
long        main(argc, argv)
int     argc;
UBYTE  *argv[];
{
   	struct WBStartup       *wbmsg;
	struct WBArg           *wbarg;
   	struct DiskObject      *diskobj;
   	struct RDArgs          *rdargs;
   	ULONG                   args[4] = { NULL, NULL, NULL, NULL };
   	UBYTE                  *toolstring;

   	UBYTE                  *initiallog = NULL;
   	BPTR                    olddir = NULL, newdir;
   	BOOL                    intserveradded = FALSE;
	VBS_error_t				error;
	char		       *cp;	
	int						i;

	if(argc==2 && !strcmp(argv[1], "hz"))
		VBS_hz = 1;
	if(argc==2 && !strcmp(argv[1], "spritedma"))
		VBS_sd = 1;
	if(argc==2 && !strcmp(argv[1], "68k"))
		VBSG_is68000 = TRUE;
   
   	if(!(SysBase->AttnFlags & AFF_68020))
   		VBSG_is68000 = TRUE;
#if 0
   	if(argc==2 && !strcmp(argv[1], "w")) {
		FB_TestWrite();return(0);
   	}
   	if(argc==2 && !strcmp(argv[1], "r")) {
		FB_TestRead(); return(0);
   	}
#endif
/*	while(!LPRESSED)
		;
	Delay(50);		
	while(LPRESSED)
		; */
			  	
   	if(VBS_Init(argc, argv))
      		goto quit;
      		
      	
      		
	if(!VBSG_is68000) {
   		if(RSG_Init())
      			goto quit;
      	}
#if 0
   tooltypes  = ArgArrayInit(argc, argv);

   toolstring = ArgString(tooltypes, SETTINGSTT, "\0");
   strcpy(VBSG_prefsfile, toolstring);

   toolstring = ArgString(tooltypes, PUBSCREENTT, "Workbench");
   strcpy(VBSG_pubscreenname, toolstring);

   toolint    = ArgInt(tooltypes, WRITEFACTORTT, DEFWRITEFACTOR);

   toolint    = ArgInt(tooltypes, PERFILETT, DEFPERFILE);

   toolint    = ArgInt(tooltypes, PERDIRTT, DEFPERDIR);

   toolstring = ArgString(tooltypes, READDISPLAYBLANKTT, OFFTT);
   if(!strcmp(toolstring, ONTT))
      VBSG_readdisplayblank = TRUE;

   ArgArrayDone();

   if(!argc)
   {
      if(_WBenchMsg->sm_NumArgs >= 2 &&
         (newdir = _WBenchMsg->sm_ArgList[1].wa_Lock))
#endif
   
   	if(!argc)
 	{
      	wbmsg = (struct WBStartup *)argv;
      	wbarg = wbmsg->sm_ArgList;
       	if((*wbarg->wa_Name) && (diskobj = GetDiskObject(wbarg->wa_Name)))
       	{
       		if(toolstring = FindToolType(diskobj->do_ToolTypes, PUBSCREENTT))
      			strcpy(VBSG_pubscreenname, toolstring);
           	else
           		strcpy(VBSG_pubscreenname, "Workbench");

          	if(toolstring = FindToolType(diskobj->do_ToolTypes, SETTINGSTT))
           		strcpy(VBSG_prefsfile, toolstring);
          	else
           		VBSG_prefsfile[0] = '\0';


	      	cp = FindToolType(diskobj->do_ToolTypes, WRITEFACTORTX);
      		if(cp && (i = atoi(cp))>DEFWRITEFACTOR)
         		VBSG_writefactor = i;

      		cp = FindToolType(diskobj->do_ToolTypes, PERFILETX);
      		if(cp && (i = atoi(cp))>DEFPERFILE)
         		VBSG_perfile = i;

      		cp = FindToolType(diskobj->do_ToolTypes, PERDIRTX);
      		if(cp && (i = atoi(cp))>DEFPERDIR)
         		VBSG_perdir = i;

			cp = FindToolType(diskobj->do_ToolTypes, READDISPLAYBLANKTX);
      		VBSG_readdisplayblank = cp && !strcmp(cp, "on");

          	FreeDiskObject(diskobj);
      	}

		if(wbmsg->sm_NumArgs >= 2 && (newdir = wbmsg->sm_ArgList[1].wa_Lock))
      	{
        	olddir     = CurrentDir(newdir);
            initiallog = wbmsg->sm_ArgList[1].wa_Name;
      	}
   	}
   	else
   	{
      	if(rdargs = ReadArgs(VBS_template, args, NULL))
      	{
         	if(args[0])
            	strcpy(VBSG_pubscreenname, (UBYTE *)args[0]);
         	else
            	strcpy(VBSG_pubscreenname, "Workbench");

         	if(args[1])
            	strcpy(VBSG_prefsfile, (UBYTE *)args[1]);
         	else
            	VBSG_prefsfile[0] = '\0';

         	FreeArgs(rdargs);
		}
	}
   	AddIntServer(INTB_VERTB, &VBS_vbi); intserveradded = TRUE;

   	error = TRUE;if(*VBSG_prefsfile)
   		error = SETG_LoadSettings();

   	if(error)
   	{
      	strcpy(VBSG_prefsfile, "PROGDIR:VBS.prefs");
      	error = SETG_LoadSettings();
   	}

   	if(error)
   	{
      	strcpy(VBSG_prefsfile, "ENV:VBS/VBS.prefs");
      	error = SETG_LoadSettings();
   	}

   	if(error)
      	strcpy(VBSG_prefsfile, "PROGDIR:VBS.prefs");

   	UIG_MakeTextAttr(&VBSG_large_ta, VBS_largefont, VBS_largesize);

   	UIG_MakeTextAttr(&VBSG_small_ta, VBS_smallfont, VBS_smallsize);

   	UIG_MakeTextAttr(&VBSG_pad_ta,   VBS_padfont,   VBS_padsize);

   	UIG_MakeTextAttr(&VBSG_text_ta,  VBS_textfont,  VBS_textsize);

   	if(BAG_Init())
      		goto quit;
/*   VBS_VIInit(BAG_planes[0], BAG_planes[1], &BAG_view0, &BAG_view1);
   wordt nu vanuit BAG_Init aangeroepen */

   	while(VMG_Menu(initiallog, VBSG_logdrawer));
   	if(olddir) {
	      	CurrentDir(olddir);
   	}
quit:
   	if(intserveradded)
      	RemIntServer(INTB_VERTB, &VBS_vbi);
   	BAG_Cleanup();
   	OpenWorkBench();
	if(!VBSG_is68000) {
	   	RSG_Cleanup();
	}
   	VBS_Cleanup();
   	return(0L);
}
#if 0
static UBYTE *FGetsN(BPTR fh, UBYTE *buf, ULONG len)
{
   UBYTE   *cp;
   UBYTE   *result;

   result = FGets(fh, buf, len);
   if(result)
   {
      cp = stpchr(buf, '\n');
      if(cp)
         *cp = '\0';
   }
   return(result);
}

static void VBS_SeparateNameSize(UBYTE *s, UBYTE *name, int *size)
{
   UBYTE    *cp;

   strcpy(name, s);
   cp = stpchr(name, '-');
   if(cp) {
      *cp = 0; *size = atoi(cp+1);
   } else {
      *size = 8;
   }
}
#endif
/*struct GfxBase       *GfxBase;
struct Library       *LayersBase;
struct IntuitionBase *IntuitionBase;
struct Library       *DiskfontBase, *IconBase;*/
struct Library       *AslBase;
struct Library       *GadToolsBase;
struct DiskResource  *DRResource;

static BOOL   VBS_Init(argc, argv)
int   argc;
UBYTE *argv[];
{
   GfxBase       = (struct GfxBase *)OpenLibrary("graphics.library", 37L);
   LayersBase    = OpenLibrary("layers.library", 37L);
   IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37L);
   AslBase       = OpenLibrary("asl.library", 37L);
   GadToolsBase  = OpenLibrary("gadtools.library", 37L);
   DRResource    = (struct DiskResource *)OpenResource(DISKNAME);
   if(!argc)
      IconBase   = OpenLibrary("icon.library", 37L);
   return((BOOL)(!GfxBase || !IntuitionBase || !LayersBase ||
                 !AslBase || !GadToolsBase  || !DRResource));
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
   if(AslBase)
      CloseLibrary(AslBase);
   if(GadToolsBase)
      CloseLibrary(GadToolsBase);
}

/*void  XCEXIT()
{
}*/

void  stub()
{
}
