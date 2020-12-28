*video.i
	OPT	o+,ow-
	INCDIR	"INCLUDE:"
	INCLUDE	"exec/types.i"
	INCLUDE	"exec/libraries.i"
	INCLUDE	"exec/funcdef.i"
	INCLUDE	"exec/memory.i"
	INCLUDE	"exec/exec_lib.i"
	INCLUDE	"graphics/display.i"
	INCLUDE	"hardware/custom.i"
	INCLUDE	"hardware/intbits.i"
	INCLUDE	"hardware/dmabits.i"
	INCLUDE	"hardware/adkbits.i"			
	INCLUDE	"hardware/cia.i"
	INCLUDE	"graphics/graphics_lib.i"

	INCLUDE	"tvsystem.i"


			

PERIODE		equ	4
NUMREPEAT	equ	3
HSTART		equ	113
*107+8

VISHEIGHT	equ	150
WHITE		equ	$FFF
BLACK		equ	$000
TRACKSINCACHE	equ	4
BYTESPERBLOCK	equ	64

SYNC		equ	$4489
SECTORSPERTRACK	equ	11
NUMTRACKS	equ	160
SECSIZE		equ	$440
SEEKN		equ	-2
SEEKP		equ	2
GAPSIZE		equ	$500
BLOCKSPERTRACK	equ	94
TRACKBUFSIZE	equ	GAPSIZE+128*BLOCKSPERTRACK+4
RETRIES		equ	4
PREREAD		equ	3
POSTGAP		equ	0
PREGAP		equ	1
WRITTEN		equ	2

		ifd	PAL
BLOCKSPERFRAME	equ	16
FRAMESPERTRACK	equ	6
VSTART		equ	32
WIDTH		equ	352
;xHEIGHT		equ	282
VIDEOHEIGHT	equ	272
RSHEIGHT	equ 	271
RSBYTESPERLINE	equ 	3
RSBLOCKSIZE	equ	86*RSBYTESPERLINE
RSBLOCKSPERFRAME equ	3
RSBLOCKHDRSIZE	equ	2*RSBYTESPERLINE
RSBLOCKTRAILERSZ equ	1*RSBYTESPERLINE
		endc
						
		ifd	NTSC
BLOCKSPERFRAME	equ	13
FRAMESPERTRACK	equ	8
VSTART		equ	32
WIDTH		equ	352
;xHEIGHT		equ	227

VIDEOHEIGHT	equ	221
RSHEIGHT	equ 	220
RSBYTESPERLINE	equ 	3
RSBLOCKSIZE	equ	69*RSBYTESPERLINE
RSBLOCKSPERFRAME equ	3
RSBLOCKHDRSIZE	equ	2*RSBYTESPERLINE
RSBLOCKTRAILERSZ equ	1*RSBYTESPERLINE
		endc

DATAOFFSET	equ	8*8+4+12*4

* video sync values:
FIRSTBLOCKSYNC	equ	$44
TRACKSYNC	equ	$55
BACKUPSYNC	equ	$33
HEADERSYNC0	equ	$02
HEADERSYNC1	equ	$CC
ERRORSYNC	equ	$66


BH0		equ	%11011011
BH1		equ	%00001111
BH2		equ	%11110000
BH3		equ	%10101010
BH4		equ	%00110011
BH5		equ	%00000000

N		equ	255
K		equ	239




INTF_MASTER	equ	INTF_INTEN
ciaa		equ	$BFE001
ciab		equ	$BFD000

DSKF_DMAEN 	equ	(1<<15)
DSKF_WRITE 	equ	(1<<14)

CIAF_NDSKSEL	equ	(%01111000)
CIAF_NDSKSEL0	equ	(%01111000-CIAF_DSKSEL0)
CIAF_NDSKSEL1	equ	(%01111000-CIAF_DSKSEL1)
CIAF_NDSKSEL2	equ	(%01111000-CIAF_DSKSEL2)
CIAF_NDSKSEL3	equ	(%01111000-CIAF_DSKSEL3)


