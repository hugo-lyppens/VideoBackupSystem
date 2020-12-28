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

#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include "vbs.h"


extern UBYTE            VBSG_errorinfo[];
extern struct TextAttr  VBSG_pad_ta;
extern struct AudChannel aud[];
extern UWORD		dmacon;
UWORD __chip WaitPointer[] =
{
   0x0000, 0x0000,

   0x0400, 0x07C0,
   0x0000, 0x07C0,
   0x0100, 0x0380,
   0x0000, 0x07E0,
   0x07C0, 0x1FF8,
   0x1FF0, 0x3FEC,
   0x3FF8, 0x7FDE,
   0x3FF8, 0x7FBE,
   0x7FFC, 0xFF7F,
   0x7EFC, 0xFFFF,
   0x7FFC, 0xFFFF,
   0x3FF8, 0x7FFE,
   0x3FF8, 0x7FFE,
   0x1FF0, 0x3FFC,
   0x07C0, 0x1FF8,
   0x0000, 0x07E0,

   0x0000, 0x0000
};

struct Requester WaitRequest;
BOOL             BlockedWindow;

BOOL UIG_FileExists(name)
UBYTE *name;
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
        GT_SetGadgetAttrs(l->HorizGadget, w, NULL,
                           GTSC_Total,   l->MaxTotalWidth,
                           GTSC_Visible, l->WinWidth,
                           GTSC_Top,     leftx,
                           TAG_END);
      
	GT_SetGadgetAttrs(l->VertGadget, w, NULL,
                           GTSC_Total,   l->NumRows,
                           GTSC_Visible, l->WinRows,
                           GTSC_Top,     toprow,
                           TAG_END);
}

BOOL UIG_InitPad(ld, window)
VBS_ListData_t  *ld;
struct Window   *window;
{
   struct RastPort        *rppad;
   BOOL                    error = TRUE;
   struct Rectangle	   rect;
   
   ld->Region = NULL;	
   ld->RastPort = *(window->RPort);
   
   rppad = &ld->RastPort;
   SetDrMd(rppad, JAM2);
   SetAPen(rppad, 3);
#if 0
   ld->LayerInfo = NewLayerInfo();
   if(!ld->LayerInfo)
      goto quit;

   if(!(ld->Layer = CreateUpfrontLayer(ld->LayerInfo,
      &window->WScreen->BitMap,
      ld->PadGadget->LeftEdge + window->LeftEdge,
      ld->PadGadget->TopEdge  + window->TopEdge,
      ld->PadGadget->LeftEdge + ld->PadGadget->Width-1  + window->LeftEdge,
      ld->PadGadget->TopEdge  + ld->PadGadget->Height-1 + window->TopEdge,
      LAYERSIMPLE, NULL)))
         goto quit;
   rppad = ld->Layer->rp;
   ld->RastPort = rppad;
#endif   
   if(&VBSG_pad_ta)
      ld->Font = UIG_OpenFont(&VBSG_pad_ta);
   if(ld->Font)
      SetFont(rppad, ld->Font);

   ld->LeftEdge	  = ld->PadGadget->LeftEdge;
   ld->TopEdge	  = ld->PadGadget->TopEdge;
   ld->WinRows    = ld->PadGadget->Height/rppad->TxHeight;

   ld->WinHeight  = ld->WinRows  * rppad->TxHeight;

   ld->WinWidth   = ld->PadGadget->Width;

   ld->Region = NewRegion();
   rect.MinX	= ld->LeftEdge;
   rect.MinY	= ld->TopEdge;
   rect.MaxX	= ld->LeftEdge + ld->WinWidth-1;
   rect.MaxY	= ld->TopEdge  + ld->WinHeight-1;
   OrRectRegion(ld->Region, &rect);   
   ld->Wwidth     = TextLength(rppad, "W", 1);
   error = FALSE;
quit:
   return(error);
}

