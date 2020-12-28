#include "sys.h"
#define NUMBER(x) (sizeof(x)/sizeof(x[0]))
#define LPRESSED        (!((*((UBYTE *)0xBFE001))&64))
#define LEN(s)          ((sizeof s) -1)
#define DeleteLayer(dummy,layer) DeleteLayer((long)(dummy),layer)

typedef struct VBS_Node     VBS_Node_t;
typedef struct VBS_DirNode  VBS_DirNode_t;
typedef struct VBS_FileNode VBS_FileNode_t;
typedef struct VBS_ListData VBS_ListData_t;
typedef struct VBS_DirEntry VBS_DirEntry_t;

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
   char             Name[1];

};

struct VBS_FileNode {
   VBS_FileNode_t  *Next;
   LONG             Size;
   LONG             Protection;
   struct DateStamp Date;
   int              TextLength;
   UBYTE            Flags;
   UBYTE            NameLen;
   char             Name[1];
};

struct VBS_ListData {
   struct Gadget   *PadGadget, *HorizGadget, *VertGadget,\
                   *ArrowLeft, *ArrowRight, *ArrowUp, *ArrowDown;
   struct TextFont *Font;
   struct Layer_Info *LayerInfo;
   struct Layer    *Layer;
   struct RastPort *RastPort;

   int              Wwidth;
   int              WinWidth, WinHeight, WinRows;
   int              NumEntries;
   int              TopRow, LeftX;
   int              NumRows, NumCols, MaxEltWidth, MaxTotalWidth;
};
struct VBS_DirEntry {
   VBS_DirNode_t  *dn;
   int             x, y;
};

#define  ERRF_READERROR         128
#define  ERRF_INCLUDE            64
#define  ERRF_VERIFYERROR        32
#define  ERRF_FILEEXISTSERROR    16
#define  ERRF_FILENOTEXISTSERROR  8

typedef enum {
   EOK = 0, EREAD, EWRITE, EMEMORY, EVERIFY, ESEARCH,
   EWRITEPROTECTED, ENODISKINDRIVE, EBREAK, EVIDEOREAD,
   EVIDEOCONNECTION, ETREEREAD, ETREEREADV, ENOTIMPLEMENTED, ELOGEXISTS,
   ECOULDNTCREATELOG, EUPDATELOG, EENTERTIME, EWRITEREPORT
} VBS_error_t;

#define ERROR_CANCEL 50
#define ERROR_RETRY  51


#define  NUM_REPEAT     3

#define  VIDEOWIDTH           352
#define  HSTART               (113)
#ifdef PAL_VIDEO
#define  VIDEOHEIGHT          (272)
#define  VSTART               33
#define  JIFFIES_PER_SECOND   50
#define  MEMSIZE              (1024*128)
#else
#define  VIDEOHEIGHT          (221)
#define  VSTART               33
#define  JIFFIES_PER_SECOND   60
#define  MEMSIZE              (157*13*64)
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
enum {BT_ILLEGAL = 0, BT_FS, BT_AMIGADOS, BT_MSDOS, BT_TESTPATTERN };
/*extern BPTR stdout;*/

void             *GetUnit(struct DiscResourceUnit *);
void              GiveUnit(void);

long              main(int, char **);
/*void              printf(char *,);
void              sprintf(char *, char *,);*/
char              toupperf(char);
void              AddDirNode(char *, int);
int               FastMemCmp(char *, char *, long);
int               StrCmpNoCase(char *, char *);
char             *FileRequest(struct Screen *, struct Window *, char *,\
                              char *, char **, char *);
void              FreeDir(void);
BOOL              Write2(BPTR, char *, long);
void              Rect(struct RastPort *, int, int, int, int, int);

BOOL              VMG_AddLogEntry(ULONG, ULONG, char *);
void              UIG_AdjustProps(struct Window *, VBS_ListData_t *,\
                     int, int);
char             *NTG_AskNameTime(struct Window *, ULONG *, BOOL *, char *);
int               FLG_AskFloppy(struct Window *, ULONG *, BOOL *);
char             *PAG_AskPath(struct Window *, char *);
extern char      *VBS_VIInit(PLANEPTR,PLANEPTR,struct View *,struct View *);
VBS_error_t       BAG_Backup(VBS_DirNode_t  *, char *, char *, BOOL, char *, long);
extern BOOL       VFG_BackupFloppy(int, UBYTE *);
void              UIG_ChangeGadgetText(struct Window *, struct Gadget *,
                                       char *, int);
BOOL              BAG_CheckConnection(struct Screen *, struct Window *);
extern BOOL       VBS_CheckInterval(long, long);
extern long       VBS_CheckPattern(char *, long, struct ErrorSpec *);

