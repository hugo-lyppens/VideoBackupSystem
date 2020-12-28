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

enum { SAVEGAD = 0, USEGAD, CANCELGAD, COUNTERGAD, PUBSCREENGAD,
       CLOSEWBGAD, VIDEOCHECKGAD, DISKCHANGEGAD, BEEPAFTERGAD,
       EXTRADELAYGAD, LOGDRAWERGAD,
       REPORTDRAWERGAD, TEXTGAD, PADGAD, LARGEGAD, SMALLGAD, SCREENMODEGAD };

static struct TextAttr      text_ta, pad_ta, large_ta, small_ta;
static UBYTE                text_fn[32], pad_fn[32], large_fn[32], small_fn[32];
static struct List         *DispList = NULL;

static struct Window       *reqwin = NULL;
static struct Gadget       *propertiesgad;
   extern BOOL              VMG_Init(void);
   extern void              VMG_Cleanup(void);

extern char		 VBS_largefont[];
extern int		 VBS_largesize;
extern char		 VBS_smallfont[];
extern int		 VBS_smallsize;
extern char		 VBS_textfont[];
extern int		 VBS_textsize;
extern char		 VBS_padfont[];
extern int		 VBS_padsize;
   extern struct Screen    *VBSG_screen;
   extern VBS_screentype_t  VBSG_screentype;
   extern BOOL              VBSG_usepubscreen;
   extern BOOL              VBSG_closeworkbench;
   extern BOOL              VBSG_novideocheck;
   extern BOOL              VBSG_nochange;
   extern BOOL              VBSG_beepafter;
   extern UBYTE		    VBSG_extradelay;
   extern UBYTE             VBSG_logdrawer[];
   extern UBYTE             VBSG_reportdrawer[];
   extern struct TextAttr   VBSG_text_ta;
   extern struct TextAttr   VBSG_pad_ta;
   extern struct TextAttr   VBSG_large_ta;
   extern struct TextAttr   VBSG_small_ta;
   extern VBS_countertype_t VBSG_countertype;
   extern UBYTE             VBSG_prefsfile[];
   extern UBYTE             VBSG_errorinfo[];
   extern ULONG             VBSG_displayid;

static VBS_error_t	SET_SaveSettings(void);

                           
BOOL SETG_Settings(struct Window *window)
{
   struct Screen           *wbscreen;
   struct Gadget           *gad, *countergad, *pubscreengad, *closewbgad,
                           *videocheckgad, *diskchangegad, *logdrawergad,
                           *beepaftergad, *extradelaygad,
                           *reportdrawergad, *screenmodegad, *glist = NULL;
   struct NewGadget         newgad;
   void                    *visinfo;
   struct IntuiMessage     *imsg;
   ULONG                    class;
   UWORD                    code;
   BOOL                     use = FALSE, save = FALSE, reopen = FALSE,
                            done = FALSE, usepubscreen, closeworkbench,
                            newvm = FALSE, newba = FALSE, newsc = FALSE,
                            envarc = FALSE;
   int                      winwidth, winheight, twosp, ysize, ytop, tmp;
   int			    tmpy;
   ULONG                    gid;
   STRPTR                   CounterTypes[] =
                               { TIMEDTX, NUMERICTX, 0 };
   UBYTE                   *filereq_result;
   VBS_countertype_t        counttype;
   UBYTE                    logdrawer[256], reportdrawer[256],
                            prefsfilename[256], string[64];
   VBS_DispNode_t          *selectednode;
   ULONG                    modeid = VBSG_displayid;
   UWORD                    scrheight;
   BPTR                     prefsfile = NULL;
   VBS_error_t		    error;
   
   UIG_BlockInput(window);

   text_ta.ta_Name  = text_fn;
   SET_CopyTextAttr(&text_ta, &VBSG_text_ta);
   pad_ta.ta_Name   = pad_fn;
   SET_CopyTextAttr(&pad_ta, &VBSG_pad_ta);
   large_ta.ta_Name = large_fn;
   SET_CopyTextAttr(&large_ta, &VBSG_large_ta);
   small_ta.ta_Name = small_fn;
   SET_CopyTextAttr(&small_ta, &VBSG_small_ta);

   ysize     = VBSG_text_ta.ta_YSize;
   ytop      = window->WScreen->WBorTop +
               window->WScreen->Font->ta_YSize + 1;
   winwidth  = (window->Width * 7/8);
   twosp     = TextLength(window->RPort, "ee", 2);

   counttype      = VBSG_countertype;
   usepubscreen   = VBSG_usepubscreen;
   closeworkbench = VBSG_closeworkbench;

   strcpy(logdrawer, VBSG_logdrawer);
   strcpy(reportdrawer, VBSG_reportdrawer);

   if(!(visinfo = GetVisualInfo(window->WScreen, TAG_END)))
      goto quit;

   gad = CreateContext(&glist);

   newgad.ng_Height     = (ysize * 3) >> 1;
   newgad.ng_TextAttr   = &VBSG_text_ta;
   newgad.ng_VisualInfo = visinfo;
   newgad.ng_Flags      = PLACETEXT_LEFT;

   newgad.ng_LeftEdge   = PIXLEN(window->RPort, COUNTERTYPETX) + twosp*3/2;
   newgad.ng_TopEdge    = ytop + ysize;
   newgad.ng_Width      = PIXLEN(window->RPort, NUMERICTX) + twosp*3;
   newgad.ng_GadgetText = COUNTERTYPETX;
   newgad.ng_GadgetID   = COUNTERGAD;
   gad = CreateGadget(CYCLE_KIND, gad, &newgad,
                      GTCY_Labels,   CounterTypes,
                      GTCY_Active,   (VBSG_countertype==TIMEDCT?0:1),
                      GT_Underscore, '_',
                      TAG_END);
   countergad = gad;

   newgad.ng_LeftEdge   = PIXLEN(window->RPort, EXTRADELAYTX) + twosp*3/2;
   newgad.ng_TopEdge   += ysize*2;
   newgad.ng_Width      = twosp*2;
   newgad.ng_GadgetText = EXTRADELAYTX;
   newgad.ng_GadgetID   = EXTRADELAYGAD;
   gad = CreateGadget(INTEGER_KIND, gad, &newgad,
                      GTIN_Number,  VBSG_extradelay,
                      GTIN_MaxChars, 2,
                      GT_Underscore, '_',
                      TAG_END);
   extradelaygad = gad;

   newgad.ng_LeftEdge   = twosp*3/2;
   newgad.ng_TopEdge   += ysize*2;
   newgad.ng_Width      = twosp;
   newgad.ng_GadgetText = USEPUBSCREENTX;
   newgad.ng_GadgetID   = PUBSCREENGAD;
   newgad.ng_Flags      = PLACETEXT_RIGHT;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GTCB_Checked,  (VBSG_usepubscreen),
                      GT_Underscore, '_',
                      TAG_END);
   pubscreengad = gad;
