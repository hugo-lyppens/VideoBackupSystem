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

#define PATHSIZE 256


UBYTE path[PATHSIZE];
UBYTE file[PATHSIZE];

UBYTE *FileRequest(win, header, drawer, nameptr, presetname, reqtype)
struct Window      *win;
UBYTE              *header, *drawer, **nameptr, *presetname;
VM_requestertype_t  reqtype;
{
   struct FileRequester *fr;
   UBYTE  drawerbuf[PATHSIZE];

   if(!drawer)
   {
      GetCurrentDirName(drawerbuf, PATHSIZE);
      drawer = drawerbuf;
   }

   if(fr = (struct FileRequester *)
      AllocAslRequestTags(ASL_FileRequest,
                          ASL_Hail,       header,
                          ASL_Width,      ((win->Width * 9) >> 4),
                          ASL_Height,     ((win->Height * 2) / 3),
                          ASL_LeftEdge,   ((win->Width * 3) >> 4),
                          ASL_TopEdge,    (win->Height / 6),
                          ASL_Window,     win,
#ifdef DEUTSCH
                          ASL_OKText,     (reqtype==LOADRT?"Öffnen":(reqtype==SAVERT?"Speichern":"Wählen")),
#else
                          ASL_OKText,     (reqtype==LOADRT?"Open":(reqtype==SAVERT?"Save":"Select")),
#endif
                          (presetname?ASL_File:TAG_IGNORE),
                                          presetname,
                          (drawer?ASL_Dir:TAG_IGNORE),
                                          drawer,
                          ASL_Pattern,    "~(#?.info)",
                          ASL_FuncFlags,  (reqtype==SAVERT?FILF_SAVE:0),
                          ASL_ExtFlags1,  (reqtype==DIRRT?FIL1F_NOFILES:0),
                          TAG_DONE))
   {
      if(AslRequest(fr, NULL))
      {
         *path = 0;
         AddPart(path, fr->rf_Dir, PATHSIZE);
         AddPart(path, fr->rf_File, PATHSIZE);
         if(nameptr)
            *nameptr = FilePart(path);
      }
      else
         *path = 0;
      FreeAslRequest(fr);
   }
   if(*path)
      return(path);
   else
      return(NULL);
}
