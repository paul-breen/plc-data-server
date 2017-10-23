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
#         pds_plc_cnf_scan.o pds_plc_cnf.o 
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

# Include paths for headers:
INCS += -I.

# List of targets to build:
OBJS = pds_plc_cnf_scan.o pds_plc_cnf.o 

# List of lex output files:
LEXOUT = pds_plc_cnf_scan.c 

# Set the cc compile flags:

# Set the lex compile flags:
# LEXFLAGS = -d -Ppds_plc_cnf       # For debugging
LEXFLAGS = -Ppds_plc_cnf          # For release

# Dependencies:
DEPS = pds_plc_cnf.h

########################### END OF CONFIGURE BLOCK ############################

# default target (all) - build everything:
all: $(OBJS)

# Tidy directory:
clean: 
	rm -f $(OBJS) $(LEXOUT)

# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# Compile rule (same for all .l files)
# N.B.:  The generated .c file is deleted!!:
.l.c:
	$(LEX) $(LEXFLAGS) -o$*.c $<

# header file dependencies:
$(OBJS): $(DEPS)