extern BOOL       VBS_CheckLoopback(void);
void              BAG_Cleanup(void);
void              UIG_CleanupPad(VBS_ListData_t *);
long              VBS_MemCmpL(char *, char *, long);
int               UIG_Date(char *, long);
void              BAG_DelayJiffies(long);
BOOL              UIG_DelayLMB(long);
BOOL              FLBG_DriveAvailable(int);
BOOL              UIG_FileExists(char *);
long              VBS_FindMemLimit(void);
extern LONG       VBS_FreeTimer(void);
void              BAG_FreeTree(VBS_DirNode_t *);
VBS_error_t       FLBG_FloppyBackup(struct Screen *, int, BOOL, ULONG);
VBS_error_t       FLBG_FloppyRestore(struct Screen *, \
                      char *, int, BOOL);
VBS_error_t       HDBG_FSBackup(struct Screen *, char *, char *, int, BOOL);
VBS_error_t       HDBG_FSRestore(struct Screen *, char *, char *, int);
VBS_error_t       HDBG_FSVerify(struct Screen *, char *, char *, int);
extern LONG       VBS_GetTimer(void);
VBS_error_t       BAG_Init(char *, int, char *, int);
BOOL              UIG_InitPad(VBS_ListData_t *, struct Window *, char *, int);
char             *UIG_MakeProtString(ULONG);
void              VMG_Menu(char *, char *, int, char *);
int               ERRG_Message(struct Window *, VBS_error_t);
void              UIG_NewView(struct Window *, VBS_ListData_t   *,
                     int, int *, int *);
void              UIG_NewModifyProp(struct Gadget *, struct Window *,\
                     struct Requester *, long, long, long, long, long, long);
struct Window    *UIG_OpenWindow(struct Screen *, struct NewWindow *, int);
struct TextFont  *UIG_OpenFont(char *, int);

void              BAG_PrepareTree(VBS_DirNode_t *,
                      struct RastPort *, long *, long *, long *,
                      VBS_ListData_t *, int, int);
void              BAG_PrintTree(VBS_DirNode_t *);
void              BAG_PrintMsg(char *);
void              UIG_ProcessGadget(struct RastPort *, struct Gadget *);
void              UIG_ProcessGadgets(struct RastPort *, struct Gadget *);
void              UIG_ProcessRequester(struct Requester *);
extern VBS_error_t       VBS_ReadMem(char *, long);
extern LONG       VBS_ReadTimer(void);
VBS_error_t       BAG_RestoreFilesTo(VBS_DirNode_t *, char *, char *, long);
extern VBS_error_t      VFG_RestoreFloppy(int);
void              BAG_RestoreVideoScreen(void);
void              BAG_RestoreScreen(void);
extern void       VBS_RestoreScreen(void);
VBS_error_t       BAG_RestoreTree(VBS_DirNode_t **, char *, long);
VBS_error_t       BAG_ScanDirectories(VBS_DirNode_t **, char *,
                      struct RastPort *, long *, long *, long *,
                      VBS_ListData_t *, int, int);
VBS_error_t       BAG_Search(char, char *, char * );
extern char      *VBS_Search(void);
extern long       VBS_SearchFirstBlockSync(void);
extern void       VFG_SetBuffers(APTR, APTR, APTR, APTR);
void              BAG_SetupScreen(struct Screen *);
void              BAG_SetupVideoScreen(struct Screen *);
void              BAG_ShowMessage(struct Screen *, char *, char *, UWORD, UWORD);
extern void       VBS_ShowBlack(void);
extern void       VBS_ShowCopperList(USHORT *);
extern void       VBS_ShowPlane(PLANEPTR, int, int, int, int, int, int ,int);
int               UIG_Time(char *, long, long);
int               VMG_StringToTime(char *, ULONG *);
int               VMG_TimeToString(char *, ULONG);
void              UIG_ToggleGadget(struct RastPort *, struct Gadget *);
VBS_error_t       BAG_VerifyFilesFrom(VBS_DirNode_t *, char *, char *, char *, long);
extern BOOL       VBS_VerifyMem(char *, long);
extern void       VBS_VertBServer(void);
extern BOOL       VBS_WaitFire(void);
extern void       VBS_WriteBackupHeader(char *);
extern void       VBS_WriteErrorIndicator(void);
extern void       VBS_WriteFirstBlockSync(void);
VBS_error_t       BAG_WriteHeader(char, char *, char *, struct DateStamp *);
extern void       VBS_WriteMem(char *, long, int);
extern void       VBS_WriteTestPattern(char *);
extern void       VBS_WriteVisual(PLANEPTR);
void              SRTG_SortLL(VBS_Node_t **, int(*)());
/*VBS_Node_t *, VBS_Node_t *));*/
void              SRTG_SortTree(VBS_DirNode_t **);

