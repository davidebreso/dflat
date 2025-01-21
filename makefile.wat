#-------------------------------------------------------------------
#      D - F L A T   M A K E F I L E   -   W A T C O M   C
#-------------------------------------------------------------------

all : memopad.exe memopad.hlp

#-------------------------------------------------------------------
#  Delete the FULL macro for a minimal D-Flat application. You can
#  selectively remove features by deleting #define statements at
#  the beginning of DFLAT.H
#-------------------------------------------------------------------
FULL = BUILD_FULL_DFLAT
#-------------------------------------------------------------------
#  Delete the TESTING macro to eliminate the D-Flat Log and the Reload
#  Help file selection on the Help menu
#-------------------------------------------------------------------
TESTING = TESTING_DFLAT
#-------------------------------------------------------------------
MODEL = l
#------------------------------------------------
COMPILE = wcc -bt=dos -os -dWATCOM -d$(FULL) -d$(TESTING) -w4 -s -m$(MODEL)
#------------------------------------------------

.c.o:
    $(COMPILE) -fo=$@ $*

memopad.exe : memopad.o dialogs.o menus.o dflat.lib
     wcl memopad.o dialogs.o menus.o dflat.lib -k8192 -fe=memopad.exe

dflat.lib :   window.o video.o message.o                         &
              mouse.o console.o textbox.o listbox.o            &
              normal.o config.o menu.o menubar.o popdown.o   &
              rect.o applicat.o keys.o sysmenu.o editbox.o   &
              dialbox.o button.o fileopen.o msgbox.o           &
              helpbox.o log.o lists.o statbar.o decomp.o     &
              combobox.o pictbox.o calendar.o barchart.o       &
              clipbord.o search.o dfalloc.o checkbox.o         &
              text.o radio.o box.o spinbutt.o  watch.o       &
              slidebox.o direct.o editor.o
	rm -f dflat.lib
	wlib dflat @dflat

huffc.exe : huffc.c htree.c htree.h
     wcl -bcl=dos -dWATCOM -ml huffc.c htree.c

fixhelp.exe : fixhelp.c decomp.c
     wcl -bcl=dos -dWATCOM -ml fixhelp.c decomp.c


# Note that if you're compiling in 64-bit Windows, huffc.exe and fixhelp.exe
# will fail to execute and you will have to do this last step manually in
# DOS.
memopad.hlp : memopad.txt huffc.exe fixhelp.exe
	emu2 huffc.exe memopad.txt memopad.hlp
	emu2 fixhelp.exe memopad
	
clean: .symbolic
    rm -f *.o
    rm -f *.lib
    rm -f *.err
    rm -f *.hlp
    rm -f *.exe

