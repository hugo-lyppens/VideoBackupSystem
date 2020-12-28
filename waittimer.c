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

#define SECSPERMIN (60)
#define MINSPERHOUR (60)
#define SECSPERHOUR (MINSPERHOUR * SECSPERMIN)
#define HOURSPERDAY (24)
#define MINSPERDAY (MINSPERHOUR * HOURSPERDAY)
#define SECSPERDAY (SECSPERHOUR * HOURSPERDAY)

BOOL WTG_TimerRequest(LONG, UBYTE *);
BOOL WTG_StringToTime(UBYTE *, LONG *);

extern BOOL VMG_ScreenInit(VOID);
extern VOID VMG_ScreenCleanup(VOID);
extern BOOL HDBG_Init(VOID);
extern VOID HDBG_Cleanup(VOID);

extern struct TextAttr VBSG_text_ta;

struct Library *TimerBase;

struct IntuiText SecondIText =
{
   1, 0,
   JAM1,
   0, 0,
   NULL,
   NULL,
   NULL
};

struct IntuiText FirstIText =
{
   1, 0,
   JAM1,
   0, 0,
   NULL,
   STARTSATTX,
   &SecondIText
};

BOOL WTG_TimerRequest(LONG seconds, UBYTE *timestring)
{
   struct Screen       *defpubscreen = NULL;
   struct Window       *reqwin = NULL;
   struct Gadget       *gad, *glist = NULL;
   struct NewGadget     newgad;
   void                *visinfo = NULL;
   struct IntuiMessage *imsg;
   ULONG                class;
   UWORD                code;
   int                  winwidth, winheight, xsize, ysize, xleft, ytop;
   WORD                 zoomsizes[4];

   struct timeval      *time1, *time2, *time3;
   struct timerequest  *tr;
   struct MsgPort      *tp;
   LONG                 error, timecmp;
   BOOL                 result = FALSE;
   ULONG                windowsig, timersig, signals;

   time1 = (struct timeval *)AllocVec(sizeof(struct timeval),
                                      MEMF_PUBLIC | MEMF_CLEAR);
   time2 = (struct timeval *)AllocVec(sizeof(struct timeval),
                                      MEMF_PUBLIC | MEMF_CLEAR);
   time3 = (struct timeval *)AllocVec(sizeof(struct timeval),
                                      MEMF_PUBLIC | MEMF_CLEAR);
   tr    = (struct timerequest *)AllocVec(sizeof(struct timerequest),
                                          MEMF_PUBLIC | MEMF_CLEAR);
   if(!time1 || !time2 || !time3 || !tr) goto quit;

   error = OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)tr, 0L);
   if(error) goto quit;

   TimerBase = (struct Library *)tr->tr_node.io_Device;

   tp    = CreateMsgPort();
   if(!tp) goto quit;

   if(!(defpubscreen = LockPubScreen(NULL)))
      goto quit;

   xsize        = TextLength(&defpubscreen->RastPort, "h", 1);
   ysize        = VBSG_text_ta.ta_YSize;
   xleft        = defpubscreen->WBorLeft;
   ytop         = defpubscreen->WBorTop +
                  defpubscreen->Font->ta_YSize + 1;
   winwidth     = PIXLEN(&defpubscreen->RastPort, STARTSATTX) + xsize*16;
   winheight    = ysize*25/4;
   zoomsizes[0] = 0;
   zoomsizes[1] = ytop;
   zoomsizes[2] = winwidth + defpubscreen->WBorLeft + defpubscreen->WBorRight;
   zoomsizes[3] = ytop;

   if(!(visinfo = GetVisualInfo(defpubscreen, TAG_END)))
      goto quit;

   newgad.ng_Height     = ysize * 7/4;
   newgad.ng_TextAttr   = defpubscreen->Font;
   newgad.ng_VisualInfo = visinfo;
   newgad.ng_Flags      = PLACETEXT_IN;

   gad = CreateContext(&glist);

   newgad.ng_GadgetText = CANCELTX;
   newgad.ng_TopEdge    = ytop + ysize*4;
   newgad.ng_Width      = PIXLEN(&defpubscreen->RastPort, CANCELTX) + xsize*2;
   newgad.ng_LeftEdge   = xleft + (winwidth - newgad.ng_Width)>>1;
   gad = CreateGadget(BUTTON_KIND, gad, &newgad,
                      GT_Underscore, '_',
                      TAG_END);

   if(!gad)
      goto quit;

   reqwin = OpenWindowTags(NULL,
                           WA_PubScreen,   defpubscreen,
                           WA_Activate,    TRUE,
                           WA_Left,        0,
                           WA_Top,         ytop,
                           WA_InnerWidth,  winwidth,
                           WA_InnerHeight, winheight,
                           WA_AutoAdjust,  TRUE,
                           WA_DragBar,     TRUE,
                           WA_DepthGadget, TRUE,
                           WA_Title,       "Video Backup System",
                           WA_IDCMP,       (IDCMP_REFRESHWINDOW | IDCMP_VANILLAKEY | BUTTONIDCMP),
                           WA_Gadgets,     glist,
                           WA_Zoom,        &zoomsizes[0],
                           TAG_END);

   FirstIText.LeftEdge   = xleft + (winwidth - PIXLEN(&defpubscreen->RastPort, STARTSATTX))>>1;
   FirstIText.TopEdge    = ytop + ysize/2;
   FirstIText.ITextFont  = defpubscreen->Font;

   SecondIText.LeftEdge  = xleft + (winwidth - PIXLEN(&defpubscreen->RastPort, timestring))>>1;
   SecondIText.TopEdge   = ytop + ysize*2;
   SecondIText.ITextFont = defpubscreen->Font;
   SecondIText.IText     = timestring;

   UnlockPubScreen(NULL, defpubscreen);

   PrintIText(reqwin->RPort, &FirstIText, 0, 0);

   HDBG_Cleanup();
   VMG_ScreenCleanup();

   tr->tr_node.io_Message.mn_Node.ln_Type = NT_MESSAGE;
   tr->tr_node.io_Message.mn_Node.ln_Pri  = 0;
   tr->tr_node.io_Message.mn_Node.ln_Name = NULL;
   tr->tr_node.io_Message.mn_ReplyPort    = tp;
   tr->tr_node.io_Command                 = TR_GETSYSTIME;

   DoIO((struct IORequest *)tr);

   time1->tv_secs  = (tr->tr_time.tv_secs % SECSPERDAY);
   time1->tv_micro = 0;

   time2->tv_secs  = seconds;
   time2->tv_micro = 0;

   time3->tv_secs  = SECSPERDAY;
   time3->tv_micro = 0;

   timecmp = CmpTime(time1, time2);
   if(timecmp < 0)
      AddTime(time2, time3);

   SubTime(time2, time1);

   tr->tr_time.tv_secs                    = time2->tv_secs;
   tr->tr_time.tv_micro                   = 0;
   tr->tr_node.io_Message.mn_Node.ln_Type = NT_MESSAGE;
   tr->tr_node.io_Message.mn_Node.ln_Pri  = 0;
   tr->tr_node.io_Message.mn_Node.ln_Name = NULL;
   tr->tr_node.io_Message.mn_ReplyPort    = tp;
   tr->tr_node.io_Command                 = TR_ADDREQUEST;

   SendIO((struct IORequest*)tr);

   windowsig = 1 << reqwin->UserPort->mp_SigBit;
   timersig  = 1 << tp->mp_SigBit;

