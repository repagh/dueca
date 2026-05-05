# Creating runtest configuration files {#runtest}

## Introduction

DUECA is a framework for real-time simulation programs, consisting of a
library and supporting scripts and programs. To test the library before
a new release is drafted, a number of tests are defined. Some of these
tests involve checking out and building DUECA projects, and running these.

To automate this process, a Python script has been created that can run
the check-out, do the compile, then run the resulting DUECA project, and
interact with the project windows, clicking buttons and the like. The success
in operating the DUECA windows and the build and run processes determines
success of the test.

This section is meant for developers and testers who want to help out in
creating or updating the test cases.

## Development environment

Creating or modifying test scenarios requires a development environment. You
will need a number of extra dependencies:

- xmlstarlet - manipulation of XML files
- wmctrl - a program for window interaction under X windows
- openbox - window manager
- Xnest - Nested X server for capturing or viewing interaction with your
  DUECA program
- Xvfb - Headless X server for running the tests

To get the development environment

- Use git to clone (your fork of) the DUECA sources
- Use or adapt the `linuxtmp.script` script to configure the test build
- Build and install it

From the `build-linux` folder you can now run the tests, view the tests, or
create new tests. To run the configured tests:

~~~~{.sh}
ninja test/runtest/test
~~~~

After such a run, the file `test/runtest/Testing/Temporary/LastTest.log contains
a log of all the tests. In test/runtest, you can find screenshots of the DUECA
program, if these were configured in the test script, and any screenshots of
failures to find items in the interface.

## The `testrunner.py` script

The script running the tests reads a scenario file with the test scenario and
then does the following:

- The image templates (sections of the interface), that are used to check the
  state of the interface are loaded at the start of the script.
- Based on a repository, project name and build options, it will check out the
  project code, and build it.
- Based on a platform name and involved nodes, the `links.script` files there
  are sourced to prepare the run folders.
- DUECA (or any script or program if so configured) is started on each node.
- Then the script runs all its actions:
  - Matching the given window or window areas to template images.
  - Checking for specific colors at certain locations.
  - Pressing and releasing buttons at certain locations, absolute or either
    relative to the results/position of the previous check.
  - Perform key presses.
  - Take screenshots at appropriate moments.
- After a run, the printout from the script actions, and the outputs of the
  different DUECA processes are given, when running the script from a test,
  this will end up in the log.

The script is not run directly, but is typically run in one of three modes:

- When part of the CMake test suite, the script is invoked through the
  automatically generated script files in the `build-linux/test/runtest`
  folder, you can do a manual run of these by invoking for example:

  ~~~~{.sh}
  cd build-linux/test/runtest
  ./test_run_tmssolo
  cd ../../..
  ~~~~

  This uses the `Xvfb` server, which provides a full X environment, but does not
  have a visual output.

- When you want to inspect a test scenario, you can run it with the `viewrun`
  script, from the `build-linux` folder, for example run:

  ~~~~{.sh}
  ./test/viewrun ../test/runtest/tmsudptwo.xml
  ~~~~

  Output appears on the terminal and is visible in an X window, and any
  screenshots made, either regular as specified in the script, or after the
  script fails to detect interface changes, are saved to the current folder.
  This currently uses the Xephyr server.

- When you want to create a new script, you can use the `createrun` command for
  a DUECA project build with a `gtk3` interface, or the `createrun_gtk4` command
  for a `gtk4` interface.

## Creating a new scenario script

To create a new scenario script, you need an initial xml file with the project
definition, build and run instructions. Because such files sometimes need to
be regenerated, the DUECA build process will create "cleaned" versions of
the scenario scripts if you have the `xmlstarlet` program installed. These are
created in the `build-linux` folder. For example
to re-create the `simplesimulation` script, do the following:

~~~~{.sh}
cp simplesimulation-clean.xml simplesimulation.xml
./test/createrun simplesimulation.xml
~~~~

For new test scenarios, you can adapt one of the existing cleaned scenarios,
detailing the project name, repository, run folders and start commands.

The `createrun` script will use the `testrunner.py` to perform its actions.
First the given project is checked out and compiled, then an X window comes up.
The DUECA executable or other scripts, as specified in the cleaned start script,
are started, usually producing a DUECA interface. Keep an eye on the terminal
from where you started createrun, to see the program's reactions. You can now
interact with the DUECA program to operate it, and use function keys to insert
checks or specific actions. Key clicks and keyboard presses will be monitored
and coded in the xml file.

A good strategy for creating a scenario is to check before you act. The `gtk3`
and `gtk4` folders under `test/runtest` contain a number of small images that
can be recognised by the script. To start, move the mouse near DUECA's first
green button (it should become green). Press f4, f5, f6 or f7. Each of these
buttons will grab an area around the mouse's current location (the sizes differ,
respectively 48, 64, 96 and 140 pixels), and figures out which of the template
files in the `gtk3` (or `gtk4`) folder best matches. The terminal will spit out
a message about the chosen file, or a list of files tried if no good match is
found.

For words, such as in file menus, you can now also use tesseract character
recognition to create a check. Function keys f8, f9 and f10 produce such a
test recognition check, in areas of 64, 96 and 140 pixels. The recognized
word closest to the cursor is chosen, and recorded for checking when playing
a scenario.

The recording can be done in relative mode or in absolute mode. In absolute
mode, each click will be within absolute coordinates, measured from the top
left of the window in which the click is done. In relative mode, the clicks are
performed relative to the location where you performed the last colour or image
detection. In this way, if elements in the GUI shift a little, the actions shift
with them. Recording starts in relative mode, by pressing f12 it will enter
absolute mode (which will be good for clicks close to the top left of the
window), and f11 enters relative again.

An alternative way of checking the interface is by checking for a specific
colour. Use f1 to do this.

To insert a snapshot of the screen, press f2.

After completing the scenario, press the escape key, and perform a mouse
click. The window will close and the updated xml file will be written.

## Fixing up and fixing failures

After a run with a `createrun` script, you might tweak the scenario. Typically,
you may increase the wait times before detections or clicks, to let the program
run a little.

To view the result, invoke the `test/viewrun` script with the created or updated
scenario file. You can follow along, and see the actions in the terminal where
you started the view. In cases where an image may not be recognized, the program
will print out a message and produce a screenshot, with the search area marked
in red.

Typically, different distributions of the linux operating system, or different
installed fonts, may produce slightly different interfaces. To accommodate this,
you can take the screenshot from a failed test, cut out the word or button that
should have been recognized, and save it as an additional version of that image,
by separating base name and version by an underscore, for example if "File" was
not recognize as matching the image `word-file.png`, you can save the alternate
image as `word-file_v2.png`, and add it to the DUECA source. Note that, for
words, using OCR is usually a better strategy.

The differences appear to be quite subtle. Although the environment in which
the DUECA interfaces run is controlled as much as possible, there are
differences between different environments, such as:

- Development on your chosen Linux distribution (e.g., Fedora)
- Local runs using `act`, nominally with the Ubuntu distribution also used on
  the CI runs, with Xvfb.
- Runs on github, with github actions, should be the same Xvfb and Ubuntu
  distribution as with `act`, but there still appear to be differences.

To fix this, you can take the screenshots produced after failed matches, and
cut out with gimp or a similar program, the template files that the runner
should recognize.
