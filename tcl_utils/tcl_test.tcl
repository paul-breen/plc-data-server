#!/usr/bin/tclsh

if {$argc != 1} {
  puts "Usage: $argv0 tagname"
  exit
}

load /usr/local/pds/lib/libpds_tcl.so

set PDS_IPCKEY 1423779

set tagname [lindex $argv 0]

# Connect to the PDS
set conn [pdsconnect $PDS_IPCKEY]

# Get the current value for the tag
set tagvalue [pdsget_tag $conn $tagname]

# Print the tag value
puts "Get tag: name = $tagname, value = $tagvalue"

# Disconnect from the PDS
pdsdisconnect $conn

