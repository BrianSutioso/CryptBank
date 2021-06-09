#ifndef CRYPTBANK_BLOCK_RECORD_H
#define CRYPTBANK_BLOCK_RECORD_H

#include <file_info.h>
#include <block_header.h>
#include <crypt.pb.h>
#include <string>

class BlockRecord
{
public:
  const std::unique_ptr<BlockHeader> block_header;
  const uint32_t height;
  const uint32_t num_transactions;
  const std::string block_file_stored;
  const std::string undo_file_stored;
  uint16_t block_offset_start;
  uint16_t block_offset_end;
  uint16_t undo_offset_start;
  uint16_t undo_offset_end;

  BlockRecord(std::unique_ptr<BlockHeader> block_header_,
              uint32_t num_transactions_, uint32_t height_,
              const FileInfo &block_info_, const FileInfo &undo_info_);

  static std::string serialize(const BlockRecord &block_record);
  static std::unique_ptr<BlockRecord> deserialize(
      const std::string &serialized_block_record);
};

#endif
