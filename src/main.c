#include "common.h"
#include "ipc.h"
#include "accounts.h"
#include "transaction.h"

int account_sems; // Global semaphore ID

int main(int argc, char *argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <transaction_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  FILE *fp = fopen(argv[1], "r");
  if(!fp){
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  
  int num_accounts, num_transactions;
  fscanf(fp, "%d %d\n", &num_accounts, &num_transactions);
  
  // Shared memory setup
  int shm_accounts =
      create_shared_memory(SHM_KEY_ACCOUNTS, sizeof(Account) * MAX_ACCOUNTS);
  int shm_logs = create_shared_memory(SHM_KEY_LOGS, sizeof(TransactionLog)
      * MAX_TRANSACTIONS);
  
  Account *accounts = (Account *)attach_shared_memory(shm_accounts);
  TransactionLog *logs = (TransactionLog *)attach_shared_memory(shm_logs);
  
  initialize_accounts(accounts, num_accounts);
  
  // Init account balances
  for(int i = 0; i < num_accounts; i++){
    int acc_num, bal;
    fscanf(fp, "%d %d\n", &acc_num, &bal);
    accounts[acc_num].balance = bal;
  }
  
  // Init semaphores
  account_sems = create_semaphore_set(SEM_KEY_BASE, num_accounts);
  for(int i = 0; i < num_accounts; i++){
    init_semaphore(account_sems, i, 1);
  }
  
  char line[128];
  int txn_id = 0;
  while(fgets(line, sizeof(line), fp) && txn_id < num_transactions){
    TransactionLog t;
    if(!parse_transaction(line, &t, txn_id)){
      fprintf(stderr, "Invalid transaction format on line %d\n", txn_id + 1);
      continue;
    }
    
    pid_t pid = fork();
    if(pid == 0){ // child
      int status = 0;
      switch(t.type){
        case DEPOSIT:status = deposit(accounts, t.to_account, t.amount);
          break;
        case WITHDRAW:status = withdraw(accounts, t.from_account, t.amount);
          break;
        case TRANSFER:status = transfer(accounts, t.from_account, t.to_account,
                                        t.amount);
          break;
      }
      exit(status ? 0 : -1);
    } else {
      int stat;
      waitpid(pid, &stat, 0);
      if(WIFEXITED(stat) && WEXITSTATUS(stat) == 0){
        t.success = 1;
      } else {
        // Retry once if failed
        pid_t retry_pid = fork();
        if(retry_pid == 0){
          int status = 0;
          switch(t.type){
            case DEPOSIT:status = deposit(accounts, t.to_account, t.amount);
              break;
            case WITHDRAW:status = withdraw(accounts, t.from_account, t.amount);
              break;
            case TRANSFER:status =
                              transfer(accounts, t.from_account, t.to_account,
                                       t.amount);
              break;
          }
          exit(status ? 0 : -1);
        } else {
          waitpid(retry_pid, &stat, 0);
          t.success = WIFEXITED(stat) && WEXITSTATUS(stat) == 0;
        }
      }
      log_transaction(logs, txn_id, t);
      txn_id++;
    }
  }
  
  fclose(fp);
  
  print_final_balances(accounts, num_accounts);
  print_transaction_log(logs, txn_id);
  
  // Cleanup
  detach_shared_memory(accounts);
  detach_shared_memory(logs);
  destroy_shared_memory(shm_accounts);
  destroy_shared_memory(shm_logs);
  destroy_semaphore_set(account_sems);
  
  return 0;
}
