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
  
  TransactionLog temp_logs[MAX_TRANSACTIONS];
  pid_t child_pids[MAX_TRANSACTIONS];
  int retry_indices[MAX_TRANSACTIONS];
  int txn_id = 0;
  char line[128];
  
  // 1. Read and fork all transactions
  while(fgets(line, sizeof(line), fp) && txn_id < num_transactions){
    if(!parse_transaction(line, &temp_logs[txn_id], txn_id)){
      fprintf(stderr, "Invalid transaction format on line %d\n", txn_id + 1);
      continue;
    }
    
    pid_t pid = fork();
    if(pid == 0){
      TransactionLog *t = &temp_logs[txn_id];
      int status = 0;
      switch(t->type){
        case DEPOSIT: status = deposit(accounts, t->to_account, t->amount);
          break;
        case WITHDRAW: status = withdraw(accounts, t->from_account, t->amount);
          break;
        case TRANSFER: status =
                           transfer(accounts, t->from_account, t->to_account,
                                    t->amount);
          break;
      }
      exit(status ? 0 : -1);
    } else {
      child_pids[txn_id] = pid;
    }
    txn_id++;
  }
  
  fclose(fp);
  
  int log_index = 0;
  int retry_count = 0;
  
  // 2. Wait and record results
  for(int i = 0; i < txn_id; i++){
    int status;
    waitpid(child_pids[i], &status, 0);
    
    temp_logs[i].transaction_id = log_index;
    if(WIFEXITED(status) && WEXITSTATUS(status) == 0){
      temp_logs[i].success = 1;
      temp_logs[i].is_retry = 0;
      log_transaction(logs, log_index++, temp_logs[i]);
    } else {
      temp_logs[i].success = 0;
      temp_logs[i].is_retry = 0;
      log_transaction(logs, log_index++, temp_logs[i]);
      retry_indices[retry_count++] = i;
    }
  }
  
  // 3. Retry failed transactions
  for(int j = 0; j < retry_count; j++){
    int i = retry_indices[j];
    TransactionLog retry_log = temp_logs[i];
    
    pid_t retry_pid = fork();
    if(retry_pid == 0){
      int status = 0;
      switch(retry_log.type){
        case DEPOSIT: status = deposit(accounts, retry_log.to_account,
                                       retry_log.amount);
          break;
        case WITHDRAW: status = withdraw(accounts, retry_log.from_account,
                                         retry_log.amount);
          break;
        case TRANSFER: status = transfer(accounts, retry_log.from_account,
                                         retry_log.to_account,
                                         retry_log.amount);
          break;
      }
      exit(status ? 0 : -1);
    } else {
      int retry_status;
      waitpid(retry_pid, &retry_status, 0);
      retry_log.success =
          (WIFEXITED(retry_status) && WEXITSTATUS(retry_status) == 0);
      retry_log.is_retry = 1;
      retry_log.transaction_id = temp_logs[i].transaction_id; // same ID
      log_transaction(logs, log_index++, retry_log);
    }
  }
  
  // 4. Output results
  print_final_balances(accounts, num_accounts);
  print_transaction_log(logs, log_index);
  
  // 5. Cleanup
  detach_shared_memory(accounts);
  detach_shared_memory(logs);
  destroy_shared_memory(shm_accounts);
  destroy_shared_memory(shm_logs);
  destroy_semaphore_set(account_sems);
  
  return 0;
}
