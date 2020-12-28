#include "headers.h"

#define VBSVER	"3.2"

#define NUMBER(x) (sizeof(x)/sizeof(x[0]))
#define LPRESSED        (!((*((UBYTE *)0xBFE001))&64))
#define LEN(s)          ((sizeof s) -1)
#define DeleteLayer(dummy,layer) DeleteLayer((long)(dummy),layer)

#define max(a,b)  ((a)>(b)?(a):(b))
#define min(a,b)  ((a)<(b)?(a):(b))

typedef struct VBS_Node     VBS_Node_t;
typedef struct VBS_DirNode  VBS_DirNode_t;
typedef struct VBS_FileNode VBS_FileNode_t;
typedef struct VBS_ListData VBS_ListData_t;
typedef struct VBS_DirEntry VBS_DirEntry_t;
typedef struct VBS_DispNode VBS_DispNode_t;


struct VBS_DispNode
{
   struct Node          dn_Node;
   struct DimensionInfo DimInfo;
   struct DisplayInfo   DispInfo;
   struct MonitorInfo   MonInfo;
   ULONG                ModeID;
   UWORD                Number;
};

struct VBS_Node {
   VBS_Node_t      *Next;
};

struct VBS_DirNode {
   VBS_DirNode_t   *Next;
   VBS_DirNode_t   *ParentDir;
   VBS_DirNode_t   *FirstDir;

   VBS_FileNode_t  *FirstFile;

   LONG             Protection;
   struct DateStamp Date;
   int              TextLength;
   UBYTE            Flags;
   UBYTE            NameLen;
   UBYTE            Name[1];

};

struct VBS_FileNode {
   VBS_FileNode_t  *Next;
   LONG             Size;
   LONG             Protection;
   struct DateStamp Date;
   int              TextLength;
   UBYTE            Flags;
   UBYTE            NameLen;
   UBYTE            Name[1];
};

struct VBS_ListData {
   struct Gadget     *PadGadget, *HorizGadget, *VertGadget;
   struct TextFont   *Font;
#if 0
   struct Layer_Info *LayerInfo;
   struct Layer      *Layer;
#endif   
   struct RastPort    RastPort;
   struct Region     *Region;
   int                Wwidth;
   int		      LeftEdge, TopEdge;
   int                WinWidth, WinHeight, WinRows;
   int                NumEntries;
   int                TopRow, LeftX;
   int                NumRows, NumCols, MaxEltWidth, MaxTotalWidth;
};

struct VBS_DirEntry {
   VBS_DirNode_t  *dn;
   int             x, y;
};
#define  ERRF_ERRMASK		(128|32|16|8)
#define  ERRF_READERROR          128
#define  ERRF_INCLUDE             64
#define  ERRF_VERIFYERROR         32
#define  ERRF_FILEEXISTSERROR     16
#define  ERRF_FILENOTEXISTSERROR   8
#define  ERRF_COMMENT		   4

typedef enum {
   EOK = 0, EREAD, EWRITE, EMEMORY, EVERIFY, ESEARCH,
   EWRITEPROTECTED, ENODISKINDRIVE, EBREAK, EVIDEOREAD,
   EWRONGCHUNK,EENCOUNTERED45,
   EVIDEOCONNECTION, ETREEREAD, ETREEREADV, ENOTIMPLEMENTED,
   ELOGEXISTS, ECOULDNTCREATELOG, ECOULDNTLOADLOG, EUPDATELOG,
   EENTERTIME, EWRITEREPORT, ESTARTTIME
} VBS_error_t;

typedef enum {
   PUBLICST, CUSTOMST
} VBS_screentype_t;

typedef enum {
   NUMERICCT, TIMEDCT
} VBS_countertype_t;

typedef enum {
   LOADRT, SAVERT, DIRRT
} VM_requestertype_t;

typedef enum {
   BACKUPOT, RESTOREOT, VERIFYOT
} VM_operationtype_t;

#define ERROR_CANCEL 0
#define ERROR_RETRY  1
#define ERROR_OTHER  2


#define  NUM_REPEAT     3

#define  VIDEOWIDTH           352
#define  HSTART               (113)
#ifdef PAL_VIDEO
#define  VIDEOHEIGHT          (272)
#define  VSTART               33
#define  JIFFIES_PER_SECOND   50
#define  MEMSIZE              (1024*128)
#define  RSCR68000HEIGHT      256
#define  VIDEO_MONITOR_ID     PAL_MONITOR_ID
#else
#define  VIDEOHEIGHT          (221)
#define  VSTART               32
#define  JIFFIES_PER_SECOND   60
#define  MEMSIZE              (157*13*64)
#define  RSCR68000HEIGHT      200
#define  VIDEO_MONITOR_ID     NTSC_MONITOR_ID
#endif

