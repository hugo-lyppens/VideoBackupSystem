#include <exec/types.h>

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/copper.h>
#include <graphics/view.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxmacros.h>
#include <graphics/videocontrol.h>
#include <utility/tagitem.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

#include <stdlib.h>
#include <string.h>

#include "simpleview.h"




BOOL SVG_CreateSimpleView(sv, x, y, width, height, depth,
					modeid)
SimpleView	*sv;
UWORD	 	 x, y, width, height, depth;
ULONG		 modeid;
{
	BOOL				 error 	  = TRUE;
	int				 i;
	struct ColorMap			*cm	  = NULL;

	memset((char *)sv, 0, sizeof (SimpleView));
	sv->X = x; sv->Y = y; sv->Width = width; sv->Height = height;
	sv->Depth  = depth;
	sv->ModeId = modeid;
	InitView(&sv->View);
	sv->View.DxOffset	= x;
	sv->View.DyOffset	= y;

	InitBitMap(&sv->BitMap, depth, width, height);
	
	for(i = 0; i<depth; i++) {
		if(!(sv->BitMap.Planes[i] = AllocRaster(width, height)))
			goto quit;
	}
	sv->RasInfo.BitMap = &sv->BitMap;
	
	InitVPort(&sv->ViewPort);
	sv->View.ViewPort 	= &sv->ViewPort;
	sv->ViewPort.RasInfo 	= &sv->RasInfo;
	sv->ViewPort.DWidth	= width;
	sv->ViewPort.DHeight	= height;
	sv->ViewPort.Modes	= (UWORD)(modeid & 0x0000FFFF);
	
	cm = GetColorMap(1<<depth);
	if(!cm)
		goto quit;
	sv->ViewPort.ColorMap	= cm;
	sv->ColorMap = cm;

	MakeVPort(&sv->View, &sv->ViewPort);
	sv->MakeVPort = TRUE;
	MrgCop(&sv->View);
	
	error = FALSE;
quit:
	if(error) {
		SVG_DestroySimpleView(sv);
	}
	return(error);
}
	
void SVG_DestroySimpleView(sv)
SimpleView	*sv;
{
	int	 i;
	
	if(sv->View.LOFCprList)
		FreeCprList(sv->View.LOFCprList);
	if(sv->View.SHFCprList)
		FreeCprList(sv->View.SHFCprList);
	
	if(sv->MakeVPort)
		FreeVPortCopLists(&sv->ViewPort);
	if(sv->ColorMap)
		FreeColorMap(sv->ColorMap);
	for(i = 0; i<sv->Depth; i++) {
		if(sv->BitMap.Planes[i])
			FreeRaster(sv->BitMap.Planes[i], sv->Width, sv->Height);
	}
	memset((char *)sv, 0, sizeof (SimpleView));	
}
