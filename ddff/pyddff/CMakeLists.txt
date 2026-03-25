# find PyPA build
execute_process(COMMAND ${Python_EXECUTABLE} -c "import build"
  RESULT_VARIABLE PYTHON_HAVE_PYPABUILD)

if(NOT PYTHON_HAVE_PYPABUILD EQUAL 0)
  message(WARNING
    "Prefer python module PyPA build; run python -m pip install build")
endif()

# figure the python major.minor version string
execute_process(COMMAND ${Python_EXECUTABLE} -c
  "import sys; print('.'.join(map(str,sys.version_info[:2])),end='')"
  OUTPUT_VARIABLE PYTHON_MAJMIN)
execute_process(COMMAND ${Python_EXECUTABLE} -c
  "import sys; print(sys.version_info[0],end='')"
  OUTPUT_VARIABLE PYTHON_MAJOR)

# list all python modules and files that create the package
set(DEPS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ddff_convert.py
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ddffbase.py
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ddffinventoried.py
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ddfftagged.py
  ${CMAKE_CURRENT_SOURCE_DIR}/README.md
  ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml
)

# add the target
set(DDFFVERSION 1.0.2)
add_custom_target(
  pyddff ALL
  DEPENDS
  dist/pyddff-${DDFFVERSION}.tar.gz
  dist/pyddff-${DDFFVERSION}-py${PYTHON_MAJOR}-none-any.whl)

# find an installation folder for the Python packages; we know
# pyparsing is installed, since we need it for dco objects
if(NOT PYSITEDIR)
  execute_process(
    COMMAND ${Python_EXECUTABLE} -c
    "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
    OUTPUT_VARIABLE PYSITEDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Calculated python package install as ${PYSITEDIR}")
endif()

set(PYMOD_INSTALL_TYPE bdist_wheel)

# create folder for install if needed
install(DIRECTORY DESTINATION ${PYSITEDIR})

# install the python package, note that "DESTDIR" needs to be kept intact
# for modification at install stage. The flags are:
# --isolated prevents downloads
# --no-deps, dependencies should be in dueca packages
#
# this code gets executed by the build system (ninja, Makefile or the like)
#
# alternative to --target would be
# --root=\$ENV{DESTDIR} --prefix=${CMAKE_INSTALL_PREFIX}
# This does not work on debian-based (install will be in /usr/local)
# but would add any executable scripts
install(CODE "execute_process(COMMAND bash -c \"${Python_EXECUTABLE} -m pip install --isolated --no-index --no-deps --target=\$ENV{DESTDIR}${PYSITEDIR} --find-links=${CMAKE_CURRENT_BINARY_DIR}/dist --upgrade pyddff\")")

# because we use --target, and not --root and --prefix, the script configured
# in pyproject.toml is not automatically installed. need manual version.
configure_file(ddff_convert.py ddff-convert @ONLY)
install(
  PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/ddff-convert
  DESTINATION ${CMAKE_INSTALL_BINDIR})

if(PYTHON_HAVE_PYPABUILD EQUAL 0)
  # simple build with PyPA build, following PEP-517, PEP-518, using
  # setup.cfg, pyproject.toml
  add_custom_command(
    OUTPUT
    dist/pyddff-${DDFFVERSION}.tar.gz
    dist/pyddff-${DDFFVERSION}-py${PYTHON_MAJOR}-none-any.whl

    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/timestamp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}

    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/dist
    COMMAND ${Python_EXECUTABLE} -m build --no-isolation --outdir ${CMAKE_CURRENT_BINARY_DIR}/dist
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/timestamp
    DEPENDS ${DEPS}
  )
else()
  # Alternative build with "conventional" python build tools, with
  # setup.py
  configure_file(setup.py.in setup.py @ONLY)

  add_custom_command(
    OUTPUT
    dist/pyddff-${DDFFVERSION}.tar.gz
    dist/pyddff-${DDFFVERSION}-py${PYTHON_MAJOR}-none-any.whl

    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/timestamp

    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_BINARY_DIR}/README.md

    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src

    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/dist
    COMMAND ${Python_EXECUTABLE} setup.py bdist_wheel
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/timestamp

    DEPENDS ${DEPS} ${CMAKE_CURRENT_BINARY_DIR}/setup.py
  )


endif()

# ddff-convert should have been installed by python install?
