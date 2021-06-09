#include <undo_coin_record.h>

UndoCoinRecord::UndoCoinRecord(
    uint8_t version_,
    std::vector<uint32_t> utxo_, std::vector<uint32_t> amounts_,
    std::vector<uint32_t> public_keys_) : version(version_),
                                          utxo(std::move(utxo_)), amounts(std::move(amounts_)),
                                          public_keys(std::move(public_keys_)) {}
