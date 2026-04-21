# Logging with DDFF {#ddfflogging}

## Introduction

DDFF or Delft Data File Format, is a datafile layout introduced for DUECA/DUSIME
record and replay facilities, and it is also available and useful for generic
logging. The low-level file format is itself based on the [msgpack](https://msgpack.org/)
binary packing and unpacking format. To facilitate recording and replay of (generally the
input of) a simulation, the DDFF loggers provide real-time logging and reading of data.
This logging backbone can be used in a number of different ways:

- Easy logging of data that is already sent over channels. The "ddff-logger" module can be
  configured to read one or more data channels, or also autodetect entries appearing in data
  channels, and write these to a ddff file.

- Custom logging or re-reading of data. Although initially not recommended, it is also
  possible to access the ddff logging components yourself. Since in most cases the ddff-logger
  can provide all logging needs, manual creation of logs and sending data is not needed. If
  you still want to use this option, take a look at the test code in the `test/ddff` folder.

- Record and replay facilities for simulations. If you want to replay simulation runs, the
  coolest way to go about this is by recording all variable (user) input, and replaying that
  to drive the exact same simulation again. Such deterministic simulations are the holy grail
  of simulation professionalism. If you can manage an exact replay from a replay of only the input, you can be show that:
  - You eliminated all race conditions and other sources of non-determinism.
  - You can capture and restore a complete representation of your simulation's state.

DUECA also provides facilities for logging in the HDF5 format. Unfortunately, logging
real-time data in that format can require quite some processing power, and if you try
to log from somewhere between 10 and 20 channels, the hdf5 logger may clog up your
simulation. If that happens (or anyway), you can easily switch over to DDFF, since the
modules have compatible arguments for their configuration. A Python-based program can
be used to convert your DDFF file into a HDF5 format that is very close to the type of
file DUECA would directly produce, so this step will have minimal impact on your data
processing. The ddff reading is also directly usable from your own Python scripts.