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

#define BCOLOR    1

#include "error_ui.c"

#define REQ          errorRequesterStructure1
#define MAXLINES     4

typedef struct {
   char    *LeftSelection, *RightSelection;
   int      NumLines;
   char    *Lines[MAXLINES];
} ERR_message_t;

#ifdef DEUTSCH

static ERR_message_t ERR_messages[] = {
   {NULL, NULL,     0, NULL},
   {NULL, "OK",     1, "Lesefehler!"},
   {NULL, "OK",     1, "Schreibfehler!"},
   {NULL, "Leider", 1, "Zuwenig Speicher!"},
   {NULL, "OK",     1, "Verify-Fehler!"},
   {NULL, "OK",     1, "Suche abgebrochen!"},
   {NULL, "OK",     1, "Disk schreibgeschutzt!"},
   {NULL, "OK",     1, "Kein Disk im Laufwerk!"},
   {NULL, "OK",     1, "Operation abgebrochen!"},
   {NULL, "OK",     1, "Video-Lesefehler !"},
   {   "jetzt OK", "Abbruch",
    3, "Video-VerbindungsÜberprÜfung negativ!",
       "Schalte Video ein, selektiere AUX-kanal",
       "und überprüfen sie Bitte die Kabel."},
   {"OK", "Abbruch",
    3, "Verzeichnisstruktur wurde gelesen. Bitte",
       "drÜcken Sie PAUSE oder STOP am Video",
       "Selektiere 'OK' zum Dateiauswahl"},
   {"OK", "Abbruch",
    3, "Verzeichnisstruktur wurde gelesen. Bitte",
       "drÜcken Sie PAUSE oder STOP am Video",
       "Selektiere 'OK' zum Dateiauswahl"},
   {NULL, "Abbruch", 1, "Dieser Option wurde noch nicht implementiert!"},
   {NULL, "Abbruch", 1, "Dieses Bandverzeichnis existstiert schon."},
   {NULL, "OK",     1, "Konnte Verzeichnisdatei nicht erstellen!"},
   {"Ja", "Nein",    2, "Bandverzeichnis geändert. Wollen Sie",
                       "es aktualisieren?"},
   {NULL, "OK",  2, "Bitte geben Sie die Anfangsstelle am Band ein,",
                       "im Format 'ST:MI:SE'."},
   {NULL, "OK",     1, "Fehler beim Schreiben der Rapport-Datei!"}
};

#else

static ERR_message_t ERR_messages[] = {
   {NULL, NULL,     0, NULL},
   {NULL, "OK",     1, "Read Error!"},
   {NULL, "OK",     1, "Write Error!"},
   {NULL, "Forget It!", 1, "Not Enough Memory!"},
   {NULL, "OK",     1, "Verify Error!"},
   {NULL, "OK",     1, "Search Abort!"},
   {NULL, "OK",     1, "Disk write protected!"},
   {NULL, "OK",     1, "No Disk in Drive!"},
   {NULL, "OK",     1, "Break error !"},
   {NULL, "OK",     1, "Video Read Error !"},
   {   "OK now", "Abort",
    3, "Video Connection Check failed !",
       "Switch on video, select AUX channel",
       "and check connections"},
   {"OK", "Abort Restore",
    3, "Tree information now read. Please",
       "press PAUSE or STOP on video",
       "Click OK to select files"},
   {"OK", "Abort Verify",
    3, "Tree information now read. Please",
       "press PAUSE or STOP on video",
       "Click OK to select files"},
   {NULL, "Cancel", 1, "This feature is not implemented yet!"},
   {NULL, "Cancel", 1, "This log file already exists"},
   {NULL, "OK",     1, "Couldn't create log file!"},
   {"Yes", "No",    2, "Log File changed. Do you want",
                       "to save the changes?"},
   {NULL, "Retry",  2, "Please enter video tape start location,",
                       "using format 'H:MM:SS'."},
   {NULL, "OK",     1, "Error writing Report file !"}
};
#endif


static struct TextAttr TOPAZ80 = {
   (STRPTR)"topaz.font",
   TOPAZ_EIGHTY,0,0
};
int   ERRG_Message(window, error)
struct Window  *window;
VBS_error_t     error;
{
   struct IntuiMessage  *message;
   USHORT                gid;
   ULONG                 class;
   char                 *s;
   struct RastPort      *rp;
   ERR_message_t        *errmsg;
   int                   i;
   struct TextFont      *font = NULL;

   if(!error)
      return(ERROR_RETRY);
   errmsg = &ERR_messages[(int)error];
   REQ.ReqGadget = errmsg->LeftSelection ? &error_retrygadget :
                                           &error_cancelgadget;

   error_retrygadget.GadgetText->IText  = errmsg->LeftSelection;
   error_cancelgadget.GadgetText->IText = errmsg->RightSelection;
   REQ.LeftEdge = window->Width  - REQ.Width >> 1;
   REQ.TopEdge  = window->Height - REQ.Height >> 1;

   Request(&REQ, window);
   UIG_ProcessRequester(&REQ);

   rp = REQ.ReqLayer->rp;
   font = OpenFont(&TOPAZ80);
   if(!font)
      goto nofont;
   SetFont(rp, font);
   SetAPen(rp, 0); SetDrMd(rp, JAM1);
   for(i = 0; i<errmsg->NumLines; i++) {
      Move(rp, 16, 18+(8*i)); s = errmsg->Lines[i];
      Text(rp, s, strlen(s));
   }
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

nofont:
   EndRequest(&REQ, window);
   if(font)
      CloseFont(font); font = NULL;

   return((int)gid);
}

