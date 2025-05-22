#include "accounts.h"
#include "ipc.h"

extern int account_sems; // semaphore ID, main.c'de tanımlanacak

void initialize_accounts(Account *accounts, int num_accounts){
  for(int i = 0; i < num_accounts; i++){
    accounts[i].account_number = i;
    accounts[i].balance = 0;
  }
}

void print_final_balances(Account *accounts, int num_accounts){
  printf("Final account balances:\n");
  for(int i = 0; i < num_accounts; i++){
    printf("Account %d: %d\n", accounts[i].account_number, accounts[i].balance);
  }
}

int deposit(Account *accounts, int account_id, int amount){
  semaphore_wait(account_sems, account_id);
  accounts[account_id].balance += amount;
  semaphore_signal(account_sems, account_id);
  return 1;
}

int withdraw(Account *accounts, int account_id, int amount){
  semaphore_wait(account_sems, account_id);
  if(accounts[account_id].balance < amount){
    semaphore_signal(account_sems, account_id);
    return 0; // insufficient funds
  }
  accounts[account_id].balance -= amount;
  semaphore_signal(account_sems, account_id);
  return 1;
}

int transfer(Account *accounts, int from_id, int to_id, int amount){
  int first = from_id < to_id ? from_id : to_id;
  int second = from_id < to_id ? to_id : from_id;
  
  semaphore_wait(account_sems, first);
  semaphore_wait(account_sems, second);
  
  if(accounts[from_id].balance < amount){
    semaphore_signal(account_sems, second);
    semaphore_signal(account_sems, first);
    return 0;
  }
  
  accounts[from_id].balance -= amount;
  accounts[to_id].balance += amount;
  
  semaphore_signal(account_sems, second);
  semaphore_signal(account_sems, first);
  return 1;
}