/* Gadget.UserData definitions */
#define  BCOLORMASK     15
#define  BACKFILL       128

/* Colors */
#define GADGBCOLOR   5

#define WINDOWTOP    11

/* Pad controls */

#define HORIZ       0
#define VERT        1
#define ARROW_LEFT  2
#define ARROW_RIGHT 3
#define ARROW_UP    4
#define ARROW_DOWN  5

/* Logging */
#define DO_NOT_LOG  0xFFFFFFFF
enum {BT_ILLEGAL = 0, BT_FS, BT_AMIGADOS, BT_MSDOS, BT_TESTPATTERN,
      BT_FSFAST, BT_FSFASTCOMPR};
/*extern BPTR stdout;*/

void             *GetUnit(struct DiscResourceUnit *);
void              GiveUnit(void);

long              main(int, char **);

UBYTE             toupperf(UBYTE);
void              AddDirNode(UBYTE *, int);
int               FastMemCmp(UBYTE *, UBYTE *, long);
int               StrCmpNoCase(UBYTE *, UBYTE *);
UBYTE            *FileRequest(struct Window *, UBYTE *, UBYTE *,\
                              UBYTE **, UBYTE *, VM_requestertype_t);
/* void              FreeDir(void);*/
BOOL              Write2(BPTR, UBYTE *, long);
void              Rect(struct RastPort *, int, int, int, int, int);

BOOL              VMG_AddLogEntry(ULONG, ULONG, UBYTE *);
void              UIG_AdjustProps(struct Window *, VBS_ListData_t *,\
                     int, int);
UBYTE            *NTG_AskNameTime(struct Window *, ULONG *, BOOL *, UBYTE *, VM_operationtype_t);
int               FLG_AskFloppy(struct Window *, ULONG *, BOOL *, UBYTE **, VM_operationtype_t);
UBYTE            *PAG_AskPath(struct Window *, UBYTE *);
extern UBYTE     *VBS_VIInit(PLANEPTR,PLANEPTR,struct View *,struct View *);
VBS_error_t       BAG_Backup(char, VBS_DirNode_t  *, UBYTE *, UBYTE *, BOOL);
extern BOOL       VFG_BackupFloppy(int, UBYTE *);
VOID		  UIG_Beep(int);
BOOL              BAG_CheckConnection(struct Screen *, struct Window *);
extern BOOL       VBS_CheckInterval(long, long);
extern long       VBS_CheckPattern(UBYTE *, long, struct ErrorSpec *);

extern BOOL       VBS_CheckLoopback(void);
void              BAG_Cleanup(void);
void              UIG_CleanupPad(VBS_ListData_t *);
BOOL		  VWG_CorrectErrs(char *, long, char *);
long              VBS_MemCmpL(UBYTE *, UBYTE *, long);
int               UIG_Date(UBYTE *, long);
void              BAG_DelayJiffies(long);
BOOL              UIG_DelayLMB(long);
BOOL              FLBG_DriveAvailable(int);
BOOL              UIG_FileExists(UBYTE *);
void		  UIG_FillPadBgnd(struct VBS_ListData *, int);
void		  UIG_FillWindowBgnd(struct Window *, int);
long              VBS_FindMemLimit(void);
extern LONG       VBS_FreeTimer(void);
void              BAG_FreeTree(VBS_DirNode_t *);
VBS_error_t       FLBG_FloppyBackup(struct Screen *, int, BOOL, ULONG);
VBS_error_t       FLBG_FloppyRestore(struct Screen *, \
                      UBYTE *, int, BOOL);
VBS_error_t       HDBG_FSBackup(BOOL);
VBS_error_t       HDBG_FSRestore(struct Window *, UBYTE *);
VBS_error_t       HDBG_FSVerify(struct Window *, UBYTE *);
extern LONG       VBS_GetTimer(void);
VBS_error_t       BAG_Init(void);
BOOL              UIG_InitPad(VBS_ListData_t *, struct Window *);
BOOL 		  VMG_LogEmpty(void);
UBYTE            *UIG_MakeProtString(ULONG);
BOOL              VMG_Menu(UBYTE *, UBYTE *);
int               ERRG_Message(struct Window *, VBS_error_t);
void              UIG_NewView(struct Window *, VBS_ListData_t   *,
                     int, int *, int *);
