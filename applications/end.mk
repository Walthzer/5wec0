# The libpynq library.
SUDO?=sudo
LIBPYNQ_A:=${PROJECT_ROOT}/build/lib/libpynq.a

# Generate list of objects and dependency files.
SOURCES_OBJ+=$(SOURCES:.c=.o)
D_FILES:=$(SOURCES_OBJ:.o=.d)

BUILD_DIR:=${PROJECT_ROOT}/build

all: main

# Make sure the libpynq is compiled.
${LIBPYNQ_A}:
	make -C ${PROJECT_ROOT}

# Generate dependencies.
%.d: %.c
	$(DEP_PRINT)
	$(VERBOSE)${CC} -c -MT"$*.o" -MM -o $@  $^ ${CFLAGS}

%.o: %.c
	$(OBJ_PRINT)
	$(VERBOSE)${CC} -c -o $@ $< ${CFLAGS}

indent: ${SOURCES}
	clang-format -i $^

# when doing a clean do not build the dependencies files.
ifneq (clean,$(MAKECMDGOALS))
-include ${D_FILES}
endif

main: ${SOURCES_OBJ} ${LIBPYNQ_A} ${EXTERNAL_LIBS}
	${BUILD_PRINT}
	$(VERBOSE)${CC} -o $@ $^   ${LDFLAGS}
	$(VERBOSE)${SUDO} setcap cap_sys_rawio+ep ./${@}

run: main
	./main

.PHONY: clean realclean indent
clean:
	$(CLEAN_PRINT)
	$(VERBOSE)rm -f ${SOURCES_OBJ} ${D_FILES} main

realclean veryclean: clean
	rm -rf ${BUILD_DIR}
