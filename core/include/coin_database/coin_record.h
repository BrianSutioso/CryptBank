#ifndef CRYPTBANK_COIN_RECORD_H
#define CRYPTBANK_COIN_RECORD_H

#include <transaction.h>
#include <crypt.pb.h>
#include <string>

class CoinRecord
{

public:
  const uint8_t version;
  std::vector<uint32_t> utxo;
  std::vector<uint32_t> amounts;
  std::vector<uint32_t> public_keys;

  CoinRecord(uint8_t version_,
             std::vector<uint32_t> utxo_, std::vector<uint32_t> amounts_,
             std::vector<uint32_t> public_keys_);

  static std::string serialize(const CoinRecord &coin_record);
  static std::unique_ptr<CoinRecord> deserialize(const std::string &serialized_coin_record);
};

#endif
