#!/usr/bin/wish -f

###############################################################################
# Project:  PLC data server
# Program:  chkcnf.tcl
# Purpose:  To validate a plc.cnf file, and go online to the configured PLCs
# Author:   Paul M. Breen
# Date:     2000-03-24
###############################################################################

###############################################################################
# Function to close application gracefully
###############################################################################
proc CloseApp {} {
global pdsconn

   if {[string length $pdsconn] > 0} { 
      if [catch {pdsdisconnect $pdsconn} reply] {
         tk_messageBox -type ok -message {Failed to disconnect from PLC data server !} -icon error -title {Error}
      }
   }
   exit
}



###############################################################################
# Function to connect to PLC                                       
###############################################################################
proc ConnectPLC {} {
global IPCKEY pdsconn .mainmenu.plcmenu

   if {[string length $pdsconn] == 0 } {
      if [catch {pdsconnect $IPCKEY} pdsconn] {
         tk_messageBox -type ok -message {Failed to connect!} -icon error -title {Error}
         set pdsconn {}
      } else {
         .mainmenu.plcmenu entryconfigure 0 -state disabled
         .mainmenu.plcmenu entryconfigure 1 -state normal
         .mainmenu.plcmenu entryconfigure 3 -state normal
         bind . <Control-C> {}
         bind . <Control-c> {}
         bind . <Control-D> {DisconnectPLC}
         bind . <Control-d> {DisconnectPLC}
      }
   } else {
      tk_messageBox -type ok -message {Already connected!} -icon error -title {Error}
   }
}



###############################################################################
# Function to disconnect from PLC                                       
###############################################################################
proc DisconnectPLC {} {
global pdsconn 

   if {[string length $pdsconn] > 0} { 
      if [catch {pdsdisconnect $pdsconn} reply] {
         tk_messageBox -type ok -message {Failed to disconnect!} -icon error -title {Error}
      } else {
         set pdsconn {}
         .mainmenu.plcmenu entryconfigure 0 -state normal
         .mainmenu.plcmenu entryconfigure 1 -state disabled
         .mainmenu.plcmenu entryconfigure 3 -state disabled
         bind . <Control-C> {ConnectPLC}
         bind . <Control-c> {ConnectPLC}
         bind . <Control-D> {}
         bind . <Control-d> {}
      }
   } else {
      tk_messageBox -type ok -message {Not connected!} -icon error -title {Error}
   }
}



###############################################################################
# Function to convert a byte to its printable char representation or '.'
###############################################################################
proc ByteToPrintChar {byte} {
  set char {.}

  if {$byte >= 32 && $byte <= 126} {
    set char [format "%c" $byte]
  }

  return $char
}



###############################################################################
# Function to scan PLC tags listed in selectedarray
###############################################################################
proc ScanPLC {} {
global selectedarray IPCKEY READPLC pdsconn PRINT_CURR_VAL_TOP

   set longest 0

   if { $READPLC == 1 } {
      if {[string length $pdsconn] == 0} {
         tk_messageBox -type ok -message {Not connected!} -icon error -title {Error}
         return
      } else {
         # Calculate the longest tagname
         for {set x 0} {$x < [llength $selectedarray]} {incr x} {
            set len [string length [string trim [lindex $selectedarray $x]]]
            if {$len > $longest} {
               set longest $len
            } 
         }
         for {set x 0} {$x < [llength $selectedarray]} {incr x} {
            set tagname [string trim [lindex $selectedarray $x]]
            set msg [trailpad $tagname $longest]
            append msg { = }
            set result [pdsget_tag $pdsconn $tagname]
            set high [expr $result & 0xff00]
            set high [expr $high >> 8]
            set low [expr $result & 0x00ff]
            set highchar [ByteToPrintChar $high]
            set lowchar [ByteToPrintChar $low]
            append msg [leadpad $result 6]
            append msg {, }
            append msg [leadpad $high 3]
            append msg {, }
            append msg [leadpad $low 3]
            append msg {, }
            append msg $highchar
            append msg $lowchar

            # Optionally print the current value at the TOP of the list
            # OR at the bottom of the list (ensuring it can always be seen)
            if { $PRINT_CURR_VAL_TOP } {
               .onlinebox.ansframe.listbox insert 0 $msg
            } else {
               .onlinebox.ansframe.listbox insert end $msg
               .onlinebox.ansframe.listbox see end
            }
         }
      }
   after 1000 {ScanPLC}
   }
}
 


###############################################################################
# Function to extract the directory from the full filename
###############################################################################
proc dirpath {filename} {
   set x -1

   # Find the last slash char
   for {set x 0} {$x < [string length $filename]} {incr x} {
      set char [string range $filename $x $x]
      if {$char == {/}} {
         set pos $x
      }
   }
   if { $pos == -1} {
      # No slash found
      return {/}
   } else {
      return [string range $filename 0 $pos]
   }   
}



###############################################################################
# Function to clear the production list
###############################################################################
proc ClearList {boxname} {
   set len [expr [$boxname size] - 1]
   for {set i $len} {$i >= 0} {incr i -1} {
      $boxname delete $i
   }
}



###############################################################################
# Function to set the status of the given frame
###############################################################################
proc IndicateStatusFrame {framename status} {
  if {$status == 0} {
    $framename configure -bg {Green}
  } else {
    $framename configure -bg {Red}
  }
}



###############################################################################
# Function to set the status of a selected file's tags
###############################################################################
proc IndicateTagsStatus {status} {
  IndicateStatusFrame .tagframe.statusframe $status
}



###############################################################################
# Function to set the status of a selected file's blocks
###############################################################################
proc IndicateBlocksStatus {status} {
  IndicateStatusFrame .blockframe.statusframe $status
}



