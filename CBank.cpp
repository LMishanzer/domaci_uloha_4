#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

#define START_SIZE 100

struct Transaction
{
    bool to_me;
    const char * acc;
    unsigned int amount;
    const char * sign;
};

class CAccount
{
public:
    // implicit constructor
    CAccount()
    {
        m_id_acc = nullptr;
        m_init_balance = 0;
        m_current_balance = 0;
        m_transactions = new struct Transaction[START_SIZE];
        m_current_transaction = 0;
        m_transaction_size = START_SIZE;
    }

    // constructor
//        CAccount(const char * accID, const int initBalance)
//                : m_id_acc(accID), m_init_balance(initBalance), m_current_balance(initBalance)
//        {
//            m_transactions = new struct Transaction[START_SIZE];
//            m_transaction_size = START_SIZE;
//            m_current_transaction = 0;
//        }

    CAccount &operator = (const CAccount &acc)
    {
        if (&acc == this)
            return *this;

        m_id_acc = acc.m_id_acc;
        m_init_balance = acc.m_init_balance;
        m_current_balance = acc.m_current_balance;
        m_current_transaction = acc.m_current_transaction;
        m_transaction_size = acc.m_transaction_size;

        m_transactions = new struct Transaction[m_transaction_size];
        for (int i = 0; i < m_transaction_size; i++)
        {
            m_transactions[i] = acc.m_transactions[i];
        }

        return *this;
    }

    // destructor
    ~CAccount()
    {
        delete[] m_transactions;
    }

    // return current balance
    int Balance() const
    {
        return m_current_balance;
    }

    // archiving transaction
    void AddTransaction(bool to_me, const char * acc, const unsigned int amount, const char * sign)
    {
        if (IsTransactionFull())
            ExpandTransaction();

        m_transactions[m_current_transaction].to_me = to_me;
        m_transactions[m_current_transaction].acc = acc;
        m_transactions[m_current_transaction].amount = amount;
        m_transactions[m_current_transaction].sign = sign;

        m_current_transaction++;
    }

    // clears current balance and list of m_transactions
    void Trim()
    {
        m_current_balance = m_init_balance;
        delete [] m_transactions;
        m_transactions = nullptr;
    }

    friend ostringstream &operator<<(ostringstream &os, const CAccount &acc);

    const char * m_id_acc;
    int m_init_balance;
    int m_current_balance;
    struct Transaction * m_transactions;
    int m_current_transaction;

private:
    bool IsTransactionFull() const
    {
        return m_current_transaction == m_transaction_size;
    }

    void ExpandTransaction()
    {
        auto * temp = new struct Transaction[2 * m_transaction_size];
        for (int i = 0; i < m_transaction_size; i++)
        {
            temp[i] = m_transactions[i];
        }
        delete [] m_transactions;
        m_transaction_size *= 2;
        m_transactions = temp;
    }

    int m_transaction_size;
};

class CBank
{
public:
    // default constructor
    CBank()
    {
        m_accounts = new CAccount[START_SIZE];
        m_current_account = 0;
        m_account_size = START_SIZE;
    }

    // copy constructor


    // destructor
    ~CBank()
    {
        delete [] m_accounts;
    }
    // operator =

    bool NewAccount ( const char * accID, int initialBalance );

    bool   Transaction   ( const char * debAccID, const char * credAccID, unsigned int amount,
            const char * signature );
    bool   TrimAccount   ( const char * accID );
    CAccount &Account ( const char * accID );

private:
    bool IsAccountFull() const;
    void ExpandAccount();
    CAccount * IsInBank(const char * idAcc);

    CAccount * m_accounts;
    int m_current_account;
    int m_account_size;
};

// printing account to stream
ostringstream &operator<<(ostringstream &os, const CAccount &acc)
{
    os << acc.m_id_acc << ":\n   " << acc.m_init_balance << '\n';

    for (int i = 0; i < acc.m_current_transaction; i++)
    {
        os << (acc.m_transactions[i].to_me ? " + " : " - ") << acc.m_transactions[i].amount << ", "
        << (acc.m_transactions[i].to_me ? "from: " : "to: ") << acc.m_transactions[i].acc << ", "
        << acc.m_transactions[i].sign << '\n';
    }

    os << " = " << acc.m_current_balance << "\n";

    return os;
}

bool CBank::NewAccount ( const char * accID, int initialBalance )
{
    if (IsInBank(accID))
        return false;

    if (IsAccountFull())
    {
        ExpandAccount();
    }

    m_accounts[m_current_account].m_id_acc = accID;
    m_accounts[m_current_account].m_init_balance = initialBalance;
    m_accounts[m_current_account].m_current_balance = initialBalance;

    m_current_account++;

    return true;
}

CAccount &CBank::Account(const char *accID)
{
    CAccount * temp = IsInBank(accID);

    if (!temp)
        throw "Exception!";

    return *temp;
}

bool CBank::Transaction (const char * debAccID, const char * credAccID, unsigned int amount,
                       const char * signature )
{
    // control IDs
    if (strcmp(debAccID, credAccID) == 0)
        return false;

    auto * deb = IsInBank(debAccID);
    auto * cred = IsInBank(credAccID);

    // controlling accounts
    if (!deb || !cred)
        return false;

    // transaction
    deb->m_current_balance -= (int) amount;
    cred->m_current_balance += (int) amount;

    // saving
    deb->AddTransaction(false, credAccID, amount, signature);
    cred->AddTransaction(true, debAccID, amount, signature);

    return true;
}

bool CBank::TrimAccount ( const char * accID )
{
    auto * acc = IsInBank(accID);

    if (!acc)
        return false;

    acc->Trim();

    return true;
}

void CBank::ExpandAccount()
{
    auto * temp = new CAccount[2 * m_account_size];
    for (int i = 0; i < m_account_size; i++)
    {
        temp[i] = m_accounts[i];
    }
    delete [] m_accounts;
    m_account_size *= 2;
    m_accounts = temp;
}

bool CBank::IsAccountFull() const
{
    return m_current_account == m_account_size;
}

CAccount * CBank::IsInBank(const char * idAcc)
{
    for (int i = 0; i < m_current_account; i++)
    {
        if (strcmp(m_accounts[i].m_id_acc, idAcc) == 0)
            return &m_accounts[i];
    }
    return nullptr;
}
