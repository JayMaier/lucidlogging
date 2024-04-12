ARCH_TYPE = $(shell dpkg --print-architecture)

# ifeq ($(ARCH_TYPE), amd64)

# LDFLAGS = -LArenaSDK_Linux_ARM64/lib64 \
#           -LArenaSDK_Linux_ARM64/GenICam/library/lib/Linux64_x64 \
#           -LArenaSDK_Linux_ARM64/ffmpeg \
# 		  $(OPENCV_LIB_PATH)
          
# GENICAMLIBS = -lGCBase_gcc54_v3_3_LUCID \
#               -lGenApi_gcc54_v3_3_LUCID \
#               -lLog_gcc54_v3_3_LUCID \
#               -llog4cpp_gcc54_v3_3_LUCID \
#               -lMathParser_gcc54_v3_3_LUCID \
#               -lNodeMapData_gcc54_v3_3_LUCID \
#               -lXmlParser_gcc54_v3_3_LUCID

# OUTDIR = ../../../OutputDirectory/Linux/x64Release/

# else ifeq ($(ARCH_TYPE), armhf)

# LDFLAGS = -LArenaSDK_Linux_ARM64/lib \
#           -LArenaSDK_Linux_ARM64/GenICam/library/lib/Linux32_ARMhf \
#           -LArenaSDK_Linux_ARM64/ffmpeg \
# 		  $(OPENCV_LIB_PATH)

# GENICAMLIBS = -lGCBase_gcc540_v3_3_LUCID \
#               -lGenApi_gcc540_v3_3_LUCID \
#               -lLog_gcc540_v3_3_LUCID \
#               -llog4cpp_gcc540_v3_3_LUCID \
#               -lMathParser_gcc540_v3_3_LUCID \
#               -lNodeMapData_gcc540_v3_3_LUCID \
#               -lXmlParser_gcc540_v3_3_LUCID


# OUTDIR = ../../../OutputDirectory/armhf/x32Release/

# else 
ifeq ($(ARCH_TYPE), arm64)

LDFLAGS = -L../ArenaSDK_Linux_ARM64/lib \
          -L../ArenaSDK_Linux_ARM64/GenICam/library/lib/Linux64_ARM \
          -L../ArenaSDK_Linux_ARM64/ffmpeg \
		  $(OPENCV_LIB_PATH)

GENICAMLIBS = -lGCBase_gcc54_v3_3_LUCID \
              -lGenApi_gcc54_v3_3_LUCID \
              -lLog_gcc54_v3_3_LUCID \
              -llog4cpp_gcc54_v3_3_LUCID \
              -lMathParser_gcc54_v3_3_LUCID \
              -lNodeMapData_gcc54_v3_3_LUCID \
              -lXmlParser_gcc54_v3_3_LUCID


OUTDIR = .
endif

CC=g++

INCLUDE= -I../ArenaSDK_Linux_ARM64/include/Arena \
         -I../ArenaSDK_Linux_ARM64/include/Save \
         -I../ArenaSDK_Linux_ARM64/include/GenTL \
         -I../ArenaSDK_Linux_ARM64/GenICam/library/CPP/include \
		 $(OPENCV_INC_PATH)

CFLAGS=-Wall -g -O2 -std=c++11 -Wno-unknown-pragmas


FFMPEGLIBS = -lavcodec \
             -lavformat \
             -lavutil \
             -lswresample

LIBS= -larena -lsave -lgentl $(GENICAMLIBS) $(FFMPEGLIBS) -lpthread -llucidlog $(OPENCV_LIBS)
RM = rm -f

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)
DEPS = $(OBJS:%.o=%.d)

.PHONY: all
all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} $^ -o $@ $(LIBS)
	-mkdir -p $(OUTDIR)
	-cp $(TARGET) $(OUTDIR)

%.o: %.cpp ${SRCS}
	${CC} ${INCLUDE}  ${LDFLAGS} -o $@ $< -c ${CFLAGS}

${DEPS}: %.cpp
	${CC} ${CLAGS} ${INCLUDE} -MM $< >$@

-include $(OBJS:.o=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET} ${OBJS} ${DEPS}
