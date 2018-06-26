###############################################################################
# PROJECT:  PDS Utilities
# MODULE:   tio.py
# PURPOSE:  Utility program to connect to the PDS and read/write a tag
# AUTHOR:   Paul M. Breen
# DATE:     2018-06-23
###############################################################################

import argparse
import datetime

import pds

def get_printable_or_space(char):
    return (char if 31 < char < 127 else 32)
 
def set_tag(args, tag):
    tag.value = args.set
    tag.mtime = round(datetime.datetime.timestamp(datetime.datetime.now()))

def get_tag(args, tag):
    if args.char:
        hi = get_printable_or_space(tag.value >> 8)
        lo = get_printable_or_space(tag.value & 0xff)
        print("{:c}{:c}".format(hi, lo))
    elif args.float:
        print("{:f}".format(tag.value))
    else:
        print("{:d}".format(tag.value))

def process_tag(args):
    conn = pds.PDSconnect(pds.PDS_IPCKEY)

    if conn.conn_status != pds.PDS_CONN_OK:
        raise ValueError("Error connecting to the PDS: {}".format(conn.status))

    tag = pds.PDSget_tag_object(conn, args.tagname)

    if tag is None:
        raise ValueError("{}: No such tagname".format(args.tagname))

    if args.set:
        set_tag(args, tag)
    else:
        get_tag(args, tag)

    pds.PDSdisconnect(conn)

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
    process_tag(args)

