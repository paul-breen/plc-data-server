#******************************************************************************
# PROJECT:  PLC Data Server
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
#         force_tag_status
#
# Change History:
#
#  1998-04-22         Initial Issue
#
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = ../..
include $(SRCDIR)/Makefile.defs

############################### CONFIGURE BLOCK ############################### 

# Libraries for link:
LIBS += $(PDS_BUILD_LIBPDS_A)

# Include paths for headers:

# List of targets to build:
TARGET = force_tag_status
TARGOBJ = force_tag_status.o

# Set the compile flags:

# Set the link flags:

# Path to the install directory:
INST_DIR = $(PDS_BIN_DIR)

# Dependencies:
DEPS = force_tag_status.h

########################### END OF CONFIGURE BLOCK ############################

# default target (all) - build everything:
all: $(TARGET)

# Tidy directory:
clean: 
	rm -f $(TARGET) $(TARGOBJ)

# Install software:
install: 
	mkdir -m755 -p $(INST_DIR) > /dev/null 2>&1
	cp $(TARGET) $(INST_DIR)
	
# Link instructions:
$(TARGET): $(TARGOBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(TARGOBJ) $(LIBS)

# Strip instructions:
strip:
	strip $(TARGET)
	
# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# Header file dependencies:
$(TARGOBJ): $(DEPS)

