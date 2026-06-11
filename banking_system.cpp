/*
 * ============================================================
 *         BANKING SYSTEM — DSA PROJECT (C++)
 * ============================================================
 * Data Structures Used:
 *   - SLL  (Singly Linked List)  → stores bank accounts
 *   - DLL  (Doubly Linked List)  → stores transaction history
 *
 * Each account starts with a default balance of 1000.
 * Transactions can be Deposited ('D') or Withdrawn ('W').
 *
 * Commands:
 *   F x       → Process next x transactions
 *   R y       → Undo last y transactions
 *   I a b c k → Insert transaction at position k
 *   D acc m   → Delete m pending transactions for account
 *   C         → Process all remaining transactions
 *   S y       → Show all transactions for account y
 *   G x       → Count accounts with balance >= x
 *   M         → Show account(s) with max balance
 *   V x       → Show balance of account x
 *   T         → Transfer amount between two accounts
 *   E         → Exit
 * ============================================================
 */

#include <iostream>
#include <string>
using namespace std;

// ============================================================
//  Forward declaration of DLL so SLL can reference it
// ============================================================
class DLL;


// ============================================================
//  NODE — Singly Linked List (Accounts)
// ============================================================
/*
 * Each Node represents one bank account.
 * Stores: account number, current balance, pointer to next account.
 */
class Node {
public:
    int   acc_no;   // Unique account identifier
    int   balance;  // Current balance (default 1000)
    Node* next;     // Link to the next account in the list

    // Constructor: initialise account with given number and ₹1000 balance
    Node(int acc) {
        acc_no  = acc;
        balance = 1000;   // Every new account starts with Rs.1000
        next    = nullptr;
    }
};


// ============================================================
//  SLL — Singly Linked List (Account Manager)
// ============================================================
/*
 * Manages a chain of Node objects (bank accounts).
 * Supports: add, search, deposit/withdraw, balance queries, transfer.
 */
class SLL {
public:
    Node* first;    // Pointer to the first account node
    Node* last;     // Pointer to the last account node (for O(1) append)
    int   length;   // Total number of accounts

    // Constructor: empty list
    SLL() {
        first  = nullptr;
        last   = nullptr;
        length = 0;
    }

    // ----------------------------------------------------------
    // push_node: Append a new account at the end of the list
    // ----------------------------------------------------------
    void push_node(int acc) {
        Node* new_node = new Node(acc);

        if (length == 0) {
            // First account — both first and last point to it
            first = last = new_node;
        } else {
            // Link after current last node, then update last
            last->next = new_node;
            last       = new_node;
        }
        length++;
    }

    // ----------------------------------------------------------
    // find_node: Linear search for account by account number
    // Returns: pointer to the node, or nullptr if not found
    // ----------------------------------------------------------
    Node* find_node(int acc) {
        Node* temp = first;
        while (temp != nullptr) {
            if (temp->acc_no == acc)
                return temp;  // Found
            temp = temp->next;
        }
        return nullptr;  // Not found
    }

    // ----------------------------------------------------------
    // transaction: Apply one deposit or withdrawal to an account
    //   mode == 'D'  → add val to balance   (Deposit)
    //   mode == 'W'  → subtract val          (Withdrawal)
    //   mode == 'W', val < 0 → used internally for UNDO
    // ----------------------------------------------------------
    void transaction(int acc, char mode, int val) {
        Node* process_node = find_node(acc);
        if (process_node != nullptr) {
            if (mode == 'D') {
                process_node->balance += val;   // Deposit: increase balance
            } else if (mode == 'W') {
                process_node->balance -= val;   // Withdrawal: decrease balance
            }
        }
    }

    // ----------------------------------------------------------
    // print_bal_more_x: Count & print accounts with balance >= x
    // ----------------------------------------------------------
    void print_bal_more_x(int x) {
        int   count = 0;
        Node* temp  = first;

        while (temp != nullptr) {
            if (temp->balance >= x)
                count++;
            temp = temp->next;
        }
        cout << "Number of accounts with balance >= " << x
             << ": " << count << "\n";
    }

