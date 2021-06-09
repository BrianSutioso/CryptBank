#include <block_record.h>

BlockRecord::BlockRecord(
    std::unique_ptr<BlockHeader> block_header_,
    uint32_t num_transactions_,
    uint32_t height_,
    const FileInfo &block_info_,
    const FileInfo &undo_info_) : block_header(std::move(block_header_)), height(height_),
                                  num_transactions(num_transactions_),
                                  block_file_stored(block_info_.file_name),
                                  block_offset_start(block_info_.start),
                                  block_offset_end(block_info_.end),
                                  undo_file_stored(undo_info_.file_name),
                                  undo_offset_start(undo_info_.start),
                                  undo_offset_end(undo_info_.end) {}

std::string BlockRecord::serialize(const BlockRecord &block_record)
{
  // Block Header
  PBlockHeader *pblock_header = new PBlockHeader();
  pblock_header->set_version(block_record.block_header->version);
  pblock_header->set_previous_block_hash(block_record.block_header->previous_block_hash);
  pblock_header->set_merkle_root(block_record.block_header->merkle_root);
  pblock_header->set_difficulty_target(block_record.block_header->difficulty_target);
  pblock_header->set_nonce(block_record.block_header->nonce);
  pblock_header->set_timestamp(block_record.block_header->timestamp);

  // PBlockRecord
  PBlockRecord pblock_record = PBlockRecord();
  pblock_record.set_allocated_block_header(pblock_header);
  pblock_record.set_height(block_record.height);
  pblock_record.set_num_transactions(block_record.num_transactions);

  pblock_record.set_block_file_stored(block_record.block_file_stored);
  pblock_record.set_block_offset_start(block_record.block_offset_start);
  pblock_record.set_block_offset_end(block_record.block_offset_end);

  pblock_record.set_undo_file_stored(block_record.undo_file_stored);
  pblock_record.set_undo_offset_start(block_record.undo_offset_start);
  pblock_record.set_undo_offset_end(block_record.undo_offset_end);

  // String
  std::string serialized_block_record;
  pblock_record.SerializeToString(&serialized_block_record);
  return serialized_block_record;
}

std::unique_ptr<BlockRecord> BlockRecord::deserialize(const std::string &serialized_block_record)
{
  PBlockRecord pblock_record = PBlockRecord();
  pblock_record.ParseFromString(serialized_block_record);
  // BlockHeader
  std::unique_ptr<BlockHeader> block_header = std::make_unique<BlockHeader>(pblock_record.block_header().version(),
                                                                            pblock_record.block_header().previous_block_hash(),
                                                                            pblock_record.block_header().merkle_root(),
                                                                            pblock_record.block_header().difficulty_target(),
                                                                            pblock_record.block_header().nonce(),
                                                                            pblock_record.block_header().timestamp());
  // FileInfos
  FileInfo block_file_info = FileInfo(
      pblock_record.block_file_stored(),
      pblock_record.block_offset_start(),
      pblock_record.block_offset_end());

  FileInfo undo_file_info = FileInfo(
      pblock_record.undo_file_stored(),
      pblock_record.undo_offset_start(),
      pblock_record.undo_offset_end());

  // BlockRecord
  std::unique_ptr<BlockRecord> block_record = std::make_unique<BlockRecord>(
      std::move(block_header),
      pblock_record.num_transactions(),
      pblock_record.height(),
      block_file_info,
      undo_file_info);

  return std::move(block_record);
}
