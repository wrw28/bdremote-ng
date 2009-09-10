function(add_splint NAME FILE)

  get_directory_property(DIRPROP INCLUDE_DIRECTORIES)

  message(STATUS "Adding split for ${NAME}, ${FILE}.")

  add_custom_command(TARGET ${NAME}
                     POST_BUILD
		     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     COMMAND splint +weak -sysdirerrors +unixlib -I${DIRPROP} ${FILE}
		     VERBATIM
  		     )

endfunction(add_splint)
