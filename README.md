# Concurrent Banking System

This project implements a concurrent banking simulation using process creation, shared memory, semaphores, and basic inter-process communication in C.

---

## 🧰 Requirements

To build and run the project, you will need:

- **C Compiler** (e.g., `gcc`)
- **CMake** ≥ 3.30.5
- **POSIX-compliant system** (Linux/macOS)
- The following system libraries (typically preinstalled on Linux):
  - `unistd.h`
  - `stdio.h`
  - `stdlib.h`
  - `string.h`
  - `errno.h`
  - `sys/types.h`
  - `sys/ipc.h`
  - `sys/shm.h`
  - `sys/sem.h`
  - `sys/wait.h`

---

## ⚙️ Build Instructions

1. Clone the repository and navigate into it.
2. Create a build directory and compile the project:

```bash
mkdir build
cd build
cmake ..
make
```

The binary will be available at: `build/bin/banking_system`

---

## ▶️ Running the Program

Prepare a transaction file like `transactions.txt`, then run:

```bash
./bin/banking_system ../transactions/transactions.txt
```

---

## 📄 Input File Format (`transactions.txt`)

```
<num_accounts> <num_transactions>
<account_id> <initial_balance>
...
<transaction_line>
```

### Transaction line format:
- `D <account_id> 0 <amount>` – Deposit
- `W <account_id> 0 <amount>` – Withdraw
- `T <from_account_id> <to_account_id> <amount>` – Transfer

Example:
```
5 3
0 500
1 600
2 700
3 800
4 900
D 0 0 100
T 2 3 300
W 1 0 200
```

---

## ✅ Output

Program prints:

- Final balances for all accounts
- A detailed transaction log in tabular format with success/failure and retry indicators

---

## 📁 File Structure

```
.
├── include/
│   ├── accounts.h
│   ├── common.h
│   ├── ipc.h
│   └── transaction.h
├── src/
│   ├── accounts.c
│   ├── ipc.c
│   ├── main.c
│   └── transaction.c
├── transactions/
│   ├── transactions.txt   # Example input
├── CMakeLists.txt
└── README.md              # This file
```

---