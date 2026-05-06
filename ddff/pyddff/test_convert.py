import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))


from pyddff.ddff_convert import parser
import os

import numpy as np

dtype = np.dtype([("one", np.float64),("two", np.float32),("three", np.int32)])

d = np.array([(3, 2.2, 3), (4.0, 5.1, 0)], dtype=dtype)
d2 = np.zeros((2,3), dtype=dtype)
d2[0] = (1, 2, 3)

#fname = f"{os.environ['HOME']}/gdapps/VarStabCitation2/VarStabCitation2/run/solo/solo/simlog-20250225_155749.ddff"
#fname = f"{os.environ['HOME']}/tmp/recording-PHLAB-new.ddff"
# fname = f"{os.environ['HOME']}/tmp/varstab/260224/runlogs/2026-02-24_12:39:03/simlog-20260224_113932.ddff"
fname = f"{os.environ['HOME']}/gdapps/VarStabCitation2/VarStabCitation2/run/solo/solo/simlog-20260422_100504.ddff"
fname = f"{os.environ['HOME']}/gdapps/DuecaTestCommunication/DuecaTestCommunication/run/solo/solo/datalog.ddff"
# separate test of the readstream
#df = DDFFTagged(fname)
pres = parser.parse_args(
    ("--verbose", "hdf5", "--as-event", "/data/testsignalspec", "/data/apsetting", "/data/gainsindi", "--outfile=/tmp/result2.hdf5", fname))
pres.handler()(pres)

pres = parser.parse_args(
    ("-v", "-v", "-v", "info", fname))
pres.handler()(pres)

pres = parser.parse_args(
    ("-v", "-v", "-v", "hdf5", "--streamids", "/data/servodetails", "--outfile=/tmp/result.hdf5", fname))
pres.handler()(pres)

# overview of contents
pres = parser.parse_args(
    ("--verbose", "info", fname))
pres.handler()(pres)

pres = parser.parse_args(
    ("--verbose", "info", "--period", "0", fname))
pres.handler()(pres)

pres = parser.parse_args(
    ("--verbose", "info", "--streamid", "/data/pilot", fname))
pres.handler()(pres)