struct Window    *UIG_OpenWindow(struct Screen *, struct NewWindow *, int);
struct TextFont  *UIG_OpenFont(struct TextAttr *);
struct TextFont  *UIG_OpenDefaultFont(void);

void              BAG_PrepareTree(VBS_DirNode_t *,
                      struct RastPort *, long *, long *, long *,
                      VBS_ListData_t *, int, int);
void              BAG_PrintTree(VBS_DirNode_t *);
void              BAG_PrintMsg(UBYTE *);
extern VBS_error_t       VBS_ReadMem(UBYTE *, long);
VBS_error_t       VBS_ReadMemRS(ULONG *, ULONG *, UBYTE *, long, UBYTE *, UBYTE *);
extern LONG       VBS_ReadTimer(void);
VBS_error_t       BAG_RestoreFilesTo(char type, VBS_DirNode_t *, char *,\
				     char *);
extern VBS_error_t      VFG_RestoreFloppy(int);
void              BAG_RestoreVideoScreen(void);
void              BAG_RestoreScreen(void);
extern void       VBS_RestoreScreen(void);
VBS_error_t       BAG_RestoreTree(char, VBS_DirNode_t **);
VBS_error_t       BAG_ScanDirectories(VBS_DirNode_t **, UBYTE *,
                      struct RastPort *, long *, long *, long *,
                      VBS_ListData_t *, int, int);
VBS_error_t       BAG_Search(UBYTE, UBYTE *, UBYTE *, UBYTE *);
extern UBYTE     *VBS_Search(void);
extern long       VBS_SearchFirstBlockSync(void);
extern void       VFG_SetBuffers(APTR, APTR, APTR, APTR);
void              BAG_SetupScreen(struct Screen *);
void              BAG_SetupVideoScreen(struct Screen *);
void              BAG_ShowMessage(struct Screen *, UBYTE *, UBYTE *, UWORD, UWORD);
extern void       VBS_ShowBlack(void);
extern void       VBS_ShowCopperList(USHORT *);
extern void       VBS_ShowPlane(PLANEPTR, int, int, int, int, int, int ,int);
int               UIG_Time(UBYTE *, long, long);
int               VMG_StringToTime(UBYTE *, ULONG *);
int               VMG_TimeToString(UBYTE *, ULONG);
VBS_error_t       BAG_VerifyFilesFrom(char, VBS_DirNode_t *, UBYTE *,\
				      char *);
extern BOOL       VBS_VerifyMem(UBYTE *, long);
extern void       VBS_VertBServer(void);
extern BOOL       VBS_WaitFire(void);
extern void       VBS_WriteBackupHeader(UBYTE *);
extern void       VBS_WriteErrorIndicator(void);
extern void       VBS_WriteFirstBlockSync(void);
VBS_error_t       BAG_WriteHeader(UBYTE, UBYTE *, UBYTE *, struct DateStamp *);
extern void       VBS_WriteMem(UBYTE *, long, int);
extern void       VBS_WriteMemRS(ULONG, ULONG, UBYTE *, int, UBYTE *, UBYTE *);
extern void       VBS_WriteTestPattern(UBYTE *);
extern void       VBS_WriteVisual(PLANEPTR);
void              SRTG_SortLL(VBS_Node_t **, int(*)());
/*VBS_Node_t *, VBS_Node_t *));*/
void              SRTG_SortTree(VBS_DirNode_t **);

VOID              UIG_BlockInput(struct Window *);
VOID              UIG_UnBlockInput(struct Window *);
BOOL              UIG_TryFont(struct TextAttr *);
VOID              UIG_MakeTextAttr(struct TextAttr *, UBYTE *, int);

BOOL              SETG_Settings(struct Window *);
VBS_error_t       SETG_LoadSettings(void);
BOOL              SET_FontRequest(struct Window *, UBYTE *, int, struct TextAttr *);
VOID              SET_CopyTextAttr(struct TextAttr *, struct TextAttr *);
VBS_DispNode_t   *SET_FindNode(ULONG);
VOID              SET_FreeScreenModes(VOID);
BOOL              SET_ScanScreenModes(VOID);
VOID              SET_SetDisplayData(VBS_DispNode_t *);

extern void kputstr(UBYTE *);
