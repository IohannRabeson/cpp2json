#
#	Copy files at build time.
#
#	dest_dir: define the destination directory path
#	target: define the parent target
#	files_list: define the list of files to be copied (don't forget to quote this param)
#
#	The copy is performed only if the destination file is not the same as the source
#	file. Each copy command is defined as custom target and add as dependency of the parent
#	target.
#
function(copy_files dest_dir target files_list)
	foreach(file_to_copy ${files_list})
		get_filename_component(file_name_only ${file_to_copy} NAME)
		get_filename_component(cleaned_target ${target} NAME_WE)
		set(dest_path ${dest_dir}/${file_name_only})
		string(MAKE_C_IDENTIFIER "${cleaned_target}_${file_name_only}" copy_target)
		add_custom_target(${copy_target}
						  ${CMAKE_COMMAND} -E
						  copy_if_different ${file_to_copy} ${dest_path}
						  VERBATIM)
		add_dependencies(${target} ${copy_target})
	endforeach()
endfunction(copy_files)
