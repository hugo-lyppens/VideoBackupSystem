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

#include "path_ui.h"
#include "path_ui.c"

#define STRINGGADGET pathGadget1
#define REQ          pathRequesterStructure1
#define PTEXT        pathIText3

char  *PAG_AskPath(window, pathstring)
struct Window  *window;
char           *pathstring;
{
   struct IntuiMessage  *message;
   USHORT                gid;
   ULONG                 class;
   char                 *s;

   PTEXT.IText = pathstring;
   Request(&REQ, window);
   UIG_ProcessRequester(&REQ);
   Delay(5);
   ActivateGadget(&STRINGGADGET, window, &REQ);
lus:
   while(!(message = (struct IntuiMessage *)GetMsg(window->UserPort)))
      WaitPort(window->UserPort);

   class = message->Class;
   if(class==GADGETDOWN || class==GADGETUP){
      gid = ((struct Gadget *)(message->IAddress))->GadgetID;
   }
   ReplyMsg((struct Message *)message);
   if( class != GADGETUP)
      goto lus;
   switch(gid) {
   case PATH_OK:
   case PATH_STRING:
      s = pathpathGadget1SIBuff;
      break;
   case PATH_CANCEL:
      s = NULL;
   }
   EndRequest(&REQ, window);
   return(s);
}


