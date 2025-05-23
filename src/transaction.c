#include "transaction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

static int parse_int(const char *str, int *valid){
  char *endptr;
  errno = 0;
  long val = strtol(str, &endptr, 10);
  if(errno != 0 || *endptr != '\0' || val < INT_MIN || val > INT_MAX){
    *valid = 0;
    return 0;
  }
  *valid = 1;
  return (int)val;
}

int parse_transaction(const char *line, TransactionLog *log, int id){
  char clean_line[128];
  strncpy(clean_line, line, sizeof(clean_line));
  clean_line[sizeof(clean_line) - 1] = '\0';
  
  size_t len = strlen(clean_line);
  while(len > 0
      && (clean_line[len - 1] == '\n' || clean_line[len - 1] == '\r')){
    clean_line[--len] = '\0';
  }
  
  char *tokens[4];
  char *token = strtok(clean_line, " ");
  int count = 0;
  while(token && count < 4){
    tokens[count++] = token;
    token = strtok(NULL, " ");
  }
  
  if(count != 4) return 0;
  
  char type_char = tokens[0][0];
  int ok1 = 0, ok2 = 0, ok3 = 0;
  
  int a1 = parse_int(tokens[1], &ok1);
  int a2 = parse_int(tokens[2], &ok2);
  int amount = parse_int(tokens[3], &ok3);
  
  if(!(ok1 && ok2 && ok3)) return 0;
  
  log->transaction_id = id;
  log->amount = amount;
  log->success = 0;
  log->is_retry = 0;
  
  switch(type_char){
    case 'D':log->type = DEPOSIT;
      log->from_account = -1;
      log->to_account = a1;
      return 1;
    case 'W':log->type = WITHDRAW;
      log->from_account = a1;
      log->to_account = -1;
      return 1;
    case 'T':log->type = TRANSFER;
      log->from_account = a1;
      log->to_account = a2;
      return 1;
    default:return 0;
  }
}

void log_transaction(TransactionLog *logs, int id, TransactionLog t){
  logs[id] = t;
}

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
    
    const char *status_str =
        t->success ? "\033[32mSuccess\033[0m" : "\033[31mFail\033[0m";
    const char *retry_str = t->is_retry ? " \033[33m(Retry)\033[0m" : "";
    
    char from_buf[16], to_buf[16];
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