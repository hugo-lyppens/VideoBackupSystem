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

#include "sys.h"
/*fr.c*/
/*Exported: FileRequest FreeDir freq*/

#ifdef DEUTSCH

#define OKTX         "OK"
#define ABORTTX      "Abbr."
#define REREADTX     " Lesen "
#define SELDRIVETX   "Wähle Laufwerk"
#define DRAWERTX     "Verzeichnis"
#define FILETX       "Datei"

#else

#define OKTX         "OK"
#define ABORTTX      "Abort"
#define REREADTX     "Reread "
#define SELDRIVETX   "Select Drive or Path"
#define DRAWERTX     "Drawer:"
#define FILETX       "File  :"

#endif

#define WIDTH     320
#define HEIGHT    200
#define DEPTH     3

#define RWIDTH    234
#define RHEIGHT   170
#define LASTGADGET greread

#define MOVE(rp,x,y) (rp)->cp_x = (x),(rp)->cp_y = (y)

#define SIMPLEGADGET(p,x,y,w,h,id) {p,x,y,w,h, \
   GADGHCOMP,RELVERIFY,REQGADGET|BOOLGADGET,NULL,NULL, \
   NULL,0,0,id,0}

char                   *FileRequest(struct Screen *, struct Window *,
                           char *,char *,char **, char *);
short                   AddDirNode(char *, char);
void                    FreeDir(void);
int                     StrCmpNoCase(char *, char *);
static struct DirNode  *GetEntry(short);
static BPTR             LockDir(char *, struct FileInfoBlock *);
static char             file[70];
static char             olddrawer[50];
static char             xxxstring[] = "XXX:";
static struct DirNode {
   struct DirNode *Next;
   char           Name[27];
   UBYTE          Type;/*if >0 then dir else file*/
}                      *firstnode;
static short            numentries;


static short            rereadxy[]= {0,0,59,0,59,10,0,10,0,0};
static short            abortxy[] = {0,0,43,0,43,10,0,10,0,0};
static short            okxy[]    = {0,0,19,0,19,10,0,10,0,0};

static struct Border    borderreread = {
   0,0,0,0,JAM1,5,&rereadxy[0], NULL
};
static struct Border    borderabort = {
   0,0,0,0,JAM1,5,&abortxy[0],  NULL
};
static struct Border    borderok = {
   0,0,0,0,JAM1,5,&okxy[0],NULL
};
static struct IntuiText treread = {
   0,0,JAM1,2,2,0, REREADTX,NULL
};
static struct IntuiText tabort = {
   0,0,JAM1,2,2,0, ABORTTX,NULL
};
static struct IntuiText tok = {
   0,0,JAM1,2,2,0, OKTX,NULL
};
static struct Gadget    greread = {
   NULL,5,157,(sizeof REREADTX -1)*8+4,11,
   GADGHCOMP,RELVERIFY,REQGADGET|BOOLGADGET,(APTR)&borderreread,NULL,
   &treread,0,0,9,0
};
static struct Gadget    gabortf = {
   &greread,161,157,44,11,
   GADGHCOMP,/*ENDGADGET|*/RELVERIFY,REQGADGET|BOOLGADGET,(APTR)&borderabort,NULL,
   &tabort,0,0,1,0
};
static struct Gadget    gokf = {
   &gabortf,209,157,20,11,
   GADGHCOMP,/*ENDGADGET|*/RELVERIFY,REQGADGET|BOOLGADGET,(APTR)&borderok,NULL,
   &tok,0,0,2,0
};
static struct StringInfo sidrawer = {
   NULL,NULL,0,50
};
static struct Gadget    gdrawer = {
   &gokf,102,130,128,8,
   GADGHCOMP,RELVERIFY,REQGADGET|STRGADGET,0,0,0,0,(APTR)&sidrawer,3,0
};
static struct StringInfo sifile = {
   &file[0],NULL,0,sizeof file
};
static struct Gadget    gfile = {
   &gdrawer,102,140,128,8,
   GADGHCOMP,/*ENDGADGET|*/RELVERIFY,REQGADGET|STRGADGET,0,0,0,0,(APTR)&sifile,4,0
};
static struct Gadget    gpanel[9] = {
   SIMPLEGADGET(&gfile,6,17,209,10,10),
   SIMPLEGADGET(&gpanel[0],6,27,209,10,11),
   SIMPLEGADGET(&gpanel[1],6,37,209,10,12),
   SIMPLEGADGET(&gpanel[2],6,47,209,10,13),
   SIMPLEGADGET(&gpanel[3],6,57,209,10,14),
   SIMPLEGADGET(&gpanel[4],6,67,209,10,15),
   SIMPLEGADGET(&gpanel[5],6,77,209,10,16),
   SIMPLEGADGET(&gpanel[6],6,87,209,10,17),
   SIMPLEGADGET(&gpanel[7],6,97,209,10,18)
};
static SHORT            pdata1[] = {
   ~0xFFFC,
   ~0x8004,
   ~0x8304,
   ~0x8784,
   ~0x8FC4,
   ~0x9FE4,
   ~0xBFF4,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0X8004,
   ~0xFFFC
};
static SHORT            pdata2[] = {
   ~0xFFFC,
   ~0X8004,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0x8784,
   ~0xBFF4,
   ~0x9FE4,
   ~0x8FC4,
   ~0x8784,
   ~0x8304,
   ~0X8004,
   ~0xFFFC
};
static struct Image     pijl1 = {
   -1,-1,14,15,1,
   &pdata1[0],2,28,0
};
static struct Image     pijl2 = {
   -1,-1,14,15,1,
   &pdata2[0],2,28,0
};

