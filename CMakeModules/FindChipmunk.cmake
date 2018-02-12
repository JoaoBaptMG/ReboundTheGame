# Locate Chipmunk physics library
# This module defines
# CHIPMUNK_FOUND, if false, do not try to link to Lua 
# CHIPMUNK_LIBRARIES
# CHIPMNK_INCLUDE_DIR, where to find chipmunk.h 
#
# Note that the expected include convention is
#  #include <chipmunk.h>
# and not
#  #include <chipmunk/chipmunk.h>
# This is because, the chipmunk location is not standardized and may exist
# in locations other than chipmunk/

#=============================================================================
# Copyright 2012, Beoran beoran@rubyforge.org.
#
# License: you just do whatever you want with this file. 
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#=============================================================================

FIND_PATH(CHIPMUNK_INCLUDE_DIR chipmunk.h
  HINTS
  $ENV{CHIPMUNK_DIR}
  PATH_SUFFIXES include/chipmunk include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

FIND_LIBRARY(CHIPMUNK_LIBRARY 
  NAMES chipmunk
  HINTS
  $ENV{CHIPMUNK_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

IF(CHIPMUNK_LIBRARY)
  # include the math library for Unix
  IF(UNIX AND NOT APPLE)
    FIND_LIBRARY(CHIPMUNK_MATH_LIBRARY m)
    SET(CHIPMUNK_LIBRARIES "${CHIPMUNK_LIBRARY};${CHIPMUNK_MATH_LIBRARY}" CACHE STRING "Chipmunk Libraries")
  # For Windows and Mac, don't need to explicitly include the math library
  ELSE(UNIX AND NOT APPLE)
    SET( CHIPMUNK_LIBRARIES "${CHIPMUNK_LIBRARY}" CACHE STRING "Chipmunk Libraries")
  ENDIF(UNIX AND NOT APPLE)
ENDIF(CHIPMUNK_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CHIPMUNK_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Chipmunk DEFAULT_MSG CHIPMUNK_LIBRARIES CHIPMUNK_INCLUDE_DIR)

MARK_AS_ADVANCED(CHIPMUNK_INCLUDE_DIR CHIPMUNK_LIBRARIES CHIPMUNK_LIBRARY CHIPMUNK_MATH_LIBRARY)
