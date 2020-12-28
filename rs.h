#define HIGHBIT	256

extern UBYTE 	RSG_machten[HIGHBIT-1];
extern UBYTE 		RSG_logaritme[HIGHBIT], RSG_multinv[HIGHBIT];

extern UBYTE   *RSG_multiply;
extern UBYTE   *RSG_shiftregtab;


VBS_error_t	RSG_Init(void);
void		RSG_Cleanup(void);

#define	N	255
#define K	239
