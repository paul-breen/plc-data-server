###############################################################################
# PROJECT:  PDS Utilities
# MODULE:   mcrd.py
# PURPOSE:  Utility program to read multiple tags in the PDS's shared memory
#           segment - continuously
# AUTHOR:   Paul M. Breen
# DATE:     2018-06-27
###############################################################################

import sys
from os import path
import time

# N.B.: This is to import the installed module.  To import during development,
#       symlink ./build/pds as pds, and then this will work for the local copy
import pds.pds as pds

def process_tags(tagnames):
    conn = pds.PDSconnect(pds.PDS_IPCKEY)

    if conn.conn_status != pds.PDS_CONN_OK:
        raise ValueError("Error connecting to the PDS: {}".format(conn.status))

    tags = []

    # N.B.: PDSget_tag_object() returns a direct pointer to memory in the
    #       PDS shared memory segment, and doesn't use semaphores to marshall
    #       access to the memory.  This gives good performance, but there is
    #       potential for updates across a given PLC block to be only
    #       partially complete when the tags are read.  If guaranteed
    #       consistency within a PLC block is required, then use a function
    #       which does marshall access to the shared memory via semaphores,
    #       such as PDSget_tag()
    for tagname in tagnames:
        tag = pds.PDSget_tag_object(conn, tagname)
        tags.append(tag)

    while True:
        try:
            for tag in tags:
                print("{} = {}".format(tag.name, tag.value))
            time.sleep(1)
        except KeyboardInterrupt:
            print("Keyboard Interrupt detected: exiting...")
            break

    pds.PDSdisconnect(conn)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        progname = path.basename(sys.argv[0])
        print("Usage: {} tagname1 [tagname2 ... tagnameN]".format(progname))
        exit(2)

    process_tags(sys.argv[1:])

    exit(0)

