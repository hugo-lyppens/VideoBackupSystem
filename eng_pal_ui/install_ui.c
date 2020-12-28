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

#include <exec/types.h>
#include <intuition/intuition.h>

#include "installtext.h"

SHORT BorderVectors1[] = {
   0,0,
   317,0,
   317,77,
   0,77,
   0,0
};
struct Border Border1 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

struct Gadget TextPad = {
   NULL,   /* next gadget */
   28,46,   /* origin XY of hit box relative to window TopLeft */
   316,76,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   NULL,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   0,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

SHORT BorderVectors2[] = {
   0,0,
   121,0,
   121,17,
   0,17,
   0,0
};
struct Border Border2 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

struct TextAttr TOPAZ60 = {
   (STRPTR)"topaz.font",
   TOPAZ_SIXTY,0,0
};
struct IntuiText IText1 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,4,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   ABORT,   /* pointer to text */
   NULL   /* next IntuiText structure */
};

struct Gadget Gadget3 = {
   &TextPad,   /* next gadget */
   28,131,   /* origin XY of hit box relative to window TopLeft */
   120,16,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

SHORT BorderVectors3[] = {
   0,0,
   121,0,
   121,17,
   0,17,
   0,0
};
struct Border Border3 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

struct IntuiText IText2 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   32,4,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   CONTINUE,   /* pointer to text */
   NULL   /* next IntuiText structure */
};

struct Gadget Gadget2 = {
   &Gadget3,   /* next gadget */
   224,131,   /* origin XY of hit box relative to window TopLeft */
   120,16,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   2,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

UBYTE Gadget1SIBuff[40] = REALTIMEDRAWER;
struct StringInfo Gadget1SInfo = {
   Gadget1SIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   40,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

SHORT BorderVectors4[] = {
   0,0,
   317,0,
   317,11,
   0,11,
   0,0
};
struct Border Border4 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

struct Gadget Gadget1 = {
   &Gadget2,   /* next gadget */
   28,30,   /* origin XY of hit box relative to window TopLeft */
   316,10,   /* hit box width and height */
   NULL,   /* gadget flags */
   NULL,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&Gadget1SInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 Gadget1

struct IntuiText IText3 = {
   1,2,JAM2,   /* front and back text pens, drawmode and fill byte */
   40,16,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   RT3DDRAWER,   /* pointer to text */
   NULL   /* next IntuiText structure */
};
struct IntuiText IText4 = {
   1,2,JAM2,   /* front and back text pens, drawmode and fill byte */
   40,16,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   SSPATH,   /* pointer to text */
   NULL   /* next IntuiText structure */
};

#define IntuiTextList1 IText3

struct NewWindow NewWindowStructure1 = {
   107,21,   /* window XY origin relative to TopLeft of screen */
   372,154,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   RMBTRAP+WINDOWDRAG+WINDOWDEPTH+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &Gadget1,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   WINTITLE,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   WBENCHSCREEN   /* destination screen type */
};


/* end of PowerWindows source generation */
