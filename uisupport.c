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

BOOL UIG_FileExists(name)
char  *name;
{
   BPTR   lock;

   lock  = Lock(name, ACCESS_READ);
   if(lock) {
      UnLock(lock);
      return(1);
   } else {
      return(0);
   }
}
void UIG_NewView(w, l, what, leftxp, toprowp)
struct Window    *w;
VBS_ListData_t   *l;
int               what;
int              *leftxp, *toprowp;
{
   struct PropInfo  *pih, *piv;
   int               x, row;
   int               wd, hd;
   long              k;
   struct Gadget    *gh, *gv;

   x   = l->LeftX;
   row = l->TopRow;
   wd  = l->MaxTotalWidth - l->WinWidth;
   hd  = l->NumRows - l->WinRows;

   gh  = l->HorizGadget;
   gv  = l->VertGadget;
   pih = (struct PropInfo *)(gh->SpecialInfo);
   piv = (struct PropInfo *)(gv->SpecialInfo);
   switch(what) {
   case HORIZ:
      k = 0;
      if(wd>0)
         k = (pih->HorizPot*wd)/MAXPOT;
      x = k;
      break;
   case VERT:
      k = 0;
      if(hd>0)
         k = (piv->VertPot*hd)/MAXPOT;
      row = k;
      break;
   }
   if(what >=ARROW_LEFT) {
      switch(what) {
      case ARROW_LEFT:
         x = x>=32 ? x-32 : 0;
         break;
      case ARROW_RIGHT:
         if(wd > 0)
            x = (x <= wd-32) ? x+32 : wd;
         break;
      case ARROW_UP:
         if(row>0)
            row--;
         break;
      case ARROW_DOWN:
         if(row < hd)
            row++;
         break;
      }
      UIG_AdjustProps(w, l, x, row);
   }
   *leftxp  = x;
   *toprowp = row;
}


void UIG_AdjustProps(w, l, leftx, toprow)
struct Window  *w;
VBS_ListData_t *l;
int             leftx, toprow;
{
   long             pot, body;
   struct Gadget   *gh, *gv;


   gh = l->HorizGadget; gv = l->VertGadget;

      pot  = 0;   body = 0xFFFF;
      if(l->MaxTotalWidth > l->WinWidth) {
         body = (0x0FFFFL*(long)l->WinWidth) / l->MaxTotalWidth;
         pot  = (0x0FFFFL*(long)leftx)/(l->MaxTotalWidth-l->WinWidth);
      }
      UIG_NewModifyProp(gh, w, NULL, AUTOKNOB|FREEHORIZ,
         pot, 0xFFFF, body, 0xFFFF ,1);

      pot  = 0;
      body = 0xFFFF;
      if(l->NumRows > l->WinRows) {
         body = (0x0FFFFL*(long)l->WinRows) / l->NumRows;
         pot  = (0x0FFFFL*(long)toprow)/(l->NumRows-l->WinRows);
      }
      UIG_NewModifyProp(gv, w, NULL, AUTOKNOB|FREEVERT,
         0xFFFF, pot, 0xFFFF, body ,1);
}

void  UIG_NewModifyProp(gadget, window, requester, flags,
         horizpot, vertpot, horizbody, vertbody, n)
struct Gadget     *gadget;
struct Window     *window;
struct Requester  *requester;
long               flags, horizpot, vertpot, horizbody, vertbody, n;
{
   register struct  PropInfo  *pi;

   pi = (struct PropInfo *)gadget->SpecialInfo;
   if(!pi)
      return;
   if(pi->Flags != flags ||
      pi->HorizPot != horizpot || pi->VertPot != vertpot ||
      pi->HorizBody != horizbody || pi->VertBody != vertbody) {
         NewModifyProp(gadget, window, requester, flags,
            horizpot, vertpot, horizbody, vertbody, n);
   }
}

