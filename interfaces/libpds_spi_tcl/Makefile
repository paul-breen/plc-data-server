#******************************************************************************
# PROJECT:  PLC Data Server library
# MODULE:   makefile
# PURPOSE:  Input to Unix 'make' program - rebuilds C programs 
# AUTHOR:   Paul M. Breen
# DATE:     1998-04-22
#
# Parameters: none
#
# Build instructions:
#   Go to directory and type 'make' 
#
#   The following targets are built:
#
#         libpds_spi_tcl.so
#
# Change History:
#
#  1998-04-22         Initial Issue
#
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = ../..
include $(SRCDIR)/Makefile.defs
include $(SRCDIR)/Makefile.tcldefs

############################### CONFIGURE BLOCK ############################### 

# Path to the install lib directory (dynamic):
LIBSO_DIR = $(PDS_LIB_DIR)

# Path to the install include directory:
INC_DIR = $(PDS_INC_DIR)

# Library files needed to build libraries (dynamic):
LIBS += $(TCL_STUB_LIB_SPEC) $(TCL_LIBS) -lc $(PDS_BUILD_LIBPDS_SPI_A)

# Object files needed to build libraries (dynamic):
OBJS = pds_spi_tcl_spi.o pds_spi_tcl_id.o

# Header files to install to support libraries (dynamic):
INCS_INST = $(PDS_BUILD_INC_DIR)/pds_spi_tcl_spi.h $(PDS_BUILD_INC_DIR)/pds_spi_tcl_id.h

# List of library targets to build (dynamic):
LIBSO = libpds_spi_tcl.so

# Include paths for headers:

# Set the cc compile flags:
# USE_TCL_STUBS is now obselete
# CFLAGS += $(TCL_SHLIB_CFLAGS) $(TCL_CFLAGS_OPTIMIZE) -DTCL_MEM_DEBUG -fPIC -DUSE_TCL_STUBS
CFLAGS += $(TCL_SHLIB_CFLAGS) $(TCL_CFLAGS_OPTIMIZE) -DTCL_MEM_DEBUG -fPIC

# Dependencies:
DEPS = $(INCS_INST)

########################### END OF CONFIGURE BLOCK ############################

# default target (all) - build everything:
all: $(LIBSO)

# Tidy directory:
clean: 
	rm -f $(LIBSO) $(OBJS)

# Install software:
install: 
	${MAKE} copy_libso
	${MAKE} copy_incs

# The dynamic library (copy):
copy_libso:
	mkdir -m755 -p $(LIBSO_DIR) > /dev/null 2>&1
	cp $(LIBSO) $(LIBSO_DIR)

# The header files (copy):
copy_incs:
	mkdir -m755 -p $(INC_DIR) > /dev/null 2>&1
	cp $(INCS_INST) $(INC_DIR)

# Build instructions (dynamic library):
$(LIBSO) : $(OBJS)
	ld -o $(LIBSO) -shared $(OBJS) $(LIBS)

# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# Header file dependencies:
$(OBJS): $(DEPS)

