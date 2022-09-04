#[=======================================================================[.rst:
FindVorbis
----------

Finds the native vorbis, vorbisenc amd vorbisfile includes and libraries.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Vorbis::vorbis``
  The Vorbis library
``Vorbis::vorbisenc``
  The VorbisEnc library
``Vorbis::vorbisfile``
  The VorbisFile library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``VORBIS_INCLUDE_DIRS``
  List of include directories when using vorbis.
``VORBIS_Enc_INCLUDE_DIRS``
  List of include directories when using vorbisenc.
``VORBIS_File_INCLUDE_DIRS``
  List of include directories when using vorbisfile.
``VORBIS_LIBRARIES``
  List of libraries when using vorbis.
``VORBIS_Enc_LIBRARIES``
  List of libraries when using vorbisenc.
``VORBIS_File_LIBRARIES``
  List of libraries when using vorbisfile.
``Vorbis_FOUND``
  True if vorbis and requested components found.
``Vorbis_FOUND``
  True if vorbis found.
``VORBIS_Enc_FOUND``
  True if vorbisenc found.
``VORBIS_Enc_FOUND``
  True if vorbisfile found.

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``VORBIS_INCLUDE_DIR``
  The directory containing ``vorbis/vorbis.h``.
``VORBIS_Enc_INCLUDE_DIR``
  The directory containing ``vorbis/vorbisenc.h``.
``VORBIS_File_INCLUDE_DIR``
  The directory containing ``vorbis/vorbisenc.h``.
``VORBIS_LIBRARY``
  The path to the vorbis library.
``VORBIS_Enc_LIBRARY``
  The path to the vorbisenc library.
``VORBIS_File_LIBRARY``
  The path to the vorbisfile library.

Hints
^^^^^

A user may set ``VORBIS_ROOT`` to a vorbis installation root to tell this module where to look.

#]=======================================================================]

if (VORBIS_INCLUDE_DIR)
	# Already in cache, be silent
	set (VORBIS_FIND_QUIETLY TRUE)
endif ()

set (VORBIS_FIND_QUIETLY TRUE)
set (VORBIS_Enc_FIND_QUIETLY TRUE)
set (VORBIS_File_FIND_QUIETLY TRUE)

find_package (Ogg QUIET)

find_package (PkgConfig QUIET)
pkg_check_modules (PC_VORBIS_Vorbis QUIET vorbis)
pkg_check_modules (PC_VORBIS_Enc QUIET vorbisenc)
pkg_check_modules (PC_VORBIS_File QUIET vorbisfile)

set (VORBIS_VERSION ${PC_VORBIS_VERSION})

find_path (VORBIS_INCLUDE_DIR vorbis/codec.h
	HINTS
		${PC_VORBIS_INCLUDEDIR}
		${PC_VORBIS_INCLUDE_DIRS}
		${VORBIS_ROOT}
	)

find_path (VORBIS_Enc_INCLUDE_DIR vorbis/vorbisenc.h
	HINTS
		${PC_VORBIS_Enc_INCLUDEDIR}
		${PC_VORBIS_Enc_INCLUDE_DIRS}
		${VORBIS_ROOT}
	)

find_path (VORBIS_File_INCLUDE_DIR vorbis/vorbisfile.h
	HINTS
		${PC_VORBIS_File_INCLUDEDIR}
		${PC_VORBIS_File_INCLUDE_DIRS}
		${VORBIS_ROOT}
	)

find_library (VORBIS_LIBRARY
	NAMES
		vorbis
		vorbis_static
		libvorbis
		libvorbis_static
	HINTS
		${PC_VORBIS_LIBDIR}
		${PC_VORBIS_LIBRARY_DIRS}
		${VORBIS_ROOT}
	)

find_library (VORBIS_Enc_LIBRARY
	NAMES
		vorbisenc
		vorbisenc_static
		libvorbisenc
		libvorbisenc_static
	HINTS
		${PC_VORBIS_Enc_LIBDIR}
		${PC_VORBIS_Enc_LIBRARY_DIRS}
		${VORBIS_ROOT}
	)

