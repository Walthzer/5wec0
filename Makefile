ROOT_DIR:=.

# applications that are part of a release
RELEASE_APPLICATIONS=end.mk shared.mk \
adc \
display-cat \
display-test \
eventloop \
gpio-buttons \
interrupt \
interrupt-poll \
logtest \
pin-indexing-tool \
read-version \
scpi-bridge \
template \
uart

RELEASE=$(shell grep '\#define LIBPYNQ_RELEASE' library/version.c | sed s'!^[^"]*\("[^"][^"]*"\)$$!\1!g')
MAJOR=$(shell grep '\#define LIBPYNQ_VERSION_MAJOR' library/version.c | sed s'![^0-9]!!g')
MINOR=$(shell grep '\#define LIBPYNQ_VERSION_MINOR' library/version.c | sed s'![^0-9]!!g')
PATCH=$(shell grep '\#define LIBPYNQ_VERSION_PATCH' library/version.c | sed s'![^0-9]!!g')
DATE=$(shell date "+%Y-%m-%d %H:%M")
RELEASE_BASENAME=libpynq-${RELEASE}-v${MAJOR}.${MINOR}.${PATCH}
LABEL="release ${RELEASE} version ${MAJOR}.${MINOR}.${PATCH} of ${DATE}"

CFLAGS:=-I. -Iplatform/ -Ilibrary/ -Iexternal/ -lm -O0 -g3 -ggdb -Wextra -Wall
# when compiling empty library:
# CFLAGS:=-I. -Iplatform/ -Ilibrary/ -Iexternal/ -lm -O0 -g3 -ggdb

LIBRARIES_SOURCES:=$(wildcard library/*.c)
LIBRARIES_HEADERS:=$(wildcard library/*.h)
EXTERNAL_SOURCES:=$(wildcard external/*.c)
EXTERNAL_HEADERS:=$(wildcard external/*.h)
LIBRARIES_OBJECTS:=$(LIBRARIES_SOURCES:.c=.o) $(EXTERNAL_SOURCES:.c=.o)


BUILD_DIR:=${ROOT_DIR}/build
LIB_DIR:=${BUILD_DIR}/lib/
LIB_PYNQ:=${LIB_DIR}/libpynq.a
LIB_SCPI:=external/scpi-parser/libscpi/dist/

OBJECTS_LIBRARIES:=$(foreach obj,$(LIBRARIES_OBJECTS),${BUILD_DIR}/$(obj))

DIRS:=$(foreach obj,$(LIBRARIES_OBJECTS),${BUILD_DIR}/$(dir $(obj))/)


APPLICATIONS_DIR:=$(dir $(wildcard applications/*/Makefile))
APPLICATIONS:=$(foreach app_dir,$(APPLICATIONS_DIR), $(app_dir)main)
APPLICATIONS_CLEAN:=$(foreach app_dir,$(APPLICATIONS_DIR), $(app_dir)clean)
APPLICATIONS_INDENT:=$(foreach app_dir,$(APPLICATIONS_DIR), $(app_dir)indent)

SOURCES:=$(wildcard *.c)
SOURCES_OBJ:=$(SOURCES:.c=.o)


D_FILES:=$(OBJECTS_LIBRARIES:.o=.d) $(LIBRARIES:.o=.d)

all: ${LIB_PYNQ} ${LIB_SCPI}

# first time 
install:
	@# these warnings don't get displayed before the .d are made due to wrong(?) dependencies
	@echo 'WARNING: if compilation never finishes, then stop it with ^C'
	@echo 'WARNING: you may have missed the -DD flag when unzipping'
	@echo 'WARNING: remove this directory and unzip again with '\''unzip -DD -q '$(RELEASE_BASENAME)'.zip'
	@# alternatively: run find . -exec touch {} \;' in the current directory
	@ [ -f external/scpi-parser/README.md ] || echo "Error: the scpi parser is missing"
	$(MAKE) all applications
	@# always copy latest version
	cp applications/read-version/main bin/compatibility-check
	sudo setcap cap_sys_rawio+ep bin/compatibility-check
	cp applications/pin-indexing-tool/main bin/pin-indexing-tool
	sudo setcap cap_sys_rawio+ep bin/pin-indexing-tool

release:
	sh bin/make_release.sh $(RELEASE_APPLICATIONS)

${LIB_SCPI}:
	$(MAKE) -C external/scpi-parser/libscpi/

ifneq (clean,$(MAKECMDGOALS))
-include ${D_FILES}
endif

.PHONY: applications
applications: ${APPLICATIONS}

applications/%/main: applications/% | ${LIB_PYNQ} ${LIB_SCPI}
	$(MAKE) -C $^

${BUILD_DIR}/%.d: %.c | ${DIRS}
	${CC} -c -MT"${BUILD_DIR}/$*.o" -MM  -o $@ $^ ${CFLAGS}

${BUILD_DIR}/%.o: %.c | ${DIRS}
	${CC} -c -o $@ $< ${CFLAGS}


${LIB_PYNQ}: ${OBJECTS_LIBRARIES} | ${LIB_DIR}
	$(AR) rcs $@ $?


%/:
	mkdir -p ${@}

indent: indent-library indent-applications

indent-library: ${LIBRARIES_SOURCES} ${LIBRARIES_HEADERS}
	clang-format -i $^

applications/%/indent: applications/%
	$(MAKE) -C $^ indent

version:
	@echo libpynq $(LABEL)
	@echo libpynq-$(RELEASE)-v$(MAJOR).$(MINOR).$(PATCH)

doc: ${BUILD_DIR}/html

${BUILD_DIR}/html: documentation/ryb.doxy ${LIBRARIES_SOURCES} ${LIBRARIES_HEADERS}
	sed -e 's/^PROJECT_NUMBER         =.*/PROJECT_NUMBER         = \"('$(LABEL)'\")/' documentation/ryb.doxy > documentation/ryb.doxy.new
	doxygen documentation/ryb.doxy.new
	rm documentation/ryb.doxy.new
	${MAKE} -C ${BUILD_DIR}/latex/

# can only clean both library and applications
clean: ${APPLICATIONS_CLEAN}
	rm -rf ${BUILD_DIR} bin/compatibility-check bin/pin-indexing-tool

applications/%/clean: applications/%
	$(MAKE) -C $^ clean

.PHONY: indent indent-library indent-applications doc clean release install doc version
