#include "headers.h"
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

extern struct Custom custom;


#define BITSPERBYTE  8
#define BYTELEN      (startlen+(BITSPERBYTE+2)*datalen)
#define NKOL         1

long     main(int, char **);
void     WriteByte(struct RastPort *, int, int, UBYTE);
static char naam[] = 
	"HMHL9a70";
struct GfxBase       *GfxBase;
struct IntuitionBase *IntuitionBase;

short                 color2[] = {
   0x000, 0xfff
};
struct   NewScreen    ns = {
   0,0,320,256,1,0,31,0, CUSTOMSCREEN|SCREENQUIET,
   0,NULL,0,0
};
struct   NewWindow    nw = {
   0, 0, 320,256, 1,0,0,
   ACTIVATE|SIMPLE_REFRESH|BORDERLESS|RMBTRAP|NOCAREREFRESH,
   NULL, NULL, NULL, NULL, NULL,
   0,0,0,0,CUSTOMSCREEN
};


struct Screen        *s;
struct Window        *w;
struct RastPort      *rp;
int                   startlen, datalen;

UWORD __chip pointer[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

#define CIAAPRA		(*(char *)0xbfe001)
#define CIABPRA		(*(char *)0xbfd000)

long   main(argc, argv)
int       argc;
char    **argv;
{
   int   yp, i;

   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
   IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);

   startlen = 32;
   datalen = 32;

   s = OpenScreen(&ns); nw.Screen = s;
   w = OpenWindow(&nw);
   LoadRGB4(&s->ViewPort, &color2[0], 16);
   SetPointer(w, pointer, 1, 16, 0, 0);
   rp = &s->RastPort;
   SetAPen(rp, 1);      /*wit*/

   for(yp = 0; yp<256; yp++) {
      WriteByte(rp, 0, yp, yp);
   }
   CIAAPRA |= 2;
   CIABPRA |= 128;
   CIABPRA &= ~192;
   Forbid(); Disable();
   
   custom.serper = 15;
   custom.intreq = INTF_RBF|INTF_VERTB;
   for(;;) {
fout:   
	while(!(custom.intreqr & INTF_VERTB))
		;
opnieuw:	
	custom.intreq = INTF_RBF|INTF_VERTB;

      	for(yp = 0; yp<256; yp++) {
      		i = custom.serdatr;
      		while(!(i & 0x4000)) {
      			if(custom.intreqr & INTF_VERTB) {
      				CIAAPRA |= 2;
      				goto opnieuw;
      			}
      			if(!(CIAAPRA & 64))
      				goto klaar;
      			i = custom.serdatr;
      		}
      		if((i & 0xFF)!=yp) {
			CIAAPRA |= 2;
      			goto fout;
      		}
      		custom.intreq = INTF_RBF;
      	}
      	CIAAPRA &= ~2;
   }
klaar:   
   Enable(); Permit();
      
   while(CIAAPRA & 64)
      ;
   CloseWindow(w);
   CloseScreen(s);
   CloseLibrary((struct Library *)IntuitionBase);
   CloseLibrary((struct Library *)GfxBase);
   CIAAPRA &=~2;
   return(0L);
}
void WriteByte(rp, x, y, byte)
struct RastPort *rp;
int              x, y;
UBYTE            byte;
{
   int   i;

   Move(rp, x, y); Draw(rp, x+startlen-1, y);
   x+=startlen;
   for(i = 0; i<BITSPERBYTE; i++) {
      if(!(byte & 1)) {
         Move(rp, x, y); Draw(rp, x+datalen-1, y);
      }
      x+=datalen;
      byte>>=1;
   }
}