static struct Gadget    gk2 = {
   &gpanel[8],    216,94,12,13,
   GADGHCOMP|GADGIMAGE,RELVERIFY,BOOLGADGET,
   (APTR)&pijl2,0,0,0,0,6/*gid*/,0
};
static struct Gadget    gk1 = {
   &gk2,      216,17,12,13,
   GADGHCOMP|GADGIMAGE,RELVERIFY,BOOLGADGET,
   (APTR)&pijl1,0,0,0,0,5/*gid*/,0
};
static struct Image     im1;
static struct PropInfo  pi1 = {
   AUTOKNOB|FREEVERT,
   0,0,0x8000,0x4444,0,0,0,0,0,0
};
static struct Gadget    gk0 = {
   &gk1, 216,31,12,62,
   GADGIMAGE,RELVERIFY,
   PROPGADGET,(APTR)&im1,0,0,0,(APTR)&pi1,7/*gid*/,0
};
static short            gdata[] = {
   115,33,9,GADGHCOMP,RELVERIFY,REQGADGET|BOOLGADGET
};
static short            reqfxy[] = {
   0,11,0,0,RWIDTH-1,0,RWIDTH-1,112,0,112,0,155,
   RWIDTH-1,155,RWIDTH-1,112,RWIDTH-1,RHEIGHT-1,
   0,RHEIGHT-1,0,11,RWIDTH-1,11
};
static struct Border    reqfborder = {
   0,0,0,0,JAM1,(sizeof reqfxy)/4,&reqfxy[0],NULL
};
static struct IntuiText thead = {
   0,0,JAM1,-1,2,0,NULL,NULL
};
static struct IntuiText tdrawer = {
   0,0,JAM1,3,130,0, DRAWERTX,&thead
};
static struct IntuiText tfile = {
   0,0,JAM1,3,140,0, FILETX, &tdrawer
};
struct Requester        freq = {
   NULL,
   0,0,RWIDTH,RHEIGHT,
   0,0,
   &gk0,
   &reqfborder,
   &tfile, /*ReqText*/
   0,5
};

static struct NewWindow FR_newwindow = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   RWIDTH, RHEIGHT,   /* window width and height */
   0,2,   /* detail and block pens */
   GADGETDOWN+GADGETUP+MENUPICK+ACTIVEWINDOW,   /* IDCMP flags */
   BORDERLESS+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

static struct TextAttr TOPAZ80 = {
   (STRPTR)"topaz.font",
   TOPAZ_EIGHTY,0,0
};

