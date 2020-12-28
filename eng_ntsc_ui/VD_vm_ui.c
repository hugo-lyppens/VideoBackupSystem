
static struct NewScreen NewScreenStructure = {
   0,0,   /* screen XY origin relative to View */
   640,200,   /* screen width and height */
   3,   /* screen depth (number of bitplanes) */
   0,2,   /* detail and block pens */
   HIRES,   /* display modes for this screen */
   CUSTOMSCREEN,   /* screen type */
   NULL,   /* pointer to default screen font */
   " Video Backup System AGA V1.5 © 1993 H. Lyppens Software Productions",   /* screen title */
   NULL,   /* first in list of custom screen gadgets */
   NULL   /* pointer to custom BitMap structure */
};

#define NEWSCREENSTRUCTURE NewScreenStructure

static USHORT Palette[] = {
   0x0000,   /* color #0 */
   0x0FEC,   /* color #1 */
   0x0998,   /* color #2 */
   0x0068,   /* color #3 */
   0x0FD0,   /* color #4 */
   0x0CCA,   /* color #5 */
   0x029D,   /* color #6 */
   0x0FFF   /* color #7 */
#define PaletteColorCount 8
};

#define PALETTE Palette

static SHORT BorderVectors1[] = {
   0,0,
   165,0,
   165,18,
   0,18,
   0,0
};
static struct Border Border1 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct TextAttr TOPAZ60 = {
   (STRPTR)"topaz.font",
   TOPAZ_SIXTY,0,0
};
static struct IntuiText IText1 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,4,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "File Verify",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget18 = {
   NULL,   /* next gadget */
   16,91,   /* origin XY of hit box relative to window TopLeft */
   164,17,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FS_VERIFY,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   419,0,
   419,16,
   0,16,
   0,0
};
static struct Border Border2 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct TextAttr TOPAZ80 = {
   (STRPTR)"topaz.font",
   TOPAZ_EIGHTY,0,0
};
static struct IntuiText IText3 = {
   3,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   169,3,   /* XY origin relative to container TopLeft */
   &TOPAZ80,   /* font pointer or NULL for default */
   "None",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   7,3,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Video Tape Log:",   /* pointer to text */
   &IText3   /* next IntuiText structure */
};

