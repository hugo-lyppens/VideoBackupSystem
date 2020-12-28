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

#define ES_SIZE sizeof(struct EasyStruct)
#define REQ_TITLE "VBS"

extern UBYTE VBSG_errorinfo[];

#ifdef DEUTSCH

static struct EasyStruct ERR_messages[] = {
   {ES_SIZE, 0, REQ_TITLE, 0, 0},
   {ES_SIZE, 0, REQ_TITLE, "Fehler bei lesen von\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Fehler bei schreiben von\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Zu wenig freien Speicher\nvorhanden für Operation", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Fehler beim Verifieren", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Suche wurde abgebrochen", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Disk ist schreibgeschütz", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Kein Diskette vorhanden\nin Laufwerk %s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Operation wurde abgebrochen", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Video-Lesefehler", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Verbinding zum Video ist nicht\nkorrekt. Bitte überprüfen Sie\ndie Kabel und selektieren\nSie den AUX-Kanal.", "Nochmal versuchen|Abbrechen"},
   {ES_SIZE, 0, REQ_TITLE, "Verzeichnisstruktur wurde gelesen.\nBitte drÜcken Sie PAUSE oder\nSTOP am Video. Selektieren Sie\n'OK' zur Dateiauswahl.", "Weiter|Backup abbrechen"},
   {ES_SIZE, 0, REQ_TITLE, "Verzeichnisstruktur wurde gelesen.\nBitte drÜcken Sie PAUSE oder\nSTOP am Video. Selektieren Sie\n'OK' zur Dateiauswahl.", "Weiter|Verifizieren abbrechen"},
   {ES_SIZE, 0, REQ_TITLE, "Diese Funktion wurde nog nicht\nimplementiert", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Die Verzeichnisdatei\n%s\nexistiert schon", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Die Verzeichnisdatei\n%s\nkonnte nicht erstellt werden.", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Die Verzeichnisdatei\n%s\nkonnte nicht geladen werden.", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Das Bandverzeichnis wurde geändert.\nTrotzdem verlassen?", "Verlassen|Speichern und verlassen|Abbrechen"},
   {ES_SIZE, 0, REQ_TITLE, "Bitte geben Sie die Anfangsstelle\nam Band ein, im Format \"%s\".", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Fehler beim Schreiben der Rapport-Datei\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Bitte geben Sie die Anfangszeit ein,\nim Format \"%s\".", "OK"}
};

#else

static struct EasyStruct ERR_messages[] = {
   {ES_SIZE, 0, REQ_TITLE, 0, 0},
   {ES_SIZE, 0, REQ_TITLE, "Error while reading\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Error while writing\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Not enough memory", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Verify error", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Search was aborted", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Disk in %s\nis write protected", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "No disk present in\ndrive %s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Operation aborted", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Video read error", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Wrong Chunk", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Encountered 45", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Video connection check failed. Make\nsure the VCR is turned on and set\nto the AUX channel, and check the\nconnections.", "Retry|Abort"},
   {ES_SIZE, 0, REQ_TITLE, "Disk tree has been read. Press PAUSE\nor STOP on the VCR. Select 'Continue'\nto select files.", "Continue|Abort restore"},
   {ES_SIZE, 0, REQ_TITLE, "Disk tree has been read. Press PAUSE\nor STOP on the VCR. Select 'Continue'\nto select files.", "Continue|Abort verify"},
   {ES_SIZE, 0, REQ_TITLE, "This function has not yet been\nimplemented", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "The log file\n%s\n already exists", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Couldn't create log file\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Couldn't load log file\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Log file has been modified.", "Save first|Continue|Cancel"},
   {ES_SIZE, 0, REQ_TITLE, "Enter video tape starting location,\nusing the format \"%s\" first.", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Error while writing report file\n%s", "OK"},
   {ES_SIZE, 0, REQ_TITLE, "Enter backup start time,\nusing the format \"%s\".", "OK"}
};

#endif

int   ERRG_Message(window, error)
struct Window  *window;
VBS_error_t     error;
{
   ULONG       IDCMPFlags = IDCMP_DISKINSERTED, *IDCMPFlagsPtr = NULL;
   int         result;

   if(error==EWRITEPROTECTED || error==ENODISKINDRIVE)
      IDCMPFlagsPtr = &IDCMPFlags;

   switch(error)
   {
      case EREAD:
      case EWRITE:
      case EWRITEPROTECTED:
      case ENODISKINDRIVE:
      case ELOGEXISTS:
      case ECOULDNTCREATELOG:
      case EENTERTIME:
      case EWRITEREPORT:
         result = (int)EasyRequest(window, &ERR_messages[(int)error], IDCMPFlagsPtr, VBSG_errorinfo);
         break;
      default:
         result = (int)EasyRequest(window, &ERR_messages[(int)error], IDCMPFlagsPtr);
   }
   return(result);
}
