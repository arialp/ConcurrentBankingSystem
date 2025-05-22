#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include "common.h"

void initialize_accounts(Account *accounts, int num_accounts);
void print_final_balances(Account *accounts, int num_accounts);

int deposit(Account *accounts, int account_id, int amount);
int withdraw(Account *accounts, int account_id, int amount);
int transfer(Account *accounts, int from_id, int to_id, int amount);

#endif // ACCOUNTS_H