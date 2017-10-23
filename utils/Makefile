#******************************************************************************
# PROJECT:  PLC Data Server
# MODULE:   Makefile
# PURPOSE:  Global makefile for the 'utils' directory
# AUTHOR:   Paul M. Breen
# DATE:     2000-05-15
#******************************************************************************

# Set the src directory path & pull in the global definitions makefile:
SRCDIR = ..
include $(SRCDIR)/Makefile.defs

###############################################################################

# Build the utility programs:
all:
	${MAKE} -C addrmm
	${MAKE} -C cnf-read
	${MAKE} -C force_tag_status

ifeq ($(USE_CURSES), true)
	${MAKE} -C mcrd
endif

	${MAKE} -C nwtio
	${MAKE} -C pds_ctl
	${MAKE} -C plcmm
	${MAKE} -C spimm
	${MAKE} -C tem
	${MAKE} -C tio

# Strip the programs:
strip:
	${MAKE} -C addrmm strip
	${MAKE} -C cnf-read strip
	${MAKE} -C force_tag_status strip

ifeq ($(USE_CURSES), true)
	${MAKE} -C mcrd strip
endif

	${MAKE} -C nwtio strip
	${MAKE} -C pds_ctl strip
	${MAKE} -C plcmm strip
	${MAKE} -C spimm strip
	${MAKE} -C tem strip
	${MAKE} -C tio strip

# Install software:
install:
	${MAKE} -C addrmm install
	${MAKE} -C cnf-read install
	${MAKE} -C force_tag_status install

ifeq ($(USE_CURSES), true)
	${MAKE} -C mcrd install
endif

	${MAKE} -C nwtio install
	${MAKE} -C pds_ctl install
	${MAKE} -C plcmm install
	${MAKE} -C spimm install
	${MAKE} -C tem install
	${MAKE} -C tio install

# Tidy the directories:
clean:
	${MAKE} -C addrmm clean
	${MAKE} -C cnf-read clean
	${MAKE} -C force_tag_status clean

ifeq ($(USE_CURSES), true)
	${MAKE} -C mcrd clean
endif

	${MAKE} -C nwtio clean
	${MAKE} -C pds_ctl clean
	${MAKE} -C plcmm clean
	${MAKE} -C spimm clean
	${MAKE} -C tem clean
	${MAKE} -C tio clean

