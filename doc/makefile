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
#         documentation
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
TARGET = pds.css pds.html pds_c_api.html pds_glossary.html pds_tcl_api.html index.html plcconfig.phtml

# Path to the install directory:
INST_DIR = $(PDS_DOC_DIR)

########################### END OF CONFIGURE BLOCK ############################

# Install software:
install: 
	mkdir -m755 -p $(INST_DIR) > /dev/null 2>&1
	cp $(TARGET) $(INST_DIR)

