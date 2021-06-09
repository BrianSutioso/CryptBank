#include <gtest/gtest.h>
#include <coin_record.h>
#include <coin_database.h>

TEST(CoinLocator, Serialize)
{
  CoinLocator coin_locator = CoinLocator(40, 33);
  std::string serialized_coin_locator = CoinLocator::serialize(coin_locator);
  std::unique_ptr<CoinLocator> deserialized_coin_locator = CoinLocator::deserialize(serialized_coin_locator);
  EXPECT_EQ(coin_locator.output_index, deserialized_coin_locator->output_index);
  EXPECT_EQ(deserialized_coin_locator->output_index, 33);

  std::string serialized_coin_locator2 = CoinLocator::serialize_from_construct(22, 19);
  std::unique_ptr<CoinLocator> deserialized_coin_locator2 = CoinLocator::deserialize(serialized_coin_locator2);
  EXPECT_EQ(deserialized_coin_locator2->output_index, 19);
}

TEST(CoinRecord, Serialize)
{
  std::vector<uint32_t> utxo = {99};
  std::vector<uint32_t> amounts = {12};
  std::vector<uint32_t> public_keys = {14};
  CoinRecord coin_record = CoinRecord(99, std::move(utxo), std::move(amounts),
                                      std::move(public_keys));
  std::string serialized_coin_record = CoinRecord::serialize(coin_record);
  std::unique_ptr<CoinRecord> deserialized_coin_record = CoinRecord::deserialize(serialized_coin_record);
  EXPECT_EQ(coin_record.version, deserialized_coin_record->version);
  EXPECT_EQ(deserialized_coin_record->version, 99);
}
