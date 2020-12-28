
static struct PropInfo VBS_filehorizSInfo = {
   AUTOKNOB+FREEHORIZ,   /* PropInfo flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,-1,   /* horizontal and vertical body values */
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   279,4,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_filehoriz = {
   NULL,   /* next gadget */
   288,170,   /* origin XY of hit box relative to window TopLeft */
   287,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&VBS_filehorizSInfo,   /* SpecialInfo structure */
   FILE_HORIZ,   /* user-definable data */
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

static struct Gadget VBS_filearrowright = {
   &VBS_filehoriz,   /* next gadget */
   575,167,   /* origin XY of hit box relative to window TopLeft */
   36,12,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FILE_ARROW_RIGHT,   /* user-definable data */
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

static struct Gadget VBS_filearrowleft = {
   &VBS_filearrowright,   /* next gadget */
   252,167,   /* origin XY of hit box relative to window TopLeft */
   36,12,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FILE_ARROW_LEFT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo VBS_dirhorizSInfo = {
   AUTOKNOB+FREEHORIZ,   /* PropInfo flags */
   -21845,-1,   /* horizontal and vertical pot values */
   -1,-1,   /* horizontal and vertical body values */
};

static struct Image Image4 = {
   0,0,   /* XY origin relative to container TopLeft */
   120,4,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_dirhoriz = {
   &VBS_filearrowleft,   /* next gadget */
   46,170,   /* origin XY of hit box relative to window TopLeft */
   128,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&VBS_dirhorizSInfo,   /* SpecialInfo structure */
   DIR_HORIZ,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData5[] = {
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

static struct Image Image5 = {
   0,0,   /* XY origin relative to container TopLeft */
   36,12,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData5,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_dirarrowright = {
   &VBS_dirhoriz,   /* next gadget */
   173,167,   /* origin XY of hit box relative to window TopLeft */
   36,12,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   DIR_ARROW_RIGHT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData6[] = {
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

static struct Image Image6 = {
   0,0,   /* XY origin relative to container TopLeft */
   36,12,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData6,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_dirarrowleft = {
   &VBS_dirarrowright,   /* next gadget */
   10,167,   /* origin XY of hit box relative to window TopLeft */
   36,12,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   DIR_ARROW_LEFT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData7[] = {
   0xFFFF,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0x8003,0xC000,0xC007,0xC000,0xE00F,0xC000,0xF01F,0xC000,
   0xF83F,0xC000,0xFC7F,0xC000,0xFEFF,0xC000,0xFFFF,0xC000,
   0xFFFF,0xC000,0xF01F,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0x9FFC,0xC000,0xCFF9,0xC000,0xE7F3,0xC000,0xF3E7,0xC000,
   0xF9CF,0xC000,0xFC9F,0xC000,0xFE3F,0xC000,0xFFFF,0xC000
};

static struct Image Image7 = {
   0,0,   /* XY origin relative to container TopLeft */
   18,16,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData7,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_filearrowdown = {
   &VBS_dirarrowleft,   /* next gadget */
   612,152,   /* origin XY of hit box relative to window TopLeft */
   18,16,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image7,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FILE_ARROW_DOWN,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo VBS_filevertSInfo = {
   AUTOKNOB+FREEVERT,   /* PropInfo flags */
   -1,0,   /* horizontal and vertical pot values */
   -1,21845,   /* horizontal and vertical body values */
};

static struct Image Image8 = {
   0,0,   /* XY origin relative to container TopLeft */
   6,33,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_filevert = {
   &VBS_filearrowdown,   /* next gadget */
   614,50,   /* origin XY of hit box relative to window TopLeft */
   14,102,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image8,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&VBS_filevertSInfo,   /* SpecialInfo structure */
   FILE_VERT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData9[] = {
   0xFFFF,0xC000,0xFEFF,0xC000,0xFC7F,0xC000,0xF83F,0xC000,
   0xF01F,0xC000,0xE00F,0xC000,0xC007,0xC000,0x8003,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xFFFF,0xC000,0xFFFF,0xC000,
   0xFFFF,0xC000,0xFE3F,0xC000,0xFC9F,0xC000,0xF9CF,0xC000,
   0xF3E7,0xC000,0xE7F3,0xC000,0xCFF9,0xC000,0x9FFC,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xFC07,0xC000,0xFFFF,0xC000
};

static struct Image Image9 = {
   0,0,   /* XY origin relative to container TopLeft */
   18,16,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData9,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_filearrowup = {
   &VBS_filevert,   /* next gadget */
   612,34,   /* origin XY of hit box relative to window TopLeft */
   18,16,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image9,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FILE_ARROW_UP,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo VBS_dirvertSInfo = {
   AUTOKNOB+FREEVERT,   /* PropInfo flags */
   -1,4940,   /* horizontal and vertical pot values */
   -1,16383,   /* horizontal and vertical body values */
};

static struct Image Image10 = {
   0,5,   /* XY origin relative to container TopLeft */
   6,25,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_dirvert = {
   &VBS_filearrowup,   /* next gadget */
   212,50,   /* origin XY of hit box relative to window TopLeft */
   14,102,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image10,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&VBS_dirvertSInfo,   /* SpecialInfo structure */
   DIR_VERT,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData11[] = {
   0xFFFF,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0x8003,0xC000,0xC007,0xC000,0xE00F,0xC000,0xF01F,0xC000,
   0xF83F,0xC000,0xFC7F,0xC000,0xFEFF,0xC000,0xFFFF,0xC000,
   0xFFFF,0xC000,0xF01F,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0x9FFC,0xC000,0xCFF9,0xC000,0xE7F3,0xC000,0xF3E7,0xC000,
   0xF9CF,0xC000,0xFC9F,0xC000,0xFE3F,0xC000,0xFFFF,0xC000
};

static struct Image Image11 = {
   0,0,   /* XY origin relative to container TopLeft */
   18,16,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData11,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_dirarrowdown = {
   &VBS_dirvert,   /* next gadget */
   210,152,   /* origin XY of hit box relative to window TopLeft */
   18,16,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image11,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   DIR_ARROW_DOWN,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData12[] = {
   0xFFFF,0xC000,0xFEFF,0xC000,0xFC7F,0xC000,0xF83F,0xC000,
   0xF01F,0xC000,0xE00F,0xC000,0xC007,0xC000,0x8003,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,0xF01F,0xC000,
   0xF01F,0xC000,0xF01F,0xC000,0xFFFF,0xC000,0xFFFF,0xC000,
   0xFFFF,0xC000,0xFE3F,0xC000,0xFC9F,0xC000,0xF9CF,0xC000,
   0xF3E7,0xC000,0xE7F3,0xC000,0xCFF9,0xC000,0x9FFC,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,0xF3E7,0xC000,
   0xF3E7,0xC000,0xF3E7,0xC000,0xFC07,0xC000,0xFFFF,0xC000
};

static struct Image Image12 = {
   0,0,   /* XY origin relative to container TopLeft */
   18,16,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData12,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget VBS_dirarrowup = {
   &VBS_dirarrowdown,   /* next gadget */
   210,34,   /* origin XY of hit box relative to window TopLeft */
   18,16,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image12,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   DIR_ARROW_UP,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors1[] = {
   0,0,
   359,0,
   359,133,
   0,133,
   0,0
};
static struct Border Border1 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget VBS_filegadget = {
   &VBS_dirarrowup,   /* next gadget */
   253,35,   /* origin XY of hit box relative to window TopLeft */
   358,132,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   FILE_WINDOW,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   199,0,
   199,133,
   0,133,
   0,0
};
static struct Border Border2 = {
   -1,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget VBS_dirgadget = {
   &VBS_filegadget,   /* next gadget */
   11,35,   /* origin XY of hit box relative to window TopLeft */
   198,132,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   DIR_WINDOW,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 VBS_dirgadget

static struct IntuiText IText1 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "  Long File List",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem6 = {
   NULL,   /* next MenuItem structure */
   0,45,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   180,9,   /* hit box width and height */
   CHECKIT+ITEMTEXT+COMMSEQ+MENUTOGGLE+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText1,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'L',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText2 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "  Include All Archived",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem5 = {
   &MenuItem6,   /* next MenuItem structure */
   0,36,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   180,9,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText2,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText3 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "  Exclude Archived",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem4 = {
   &MenuItem5,   /* next MenuItem structure */
   0,27,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   180,9,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText3,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText4 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "  Toggle Files",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem3 = {
   &MenuItem4,   /* next MenuItem structure */
   0,18,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   180,9,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText4,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText5 = {
   0,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "  Exclude Files",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem2 = {
   &MenuItem3,   /* next MenuItem structure */
   0,9,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   180,9,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText5,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText6 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "  Include Files",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem1 = {
   &MenuItem2,   /* next MenuItem structure */
   0,0,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   180,9,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText6,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct Menu Menu2 = {
   NULL,   /* next Menu structure */
   92,0,   /* XY origin of Menu hit box relative to screen TopLeft */
   128,0,   /* Menu hit box width and height */
   MENUENABLED,   /* Menu flags */
   "Files",   /* text of Menu name */
   &MenuItem1   /* MenuItem linked list pointer */
};

static struct IntuiText IText7 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Main Menu",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem8 = {
   NULL,   /* next MenuItem structure */
   0,9,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   200,9,   /* hit box width and height */
   ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText7,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'C',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText8 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Start Restore To ...  ",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct MenuItem MenuItem7 = {
   &MenuItem8,   /* next MenuItem structure */
   0,0,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   200,9,   /* hit box width and height */
   ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText8,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'S',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct Menu Menu1 = {
   &Menu2,   /* next Menu structure */
   10,0,   /* XY origin of Menu hit box relative to screen TopLeft */
   64,0,   /* Menu hit box width and height */
   MENUENABLED,   /* Menu flags */
   "Action",   /* text of Menu name */
   &MenuItem7   /* MenuItem linked list pointer */
};

#define MenuList1 Menu1

static struct NewWindow NewWindowStructure1 = {
   0,11,   /* window XY origin relative to TopLeft of screen */
   640,189,   /* window width and height */
   0,2,   /* detail and block pens */
   GADGETDOWN+GADGETUP+MENUPICK+ACTIVEWINDOW,   /* IDCMP flags */
   BORDERLESS+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &VBS_dirgadget,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};
