#include <coin.h>

Coin::Coin(std::unique_ptr<TransactionOutput> transaction_output_,
           bool is_spent_) : transaction_output(std::move(transaction_output_)), is_spent(is_spent_) {}