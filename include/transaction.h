#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "common.h"

int parse_transaction(const char *line, TransactionLog *log, int id);
void log_transaction(TransactionLog *logs, int id, TransactionLog t);
void print_transaction_log(TransactionLog *logs, int count);

#endif // TRANSACTION_H
