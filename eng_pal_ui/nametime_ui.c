
static struct Gadget name_reportfileg = {
   NULL,   /* next gadget */
   222,70,   /* origin XY of hit box relative to window TopLeft */
   215,9,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   NULL,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   NULL,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT nameBorderVectors1[] = {
   0,0,
   91,0,
   91,13,
   0,13,
   0,0
};
static struct Border nameBorder1 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   nameBorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct TextAttr TOPAZ60 = {
   (STRPTR)"topaz.font",
   TOPAZ_SIXTY,0,0
};
static struct IntuiText nameIText1 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Cancel",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget nameGadget6 = {
   &name_reportfileg,   /* next gadget */
   311,100,   /* origin XY of hit box relative to window TopLeft */
   90,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&nameBorder1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &nameIText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NAME_CANCEL,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT nameBorderVectors2[] = {
   0,0,
   51,0,
   51,13,
   0,13,
   0,0
};
static struct Border nameBorder2 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   nameBorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText nameIText2 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   18,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget nameGadget5 = {
   &nameGadget6,   /* next gadget */
   246,100,   /* origin XY of hit box relative to window TopLeft */
   50,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&nameBorder2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &nameIText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NAME_OK,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static UBYTE namename_namegadgetSIBuff[80];
static struct StringInfo namename_namegadgetSInfo = {
   namename_namegadgetSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT nameBorderVectors3[] = {
   0,0,
   257,0,
   257,9,
   0,9,
   0,0
};
static struct Border nameBorder3 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   nameBorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget name_namegadget = {
   &nameGadget5,   /* next gadget */
   145,16,   /* origin XY of hit box relative to window TopLeft */
   256,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   NULL,   /* activation flags */
   STRGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&nameBorder3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&namename_namegadgetSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE namename_timegadgetSIBuff[9] =
   "HH:MM:SS";
static struct StringInfo namename_timegadgetSInfo = {
   namename_timegadgetSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   9,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT nameBorderVectors4[] = {
   0,0,
   73,0,
   73,9,
   0,9,
   0,0
};
static struct Border nameBorder4 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   nameBorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText nameIText3 = {
   1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   -299,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Tape Start Position:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget name_timegadget = {
   &name_namegadget,   /* next gadget */
   329,35,   /* origin XY of hit box relative to window TopLeft */
   72,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   NULL,   /* activation flags */
   STRGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&nameBorder4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &nameIText3,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&namename_timegadgetSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT nameBorderVectors5[] = {
   0,0,
   371,0,
   371,10,
   0,10,
   0,0
};
static struct Border nameBorder5 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   nameBorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText nameIText4 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   50,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Set Archive-bit on backed-up files",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget name_archgadget = {
   &name_timegadget,   /* next gadget */
   31,54,   /* origin XY of hit box relative to window TopLeft */
   370,9,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&nameBorder5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &nameIText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NAME_ARCH,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT nameBorderVectors6[] = {
   0,0,
   180,0,
   180,10,
   0,10,
   0,0
};
static struct Border nameBorder6 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   nameBorderVectors6,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText nameIText5 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   18,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Send Report To ...",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget name_reportgadget = {
   &name_archgadget,   /* next gadget */
   31,70,   /* origin XY of hit box relative to window TopLeft */
   179,9,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&nameBorder6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &nameIText5,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NAME_REPORT,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

#define nameGadgetList1 name_reportgadget

static struct IntuiText nameIText6 = {
   1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   31,16,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Backup Name:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

#define nameIntuiTextList1 nameIText6

static struct Requester nameRequesterStructure1 = {
   NULL,   /* previous requester (filled in by Intuition) */
   100,55,   /* requester XY origin relative to TopLeft of window */
   440,120,   /* requester width and height */
   0,0,   /* relative to these mouse offsets if POINTREL is set */
   &nameGadgetList1,   /* gadget list */
   NULL,   /* box's border */
   &nameIntuiTextList1,   /* requester text */
   NULL,   /* requester flags */
   2,   /* back-plane fill pen */
   NULL,   /* leave these alone */
   NULL,   /* custom bitmap if PREDRAWN is set */
   NULL   /* leave this alone */
};
