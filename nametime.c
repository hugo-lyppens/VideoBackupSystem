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

#include "nametime_ui.h"
#include "nametime_ui.c"

#define REPFG            name_reportfileg
#define REPGADGET        name_reportgadget
#define ARCHGADGET       name_archgadget
#define NAMESTRINGGADGET name_namegadget
#define TIMESTRINGGADGET name_timegadget
#define REQ          nameRequesterStructure1
#define NAMESIBUFF   namename_namegadgetSIBuff
#define TIMESIBUFF   namename_timegadgetSIBuff


extern char    VBSG_reportdrawer[];

char  *NTG_AskNameTime(window, time, arch, report)
struct Window  *window;
ULONG          *time;
BOOL           *arch;
char           *report;
{
   struct IntuiMessage  *message;
   USHORT                gid;
   ULONG                 class;
   char                 *s;
   BOOL                  setarchbit = FALSE;
   struct RastPort      *rp;
   char                 *fullname, *name;
   char                  presetname[50];

   REQ.ReqGadget = &NAMESTRINGGADGET;
   if(time) {
      TIMESTRINGGADGET.NextGadget = REQ.ReqGadget;
      REQ.ReqGadget               = &TIMESTRINGGADGET;
   }
   if(arch) {
      ARCHGADGET.NextGadget = REQ.ReqGadget;
      REQ.ReqGadget         = &ARCHGADGET;
   }
   if(report) {
      *report = 0;

      REPGADGET.NextGadget  = REQ.ReqGadget;
      REQ.ReqGadget         = &REPGADGET;
   }


   strcpy(TIMESIBUFF, TIMEFORMATTX);
   Request(&REQ, window);
   UIG_ProcessRequester(&REQ);

   rp = REQ.ReqLayer->rp;
   if(report) {
      SetAPen(rp, 0); SetDrMd(rp, JAM1);
      Move(rp, REPFG.LeftEdge, REPFG.TopEdge+7);
      Text(rp, NONETX, LEN(NONETX));
   }
   ActivateGadget(&NAMESTRINGGADGET, window, &REQ);
lus:
   while(!(message = (struct IntuiMessage *)GetMsg(window->UserPort)))
      WaitPort(window->UserPort);

   class = message->Class;
   if(class==GADGETDOWN || class==GADGETUP){
      gid = ((struct Gadget *)(message->IAddress))->GadgetID;
   }
   ReplyMsg((struct Message *)message);
   if( class != GADGETUP && class != GADGETDOWN)
      goto lus;
   switch(gid) {
   case NAME_OK:
      if(time && !VMG_StringToTime(TIMESIBUFF, time)) {
         ERRG_Message(window, EENTERTIME);
         goto lus;
      }
      s = NAMESIBUFF;
      goto quit;
   case NAME_CANCEL:
      s = NULL; goto quit;
   case NAME_REPORT:
      presetname[0] = '\0';
      if(NAMESIBUFF[0]) {
         sprintf(presetname, "%s.Report", NAMESIBUFF);
      }
      fullname = FileRequest(window->WScreen, window, REPORTFNAMETX,
                             VBSG_reportdrawer, &name, presetname);
      SetAPen(rp, 2);
      RectFill(rp, REPFG.LeftEdge, REPFG.TopEdge,
                   REPFG.LeftEdge+REPFG.Width,
                   REPFG.TopEdge+REPFG.Height);
      SetAPen(rp, 0);
      SetDrMd(rp, JAM1);
      Move(rp, REPFG.LeftEdge, REPFG.TopEdge+7);
      if(fullname) {
         Text(rp, name, strlen(name));
         strcpy(report, fullname);
      } else {
         Text(rp, NONETX, LEN(NONETX));
         *report = 0;
      }
      break;
   case NAME_ARCH:
      setarchbit ^= TRUE; UIG_ToggleGadget(rp, &ARCHGADGET);
   }
   goto lus;
quit:
   if(arch)
      *arch = setarchbit;
   EndRequest(&REQ, window);
   return(s);
}

