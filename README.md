#  Banking System — DSA (C++)

A console-based banking system built in **C++** using **Singly and Doubly Linked Lists**.  
This project demonstrates core Data Structures & Algorithms concepts including dynamic memory management, pointer manipulation, forward/backward traversal, and undo operations.

---

## Data Structures Used

| Structure | Role |
|-----------|------|
| **Singly Linked List (SLL)** | Stores all bank accounts — each node is one account |
| **Doubly Linked List (DLL)** | Stores transaction history — supports forward processing & backward undo |

### Why these structures?
- **SLL** — Accounts only need sequential access; no need for backward traversal.
- **DLL** — Transactions need both forward (process) and backward (undo) traversal, which a doubly linked list handles in O(1) per step.

---

## Project Structure

```
banking_system.cpp   ← Full source code (single file)
README.md            ← This file
```

---

## How It Works

```
head <-> [T1] <-> [T2] <-> [T3] <-> ... <-> [Tn] <-> tail
                   ^
                 cursor    (marks last processed transaction)
```

- Transactions **before** the cursor → already applied to account balances  
- Transactions **after** the cursor → still pending  
- Moving the cursor **forward** processes transactions  
- Moving the cursor **backward** reverses (undoes) them  

---

## Getting Started

### Compile

```bash
g++ -o banking_system banking_system.cpp
```

### Run

```bash
./banking_system
```

### Example Session

```
Enter number of accounts to create: 3
Enter account number for account 1: 101
Enter account number for account 2: 102
Enter account number for account 3: 103

Enter number of transactions to record: 2
Transaction 1 (<acc> <D/W> <amount>): 101 D 500
Transaction 2 (<acc> <D/W> <amount>): 102 W 200

> V 101
Balance for account 101: 1500

> V 102
Balance for account 102: 800

> T
Enter: <from_acc> <to_acc> <amount>: 101 103 300
Rs.300 transferred from account 101 to account 103.

> M
Account(s) with max balance: 101
```

---

## Command Reference

| Command | Description |
|---------|-------------|
| `F x` | Process the **next x** pending transactions |
| `R y` | **Undo** the last y processed transactions |
| `I a b c k` | **Insert** transaction for account `a`, type `b` (D/W), amount `c`, at position `k` |
| `D acc m` | **Delete** next `m` pending transactions for account `acc` |
| `C` | **Process all** remaining transactions |
| `S y` | **Show** all transactions for account `y` |
| `G x` | **Count** accounts with balance >= `x` |
| `M` | Show account(s) with **maximum balance** |
| `V x` | Show **balance** of account `x` |
| `T` | **Transfer** amount between two accounts |
| `E` | **Exit** the program |

---

## Key Classes

### `Node` (SLL Node)
```
acc_no   → Account number
balance  → Current balance (starts at ₹1000)
next     → Pointer to next account
```

### `SLL` (Account Manager)
```
push_node()        → Add a new account
find_node()        → Linear search by account number
transaction()      → Apply deposit or withdrawal
print_bal_more_x() → Count accounts above threshold
print_max_balance()→ Find account(s) with highest balance
print_bal_x()      → Print balance of one account
transfer_amount()  → Move funds between accounts
```

### `DNode` (DLL Node)
```
acc     → Account number
action  → 'D' (Deposit) or 'W' (Withdrawal)
amount  → Transaction value
next    → Forward pointer
prev    → Backward pointer (for undo)
```

### `DLL` (Transaction Manager)
```
push_d_node()    → Append transaction to history
process_x()      → Apply next x transactions
undo_y()         → Reverse last y transactions
insert_node_k()  → Insert transaction at position k
delete_am()      → Delete m pending transactions for an account
process_all()    → Apply all pending transactions
print_all_of_y() → Show all transactions for an account
```

---

## Concepts Demonstrated

- Singly and Doubly Linked List construction from scratch  
- Sentinel nodes for cleaner edge-case handling  
- Cursor-based state tracking (process / undo without re-scanning)  
- Manual memory management with `new` / `delete`  
- Forward declaration to resolve circular class dependencies  

---

## Requirements

- C++11 or later  
- Any standard compiler: `g++`, `clang++`, MSVC  

---

*DSA Project | C++ | Linked Lists*