char  *FileRequest(s, win, header, drawer, nameptr, presetname)
struct Screen  *s;
struct Window  *win;
char           *header, *drawer, **nameptr, *presetname;
{
   struct Window       *w;
   struct DeviceList   *dl;
   struct Gadget       *g;
   ULONG                idcmp;
   struct RastPort     *rp;
   struct IntuiMessage *message;
   ULONG                class;
   char                *cp, *dp;
   register short       i, j;
   long                 l, k;
   static struct DateStamp oldds;
   static char          oldvolumename[30];
   static char         *slashptr;
   char                *colonptr;
   BPTR                 lock;
   struct DirNode      *dn;
   struct DirNode      *curnode;
   struct FileInfoBlock *fib;
   short                positie;
   char                 dname[16];
   struct MsgPort      *port;
   struct InfoData     *info;
   struct StandardPacket *packet;
   BOOL                 driveseld;
   struct TextFont     *font  =NULL;

   if(!(w = win)) {
      FR_newwindow.LeftEdge = (s->Width-RWIDTH)>>1;
      FR_newwindow.TopEdge  = (s->Height-RHEIGHT)>>1;
      FR_newwindow.Screen   = s;
      if(!(w = OpenWindow(&FR_newwindow)))
         goto weg;
   }
   sidrawer.Buffer=drawer;
   if(!presetname) {
      sifile.Buffer[0]='\0';
   } else {
      strcpy(sifile.Buffer, presetname);
   }
   idcmp=w->IDCMPFlags;
   ModifyIDCMP(w,GADGETUP);
   freq.LeftEdge = (w->Width-RWIDTH)>>1;
   freq.TopEdge  = (w->Height-RHEIGHT)>>1;
   thead.IText=header;thead.LeftEdge = (RWIDTH>>1)-(strlen(header)<<2);
   if(!(fib=(struct FileInfoBlock *)
       AllocMem(sizeof(struct FileInfoBlock),MEMF_CLEAR)))
      goto weg;
   if(!(info = (struct InfoData *) AllocMem(sizeof(struct InfoData),MEMF_CLEAR)))
      goto weg;
   if(!(packet = (struct StandardPacket *)
      AllocMem(sizeof(struct StandardPacket),MEMF_CLEAR)))
         goto weg;
   if(!(port = (struct MsgPort *) CreatePort(NULL,0)))
      goto weg;
   if(!(Request(&freq,w)))
      goto weg;
   ModifyIDCMP(w, GADGETUP);
   rp=freq.ReqLayer->rp;
   font = OpenFont(&TOPAZ80);
   if(!font)
      goto weg;
   SetFont(rp, font);
   SetDrMd(rp, JAM1);
   rp->AOlPen = 0;rp->Flags|=AREAOUTLINE;
   i = 5;
   for(dl = (struct DeviceList *)
      BADDR(((struct DosInfo *)
      BADDR(((struct RootNode *)DOSBase->dl_Root)->rn_Info))->di_DevInfo);
      dl; dl = (struct DeviceList *)BADDR(dl->dl_Next)){
      if( dl->dl_Type==DLT_DEVICE && dl->dl_Task ){
         j=(short)*(char *)(BADDR(dl->dl_Name));
         movmem((char *)(BADDR(dl->dl_Name))+1, dname, j);
         dname[j]=':';dname[j+1]='\0';
/*         packet->sp_Msg.mn_Node.ln_Type=NT_MESSAGE;
         packet->sp_Msg.mn_Node.ln_Name=(char *)&packet->sp_Pkt;
         packet->sp_Msg.mn_ReplyPort=port;
         packet->sp_Pkt.dp_Link=&packet->sp_Msg;
         packet->sp_Pkt.dp_Type=ACTION_DISK_INFO;
         packet->sp_Pkt.dp_Arg1=((LONG)info>>2);
         packet->sp_Pkt.dp_Port=port;
         PutMsg(dl->dl_Task,&packet->sp_Msg);
         WaitPort(port);
         if (info->id_DiskType==ID_DOS_DISK){*/
         if(!(g=(struct Gadget *)AllocMem(sizeof(struct Gadget),MEMF_CLEAR)))
            break;
         g->LeftEdge=i+1;
         SetAPen(rp,31);RectFill(rp,i,115,i+33,125);
         movmem((char *)&gdata,(char *)&g->TopEdge,sizeof gdata);
         g->UserData=(APTR)BADDR(dl->dl_Name);
         g->GadgetID=8;
         AddGList(w,g,-1,1,&freq);
         SetAPen(rp,0);Move(rp,i+1,123);
         if(j+1 > 4)
            j = 3;
         Text(rp,dname,j+1);
         i+=38;
      }
   }
   SetAPen(rp,0);Move(rp,228,16);Draw(rp,228,107);
   
laaddir:
   colonptr = strchr(drawer, ':');
   if(!colonptr || strlen(drawer)<2 )
      goto koe;
   driveseld = memcmp(drawer, xxxstring, 4);
   if (!driveseld ){
      FreeDir();AddDirNode("",0);AddDirNode(SELDRIVETX,0);
      olddrawer[0]='\0';
      goto goed2;
   }

   if(! (lock=LockDir(drawer,fib))){
      *(colonptr+1) = '\0';
      if(! (lock = LockDir(drawer, fib))){
koe:     if(olddrawer[0] && (lock = LockDir(olddrawer, fib))){
            strcpy(drawer, olddrawer);
         } else {
            strcpy(drawer, "DF0:");
            if( !(lock = LockDir(drawer, fib))) {
               strcpy(drawer, xxxstring); goto laaddir;
            }
         }
      }
      driveseld=1;
   }
   dl=(struct DeviceList *)
      BADDR(((struct FileLock *)BADDR(lock))->fl_Volume);
   cp=(char *)BADDR(dl->dl_Name);
   if(!StrCmpNoCase(drawer,olddrawer)){
      if(memcmp((char *)&dl->dl_VolumeDate,(char *)&oldds,
         sizeof(struct DateStamp)))goto laad;
      if(!memcmp(cp,oldvolumename,(int)(*cp+1))){
         goto goed;
      }
   }
laad:
   FreeDir();
   oldds=dl->dl_VolumeDate;
   movmem(cp,oldvolumename,(int)(*cp+1));
   strcpy(olddrawer, drawer);

   slashptr = NULL;
   if(!(slashptr = strrchr(drawer,'/')))
      if( *(colonptr+1) )
         slashptr = colonptr+1;
   if( slashptr )
      AddDirNode(" /",1);

   Examine(lock,fib);

   while(ExNext(lock, fib)){
      cp=fib->fib_FileName;i=strlen(cp);
      if(i<5 || StrCmpNoCase(cp+(i-5),".info")){
         if(AddDirNode(fib->fib_FileName,(char)(fib->fib_DirEntryType>0)))
            break;
      }
   }
goed:
   UnLock(lock);
goed2:
   positie=0;
   curnode=firstnode;
   if(numentries<=9){
      l=0xFFFF;
   }else{
      l=(0xFFFF*9)/numentries;
   }
   ModifyProp(&gk0,w,&freq,AUTOKNOB|FREEVERT,0,0,0x8000,l);
update2:
   SetAPen(rp,31);RectFill(rp,5,16,215,107);
   SetAPen(rp,0);
   dn=curnode;
   j=numentries;if(j>9)j=9;
   for(i=0;i<j;i++){
      Move(rp,6,24+10*i);Text(rp,dn->Name,strlen(dn->Name));
      if(dn->Type>0)
         Text(rp," (DIR)",6);
      dn=dn->Next;
   }
   goto sam;
moos:
   WaitPort(w->UserPort);
sam:
   if(!(message=(struct IntuiMessage *)GetMsg(w->UserPort)))goto moos;
   g=(struct Gadget *)(message->IAddress);
   class=message->Class;
   ReplyMsg((struct Message *)message);
   if(class!=GADGETUP)
      goto sam;
   if(g->GadgetID>=10){
      if( !driveseld )
         goto sam;
      i=g->GadgetID-10;
      if(positie+i>=numentries)
         goto sam;
      dn=curnode;
      while(i){
         dn=dn->Next;i--;
      }
      switch(dn->Type){
      case 1:
         if(dn->Name[0]!=' '){
            if(slashptr)
               strcat(drawer,"/");
            strcat(drawer,dn->Name);
pies:       file[0]='\0';RefreshGList(&gfile,w,&freq,2); /*gfile&gdrawer*/
            goto laaddir;
         } else {
            *slashptr='\0';goto pies;
         }
      case 0:
         strcpy(file, dn->Name);
         RefreshGList(&gfile,w,&freq,2);break;
      }
      goto sam;
   }
   switch(g->GadgetID){
   case 8:
      i = *(UBYTE*)g->UserData;
      movmem((char *)g->UserData+1, dname, i);
      dname[i]=':';dname[i+1]='\0';
      if( driveseld )
         strcpy(drawer, dname);
      else {
         movmem(&drawer[3 /*strlen("XXX") */], &drawer[i], strlen(drawer)-2);
         movmem(dname, drawer, i);
      }
   case 3:
      goto pies;
   case 9: /*reread*/
      olddrawer[0] = '\0';
      goto laaddir;
   case 1:
kaas:
      dp=NULL;goto weg;
   case 2:
   case 4:
      if(!file[0] || !driveseld )goto kaas;
      i=strlen(drawer);
      j=i;if(slashptr){
         j++;
      }
      if(nameptr)
         *nameptr=&file[j];
      movmem(file,file+j,strlen(file)+1);
      movmem(drawer,file,i);
      if(i!=j)
         file[i]='/';
      dp=&file[0];
      goto weg;
   case 5:
      if(positie>0){
         SetBPen(rp,31);ScrollRaster(rp,0,-10,6,17,214,106);
         positie--;curnode=GetEntry(positie);
         Move(rp,6,24);
         dn=curnode;
inspr:   Text(rp,dn->Name,strlen(dn->Name));
         if(dn->Type>0)
            Text(rp," (DIR)",6);
         k=(positie*0xFFFF)/(numentries-9);
         ModifyProp(&gk0,w,&freq,AUTOKNOB|FREEVERT,0,k,0x8000,pi1.VertBody);
      }
      break;
   case 6:
      if(positie<numentries-9){
         SetBPen(rp,31);ScrollRaster(rp,0,10,6,17,214,106);
         positie++;curnode=curnode->Next;
         for(i=0,dn=curnode;i<8;i++,dn=dn->Next)
            ;
         Move(rp,6,104);goto inspr;
      }
      break;
   case 7:
      if(numentries>9){
         k=(pi1.VertPot*(numentries-9))/0xFFFF;positie=k;
         curnode=GetEntry(positie);
         goto update2;
      }
      break;
   }
   goto sam;
weg:
   g=LASTGADGET.NextGadget;
   while(g){
      cp=(char *)g;g=g->NextGadget;FreeMem(cp,sizeof(struct Gadget));
   }
   LASTGADGET.NextGadget=NULL;
   EndRequest(&freq,w);
   ModifyIDCMP(w,idcmp);
   if (fib)    FreeMem((char *)fib, sizeof(struct FileInfoBlock));
   if (port)   DeletePort(port);
   if (info)   FreeMem((char *)info, sizeof(struct InfoData));
   if (packet) FreeMem((char *)packet, sizeof(struct StandardPacket));
   if (!win && w) CloseWindow(w);
   if(font)CloseFont(font);
   return(dp);
}
short AddDirNode(name,type)
char        *name;
register char type;
{
   register struct DirNode *node2;
   struct DirNode          *node,*node1;
   register                status;

   node1 = (struct DirNode *)&firstnode;
   node2 = firstnode;

   while(node2){
      if(node2->Type<type)
         break;
      else if(node2->Type==type){
         status=StrCmpNoCase(node2->Name,name);
         if(status>0)
            break;
         if(!status){
            node=node2;goto getname;
         }
      }
      node1 = node2; node2 = node2->Next;
   }
   if(!(node=(struct DirNode *)AllocMem(sizeof(struct DirNode),0)))
      return(-1);
   node1->Next=node;node->Next=node2;
   node->Type=type;
   numentries++;
getname:
   stccpy(node->Name,name,sizeof node->Name);
   return(0);
}
void FreeDir()
{
   register struct DirNode *n,*n1;

   n=firstnode;
   while(n){
      n1=n->Next;FreeMem((char*)n,sizeof(struct DirNode));
      n=n1;
   }
   firstnode=0;numentries=0;olddrawer[0]='\0';
   return;
}
static BPTR LockDir(drawer, fib)
char                 *drawer;
struct FileInfoBlock *fib;
{
   BPTR     lock;

   lock = Lock(drawer, ACCESS_READ);
   if(lock) {
      Examine(lock, fib);
      if(fib->fib_DirEntryType <= 0) {
         UnLock(lock); lock = NULL;
      }
   }
   return(lock);
}

static struct DirNode *GetEntry(i)
register short i;
{
   register struct DirNode *dn;

   dn=firstnode;
   while(i){
      dn=dn->Next;i--;
   }
   return(dn);
}
/*int StrCmpNoCase(s1,s2)
register char *s1,*s2;
{
   register char   status, c1, c2;


   do {
      c1 = toupperf(*s1++); c2 = toupperf(*s2++);

      status = c1-c2;
   } while(!status&&c1&&c2);

   return((int)status);
}

static char toupperf(c)
char     c;
{
   if(c>='a' && c<='z')
      return((char)(c-('a'-'A')));
   else
      return(c);
}
  */
