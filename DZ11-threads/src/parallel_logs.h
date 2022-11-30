#ifndef _PARALLELLOGS_H
#define _PARALLELLOGS_H

#include "file_list.h"
#include "logs.h"

LogsStatistics* run_parallel_logs(FileNode* list_of_files, size_t files_n, size_t n_threads);

#endif //_PARALLELLOGS_H
