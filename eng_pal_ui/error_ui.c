
static SHORT errorBorderVectors1[] = {
   0,0,
   326,0,
   326,39,
   0,39,
   0,0
};
static struct Border errorBorder1 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   errorBorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget errorGadget3 = {
   NULL,   /* next gadget */
   12,8,   /* origin XY of hit box relative to window TopLeft */
   325,38,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   NULL,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&errorBorder1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   (APTR)(BACKFILL|BCOLOR)   /* pointer to user-definable data */
};

static SHORT errorBorderVectors2[] = {
   0,0,
   151,0,
   151,13,
   0,13,
   0,0
};
static struct Border errorBorder2 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   errorBorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct TextAttr TOPAZ60 = {
   (STRPTR)"topaz.font",
   TOPAZ_SIXTY,0,0
};
static struct IntuiText errorIText1 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Cancel",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget error_cancelgadget = {
   &errorGadget3,   /* next gadget */
   187,53,   /* origin XY of hit box relative to window TopLeft */
   150,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&errorBorder2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &errorIText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   ERROR_CANCEL,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT errorBorderVectors3[] = {
   0,0,
   151,0,
   151,13,
   0,13,
   0,0
};
static struct Border errorBorder3 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   errorBorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText errorIText2 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   18,2,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Retry",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget error_retrygadget = {
   &error_cancelgadget,   /* next gadget */
   12,53,   /* origin XY of hit box relative to window TopLeft */
   150,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET+REQGADGET,   /* gadget type flags */
   (APTR)&errorBorder3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &errorIText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   ERROR_RETRY,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

#define errorGadgetList1 error_retrygadget

static struct Requester errorRequesterStructure1 = {
   NULL,   /* previous requester (filled in by Intuition) */
   135,95,   /* requester XY origin relative to TopLeft of window */
   350,70,   /* requester width and height */
   0,0,   /* relative to these mouse offsets if POINTREL is set */
   &errorGadgetList1,   /* gadget list */
   NULL,   /* box's border */
   NULL,   /* requester text */
   NULL,   /* requester flags */
   2,   /* back-plane fill pen */
   NULL,   /* leave these alone */
   NULL,   /* custom bitmap if PREDRAWN is set */
   NULL   /* leave this alone */
};
