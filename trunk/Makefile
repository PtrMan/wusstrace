# CC=gcc
# CXX=g++

CFLAGS=-Wall -O3 -Wfatal-errors -Ilibwst -DMINGW
CFLAGS_DRIVER=-Wall
LDFLAGS=-lboost_serialization -lboost_system \
	-lboost_thread 

.PHONY: all clean clean-all

SRCS = hook.cc wusstrace.cc wusstrace-dll.cc hook.cc \
	common.cc logger.cc bitset.cc console.cc pending.cc 

OBJS = $(SRCS:.cc=.o)

EXES = wusstrace.exe wusstrace.dll

all: libwst_ $(EXES)

libwst_:
	$(MAKE) -C libwst -f Makefile.win32

wusstrace.exe:
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

wusstrace.dll:
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

%.o: 
	$(CXX) -c $(CFLAGS) -o $@ $<

tags:
	-etags *.cc *.hh

clean:
	$(MAKE) -C libwst -f Makefile.win32 clean
	-del $(OBJS)
	-del TAGS

clean-all: clean
	$(MAKE) -C libwst -f Makefile.win32 clean-all
	-del $(EXES)

tarball:
	cd ..; tar -X WUSSTrace/.private -czvf wusstrace.tar.gz WUSSTrace; cd -

dist: $(EXES)
	strip wusstrace.exe || true
	strip wusstrace.dll || true
	upx wusstrace.exe || true
	upx wusstrace.dll || true

depend dep deps: $(SRCS)
	@echo Generating deps for:
	@echo $(SRCS) 
	@sed "/^### autodeps/q" Makefile > Makefile.tmp
	@$(CXX) $(CFLAGS) -MM $(SRCS) >> Makefile.tmp
	@set -e; \
	if cmp -s Makefile.tmp Makefile ; then \
		echo Makefile unchanged; \
		del Makefile.tmp; \
	else \
		echo Updating Makefile; \
		cp -f Makefile.tmp Makefile; \
		del Makefile.tmp; \
	fi

# ##################################
# ########## Dependencies ##########
# ##################################

wusstrace.exe: wusstrace.o common.o console.o
wusstrace.dll: wusstrace-dll.o common.o hook.o logger.o \
	bitset.o console.o pending.o libwst/libwst.a

### autodeps 
hook.o: hook.cc bitset.hh config.hh debug.hh hook.hh common.hh \
  libwst/type_winxp.hh libwst/syscall_tracer.hh libwst/syscall.hh \
  libwst/wstconfig.hh libwst/type_winxp.hh libwst/syscall_winxp.hh \
  libwst/tls.hh
wusstrace.o: wusstrace.cc config.hh console.hh common.hh \
  libwst/type_winxp.hh
wusstrace-dll.o: wusstrace-dll.cc bitset.hh config.hh hook.hh common.hh \
  libwst/type_winxp.hh console.hh libwst/syscall.hh libwst/wstconfig.hh \
  libwst/type_winxp.hh libwst/syscall_winxp.hh logger.hh \
  libwst/serialize-xml.hh libwst/serialize.hh \
  libwst/serialize_internal.hh libwst/syscall.hh libwst/syscall_tracer.hh \
  libwst/tls.hh libwst/blob.hh libwst/syscall_winxp_args.hh \
  libwst/syscall_tracer.hh
hook.o: hook.cc bitset.hh config.hh debug.hh hook.hh common.hh \
  libwst/type_winxp.hh libwst/syscall_tracer.hh libwst/syscall.hh \
  libwst/wstconfig.hh libwst/type_winxp.hh libwst/syscall_winxp.hh \
  libwst/tls.hh
common.o: common.cc config.hh console.hh common.hh libwst/type_winxp.hh
logger.o: logger.cc debug.hh config.hh common.hh libwst/type_winxp.hh \
  logger.hh libwst/syscall.hh libwst/wstconfig.hh libwst/type_winxp.hh \
  libwst/syscall_winxp.hh libwst/serialize-xml.hh libwst/serialize.hh \
  libwst/serialize_internal.hh libwst/syscall.hh libwst/syscall_tracer.hh \
  libwst/tls.hh libwst/blob.hh libwst/syscall_winxp_args.hh \
  libwst/syscall_tracer.hh console.hh pending.hh
bitset.o: bitset.cc bitset.hh
console.o: console.cc console.hh config.hh
pending.o: pending.cc pending.hh libwst/syscall.hh libwst/wstconfig.hh \
  libwst/type_winxp.hh libwst/syscall_winxp.hh debug.hh config.hh