CWAIT		macro
CW_MASK	set		$FFFE
			ifnc		'\3',''
CW_MASK	set		\3
			endc			
			dc.w		(((\2&255)<<8)+(\1))|1,CW_MASK
			endm
			
CMOVE		macro
			dc.w		\2,\1
			endm

wrevenbyte8 macro
			add.b		d0,d0
			scc.b		d1
*			scc.b		8(a0)
			add.w		d0,d0
			add.w		d0,d0
			move.l	(a2,d0.w),(a0)+
			move.l	(a3,d0.w),(a0)+
			move.b	d1,(a0)+
			sf.b		(a0)+
*			addq.w	#2,a0
			endm
			
wrevenbyte9 macro
			st.b		(a0)+
			lsr.w		#1,d0
			scc.b		(a0)+
			add.w		d0,d0
			add.w		d0,d0
			move.l	(a2,d0.w),(a0)+
			move.l	(a3,d0.w),(a0)+
			endm
			
wroddbyte9  macro
			move.w	#$00FF,(a0)+
			moveq		#0,d1
			move.b	d0,d1
			add.w		d1,d1
			add.w		d1,d1
			move.l	(a2,d1.w),(a0)+
			move.l	(a3,d1.w),(a0)+
			cmp.w		#256,d0
			slt.b		(a0)+
			clr.b		(a0)+
*			addq.l	#1,a0
			endm

wroddbyte8  macro
			move.w	#$00FF,(a0)+
			add.w		d0,d0
			add.w		d0,d0
			move.l	(a2,d0.w),(a0)+
			move.l	(a3,d0.w),(a0)+
*			addq.l	#4,a0
			clr.w		(a0)+
			endm



StartLijst	equ 	38


			STRUCTURE TRACKBUFFER,0
			APTR		TB_bufadr				;points to starting address of buffer
			APTR		TB_beginbuffer			;points first sector in buffer
			APTR		TB_gap					;points to beginning of track gap
			APTR		TB_aftergap				;points to first sector after gap
			APTR		TB_endbuffer			;points just after last sector read
			UBYTE		TB_valid					;whether this is a valid track			
			UBYTE		TB_pad0
			LABEL		TB_SIZEOF




			STRUCTURE VARIABLES,0
			APTR		old6C
			APTR		grafpage

			USHORT	oldadkcon
			USHORT	oldintena
			USHORT	olddmacon
			UBYTE		ciabprb
			UBYTE		second_time
			UWORD		blocknr
			UWORD		nblocksnok
			APTR		blocksok
			UWORD		headersync
			UWORD		breakval
						
			USHORT	track
			USHORT	oldtrack
			USHORT	dumptracknr

			APTR		writepos
			APTR		dumptrack
			UWORD		copofsdelt
			UWORD		framecount
			UWORD		numrepeat
			UWORD		delaycounter
			APTR		copfirst
			APTR		copsecond
			
			APTR		displaylist
			APTR		writelist
			APTR		errspecpt

			STRUCT	trackbuffers,TB_SIZEOF*TRACKSINCACHE
			APTR		flipadr
			APTR		topadr
			UBYTE		vb_occurred
			UBYTE		writepart
			ULONG		countvertb
			APTR		vb_routine
			APTR		actiplane
			APTR		nextplane
			APTR		actiview
			APTR		nextview
			UBYTE		disabled
			UBYTE		in_vb
			UWORD		blockcounter
			ULONG		decodetab
			
			APTR		curscraddr
			UWORD		blocknum
			UWORD		blockinframe
			APTR		offset
			APTR		length
			WORD		raarcnt
			WORD		samecnt
			STRUCT		bhsr,8
			ULONG		numcw
			ULONG		numcw_x_n_m_k
			APTR		regdata
			LABEL		VARIABLES_SIZEOF

			