    // ----------------------------------------------------------
    // print_max_balance: Print account number(s) with the
    // highest balance (handles ties)
    // ----------------------------------------------------------
    void print_max_balance() {
        if (first == nullptr) {
            cout << "No accounts found.\n";
            return;
        }

        // --- Pass 1: find the maximum balance value ---
        int   max_balance = -1;
        Node* temp        = first;
        while (temp != nullptr) {
            if (temp->balance > max_balance)
                max_balance = temp->balance;
            temp = temp->next;
        }

        // --- Pass 2: collect all accounts that match ---
        temp = first;
        cout << "Account(s) with max balance: ";
        while (temp != nullptr) {
            if (temp->balance == max_balance)
                cout << temp->acc_no << " ";
            temp = temp->next;
        }
        cout << "\n";
    }

    // ----------------------------------------------------------
    // print_bal_x: Print the balance of a specific account
    // ----------------------------------------------------------
    void print_bal_x(int x) {
        Node* temp = find_node(x);
        if (temp != nullptr) {
            cout << temp->balance << "\n";
        } else {
            cout << "Account not found.\n";
        }
    }

    // ----------------------------------------------------------
    // transfer_amount: Move 'amount' from one account to another.
    // Also logs two transactions (W + D) in the DLL history.
    // Defined AFTER DLL class (see bottom of file).
    // ----------------------------------------------------------
    void transfer_amount(int from_acc, int to_acc, int amount, DLL& dll);
};


// ============================================================
//  DNODE — Doubly Linked List Node (Transactions)
// ============================================================
/*
 * Each DNode records one pending/processed transaction:
 *   acc    → which account is affected
 *   action → 'D' (Deposit) or 'W' (Withdrawal)
 *   amount → how much
 * The doubly-linked structure allows bidirectional traversal
 * so we can both PROCESS (forward) and UNDO (backward).
 */
class DNode {
public:
    int    acc;     // Account number this transaction targets
    char   action;  // 'D' = Deposit, 'W' = Withdrawal
    int    amount;  // Transaction value
    DNode* next;    // Forward pointer
    DNode* prev;    // Backward pointer (enables undo)

    // Constructor with defaults (used for sentinel head/tail nodes)
    DNode(int acc = -1, char action = 'A', int val = -1) {
        this->acc    = acc;
        this->action = action;
        this->amount = val;
        next         = nullptr;
        prev         = nullptr;
    }
};


// ============================================================
//  DLL — Doubly Linked List (Transaction History)
// ============================================================
/*
 * Maintains a sequence of DNode transaction records.
 *
 * Uses TWO sentinel nodes (head & tail) to simplify
 * edge-case handling — real data lives between them.
 *
 *   head <-> [t1] <-> [t2] <-> ... <-> [tn] <-> tail
 *               ^
 *             cursor  (marks how far we've processed)
 *
 * Transactions BEFORE cursor  → already applied to accounts
 * Transactions AFTER  cursor  → still pending
 */
class DLL {
public:
    DNode* head;        // Sentinel: marks the beginning
    DNode* tail;        // Sentinel: marks the end
    DNode* cursor;      // Points to last PROCESSED transaction
    int    d_len;       // Number of real transaction nodes
    int    cursor_idx;  // Numeric index of cursor position

    // Constructor: create empty list with sentinel head & tail
    DLL() {
        head       = new DNode();   // Sentinel (not a real transaction)
        tail       = new DNode();   // Sentinel (not a real transaction)
        head->next = tail;
        tail->prev = head;
        cursor     = head;          // Nothing processed yet
        d_len      = 0;
        cursor_idx = 0;
    }

    // ----------------------------------------------------------
    // push_d_node: Append a transaction BEFORE the tail sentinel
    // (i.e., at the logical end of the list)
    // ----------------------------------------------------------
    void push_d_node(int a, char b, int c) {
        DNode* new_node = new DNode(a, b, c);

        // Insert between current last real node and tail
        new_node->prev       = tail->prev;
        tail->prev->next     = new_node;
        new_node->next       = tail;
        tail->prev           = new_node;

        d_len++;
    }

    // ----------------------------------------------------------
    // process_x: Advance the cursor and apply the next x
    // transactions to the account list (SLL).
    // Stops early if we reach the end of the list.
    // ----------------------------------------------------------
    void process_x(int x, SLL& l1) {
        while (cursor->next != tail && x > 0) {
            cursor = cursor->next;      // Move cursor forward
            cursor_idx++;
            // Apply this transaction to the SLL account
            l1.transaction(cursor->acc, cursor->action, cursor->amount);
            x--;
        }
    }

