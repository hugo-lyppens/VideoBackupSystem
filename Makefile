if eq($(a),)
   fout: 
endif

if eq($(a),d)
   EXECDEST = :VBS_D
   OBJDEST  = d_p_o
   LDINC  = deut_pal_ui
   VDINC  = deut_pal_ui
   VIDEO  = PAL
   LANG   = DEUTSCH
endif 

if eq($(a),gb)
   EXECDEST = :VBS_GB
   OBJDEST  = e_p_o
   LDINC  = eng_pal_ui
   VDINC  = eng_pal_ui
   VIDEO  = PAL
   LANG   = ENGLISH
endif

if eq($(a),usa)
   EXECDEST = :VBS_USA
   OBJDEST  = e_n_o
   LDINC  = eng_pal_ui
   VDINC  = eng_ntsc_ui
   VIDEO  = NTSC
   LANG   = ENGLISH
endif


GCSRCS      = vbs.c vm.c uisupport.c sort.c hdb.c \
              nametime.c error.c flb.c floppy.c fr.c path.c
AGACSRCS    = backup.c
OCSCSRCS    = backup_ocs.c
CSRCS       = GCSRCS AGACSRCS OCSCSRCS
GCOBJS      = $(patsubst %.c,$(OBJDEST)/%.o,$(GCSRCS))
AGACOBJS    = $(patsubst %.c,$(OBJDEST)/%.o,$(AGACSRCS))
OCSCOBJS    = $(patsubst %.c,$(OBJDEST)/%.o,$(OCSCSRCS))

GASMSRCS    = floppyrestore.a strcmpnocase.a image.a pickpt.a
OCSASMSRCS  = floppybackup_ocs.a videointerface_ocs.a writeclist0.a writeclist1.a
AGAASMSRCS  = floppybackup.a videointerface.a
ASMSRCS     = GASMSRCS OCSASMSCRS AGAASMSRCS 
GASMOBJS    = $(patsubst %.a,$(OBJDEST)/%.o,$(GASMSRCS))
OCSASMOBJS  = $(patsubst %.a,$(OBJDEST)/%.o,$(OCSASMSRCS))
AGAASMOBJS  = $(patsubst %.a,$(OBJDEST)/%.o,$(AGAASMSRCS))

AGAEXECUTABLE = $(EXECDEST)/VBS15
OCSEXECUTABLE = $(EXECDEST)/VBS_OCS

aga: setcflags $(AGAEXECUTABLE)
all: setcflags $(AGAEXECUTABLE) $(OCSEXECUTABLE)
ocs: setcflags $(OCSEXECUTABLE)


$(OBJDEST)/%.o: %.c
	sc $(SCFLAGS) OBJNAME $(OBJDEST)/ $<


$(OBJDEST)/%.o: %.a
	GenIm2 -l -o$@ $<


  
setcflags:
	SCFLAGS     = DEF $(VIDEO)_VIDEO DEF $(LANG) IDIR $(VDINC) IDIR $(LDINC)
	echo >tvsystem.i "$(VIDEO) equ 1"   


$(AGAEXECUTABLE): $(GCOBJS) $(AGACOBJS) $(GASMOBJS) $(AGAASMOBJS)
	sc LINK  \
   $(GCOBJS) $(AGACOBJS) $(GASMOBJS) $(AGAASMOBJS) \
   LIB LIB:amiga.lib VERBOSE TO $*


#DEFINE _XCEXIT=_stub DEFINE _printf=_stub DEFINE _puts=_stu

$(GCOBJS): vbs.h $(LDINC)/LD_vbstext.h
$(AGACOBJS): vbs.h $(LDINC)/LD_vbstext.h
$(OCSCOBJS): vbs.h $(LDINC)/LD_vbstext.h
  

$(OBJDEST)/vm.o:  $(VDINC)/VD_vm_ui.c vm_ui.h

$(OBJDEST)/hdb.o: $(VDINC)/VD_hdb_ui.c hdb_ui.h

$(OBJDEST)/floppy.o: $(LDINC)/floppy_ui.c floppy_ui.h

$(OBJDEST)/nametime.o:  $(LDINC)/nametime_ui.c nametime_ui.h

$(OBJDEST)/path.o:   $(LDINC)/path_ui.c path_ui.h

$(OBJDEST)/error.o:  $(LDINC)/error_ui.c


$(GASMOBJS): video.i vbserror.i
$(AGAASMOBJS): video.i vbserror.i
$(OCSASMOBJS): video.i vbserror.i


   
