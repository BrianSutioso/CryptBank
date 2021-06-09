#include <undo_block.h>

UndoBlock::UndoBlock(
    std::vector<uint32_t> transaction_hashes_,
    std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records_)
    : transaction_hashes(std::move(transaction_hashes_)),
      undo_coin_records(std::move(undo_coin_records_)) {}

std::string UndoBlock::serialize(const UndoBlock &undo_block)
{
  PUndoBlock pundo_block = PUndoBlock();
  PUndoCoinRecord *pundo_coin_record;
  for (int i = 0; i < undo_block.transaction_hashes.size(); i++)
  {
    pundo_block.add_transaction_hashes(undo_block.transaction_hashes[i]);
    // handle undo_coin_record
    pundo_coin_record = pundo_block.add_undo_coin_records();
    pundo_coin_record->set_version(undo_block.undo_coin_records[i]->version);
    for (int j = 0; j < undo_block.undo_coin_records[i]->utxo.size(); j++)
    {
      pundo_coin_record->add_utxo(undo_block.undo_coin_records[i]->utxo[j]);
      pundo_coin_record->add_amounts(undo_block.undo_coin_records[i]->amounts[j]);
      pundo_coin_record->add_public_keys(undo_block.undo_coin_records[i]->public_keys[j]);
    }
  }

  std::string serialized_undo_block;
  pundo_block.SerializeToString(&serialized_undo_block);
  return serialized_undo_block;
}

std::unique_ptr<UndoBlock> UndoBlock::deserialize(const std::string &serialized_undo_block)
{
  PUndoBlock pundo_block = PUndoBlock();

  pundo_block.ParseFromString(serialized_undo_block);

  std::vector<uint32_t> transaction_hashes;
  std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;

  for (int i = 0; i < pundo_block.transaction_hashes_size(); i++)
  {
    transaction_hashes.push_back(pundo_block.transaction_hashes(i));
    PUndoCoinRecord pundo_coin_record = pundo_block.undo_coin_records(i);
    // handle UndoCoinRecord
    std::vector<uint32_t> utxo;
    std::vector<uint32_t> amounts;
    std::vector<uint32_t> public_keys;
    for (int j = 0; j < pundo_coin_record.utxo_size(); j++)
    {
      utxo.push_back(pundo_coin_record.utxo(j));
      amounts.push_back(pundo_coin_record.amounts(j));
      public_keys.push_back(pundo_coin_record.public_keys(j));
    }
    std::unique_ptr<UndoCoinRecord> a = std::make_unique<UndoCoinRecord>(pundo_coin_record.version(),
                                                                         utxo,
                                                                         amounts,
                                                                         public_keys);
    undo_coin_records.push_back(std::move(a));
  }
  std::unique_ptr<UndoBlock> undo_block = std::make_unique<UndoBlock>(std::move(transaction_hashes), std::move(undo_coin_records));
  return std::move(undo_block);
}
