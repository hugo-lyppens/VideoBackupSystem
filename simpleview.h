
typedef struct {
	UWORD			 X, Y, Width, Height, Depth;
	BOOL			 MakeVPort;
	ULONG			 ModeId;
	struct View		 View;
	struct ViewPort		 ViewPort;
	struct ViewExtra 	*ViewExtra;
	struct ViewPortExtra	*ViewPortExtra;
	struct RasInfo		 RasInfo;
	struct BitMap		 BitMap;
	struct ColorMap		*ColorMap;
} SimpleView;

BOOL	SVG_CreateSimpleView(SimpleView *, UWORD, UWORD, UWORD, UWORD,
			     UWORD, ULONG);
void	SVG_DestroySimpleView(SimpleView *);
			 