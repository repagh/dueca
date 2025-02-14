#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Apr  7 19:11:09 2022

@author: repa
"""
try:
    from .ddffbase import DDFF, DDFFStream, dprint
except:
    from ddffbase import DDFF, DDFFStream, dprint
import json
import numpy as np


class DDFFInventoriedStream:
    """Datastream in an inventory

    Such a datastream is timed or tagged (usually with an integer),
    and has a series of data members all of the same type.
    """

    class TimeIt:
        """Iterator for time or tag values."""

        def __init__(self, ddffs: DDFFStream):
            """Create an iterator for time values on an inventoried stream

            Parameters
            ----------
            ddffs : DDFFStream
                Base data stream
            """
            self.ddffs = ddffs

        def __iter__(self):
            self.it = iter(self.ddffs)
            return self

        def __next__(self):
            tmp = next(self.it)
            try:
                return tmp[0]
            except TypeError:
                print("could not get time from", tmp, "stream", self.ddffs)

    class ValueIt:
        """Iterator for data values"""

        def __init__(self, ddffs: DDFFStream, idx: int | None):
            """Create an iterator for data values on an inventoried stream

            Parameters
            ----------
            ddffs : DDFFStream
                Base data stream
            idx : int
                Data member to return, or None for returning the complete
                data structure
            """
            self.ddffs = ddffs
            self.idx = idx

        def __iter__(self):
            self.it = iter(self.ddffs)
            return self

        def __next__(self):
            tmp = next(self.it)
            try:
                return tmp[-1][self.idx]
            except IndexError as e:
                print(f"error {e}")

    def __init__(self, ddffs: DDFFStream, tag: str, description: str):
        """Create a parsed inventory of stream id's and contents

        Arguments:
            ddffs -- Raw stream
            tag -- Name of the stream, id
            description -- JSON encoded string with a description of the
            stream contents.
        """
        self.base = ddffs
        self.tag = tag
        self.structure = json.loads(description)
        self.klass = self.structure["class"]
        self.members = {}
        for im, m in enumerate(self.structure["members"]):
            self.members[m["name"]] = im

    def time(self):
        """Create an iterator to run through all time values in the
            data stream.

        Returns
        -------
        TimeIt
            Iterator to get all time values.
        """
        return DDFFInventoriedStream.TimeIt(self.base)

    def __getitem__(self, key: int | str | None):
        """Obtain an iterator on the data

        Parameters
        ----------
        key : int|str|None
            Which part of the data to access, None gives whole structure

        Returns
        -------
        ValueIt
            Iterator on values.
        """
        if key is None:
            return DDFFInventoriedStream.ValueIt(self.base, None)
        elif isinstance(key, int):
            return DDFFInventoriedStream.ValueIt(self.base, key)
        else:
            return DDFFInventoriedStream.ValueIt(self.base, self.members[key])

    def __str__(self):
        return f'Object(class="{self.klass}",members={", ".join(self.members.keys())})'

    def getClass(self, key: int | str | None = None):
        if key is None:
            return self.structure["class"]
        if isinstance(key, int):
            return self.structure["members"][key]["class"]
        return self.structure["members"][self.members[key]]["class"]

    def getSize(self, key: int | str):
        try:
            if isinstance(key, int):
                return self.structure["members"][key]["size"]
            return self.structure["members"][self.members[key]]["size"]
        except KeyError:
            return 1

    def getMeta(self, key: int | str | None = None):
        """ Metadata description of the data in this stream

        Parameters
        ----------
        key : int | str | None, optional
            access either metadata for a single member (give by key) or for the whole 
            stream, by default None

        Returns
        -------
        dict
            dictionary with data on the stream or member.
        """
        if isinstance(key, str):
            key = self.members[key]
        if key is None:
            return self.structure["members"]
        return self.structure["members"][key]


class DDFFInventoried(DDFF):
    """Inventoried file representation.

    The inventory functions as a dictionary with "streams",
    partial data in the file of a specific type. Each stream
    is named, however, you can also access the stream with their
    integer id. Stream 0 for such an inventoried file is by
    definition the inventory itself.

    The dictionary members are data streams of DDFFInventoriedStream
    type. These are tagged with a time or index, and are all of the same
    data type.
    """

    def __init__(self, fname, mode="r", *args, **kwargs):
        """Open a DDFF datafile with stream inventory

        Arguments:
            fname -- filename to open

        Keyword Arguments:
            mode -- open mode, read or write (default: {'r'})
        """

        # analyse with base DDFF read
        super().__init__(fname, mode=mode, *args, **kwargs)
        self.mapping = {}
        dprint("number of streams", len(self.streams), self.streams[0])

        # stream #0 should now be the inventory
        for tag, streamid, description in self.streams[0]:

            # replace/swap the raw streams with inventoried ones
            self.streams[streamid] = DDFFInventoriedStream(
                self.streams[streamid], tag, description
            )

            self.mapping[tag] = self.streams[streamid]

    def inventory(self):
        """Return the inventory itself, for compatibility purposes

        Returns
        -------
        DDFFInventoried
            Object itself.
        """
        return self

    def time(self, key, period=None):
        """Access time stamps for a specific named stream

        Arguments:
            period -- Optional, ignored
            key -- Name or number of requested stream

        Returns:
            Iterator for time ticks
        """
        if isinstance(key, int):
            return self.streams[self.streams[0][key][1]]
        return self.mapping[key].time()

    def __getitem__(self, keyname):
        """Access data, from a specific named stream

        Arguments:
            keyname -- tuple(streamid,member), tuple(streamid,period,member) or only streamid
            Defines which data stream should be returned. The member string
            indicates which data member to return.

        Returns:
            Iterator for data, either for a single member, or the whole data
            list/struct
        """
        try:
            if isinstance(keyname, str | int):
                key = keyname
                varname = None
            elif len(keyname) == 2:
                key, varname = keyname
            elif len(keyname) == 3:
                key, _, varname = keyname
        except ValueError:
            key = keyname
            varname = None

        if isinstance(key, int):
            stream = self.streams[self.streams[0][key][1]]
        else:
            stream = self.mapping[key]

        # return an iterator on the data (or all), corresponding to
        # this stream
        return stream[varname]

    def stream(self, key: str|int):
        """ Access a specific data stream in the file

        Parameters
        ----------
        key : str|int
            Stream id or number to access

        Returns
        -------
        DDFFInventoriedStream
            Data access
        """
        if isinstance(key, int):
            return self.streams[self.streams[0][key][1]]
        return self.mapping[key]

    def keys(self):
        """Return an overview of available named streams

        Returns:
            Iterator of strings
        """
        return [k[0] for k in self.streams[0]]


if __name__ == "__main__":

    stuff = DDFFInventoried("../recordings-PHLAB-new.ddff")

    # known entries
    print(stuff.keys())

    for t in stuff.time("WriteUnified:first blip"):
        print(t)

    for x in stuff["WriteUnified:first blip", "rx"]:
        print(x)
    for x in stuff["WriteUnified:first blip", "ry"]:
        print(x)
