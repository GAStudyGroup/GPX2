app = GPX2

srcExt = cpp
srcDir = src
objDir = obj
binDir = bin
inc = $(shell find -type f -iname "*.hpp" -printf "%h\n" | sort -u)

debug = 1

CFlags = -Wall -std=gnu++17
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

.phony: all clean distclean
	
all: $(binDir)/$(app)

$(binDir)/$(app): buildrepo $(objects)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	$(CC) $(objects) $(LDFlags) -o $@

$(objDir)/%.o: %.$(srcExt)
	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	$(CC) $(CFlags) $< -o $@

clean:
	$(RM) -r $(objDir)

distclean: clean
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