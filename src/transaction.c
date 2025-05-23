#include "transaction.h"
#include <stdio.h>

int parse_transaction(const char *line, TransactionLog *log, int id){
  char type_char;
  int acc1 = -1, acc2 = -1, amount = 0;
  
  int matched = sscanf(line, "%c %d %d %d", &type_char, &acc1, &acc2, &amount);
  
  log->transaction_id = id;
  log->amount = amount;
  log->success = 0;
  
  switch(type_char){
    case 'D':if(matched != 4) return 0;
      log->type = DEPOSIT;
      log->from_account = -1;      // Deposit işleminde kullanılmaz
      log->to_account = acc1;      // acc1 → hedef hesap
      return 1;
    case 'W':if(matched != 4) return 0;
      log->type = WITHDRAW;
      log->from_account = acc1;    // acc1 → kaynak hesap
      log->to_account = -1;        // Withdraw işleminde kullanılmaz
      return 1;
    case 'T':if(matched != 4) return 0;
      log->type = TRANSFER;
      log->from_account = acc1;    // acc1 → kaynak
      log->to_account = acc2;      // acc2 → hedef
      return 1;
    default:return 0;
  }
}

void log_transaction(TransactionLog *logs, int id, TransactionLog t){
  logs[id] = t;
}

#include <stdio.h>

void print_transaction_log(TransactionLog *logs, int count){
  printf("\n\033[1mTransaction Log:\033[0m\n");
  printf(
      "+----------------+-------------+-----------+-----------+--------+-------------------+\n");
  printf(
      "| Transaction ID | Type        | From      | To        | Amount | Status            |\n");
  printf(
      "+----------------+-------------+-----------+-----------+--------+-------------------+\n");
  
  for(int i = 0; i < count; i++){
    TransactionLog *t = &logs[i];
    
    const char *type_str = "";
    switch(t->type){
      case DEPOSIT: type_str = "Deposit";
        break;
      case WITHDRAW: type_str = "Withdraw";
        break;
      case TRANSFER: type_str = "Transfer";
        break;
    }
    
    // Status coloring
    const char *status_str =
        t->success ? "\033[32mSuccess\033[0m" : "\033[31mFail\033[0m";
    const char *retry_str = t->is_retry ? " \033[33m(Retry)\033[0m" : "";
    
    // FROM / TO hesap numaraları
    char from_buf[16];
    char to_buf[16];
    snprintf(from_buf, sizeof(from_buf), "%s", t->from_account >= 0 ?
                                               (sprintf((char[16]){0}, "%d",
                                                        t->from_account), (char[16]){
                                                   0}) : "-");
    
    snprintf(to_buf, sizeof(to_buf), "%s", t->to_account >= 0 ?
                                           (sprintf((char[16]){0}, "%d",
                                                    t->to_account), (char[16]){
                                               0}) : "-");
    
    // Alternatif güvenli ve net versiyon:
    if(t->from_account >= 0)
      snprintf(from_buf, sizeof(from_buf), "%d", t->from_account);
    else
      snprintf(from_buf, sizeof(from_buf), "-");
    
    if(t->to_account >= 0)
      snprintf(to_buf, sizeof(to_buf), "%d", t->to_account);
    else
      snprintf(to_buf, sizeof(to_buf), "-");
    
    printf("| %-14d | %-11s | %-9s | %-9s | %-6d | %-18s%-8s |\n",
           t->transaction_id,
           type_str,
           from_buf,
           to_buf,
           t->amount,
           status_str,
           retry_str);
  }
  
  printf(
      "+----------------+-------------+-----------+-----------+--------+-------------------+\n");
}
