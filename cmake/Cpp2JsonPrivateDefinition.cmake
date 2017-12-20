function(cpp2json_private_stuff targetName visibility)
# Define tags used by each annotation.
target_compile_definitions(${targetName} ${visibility}
                           CPP2JSON_EXCLUDE_TAG="cpp2json_exclude_definition"
                           CPP2JSON_ARRAY_TAG="cpp2json_considere_as_array"
)
endfunction(cpp2json_private_stuff)