find_library (VORBIS_File_LIBRARY
	NAMES
		vorbisfile
		vorbisfile_static
		libvorbisfile
		libvorbisfile_static
	HINTS
		${PC_VORBIS_File_LIBDIR}
		${PC_VORBIS_File_LIBRARY_DIRS}
		${VORBIS_ROOT}
	)

include (FindPackageHandleStandardArgs)

if (VORBIS_LIBRARY AND VORBIS_INCLUDE_DIR AND Ogg_FOUND)
    set (Vorbis_FOUND TRUE)
endif ()

if (VORBIS_Enc_LIBRARY AND VORBIS_Enc_INCLUDE_DIR AND Vorbis_FOUND)
    set (VORBIS_Enc_FOUND TRUE)
endif ()

if (Vorbis_FOUND AND VORBIS_File_LIBRARY AND VORBIS_File_INCLUDE_DIR)
    set (VORBIS_File_FOUND TRUE)
endif ()

find_package_handle_standard_args (Vorbis
	REQUIRED_VARS
		VORBIS_LIBRARY
		VORBIS_INCLUDE_DIR
		Ogg_FOUND
	HANDLE_COMPONENTS
	VERSION_VAR VORBIS_VERSION)


if (Vorbis_FOUND)
	set (VORBIS_INCLUDE_DIRS ${VORBIS_INCLUDE_DIR})
	set (VORBIS_LIBRARIES ${VORBIS_LIBRARY} ${OGG_LIBRARIES})
    if (NOT TARGET Vorbis::vorbis)
		add_library (Vorbis::vorbis UNKNOWN IMPORTED)
		set_target_properties (Vorbis::vorbis PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${VORBIS_INCLUDE_DIR}"
			IMPORTED_LOCATION "${VORBIS_LIBRARY}"
			INTERFACE_LINK_LIBRARIES Ogg::ogg
		)
	endif ()

	if (VORBIS_Enc_FOUND)
		set (VORBIS_Enc_INCLUDE_DIRS ${VORBIS_Enc_INCLUDE_DIR})
		set (VORBIS_Enc_LIBRARIES ${VORBIS_Enc_LIBRARY} ${VORBIS_Enc_LIBRARIES})
		list(APPEND VORBIS_LIBRARIES ${VORBIS_Enc_LIBRARY})
		if (NOT TARGET Vorbis::vorbisenc)
			add_library (Vorbis::vorbisenc UNKNOWN IMPORTED)
			set_target_properties (Vorbis::vorbisenc PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${VORBIS_Enc_INCLUDE_DIR}"
				IMPORTED_LOCATION "${VORBIS_Enc_LIBRARY}"
				INTERFACE_LINK_LIBRARIES Vorbis::vorbis
			)
		endif ()
	endif ()

	if (VORBIS_File_FOUND)
		set (VORBIS_File_INCLUDE_DIRS ${VORBIS_File_INCLUDE_DIR})
		set (VORBIS_File_LIBRARIES ${VORBIS_File_LIBRARY} ${VORBIS_File_LIBRARIES})
		list(APPEND VORBIS_LIBRARIES ${VORBIS_File_LIBRARY})
		if (NOT TARGET Vorbis::vorbisfile)
			add_library (Vorbis::vorbisfile UNKNOWN IMPORTED)
			set_target_properties (Vorbis::vorbisfile PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${VORBIS_File_INCLUDE_DIR}"
				IMPORTED_LOCATION "${VORBIS_File_LIBRARY}"
				INTERFACE_LINK_LIBRARIES Vorbis::vorbis
			)
		endif ()
	endif ()

endif ()

mark_as_advanced (VORBIS_INCLUDE_DIR VORBIS_LIBRARY)
mark_as_advanced (VORBIS_Enc_INCLUDE_DIR VORBIS_Enc_LIBRARY)
mark_as_advanced (VORBIS_File_INCLUDE_DIR VORBIS_File_LIBRARY)
