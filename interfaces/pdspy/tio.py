import pds
import datetime
import time

conn = pds.PDSconnect(pds.PDS_IPCKEY)

if conn.conn_status != pds.PDS_CONN_OK:
    raise ValueError("Error connecting to the PDS: {}".format(conn.status))

tagname = "pds_mb_read_word2"
tag = pds.PDSget_tag_object(conn, tagname)

if tag is None:
    raise ValueError("{}: No such tagname".format(tagname))

print("{} = {} ({} = {})".format(tag.name, tag.value, tag.mtime, datetime.datetime.fromtimestamp(tag.mtime).strftime("%Y-%m-%dT%H:%M:%S")))

while True:
    try:
        print("{} = {} ({} = {})".format(tag.name, tag.value, tag.mtime, datetime.datetime.fromtimestamp(tag.mtime).strftime("%Y-%m-%dT%H:%M:%S")))
        time.sleep(1)
    except KeyboardInterrupt:
        print("Keyboard Interrupt detected: exiting...")
        exit(0)

pds.PDSdisconnect(conn)

exit(0)

