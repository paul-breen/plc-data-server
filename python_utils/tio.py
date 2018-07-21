###############################################################################
# PROJECT:  PDS Utilities
# MODULE:   tio.py
# PURPOSE:  Utility program to connect to the PDS and read/write a tag
# AUTHOR:   Paul M. Breen
# DATE:     2018-06-23
###############################################################################

import argparse
import datetime

# N.B.: This is to import the installed module.  To import during development,
#       symlink ./build/pds as pds, and then this will work for the local copy
import pds.pds as pds

def get_printable_or_space(char):
    return (char if 31 < char < 127 else 32)
 
def set_tag(conn, args):
    retval = pds.PDSset_tag(conn, args.tagname, [args.set])

    if retval == -1:
        print("Failed to set tag {} value".format(args.tagname))

    return retval

def get_tag(conn, args):
    retval, tagvalue = pds.PDSget_tag(conn, args.tagname)

    if retval == -1:
        print("Failed to get tag {} value".format(args.tagname))
    else:
        tagvalue = int(tagvalue)

        if args.char:
            hi = get_printable_or_space(tagvalue >> 8)
            lo = get_printable_or_space(tagvalue & 0xff)
            print("{:c}{:c}".format(hi, lo))
        elif args.float:
            print("{:f}".format(tagvalue))
        else:
            print("{:d}".format(tagvalue))

    return retval

def process_tag(args):
    conn = pds.PDSconnect(pds.PDS_IPCKEY)

    if conn.conn_status != pds.PDS_CONN_OK:
        raise ValueError("Error connecting to the PDS: {}".format(conn.status))

    if args.set:
        retval = set_tag(conn, args)
    else:
        retval = get_tag(conn, args)

    pds.PDSdisconnect(conn)

    return retval

def parse_tio_cmdln():
    parser = argparse.ArgumentParser(description="get or set the value of the given tag")
    parser.add_argument("tagname", help="name of the tag to get or set")

    group = parser.add_mutually_exclusive_group()
    group.add_argument("-c", "--char", help="format tag value as characters", action="store_true")
    group.add_argument("-f", "--float", help="format tag value as float", action="store_true")
    group.add_argument("-i", "--int", help="format tag value as integer", action="store_true")

    group = parser.add_mutually_exclusive_group()
    group.add_argument("-g", "--get", help="get the tag value (default)", action="store_true")
    group.add_argument("-s", "--set", help="set the tag value to TAGVALUE", metavar="TAGVALUE", type=int)

    args = parser.parse_args()

    return args

if __name__ == '__main__':
    args = parse_tio_cmdln()
    retval = process_tag(args)

    exit(retval)

