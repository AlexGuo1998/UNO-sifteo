APP = uno

include $(SDK_DIR)/Makefile.defs

OBJS = \
	$(ASSETS).gen.o \
	global.o \
	scroller.o \
	stack.o \
	gamelogic.o \
	animation.o \
	cubeman.o \
	menulogic.o \
	savedata.o \
	main.o

ASSETDEPS += *.png $(ASSETS).lua
CDEPS += *.h

CFLAGS += -D CUBE_ALLOCATION=12 -D MENU_LOGS_ENABLED

# build assets.html to proof stir-processed assets.
# comment out to disable.
ASSETS_BUILD_PROOF := yes

include $(SDK_DIR)/Makefile.rules
