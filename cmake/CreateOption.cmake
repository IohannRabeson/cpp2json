#
#	Define somes macros
#
macro(create_option type var default docstring)
	if(NOT DEFINED ${var})
		set(${var} ${default})
	endif()
	set(${var} ${${var}} CACHE ${type} ${docstring})
endmacro()

macro(create_strings_option var default docstring possibles)
	if(NOT DEFINED ${var})
		set(${var} ${default})
	endif()
	set(${var} ${${var}} CACHE STRING ${docstring})
	set_property(CACHE ${var} PROPERTY STRINGS ${possibles})
endmacro()