loop:
   signals = Wait(windowsig | timersig);
   if(signals & timersig)
   {
      result = TRUE;
      WaitIO((struct IORequest *)tr);
      goto done;
   }
   if(signals & windowsig)
   {
      imsg = GT_GetIMsg(reqwin->UserPort);
      class = imsg->Class;
      code = imsg->Code;
      GT_ReplyIMsg(imsg);
      switch(class)
      {
         case IDCMP_GADGETUP:
         case IDCMP_GADGETDOWN:
            result = FALSE;
            AbortIO((struct IORequest *)tr);
            WaitIO((struct IORequest *)tr);
            goto done;
         case IDCMP_VANILLAKEY:
#ifdef DEUTSCH
            if(code=='a' || code=='A')
#else
            if(code=='c' || code=='C')
#endif
            {
               result = FALSE;
               AbortIO((struct IORequest *)tr);
               WaitIO((struct IORequest *)tr);
               goto done;
            }
            break;
         case IDCMP_REFRESHWINDOW:
            GT_BeginRefresh(reqwin);
            PrintIText(reqwin->RPort, &FirstIText, 0, 0);
            GT_EndRefresh(reqwin, TRUE);
            break;
      }
   }
   goto loop;

done:
   VMG_ScreenInit();
   HDBG_Init();

quit:
   if(reqwin)
      CloseWindow(reqwin);
   FreeGadgets(glist);
   FreeVisualInfo(visinfo);

   if(!error)
   {
      CloseDevice((struct IORequest *)tr);
      TimerBase = (struct Library *)(-1);
   }
   if(tp)     DeleteMsgPort(tp);
   if(tr)     FreeVec(tr);
   if(time1)  FreeVec(time1);
   if(time2)  FreeVec(time2);
   if(time3)  FreeVec(time3);
   return(result);
}

BOOL WTG_StringToTime(string, time)
register UBYTE    *string;
LONG              *time;
{
   int           i, l, te;
   UBYTE         timeelements[3];
   UBYTE        *p;
   static UBYTE  maxelements[3] =
   {
      HOURSPERDAY, MINSPERHOUR, SECSPERMIN
   };
   p = string;
   for(i = 0; i<3; i++)
   {
      if(i && *p++ != ':')
         return(FALSE);
      l = stcd_i(p, &te);
      if(l<=0 || l>2 || te<0 || te >= maxelements[i])
         return(FALSE);
      timeelements[i] = (UBYTE)te;
      p+=l;
   }
   *time = timeelements[0] * SECSPERHOUR +
           timeelements[1] * SECSPERMIN +
           timeelements[2];
   return(TRUE);
}
