#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include "vbs.h"
#include "rs.h"


#define POL 		0x171

/* "Coding Theory The Essentials PURE AND APPLIED MATH"
   "D.G. Hoffman, D.A. Leonard, C.C. Lindner, K.T. Phelps, C.A. Rodger,
    J.R. Wall.  Marcel Dekker, Inc.  ISBN 0-8247-8611-4"
    E-mail: KTPHELPS@DUCVAX.AUBURN.EDU
*/
/*#define POL     0x171
;0xb;0x13;0x171*/

UWORD		LastGlobal=7;
UBYTE 		RSG_machten[HIGHBIT-1];
UBYTE 		RSG_logaritme[HIGHBIT], RSG_multinv[HIGHBIT];
static short 	times2(short, short);
static short 	multinverse(short);

UBYTE	       *RSG_multiply = NULL;
UBYTE	       *RSG_shiftregtab = NULL;

static UBYTE 	genpol[] = {
	0x81, 0xE7, 0xF4, 0xBA, 0x72, 0x1E, 0xCF, 0xB2,
	0xD4, 0x62, 0x5F, 0x1B, 0x6C, 0xF3, 0x01, 0x54
};

#define times(a,b) multiply[a][b]

VBS_error_t	RSG_Init()
{
    unsigned short	getal, i, j, sh;
    UBYTE		*p;

    getal = 1;
    for(i = 0; i<HIGHBIT-1; i++) {
	RSG_machten[i] = getal;
	RSG_logaritme[getal] = i;
	getal <<=1;
	if(getal & HIGHBIT) {
	    getal ^= POL;
	}
    }
    RSG_multiply = (UBYTE *)AllocMem(65536L, 0);
    if(!RSG_multiply)
	goto error;
    for(i = 0; i<HIGHBIT; i++ ) {
	if(i) RSG_multinv[i] = multinverse(i);
	for(j = 0; j<HIGHBIT; j++) {
	    RSG_multiply[i*256+j] = times2(i, j);
	}
    }
    RSG_shiftregtab = (UBYTE *)AllocMem(65536L, 0);
    if(!RSG_shiftregtab)
    	goto error;
    for(sh = 0; sh<16; sh++) {
    	for(i = 0; i<HIGHBIT; i++) {
    	    p = &RSG_multiply[i*256];
    	    for(j = 0; j<16; j++) {
    	    	RSG_shiftregtab[((j+sh)&0x0f)+(i<<4)+(sh<<12)] =
    	    		p[genpol[j]];
    	    }
    	}
    }
    return(EOK);
error:
	RSG_Cleanup();
	return(EMEMORY);	    
}
void	RSG_Cleanup()
{
	if(RSG_multiply) {
		FreeMem(RSG_multiply,    65536L);
		RSG_multiply = NULL;
	}
	if(RSG_shiftregtab) {
		FreeMem(RSG_shiftregtab, 65536L);
		RSG_shiftregtab = NULL;
	}
	
}

static short multinverse(x)
short x;
{
    switch(x) {
    case 0:
	return(-1);
    case 1:
	return(1);
    default:
	return(RSG_machten[HIGHBIT-1-RSG_logaritme[x]]);
    }
}




static short times2(i, j)
short i, j;
{
    short r, b, k;

    r = 0; k = j;
    for(b = 1; b<HIGHBIT; b<<=1, k<<=1) {
	if(k & HIGHBIT) k^=POL;
	if(i & b) r ^= k;
    }
    return(r);
}