struct Window *UIG_OpenWindow(screen, nw, color)
struct Screen    *screen;
struct NewWindow *nw;
int               color;
{
   struct Gadget         *first;
   struct RastPort       *rp;
   struct Window         *w;

   first = nw->FirstGadget;
   nw->FirstGadget = NULL;
   nw->Screen      = screen;
   w = OpenWindow(nw);
   nw->FirstGadget = first;
   if(!w)
      return(NULL);
   rp = w->RPort;
   SetRast(rp, color);
   UIG_ProcessGadgets(rp, first);
   AddGList(w, first, -1, 100, NULL);
   return(w);
}
void  UIG_ProcessRequester(req)
struct Requester  *req;
{
   struct RastPort *rp;

   rp = req->ReqLayer->rp;

   Rect(rp, 0, 0, req->Width, req->Height, 3);

   UIG_ProcessGadgets(rp, req->ReqGadget);
   RefreshGList(req->ReqGadget, req->RWindow, req, 100);
}

void  UIG_ProcessGadgets(rp, g)
struct RastPort *rp;
struct Gadget   *g;
{
   while(g) {
      UIG_ProcessGadget(rp, g);
      g = g->NextGadget;
   }
}

void  UIG_ProcessGadget(rp, g)
struct RastPort *rp;
struct Gadget   *g;
{
   int      x, y;
   long     l;

      l = (ULONG)g->UserData;
      if(l & BACKFILL) {
         x = g->LeftEdge; y = g->TopEdge;
         SetAPen(rp, l & BCOLORMASK);
         RectFill(rp, x, y, x+g->Width-1, y+g->Height-1);
      }
}

void UIG_ChangeGadgetText(window, gadget, text, seq)
struct Window *window;
struct Gadget *gadget;
char          *text;
int            seq;
{
   struct IntuiText  *itext;


   if(itext = gadget->GadgetText) {
      while(itext->NextText && seq) {
         itext  = itext->NextText; seq--;
      }
      itext->IText = text;
      UIG_ProcessGadget(window->RPort, gadget);
      RefreshGList(gadget, window, NULL, 1);
   }
}

BOOL UIG_InitPad(ld, window, fontname, fontheight)
VBS_ListData_t *ld;
struct Window  *window;
char           *fontname;
int             fontheight;
{
   struct RastPort  *rp, *rppad;
   BOOL              error = TRUE;

   rp = window->RPort;
   SetDrMd(rp, JAM2);
   SetAPen(rp, 3);
   RectFill(rp, ld->ArrowLeft->LeftEdge, ld->ArrowLeft->TopEdge,
                               ld->ArrowRight->LeftEdge,
                               ld->ArrowRight->TopEdge+ld->ArrowRight->Height-1);
   RectFill(rp, ld->ArrowUp->LeftEdge, ld->ArrowUp->TopEdge,
                               ld->ArrowDown->LeftEdge+ld->ArrowDown->Width-1,
                               ld->ArrowDown->TopEdge);

   ld->LayerInfo = NewLayerInfo();
   if(!ld->LayerInfo)
      goto quit;

   if(!(ld->Layer = CreateUpfrontLayer(ld->LayerInfo,
      &window->WScreen->BitMap,
      ld->PadGadget->LeftEdge, ld->PadGadget->TopEdge + window->TopEdge,
      ld->PadGadget->LeftEdge + ld->PadGadget->Width-1,
      ld->PadGadget->TopEdge  + ld->PadGadget->Height-1 + window->TopEdge,
      LAYERSIMPLE,NULL)))
         goto quit;
   rppad = ld->Layer->rp;
   ld->RastPort = rppad;
   if(fontname && fontheight)
      ld->Font = UIG_OpenFont(fontname, fontheight);
   if(ld->Font)
      SetFont(rppad, ld->Font);

   ld->WinRows    = ld->PadGadget->Height/rppad->TxHeight;

   ld->WinHeight  = ld->WinRows  * rppad->TxHeight;

   ld->WinWidth   = ld->PadGadget->Width;
   ld->Wwidth     = TextLength(rppad, "W", 1);
   error = FALSE;
quit:
   return(error);
}

