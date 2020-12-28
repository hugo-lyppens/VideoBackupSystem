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

static int       SRT_CompareDirsByName(VBS_DirNode_t *, VBS_DirNode_t *);
static int       SRT_CompareFilesByName(VBS_FileNode_t *, VBS_FileNode_t *);

void          SRTG_SortLL(list, compare)
VBS_Node_t  **list;
int         (*compare)();/*VBS_Node_t *, VBS_Node_t *);*/
{
   BOOL         changed = TRUE;
   VBS_Node_t  *prevnode, *nextnode, *node, *temp;

   while(changed) {
      changed = FALSE;
      prevnode = (VBS_Node_t *)list;
      if(list)
         node     = *list;
      else
         return;

      if(node)
         nextnode = node->Next;
      else
         return;

      while(nextnode) {
         if(compare(node, nextnode)>0) {
            prevnode->Next = nextnode;
            node->Next     = nextnode->Next;
            nextnode->Next = node;
            changed        = TRUE;
            temp = node; node = nextnode; nextnode = temp;
         }
         prevnode = node; node = nextnode; nextnode = nextnode->Next;
      }
   }
   return;
}
static int SRT_CompareDirsByName(dn1, dn2)
VBS_DirNode_t   *dn1, *dn2;
{
   return(StrCmpNoCase(dn1->Name, dn2->Name));
}
static int SRT_CompareFilesByName(fn1, fn2)
VBS_FileNode_t  *fn1, *fn2;
{
   return(StrCmpNoCase(fn1->Name, fn2->Name));
}
/*
int StrCmpNoCase(s1,s2)
register UBYTE *s1, *s2;
{
   register UBYTE  status, c1, c2;


   do {
      c1 = toupperf(*s1++); c2 = toupperf(*s2++);

      status = c1-c2;
   } while(!status&&c1&&c2);

   return((int)status);
}
*/
UBYTE toupperf(c)
UBYTE    c;
{
   if(c>='a' && c<='z')
      return((UBYTE)(c-('a'-'A')));
   else
      return(c);
}

void  SRTG_SortTree(dnp)
VBS_DirNode_t  **dnp;
{
   VBS_DirNode_t  *dn;

   SRTG_SortLL((VBS_Node_t **)dnp, SRT_CompareDirsByName);
   while(dn = *dnp) {
      SRTG_SortLL((VBS_Node_t **)&dn->FirstFile, SRT_CompareFilesByName);
      SRTG_SortTree(&dn->FirstDir);
      dnp = (VBS_DirNode_t **)dn;
   }
}

