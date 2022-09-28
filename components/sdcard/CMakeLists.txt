set(component_srcs "sdcard.c")

idf_component_register(SRCS "${component_srcs}" 
	INCLUDE_DIRS ".")
