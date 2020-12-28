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

#include "vbs.h"
#include "LD_vbstext.h"

enum { DF0GAD = 0, DF1GAD, DF2GAD, DF3GAD, OKGAD, CANCELGAD, MULTIGAD,
       TIMEGAD, NAMEGAD };

static UBYTE          FL_pow2[4] = { 1, 2, 4, 8 };
static UBYTE          FL_avmask = 0;
static UBYTE          FL_namegadbuff[80];
static UBYTE          FL_timegadbuff[9];

extern VBS_screentype_t  VBSG_screentype;
extern struct TextAttr   VBSG_text_ta;
extern VBS_countertype_t VBSG_countertype;
extern UBYTE             VBSG_errorinfo[];

int FLG_AskFloppy(window, time, multiple, name, operation)
struct Window      *window;
ULONG              *time;
BOOL               *multiple;
UBYTE             **name;
VM_operationtype_t  operation;
{
   struct Window           *reqwin = NULL;
   struct Gadget           *gad, *multigad, *namegad, *timegad, *drivegad[4],
                           *glist = NULL;
   struct NewGadget         newgad;
   void                    *visinfo = NULL;
   struct IntuiMessage     *imsg;
   ULONG                    class;
   UWORD                    code;
   BOOL                     done = FALSE, driveexists, checkedone = FALSE;
   UBYTE                   *drivename[4] = { "DF_0:", "DF_1:", "DF_2:", "DF_3:" };
   int                      winwidth, winheight, twosp, ysize, ytop, tmp,
                            drive, numdrives, result = 0;
   ULONG                    gid;

   UIG_BlockInput(window);

   ysize     = VBSG_text_ta.ta_YSize;
   ytop      = window->WScreen->WBorTop +
               window->WScreen->Font->ta_YSize + 1;
   winwidth  = (window->Width >> 1);
   winheight = ysize*12 + (time?ysize*2:0) + (name?ysize*2:0);
   twosp     = TextLength(window->RPort, "hh", 2);

   if(!(visinfo = GetVisualInfo(window->WScreen, TAG_END)))
      goto quit;

   newgad.ng_Height     = ysize * 3/2;
   newgad.ng_TextAttr   = &VBSG_text_ta;
   newgad.ng_VisualInfo = visinfo;
   newgad.ng_Flags      = PLACETEXT_IN;

   gad = CreateContext(&glist);

   if(operation==BACKUPOT)
      newgad.ng_Width   = PIXLEN(window->RPort, STBACKUPTX) + twosp;
   else
      newgad.ng_Width   = PIXLEN(window->RPort, STRESTORETX) + twosp;
   newgad.ng_LeftEdge   = twosp;
   newgad.ng_TopEdge    = ytop + ysize*19/2 +
                          ysize*((time!=NULL?2:0) + (name!=NULL?2:0));
   newgad.ng_GadgetText = (operation==BACKUPOT?STBACKUPTX:STRESTORETX);
   newgad.ng_GadgetID   = OKGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   tmp = PIXLEN(window->RPort, CANCELTX) + twosp;
   newgad.ng_LeftEdge   = winwidth - tmp - twosp;
   newgad.ng_Width      = tmp;
   newgad.ng_GadgetText = CANCELTX;
   newgad.ng_GadgetID   = CANCELGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   tmp = PIXLEN(window->RPort, BACKUPNAMETX) + twosp;
   newgad.ng_LeftEdge   = tmp;
   newgad.ng_Flags      = PLACETEXT_LEFT;
   if(name)
   {
      newgad.ng_Width      = winwidth - tmp - twosp;
      newgad.ng_TopEdge    = ytop + ysize*19/2 + ysize*(time!=NULL?2:0);
      newgad.ng_GadgetText = BACKUPNAMETX;
      newgad.ng_GadgetID   = NAMEGAD;
      gad = CreateGadget(STRING_KIND, gad, &newgad,
                         GTST_MaxChars, 80,
                         GT_Underscore, '_',
                         TAG_END);
      namegad = gad;
   }

   if(time)
   {
      if(VBSG_countertype==TIMEDCT)
      {
         newgad.ng_Width      = PIXLEN(window->RPort, TIMEFORMATTX) + twosp*2;
         newgad.ng_GadgetText = TIMETX;
         strcpy(FL_timegadbuff, TIMEFORMATTX);
      }
      else
      {
         newgad.ng_Width      = PIXLEN(window->RPort, COUNTFORMATTX) + twosp*2;
         newgad.ng_GadgetText = COUNTERTX;
         strcpy(FL_timegadbuff, COUNTFORMATTX);
      }
      newgad.ng_LeftEdge   = tmp;
      newgad.ng_TopEdge    = ytop + ysize*15/2;
      newgad.ng_GadgetID   = TIMEGAD;
      gad = CreateGadget(STRING_KIND, gad, &newgad,
                         GTST_String,   FL_timegadbuff,
                         GTST_MaxChars, 8,
                         GT_Underscore, '_',
                         TAG_END);
      timegad = gad;
   }

   newgad.ng_TopEdge    = ytop + ysize*15/2 + ysize*(time!=NULL?2:0);
   newgad.ng_Width      = twosp;
   newgad.ng_Flags      = PLACETEXT_RIGHT;
   newgad.ng_GadgetText = MULTITX;
   newgad.ng_GadgetID   = MULTIGAD;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);
   multigad = gad;

   if(!FL_avmask)
   {
      for(drive = 0; drive < 4; drive++)
      {
         if(FLBG_DriveAvailable(drive))
         {
            FL_avmask |= FL_pow2[drive];
         }
      }
   }

   numdrives = 0;
   tmp = ysize*3/2;
   for(drive = 0; drive < 4; drive++)
   {
      driveexists = (FL_pow2[drive]&FL_avmask);
      newgad.ng_TopEdge    = ytop + ysize + drive*tmp;
      newgad.ng_GadgetText = drivename[drive];
      newgad.ng_GadgetID   = DF0GAD + drive;
      gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                         GTCB_Checked, (!checkedone && driveexists),
                         GA_Disabled,  (!driveexists),
                         GT_Underscore, '_',
                         TAG_END);
      drivegad[drive] = gad;
      if(!checkedone && driveexists)
      {
         checkedone = TRUE;
         numdrives++;
      }
   }

   if(!gad || numdrives!=1)
      goto quit;

   reqwin = OpenWindowTags(NULL,
                           (VBSG_screentype==PUBLICST?WA_PubScreen:WA_CustomScreen),
                                           window->WScreen,
                           WA_Activate,    TRUE,
                           WA_Left,        ((window->Width  - winwidth)  >> 1),
                           WA_Top,         ((window->Height - winheight) >> 1),
                           WA_InnerWidth,  winwidth,
                           WA_InnerHeight, winheight,
                           WA_AutoAdjust,  TRUE,
                           WA_DragBar,     TRUE,
                           WA_DepthGadget, TRUE,
                           WA_Title,       (operation==BACKUPOT?BACKUPFLOPPYTX:RESTOREFLOPPYTX),
                           WA_IDCMP,       (IDCMP_REFRESHWINDOW | IDCMP_VANILLAKEY | BUTTONIDCMP | CHECKBOXIDCMP),
                           WA_Gadgets,     glist,
                           TAG_END);

   if(!reqwin)
      goto quit;

   GT_RefreshWindow(reqwin, NULL);

   if(name)
      ActivateGadget(namegad, reqwin, NULL);

   while(!done)
   {
      Wait(1 << reqwin->UserPort->mp_SigBit);
      while(!done && (imsg = GT_GetIMsg(reqwin->UserPort)))
      {
         class = imsg->Class;
         code = imsg->Code;
         if(class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN)
         {
            gad = imsg->IAddress;
            gid = gad->GadgetID;
         }
         switch(imsg->Class)
         {
            case IDCMP_VANILLAKEY:
               gid = ~0;
               switch(code)
               {
                  case '0':
                  case '1':
                  case '2':
                  case '3':
                     drive = (int)(code - '0');
                     if(!(FL_avmask & FL_pow2[drive]))
                        break;
                     GT_SetGadgetAttrs(drivegad[drive], reqwin, NULL,
                                       GTCB_Checked, !(drivegad[drive]->Flags & GFLG_SELECTED),
                                       TAG_END);
                     gid = DF0GAD + drive;
                     break;
                  case 's':
                  case 'S':
                     gid = OKGAD;
                     break;
                  case 'n':
                  case 'N':
                     if(!name)
                        break;
                     ActivateGadget(namegad, reqwin, NULL);
                     break;
#ifdef DEUTSCH
                  case 'a':
                  case 'A':
                     gid = CANCELGAD;
                     break;
                  case 'd':
                  case 'D':
                     GT_SetGadgetAttrs(multigad, reqwin, NULL,
                                       GTCB_Checked, !(multigad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     gid = MULTIGAD;
                     break;
                  case 'z':
                  case 'Z':
                     if(!time)
                        break;
                     ActivateGadget(timegad, reqwin, NULL);
                     break;
#else
                  case 'c':
                  case 'C':
                     gid = CANCELGAD;
                     break;
                  case 'm':
                  case 'M':
                     GT_SetGadgetAttrs(multigad, reqwin, NULL,
                                       GTCB_Checked, !(multigad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     gid = MULTIGAD;
                     break;
                  case 't':
                  case 'T':
                     if(!time)
                        break;
                     ActivateGadget(timegad, reqwin, NULL);
                     break;
#endif
               }
            /* geen break hier! */
            case IDCMP_GADGETUP:
               switch(gid)
               {
                  case CANCELGAD:
                     result = 0;
                     done = TRUE;
                     break;
                  case OKGAD:
                     for(drive = 0; drive < 4; drive++)
                        if(drivegad[drive]->Flags & GFLG_SELECTED)
                           result |= FL_pow2[drive];
                     *multiple = (BOOL)(multigad->Flags & GFLG_SELECTED);
                     if(*name)
                     {
                        strcpy(FL_namegadbuff, ((struct StringInfo *)namegad->SpecialInfo)->Buffer);
                        *name = FL_namegadbuff;
                     }
                     if(time && !VMG_StringToTime(((struct StringInfo *)timegad->SpecialInfo)->Buffer, time))
                     {
                        strcpy(VBSG_errorinfo, (VBSG_countertype==TIMEDCT?TIMEFORMATTX:COUNTFORMATTX));
                        ERRG_Message(window, EENTERTIME);
                     }
                     else
                        done = TRUE;
                     break;
                  case MULTIGAD:
                     if(!(multigad->Flags & GFLG_SELECTED))
                     {
                        checkedone = FALSE;
                        for(drive = 0; drive < 4; drive++)
                        {
                           if(!checkedone)
                           {
                              if(drivegad[drive]->Flags & GFLG_SELECTED)
                                 checkedone = TRUE;
                           }
                           else
                              GT_SetGadgetAttrs(drivegad[drive], reqwin, NULL,
                                                GTCB_Checked, FALSE,
                                                TAG_END);
                        }
                        numdrives=1;
                     }
                     break;
                  case DF0GAD:
                  case DF1GAD:
                  case DF2GAD:
                  case DF3GAD:
                     if(!(multigad->Flags & GFLG_SELECTED))
                     {
                        if(drivegad[gid]->Flags & GFLG_SELECTED)
                        {
                           for(drive = 0; drive < 4; drive++)
                              if(drive!=gid)
                                 GT_SetGadgetAttrs(drivegad[drive], reqwin, NULL,
                                                   GTCB_Checked, FALSE,
                                                   TAG_END);
                        }
                        else
                           GT_SetGadgetAttrs(drivegad[gid], reqwin, NULL,
                                             GTCB_Checked, TRUE,
                                             TAG_END);
                     }
                     else
                     {
                        if(!(drivegad[gid]->Flags & GFLG_SELECTED))
                        {
                           if(!--numdrives)
                           {
                              GT_SetGadgetAttrs(drivegad[gid], reqwin, NULL,
                                                GTCB_Checked, TRUE,
                                                TAG_END);
                              numdrives=1;
                           }
                        }
                        else
                           numdrives++;
                     }
                     break;
               }
            case IDCMP_REFRESHWINDOW:
               GT_BeginRefresh(reqwin);
               GT_EndRefresh(reqwin, TRUE);
               break;
         }
         GT_ReplyIMsg(imsg);
      }
   }

quit:
   if(reqwin)
      CloseWindow(reqwin);
   FreeGadgets(glist);
   FreeVisualInfo(visinfo);
   UIG_UnBlockInput(window);
   return(result);
}
