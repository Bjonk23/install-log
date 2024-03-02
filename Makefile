include project.mk

all: $(PROGRAM)

INCPATH += $(patsubst %,-I%,$(MODULES))
CFLAGS += -pipe $(LIBPATH) $(INCPATH)

# Include module descriptions
include $(patsubst %,%/module.mk,$(MODULES))

# Determine the object and dependency files
OBJ := $(patsubst %.c,%.o,$(filter %.c,$(SRC)))
DEP := $(patsubst %.c,%.d,$(filter %.c,$(SRC)))

# Link the program
$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

# Generate object and dependency files from source
%.d %.o: %.c
	$(CC) $(CFLAGS) -MMD -c -o $(patsubst %.d,%.o,$@) $<

# Include dependencies
-include $(DEP)

# Remove generated files
clean:
	-rm -f core $(PROGRAM) $(OBJ) $(DEP)

# Place install-log in system directories
install: all
	install -m 755 $(PROGRAM) /usr/bin
	install -m 644 install-log.rc /etc

# Tell make about special (non-file) targets
.PHONY: all clean run install
