#!@Python_EXECUTABLE@
# PYTHON_ARGCOMPLETE_OK
# -*-python-*-
"""item            : ddff-convert
made by         : RvP
date            : 2023
category        : python program
description     : Conversion of ddff data
language        : python
changes         :
copyright       : 2023 Rene van Paassen
license         : EUPL-1.2
"""
import os
import sys
import argparse
import argcomplete
import h5py

from .ddffbase import vprint, set_verbose
from .ddfftagged import DDFFTagged
from .ddffinventoried import DDFFInventoried

parser = argparse.ArgumentParser(description="Convert or inspect DDFF data")
parser.add_argument(
    "-v",
    "--verbose",
    action="count",
    default=0,
    help="Verbose run with information output",
)
subparsers = parser.add_subparsers(help="commands", title="commands")


class Info:
    """List properties of DDFF files"""

    command = "info"

    @classmethod
    def args(cls, subparsers):
        parser = subparsers.add_parser(cls.command, help="Inspect a ddff file")
        parser.add_argument(
            "--period", type=str, help="Inspect a specific recording period"
        )
        parser.add_argument(
            "--inventory",
            action="store_true",
            help="Inventory-only, no time period inspection",
        )
        parser.add_argument("--streamid", type=str, help="Inspect a specific stream")
        parser.add_argument("filename", type=str, help="File name to be analysed")
        parser.set_defaults(handler=Info)

    def __call__(self, ns: argparse.Namespace):

        if ns.inventory:
            f = DDFFInventoried(ns.filename)
        else:
            f = DDFFTagged(ns.filename)

        if ns.inventory and ns.period:
            print(
                "--inventory and --period options cannot be combined", file=sys.stderr
            )
            sys.exit(-1)

        if ns.period:
            try:
                print(f"Details for period {ns.period}:\n", str(f.tags()[ns.period]))
            except KeyError:
                print(f"Cannot find period {ns.period}", file=sys.stderr)
            except Exception as e:
                print(f"Cannot read index, error {e}")

        if ns.streamid:
            try:
                print(
                    f"Details for stream {ns.streamid}\n", str(f.mapping[ns.streamid])
                )
            except KeyError:
                print(f"Cannot find stream {ns.streamid}", file=sys.stderr)
            except Exception as e:
                print(f"Cannot read streamid, error {e}", file=sys.stderr)

        if not ns.streamid and not ns.period:
            try:
                if not ns.inventory:
                    print(
                        'Available periods:\n"',
                        '", "'.join(f.tags().keys()),
                        '"',
                        sep="",
                    )
                print(
                    'Available streams:\n"',
                    '", "'.join(f.keys()),
                    '"',
                    sep="",
                )
            except Exception as e:
                print(f"Cannot read periods/inventory, error {e}", file=sys.stderr)


Info.args(subparsers)


# def doExclude(x: list, idxes: list):
#     for i in reversed(idxes):
#         del x[i]
#     return tuple(x)


class ToHdf5:
    """Convert a ddff file to a hdf5 format"""

    command = "hdf5"

    @classmethod
    def args(cls, _subparsers):
        """Add appropriate options for the subcommand hdf5

        Parameters
        ----------
        subparsers : List of sub parsers
            List to add the command sub parser
        """
        parser = _subparsers.add_parser(
            cls.command,
            help="""Convert a ddff file to hdf5

In principle, each stream is converted to an equivalently named group in the hdf5 file.
This group has members "tick", an array with the timing, and "data" a group with the
data, with each object member having a data array with its data.

if the --as-event flag is used, "data" will become a data array with the data
as a struct.""",
        )
        parser.add_argument(
            "--period", type=str, help="Convert a specific recording period"
        )
        parser.add_argument(
            "--compress",
            type=str,
            choices=("gzip", "lzf", ""),
            default="",
            help="Specify a compression method",
        )
        parser.add_argument(
            "--inventory",
            action="store_true",
            help="Inventory-only, no time period inspection",
        )
        parser.add_argument(
            "--streamids", nargs="+", default=[], help="Convert specific stream(s)"
        )
        parser.add_argument(
            "--as-event", nargs="+", default=[], help="Write as array of objects"
        )
        parser.add_argument(
            "--outfile",
            type=str,
            default="",
            help="Filename for output file, if not specified, created from\n"
            "the input filename",
        )
        parser.add_argument(
            "--expected-size",
            type=int,
            default=1000,
            help="Expected data size, for pre-allocating numpy arrays",
        )
        parser.add_argument("filename", type=str, help="File name to be analysed")
        parser.set_defaults(handler=ToHdf5)

    def __call__(self, ns: argparse.Namespace):
        """Perform the class action

        Parameters
        ----------
        ns : argparse.Namespace
            Parsed command line arguments
        """

        if ns.compress:
            compressargs = {"compression": ns.compress}
        else:
            compressargs = {}

        if ns.inventory:
            f = DDFFInventoried(ns.filename)
        else:
            f = DDFFTagged(ns.filename)

        if ns.period:
            pargs = dict(period=ns.period)
        else:
            pargs = dict()

        vprint("Opened file", ns.filename)

        # either all stream id's, or just the selected ones
        if not ns.streamids:
            ns.streamids = list(f.keys())

        # hdf5 file name
        if not ns.outfile:
            if ns.filename.endswith(".ddff"):
                ns.outfile = os.path.basename(ns.filename[:-4] + "hdf5")
            else:
                ns.outfile = os.path.basename(ns.filename + ".hdf5")
        vprint("output file", ns.outfile)

        # create the file
        hf = h5py.File(ns.outfile, "w")

        for streamid in ns.streamids:
            vprint("Processing stream", streamid)
            gg = hf.create_group(streamid)

            if streamid in ns.as_event:
                time, data = f[streamid].get_events(**pargs, icount=ns.expected_size)
                gg.create_dataset("data", data=data, **compressargs)
            else:
                dg = gg.create_group("data")
                time, _, values = f[streamid].get_data(**pargs, icount=ns.expected_size)
                for m, v in values.items():
                    dg.create_dataset(m, data=v, **compressargs)
            vprint(f"number of data points {time.shape[0]}")
            gg.create_dataset("tick", data=time, **compressargs)

        hf.close()


ToHdf5.args(subparsers)

argcomplete.autocomplete(parser)

def main():
    """Execute a conversion or info action
    """

    # parse arguments
    pres = parser.parse_args(sys.argv[1:])

    # verbose output, todo
    if pres.verbose:
        set_verbose(pres.verbose)

    # extract the handler
    try:
        hclass = pres.handler
    except AttributeError:
        parser.print_usage()
        sys.exit(-1)

    # create and run the handler
    handler = hclass()
    handler(pres)

if __name__ == "__main__":
    main()