static struct Gadget VM_lognamegadget = {
   &Gadget18,   /* next gadget */
   211,7,   /* origin XY of hit box relative to window TopLeft */
   418,15,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   NULL,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors3[] = {
   0,0,
   165,0,
   165,12,
   0,12,
   0,0
};
static struct Border Border3 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText4 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,1,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Create New Log",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget16 = {
   &VM_lognamegadget,   /* next gadget */
   16,154,   /* origin XY of hit box relative to window TopLeft */
   164,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   CREATE_NEW_LOG,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors4[] = {
   0,0,
   165,0,
   165,12,
   0,12,
   0,0
};
static struct Border Border4 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText5 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,1,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Update Log File",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget15 = {
   &Gadget16,   /* next gadget */
   16,140,   /* origin XY of hit box relative to window TopLeft */
   164,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText5,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   UPDATE_LOG,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors5[] = {
   0,0,
   165,0,
   165,12,
   0,12,
   0,0
};
static struct Border Border5 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText6 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,1,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Del Log Entry",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget14 = {
   &Gadget15,   /* next gadget */
   16,126,   /* origin XY of hit box relative to window TopLeft */
   164,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText6,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   DEL_LOG_ENTRY,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors6[] = {
   0,0,
   165,0,
   165,12,
   0,12,
   0,0
};
static struct Border Border6 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors6,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText7 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,1,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Log File ... ",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget VM_logfilegadget = {
   &Gadget14,   /* next gadget */
   16,112,   /* origin XY of hit box relative to window TopLeft */
   164,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText7,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   LOG_FILE,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors7[] = {
   0,0,
   165,0,
   165,18,
   0,18,
   0,0
};
static struct Border Border7 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors7,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText8 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,4,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Quit VBS",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget12 = {
   &VM_logfilegadget,   /* next gadget */
   16,169,   /* origin XY of hit box relative to window TopLeft */
   164,17,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border7,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText8,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   QUIT_VBS,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors8[] = {
   0,0,
   165,0,
   165,18,
   0,18,
   0,0
};
static struct Border Border8 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors8,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText9 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,4,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "File Restore",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget11 = {
   &Gadget12,   /* next gadget */
   16,70,   /* origin XY of hit box relative to window TopLeft */
   164,17,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border8,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText9,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FS_RESTORE,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors9[] = {
   0,0,
   165,0,
   165,18,
   0,18,
   0,0
};
static struct Border Border9 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors9,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText10 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,4,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "File Backup",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget10 = {
   &Gadget11,   /* next gadget */
   16,49,   /* origin XY of hit box relative to window TopLeft */
   164,17,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border9,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText10,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FS_BACKUP,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors10[] = {
   0,0,
   165,0,
   165,18,
   0,18,
   0,0
};
static struct Border Border10 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors10,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText11 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,4,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Floppy Restore",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget9 = {
   &Gadget10,   /* next gadget */
   16,28,   /* origin XY of hit box relative to window TopLeft */
   164,17,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border10,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText11,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_RESTORE,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors11[] = {
   0,0,
   165,0,
   165,18,
   0,18,
   0,0
};
static struct Border Border11 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors11,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText12 = {
   0,5,JAM2,   /* front and back text pens, drawmode and fill byte */
   14,5,   /* XY origin relative to container TopLeft */
   &TOPAZ60,   /* font pointer or NULL for default */
   "Floppy Backup",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget8 = {
   &Gadget9,   /* next gadget */
   16,7,   /* origin XY of hit box relative to window TopLeft */
   164,17,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border11,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText12,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FLOPPY_BACKUP,   /* user-definable data */
   (APTR)(BACKFILL|GADGBCOLOR)   /* pointer to user-definable data */
};

static SHORT BorderVectors12[] = {
   0,0,
   401,0,
   401,149,
   0,149,
   0,0
};
static struct Border Border12 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors12,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget VM_loggadget = {
   &Gadget8,   /* next gadget */
   211,26,   /* origin XY of hit box relative to window TopLeft */
   400,148,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border12,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   LOG_WINDOW,   /* user-definable data */
   (APTR)(BACKFILL|BCOLOR)   /* pointer to user-definable data */
};

static struct PropInfo VM_loghorizSInfo = {
   AUTOKNOB+FREEHORIZ,   /* PropInfo flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,-1,   /* horizontal and vertical body values */
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   321,4,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VM_loghoriz = {
   &VM_loggadget,   /* next gadget */
   246,177,   /* origin XY of hit box relative to window TopLeft */
   329,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&VM_loghorizSInfo,   /* SpecialInfo structure */
   LOG_HORIZ,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData2[] = {
   0xFFFF,0xFFFF,0xF000,0xFFFF,0x0FFF,0xF000,0xFFFF,0x00FF,
   0xF000,0xF000,0x000F,0xF000,0xF000,0x0001,0xF000,0xF000,
   0x0001,0xF000,0xF000,0x000F,0xF000,0xFFFF,0x80FF,0xF000,
   0xFFFF,0x8FFF,0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,0xFFFF,
   0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,
   0x0FFF,0xF000,0xFFFF,0x70FF,0xF000,0xF000,0x7F0F,0xF000,
   0xCFFF,0xFFF1,0xF000,0xCFFF,0xFFFE,0x3000,0xCFFF,0xFFF1,
   0xF000,0xC000,0x7F0F,0xF000,0xFFFF,0x70FF,0xF000,0xFFFF,
   0x0FFF,0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,0xFFFF,0xF000
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   36,12,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData2,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VM_logarrowright = {
   &VM_loghoriz,   /* next gadget */
   575,174,   /* origin XY of hit box relative to window TopLeft */
   36,12,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   LOG_ARROW_RIGHT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData3[] = {
   0xFFFF,0xFFFF,0xF000,0xFFFF,0x0FFF,0xF000,0xFFF0,0x0FFF,
   0xF000,0xFF00,0x0000,0xF000,0xF800,0x0000,0xF000,0xF800,
   0x0000,0xF000,0xFF00,0x0000,0xF000,0xFFF0,0x1FFF,0xF000,
   0xFFFF,0x1FFF,0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,0xFFFF,
   0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,
   0x0FFF,0xF000,0xFFF0,0xEFFF,0xF000,0xFF0F,0xE000,0xF000,
   0xF8FF,0xFFFF,0x3000,0xC7FF,0xFFFF,0x3000,0xF8FF,0xFFFF,
   0x3000,0xFF0F,0xE000,0x3000,0xFFF0,0xEFFF,0xF000,0xFFFF,
   0x0FFF,0xF000,0xFFFF,0xFFFF,0xF000,0xFFFF,0xFFFF,0xF000
};

static struct Image Image3 = {
   0,0,   /* XY origin relative to container TopLeft */
   36,12,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData3,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VM_logarrowleft = {
   &VM_logarrowright,   /* next gadget */
   210,174,   /* origin XY of hit box relative to window TopLeft */
   36,12,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   LOG_ARROW_LEFT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData4[] = {
   0xFFFF,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0x8003,0xC000,0xC007,0xC000,0xE00F,0xC000,0xF01F,0xC000,
   0xF83F,0xC000,0xFC7F,0xC000,0xFEFF,0xC000,0xFFFF,0xC000,
   0xFFFF,0xC000,0xF01F,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0x9FFC,0xC000,0xCFF9,0xC000,0xE7F3,0xC000,0xF3E7,0xC000,
   0xF9CF,0xC000,0xFC9F,0xC000,0xFE3F,0xC000,0xFFFF,0xC000
};

static struct Image Image4 = {
   0,0,   /* XY origin relative to container TopLeft */
   18,16,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData4,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VM_logarrowdown = {
   &VM_logarrowleft,   /* next gadget */
   612,159,   /* origin XY of hit box relative to window TopLeft */
   18,16,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   LOG_ARROW_DOWN,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo VM_logvertSInfo = {
   AUTOKNOB+FREEVERT,   /* PropInfo flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,-1,   /* horizontal and vertical body values */
};

static struct Image Image5 = {
   0,0,   /* XY origin relative to container TopLeft */
   6,114,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VM_logvert = {
   &VM_logarrowdown,   /* next gadget */
   614,41,   /* origin XY of hit box relative to window TopLeft */
   14,118,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&VM_logvertSInfo,   /* SpecialInfo structure */
   LOG_VERT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData6[] = {
   0xFFFF,0xC000,0xFEFF,0xC000,0xFC7F,0xC000,0xF83F,0xC000,
   0xF01F,0xC000,0xE00F,0xC000,0xC007,0xC000,0x8003,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xFFFF,0xC000,0xFFFF,0xC000,
   0xFFFF,0xC000,0xFE3F,0xC000,0xFC9F,0xC000,0xF9CF,0xC000,
   0xF3E7,0xC000,0xE7F3,0xC000,0xCFF9,0xC000,0x9FFC,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xFC07,0xC000,0xFFFF,0xC000
};

static struct Image Image6 = {
   0,0,   /* XY origin relative to container TopLeft */
   18,16,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData6,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VM_logarrowup = {
   &VM_logvert,   /* next gadget */
   612,25,   /* origin XY of hit box relative to window TopLeft */
   18,16,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   LOG_ARROW_UP,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 VM_logarrowup

static struct NewWindow NewWindowStructure1 = {
   0,11,   /* window XY origin relative to TopLeft of screen */
   640,189,   /* window width and height */
   0,2,   /* detail and block pens */
   GADGETDOWN+GADGETUP+MENUPICK+ACTIVEWINDOW,   /* IDCMP flags */
   BORDERLESS+ACTIVATE+RMBTRAP+NOCAREREFRESH,   /* other window flags */
   &VM_logarrowup,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};
