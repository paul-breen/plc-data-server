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
#         cnf-read
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

# Path to PLC config scanner:
CONF_SCAN_DIR = $(SRCDIR)/server/plc_config_scanner

# Libraries for link:
LIBS += $(PDS_BUILD_LIBSUPPORT_A) $(LEXLIB)

# Include paths for headers:
INCS += -I$(CONF_SCAN_DIR)

# List of targets to build:
TARGET = cnf-read
TARGOBJ = cnf-read.o
SCANOBJ = $(CONF_SCAN_DIR)/pds_plc_cnf.o $(CONF_SCAN_DIR)/pds_plc_cnf_scan.o

# Set the compile flags:

# Set the link flags:

# Path to the install directory:
INST_DIR = $(PDS_BIN_DIR)

# Dependencies:
DEPS = cnf-read.h $(CONF_SCAN_DIR)/pds_plc_cnf.h

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
$(TARGET): $(TARGOBJ) $(SCANOBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(TARGOBJ) $(SCANOBJ) $(LIBS)

# Strip instructions:
strip:
	strip $(TARGET)
	
# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# Rule to compile the server config scanner code:
$(SCANOBJ):
	${MAKE} -C $(CONF_SCAN_DIR)

# Header file dependencies:
$(TARGOBJ): $(DEPS)

