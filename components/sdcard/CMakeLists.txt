set(component_srcs "sdcard.c")

idf_component_register(SRCS "${component_srcs}" 
	PRIV_REQUIRES fatfs
	INCLUDE_DIRS ".")
