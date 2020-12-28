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

extern UBYTE VBSG_reportdrawer[];
extern UBYTE VBSG_errorinfo[];
extern VBS_screentype_t  VBSG_screentype;
extern struct TextAttr   VBSG_text_ta;
extern VBS_countertype_t VBSG_countertype;

enum { OKGAD, CANCELGAD, NAMEGAD, COUNTERGAD, ARCHGAD, REPORTGAD, DISPLAYGAD };

static UBYTE NT_NameBuff[80];
static UBYTE NT_timegadbuff[9];

UBYTE *NTG_AskNameTime(struct Window *window, ULONG *time, BOOL *arch,
                      UBYTE *report, VM_operationtype_t operation)
{
   struct Window           *reqwin = NULL;
   struct Gadget           *gad, *namegad = NULL, *countergad, *archivegad,
                           *displaygad, *glist = NULL;
   struct NewGadget         newgad;
   void                    *visinfo;
   BOOL                     done = FALSE;
   int                      winwidth, winheight, twosp, ysize, ybot, tmp;
   struct IntuiMessage     *imsg;
   ULONG                    gid;
   ULONG                    class;
   UWORD                    code;
   UBYTE                   *result = NULL, *reportptr;
   UBYTE                    presetname[256];

   UIG_BlockInput(window);

   ysize     = VBSG_text_ta.ta_YSize;
   twosp     = TextLength(window->RPort, "hh", 2);

   ybot      = window->WScreen->WBorTop +
               window->WScreen->Font->ta_YSize +
               ysize + 1;

   winwidth  = window->Width * 2 / 3;

   if(!(visinfo = GetVisualInfo(window->WScreen, TAG_END)))
      goto quit;

   gad = CreateContext(&glist);

   newgad.ng_Height     = ysize * 3/2;
   newgad.ng_TextAttr   = &VBSG_text_ta;
   newgad.ng_VisualInfo = visinfo;
   newgad.ng_Flags      = PLACETEXT_LEFT;

   tmp = PIXLEN(window->RPort, BACKUPNAMETX) + twosp;
   newgad.ng_Width      = winwidth - tmp - twosp;
   newgad.ng_LeftEdge   = tmp;
   newgad.ng_TopEdge    = ybot;
   newgad.ng_GadgetText = BACKUPNAMETX;
   newgad.ng_GadgetID   = NAMEGAD;
   if(operation!=BACKUPOT)
   {
      gad = CreateGadget(STRING_KIND, gad, &newgad,
                         GTST_MaxChars, 80,
                         GT_Underscore, '_',
                         TAG_END);
      namegad = gad;
      ybot += ysize*2;
   }

   if(time)
   {
      newgad.ng_TopEdge    = ybot;
      if(VBSG_countertype ==TIMEDCT)
      {
         newgad.ng_Width      = PIXLEN(window->RPort, TIMEFORMATTX) + twosp*2;
         newgad.ng_GadgetText = TIMETX;
         strcpy(NT_timegadbuff, TIMEFORMATTX);
      }
      else
      {
         newgad.ng_Width      = PIXLEN(window->RPort, COUNTFORMATTX) + twosp*2;
         newgad.ng_GadgetText = COUNTERTX;
         strcpy(NT_timegadbuff, COUNTFORMATTX);
      }
      newgad.ng_GadgetID = COUNTERGAD;
      gad = CreateGadget(STRING_KIND, gad, &newgad,
      			 GTST_String,   NT_timegadbuff,
                         GTST_MaxChars, 8,
                         GT_Underscore, '_',
                         TAG_END);
      countergad = gad;
      ybot += ysize*2;
   }

   newgad.ng_Flags = PLACETEXT_RIGHT;

   if(arch)
   {
      newgad.ng_TopEdge    = ybot;
      newgad.ng_Width      = twosp;
      newgad.ng_GadgetText = ARCHIVETX;
      newgad.ng_GadgetID   = ARCHGAD;
      gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                         GT_Underscore, '_',
                         TAG_END);
      archivegad = gad;
      ybot += ysize*2;
   }

   if(report)
   {
      tmp = PIXLEN(window->RPort, REPORTTX) + twosp;
      newgad.ng_TopEdge    = ybot;
      newgad.ng_Width      = tmp;
      newgad.ng_Flags      = PLACETEXT_IN;
      newgad.ng_GadgetText = REPORTTX;
      newgad.ng_GadgetID   = REPORTGAD;
      gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                         GT_Underscore, '_',
                         TAG_END);

      tmp                  = newgad.ng_LeftEdge + tmp + twosp;
      newgad.ng_LeftEdge   = tmp;
      newgad.ng_Width      = winwidth - tmp - twosp;
      newgad.ng_GadgetText = NULL;
      newgad.ng_GadgetID   = DISPLAYGAD;
      gad = CreateGadget(TEXT_KIND, gad, &newgad,
                         GTTX_Border,   TRUE,
                         TAG_END);
      displaygad = gad;
      ybot += ysize*2;
   }

   newgad.ng_Width      = PIXLEN(window->RPort, OKTX) + twosp*2;
   newgad.ng_LeftEdge   = twosp;
   newgad.ng_TopEdge    = ybot;
   newgad.ng_Flags      = PLACETEXT_IN;
   newgad.ng_GadgetText = OKTX;
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

   if(!gad)
      goto quit;

   winheight = ybot + ysize;

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
                           WA_Title,       (operation==BACKUPOT?BACKUPFILESTX:(operation==RESTOREOT?RESTOREFILESTX:VERIFYFILESTX)),
                           WA_IDCMP,       (IDCMP_REFRESHWINDOW | IDCMP_VANILLAKEY | BUTTONIDCMP | CHECKBOXIDCMP | STRINGIDCMP),
                           WA_Gadgets,     glist,
                           TAG_END);

   if(!reqwin)
      goto quit;
   GT_RefreshWindow(reqwin, NULL);

   ActivateGadget(namegad, reqwin, NULL);

   if(report)
      *report = 0;

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
         GT_ReplyIMsg(imsg);
         switch(class)
         {
            case IDCMP_VANILLAKEY:
               gid = ~0;
               switch(code)
               {
                  case 'o':
                  case 'O':
                     gid = OKGAD;
                     break;
                  case 'n':
                  case 'N':
                     ActivateGadget(namegad, reqwin, NULL);
                     break;
                  case 'r':
                  case 'R':
                     if(!report)
                        break;
                     gid = REPORTGAD;
                     break;
#ifdef DEUTSCH
                  case 'a':
                  case 'A':
                     gid = CANCELGAD;
                     break;
                  case 'b':
                  case 'B':
                     if(!arch)
                     GT_SetGadgetAttrs(archivegad, reqwin, NULL,
                                       GTCB_Checked, !(archivegad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     gid = ARCHGAD;
                     break;
                  case 'z':
                  case 'Z':
                     if(!time)
                        break;
                     GT_SetGadgetAttrs(countergad, reqwin, NULL,
                                       GTCB_Checked, !(countergad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     gid = ARCHGAD;
                     break;
#else
                  case 'c':
                  case 'C':
                     gid = CANCELGAD;
                     break;
                  case 'a':
                  case 'A':
                     if(!arch)
                     GT_SetGadgetAttrs(archivegad, reqwin, NULL,
                                       GTCB_Checked, !(archivegad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     gid = ARCHGAD;
                     break;
                  case 't':
                  case 'T':
                     if(!time)
                        break;
                     GT_SetGadgetAttrs(countergad, reqwin, NULL,
                                       GTCB_Checked, !(countergad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     gid = ARCHGAD;
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
                     if(time && !VMG_StringToTime(((struct StringInfo *)countergad->SpecialInfo)->Buffer, time))
                     {
                        strcpy(VBSG_errorinfo, (VBSG_countertype==TIMEDCT?TIMEFORMATTX:COUNTFORMATTX));
                        ERRG_Message(reqwin, EENTERTIME);
                     }
                     else
                     {
                        if(operation!=BACKUPOT) {
                           strcpy(NT_NameBuff, ((struct StringInfo *)namegad->SpecialInfo)->Buffer);
                           result = NT_NameBuff;
                        } else {
                           result = (UBYTE *)TRUE;
			}                           
                        if(arch)
                           *arch = (archivegad->Flags & GFLG_SELECTED)!=0;
                        done = TRUE;
                     }
                     break;
                  case REPORTGAD:
                     *presetname = 0;
                     if(namegad && *(((struct StringInfo *)namegad->SpecialInfo)->Buffer))
                        sprintf(presetname, "%s.Report", ((struct StringInfo *)namegad->SpecialInfo)->Buffer);

                     reportptr = FileRequest(window, REPORTFNAMETX,
                                             VBSG_reportdrawer, NULL,
                                             presetname, SAVERT);
                     if(reportptr)
                     {
                        strcpy(report, reportptr);
                        GT_SetGadgetAttrs(displaygad, reqwin, NULL,
                                          GTTX_Text, report,
                                          TAG_END);
                     }
                     else
                     {
                        *report = 0;
                        GT_SetGadgetAttrs(displaygad, reqwin, NULL,
                                          GTTX_Text, NULL,
                                          TAG_END);
                     }
                     break;
               }
            case IDCMP_REFRESHWINDOW:
               GT_BeginRefresh(reqwin);
               GT_EndRefresh(reqwin, TRUE);
               break;
         }
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
