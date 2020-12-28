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

#include "floppy_ui.h"
#include "floppy_ui.c"

static void    FL_ToggleGadgets(int, BOOL);

#define TIMESTRINGGADGET  floppy_timegadget
#define REQ               floppyRequesterStructure1
#define TIMESIBUFF        floppyfloppy_timegadgetSIBuff
#define TAPEITEXT         floppyIText9
#define GDF0              floppy_df0gadget
#define GDF1              floppy_df1gadget
#define GDF2              floppy_df2gadget
#define GDF3              floppy_df3gadget
#define MULTGADGET        floppy_multgadget

static struct Gadget *FL_diskgadgets[] = {
      &GDF0, &GDF1, &GDF2, &GDF3
};
static UBYTE          FL_pow2[4] = { 1, 2, 4, 8 };
static UBYTE          FL_avmask = 0;


int    FLG_AskFloppy(window, time, multiple)
struct Window  *window;
ULONG          *time;
BOOL           *multiple;
{
   static int            FL_flmask   = 1;
   static BOOL           FL_multiple = FALSE;
   struct IntuiMessage  *message;
   USHORT                gid;
   ULONG                 class;
   int                   i, mask;

   REQ.ReqGadget = time ? &floppy_timegadget : &floppy_df0gadget;
   TAPEITEXT.IText = time ? TAPESTARTPOSTX : NULL;
   if(!FL_avmask) {
      for(i = 0; i<4; i++) {
         if(FLBG_DriveAvailable(i)) {
            FL_avmask  |= FL_pow2[i];
            FL_diskgadgets[i]->Flags &= ~GADGDISABLED;
         } else {
            FL_diskgadgets[i]->Flags |=  GADGDISABLED;
         }
      }
   }

   strcpy(TIMESIBUFF, TIMEFORMATTX);
   Request(&REQ, window);
   UIG_ProcessRequester(&REQ);
   FL_ToggleGadgets(FL_flmask, FL_multiple);
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

   if(gid >= FLOPPY_DF0 && gid <= FLOPPY_DF3) {
      i = FL_pow2[gid-FLOPPY_DF0];
      if(FL_flmask!=i) {
         if(FL_multiple) {
            FL_ToggleGadgets(i, FALSE); FL_flmask ^= i;
         } else {
            FL_ToggleGadgets(FL_flmask | i, FALSE); FL_flmask = i;
         }
      }
      goto lus;
   }
   if(gid == FLOPPY_MULTIPLE) {
      i = 0;
      if(FL_multiple) {
         for(mask = 1; mask<16; mask<<=1) {
            if(FL_flmask & mask)
               break;
         }
         i = FL_flmask ^ mask; FL_flmask = mask;
      }
      FL_ToggleGadgets(i, TRUE); FL_multiple ^= TRUE; goto lus;
   }
   if(gid == FLOPPY_OK) {
      if(time && !VMG_StringToTime(TIMESIBUFF, time)) {
         ERRG_Message(window, EENTERTIME);
         goto lus;
      }
      *multiple = FL_multiple;

      i = FL_flmask;
   } else {
      i = 0;
   }
   EndRequest(&REQ, window);
   return(i);
}
static void     FL_ToggleGadgets(tflmask, tmultiple)
int      tflmask;
BOOL     tmultiple;
{
   int      i;

   for(i = 0; i<4; i++) {
      if(tflmask & FL_pow2[i])
         UIG_ToggleGadget(REQ.ReqLayer->rp, FL_diskgadgets[i]);
   }
   if(tmultiple)
      UIG_ToggleGadget(REQ.ReqLayer->rp, &MULTGADGET);
}

