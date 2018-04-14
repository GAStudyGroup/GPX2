COM_COLOR   = \033[0;34m
OBJ_COLOR   = \033[0m
OK_COLOR    = \033[0;32m
ERROR_COLOR = \033[0;31m
WARN_COLOR  = \033[0;33m
NO_COLOR    = \033[m

OK_STRING    = "[OK]"
ERROR_STRING = "[ERROR]"
WARN_STRING  = "[WARNING]"
COM_STRING   = "Compiling"


define run_and_test
printf "%b" "$(COM_COLOR) $(COM_STRING) $(OBJ_COLOR) $(@F) $(NO_COLOR)\r"; \
$(1) 2> $@.log; \
RESULT=$$?; \
if [ $$RESULT -ne 0 ]; then \
	printf "%-60b%b" "$(COM_COLOR) $(COM_STRING) $(OBJ_COLOR) $@" "$(ERROR_COLOR) $(ERROR_STRING) $(NO_COLOR)\n"   ; \
elif [ -s $@.log ]; then \
	printf "%-60b%b" "$(COM_COLOR) $(COM_STRING) $(OBJ_COLOR) $@" "$(WARN_COLOR) $(WARN_STRING) $(NO_COLOR)\n"   ; \
else  \
	printf "%-60b%b" "$(COM_COLOR) $(COM_STRING) $(OBJ_COLOR) $(@F)" "$(OK_COLOR) $(OK_STRING) $(NO_COLOR)\n"   ; \
fi; \
cat $@.log; \
rm -f $@.log; \
exit $$RESULT
endef


app = GPX2

srcExt = cpp
srcDir = src
objDir = obj
binDir = bin
inc = $(shell find -type f -iname "*.hpp" -printf "%h\n" | sort -u)

debug = 0

CFlags = -Wall -std=gnu++17 -O3
LDFlags =
libs =
libDir =

#************************ DO NOT EDIT BELOW THIS LINE! ****************
ifeq ($(debug),1)
	debug=-g
else
	debug=
endif

inc := $(addprefix -I,$(inc))
libs := $(addprefix -l,$(libs))
libDir := $(addprefix -L,$(libDir))
CFlags += -c $(debug) $(inc) $(libDir) $(libs)
sources := $(shell find $(srcDir) -name '*.$(srcExt)')
srcDirs := $(shell find . -name '*.$(srcExt)' -exec dirname {} \; | uniq)
objects := $(patsubst %.$(srcExt),$(objDir)/%.o,$(sources))

ifeq ($(srcExt),cpp)
	CC = $(CXX)
else
	CFlags += -std=gnu99
endif

.phony: all clean cleanbin
	
all: $(binDir)/$(app)

$(binDir)/$(app): COM_STRING = "Linking"
$(binDir)/$(app): buildrepo $(objects)
	@mkdir -p `dirname $@`
	@$(call run_and_test, $(CC) $(objects) $(LDFlags) -o $@)

$(objDir)/%.o: COM_STRING = "Compiling"
$(objDir)/%.o: %.$(srcExt)
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(call run_and_test, $(CC) $(CFlags) $< -o $@)

clean:
	$(RM) -r $(objDir)

cleanbin: clean
	$(RM) -r $(binDir)/$(app)

buildrepo:
	@$(call make-repo)

define make-repo
	for dir in $(srcDirs); \
	do \
	mkdir -p $(objDir)/$$dir; \
	done
endef

# usage: $(call make-depend,source-file,object-file,depend-file)
define make-depend
	$(CC) -MM       \
				-MF $3    \
				-MP       \
				-MT $2    \
				$(CFlags) \
				$1
endef