###############################################################################
# Function to validate a selected file
###############################################################################
proc ValidateFile {fd} {
  global blockerr selectedarray

  set blockerr 0
  set selectedarray {}

  plcs $fd
  headers $fd
  blocksize $fd
  chkorder $fd 
  chkqty $fd
  chktags $fd

  IndicateBlocksStatus $blockerr
}



###############################################################################
# Function to enable functionality for a validated file
###############################################################################
proc EnableValidFileFunctionality {filename} {
  .mainmenu.filemenu entryconfigure 1 -state normal
  .mainmenu.editmenu entryconfigure 0 -state normal
  .mainmenu.plcmenu entryconfigure 0 -state normal
  bind . <Control-R> {ReloadFile $filename}
  bind . <Control-r> {ReloadFile $filename}
  bind . <Control-C> {ConnectPLC}
  bind . <Control-c> {ConnectPLC}
  bind . <Control-F> {SearchBox}
  bind . <Control-f> {SearchBox}
}



###############################################################################
# Function to select a file to work on
###############################################################################
proc ReloadFile {filename} {
   global blockerr selectedarray

   set blockerr 0
   set selectedarray {}

   if { [string length $filename] != 0 } {
      set fd [fileopen $filename r]

      if { [string compare $fd {error}] != 0 } {
         ValidateFile $fd
         close $fd
      }
   }
}



###############################################################################
# Function to select a file to work on
###############################################################################
proc ChooseFile {} {
   global filename blockerr selectedarray dir

   set suffix ".cnf"
   set init_filename "plc${suffix}"
   set types [list [list "PDS Configurations" $suffix] {"All Files" *}]
   set title "Open Cnf File"
   set oldfilename $filename
   set filename [tk_getOpenFile -initialdir $dir -initialfile $init_filename -filetypes $types -title $title]

   if { [string length $filename] == 0 } {
      # No file chosen so reset to last one
      set filename $oldfilename 
   } else {
      set blockerr 0
      set selectedarray {}
      set dir [dirpath $filename]
      set fd [fileopen $filename r]

      if { [string compare $fd {error}] != 0 } {
         ValidateFile $fd
         close $fd
         EnableValidFileFunctionality $filename
      }
   }
}



###############################################################################
# Procedure to open a file and return the handle or an error
###############################################################################
proc fileopen {file mode} {
   if { [file isfile $file] == 1 } {
     if [catch {open $file $mode} fhandle] {
        tk_messageBox -type ok -message {Failed to open file!} -icon error -title {Error}
        return error
     } else { 
        return $fhandle
     }
   } else {
     # No such file
     puts stderr {ERROR: No such file.}
     return error
   }
}



###############################################################################
# Procedure to get the conf file function from a header line
###############################################################################
proc cnf_function {line} {
  set start [string first {/} $line 1]
  set end [string first {/} $line [expr $start + 1]]
  set len [string length $line]
  if { $start == -1 } {
     # Not found
     return {Error}
  } else {
     # Found so strip the function code off
     set line [string range $line [expr $start + 1] [expr $end - 1]]
     # Now pad out to 9 chars
     set tmp {        }
     append line $tmp
     set line [string range $line 0 8] 
     return $line
  }
}



###############################################################################
# Procedure to strip the ip address / port from a header line
###############################################################################
proc ipaddr {line} {

   # Found a header line so strip out the IP address and port
   set num 0
   set xit 0
   set startpos 0
   set endpos 0
   set pcnt 0
   set plcfield {}

   while { ($num < [string length $line]) && ($xit == 0) } {
      set char [string index $line $num]
      if { [string compare $char {/}] == 0 } {
         # Found a slash
         incr pcnt
      }
      if { ($pcnt == 4) && ($startpos == 0) } {
         set startpos [expr $num + 1]
      }
      if { $pcnt == 5 } {
         set endpos [expr $num - 1]
         set plcfield [ string range $line $startpos $endpos]
         set xit 1
      }
   incr num
   }
   return $plcfield
}



###############################################################################
# Procedure to add trailing white space to a string
###############################################################################
proc trailpad {text len} {
   set tail {}
   for {set x 0} {$x < $len} {incr x} {
      append tail { }
   }
   append text $tail
   set text [string range $text 0 [expr $len - 1]] 
   return $text 
}



###############################################################################
# Procedure to add leadsing white space to a string
###############################################################################
proc leadpad {text len} {
   set tail {}
   for {set x 0} {$x < $len} {incr x} {
      append tail { }
   }
   append tail $text
   set full [string length $tail]
   set text [string range $tail [expr $full - $len] $full] 
   return $text 
}