    // ----------------------------------------------------------
    // undo_y: Move the cursor BACKWARD and REVERSE the last y
    // transactions (restores account balances).
    //
    // Reversal trick: pass -(amount) as the value.
    //   If original was D +500, undo does D -500 (or W+500 equivalent)
    // ----------------------------------------------------------
    void undo_y(int y, SLL& l1) {
        while (cursor != head && y > 0) {
            // Reverse current transaction by negating the amount
            l1.transaction(cursor->acc, cursor->action, -(cursor->amount));
            cursor = cursor->prev;  // Step cursor back
            cursor_idx--;
            y--;
        }
    }

    // ----------------------------------------------------------
    // insert_node_k: Insert a new transaction at position k
    // (1-indexed). If the position is BEFORE or AT the cursor,
    // it is already "processed", so apply it to the SLL now.
    // ----------------------------------------------------------
    void insert_node_k(int a, char b, int c, int k, SLL& l1) {
        if (k < 1 || k > d_len) {
            cout << "Invalid position. Must be between 1 and " << d_len << "\n";
            return;
        }

        DNode* add_node = new DNode(a, b, c);

        // Walk to the k-th node (1-indexed from head sentinel)
        DNode* temp = head;
        for (int i = 0; i < k; i++)
            temp = temp->next;

        // Splice new_node between temp and temp->next
        temp->next->prev = add_node;
        add_node->next   = temp->next;
        temp->next       = add_node;
        add_node->prev   = temp;

        // If inserted at or before cursor → apply immediately
        if (temp != cursor) {
            l1.transaction(a, b, c);
        }

        d_len++;
    }

    // ----------------------------------------------------------
    // delete_am: Delete the NEXT m pending transactions for
    // account 'acc' (only looks AFTER the cursor).
    // These transactions have not been processed yet, so no
    // balance reversal is needed.
    // ----------------------------------------------------------
    void delete_am(int acc, int m, SLL& /*l1*/) {
        DNode* temp = cursor->next;  // Start just after cursor

        while (temp != tail && m != 0) {
            if (temp->acc == acc) {
                // Unlink this node
                DNode* del_node    = temp;
                temp->prev->next   = temp->next;
                temp->next->prev   = temp->prev;
                temp               = temp->next;

                delete del_node;   // Free memory
                d_len--;
                m--;
            } else {
                temp = temp->next;
            }
        }
    }

    // ----------------------------------------------------------
    // process_all: Process every remaining transaction
    // (convenience wrapper around process_x)
    // ----------------------------------------------------------
    void process_all(SLL& l1) {
        process_x(d_len, l1);  // d_len is an upper bound; stops at tail
    }

    // ----------------------------------------------------------
    // print_all_of_y: Print every transaction in the list
    // (processed or pending) belonging to account 'acc'
    // ----------------------------------------------------------
    void print_all_of_y(int acc) {
        DNode* temp = head->next;
        bool found  = false;

        while (temp != tail) {
            if (temp->acc == acc) {
                cout << "  Acc: " << temp->acc
                     << "  Type: " << temp->action
                     << "  Amount: " << temp->amount << "\n";
                found = true;
            }
            temp = temp->next;
        }

        if (!found)
            cout << "  No transactions found for account " << acc << "\n";
    }
};


// ============================================================
//  SLL::transfer_amount  (defined here — DLL is now complete)
// ============================================================
/*
 * Transfers 'amount' from from_acc to to_acc.
 * Validates both accounts exist and source has sufficient funds.
 * Records the operation as two DLL entries: one W and one D.
 */
void SLL::transfer_amount(int from_acc, int to_acc, int amount, DLL& dll) {
    Node* from_node = find_node(from_acc);
    Node* to_node   = find_node(to_acc);

    // Validate accounts
    if (from_node == nullptr || to_node == nullptr) {
        cout << "Error: One or both account numbers are invalid.\n";
        return;
    }

    // Check sufficient balance
    if (from_node->balance < amount) {
        cout << "Error: Insufficient funds. Available balance: "
             << from_node->balance << "\n";
        return;
    }

    // Perform the transfer
    from_node->balance -= amount;
    to_node->balance   += amount;

    // Log both legs in the transaction history
    dll.push_d_node(from_acc, 'W', amount);
    dll.push_d_node(to_acc,   'D', amount);

    cout << "Rs." << amount << " transferred from account "
         << from_acc << " to account " << to_acc << ".\n";
}


