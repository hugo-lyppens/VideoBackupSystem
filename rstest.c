#include <math.h>
#include <stdio.h>
#include "vbs.h"
#include "rs.h"

extern unsigned long VWG_ErrCorr;

long	main(argc, argv)
int	 argc;
char	*argv[];
{
	char	*data = NULL, *modulotab = NULL, *p;
	int	 i,k,j,f,v;
	char	*newfrom, *newto;
	char	*data2 = 0, *data3 = 0;
	BPTR	 fn;
	long	 fl;
	long     l;
	
	if(argc<1000)
		l = 299900;
	
	data = AllocMem(1024*255, 0);
	if(!data)
		goto quit;
	CopyMemQuick(data, data+1, l);
	data2 = AllocMem(1024*255, 0);
	if(!data2)
		goto quit;
	data3 = AllocMem(1024*255, 0);
	if(!data3)
		goto quit;
	modulotab = AllocMem(1024*16, 0);
	if(!modulotab)
		goto quit;
#ifdef KKK		
	if(argc<2)
		goto quit;
	fn = Open(argv[1], MODE_OLDFILE);
	if(fn<=0)
		goto quit;
	fl = Read(fn, data, 1024*255);
	if(fl<=0)
		goto quit;
	Close(fn);	
	data2[1024]=52;
	printf("Now compressing 100x \n");
	for(i = 0; i<1; i++)
		DCG_Compress(data, fl, &newfrom,
			data2, 1024, &newto, modulotab);
	printf("%ld %ld compressed to %ld\n", fl, newfrom-data, newto-data2);
	printf("Now uncompressing 100x \n");
	for(i = 0; i<1; i++)
		DCG_Uncompress(data2, &newfrom, data3, &newto);
	printf("%ld %ld uncompressed to %ld\n", fl, newfrom-data2,
		newto-data3);
	fl=1024;
	for(k = 0; k<fl; k++) {
		if(data[k]!=data3[k]) {
			printf("%08lx %02x %02x\n", k, data[k], data3[k]);
		}
	}
	printf("data2[1024]=%d\n", data2[1024]);
	if(memcmp(data, data3, fl))
		printf("FOUT!\n");
		
#endif		
		
#if 1
	if(RSG_Init())
		goto quit;
			
	printf("RSG_shiftregtab %lx RSG_mul %lx data %lx redundata %lx mt %lx\n",
		RSG_shiftregtab, RSG_multiply, data, data+239*1024, modulotab);

     for(k = 0; k<20000; k++) {
	memset(data, 0, 1024*239);
	VWG_Encode(data, 10, RSG_shiftregtab, modulotab);
	for(f = 0; f<8; f++) {
		l = (lrand48()>>12) % 255;
		v = (lrand48()>>13) & 0xFF;
		data[l*1024] ^= v;
		printf("Error of %02x at %02lx\n", v, l);
	}
	printf("Now decoding ... %d\n", k);
	VWG_Decode(data, 10, RSG_shiftregtab, modulotab);
	i=VWG_CorrectErrs(data, 10, modulotab);
	printf("correrr res %d errcorr %d\n", i, VWG_ErrCorr);
	p = data;
	for(i = 0; i<1024*255/4; i++)
		if(*p++)
			break;
	if(i<1024*255/4) {	
		printf("FOUT!\n");
		break;
	}
	if(!(*((char *)0xbfe001) &64))
		break;
     }
#endif
quit:
	if(data)
		FreeMem(data, 1024*255);
	if(data2)
		FreeMem(data2, 1024*255);
	if(data3)
		FreeMem(data3, 1024*255);
	if(modulotab)
		FreeMem(modulotab, 1024*16);		
	RSG_Cleanup();	
	return(0);
}
