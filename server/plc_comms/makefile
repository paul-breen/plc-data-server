#******************************************************************************
# PROJECT:  PLC data server  
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
#         pds_plc_comms.o
#
# Change History:
#
#  1998-04-22         Initial Issue
#
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = ../..
include $(SRCDIR)/Makefile.defs

############################## CONFIGURE BLOCK ############################### 

# Include paths for headers:

# List of targets to build:
OBJS = pds_plc_comms.o

# Set the cc compile flags:

# Dependencies:
DEPS = pds_plc_comms.h

########################### END OF CONFIGURE BLOCK ############################

# default target (all) - build everything:
all: $(OBJS)

# Tidy directory:
clean: 
	rm -f $(OBJS) 

# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# header file dependencies:
$(OBJS): $(DEPS)