void 	UIG_CleanupPad(ld)
VBS_ListData_t *ld;
{
#if 0
   if(ld->Layer) {
      DeleteLayer(ld->LayerInfo , ld->Layer);
      DisposeLayerInfo(ld->LayerInfo);
   }
#endif   
   if(ld->Region)
   	DisposeRegion(ld->Region);
   if(ld->Font)
      	CloseFont(ld->Font);
}
void 	UIG_FillPadBgnd(ld, color)
VBS_ListData_t *ld;
int		color;
{
	struct RastPort	*rp;
	
	rp = &ld->RastPort;
	SetAPen(rp, color);
	RectFill(rp, ld->PadGadget->LeftEdge, ld->PadGadget->TopEdge,
      		     ld->PadGadget->LeftEdge + ld->PadGadget->Width-1,
		     ld->PadGadget->TopEdge  + ld->PadGadget->Height-1);
}
void	UIG_FillWindowBgnd(w, color)
struct Window	*w;
int		 color;
{
	struct RastPort	*rp;
	
	rp = w->RPort;
	SetAPen(rp, color);
	RectFill(rp, w->BorderLeft, w->BorderTop,
		     w->Width -1-w->BorderRight, 
		     w->Height-1-w->BorderBottom);
}
/*extern struct Library *DiskfontBase;*/

extern struct TextAttr VBS_ta8;

VOID UIG_MakeTextAttr(struct TextAttr *ta, UBYTE *name, int height)
{
   ta->ta_Name  = name;
   ta->ta_YSize = height;
   ta->ta_Style = 0;
   ta->ta_Flags = 0;
}

struct TextFont *UIG_OpenFont(struct TextAttr *ta)
{
   static BOOL       tried_diskfont = FALSE;
   struct TextFont  *font = NULL;

   font = OpenFont(ta);
   if(font && font->tf_YSize != ta->ta_YSize) {
      CloseFont(font); font = NULL;
   }
   if(!font) {
      if(!DiskfontBase && !tried_diskfont) {
         DiskfontBase = OpenLibrary("diskfont.library", 0);
         tried_diskfont = TRUE;
      }
      if(DiskfontBase)
         font = OpenDiskFont(ta);
   }
   if(!font)
   return(font);
}

struct TextFont *UIG_OpenDefaultFont(void)
{
   struct TextAttr  topaz80 = { "topaz.font", 8, 0, 0 };

   return(OpenFont(&topaz80));
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
UBYTE    *s;
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
UBYTE    *s;
long      mins, ticks;
{
   sprintf(s, "%02ld:%02ld:%02ld", mins/60, mins%60, ticks/TICKS_PER_SECOND);
   return(2+1+2+1+2);
}

BOOL   Write2(fn, buffer, len)
BPTR      fn;
UBYTE    *buffer;
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

UBYTE    *UIG_MakeProtString(protection)
ULONG     protection;
{
   static UBYTE       protbitnames[] = "hsparwed",
                      protstring[9] = "--------";
   USHORT             prot, i, bm;

   prot = protection ^ 0xF;
   for(bm = 128, i = 0; i<8; bm >>=1, i++ ) {
      protstring[i] = prot&bm ? protbitnames[i] : '-';
   }

   return(protstring);
}

VOID UIG_BlockInput(struct Window *win)
{
   extern UWORD __chip      WaitPointer[];
   extern struct Requester  WaitRequest;
   extern BOOL              BlockedWindow;

   InitRequester(&WaitRequest);
   if(Request(&WaitRequest, win))
   {
      SetPointer(win, WaitPointer, 16, 16, -6, 0);
      BlockedWindow = TRUE;
   }
   else
      BlockedWindow = FALSE;
}

VOID UIG_UnBlockInput(struct Window *win)
{
   extern struct Requester  WaitRequest;
   extern BOOL              BlockedWindow;

   if(BlockedWindow)
   {
      ClearPointer(win);
      EndRequest(&WaitRequest, win);
   }
}

#define CLOCK	3579545

UWORD __chip WaveForm = 0x807F;

VOID UIG_Beep(freq)
int	 freq;
{
	if(freq) {
		aud[0].ac_ptr = &WaveForm;
		aud[0].ac_len = 1;
		aud[0].ac_per = (CLOCK/2)/freq;
		aud[0].ac_vol = 64;
		dmacon = DMAF_SETCLR|DMAF_AUD0;
	} else {
		aud[0].ac_vol = 0;
		dmacon = DMAF_AUD0;
	}
}

	