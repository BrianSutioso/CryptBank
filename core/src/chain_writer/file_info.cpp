#include <file_info.h>

FileInfo::FileInfo(std::string file_name_, uint16_t start_,
                   uint16_t end_) : file_name(file_name_),
                                    start(start_), end(end_) {}