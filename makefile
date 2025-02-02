#-------------------------------------------------------------------
#        D - F A S T   M A K E F I L E  -  Borland C++ 2.0
#-------------------------------------------------------------------

all : demo.exe

#-------------------------------------------------------------------
# NOTE: Set DRIVE to match where you installed your compiler
#-------------------------------------------------------------------
DRIVE = d:\bcc
#-------------------------------------------------------------------
#  Delete the TESTING macro to eliminate the Reload
#  Help file selection on the Help menu.
#-------------------------------------------------------------------
TESTING = TESTING_DFLAT
#-------------------------------------------------------------------
#  This macro builds the full D-Flat system with all options enabled.
#  Comment it out for a minimum system or selectively
#  comment out the #defines at the top of dflat.h.
#-------------------------------------------------------------------
FULL = BUILD_FULL_DFLAT
#-------------------------------------------------------------------

MODEL = l
#-------------------------------------------------------------------
# NOTE: Delete the DEBUG, ASMDEBUG, and LINKDEBUG macros to
# build without debugging information in the .EXE
#-------------------------------------------------------------------
DEBUG = -v
ASMDEBUG = /zi
LINKDEBUG = /m /v
#-------------------------------------------------------------------
# NOTE: Temporary file space. Change to match your computer.
#       A RAM disk works best.
#-------------------------------------------------------------------
HEADERS=$(TEMP)\tcdef.sym
#------------------------------------------------
COMPILE = bcc $(DEBUG) -D$(TESTING) -D$(FULL) -DBCPP -c -d -m$(MODEL) -H=$(HEADERS)
ASM = tasm $(ASMDEBUG) /mx
LINK= tlink $(LINKDEBUG) $(DRIVE)\lib\c0$(MODEL)
LIBS= $(DRIVE)\lib\c$(MODEL)
#------------------------------------------------

.autodepend

.c.obj:
    $(COMPILE) {$*.c }

.asm.obj:
    $(ASM) $<

demo.exe : demo.obj dfast.lib
    $(LINK) demo,demo.exe,demo,dfast $(LIBS)

dfast.lib :   video.obj mouse.obj console.obj
	del dfast.lib
	tlib dfast @dfast.bld

huffc.exe : huffc.obj htree.obj
    $(LINK) huffc htree,$*.exe,$*,$(LIBS)

fixhelp.exe : fixhelp.obj decomp.obj
    $(LINK) fixhelp decomp,$*.exe,$*,$(LIBS)

memopad.hlp : memopad.txt huffc.exe fixhelp.exe
    huffc memopad.txt memopad.hlp
    fixhelp memopad

release: memopad.exe memopad.hlp memopad.map
    copy $< bin\

clean:
    del /q *.obj
    del /q *.map
    del /q *.lib
    del /q *.exe
    del /q *.bak
    del /q *.hlp
    del /q $(HEADERS)

