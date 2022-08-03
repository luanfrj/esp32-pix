set(srcs "main.c")

idf_component_register(SRCS ${srcs} INCLUDE_DIRS ".")
# idf_component_register(SRCS ${srcs} INCLUDE_DIRS "." EMBED_TXTFILES luan_eng_br.pem)