#!/usr/bin/tclsh

if {$argc != 1} {
  puts "Usage: $argv0 tagname"
  exit
}

load /usr/local/pds/lib/libpds_spi_tcl.so

set PDS_SPI_IPCKEY 1423780

set tagname [lindex $argv 0]

# Connect to the PDS SPI
set conn [pds_spiconnect $PDS_SPI_IPCKEY]

# Get the current value for the SPI tag
set tagvalue [pds_spiget_tag $conn $tagname]

# Print the SPI tag value
puts "Get tag: name = $tagname, value = $tagvalue"

# Disconnect from the PDS SPI
pds_spidisconnect $conn

