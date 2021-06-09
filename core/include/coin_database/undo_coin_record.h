#ifndef CRYPTBANK_UNDO_COIN_RECORD_H
#define CRYPTBANK_UNDO_COIN_RECORD_H

#include <vector>
#include <transaction.h>
class UndoCoinRecord
{
public:
  const uint8_t version;
  std::vector<uint32_t> utxo = {};
  std::vector<uint32_t> amounts = {};
  std::vector<uint32_t> public_keys = {};

  UndoCoinRecord(uint8_t version_,
                 std::vector<uint32_t> utxo_, std::vector<uint32_t> amounts_,
                 std::vector<uint32_t> public_keys_);
};

#endif
