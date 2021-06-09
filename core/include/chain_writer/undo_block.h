#ifndef CRYPTBANK_UNDO_BLOCK_H
#define CRYPTBANK_UNDO_BLOCK_H

#include <undo_coin_record.h>
#include <crypt.pb.h>
#include <string>

class UndoBlock
{
public:
  const std::vector<uint32_t> transaction_hashes;
  const std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;

  UndoBlock(std::vector<uint32_t> transaction_hashes_,
            std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records_);

  static std::string serialize(const UndoBlock &undo_block);
  static std::unique_ptr<UndoBlock> deserialize(const std::string &serialized_undo_block);
};

#endif
