#******************************************************************************
# PROJECT:  PLC Data Server library
# MODULE:   makefile
# PURPOSE:  Input to Unix 'make' program - rebuilds C programs 
# AUTHOR:   Paul Breen
# DATE:     1998-04-22
#
# Parameters: none
#
# Build instructions:
#   Go to directory and type 'make' 
#
#   The following targets are built:
#
#         libpdsnp.a libpdsnp.so
#
# Change History:
#
#  1998-04-22          Initial Issue
#
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:

SRCDIR = ../..
include $(SRCDIR)/Makefile.defs

############################### CONFIGURE BLOCK ############################### 

# Path to the install lib directory (static):
LIBA_DIR = $(PDS_LIB_DIR)

# Path to the install lib directory (dynamic):
LIBSO_DIR = $(PDS_LIB_DIR)

# Path to the install include directory:
INC_DIR = $(PDS_INC_DIR)

# Object files needed to build libraries (static and dynamic):
OBJS = pdsnp_api.o pdsnp_comms.o

# N.B.: We include the network comms functions here to make this library
#       more self contained
EXTERN_OBJS = $(PDS_BUILD_LIBSUPPORT_DIR)/nw_comms.o
EXTERN_SRC = $(PDS_BUILD_LIBSUPPORT_DIR)/nw_comms.c

# Header files to install to support libraries (static and dynamic):
INCS_INST = $(PDS_BUILD_INC_DIR)/pdsnp.h $(PDS_BUILD_INC_DIR)/pdsnp_api.h $(PDS_BUILD_INC_DIR)/pdsnp_defs.h $(PDS_BUILD_INC_DIR)/pdsnp_comms.h

# List of library targets to build (static and dynamic):
LIBA = libpdsnp.a
LIBSO = libpdsnp.so

# Include paths for headers:

# Set the cc compile flags:

# Dependencies:
DEPS = $(INCS_INST)

########################### END OF CONFIGURE BLOCK ############################

# Default target (all) - build everything:
all: $(LIBA) $(LIBSO)

# Tidy directory:
clean: 
	rm -f $(LIBA) $(LIBSO) $(OBJS)

# Install software:
install: 
	${MAKE} copy_liba
	${MAKE} copy_libso
	${MAKE} copy_incs

# The static library (copy):
copy_liba:
	mkdir -m755 -p $(LIBA_DIR) > /dev/null 2>&1
	cp $(LIBA) $(LIBA_DIR)

# The dynamic library (copy):
copy_libso:
	mkdir -m755 -p $(LIBSO_DIR) > /dev/null 2>&1
	cp $(LIBSO) $(LIBSO_DIR)

# The header files (copy):
copy_incs:
	mkdir -m755 -p $(INC_DIR) > /dev/null 2>&1
	cp $(INCS_INST) $(INC_DIR)

# Build instructions (static library):
$(LIBA) : $(OBJS) $(EXTERN_OBJS)
	ar -rus $(LIBA) $(OBJS) $(EXTERN_OBJS)

# Build instructions (dynamic library):
$(LIBSO) : $(OBJS) $(EXTERN_OBJS)
	ld -o $(LIBSO) -shared $(OBJS) $(EXTERN_OBJS)

# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# Header file dependencies:
$(OBJS): $(DEPS)

# External file dependencies:
$(EXTERN_OBJS): $(EXTERN_SRC)
	$(CC) -c $(CFLAGS) $(INCS) $< -o $@

