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
#         pdsd
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

# Libraries for link:
LIBS += $(PDS_BUILD_LIBSUPPORT_A) $(PDS_BUILD_LIBPDS_SPI_A) $(LEXLIB)

# Include paths for headers:

# Sub-directory paths:
CONF_DIR = plc_config_scanner
COMMS_DIR = plc_comms
DRV_DIR = drivers

# List of targets to build:
TARGET = pdsd
TARGOBJ = pds_main.o pds_io.o pds_mem.o pds_conn.o
CONFOBJ = $(CONF_DIR)/pds_plc_cnf.o $(CONF_DIR)/pds_plc_cnf_scan.o
COMMSOBJ = $(COMMS_DIR)/pds_plc_comms.o
DRVOBJ = $(DRV_DIR)/pds_mb.o $(DRV_DIR)/pds_mb_err.o $(DRV_DIR)/pds_dh.o $(DRV_DIR)/pds_dh_err.o $(DRV_DIR)/pds_cip.o $(DRV_DIR)/pds_cip_err.o

# Set the compile flags:
# CFLAGS = -D_SVID_SOURCE -g -ansi -m486           # For debugging

# Set the link flags:

# Path to the install directory:
INST_DIR = $(PDS_BIN_DIR)

# Dependencies:
DEPS = pds_srv.h \
$(CONF_DIR)/pds_plc_cnf.h $(COMMS_DIR)/pds_plc_comms.h

########################### END OF CONFIGURE BLOCK ############################

# default target (all) - build everything:
all: $(TARGET)

# Tidy directory:
clean: 
	rm -f $(TARGET) $(TARGOBJ) $(CONFOBJ) $(COMMSOBJ) $(DRVOBJ)

# Install software:
install: 
	mkdir -m755 -p $(INST_DIR) > /dev/null 2>&1
	cp $(TARGET) $(INST_DIR)

# Link instructions:
$(TARGET): $(TARGOBJ)
	${MAKE} -C $(CONF_DIR)
	${MAKE} -C $(COMMS_DIR)
	${MAKE} -C $(DRV_DIR)
	$(CC) $(LDFLAGS) -o $(TARGET) $(TARGOBJ) $(CONFOBJ) $(COMMSOBJ) \
	$(DRVOBJ) $(LIBS)

# Strip instructions (comment out if debugging):
strip:
	$(STRIP) $(TARGET)
	
# Compile rule (same for all .c files):
.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $<

# Header file dependencies:
$(TARGOBJ): $(DEPS)
$(CONFOBJ): $(DEPS)
$(COMMSOBJ): $(DEPS)
$(DRVOBJ): $(DEPS)

