###############################################################################
# PROJECT:  PLC Data Server
# MODULE:   libtcl.tcl
# PURPOSE:  Utility functions for Tcl/Tk GUI programs
# AUTHOR:   Paul M. Breen
# DATE:     1998-04-22
###############################################################################

# Configure some useful fonts
set fontfix -*-Courier-Medium-R-Normal--*-140-*-*-*-*-*-*
# set smallfontfix -*-Courier-Medium-R-Normal--*-130-*-*-*-*-*-*
set smallfontfix -adobe-courier-medium-r-normal--12-120-75-75-m-70-iso8859-1 
set font -*-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*
set fontlb -*-Helvetica-Medium-R-Normal--*-155-*-*-*-*-*-*
set fontbg -*-Helvetica-Medium-R-Normal--*-240-*-*-*-*-*-*

###############################################################################
# Stops window being minimized
###############################################################################
proc nomin win {
  wm protocol $win WM_DELETE_WINDOW { }
  bind $win <Unmap> "wm deiconify $win"
}



###############################################################################
# Centre to window
###############################################################################
proc centre win {
  update idletasks
  set x [wm maxsize $win]
  set mx [lindex $x 0]
  set my [lindex $x 1]
  set xw [winfo reqwidth $win]
  set yw [winfo reqheight $win]
  wm deiconify $win
  wm geometry $win [format "+%d+%d" [expr ($mx-$xw)/2] [expr ($my-$yw)/2]]
  update
}



###############################################################################
# Title Bar With Help About
###############################################################################
proc mbar win {
  global font fontlb fontbg
  frame .mbar -relief raised -bd 2
  pack .mbar -side top -fill x
  menubutton .mbar.help -text "Help" -underline 0 -menu .mbar.help.menu
  pack .mbar.help -side right
  menu .mbar.help.menu
  .mbar.help.menu add command -command about -label About
}

