## PLC Data Server

### Outline

The PLC Data Server (PDS) makes PLC data available to client programs through a simple API.

The PDS acts as a piece of middleware, simplifying a client's access to a PLC.  It takes care of the fieldbus communication to a PLC and abstracts the fieldbus protocol via a *tag abstraction* - that is, a reference in the PLC is mapped to a tagname and the corresponding data value is mapped to a tagvalue.

### Method

The PDS reads in its configuration file and based on the contents, sets up memory structures to hold data for PLC discrete bits, words etc.  The configuration file can describe multiple PLCs, and each PLC can have multiple blocks.  The communication method for each PLC block is configured as one of a number of supported fieldbus protocols, either over TCP/IP or a serial connection.

The PDS then splits into two distinct processes, one to handle all read requests for PLC data and one to handle all requests to write data to a PLC.  In this way the PDS is a concurrent read/write server.

The read process sets up an appropriate fieldbus query for all read/mapped-write blocks found in the configuration file and sits in a continuous loop, running in turn the queries against the PLC.  It uses the data returned by the PLC to populate a shared memory segment.

The write process sits polling a message queue and when it receives a request from a client program to write data to the PLC, it constructs the appropriate fieldbus query and sends it to the PLC.

All communication between the PDS and a PLC is via an appropriate fieldbus protocol, either over TCP/IP or a serial connection.  All communication between a client program and the PDS is via UNIX Inter-Process Communication (IPC).  A client reads PLC data by accessing the PDS's shared memory segment (controlled by semaphores).  A client writes data to the PLC by sending a message on a message queue to the PDS.  The client/PDS communication is encapsulated in the PDS API.

Optionally, an installation can also enable the PDS network stub.  This provides clients with a TCP/IP interface to the PDS, using a simple PDS Network Protocol (PDSNP) encapsulated in the PDSNP API.

### Installation Instructions

To configure, build, and install the PLC Data Server (PDS), type:

```
./configure
make
make install
```

This will build the core C interface libraries, the PDS, and a set of utility clients.  They will be installed in a tree under `/usr/local/pds`.

To install to a different installation directory, for example `~/local/pds`, type:

```
./configure --prefix=~/local/pds
make
make install
```

See `./configure --help` for available options.

To also build the Tcl/Tk interface libraries and utilities, type:

```
./configure --with-tcl --with-tclconfig=/usr/lib
make
make install
```

ensuring the correct path for your `tclConfig.sh` script.

In addition, the PDS can be network enabled by building the PDS network stub:

```
./configure --with-nwstub
make
make install
```

**Caution**: The network stub provides an unauthenticated read/write interface to the PDS.  By default, the PDS network stub will listen on `localhost`, port `9574`, so access to the PDS host is still required by clients.  However, in this case, you may as well just use the PDS without the network stub.

To configure the PDS network stub to listen on all network interfaces, and on a different port (`9999`, say), type:

```
./configure --with-nwstub --with-nwstub-host=0.0.0.0 --with-nwstub-port=9999
make
make install
```

Enabling the PDS network stub with the `--with-nwstub` option, will configure the PDS control script (`pds`) to start/stop the PDS network stub as well as the PDS.

### Run the PDS

To run the PDS, `cd` to the `bin` directory (under install directory) and run the PDS control script.

To start the PDS type `pds start`, to stop the PDS type `pds stop`.  An example PLC configuration file has been provided as a template for your own configuration and is located in the `data` directory.

By default, the PLC configuration file is called `plc.cnf`.  It is a plain-text formatted configuration file.

#### Example PLC Configuration File