#if 0
   newgad.ng_TopEdge   += ysize*3/2;
   newgad.ng_Width      = twosp;
   newgad.ng_GadgetText = CLOSEWORKBENCHTX;
   newgad.ng_GadgetID   = CLOSEWBGAD;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GTCB_Checked,  (VBSG_closeworkbench),
                      GT_Underscore, '_',
                      TAG_END);
   closewbgad = gad;
#endif
   newgad.ng_TopEdge   += ysize*3/2;
   newgad.ng_Width      = twosp;
   newgad.ng_GadgetText = VIDEOCHECKTX;
   newgad.ng_GadgetID   = VIDEOCHECKGAD;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GTCB_Checked,  (!VBSG_novideocheck),
                      GT_Underscore, '_',
                      TAG_END);
   videocheckgad = gad;

   newgad.ng_TopEdge   += ysize*3/2;
   newgad.ng_Width      = twosp;
   newgad.ng_GadgetText = DISKCHANGETX;
   newgad.ng_GadgetID   = DISKCHANGEGAD;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GTCB_Checked,  (!VBSG_nochange),
                      GT_Underscore, '_',
                      TAG_END);
   diskchangegad = gad;

   newgad.ng_TopEdge   += ysize*3/2;
   newgad.ng_Width      = twosp;
   newgad.ng_GadgetText = BEEPAFTERTX;
   newgad.ng_GadgetID   = BEEPAFTERGAD;
   gad = CreateGadget(CHECKBOX_KIND, gad, &newgad,
                      GTCB_Checked,  VBSG_beepafter,
                      GT_Underscore, '_',
                      TAG_END);
   beepaftergad = gad;


   tmp  = PIXLEN(window->RPort, SELECTREPORTDIRTX) + twosp;
   newgad.ng_TopEdge   += ysize*5/2;
   tmpy = newgad.ng_TopEdge;
   newgad.ng_Width      = tmp;
   newgad.ng_Height     = ysize*3/2;
   newgad.ng_GadgetText = SELECTLOGDIRTX;
   newgad.ng_GadgetID   = LOGDRAWERGAD;
   newgad.ng_Flags      = PLACETEXT_IN;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_TopEdge   += ysize*7/4;
   newgad.ng_GadgetText = SELECTREPORTDIRTX;
   newgad.ng_GadgetID   = REPORTDRAWERGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge   = tmp + twosp * 5/2;
   newgad.ng_TopEdge    = tmpy;
   newgad.ng_Width      = winwidth - tmp - twosp*4;
   newgad.ng_GadgetText = NULL;
   gad = CreateGadget(TEXT_KIND, gad, &newgad,
                      GTTX_Text,   &logdrawer,
                      GTTX_Border, TRUE,
                      TAG_END);
   logdrawergad = gad;

   newgad.ng_TopEdge   += ysize*7/4;
   gad = CreateGadget(TEXT_KIND, gad, &newgad,
                      GTTX_Text,   &reportdrawer,
                      GTTX_Border, TRUE,
                      TAG_END);
   reportdrawergad = gad;

   tmp = (winwidth - twosp*6) >> 1;
   newgad.ng_LeftEdge   = twosp*3/2;
   newgad.ng_TopEdge   += ysize*2;
   tmpy = newgad.ng_TopEdge;
   newgad.ng_Width = tmp;
   newgad.ng_GadgetText = SELECTTEXTTX;
   newgad.ng_GadgetID   = TEXTGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_TopEdge   += ysize*7/4;
   newgad.ng_GadgetText = SELECTPADTX;
   newgad.ng_GadgetID   = PADGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge   = winwidth - twosp*3/2 - tmp;
   newgad.ng_TopEdge    = tmpy;
   newgad.ng_GadgetText = SELECTLARGETX;
   newgad.ng_GadgetID   = LARGEGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_TopEdge   += ysize*7/4;
   newgad.ng_GadgetText = SELECTSMALLTX;
   newgad.ng_GadgetID   = SMALLGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   tmp = PIXLEN(window->RPort, CANCELTX) + twosp;
   newgad.ng_LeftEdge   = twosp * 3/2;
   newgad.ng_TopEdge   += ysize*2;
   newgad.ng_Width      = tmp;
   newgad.ng_GadgetText = SAVETX;
   newgad.ng_GadgetID   = SAVEGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge      = (winwidth - tmp) >> 1;
   newgad.ng_GadgetText    = USETX;
   newgad.ng_GadgetID      = USEGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   newgad.ng_LeftEdge      = winwidth - tmp - twosp* 3/2;
   newgad.ng_GadgetText    = CANCELTX;
   newgad.ng_GadgetID      = CANCELGAD;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   winheight = newgad.ng_TopEdge + ysize;

   tmp = twosp*16;
   newgad.ng_LeftEdge   = tmp;
   newgad.ng_TopEdge    = ytop+ysize*5/2;
   newgad.ng_Width      = winwidth-twosp*3/2-tmp;
   newgad.ng_Height     = ysize*11/2;
   newgad.ng_GadgetText = SCREENMODETX;
   newgad.ng_GadgetID   = SCREENMODEGAD;
   newgad.ng_Flags      = PLACETEXT_ABOVE;
   gad = CreateGadget(LISTVIEW_KIND, gad, &newgad,
                      GTLV_ShowSelected, NULL,
                      GT_Underscore,     '_',
                      GTLV_Labels,       NULL,
                      TAG_END);
   screenmodegad = gad;

   newgad.ng_TopEdge   += ysize*11/2;
   newgad.ng_Height     = ysize*3/2;
   newgad.ng_GadgetText = NULL;
   gad = CreateGadget(TEXT_KIND, gad, &newgad,
                      GTTX_Border,       TRUE,
                      TAG_END);
   propertiesgad = gad;

   if(!gad)
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
                           WA_Title,       SETTINGSTITLETX,
                           WA_IDCMP,       (IDCMP_REFRESHWINDOW | IDCMP_VANILLAKEY | IDCMP_RAWKEY | BUTTONIDCMP | CHECKBOXIDCMP | CYCLEIDCMP | LISTVIEWIDCMP),
                           WA_Gadgets,     glist,
                           TAG_END);

   if(!reqwin)
      goto quit;

   GT_RefreshWindow(reqwin, NULL);

   if(!SET_ScanScreenModes())
      goto quit;

   GT_SetGadgetAttrs(screenmodegad, reqwin, NULL,
                     GTLV_Labels,   DispList,
                     TAG_END);

   modeid = VBSG_displayid;
   selectednode = (VBS_DispNode_t *)DispList->lh_Head;
   while(selectednode->dn_Node.ln_Succ)
   {
      if(selectednode->ModeID == modeid)
         break;
      selectednode = (VBS_DispNode_t *)selectednode->dn_Node.ln_Succ;
   }
   if(selectednode->ModeID != modeid)
      selectednode = (VBS_DispNode_t *)DispList->lh_Head;

   GT_SetGadgetAttrs(screenmodegad, reqwin, NULL,
                     GTLV_Selected, selectednode->Number,
                     TAG_END);

   SET_SetDisplayData(selectednode);

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
#if 0
                  case 'w':
                  case 'W':
                     GT_SetGadgetAttrs(closewbgad, reqwin, NULL,
                                       GTCB_Checked, !(closewbgad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     break;
#endif                     
                  case 'v':
                  case 'V':
                     GT_SetGadgetAttrs(videocheckgad, reqwin, NULL,
                                       GTCB_Checked, !(videocheckgad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     break;
                  case 'd':
                  case 'D':
                     GT_SetGadgetAttrs(diskchangegad, reqwin, NULL,
                                       GTCB_Checked, !(diskchangegad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     break;
                  case 'r':
                  case 'R':
                     gid = REPORTDRAWERGAD;
                     break;
		  case 'a':
                  case 'A':
                     gid = LARGEGAD;
                     break;
                  case 'n':
                  case 'N':
                     gid = SMALLGAD;
                     break;
                  case 't':
                  case 'T':
                     gid = TEXTGAD;
                     break;
                  case 'f':
                  case 'F':
                     gid = PADGAD;
                     break;
                  case 's':
                  case 'S':
                     gid = SAVEGAD;
                     break;
#ifdef DEUTSCH
                  case 'z':
                  case 'Z':
                     counttype = (counttype==NUMERICCT?TIMEDCT:NUMERICCT);
                     GT_SetGadgetAttrs(countergad, reqwin, NULL,
                                       GTCY_Active, (counttype==TIMEDCT?0:1),
                                       TAG_END);
                     break;
                  case 'u':
                  case 'U':
                     GT_SetGadgetAttrs(pubscreengad, reqwin, NULL,
                                       GTCB_Checked, !(pubscreengad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     break;
                  case 'b':
                  case 'B':
                     gid = LOGDRAWERGAD;
                     break;
                  case 'g':
                  case 'G':
                     gid = LARGEGAD;
                     break;
                  case 'k':
                  case 'K':
                     gid = SMALLGAD;
                     break;
                  case 'n':
                  case 'N':
                     gid = USEGAD;
                     break;
                  case 'a':
                  case 'A':
                     gid = CANCELGAD;
                     break;
                  case 'i':
                     if(selectednode && selectednode->dn_Node.ln_Succ->ln_Succ)
                     {
                        selectednode = (VBS_DispNode_t *)selectednode->dn_Node.ln_Succ;
                        code = selectednode->Number;
                        GT_SetGadgetAttrs(screenmodegad, reqwin, NULL,
                                          GTLV_Selected, code,
                                          TAG_END);
                        gid = SCREENMODEGAD;
                     }
                     break;
                  case 'I':
                     if(selectednode && selectednode->dn_Node.ln_Pred->ln_Pred)
                     {
                        selectednode = (VBS_DispNode_t *)selectednode->dn_Node.ln_Pred;
                        code = selectednode->Number;
                        GT_SetGadgetAttrs(screenmodegad, reqwin, NULL,
                                          GTLV_Selected, code,
                                          TAG_END);
                        gid = SCREENMODEGAD;
                     }
                     break;
#else
                  case 'o':
                  case 'O':
                     counttype = (counttype==NUMERICCT?TIMEDCT:NUMERICCT);
                     GT_SetGadgetAttrs(countergad, reqwin, NULL,
                                       GTCY_Active, (counttype==TIMEDCT?0:1),
                                       TAG_END);
                     break;
                  case 'p':
                  case 'P':
                     GT_SetGadgetAttrs(pubscreengad, reqwin, NULL,
                                       GTCB_Checked, !(pubscreengad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     break;
                  case 'b':
                  case 'B':
                     GT_SetGadgetAttrs(beepaftergad, reqwin, NULL,
                                       GTCB_Checked, !(beepaftergad->Flags & GFLG_SELECTED),
                                       TAG_END);
                     break;
                  case 'l':
                  case 'L':
                     gid = LOGDRAWERGAD;
                     break;
                  case 'u':
                  case 'U':
                     gid = USEGAD;
                     break;
                  case 'c':
                  case 'C':
                     gid = CANCELGAD;
                     break;
                  case 'm':
                     if(selectednode && selectednode->dn_Node.ln_Succ->ln_Succ)
                     {
                        selectednode = (VBS_DispNode_t *)selectednode->dn_Node.ln_Succ;
                        code = selectednode->Number;
                        GT_SetGadgetAttrs(screenmodegad, reqwin, NULL,
                                          GTLV_Selected, code,
                                          TAG_END);
                        gid = SCREENMODEGAD;
                     }
                     break;
                  case 'M':
                     if(selectednode && selectednode->dn_Node.ln_Pred->ln_Pred)
                     {
                        selectednode = (VBS_DispNode_t *)selectednode->dn_Node.ln_Pred;
                        code = selectednode->Number;
                        GT_SetGadgetAttrs(screenmodegad, reqwin, NULL,
                                          GTLV_Selected, code,
                                          TAG_END);
                        gid = SCREENMODEGAD;
                     }
                     break;
#endif
               }
            /* geen break hier! */
            case IDCMP_GADGETUP:
               switch(gid)
               {
                  case SAVEGAD:
                     use = TRUE;
                     save = TRUE;
                     done = TRUE;
                     break;
                  case USEGAD:
                     use = TRUE;
                     done = TRUE;
                     break;
                  case CANCELGAD:
                     done = TRUE;
                     break;
                  case COUNTERGAD:
                     counttype = (imsg->Code==0?TIMEDCT:NUMERICCT);
                     break;
                  case TEXTGAD:
                     if(SET_FontRequest(window, SELECTTEXTTTX, (window->Height / 25), &text_ta))
                        newvm = TRUE;
                     break;
                  case PADGAD:
                     if(SET_FontRequest(window, SELECTPADTTX, (window->Height / 25), &pad_ta))
                        newvm = TRUE;
                     break;
                  case LARGEGAD:
                     if(SET_FontRequest(window, SELECTLARGETTX, 100, &large_ta))
                        newba = TRUE;
                     break;
                  case SMALLGAD:
                     if(SET_FontRequest(window, SELECTSMALLTTX, 50, &small_ta))
                        newba = TRUE;
                     break;
                  case LOGDRAWERGAD:
                     filereq_result = FileRequest(window, SELECTLOGDIRTTX,
                                                  logdrawer, NULL,
                                                  NULL, DIRRT);
                     if(filereq_result)
                     {
                        strcpy(logdrawer, filereq_result);
                        GT_SetGadgetAttrs(logdrawergad, reqwin, NULL,
                                          GTTX_Text, logdrawer,
                                          TAG_END);
                     }
                     break;
                  case REPORTDRAWERGAD:
                     filereq_result = FileRequest(window, SELECTREPORTDIRTTX,
                                                  reportdrawer, NULL,
                                                  NULL, DIRRT);
                     if(filereq_result)
                     {
                        strcpy(reportdrawer, filereq_result);
                        GT_SetGadgetAttrs(reportdrawergad, reqwin, NULL,
                                          GTTX_Text, reportdrawer,
                                          TAG_END);
                     }
                     break;
                  case SCREENMODEGAD:
                     selectednode = SET_FindNode(code);
                     SET_SetDisplayData(selectednode);
                     modeid = selectednode->ModeID;
                     newsc = TRUE;
                     break;
               }
               break;
#if 0               
            case IDCMP_RAWKEY:
               if(code==0x5F)
               {
                  HLPG_ShowHelp("settings");
               }
               break;
#endif               
            case IDCMP_REFRESHWINDOW:
               GT_BeginRefresh(reqwin);
               GT_EndRefresh(reqwin, TRUE);
               break;
         }
         GT_ReplyIMsg(imsg);
      }
   }

   if(use)
   {
      if(newsc)
      {
         reopen = TRUE;
         scrheight = selectednode->DimInfo.Nominal.MaxY -
                     selectednode->DimInfo.Nominal.MinY + 1;
         if(text_ta.ta_YSize > (scrheight/25))
         {
            text_ta.ta_YSize = (8);
            strcpy(text_ta.ta_Name, "topaz.font");
            newvm = TRUE;
         }
         if(pad_ta.ta_YSize > (scrheight/25))
         {
            pad_ta.ta_YSize = (8);
            strcpy(pad_ta.ta_Name, "topaz.font");
            newvm = TRUE;
         }
      }
      if(VMG_LogEmpty())
	      VBSG_countertype = counttype;
      VBSG_usepubscreen =
         (pubscreengad->Flags & GFLG_SELECTED);
      if(VBSG_usepubscreen!=usepubscreen)
         reopen = TRUE;
#if 0         
      VBSG_closeworkbench =
         (closewbgad->Flags & GFLG_SELECTED);       
      if(VBSG_closeworkbench && (wbscreen = LockPubScreen("Workbench")))
      {
         UnlockPubScreen("Workbench", wbscreen);
         CloseWorkBench();
      }
      else
      if(!VBSG_closeworkbench)
      {
         OpenWorkBench();
         ScreenToFront(reqwin->WScreen);
      }
#endif      
      VBSG_novideocheck =
         !(videocheckgad->Flags & GFLG_SELECTED);
      VBSG_nochange =
         !(diskchangegad->Flags & GFLG_SELECTED);
      VBSG_beepafter =
         (beepaftergad->Flags & GFLG_SELECTED);
      VBSG_displayid = modeid;
      strcpy(VBSG_logdrawer, logdrawer);
      strcpy(VBSG_reportdrawer, reportdrawer);
      SET_CopyTextAttr(&VBSG_text_ta, &text_ta);
      SET_CopyTextAttr(&VBSG_pad_ta, &pad_ta);
      SET_CopyTextAttr(&VBSG_large_ta, &large_ta);
      SET_CopyTextAttr(&VBSG_small_ta, &small_ta);

      VBSG_extradelay = ((struct StringInfo *)extradelaygad->SpecialInfo)->LongInt;

   }

   if(save)
   {
   	if(error = SET_SaveSettings()) {
	         strcpy(VBSG_errorinfo, "settings file");
        	 ERRG_Message(reqwin, error);
       	}
   	
   }
#if 0
rewrite:
      if(prefsfile)
      {
         FPuts(prefsfile, "VBS_preferences_file_v1\n");

         FPuts(prefsfile, VBSG_large_ta.ta_Name);
         sprintf(string, "-%ld\n", VBSG_large_ta.ta_YSize);
         FPuts(prefsfile, string);

         FPuts(prefsfile, VBSG_small_ta.ta_Name);
         sprintf(string, "-%ld\n", VBSG_small_ta.ta_YSize);
         FPuts(prefsfile, string);

         FPuts(prefsfile, VBSG_text_ta.ta_Name);
         sprintf(string, "-%ld\n", VBSG_text_ta.ta_YSize);
         FPuts(prefsfile, string);

         FPuts(prefsfile, VBSG_pad_ta.ta_Name);
         sprintf(string, "-%ld\n", VBSG_pad_ta.ta_YSize);
         FPuts(prefsfile, string);

         FPuts(prefsfile, VBSG_logdrawer);
         FPutC(prefsfile, '\n');
         FPuts(prefsfile, VBSG_reportdrawer);
         FPutC(prefsfile, '\n');

         sprintf(string, "%lu\n", VBSG_displayid);
         FPuts(prefsfile, string);

         FPutC(prefsfile, ((UBYTE)VBSG_countertype + '0'));
         FPutC(prefsfile, VBSG_usepubscreen?'1':'0');
         FPutC(prefsfile, VBSG_closeworkbench?'1':'0');
         FPutC(prefsfile, VBSG_novideocheck?'1':'0');
         FPutC(prefsfile, VBSG_nochange?'1':'0');

         Close(prefsfile);
      }
      else
      {
         strcpy(VBSG_errorinfo, "settings file");
         ERRG_Message(reqwin, EWRITE);
      }

      if(envarc)
      {
         UnLock(CreateDir("ENVARC:VBS"));
         prefsfile = Open("ENVARC:VBS/VBS.prefs", MODE_NEWFILE);
         if(prefsfile)
         {
            envarc = FALSE;
            goto rewrite;
         }
      }
#endif      
   

quit:
   SET_FreeScreenModes();
   if(reqwin)
      CloseWindow(reqwin);
   FreeGadgets(glist);
   FreeVisualInfo(visinfo);
   UIG_UnBlockInput(window);
   if(use && newba)
   {
      BAG_Cleanup();
      BAG_Init();
   }
   if(use && newvm)
   {
      VMG_Cleanup();
      VMG_Init();
   }
   if(reopen)
      return(TRUE);
   else
      return(FALSE);
}


static void SET_SeparateNameSize(UBYTE *s, UBYTE *name, int *size)
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

VBS_error_t	SETG_LoadSettings()
{
	FILE		*fp = NULL;
	VBS_error_t	 error = EOK;
	char		 line[80], *p, *eqsign, c;
	int		 i;

	fp = fopen(VBSG_prefsfile, "r");
	if(!fp) {
		error = EREAD; goto quit;
	}

	do {
		if(!fgets(line, sizeof line, fp)) {
			error = EREAD;
			goto quit;
		}
	} while(memcmp(line, "[VBSOptions]", 12));
	while(fgets(line, sizeof line, fp)) {
		p = &line[0];eqsign=0;
		while(c = *p) {
			if((eqsign && c==' ')||c=='\n'||c=='=')
				*p = 0;
			p++;
			if(c == '=')
				eqsign = p;
		}
		if(!stricmp(line, "LARGEFONT")) {
			SET_SeparateNameSize(eqsign, 
				VBS_largefont, &VBS_largesize);
		}
		if(!stricmp(line, "SMALLFONT")) {
			SET_SeparateNameSize(eqsign, 
				VBS_smallfont, &VBS_smallsize);
		}
		if(!stricmp(line, "TEXTFONT")) {
			SET_SeparateNameSize(eqsign, 
				VBS_textfont, &VBS_textsize);
		}
		if(!stricmp(line, "PADFONT")) {
			SET_SeparateNameSize(eqsign, 
				VBS_padfont, &VBS_padsize);
		}
		if(!stricmp(line, "LOGDRAWER"))
			strcpy(VBSG_logdrawer, eqsign);
/*		if(!stricmp(line, "LOGFILE"))
			strcpy(VBSG_initlogfile, eqsign);*/
		if(!stricmp(line, "REPORTDRAWER"))
			strcpy(VBSG_reportdrawer, eqsign);
		if(!stricmp(line, "VIDEOCHECK"))
			VBSG_novideocheck =
			   !stricmp(eqsign, "no")||!stricmp(eqsign, "off");
		if(!stricmp(line, "EXTRADELAY"))
			VBSG_extradelay = atoi(eqsign);
		if(!stricmp(line, "COUNTERTYPE"))
			VBSG_countertype = stricmp(eqsign, "numeric")?TIMEDCT:NUMERICCT;
		if(!stricmp(line, "USEPUBSCREEN"))
			VBSG_usepubscreen = !stricmp(eqsign, "on") ||
					    !stricmp(eqsign, "yes");
		if(!stricmp(line, "DISKCHANGE"))
			VBSG_nochange =
			   !stricmp(eqsign, "no")||!stricmp(eqsign, "off");
		if(!stricmp(line, "BEEPAFTERFILEBACKUP"))
			VBSG_beepafter =
			   !stricmp(eqsign, "yes")||!stricmp(eqsign, "on");
		if(!stricmp(line, "ModeID"))
			VBSG_displayid = strtoul(eqsign, &p, 16);
	}
quit:
	if(fp)
		fclose(fp);
	return(error);
}

static VBS_error_t	SET_SaveSettings()
{
	FILE		*fp = NULL;
	VBS_error_t	 error = EWRITE;

	fp = fopen(VBSG_prefsfile, "w");
	if(!fp)
		goto quit;
	if(fprintf(fp, "[VBSOptions]\n")==EOF)
		goto quit;

	if(fprintf(fp, "TextFont=%s-%d\n", 
		VBSG_text_ta.ta_Name, VBSG_text_ta.ta_YSize) == EOF)
			goto quit;
	if(fprintf(fp, "PadFont=%s-%d\n", 
		VBSG_pad_ta.ta_Name, VBSG_pad_ta.ta_YSize) == EOF)
			goto quit;
	if(fprintf(fp, "LargeFont=%s-%d\n", 
		VBSG_large_ta.ta_Name, VBSG_large_ta.ta_YSize) == EOF)
			goto quit;
	if(fprintf(fp, "SmallFont=%s-%d\n", 
		VBSG_small_ta.ta_Name, VBSG_small_ta.ta_YSize) == EOF)
			goto quit;

	if(fprintf(fp, "LogDrawer=%s\n", VBSG_logdrawer) == EOF)
		goto quit;
/*	if(fprintf(fp, "LogFile=%s\n", VBSG_initlogfile) == EOF)
		goto quit;*/
	if(fprintf(fp, "ReportDrawer=%s\n", VBSG_reportdrawer) == EOF)
		goto quit;
	if(fprintf(fp, "CounterType=%s\n", VBSG_countertype==NUMERICCT?"numeric":"time")
		== EOF)
		goto quit;
	if(fprintf(fp, "ExtraDelay=%d\n", VBSG_extradelay) == EOF)
		goto quit;
	if(fprintf(fp, "VideoCheck=%s\n", VBSG_novideocheck?"off":"on")
		== EOF)
		goto quit;
	if(fprintf(fp, "DiskChange=%s\n", VBSG_nochange?"off":"on")
		== EOF)
		goto quit;
	if(fprintf(fp, "BeepAfterFileBackup=%s\n", VBSG_beepafter?"on":"off")
		== EOF)
		goto quit;
	if(fprintf(fp, "UsePubScreen=%s\n", VBSG_usepubscreen?"on":"off")
		== EOF)
		goto quit;
	if(fprintf(fp, "ModeID=%lX\n", VBSG_displayid)
		== EOF)
		goto quit;
	error = EOK;
quit:
	if(fp)
		fclose(fp);
	return(error);
}



BOOL SET_FontRequest(struct Window *win, UBYTE *header, int maxheight,
                     struct TextAttr *return_ta)
{
   struct FontRequester *fr;
   BOOL                  result = FALSE;

   if(fr = (struct FontRequester *)
      AllocAslRequestTags(ASL_FontRequest,
                          ASL_Hail,       header,
                          ASL_Width,      (win->Width >> 1),
                          ASL_Height,     ((win->Height * 3) >> 2),
                          ASL_LeftEdge,   (win->Width >> 2),
                          ASL_TopEdge,    ((win->Height * 3) >> 3),
                          ASL_Window,     win,
                          ASL_MaxHeight,  maxheight,
                          ASL_FontName,   return_ta->ta_Name,
                          ASL_FontHeight, return_ta->ta_YSize,
                          TAG_DONE))
   {
      if(AslRequest(fr, NULL))
      {
         strcpy(return_ta->ta_Name, fr->fo_Attr.ta_Name);
         return_ta->ta_YSize = fr->fo_Attr.ta_YSize;
         return_ta->ta_Style = 0;
         return_ta->ta_Flags = 0;
         result = TRUE;
      }
      FreeAslRequest(fr);
   }
   return(result);
}

VOID SET_CopyTextAttr(struct TextAttr *d, struct TextAttr *s)
{
   strcpy(d->ta_Name, s->ta_Name);
   d->ta_YSize = s->ta_YSize;
}

VBS_DispNode_t *SET_FindNode(ULONG nr)
{
   VBS_DispNode_t *dnode = (VBS_DispNode_t *)DispList->lh_Head;

   while(nr-- > 0)
   dnode = (VBS_DispNode_t *)dnode->dn_Node.ln_Succ;

   return(dnode);
}

VOID SET_FreeScreenModes()
{
   VBS_DispNode_t *wnode, *nnode;

   wnode = (VBS_DispNode_t *)DispList->lh_Head;
   while(nnode = (VBS_DispNode_t *)(wnode->dn_Node.ln_Succ))
   {
      if(wnode->dn_Node.ln_Name)
         FreeVec(wnode->dn_Node.ln_Name);
      Remove((struct Node *)wnode);
      FreeVec(wnode);
      wnode = nnode;
   }
   FreeVec(DispList);
}

BOOL SET_ScanScreenModes()
{
   struct DisplayInfo  dinfo;
   struct DimensionInfo  dimin;
   struct MonitorInfo  minfo;
   struct NameInfo     ninfo;
   VBS_DispNode_t     *dnode;
   ULONG               ModeID = INVALID_ID;
   struct Screen      *defpubscreen;
   UBYTE              *wbscreentext = USEWBVALUESTX;
   UWORD               num = 0, xdim;

   if(!(DispList = AllocVec(sizeof(struct List), MEMF_CLEAR|MEMF_PUBLIC)))
   {
      goto quit;
   }
   
   NewList(DispList);

   if(defpubscreen = LockPubScreen(NULL))
   {
      ModeID       = GetVPModeID(&defpubscreen->ViewPort);
      UnlockPubScreen(NULL, defpubscreen);
      
      if(!(dnode = AllocVec(sizeof(VBS_DispNode_t), MEMF_CLEAR|MEMF_PUBLIC)))
         goto next;

      if(!(dnode->dn_Node.ln_Name = AllocVec(strlen(wbscreentext)+1, MEMF_CLEAR|MEMF_PUBLIC)))
      {
         FreeVec(dnode);
         goto next;
      }

      strcpy(dnode->dn_Node.ln_Name, wbscreentext);
      GetDisplayInfoData(NULL, (UBYTE *)&(dnode->DimInfo), sizeof(struct DimensionInfo), DTAG_DIMS, ModeID);
      GetDisplayInfoData(NULL, (UBYTE *)&(dnode->MonInfo), sizeof(struct MonitorInfo), DTAG_MNTR, ModeID);
      GetDisplayInfoData(NULL, (UBYTE *)&(dnode->DispInfo), sizeof(struct DisplayInfo), DTAG_DISP, ModeID);
      dnode->ModeID = INVALID_ID/*ModeID*/;
      dnode->Number = num++;
      AddTail(DispList, (struct Node *)dnode);
   }
next:
   ModeID = INVALID_ID;
   while((ModeID = NextDisplayInfo(ModeID)) != INVALID_ID)
   {
      if(!GetDisplayInfoData(NULL, (UBYTE *)&dinfo, sizeof(dinfo), DTAG_DISP, ModeID))
         continue;
      if(dinfo.NotAvailable || !(ModeID & MONITOR_ID_MASK)/* || !(ModeID & HIRES_KEY)*/)
         continue;
      if(!GetDisplayInfoData(NULL, (UBYTE *)&minfo, sizeof(minfo), DTAG_MNTR, ModeID))
         continue;
      if(!GetDisplayInfoData(NULL, (UBYTE *)&ninfo, sizeof(ninfo), DTAG_NAME, ModeID))
         continue;
      if(!GetDisplayInfoData(NULL, (UBYTE *)&dimin, sizeof(dimin), DTAG_DIMS, ModeID))
         continue;
      xdim = dimin.Nominal.MaxX - dimin.Nominal.MinX + 1;
      if(xdim<630)
      	 continue;

      if(!(dnode = AllocVec(sizeof(VBS_DispNode_t), MEMF_CLEAR|MEMF_PUBLIC)))
         continue;

      if(!(dnode->dn_Node.ln_Name = AllocVec(strlen(ninfo.Name)+1, MEMF_CLEAR|MEMF_PUBLIC)))
      {
         FreeVec(dnode);
         continue;
      }

      strcpy(dnode->dn_Node.ln_Name, ninfo.Name);
      GetDisplayInfoData(NULL, (UBYTE *)&(dnode->DimInfo), sizeof(struct DimensionInfo), DTAG_DIMS, ModeID);
      GetDisplayInfoData(NULL, (UBYTE *)&(dnode->MonInfo), sizeof(struct MonitorInfo), DTAG_MNTR, ModeID);
      GetDisplayInfoData(NULL, (UBYTE *)&(dnode->DispInfo), sizeof(struct DisplayInfo), DTAG_DISP, ModeID);
      dnode->ModeID = ModeID;
      dnode->Number = num++;
      AddTail(DispList, (struct Node *)dnode);
   }
   return(TRUE);

quit:
   return(FALSE);
}

VOID SET_SetDisplayData(VBS_DispNode_t *node)
{
   struct DimensionInfo *diminfo = &(node->DimInfo);
   struct MonitorInfo   *moninfo = &(node->MonInfo);
   UWORD                 xdim, ydim, cols, hz;
   UBYTE                 str[80];

   xdim = diminfo->Nominal.MaxX - diminfo->Nominal.MinX + 1;
   ydim = diminfo->Nominal.MaxY - diminfo->Nominal.MinY + 1;
   cols = 1<<(diminfo->MaxDepth);
   if(cols>8)
   	cols = 8;
/*   hz   = 3571429/(moninfo->TotalRows)/(moninfo->TotalColorClocks);*/
   sprintf(str, PROPERTIESTX, xdim, ydim, cols);
   GT_SetGadgetAttrs(propertiesgad, reqwin, NULL,
                     GTTX_Text,     str,
                     GTTX_CopyText, TRUE,
                     TAG_END);
}
