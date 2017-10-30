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
#         chkcnf.tcl
#
# Change History:
#
#  1998-04-22         Initial Issue
#
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = ..
include $(SRCDIR)/Makefile.defs

############################### CONFIGURE BLOCK ############################### 

# List of targets to build:
TARGET = chkcnf.tcl

# Path to the install directory:
INST_DIR = $(PDS_BIN_DIR)

########################### END OF CONFIGURE BLOCK ############################

# Install software:
install: 
	chmod 755 $(TARGET)
	mkdir -m755 -p $(INST_DIR) > /dev/null 2>&1
	cp $(TARGET) $(INST_DIR)

