#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_ACCOUNTS 100
#define MAX_TRANSACTIONS 1000
#define SHM_KEY_ACCOUNTS 0x1234
#define SHM_KEY_LOGS     0x5678
#define SEM_KEY_BASE     0x1111

// Transaction types
typedef enum {
  TRANSFER = 0,
  DEPOSIT = 1,
  WITHDRAW = 2
} TransactionType;

// Account structure
typedef struct {
  int account_number;
  int balance;
} Account;

// Transaction log entry
typedef struct {
  int transaction_id;
  TransactionType type;
  int from_account;
  int to_account;
  int amount;
  int success; // 1 if successful, 0 if failed
} TransactionLog;

#endif // COMMON_H
