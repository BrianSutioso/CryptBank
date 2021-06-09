#ifndef CRYPTBANK_CHAIN_WRITER_H
#define CRYPTBANK_CHAIN_WRITER_H

#include <block_record.h>
#include <stdio.h>
#include <mutex>
#include <file_info.h>
#include <undo_block.h>
#include "../primitives/block.h"

class ChainWriter
{
private:
  static const std::string _file_extension;
  static const std::string _data_directory;
  static const std::string _block_filename;
  uint16_t _current_block_file_number;
  uint16_t _current_block_offset;
  static const uint16_t _max_block_file_size;
  static const std::string _undo_filename;
  uint16_t _current_undo_file_number;
  uint16_t _current_undo_offset;
  static const uint16_t _max_undo_file_size;

public:
  ChainWriter();

  static std::string get_data_directory() { return _data_directory; }
  static std::string get_file_extension() { return _file_extension; }
  static std::string get_block_filename() { return _block_filename; }
  static std::string get_undo_filename() { return _undo_filename; }
  static uint16_t get_max_block_file_size() { return _max_block_file_size; }
  static uint16_t get_max_undo_file_size() { return _max_undo_file_size; }

  std::unique_ptr<BlockRecord> store_block(const Block &block, uint32_t height);
  std::unique_ptr<FileInfo> write_block(std::string serialized_block);
  std::unique_ptr<FileInfo> write_undo_block(std::string serialized_block);
  std::string read_block(const FileInfo &block_location);
  std::string read_undo_block(const FileInfo &undo_block_location);

  ChainWriter(ChainWriter &&other) = delete;
  ChainWriter &operator=(const ChainWriter &other) = delete;
};

#endif
