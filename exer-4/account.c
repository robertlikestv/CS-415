#include "account.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Transfer money to an account
void transfer(account** accounts, int num_accounts, char* src_account, char* password, char* dest_account, char* trans_amount){
	
	// Finding the accounts
	int src_pos = -1;
	int dest_pos = -1;

	for (int i = 0; i < num_accounts; i++){
		if (strcmp(accounts[i]->account_number, src_account) == 0){
			src_pos = i;
		}
		if (strcmp(accounts[i]->account_number, dest_account) == 0){
			dest_pos = i;
		}
		if (src_pos != -1 && dest_pos != -1){
		 	break;
		}
	}

	if (src_pos == -1 || dest_pos == -1) return; // Account not found

	// Locking the mutex locks for the two accounts
	pthread_mutex_lock(&accounts[src_pos]->ac_lock);
	pthread_mutex_lock(&accounts[dest_pos]->ac_lock);

	// If the password matches, we do the operations
	if (strcmp(accounts[src_pos]->password, password) == 0){
		double trans_amt = atof(trans_amount);
		accounts[src_pos]->balance -= trans_amt;
		accounts[src_pos]->transaction_tracker += trans_amt;
		accounts[dest_pos]->balance += trans_amt;
	}

	// Unlocking the account mutex locks
	pthread_mutex_unlock(&accounts[dest_pos]->ac_lock);
	pthread_mutex_unlock(&accounts[src_pos]->ac_lock);
}

// Print balance of an account
void check_bal(account** accounts, int num_accounts, char* account_num, char* password){
	// Finding the account
	int src_pos = -1;
	for (int i = 0; i < num_accounts; i++){
		if (strcmp(accounts[i]->account_number, account_num) == 0){
			src_pos = i;
			break;
		}
	}

	if (src_pos == -1) return; // Account not found

	// Locking the account mutex lock
	pthread_mutex_lock(&accounts[src_pos]->ac_lock);

	// Printing the account balance
	if (strcmp(accounts[src_pos]->password, password) == 0){
		printf("Balance for account %s: %.2lf\n", account_num, accounts[src_pos]->balance);
	}
	// Unlocking the account mutex lock
	pthread_mutex_unlock(&accounts[src_pos]->ac_lock);
}

// Add money to an account
void deposit(account** accounts, int num_accounts, char* account_num, char* password, char* amount){
	// Finding the account
	int src_pos = -1;
	for (int i = 0; i < num_accounts; i++){
		if (strcmp(accounts[i]->account_number, account_num) == 0){
			src_pos = i;
			break;
		}
	}

	if (src_pos == -1) return; // Account not found

	// Locking the account mutex lock
	pthread_mutex_lock(&accounts[src_pos]->ac_lock);

	// Depositing money in their account
	if (strcmp(accounts[src_pos]->password, password) == 0){
		double trans_amt = atof(amount);
		accounts[src_pos]->balance += trans_amt;
		accounts[src_pos]->transaction_tracker += trans_amt;
	}

	// Unlocking the account mutex lock
	pthread_mutex_unlock(&accounts[src_pos]->ac_lock);
}

// Withdraw money from an account
void withdraw(account** accounts, int num_accounts, char* account_num, char* password, char* amount){
	// Finding the account
	int src_pos = -1;
	for (int i = 0; i < num_accounts; i++){
		if (strcmp(accounts[i]->account_number, account_num) == 0){
			src_pos = i;
			break;
		}
	}

	if (src_pos == -1) return; // Account not found

	// Locking the account mutex lock
	pthread_mutex_lock(&accounts[src_pos]->ac_lock);

	// Withdrawing money from the account
	if (strcmp(accounts[src_pos]->password, password) == 0){
		double trans_amt = atof(amount);
		accounts[src_pos]->balance -= trans_amt;
		accounts[src_pos]->transaction_tracker += trans_amt;
	}

	//Unlocking the account mutex lock
	pthread_mutex_unlock(&accounts[src_pos]->ac_lock);
}
