TARGET = template

O = o

# OBJS = src/sdltest.o
OBJS += src/main.o src/heap.o src/hud.o src/inventory.o src/level.o src/palette.o  src/pathfinder.o src/player.o \
src/render.o src/room.o src/title_screen.o src/utility.o src/world.o src/saver.o src/enemies.o src/achivements.o \
src/bullet.o src/minimap.o src/popup.o src/statistics.o


PSP_LARGE_MEMORY = 1

INCDIR = includes psp-includes
CFLAGS = -O3 -G0 -Wall -DPDCDEBUG
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = libs
LDFLAGS =
LIBS = -lPDCurses -lSDL -lSDLmain -lSDL_ttf -lfreetype -lGL -lGLU -lglut -lz -lpspvfpu -lpsphprm -lpspsdk -lpspctrl -lpspumd -lpsprtc \
       -lpsppower -lpspgum -lpspgu -lpspaudiolib -lpspaudio -lpsphttp -lpspssl -lpspwlan \
	   -lpspnet_adhocmatching -lpspnet_adhoc -lpspnet_adhocctl -lm -lpspvram

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = SSYP Game

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

%.o: %.bmp
	bin2o -i $< $@ $(addsuffix _bmp, $(basename $(notdir $<) ))

%.o: %.jpg
	bin2o -i $< $@ $(addsuffix _jpg, $(basename $(notdir $<) ))

%.o: %.png
	bin2o -i $< $@ $(addsuffix _png, $(basename $(notdir $<) ))
