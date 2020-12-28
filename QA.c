#include "vbs.h"
#include <hardware/intbits.h>

struct ErrorSpec {
   long  Blocks;
   long  Bytes;
   long  Zeros;
   long  Ones;
}  errors;

long     VBS_CheckPattern(char *, long, struct ErrorSpec *);
void     VBS_WriteTestPattern(char *);
void     QA_GenerateTestPattern(char *, int, int);


static char              QA_writeheader[64] = " Testpatroon";
static struct Interrupt  QA_vbi = {
   {
      NULL, NULL, NT_INTERRUPT, -60, NULL
   },
   NULL, VBS_VertBServer
};

struct GfxBase *GfxBase;


/*#ifdef   NTSC
#define  PATHEIGHT   13*16
#else*/
#define  PATHEIGHT   16*16
/*#endif*/

char     QA_testpat[4*PATHEIGHT];

main(argc, argv)
int    argc;
char  *argv[];
{
   char     *readheader;
   long      nr;

   QA_writeheader[0] = BT_TESTPATTERN;
   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
   AddIntServer(INTB_VERTB, &QA_vbi);
   VBS_VIInit();
   QA_GenerateTestPattern(QA_testpat, 4, PATHEIGHT);
   if(argc>1) {
      Forbid();
      VBS_WriteBackupHeader(QA_writeheader);
      Permit();
      Delay(50);
      Forbid();
      VBS_WriteTestPattern(QA_testpat);
      Permit();
      Delay(5);
      VBS_ShowBlack();
      Delay(50);
      VBS_RestoreScreen();
   } else {
      for(;;) {
         Forbid(); readheader = VBS_Search(); Permit();
         printf("Ik heb iets %s %d\n", readheader,sizeof(QA_testpat));
         if(!readheader)
            goto quit;
         if(strcmp(readheader, QA_writeheader))
            continue;
         printf("Ik begin nu te testen!\n");
         Forbid();Disable();
         nr = VBS_CheckPattern(QA_testpat, sizeof(QA_testpat), &errors);
         Permit();Enable();
         printf("Klaar: %ld blokken gelezen, moet zijn %ld\n", nr,
            (500*PATHEIGHT/16));
         printf("%ld blokken, %ld bytes, %ld nullen en %ld enen fout\n\n",
            errors.Blocks, errors.Bytes, errors.Zeros, errors.Ones);
         VBS_RestoreScreen();
      }
   }
quit:
   RemIntServer(INTB_VERTB, &QA_vbi);
   if(GfxBase)
      CloseLibrary((struct Library *)GfxBase);
   return(0);
}

void   QA_GenerateTestPattern(pat, w, h)
char  *pat;
int    w, h;
{
   int   i;

   printf("Pattern %lx\n", pat);
   for(i = 0; i<w*h; i++) {
      pat[i] = i&255;
   }
}