###############################################################################
# Procedure to check the tag names in the configuration file
###############################################################################
proc chktags {handle} {
global tab tagarray fulltags

   # Clear the current display
   ClearList .tagframe.listbox
 
   # Create an empty array for the results
   set tagarray {}
   set fulltags {}

   # Create counters
   set cnt 0
   set longest 0

   # Find the longest tagname
   if { [string compare $handle {error}] != 0 } {
      seek $handle 0
      while {[eof $handle] == 0} {
        gets $handle line
        set line [string trim $line]
        set line [string trim $line $tab]
        if { ([string range $line 0 0] == {/}) || ([eof $handle] != 0) } {
           # Found a header line so strip out the IP address and port
        } else {
           # Just an ordinary line but could be blank or a comment
           if { ([string range $line 0 0] == {#})||([string length $line] == 0)} {
              # Do nothing!
           } else {
              # A valid line we assume
              set endpos [string wordend $line 0] 
              set tagname [string range $line 0 [expr $endpos - 1]]
              set tagname [string trim $tagname]
              set tagname [string trim $tagname $tab]
              set len [string length $tagname]
              if { $len > $longest } {
                 set longest $len
              }
           } 
        }
      } 
   }

   # Now create the arrays

   if { [string compare $handle {error}] != 0 } {
      seek $handle 0
      while {[eof $handle] == 0} {
        gets $handle line
        set line [string trim $line]
        set line [string trim $line $tab]
        if { ([string range $line 0 0] == {/}) || ([eof $handle] != 0) } {
           # Found a header line so strip out the IP address and port
           set plcfield [ipaddr $line]
        } else {
           # Just an ordinary line but could be blank or a comment
           if { ([string range $line 0 0] == {#})||([string length $line] == 0)} {
              # Do nothing!
           } else {
              # A valid line we assume
              set endpos [string wordend $line 0] 
              set tagname [string range $line 0 [expr $endpos - 1]]
              set tagname [string trim $tagname]
              set tagname [string trim $tagname $tab]
              lappend tagarray $tagname
              set tagname [trailpad $tagname $longest]
              set address [addressline $line]
              set address [trailpad $address 6]
              lappend fulltags [list $tagname $address $plcfield]
           } 
        }
      } 
      set ordered [lsort $tagarray]
      set tagarray $ordered
      unset ordered  
      # Now print tagarray
      for {set x 0} {$x < [llength $tagarray]} {incr x} {
         set mess [lindex $tagarray $x]
         .tagframe.listbox insert end $mess 
      }
      set cnt 0
      for {set x 1} {$x < [llength $tagarray]} {incr x} {
         set first [lindex $tagarray [expr $x - 1]]
         set second [lindex $tagarray $x]
         if {[string compare $first $second] == 0} {
            # Both the same tagname  
            incr cnt
         } 
      }
      set msg {================================================}
      .tagframe.listbox insert 0 $msg 
      if { $cnt == 0 } {
         .tagframe.listbox insert 1 {OK - All tags unique}
      } else {
         set msg {ERROR! }
         append msg $cnt
         append msg { pair(s) of duplicated tags}
         .tagframe.listbox insert 1 $msg
      }
      set msg {================================================}
      .tagframe.listbox insert 2 $msg
      IndicateTagsStatus $cnt
   }
}



###############################################################################
# Procedure to check the numeric quantity of tags in each block 
# in the configuration file
###############################################################################
proc chkqty {handle} {
global tab BLOCKLIMIT blockerr

   # Create an empty array for the results
   set qtyarray {}

   # Create counters
   set cnt 0
   set status {Quantity OK}

   if { [string compare $handle {error}] != 0 } {
      seek $handle 0
      set block 0
      while {[eof $handle] == 0} {
        gets $handle line
        set line [string trim $line]
        set line [string trim $line $tab]
        if { ([string range $line 0 0] == {/}) || ([eof $handle] != 0) } {
           # Found a header line
           if { $block != 0 } {
              # Create an entry in the list
              if {$cnt > $BLOCKLIMIT} {
                 set status {ERROR! Too many tags within data block}
                 set blockerr 1
              }
              lappend qtyarray [list $block $type $cnt $status] 
              # Now remember cnf_function 
              set type [cnf_function $line]
              # Now reset counters
              set cnt 0
              set status {Quantity OK}
           } else {
              # Remember block code
              set type [cnf_function $line]
           }
           incr block
        } else {
           # Just an ordinary line but could be blank or a comment
           if { ([string range $line 0 0] == {#})||([string length $line] == 0)} {
              # Do nothing!
           } else {
              # A valid line we assume
              set address [addressline $line]
              incr cnt
           } 
        }
      } 
      set msg {==============================================================================}
      .blockframe.listbox insert end $msg 
      set msg {Checking address tag quantities within blocks}
      .blockframe.listbox insert end $msg 
      set msg {==============================================================================}
      .blockframe.listbox insert end $msg 
      # Now print qtyarray
      for {set x 0} {$x < [llength $qtyarray]} {incr x} {
         set record [lindex $qtyarray $x]
         # Extract the separate entries from the list
         set blocknum [lindex $record 0]     
         set type [lindex $record 1]     
         set num [lindex $record 2]     
         set status [lindex $record 3]     
         set mess [trailpad $type 9]
         append mess {: block=}
         append mess [leadpad $blocknum 2]
         append mess {, tags=}
         append mess [leadpad $num 3]
         append mess {, }
         append mess $status 
         .blockframe.listbox insert end $mess 
      }
   } 
}



###############################################################################
# Procedure to check the numeric order of each block in the 
# configuration file
###############################################################################
proc chkorder {handle} {
global tab blockerr

   # Create an empty array for the results
   set orderarray {}

   # Create counters
   set this 0
   set last 0
   set status {Ordering OK}

   if { [string compare $handle {error}] != 0 } {
      seek $handle 0
      set block 0
      while {[eof $handle] == 0} {
        gets $handle line
        set line [string trim $line]
        set line [string trim $line $tab]
        if { ([string range $line 0 0] == {/}) || ([eof $handle] != 0) } {
           # Found a header line
           if { $block != 0 } {
              # Create an entry in the list
              lappend orderarray [list $block $type $status] 
              # Now remember cnf_function 
              set type [cnf_function $line]
              # Now reset counters
              set this 0
              set last 0 
              set status {Ordering OK}
           } else {
              # Remember block code
              set type [cnf_function $line]
           }
           incr block
        } else {
           # Just an ordinary line but could be blank or a comment
           if { ([string range $line 0 0] == {#})||([string length $line] == 0)} {
              # Do nothing!
           } else {
              # A valid line we assume
              set address [addressline $line]
              set last $this
              set this $address
              if { $address < $last } {
                 set status {ERROR! Addresses out of order}
                 set blockerr 1 
              } 
           } 
        }
      } 
      set msg {==============================================================================}
      .blockframe.listbox insert end $msg 
      set msg {Checking address ordering within blocks}
      .blockframe.listbox insert end $msg 
      set msg {==============================================================================}
      .blockframe.listbox insert end $msg 
      # Now print orderarray
      for {set x 0} {$x < [llength $orderarray]} {incr x} {
         set record [lindex $orderarray $x]
         # Extract the separate entries from the list
         set blocknum [lindex $record 0]     
         set type [lindex $record 1]     
         set status [lindex $record 2]     
         set mess [trailpad $type 9]
         append mess {: block=}
         append mess [leadpad $blocknum 2]
         append mess {, }
         append mess $status 
         .blockframe.listbox insert end $mess 
      }
   }
}



###############################################################################
# Procedure to count the data blocks in the configuration file
# by type
###############################################################################
proc countheaders {handle type} {
global tab 

  set counter 0
  set len [string length $type]
  while { [eof $handle] == 0 } {
    gets $handle line

    # Ignore any comment lines
    if { [string equal -length 1 {#} $line] } {
      continue
    }
    set line [string trim $line]
    set line [string trim $line $tab]
    set start [string first $type $line]
    set end [expr ($start + $len) - 1]
    if { [string range $line $start $end] == $type } {
      incr counter
    }
  }
  return $counter
}



###############################################################################
# Procedure to count the data blocks in the configuration file
###############################################################################
proc headers {handle} {
global blockerr

   ClearList .blockframe.listbox

   set cnt 0

   if { [string compare $handle {error}] != 0 } {
      set msg {==============================================================================}
      .blockframe.listbox insert end $msg 
      set msg {Checking block types}
      .blockframe.listbox insert end $msg 
      set msg {==============================================================================}
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /BREAD]
      set cnt [expr $cnt + $blocks]
      set msg {Bit read blocks               = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /BWRITE]
      set cnt [expr $cnt + $blocks]
      set msg {Bit write blocks              = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /CREAD]
      set cnt [expr $cnt + $blocks]
      set msg {Byte read blocks              = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /CWRITE]
      set cnt [expr $cnt + $blocks]
      set msg {Byte write blocks             = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /WREAD]
      set cnt [expr $cnt + $blocks]
      set msg {16 bit integer read blocks    = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /WWRITE]
      set cnt [expr $cnt + $blocks]
      set msg {16 bit integer write blocks   = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /LREAD]
      set cnt [expr $cnt + $blocks]
      set msg {32 bit integer read blocks    = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /LWRITE]
      set cnt [expr $cnt + $blocks]
      set msg {32 bit integer write blocks   = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /FREAD]
      set cnt [expr $cnt + $blocks]
      set msg {32 bit float read blocks      = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /FWRITE]
      set cnt [expr $cnt + $blocks]
      set msg {32 bit float write blocks     = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /DREAD]
      set cnt [expr $cnt + $blocks]
      set msg {64 bit float read blocks      = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /DWRITE]
      set cnt [expr $cnt + $blocks]
      set msg {64 bit float write blocks     = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 

      seek $handle 0
      set blocks [countheaders $handle /]
      set msg {Total number of datablocks    = }
      append msg [leadpad $blocks 3]
      .blockframe.listbox insert end $msg 
      if {$blocks != $cnt} {
         set msg {ERROR! Number of datablocks mismatch}
         set blockerr 1
         .blockframe.listbox insert end $msg 
         set msg {Sum = }
         append msg [leadpad $cnt 3]
         append msg {, Read = }
         append msg [leadpad $blocks 3]
         .blockframe.listbox insert end $msg 
      }           
      set msg {==============================================================================}
      .blockframe.listbox insert end $msg 
   }
   return $blocks
}



###############################################################################
# Procedure to format an address into octets
###############################################################################
proc plcformat {plcfield} {

  # Format numbers to be 3 digits per octect
  set lead {000}
  set dot1 0     
  set dot2 0     
  set dot3 0     
  set colon1 0     
  set origlen [string length $plcfield]
  set len $origlen
  set dotnum 0
  set x 0

  while { ($x < $len) && ($colon1 == 0) } {
     set char [string index $plcfield $x]
     if { [string compare $char {.}] == 0 } {
        incr dotnum
        switch -exact -- $dotnum {
           1 {set dot1 $x} 
           2 {set dot2 $x} 
           3 {set dot3 $x} 
        }
     }
     if { [string compare $char {:}] == 0 } {
        set colon1 $x
     }
     incr x
  }   
  # Now have all the breakpoints
  set oct1 [string range $plcfield 0 [expr $dot1 - 1]]
  set tmp $lead
  append tmp $oct1
  set len [string length $tmp]
  set oct1 [string range $tmp [expr $len - 3] $len]
  set oct2 [string range $plcfield [expr $dot1 + 1] [expr $dot2 - 1]]
  set tmp $lead
  append tmp $oct2
  set len [string length $tmp]
  set oct2 [string range $tmp [expr $len - 3] $len]
  set oct3 [string range $plcfield [expr $dot2 + 1] [expr $dot3 - 1]]
  set tmp $lead
  append tmp $oct3
  set len [string length $tmp]
  set oct3 [string range $tmp [expr $len - 3] $len]
  set oct4 [string range $plcfield [expr $dot3 + 1] [expr $colon1 - 1]]
  set tmp $lead
  append tmp $oct4
  set len [string length $tmp]
  set oct4 [string range $tmp [expr $len - 3] $len]
  set port [string range $plcfield [expr $colon1 + 1] [expr $origlen - 1]]
  # Now create the proper ip address
  set reply $oct1
  append reply {.}
  append reply $oct2
  append reply {.}
  append reply $oct3
  append reply {.}
  append reply $oct4
  append reply {:}
  append reply $port

  return $reply
}



###############################################################################
# Procedure to check if a PLC address is already in the list
###############################################################################
proc chkplcunique {plcfield plclist} {

  # Format numbers to be 3 digits per octet
  set plcfield [plcformat $plcfield]     

  # First check if plclist is empty
  if { [llength $plclist] == 0 } {
     # Add this record as list is empty
     return $plcfield 
  } else {
     # List already has data in it
     set max [llength $plclist]
     set x 0
     set match 0
     while { $x < $max} {
        if { [string compare [lindex $plclist $x] $plcfield] == 0} {
           set match 1
        }
        incr x
     }
     if { $match == 0} {
        return $plcfield
     } else { 
        return {}
     }
  }
}



###############################################################################
# Procedure to count the plcs in the configuration file
###############################################################################
proc countplcs {handle} {
global tab 

  set counter 0
  set plclist {}

  # Position at start of file 
  seek $handle 0
  while {[eof $handle] == 0} {
    gets $handle line
    set line [string trim $line]
    set line [string trim $line $tab]
    if { [string range $line 0 0] == {/} } {
      set plcfield [ipaddr $line]
      set plcfield [chkplcunique $plcfield $plclist]  
      # Add PLC to list if not already present
      if {[string length $plcfield] > 0 } { 
         lappend plclist $plcfield
      }
    }
  }
  return $plclist
}



###############################################################################
# Procedure to count the number of differnet PLCs accessed through
# this configuration file
###############################################################################
proc plcs {handle} {

   ClearList .plcframe.listbox
   if { [string compare $handle {error}] != 0 } {
      seek $handle 0
      set plclist [countplcs $handle]
      for {set x 0} {$x < [llength $plclist]} {incr x} {
         .plcframe.listbox insert end [lindex $plclist $x] 
      }
   }
}



###############################################################################
# Procedure to check the size limits of each block in the
# configuration file
###############################################################################
proc checksize {blockarray} {
global BREADMAX WREADMAX BWRITEMAX WWRITEMAX blockerr

   set msg {Checking block sizes}
   .blockframe.listbox insert end $msg 
   set msg {==============================================================================}
   .blockframe.listbox insert end $msg 
   for {set x 0} {$x < [llength $blockarray]} {incr x} {
      # Step through each block checking size
      set record [lindex $blockarray $x]
      # Extract the separate entries from the list
      set blocknum [lindex $record 0]     
      set type [lindex $record 1]     
      set min [lindex $record 2]     
      scan $min {%d} min
      set max [lindex $record 3]     
      scan $max {%d} max
      set range [expr $max - $min]
      incr range
      set min [lindex $record 2]     
      set max [lindex $record 3]     
      # Prepare message to display
      set mess [trailpad $type 9]
      append mess {: block=}
      append mess [leadpad $blocknum 2]
      append mess {, min=}
      append mess [leadpad $min 6]
      append mess {, max=}
      append mess [leadpad $max 6]
      append mess {, range=}
      append mess [leadpad $range 3]
      append mess {, status=}

      # Remove padding from type
      set type [string trim $type]
      
      switch -exact -- $type {
         BREAD {if {$range > $BREADMAX} {
                      append mess {Too big!}
                      set blockerr 1
                     } else {
                      append mess {OK}
                     }
                  }
         BWRITE {if {$range > $BWRITEMAX} {
                      append mess {Too big!}
                      set blockerr 1
                     } else {
                      append mess {OK}
                     }
                  }
         CWRITE -
         WWRITE -
         LWRITE -
         FWRITE -
         DWRITE   {if {$range > $WWRITEMAX} {
                      append mess {Too big!}
                      set blockerr 1
                     } else {
                      append mess {OK}
                     }
                  }
         CREAD -
         WREAD -
         LREAD -
         FREAD -
         DREAD    {if {$range > $WREADMAX} {
                      append mess {Too big!}
                      set blockerr 1
                     } else {
                      append mess {OK}
                     }
                  }
         default {set mess $type; append mess {: ERROR! Unknown command.}; set blockerr 1}
     } 
     .blockframe.listbox insert end $mess 
   }
}



###############################################################################
# Procedure to get the address from a tag line
###############################################################################
proc addressline {line} {
global tab

   set len [string length $line]
   # Now find the end of the tagname
   set endword [string wordend $line 0]
   # Throw away the tagname
   set line [string range $line $endword [expr $len - 1]]
   set line [string trim $line]
   set line [string trim $line $tab]
   # Now find the end of the address 
   set endword [string wordend $line 0]
   # Throw away anything after the address
   set line [string range $line 0 $endword]
   set line [string trim $line]
   set line [string trim $line $tab]

   return $line
}



###############################################################################
# Procedure to check the numeric range of each block in the 
# configuration file
###############################################################################
proc blocksize {handle} {
global tab

   #ClearList .blockframe.listbox

   # Create an empty array for the results
   set blockarray {}

   # Create counters
   set min 999999
   set max 0

   if { [string compare $handle {error}] != 0 } {
      seek $handle 0
      set block 0
      while {[eof $handle] == 0} {
        gets $handle line
        set line [string trim $line]
        set line [string trim $line $tab]
        if { ([string range $line 0 0] == {/}) || ([eof $handle] != 0) } {
           # Found a header line
           if { $block != 0 } {
              # Create an entry in the list
              lappend blockarray [list $block $type $min $max] 
              # Now remember cnf_function 
              set type [cnf_function $line]
              # Now reset counters
              set min 999999
              set max 0 
           } else {
              # Remember block code
              set type [cnf_function $line]
           }
           incr block
        } else {
           # Just an ordinary line but could be blank or a comment
           if { ([string range $line 0 0] == {#})||([string length $line] == 0)} {
              # Do nothing!
           } else {
              # A valid line we assume
              set address [addressline $line]
              if { $address < $min } {
                 set min $address
              } 
              if { $address > $max } {
                 set max $address
              } 
           } 
        }
      } 
      # Now print blockarray
      checksize $blockarray 
   }
}



###############################################################################
# Function to close a window                                            
###############################################################################
proc CloseDialog {windname} {
   destroy $windname
}



###############################################################################
# Function to search for matching tags
###############################################################################
proc SearchFor {searchstr match windname} {
global fulltags

   # Clear the current display
   ClearList $windname.listbox
 
   for {set x 0} {$x < [llength $fulltags]} {incr x} {
      set record [lindex $fulltags $x]
      set tagname [lindex $record 0]
      if { $match == 0 } {
         # Exact match
         if { [string compare $searchstr $tagname] == 0 } {
            # Strings match
            set msg [lindex $record 0]
            append msg " at "
            append msg [lindex $record 1]
            append msg " in "
            append msg [lindex $record 2]
            $windname.listbox insert end $msg
         } 
      } else {
         # Contains
         if { [string first $searchstr $tagname] != -1 } {
            # Strings match
            set msg [lindex $record 0]
            append msg " at "
            append msg [lindex $record 1]
            append msg " in "
            append msg [lindex $record 2]
            $windname.listbox insert end $msg
         } 
      }
   }
}



###############################################################################
# Function to present the Summary dialog box
###############################################################################
proc SearchBox {} {
   global windname fulltags match fontfix

   set windname .searchbox
   set bg {Light Yellow}
   set padx 5
   set pady $padx
   set searchstr {}

   if { [winfo exists $windname] == 1 } {
      puts "Window exists so raising it!"
      raise $windname
   } else {
      toplevel $windname -class Toplevel -background $bg -padx $padx -pady $pady
      wm title $windname "Search" 
      wm geometry $windname 700x460+50+40
      wm minsize $windname 700 460
      wm overrideredirect $windname 0
      wm deiconify $windname 

      # Configure the widgets for the window
      label $windname.label0 -background $bg -text {Tagname:}
      entry $windname.entry0 -background {White} -foreground {Black} -textvariable searchstr -width 45

      label $windname.label1 -background $bg -text {Matching criteria} 
      frame $windname.radioframe -background $bg
      radiobutton $windname.radioframe.exact -background $bg -text {Exact} -variable match -value 0 
      radiobutton $windname.radioframe.close -background $bg -text {Contains} -variable match -value 1

      listbox $windname.listbox -font $fontfix -xscrollcommand "$windname.scrollx set" -yscrollcommand "$windname.scrolly set" -width 45 -height 10
      scrollbar $windname.scrollx -orient horizontal -command "$windname.listbox xview"
      scrollbar $windname.scrolly -orient vertical -command "$windname.listbox yview"

      button $windname.closebutton -padx 9 -pady 3 -text {Close} -command {CloseDialog $windname} -width 8
      button $windname.searchbutton -padx 9 -pady 3 -text {Search} -command {SearchFor $searchstr $match $windname}

      # Layout the window
      grid $windname.label0 -padx $padx -pady $pady -sticky w
      grid $windname.entry0 -column 1 -row 0 -padx $padx -pady $pady -sticky nsew
      grid $windname.searchbutton x -column 2 -row 0 -padx $padx -pady $pady

      grid $windname.label1 -row 1 -sticky w
      grid $windname.radioframe -column 1 -row 1 -padx $padx -pady $pady -sticky nsew
      grid $windname.radioframe.exact $windname.radioframe.close -row 1 -sticky ew

      grid $windname.listbox $windname.scrolly -sticky nsew -columnspan 3
      grid $windname.scrollx -columnspan 3 -sticky nsew
      grid $windname.closebutton -pady $pady -columnspan 3

      # Resize the widgets with the window
      grid columnconfigure $windname "0 1" -weight 1
      grid rowconfigure $windname 2 -weight 1

      # Key bindings
      focus $windname.entry0
      bind $windname <Return> {SearchFor $searchstr $match $windname}
      bind $windname <Escape> {CloseDialog $windname}
   } 
}



###############################################################################
# Function to delete the passed value from list of tags to be read in the PLC
###############################################################################
proc DelSelected {tagname pos} {
global fontfix windname filename selectedarray

   # Remove white space from tagname
   set tagname [string trim $tagname]

   if { [string length $tagname] > 0} {
      # Passed value is not blank
      # Now search for tag in selectedarray
      if { [set pos [lsearch $selectedarray $tagname]] == -1 } { 
         # Tagname not already in list so add it
      } else {
         # Delete from selectedarray
         set selectedarray [lreplace $selectedarray $pos $pos]
         .onlinebox.selframe.listbox delete $pos
      }
   }
}



###############################################################################
# Function to add the passed value to list of tags to be read in the PLC
###############################################################################
proc AddSelected {tagname} {
global fontfix windname filename selectedarray

   # Remove white space from tagname
   set tagname [string trim $tagname]

   if { [string length $tagname] > 0} {
      # Passed value is not blank
      # Now search for tag in selectedarray
      if { [lsearch $selectedarray $tagname] == -1 } { 
         # Tagname not already in list so add it
         lappend selectedarray $tagname
         .onlinebox.selframe.listbox insert end $tagname
      }
   }
}



###############################################################################
# Function to present the Online dialog box
###############################################################################
proc OnlineBox {} {
   global fontfix windname filename tagarray selectedarray pdsconn

   set padx 5
   set pady $padx

   if {[string length $filename] == 0} {
      tk_messageBox -type ok -message {No configuration file has been opened yet!} -icon error -title {Error}
   } else {
      set windname .onlinebox
      set bg {Light Yellow}
      if { [winfo exists $windname] == 1 } {
         puts "Window exists so raising it!"
         raise $windname
      } else {
         toplevel $windname -class Toplevel -background $bg -padx $padx -pady $pady
         wm title $windname "Online to PLC" 
         wm geometry $windname 780x560+5+5
         wm minsize $windname 780 560
         wm overrideredirect $windname 0
         wm deiconify $windname 

         # Configure the widgets for the window
         frame $windname.tagframe -borderwidth 2 -relief sunken
         label $windname.tagframe.label1 -borderwidth 1 -text {Tag Names}
         listbox $windname.tagframe.listbox -font $fontfix -yscrollcommand "$windname.tagframe.scroller set" -width 45 -height 10
         scrollbar $windname.tagframe.scroller -orient vertical -command "$windname.tagframe.listbox yview"

         frame $windname.selframe -borderwidth 2 -relief sunken
         label $windname.selframe.label1 -borderwidth 1 -text {Selected}
         listbox $windname.selframe.listbox -font $fontfix -yscrollcommand "$windname.selframe.scroller set" -width 45 -height 10
         scrollbar $windname.selframe.scroller -orient vertical -command "$windname.selframe.listbox yview"

         frame $windname.ansframe -borderwidth 2 -relief sunken
         label $windname.ansframe.label1 -borderwidth 1 -text {Results (word, highbyte, lowbyte, highchar lowchar)}
         listbox $windname.ansframe.listbox -font $fontfix -yscrollcommand "$windname.ansframe.scroller set" -width 70 -height 10
         scrollbar $windname.ansframe.scroller -orient vertical -command "$windname.ansframe.listbox yview"

         button $windname.startbutton -padx 9 -pady 3 -text {Start} -command {set READPLC 1; ScanPLC}
         button $windname.stopbutton -padx 9 -pady 3 -text {Stop} -command {set READPLC 0}
         button $windname.clearbutton -padx 9 -pady 3 -text {Clear} -command {ClearList $windname.ansframe.listbox}
         button $windname.closebutton -padx 9 -pady 3 -text {Close} -command {set READPLC 0; CloseDialog $windname}

         # Layout the window
         grid $windname.tagframe -sticky nsew -columnspan 2
         grid $windname.selframe -column 2 -row 0 -sticky nsew -columnspan 2

         grid $windname.tagframe.label1 -padx $padx -sticky w
         grid $windname.tagframe.listbox $windname.tagframe.scroller -sticky nsew -row 1

         grid $windname.selframe.label1 -padx $padx -sticky w
         grid $windname.selframe.listbox $windname.selframe.scroller -sticky nsew -row 1
  
         grid $windname.ansframe -sticky nsew -columnspan 3 -rowspan 3
         grid $windname.ansframe.label1 -padx $padx -sticky w
         grid $windname.ansframe.listbox $windname.ansframe.scroller -sticky nsew -row 1

         grid $windname.startbutton -column 3 -row 1
         grid $windname.stopbutton -column 3 -row 2
         grid $windname.clearbutton -column 3 -row 3
         grid $windname.closebutton -pady $pady -columnspan 4

         # Resize the widgets with the window
         grid columnconfigure $windname "0 1 2 3" -weight 1
         grid rowconfigure $windname "0 1 2 3" -weight 1

         grid columnconfigure $windname.tagframe 0 -weight 1
         grid rowconfigure $windname.tagframe 1 -weight 1

         grid columnconfigure $windname.selframe 0 -weight 1
         grid rowconfigure $windname.selframe 1 -weight 1

         grid columnconfigure $windname.ansframe 0 -weight 1
         grid rowconfigure $windname.ansframe 1 -weight 1

         # Key bindings
         focus $windname.tagframe.listbox
         bind $windname.tagframe.listbox <Double-1> {AddSelected [$windname.tagframe.listbox get [%W curselection]]}
         bind $windname.tagframe.listbox <Return> {AddSelected [$windname.tagframe.listbox get [%W curselection]]}
         bind $windname.selframe.listbox <Double-1> {DelSelected [$windname.selframe.listbox get [%W curselection]] [%W curselection]}
         bind $windname.selframe.listbox <Return> {DelSelected [$windname.selframe.listbox get [%W curselection]] [%W curselection]}
         bind $windname.startbutton <Return> {set READPLC 1; ScanPLC}
         bind $windname.stopbutton <Return> {set READPLC 0}
         bind $windname.clearbutton <Return> {ClearList $windname.ansframe.listbox}
         bind $windname.closebutton <Return> {set READPLC 0; CloseDialog $windname}
         bind $windname <Escape> {set READPLC 0; CloseDialog $windname}
      }

      # Load all available tags
      for {set x 0} {$x < [llength $tagarray]} {incr x} {
         set mess [lindex $tagarray $x]
         $windname.tagframe.listbox insert end $mess 
      }
      
      for {set x 0} {$x < [llength $selectedarray]} {incr x} {
         set mess [lindex $selectedarray $x]
         $windname.selframe.listbox insert end $mess 
      }
   }
}



###############################################################################
# Function to present the About dialog box
###############################################################################
proc AboutBox {} {
  global pds_version tcl_patchLevel

  set title "About"
  set message "PDS: $pds_version\nTcl/Tk: $tcl_patchLevel\nAuthor: Paul M. Breen"

  tk_messageBox -type ok -icon info -title $title -message $message
}



###############################################################################
# Function to generate the main display window
###############################################################################
proc MainDisplay {base} {
  global fontfix tmpmess filename pdsconn

  set bg {Light Yellow}
  set padx 5
  set pady $padx
  set statusw 15
  set statush $statusw

  # Configure the window
  $base config -background $bg -padx $padx -pady $pady
  $base config -menu .mainmenu
  wm focusmodel $base passive
  wm title $base "PLC Data Server - plc.cnf Checker" 
  wm geometry $base 800x600+0+0
  wm overrideredirect $base 0
  wm deiconify $base

  # Configure the menus
  menu .mainmenu -cursor {} -tearoff 0
  .mainmenu add cascade -menu .mainmenu.filemenu -label File -underline 0
  .mainmenu add cascade -menu .mainmenu.editmenu -label Edit -underline 0
  .mainmenu add cascade -menu .mainmenu.plcmenu -label PLC -underline 0
  .mainmenu add cascade -menu .mainmenu.aboutmenu -label Help -underline 0
  menu .mainmenu.filemenu -tearoff 0
  .mainmenu.filemenu add command -command {ChooseFile} -label {Open} -underline 0 -accelerator {Ctrl+O}
  .mainmenu.filemenu add command -command {ReloadFile $filename} -label {Reload} -underline 0 -accelerator {Ctrl+R}
  .mainmenu.filemenu add command -command {CloseApp} -label {Exit} -underline 1 -accelerator {Ctrl+X}
  .mainmenu.filemenu entryconfigure 1 -state disabled
  menu .mainmenu.editmenu -tearoff 0
  .mainmenu.editmenu add command -command {SearchBox} -label {Find} -underline 0 -accelerator {Ctrl+F}
  .mainmenu.editmenu entryconfigure 1 -state disabled
  menu .mainmenu.plcmenu -tearoff 0
  .mainmenu.plcmenu add command -command {ConnectPLC} -label {Connect} -underline 0 -accelerator {Ctrl+C}
  .mainmenu.plcmenu add command -command {DisconnectPLC} -label {Disconnect} -underline 0 -accelerator {Ctrl+D}
  .mainmenu.plcmenu add separator
  .mainmenu.plcmenu add command -command {OnlineBox} -label {Online}
  .mainmenu.plcmenu entryconfigure 0 -state disabled
  .mainmenu.plcmenu entryconfigure 1 -state disabled
  .mainmenu.plcmenu entryconfigure 3 -state disabled
  menu .mainmenu.aboutmenu -tearoff 0
  .mainmenu.aboutmenu add command -command {AboutBox} -label About -underline 0

  # Configure the widgets for the window
  frame .plcframe -borderwidth 2 -relief sunken
  label .plcframe.label1 -borderwidth 1 -text {PLCs}
  listbox .plcframe.listbox -font $fontfix -yscrollcommand ".plcframe.scroller set" -width 22 -height 10
  scrollbar .plcframe.scroller -orient vertical -command ".plcframe.listbox yview"

  frame .tagframe -borderwidth 2 -relief sunken
  label .tagframe.label1 -borderwidth 1 -text {Tag Names}
  label .tagframe.label2 -borderwidth 1 -text {Status}
  listbox .tagframe.listbox -font $fontfix -yscrollcommand ".tagframe.scroller set" -width 45 -height 10
  scrollbar .tagframe.scroller -orient vertical -command ".tagframe.listbox yview"
  frame .tagframe.statusframe -borderwidth 2 -relief sunken -width $statusw -height $statush

  frame .blockframe -borderwidth 2 -relief sunken
  label .blockframe.label1 -borderwidth 1 -text {Data Blocks}
  label .blockframe.label2 -borderwidth 1 -text {Status}
  listbox .blockframe.listbox -font $fontfix -yscrollcommand ".blockframe.scroller set" -width 69 -height 10
  scrollbar .blockframe.scroller -orient vertical -command ".blockframe.listbox yview"
  frame .blockframe.statusframe -borderwidth 2 -relief sunken -width $statusw -height $statush

  # Layout the window
  grid .plcframe .tagframe -sticky nsew

  grid .plcframe.label1 -padx $padx -sticky w
  grid .plcframe.listbox .plcframe.scroller -sticky nsew -row 1

  grid .tagframe.label1 -padx $padx -sticky w
  grid .tagframe.label2 -padx $padx -column 1 -row 0 -sticky e
  grid .tagframe.statusframe -column 2 -row 0 -sticky w
  grid .tagframe.listbox .tagframe.scroller -sticky nsew -row 1 -columnspan 2

  grid .blockframe -columnspan 2 -sticky nsew

  grid .blockframe.label1 -padx $padx -sticky w
  grid .blockframe.label2 -padx $padx -column 1 -row 0 -sticky e
  grid .blockframe.statusframe -column 2 -row 0 -sticky w
  grid .blockframe.listbox .blockframe.scroller -sticky nsew -row 1 -columnspan 2

  # Resize the widgets with the window
  grid columnconfigure . "0 1" -weight 1
  grid rowconfigure . "0 1" -weight 1

  grid columnconfigure .plcframe 0 -weight 1
  grid rowconfigure .plcframe 1 -weight 1

  grid columnconfigure .tagframe "0 1" -weight 1
  grid rowconfigure .tagframe 1 -weight 1

  grid columnconfigure .blockframe "0 1" -weight 1
  grid rowconfigure .blockframe 1 -weight 1

  # Menu key bindings
  bind . <Control-O> {ChooseFile}
  bind . <Control-o> {ChooseFile}
  bind . <Control-X> {CloseApp}
  bind . <Control-x> {CloseApp}
}



###############################################################################
# Main Program
###############################################################################
set pds_version {2.1.0}
set pds_dir {/usr/local/pds}
set filename {}
set tab \012
set BREADMAX 100
set WREADMAX 100
set BWRITEMAX 100
set WWRITEMAX 100
set BLOCKLIMIT 100
set PRINT_CURR_VAL_TOP 0
set blockerr 0
set selectedarray {}
set fulltags {}
set READPLC 0 
set IPCKEY 1423779
set pdsconn {}
set dir $pds_dir/data
set match 1
set pds_lib_dir $pds_dir/lib

source $pds_lib_dir/libtcl.tcl

if {$argc != 0} {
  puts "Error! Wrong number of arguments supplied!\n"
  puts "Usage: chkcnf.tcl"
  exit
}

load $pds_lib_dir/libpds_tcl.so
 
MainDisplay .
ChooseFile

