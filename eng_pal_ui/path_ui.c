
static SHORT pathBorderVectors1[] = {
   0,0,
   91,0,
   91,13,
   0,13,
   0,0
};
static struct Border pathBorder1 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   pathBorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct TextAttr TOPAZ60 = {
   (STRPTR)"topaz.font",
   TOPAZ_SIXTY,0,0
};
static struct IntuiText pathIText1 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Cancel",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget pathGadget3 = {
   NULL,   /* next gadget */
   316,82,   /* origin XY of hit box relative to window TopLeft */
   90,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&pathBorder1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &pathIText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   PATH_CANCEL,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT pathBorderVectors2[] = {
   0,0,
   51,0,
   51,13,
   0,13,
   0,0
};
static struct Border pathBorder2 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   pathBorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText pathIText2 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   18,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget pathGadget2 = {
   &pathGadget3,   /* next gadget */
   253,82,   /* origin XY of hit box relative to window TopLeft */
   50,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&pathBorder2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &pathIText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   PATH_OK,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static UBYTE pathpathGadget1SIBuff[80];
static struct StringInfo pathpathGadget1SInfo = {
   pathpathGadget1SIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT pathBorderVectors3[] = {
   0,0,
   377,0,
   377,9,
   0,9,
   0,0
};
static struct Border pathBorder3 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   pathBorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget pathGadget1 = {
   &pathGadget2,   /* next gadget */
   30,38,   /* origin XY of hit box relative to window TopLeft */
   376,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&pathBorder3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&pathpathGadget1SInfo,   /* SpecialInfo structure */
   PATH_STRING,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define pathGadgetList1 pathGadget1

static struct IntuiText pathIText3 = {
   1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   31,16,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Please enter full AmigaDOS path to backup from:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

#define pathIntuiTextList1 pathIText3

static struct Requester pathRequesterStructure1 = {
   NULL,   /* previous requester (filled in by Intuition) */
   100,55,   /* requester XY origin relative to TopLeft of window */
   440,100,   /* requester width and height */
   0,0,   /* relative to these mouse offsets if POINTREL is set */
   &pathGadgetList1,   /* gadget list */
   NULL,   /* box's border */
   &pathIntuiTextList1,   /* requester text */
   NULL,   /* requester flags */
   2,   /* back-plane fill pen */
   NULL,   /* leave these alone */
   NULL,   /* custom bitmap if PREDRAWN is set */
   NULL   /* leave this alone */
};
