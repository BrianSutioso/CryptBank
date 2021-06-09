#ifndef CRYPTBANK_COIN_H
#define CRYPTBANK_COIN_H

#include <transaction_output.h>

class Coin
{
public:
  std::unique_ptr<TransactionOutput> transaction_output;
  bool is_spent;

  Coin(std::unique_ptr<TransactionOutput> transaction_output_,
       bool is_spent_);
};

#endif
