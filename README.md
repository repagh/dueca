# DUECA Delft University Environment for Communication and Activation

## What is this?

DUECA is a "middleware library" for distributed (over computers)
real-time calculation processes. Modules written to use DUECA can
communicate with other modules running in a possibly distributed DUECA
process, no matter where these other modules are located. In addition
each module can specify its update rate, and access to data of the
"right time" is automatically given by DUECA.

DUSIME (Delft University SIMulation Environment) builds forth on
DUECA, to give you an environment tailored to implement real-time
(hardware in the loop and man in the loop) simulations.

DUECA/DUSIME has been used for over 20 years at the Control and
Simulation department, Aerospace Engineering, TU Delft. It forms the
backbone for many simulation projects and experiments on the SIMONA
Research Simulator and other facilities. It is released as open source
in 2022.

## How is it implemented?

It is implemented as:

 * A set of libraries, dueca, dueca-extra (additional tools), dueca-dusime,
   dueca-inter, dueca-udpcom, dueca-websock, dueca-hdf5, dueca-ddff

 * Support scripts for setting up simulations, code generation, etc.

 * There is also [Documentation](https://dueca.tudelft.nl/doc/), which
   can be generated from the `doc` folder.

 * Some example code; a [test project](https://github.com/dueca/DuecaTestCommunication), which is used in CI testing, and an [example project](https://github.com/dueca/SimpleSimulation), which shows you how to set up a simulation, and is also used in CI testing.

## What is in here?

- The `dueca` folder contains the DUECA core code; modules, channels,
  and some older communication tools. Its `gui` subfolder contains
  graphical interfaces.

- The `extra` folder contains a hodge-podge of helper stuff that may
  be useful in implementing simulations; integration routines, and rigid
  body dynamics simulation, and helper classes for IO conversions and
  implementing calibration and the like.

- `dusime` contains the code supporting simulations.

- The folder `hdf5utils` contains generic logging code with hdf5.

- The folder `udpcom` contains the code that links DUECA executables
  over a network.

- The folder `inter` can (loosely or tightly) connect multiple DUECA processes,
  offering a similar functionality to distributed simulation frameworks like HLA or DIS.
  You choose which channels need to be replicated across the DUECA simulations, and
  optionally whether timing should be synchronized.

- The code generator for communicatable objects is in `pycodegen`.

- With the `websock` addition DUECA can offer websocket communication,
  typically for connecting DUECA to external programs in python or running
  in a browser. In addition the websocket communication modules can provide
  a minimal web server, so you can fetch webpages and (javascript) code.

- The `obs` folder contains build files for rpm-based or deb-based
  distributions. Currently building for Debian, Ubuntu, Fedora and
  OpenSUSE.

- the `ddff` folder contains the code for yet another logging format,
  custom-created for efficiently saving and re-loading streaming
  data. This logging format is used by the `dusime` modules to
  implement record and replay within simulations. This logger can be
  used stand-alone, for any project able to save in msgpack format.
  Python code to convert the files to hdf5 or directly read these files
  is available in the `pyddff` sub-folder. From March 2026 onwards, this
  python code is also available from [PyPi](https://pypi.org/search/?q=pyddff).

## Building and running

DUECA can use, depending on how you configure it, a considerable
number of third party libraries. The main interface is created in GTK,
(versions 2, 3 and 4 currently). The simulation set-up is scripted in
either Python (currently preferred) or Guile (scheme).

When you are looking to use DUECA for your simulation, rather than
work on DUECA's development, the best way to start is with the
packages compiled on the OpenSUSE build service in the
[repabuild](https://build.opensuse.org/project/show/home:repabuild)
project.

For inspecting DUECA itself or development work on the middleware,
start with `linuxtmp.script` or `osxtmp.script` (as it sometimes
runs/builds on OSX with macports), and install dependencies as needed.

Note that by itself, DUECA won't do anything, you need to create your
own application code to make a project. You can check out and run the
DuecaTestCommunication project, but note that that project's only use
is as a test vehicle. The documentation also details how you can make
a project named SimpleSimulation, a "simple" project that demonstrates
joystick input, vehicle dynamcs, a first-person out of the window 3D
view, instrumentation (a compass window with OpenGL), gtk4-based
experiment control windows, distributing your
simulation over multiple executables, simulation state snapshots;
input recording, resetting a snapshot and replay, and multi-player
connection.

The [WorldView](https://github.com/dueca/WorldView) and
[FlexiStick](https://github.com/dueca/FlexiStick) projects offer various
visualization back-ends and interaction with joystick devices
respectively. You can "borrow" modules from these project to integrate
these in your own simulation projects.

In the build actions leading up to new releases, the DuecaTestCommunication and
SimpleSimulation projects are used to check DUECA's code by running
various more-or-less real-time simulations operated by a test script.
After having installed DUECA (through compilation or from packages),
you can start to use it in development of simulation projects.

Please check the documentation for the `dueca-gproject` script. You will need
an "upstream" git repository for your code.

## References

Paassen, M. M. van, Olaf Stroosma, and J. Delatour. “DUECA -
Data-Driven Activation in Distributed Real-Time Computation.” In AIAA
Modeling and Simulation Technologies Conference, 7. Denver (CA):
American Institute of Aeronautics and
Astronautics, 2000. https://doi.org/10.2514/6.2000-4503.

## Acknowledgements

Jerome, for entertaining the initial ideas

Colleagues and students, for feedback on bugs and features

Conchi, Daniel and Alexandra, for sharing their time.