```
# Project: PDS Demo
# Purpose: PLC Communications Configuration File
# Author:  Paul M. Breen
# Date:    2000-09-27
#
# Format:  /protocol/function/path/IP_address:port/base_address/poll_rate
# OR       /protocol/function/path/"tty_device"/base_address/poll_rate
#          <white space>tagname tagreference wordlength

###############################################################################
# PLC 1
###############################################################################

# Communicate via ModBus/TCP/IP to the PLC identified as IP address 10.4.8.190,
# TCP port 502, Unit ID 0.  Read 2 bits (coils) starting at reference 000101.
/MB_TCPIP/BREAD/0/10.4.8.190:502/000000/1000
	pds_mb_read_bit1		000101	0
	pds_mb_read_bit2		000102	0

# Communicate via ModBus/TCP/IP to the PLC identified as IP address 10.4.8.190,
# TCP port 502, Unit ID 0.  Write upto 2 bits (coils) starting at reference
# 000201.
/MB_TCPIP/BWRITE/0/10.4.8.190:502/000000/1000
	pds_mb_write_bit1		000201	0
	pds_mb_write_bit2		000202	0

# Communicate via ModBus/TCP/IP to the PLC identified as IP address 10.4.8.190,
# TCP port 502, Unit ID 0.  Read 2 16 bit words (registers) starting at
# reference 400101.
/MB_TCPIP/WREAD/0/10.4.8.190:502/400000/1000
	pds_mb_read_word1		400101	16
	pds_mb_read_word2		400102	16

# Communicate via ModBus/TCP/IP to the PLC identified as IP address 10.4.8.190,
# TCP port 502, Unit ID 0.  Write upto 2 16 bit words (registers) starting at
# reference 400201.
/MB_TCPIP/WWRITE/0/10.4.8.190:502/400000/1000
	pds_mb_write_word1		400201	16
	pds_mb_write_word2		400202	16

###############################################################################
# PLC 2
###############################################################################

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Read 1
# bit identified as logical_1.
/CIP_TCPIP/BREAD/1.0/10.4.8.191:44818/logical_1/1000
	pds_cip_read_bit0		0	0

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Read 1
# bit identified as logical_2.
/CIP_TCPIP/BREAD/1.0/10.4.8.191:44818/logical_2/1000
	pds_cip_read_bit1		0	0

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Read 4
# 16 bit words from the Input_Data_Int[] array
/CIP_TCPIP/WREAD/1.0/10.4.8.191:44818/Input_Data_Int[]/1000
	pds_cip_read_word0		0	16
	pds_cip_read_word1		1	16
	pds_cip_read_word5		5	16
	pds_cip_read_word7		7	16

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Write upto
# 4 16 bit words to the Input_Data_Int[] array
/CIP_TCPIP/WWRITE/1.0/10.4.8.191:44818/Input_Data_Int[]/1000
	pds_cip_write_word0		0	16
	pds_cip_write_word1		1	16
	pds_cip_write_word2		2	16
	pds_cip_write_word3		3	16

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Read from
# the 16 bit word identified as CIP tag parts
/CIP_TCPIP/WREAD/1.0/10.4.8.191:44818/parts/1000
	pds_cip_read_parts		0	16

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Write to
# the 16 bit word identified as CIP tag parts
/CIP_TCPIP/WWRITE/1.0/10.4.8.191:44818/parts/1000
	pds_cip_write_parts		0	16

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Read 3
# 32 bit words (hi-word, lo-word) from the input_dint[] array
/CIP_TCPIP/LREAD/1.0/10.4.8.191:44818/input_dint[]/1000
	pds_cip_read_hi_dint0		0	16
	pds_cip_read_lo_dint0		0	16
	pds_cip_read_hi_dint1		1	16
	pds_cip_read_lo_dint1		1	16
	pds_cip_read_hi_dint2		2	16
	pds_cip_read_lo_dint2		2	16

# Communicate via ControlNet/TCP/IP to the PLC identified as IP address
# 10.4.8.191, TCP port 44818, Routing Path 1.0 (backplane, slot 0).  Write upto
# 3 32 bit words (hi-word, lo-word) to the input_dint[] array
/CIP_TCPIP/LWRITE/1.0/10.4.8.191:44818/input_dint[]/1000
	pds_cip_write_hi_dint0		0	16
	pds_cip_write_lo_dint0		0	16
	pds_cip_write_hi_dint1		1	16
	pds_cip_write_lo_dint1		1	16
	pds_cip_write_hi_dint2		2	16
	pds_cip_write_lo_dint2		2	16

###############################################################################
# PLC 3
###############################################################################

# Communicate via DataHighway (DF1)/serial/TCP/IP to the PLC identified as
# IP address 10.4.8.192, TCP port 502, station ID 6.  Read 6 16 bit words
# starting at the $N10:0 file
/DH_SERIAL_TCPIP/WREAD/6/10.4.8.192:502/$N10:0/1000
	pds_dh_read_word0		0	16
	pds_dh_read_word1		1	16
	pds_dh_read_word2		2	16
	pds_dh_read_word3		3	16
	pds_dh_read_word5		5	16
	pds_dh_read_word17		17	16
```

