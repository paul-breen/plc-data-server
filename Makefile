#******************************************************************************
# PROJECT:  PLC Data Server
# MODULE:   Makefile
# PURPOSE:  Global makefile for the PDS
# AUTHOR:   Paul M. Breen
# DATE:     2000-05-24
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = .
include $(SRCDIR)/Makefile.defs

###############################################################################

# Build the libraries, the PDS & utilities etc.:
all:
	${MAKE} libs
	${MAKE} apis
	${MAKE} pds
	${MAKE} pds_nwstub
	${MAKE} utilities

###############################################################################

# Tidy the directories:
clean: 
	${MAKE} -C libsupport clean
	${MAKE} -C interfaces clean
	${MAKE} -C server clean
	${MAKE} -C nwstub clean
	${MAKE} -C utils clean

# Tidy the configuration output:
configclean:
	rm -f config.cache config.status config.log .tcldefs.local Makefile.defs Makefile.tcldefs include/pds_config.h control_scripts/pds

# Tidy the whole src tree:
distclean: clean configclean

# Install software:
install: 
	${MAKE} create_install_dir

	${MAKE} -C libsupport install
	${MAKE} -C interfaces install
	${MAKE} -C server strip
	${MAKE} -C server install
	${MAKE} -C nwstub strip
	${MAKE} -C nwstub install
	${MAKE} -C utils strip
	${MAKE} -C utils install
	${MAKE} -C libtcl install
	${MAKE} -C control_scripts install
	${MAKE} -C example_plc_config install
	${MAKE} -C tcl_utils install
	${MAKE} -C python_utils install
	${MAKE} -C doc install

	${MAKE} print_success_message

###############################################################################

# Build the libraries:
libs:
	${MAKE} -C libsupport

# Build the interfaces (APIs):
apis:
	${MAKE} -C interfaces

# Build the PLC Data Server (PDS):
pds:
	${MAKE} -C server

# Build the PDS network stub:
pds_nwstub:
	${MAKE} -C nwstub

# Build the PDS utility programs:
utilities:
	${MAKE} -C utils

###############################################################################

# Create the install directory structure:
create_install_dir:
	mkdir -m755 -p $(PDS_DIR) > /dev/null 2>&1
	mkdir -m755 -p $(PDS_BIN_DIR) > /dev/null 2>&1
	mkdir -m755 -p $(PDS_DATA_DIR) > /dev/null 2>&1
	mkdir -m755 -p $(PDS_DOC_DIR) > /dev/null 2>&1
	mkdir -m755 -p $(PDS_INC_DIR) > /dev/null 2>&1
	mkdir -m755 -p $(PDS_LIB_DIR) > /dev/null 2>&1

# Print a welcome message to inform user that the build went OK:
print_success_message:
	@ echo "==============================================================================="
	@ echo "			PLC Data Server (PDS) Version $(PDS_VERSION)"
	@ echo ""
	@ echo "Built and installed OK."
	@ echo "Your configured install directory is '$(PDS_DIR)'."
	@ echo ""
	@ cat README
	@ echo "==============================================================================="

