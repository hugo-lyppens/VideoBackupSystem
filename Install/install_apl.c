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
 * Copyright (C) 1991 by Hugo Lyppens.                                     *
 * All rights reserved. An unpublished work.                               *
 *                                                                         *
 * Hugo Lyppens,                                                           *
 * Generaal Coenderslaan 25,                                               *
 * NL-5623 LT  Eindhoven.                                                  *
 * Telephone 31-40-461403                                                  *
 *                                                                         *
 ***************************************************************************/

#include    <exec/types.h>
#include    <exec/memory.h>
#include    <intuition/intuition.h>
#include    <proto/all.h>
#include    <string.h>
#include    <stdlib.h>
#include    "installtext.h"

/***********************************/
/*                                 */
/***********************************/

extern struct NewWindow NewWindowStructure1;
extern struct Gadget    Gadget1, TextPad;
extern struct IntuiText IText3, IText4;
extern char             Gadget1SIBuff[];
/***********************************/
/*                                 */
/***********************************/
int      main(void);
void     Clear(void);
void     Flush(void);
void     Redraw(void);
void     PrintString(char *);
BOOL     CopyFile(char *, char *, BOOL);
BOOL     CopyDir(char *, char *);


/*void     sprintf(char *, char *,);*/
/***********************************/
/*                                 */
/***********************************/
#define NAMELEN      80
#define NUMBER(x)    ((sizeof x)/sizeof x[0])

/***********************************/
/*                                 */
/***********************************/

struct GfxBase          *GfxBase;
struct IntuitionBase    *IntuitionBase;

static struct Window    *w;
static struct RastPort  *rp;

#ifdef DEUTSCH

static char *directories[] = {
   "Bandverz", "/Bandverz",
   "Rapport",  "/Rapport",
   "fonts/melba", "FONTS:melba"
};
static char *files[] = {
   "VBS", "/VBS",
   "VBS.info", "/VBS.info",
   "Bandverz.info", "/Bandverz.info",
   "Rapport.info", "/Rapport.info",
   "fonts/melba.font", "FONTS:melba.font"
};

#else

static char *directories[] = {
   "LogFiles", "/LogFiles",
   "Reports",  "/Reports",
   "fonts/melba", "FONTS:melba"
};
static char *files[] = {
   "VBS", "/VBS",
   "VBS.info", "/VBS.info",
   "LogFiles.info", "/LogFiles.info",
   "Reports.info", "/Reports.info",
   "fonts/melba.font", "FONTS:melba.font"
};

#endif

static int   ypos = 0;


struct TextAttr TOPAZ80 = {
   (STRPTR)"topaz.font",
   TOPAZ_EIGHTY,0,0
};

