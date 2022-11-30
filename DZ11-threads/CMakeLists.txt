project(logs_parser C)

set(SRC
	src/file_list.h         src/file_list.c
    src/combined_log.h      src/combined_log.c
    src/file_with_mutex.h   src/file_with_mutex.c
    src/logs.h             	src/logs.c
    src/parallel_logs.h     src/parallel_logs.c
	hash_map/hash_map.h		hash_map/hash_map.c
	hash_map/hash.h			hash_map/hash.c)

add_executable(logs_parser src/main.c ${SRC})
target_link_libraries(logs_parser PRIVATE -pthread)
