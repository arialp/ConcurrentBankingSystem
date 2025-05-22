#include "transaction.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int parse_transaction(const char *line, TransactionLog *log, int id){
  char type_char;
  int from = -1, to = -1, amount = 0;
  int matched = sscanf(line, "%c %d %d %d", &type_char, &from, &to, &amount);
  
  log->transaction_id = id;
  log->from_account = from;
  log->to_account = to;
  log->amount = amount;
  log->success = 0;
  
  switch(type_char){
    case 'D': log->type = DEPOSIT;
      return matched == 4;
    case 'W': log->type = WITHDRAW;
      return matched == 4;
    case 'T': log->type = TRANSFER;
      return matched == 4;
    default: return 0;
  }
}

void log_transaction(TransactionLog *logs, int id, TransactionLog t){
  logs[id] = t;
}

void print_transaction_log(TransactionLog *logs, int count){
  printf("\nTransaction Log:\n");
  for(int i = 0; i < count; i++){
    TransactionLog *t = &logs[i];
    const char *result = t->success ? "Success" : "Failed";
    
    switch(t->type){
      case DEPOSIT:
        printf("Transaction %d: Deposit %d to Account %d (%s)\n",
               t->transaction_id, t->amount, t->to_account, result);
        break;
      case WITHDRAW:
        printf("Transaction %d: Withdraw %d from Account %d (%s)\n",
               t->transaction_id, t->amount, t->from_account, result);
        break;
      case TRANSFER:
        printf(
            "Transaction %d: Transfer %d from Account %d to Account %d (%s)\n",
            t->transaction_id, t->amount, t->from_account, t->to_account,
            result);
        break;
    }
  }
}