main()
{
   enum {
      COPYFILES, CHANGESTARTUP
   }        mode;

   struct IntuiMessage *message;
   BPTR     lock;
 
   char    *colonptr, *name, *s;
   ULONG    class;
   USHORT   code, gid;
   int      i;
   BOOL     error = FALSE;
   char     drawer[NAMELEN];
   char     startup[NAMELEN], old[NAMELEN];
   char     cmd[NAMELEN];
   struct Font *font80 = NULL;

   GfxBase       = (struct GfxBase *)     OpenLibrary("graphics.library",0);
   IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library",0);
   font80        = OpenFont(&TOPAZ80);
   if(!font80)
         goto quit;

   if(!(w = OpenWindow(&NewWindowStructure1)))
      goto quit;
   rp = w->RPort;
   SetFont(rp, font80);
   Redraw();
   PrintIText(rp, &IText3, 0, 0);
   PrintString(T1);
   PrintString(T2);
   PrintString(T3);
   PrintString(T4);
   PrintString(T5);
   mode = COPYFILES;
lus:
   while(!(message=(struct IntuiMessage *)GetMsg(w->UserPort)))
      WaitPort(w->UserPort);
   class = message->Class;code = message->Code;
   if(class==GADGETUP||class==GADGETDOWN){
      gid=((struct Gadget *)(message->IAddress))->GadgetID;
   }
   ReplyMsg((struct Message *)message);
   switch(gid) {
   case 1:
      if(!error){
         switch(mode) {
         case COPYFILES:
            PrintString(INSTABBR);
            break;
         case CHANGESTARTUP:
            PrintString(INSTSUCC);
            PrintString(NOMOD);
         }
         Delay(100);
      }
      goto quit2;
   case 2:
      if(error)
         break;
      switch(mode) {
      case COPYFILES:

         strcpy(drawer, Gadget1SIBuff);
         colonptr = strrchr(drawer, ':');
         if(!colonptr) {
            PrintString(DRIVESPEC1);
            PrintString(DRIVESPEC2);
            PrintString(DRIVESPEC3);
            break;
         }
         lock = Lock(drawer, ACCESS_READ);
         if(lock) {
            PrintString(DIREX);
            UnLock(lock);
         } else {
            PrintString(CREATDIR);
            PrintString(drawer);PrintString("");
            if(!(lock = CreateDir(drawer))){
               PrintString(CNCREATDIR);
erroroccurred:
               PrintString(PRABORT);
               error = TRUE;break;
            }
            UnLock(lock);
         }
         PrintString(COPYEX);
         sprintf(cmd, "%s.info",drawer);
         if(!CopyFile("diricon", cmd, FALSE))
            goto copyerror;

         for(i = 0; i<NUMBER(files); i+=2) {
            name = files[i];
            if(files[i+1][0] == '/')
               sprintf(cmd, "%s%s", drawer, files[i+1]);
            else
               strcpy(cmd, files[i+1]);

            if(!CopyFile(name, cmd, FALSE)){
copyerror:
               PrintString(CNCOPY);
               goto erroroccurred;
            }
         }

         for(i=0; i<NUMBER(directories); i+=2) {
            name = directories[i];
            PrintString(CREATDIR);
            if(directories[i+1][0] == '/')
               sprintf(cmd, "%s%s", drawer, directories[i+1]);
            else
               strcpy(cmd, directories[i+1]);

            PrintString(cmd);
            if(!(lock = CreateDir(cmd))) {
               PrintString(CNCREATDIR);
               continue;
            }

            UnLock(lock);
            PrintString(COPYINGFILES);
            if(!CopyDir(name, cmd)) {
               PrintString(ERROCCUR);
               goto erroroccurred;
            }
         }
         PrintString(OKF);
         Delay(50);
         Clear();
         RemoveGadget(w, &Gadget1);
/*         for(s = drawer, i=0; s<=colonptr; s++, i++)
            Gadget1SIBuff[i] = *s;  */
         if(lock = Lock("s:User-Startup", ACCESS_READ)) {
            UnLock(lock);
            strcpy(&Gadget1SIBuff[0], "s:User-Startup");
         } else {
            strcpy(&Gadget1SIBuff[0], "s:Startup-Sequence");
         }
         AddGadget(w, &Gadget1, -1);
         RefreshGadgets(&Gadget1, w, NULL);
         PrintIText(rp, &IText4, 0, 0);
         PrintString(L1);
         PrintString(L2);
         PrintString(L3);
         PrintString(L4);
         sprintf(cmd, "'Assign VBS: %s'", drawer);
         PrintString(cmd);PrintString("");
         PrintString(ADD);
         PrintString(MANUAL);

         sprintf(cmd, "Assign >NIL: VBS: %s", drawer);
         lock = Open("nil:", MODE_NEWFILE);
         if(lock) {
            Execute(cmd, 0, lock);
         } else
            PrintString("Couldn't open nil:");
         mode = CHANGESTARTUP;
         Flush();
         break;

      case CHANGESTARTUP:

         strcpy(startup, Gadget1SIBuff);
         colonptr = strrchr(startup, ':');
         if(!colonptr) {
            PrintString(DRIVESPEC1);
            PrintString(DRIVESPEC2);
            PrintString(DRIVESPEC3);
            break;
         }
         PrintString("");
         sprintf(old, "%s.old", startup);
         PrintString(EXISTREN);
         PrintString(old);
         if(!Rename(startup, old)) {
            PrintString(CNREN);
            goto erroroccurred;
         }
         sprintf(cmd, "Assign VBS: %s\n", drawer);
         lock = Open(startup, MODE_NEWFILE);
         if(!lock || Write(lock, cmd, strlen(cmd))<strlen(cmd))
            goto cmod;
         Close(lock);

         if(!CopyFile(old, startup,TRUE)) {
cmod:       PrintString(CNMOD);
            goto erroroccurred;
         }
         PrintString(ADDED);
         PrintString("");
         PrintString(OKCOMPL);
         Delay(100);
         goto quit2;
      }
   }
   goto lus;
quit2:
   CloseWindow(w);
quit:
   if(font80)
      CloseFont(font80);
   CloseLibrary((struct Library *)GfxBase);
   CloseLibrary((struct Library *)IntuitionBase);
}
void Redraw()
{
   struct Gadget     *gadget;

   SetAPen(rp, 2);
   RectFill(rp, w->BorderLeft+1, w->BorderTop+1,
                w->Width-w->BorderRight-2, w->Height-w->BorderBottom-2);
   SetAPen(rp, 0);
   gadget = w->FirstGadget;
   while(gadget){
      RectFill(rp, gadget->LeftEdge, gadget->TopEdge,
                   gadget->LeftEdge+gadget->Width-1,
                   gadget->TopEdge +gadget->Height-1);
      gadget = gadget->NextGadget;
   }

   RefreshGadgets(w->FirstGadget, w, NULL);
}
void PrintString(s)
char     *s;
{
   if( ypos > 8) {
      ScrollRaster(rp, 0, 8, TextPad.LeftEdge+1, TextPad.TopEdge+1,
                             TextPad.LeftEdge + TextPad.Width-2,
                             TextPad.TopEdge  + TextPad.Height-2);
      ypos = 8;
   }
   Move(rp, TextPad.LeftEdge+4, TextPad.TopEdge +(2+7)+ypos*8);
   SetAPen(rp, 1);
   Text(rp, s, strlen(s));
   Delay(4);
   ypos++;
}
void Clear()
{
   SetAPen(rp, 0);
   RectFill(rp, TextPad.LeftEdge+1, TextPad.TopEdge+1,
                TextPad.LeftEdge + TextPad.Width -2,
                TextPad.TopEdge  + TextPad.Height-2);
   ypos = 0;
}
void Flush()
{
   struct IntuiMessage *message;

   while(message=(struct IntuiMessage *)GetMsg(w->UserPort))
      ReplyMsg((struct Message *)message);
}
BOOL CopyDir(from, to)
char     *from, *to;
{
   struct FileInfoBlock *fib;

   BPTR     fromlock = NULL, tolock = NULL;
   BOOL     success = FALSE;
   char     fromname[NAMELEN], toname[NAMELEN];
   char    *cp;

   if(!(fib = (struct FileInfoBlock *)
               AllocMem(sizeof(struct FileInfoBlock), MEMF_CLEAR)))
      goto error;

   fromlock = Lock(from, ACCESS_READ);
   tolock   = Lock(to, ACCESS_READ);

   if(!fib || !fromlock || !tolock)
      goto error;

   if(!Examine(tolock, fib))
      goto error;
   if(fib->fib_DirEntryType<=0)
      goto error;
   if(!Examine(fromlock, fib))
      goto error;
   if(fib->fib_DirEntryType<=0)
      goto error;

   while(ExNext(fromlock, fib)){
      if(fib->fib_DirEntryType>=0)
         continue;
      cp=fib->fib_FileName;
      sprintf(fromname, "%s/%s", from, cp);
      sprintf(toname,   "%s/%s", to, cp);
      if(!CopyFile(fromname, toname, FALSE))
         goto error;

   }
   success = TRUE;
error:
   if(tolock)
      UnLock(tolock);
   if(fromlock)
      UnLock(fromlock);
   if(fib)
      FreeMem((char *)fib, sizeof(struct FileInfoBlock));
   return(success);
}

BOOL CopyFile(from, to, append)
char        *from, *to;
BOOL         append;
{
   BPTR        fromfn, tofn;
   char        buf[1024];
   int         len;
   BOOL        success = FALSE;

   fromfn = Open(from, MODE_OLDFILE);
   if(fromfn <= 0)
      return(FALSE);
   if(!append) {
      tofn = Open(to, MODE_NEWFILE);
      if(tofn <= 0)
         goto closefrom;
   } else {
      tofn = Open(to, MODE_READWRITE);
      if(tofn <= 0)
         goto closefrom;
      Seek(tofn, 0, OFFSET_END);
   }

   do {
      len = Read(fromfn, buf, sizeof buf);
      if(len<0)
         goto closeboth;
      if (len>0) {
         if(Write(tofn, buf, len) < len)
            goto closeboth;
      }
   } while(len>0);
   success = TRUE; /*everything OK */

closeboth:
   Close(tofn);
closefrom:
   Close(fromfn);
   return(success);
}


