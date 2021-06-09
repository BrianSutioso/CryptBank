#include <coin_record.h>

CoinRecord::CoinRecord(uint8_t version_,
                       std::vector<uint32_t> utxo_,
                       std::vector<uint32_t> amounts_,
                       std::vector<uint32_t> public_keys_)
    : version(version_), utxo(std::move(utxo_)),
      amounts(std::move(amounts_)),
      public_keys(std::move(public_keys_)) {}

std::string CoinRecord::serialize(const CoinRecord &coin_record)
{
  PCoinRecord pcoin_record = PCoinRecord();
  pcoin_record.set_version(coin_record.version);

  for (int i = 0; i < coin_record.utxo.size(); i++)
  {
    pcoin_record.add_utxo(coin_record.utxo[i]);
    pcoin_record.add_amounts(coin_record.amounts[i]);
    pcoin_record.add_public_keys(coin_record.public_keys[i]);
  }

  std::string serialized_coin_record;
  pcoin_record.SerializeToString(&serialized_coin_record);
  return serialized_coin_record;
}

std::unique_ptr<CoinRecord> CoinRecord::deserialize(const std::string &serialized_coin_record)
{
  PCoinRecord pcoin_record = PCoinRecord();
  pcoin_record.ParseFromString(serialized_coin_record);

  std::vector<uint32_t> utxo;
  std::vector<uint32_t> amounts;
  std::vector<uint32_t> public_keys;

  for (int i = 0; i < pcoin_record.utxo_size(); i++)
  {
    utxo.push_back(pcoin_record.utxo(i));
    amounts.push_back(pcoin_record.amounts(i));
    public_keys.push_back(pcoin_record.public_keys(i));
  }

  std::unique_ptr<CoinRecord> coin_record = std::make_unique<CoinRecord>(
      pcoin_record.version(),
      utxo,
      amounts,
      public_keys);

  return coin_record;
}
