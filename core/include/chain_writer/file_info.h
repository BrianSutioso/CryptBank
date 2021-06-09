
#ifndef CRYPTBANK_FILE_INFO_H
#define CRYPTBANK_FILE_INFO_H

#include <string>

class FileInfo
{
public:
  std::string file_name;
  const uint16_t start;
  const uint16_t end;

  FileInfo(std::string file_name_, uint16_t start_, uint16_t end_);
};

#endif
