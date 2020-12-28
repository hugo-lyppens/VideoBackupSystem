
static SHORT floppyBorderVectors1[] = {
   0,0,
   120,0,
   120,12,
   0,12,
   0,0
};
static struct Border floppyBorder1 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText floppyIText1 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   4,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Multiple Disks",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget floppy_multgadget = {
   NULL,   /* next gadget */
   282,51,   /* origin XY of hit box relative to window TopLeft */
   119,11,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &floppyIText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_MULTIPLE,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT floppyBorderVectors2[] = {
   0,0,
   55,0,
   55,14,
   0,14,
   0,0
};
static struct Border floppyBorder2 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText floppyIText2 = {
   0,7,JAM2,   /* front and back text pens, drawmode and fill byte */
   13,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "DF2:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget floppy_df2gadget = {
   &floppy_multgadget,   /* next gadget */
   282,15,   /* origin XY of hit box relative to window TopLeft */
   54,13,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &floppyIText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_DF2,   /* user-definable data */
   (APTR)(BACKFILL|7)   /* pointer to user-definable data */
};

static SHORT floppyBorderVectors3[] = {
   0,0,
   55,0,
   55,14,
   0,14,
   0,0
};
static struct Border floppyBorder3 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText floppyIText3 = {
   0,7,JAM2,   /* front and back text pens, drawmode and fill byte */
   13,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "DF3:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget floppy_df3gadget = {
   &floppy_df2gadget,   /* next gadget */
   347,15,   /* origin XY of hit box relative to window TopLeft */
   54,13,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &floppyIText3,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_DF3,   /* user-definable data */
   (APTR)(BACKFILL|7)   /* pointer to user-definable data */
};

static SHORT floppyBorderVectors4[] = {
   0,0,
   51,0,
   51,13,
   0,13,
   0,0
};
static struct Border floppyBorder4 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct TextAttr TOPAZ60 = {
   (STRPTR)"topaz.font",
   TOPAZ_SIXTY,0,0
};
static struct IntuiText floppyIText4 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   18,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget floppyGadget5 = {
   &floppy_df3gadget,   /* next gadget */
   246,81,   /* origin XY of hit box relative to window TopLeft */
   50,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &floppyIText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_OK,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT floppyBorderVectors5[] = {
   0,0,
   91,0,
   91,13,
   0,13,
   0,0
};
static struct Border floppyBorder5 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText floppyIText5 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Cancel",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget floppyGadget4 = {
   &floppyGadget5,   /* next gadget */
   311,81,   /* origin XY of hit box relative to window TopLeft */
   90,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &floppyIText5,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_CANCEL,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT floppyBorderVectors6[] = {
   0,0,
   55,0,
   55,14,
   0,14,
   0,0
};
static struct Border floppyBorder6 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors6,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText floppyIText6 = {
   0,7,JAM2,   /* front and back text pens, drawmode and fill byte */
   13,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "DF1:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget floppy_df1gadget = {
   &floppyGadget4,   /* next gadget */
   217,15,   /* origin XY of hit box relative to window TopLeft */
   54,13,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &floppyIText6,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_DF1,   /* user-definable data */
   (APTR)(BACKFILL|7)   /* pointer to user-definable data */
};

static SHORT floppyBorderVectors7[] = {
   0,0,
   55,0,
   55,14,
   0,14,
   0,0
};
static struct Border floppyBorder7 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors7,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText floppyIText7 = {
   0,7,JAM2,   /* front and back text pens, drawmode and fill byte */
   13,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "DF0:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget floppy_df0gadget = {
   &floppy_df1gadget,   /* next gadget */
   151,15,   /* origin XY of hit box relative to window TopLeft */
   54,13,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder7,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &floppyIText7,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_DF0,   /* user-definable data */
   (APTR)(BACKFILL|7)   /* pointer to user-definable data */
};

static UBYTE floppyfloppy_timegadgetSIBuff[9] =
   "HH:MM:SS";
static struct StringInfo floppyfloppy_timegadgetSInfo = {
   floppyfloppy_timegadgetSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   9,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT floppyBorderVectors8[] = {
   0,0,
   73,0,
   73,9,
   0,9,
   0,0
};
static struct Border floppyBorder8 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   floppyBorderVectors8,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget floppy_timegadget = {
   &floppy_df0gadget,   /* next gadget */
   329,35,   /* origin XY of hit box relative to window TopLeft */
   72,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   NULL,   /* activation flags */
   STRGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&floppyBorder8,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&floppyfloppy_timegadgetSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define floppyGadgetList1 floppy_timegadget

static struct IntuiText floppyIText9 = {
   1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   29,35,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Starting tape position:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText floppyIText8 = {
   1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   31,18,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Drive:",   /* pointer to text */
   &floppyIText9   /* next IntuiText structure */
};

#define floppyIntuiTextList1 floppyIText8

static struct Requester floppyRequesterStructure1 = {
   NULL,   /* previous requester (filled in by Intuition) */
   100,55,   /* requester XY origin relative to TopLeft of window */
   440,100,   /* requester width and height */
   0,0,   /* relative to these mouse offsets if POINTREL is set */
   &floppyGadgetList1,   /* gadget list */
   NULL,   /* box's border */
   &floppyIntuiTextList1,   /* requester text */
   NULL,   /* requester flags */
   2,   /* back-plane fill pen */
   NULL,   /* leave these alone */
   NULL,   /* custom bitmap if PREDRAWN is set */
   NULL   /* leave this alone */
};