void UIG_CleanupPad(ld)
VBS_ListData_t *ld;
{
   if(ld->Layer) {
      DeleteLayer(ld->LayerInfo , ld->Layer);
      DisposeLayerInfo(ld->LayerInfo);
   }
   if(ld->Font)
      CloseFont(ld->Font);
}
/*extern struct Library *DiskfontBase;*/

extern struct TextAttr VBS_ta8;

struct TextFont *UIG_OpenFont(name, height)
char     *name;
int       height;
{
   static BOOL       tried_diskfont = FALSE;
   struct TextAttr   ta;
   struct TextFont  *font = NULL;

   ta.ta_Name = name; ta.ta_YSize = height;
   ta.ta_Style = 0; ta.ta_Flags = 0;
   font = OpenFont(&ta);
   if(font && font->tf_YSize != height) {
      CloseFont(font); font = NULL;
   }
   if(!font) {
      if(!DiskfontBase && !tried_diskfont) {
         DiskfontBase = OpenLibrary("diskfont.library", 0);
         tried_diskfont = TRUE;
      }
      if(DiskfontBase)
         font = OpenDiskFont(&ta);
   }
   if(!font)
      font = OpenFont(&VBS_ta8);
   return(font);
}

void Rect(rp, x, y, w, h, c)
struct RastPort   *rp;
int                x, y, w, h, c;
{
   SetAPen(rp, c);
   Move(rp, x, y); Draw(rp, x+w-1, y);
   Draw(rp, x+w-1, y+h-1); Draw(rp, x, y+h-1); Draw(rp, x, y);
}


static UBYTE   dayspermonth[] = {
   31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
static UBYTE   monthname[][4] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
   "Nov", "Dec"
};


int UIG_Date(s, days)
char     *s;
register long      days;
{
   register long year, year_days, month, feb;

   year = 78+(days/(365*3+366))*4;
   days %= 1461L;

   for(;;) {
      if(year & 3) {
         year_days = 365;
         feb = 28;
      } else {
         year_days = 366;
         feb = 29;
      }
      if(days >= year_days) {
         days -= year_days;
         year++;
      } else
         break;
   }
   dayspermonth[1] = feb;
   for(month = 0; month<12; month++) {
      if(days < (long)(dayspermonth[month]))
         break;
      days-=(long)(dayspermonth[month]);
   }
   sprintf(s, "%2ld-%s-%02ld", days+1, monthname[month], year % 100);
   return(2+1+3+1+2);
}

int UIG_Time(s, mins, ticks)
char     *s;
long      mins, ticks;
{
   sprintf(s, "%02ld:%02ld:%02ld", mins/60, mins%60, ticks/TICKS_PER_SECOND);
   return(2+1+2+1+2);
}

void UIG_ToggleGadget(rp,g)
register struct RastPort *rp;
struct Gadget *g;
{
   register short x,y;

   g->Flags^=SELECTED;x=g->LeftEdge;y=g->TopEdge;
   SetDrMd(rp,JAM1|COMPLEMENT);RectFill(rp,x,y,x+g->Width-1,y+g->Height-1);
   SetDrMd(rp,JAM1);
   return;
}

BOOL   Write2(fn, buffer, len)
BPTR      fn;
char     *buffer;
long      len;
{
   return((BOOL)(Write(fn, buffer, len)<len));
}
BOOL  UIG_DelayLMB(delay)
long        delay;
{
   long     i;

   for(i = 0; i<(delay>>1) ; i++) {
      if(LPRESSED)
         return(FALSE);
      Delay(2);
   }
   return(TRUE);
}


char     *UIG_MakeProtString(protection)
ULONG     protection;
{
   static char        protbitnames[] = "hsparwed",
                      protstring[9] = "--------";
   USHORT             prot, i, bm;

   prot = protection ^ 0xF;
   for(bm = 128, i = 0; i<8; bm >>=1, i++ ) {
      protstring[i] = prot&bm ? protbitnames[i] : '-';
   }

   return(protstring);
}