// ============================================================
//  MAIN — Driver / Menu Loop
// ============================================================
int main() {
    SLL accounts;       // Singly linked list of bank accounts
    DLL transactions;   // Doubly linked list of transactions

    // ---- Create Accounts ----
    int c;
    cout << "Enter number of accounts to create: ";
    cin  >> c;

    for (int i = 0; i < c; i++) {
        int acs;
        cout << "Enter account number for account " << (i + 1) << ": ";
        cin  >> acs;
        accounts.push_node(acs);
    }

    // ---- Record Initial Transactions ----
    int n;
    cout << "Enter number of transactions to record: ";
    cin  >> n;
    cin.ignore();  // Flush leftover newline before getline usage

    for (int i = 0; i < n; i++) {
        int  acs, am;
        char dw;
        cout << "Transaction " << (i + 1)
             << " (<acc> <D/W> <amount>): ";
        cin  >> acs >> dw >> am;
        transactions.push_d_node(acs, dw, am);
    }

    // Process all recorded transactions immediately at startup
    transactions.process_all(accounts);

    // ---- Display Command Menu ----
    cout << "\n========================================\n";
    cout << "           COMMAND REFERENCE\n";
    cout << "========================================\n";
    cout << "  F x       - Process next x transactions\n";
    cout << "  R y       - Undo last y transactions\n";
    cout << "  I a b c k - Insert txn (acc b=D/W c=amt k=pos)\n";
    cout << "  D acc m   - Delete m pending txns for acc\n";
    cout << "  C         - Process all remaining transactions\n";
    cout << "  S y       - Show all transactions for account y\n";
    cout << "  G x       - Count accounts with balance >= x\n";
    cout << "  M         - Show account(s) with max balance\n";
    cout << "  V x       - Show balance of account x\n";
    cout << "  T         - Transfer money between accounts\n";
    cout << "  E         - Exit\n";
    cout << "========================================\n";

    // ---- Command Loop ----
    string choice;
    while (true) {
        cout << "\n> ";
        cin  >> choice;

        // ---- EXIT ----
        if (choice == "E") {
            cout << "Exiting. Goodbye!\n";
            break;
        }

        // ---- F x: Process next x transactions ----
        else if (choice == "F") {
            int x;
            cin >> x;
            transactions.process_x(x, accounts);
            cout << "Processed " << x << " transaction(s).\n";
        }

        // ---- R y: Undo last y transactions ----
        else if (choice == "R") {
            int y;
            cin >> y;
            transactions.undo_y(y, accounts);
            cout << "Undid last " << y << " transaction(s).\n";
        }

        // ---- I a b c k: Insert transaction at position k ----
        else if (choice == "I") {
            int  acs, am, k;
            char dw;
            cout << "Enter: <acc> <D/W> <amount> <position>: ";
            cin  >> acs >> dw >> am >> k;
            transactions.insert_node_k(acs, dw, am, k, accounts);
            cout << "Inserted transaction at position " << k << ".\n";
        }

        // ---- D acc m: Delete m transactions for acc ----
        else if (choice == "D") {
            int acs, m;
            cout << "Enter: <acc> <count>: ";
            cin  >> acs >> m;
            transactions.delete_am(acs, m, accounts);
            cout << "Deleted " << m << " transaction(s) for account "
                 << acs << ".\n";
        }

        // ---- C: Process all remaining transactions ----
        else if (choice == "C") {
            transactions.process_all(accounts);
            cout << "All remaining transactions processed.\n";
        }

        // ---- S y: Show all transactions for account y ----
        else if (choice == "S") {
            int y;
            cin >> y;
            cout << "Transactions for account " << y << ":\n";
            transactions.print_all_of_y(y);
        }

        // ---- G x: Count accounts with balance >= x ----
        else if (choice == "G") {
            int x;
            cin >> x;
            accounts.print_bal_more_x(x);
        }

        // ---- M: Show account(s) with maximum balance ----
        else if (choice == "M") {
            accounts.print_max_balance();
        }

        // ---- V x: Show balance of account x ----
        else if (choice == "V") {
            int x;
            cin >> x;
            cout << "Balance for account " << x << ": ";
            accounts.print_bal_x(x);
        }

        // ---- T: Transfer between two accounts ----
        else if (choice == "T") {
            int from_acc, to_acc, amount;
            cout << "Enter: <from_acc> <to_acc> <amount>: ";
            cin  >> from_acc >> to_acc >> amount;
            accounts.transfer_amount(from_acc, to_acc, amount, transactions);
        }

        // ---- Unknown command ----
        else {
            cout << "Unknown command '" << choice
                 << "'. Type E to exit.\n";
        }
    }

    return 0;
}
