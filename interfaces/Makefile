#******************************************************************************
# PROJECT:  PLC Data Server
# MODULE:   Makefile
# PURPOSE:  Global makefile for the 'interfaces' directory
# AUTHOR:   Paul M. Breen                          
# DATE:     2000-05-24
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = ..
include $(SRCDIR)/Makefile.defs

###############################################################################

# Build the configured interfaces:
all:
	${MAKE} -C libpds
	${MAKE} -C libpdsnp
	${MAKE} -C libpds_spi

ifeq ($(USE_TCL), true)
	${MAKE} -C libpds_tcl
	${MAKE} -C libpds_spi_tcl
endif

ifeq ($(USE_PYTHON), true)
	${MAKE} -C pdspy
endif

# Tidy the directories:
clean: 
	${MAKE} -C libpds clean
	${MAKE} -C libpdsnp clean
	${MAKE} -C libpds_spi clean

ifeq ($(USE_TCL), true)
	${MAKE} -C libpds_tcl clean
	${MAKE} -C libpds_spi_tcl clean
endif

ifeq ($(USE_PYTHON), true)
	${MAKE} -C pdspy clean
endif

# Install software:
install: 
	${MAKE} -C libpds install
	${MAKE} -C libpdsnp install
	${MAKE} -C libpds_spi install

ifeq ($(USE_TCL), true)
	${MAKE} -C libpds_tcl install
	${MAKE} -C libpds_spi_tcl install
endif

ifeq ($(USE_PYTHON), true)
	${MAKE} -C pdspy install
endif

