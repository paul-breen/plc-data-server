#******************************************************************************
# PROJECT:  PLC Data Server library
# MODULE:   makefile
# PURPOSE:  Input to Unix 'make' program - rebuilds C programs 
# AUTHOR:   Paul M. Breen
# DATE:     2018-06-22
#
# Parameters: none
#
# Build instructions:
#   Go to directory and type 'make' 
#
#   The following targets are built:
#
#         __init__.py
#         pds.py
#         _pds.so
#
# Change History:
#
#  2018-06-22         Initial Issue
#
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = ../..
include $(SRCDIR)/Makefile.defs

############################### CONFIGURE BLOCK ############################### 

# Path to the install dist build directories (dynamic):
MODULE_DIST_DIR_ROOT = build
MODULE_DIST_DIR = $(MODULE_DIST_DIR_ROOT)/pds

# Library files needed to build libraries (dynamic):
LIBS += -lc $(PDS_BUILD_LIBPDS_A)

# Swig source interface file needed to build libraries (dynamic):
SRC = pds.i

# Generated source files needed to build libraries (dynamic):
GEN_SRCS = pds_wrap.c

# Object files needed to build libraries (dynamic):
OBJS = pds_wrap.o

# Header files to install to support libraries (dynamic):

# List of module targets to build:
MODULE = pds.py
LIBSO = _pds.so
MODULE_INIT = __init__.py
MODULE_BUILDER = setup.py

# Include paths for headers:

# Set the cc compile flags:

# Dependencies:
DEPS = $(SRC)

########################### END OF CONFIGURE BLOCK ############################

# default target (all) - build everything:
all: $(MODULE)
	${MAKE} copy_module

# Tidy directory:
clean: 
	rm -f $(MODULE) $(LIBSO) $(GEN_SRCS) $(OBJS)
	rm -rf $(MODULE_DIST_DIR_ROOT)
	rm -rf __pycache__

# Install software:
install: 
	# N.B.: This must be semicolon-separated to run in a single shell
	cd $(MODULE_DIST_DIR_ROOT); pip install --upgrade .

# The module (copy):
copy_module:
	mkdir -m755 -p $(MODULE_DIST_DIR) > /dev/null 2>&1
	cp $(LIBSO) $(MODULE_DIST_DIR)
	cp $(MODULE) $(MODULE_DIST_DIR)
	cp $(MODULE_INIT) $(MODULE_DIST_DIR)
	cp $(MODULE_BUILDER) $(MODULE_DIST_DIR_ROOT)

# Build instructions (module):
$(MODULE) : $(LIBSO)

# Build instructions (dynamic library):
$(LIBSO) : $(OBJS)
	ld -o $(LIBSO) -shared $(OBJS) $(LIBS)

# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# Generate rule:
$(GEN_SRCS) : $(SRC)
	$(SWIG) -python -I$(PDS_BUILD_INC_DIR) $(SRC)

