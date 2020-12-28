#include "vbs.h"
#include "rs.h"

#define DATASIZERS	239*1024
#define CHECKSIZERS	16*1024
static UWORD  BA_emptypt[] =
{
   0x0000, 0x0000,

   0x0000, 0x0000,
   0x0000, 0x0000,

   0x0000, 0x0000
};

extern short VBS_empty_pointer[];

void	 FB_TestWrite()
{
	char	*data  = NULL;
	char	*check = NULL;
	int	 i;
	struct Window *w;

	RSG_Init();
	
	w = IntuitionBase->ActiveWindow;
	if(w){
		printf("w is %x\n", w);
 	       SetPointer(w, VBS_empty_pointer, 2, 16, 0, 0);
	}
	
	data = AllocMem(DATASIZERS, 0);
	if(!data)
		goto quit;
	for(i = 0; i<DATASIZERS; i++) {
		data[i]=i;
	}
	check = AllocMem(CHECKSIZERS, 0);
	if(!check)
		goto quit;
	memset(check, 0x45, CHECKSIZERS);
	BAG_SetupVideoScreen(NULL);
	VBS_WriteMemRS(0, 200, data, 10, RSG_shiftregtab, check);
	BAG_RestoreVideoScreen();
	
quit:	if(data)
		FreeMem(data, DATASIZERS);
	if(check)
		FreeMem(check, CHECKSIZERS);
	if(w)ClearPointer(w);
	RSG_Cleanup();
}

char nulls[16];

void	 FB_TestRead()
{
	unsigned char	*data  = NULL;
	unsigned char	*check = NULL;
	int	 i, r, c;
	unsigned long	offset, length;
	long		errcorr = 0;

	RSG_Init();
	

	data = AllocMem(DATASIZERS+CHECKSIZERS, MEMF_CLEAR);
	if(!data)
		goto quit;
	check = AllocMem(CHECKSIZERS, MEMF_CLEAR);
	if(!check)
		goto quit;
	memset(data, 0, DATASIZERS+CHECKSIZERS);
	offset = 0;
	for(;;) {
		printf("Looking for %d\n", offset);
		r = VBS_ReadMemRS(&offset, &length, (char *)data,
			  10, RSG_shiftregtab, check);
		printf("Result %d Offset %d Length %d\n", r, offset, length);
		if(r==EBREAK)
			break;
		if(!r) {
			errcorr=VWG_CorrectErrs(data, 10, check);
			printf("CorrErr %d\n", errcorr);
			for(c = 0; c<1024; c++) {
				if(memcmp(check+c*16, nulls, 16))
					printf("%d ", c);
			}
			if(errcorr)
				printf("\n");
		}
		offset+=length;
	}

quit:	if(data)
		FreeMem(data,  DATASIZERS+CHECKSIZERS);
	if(check)
		FreeMem(check, CHECKSIZERS);
	RSG_Cleanup();
}
