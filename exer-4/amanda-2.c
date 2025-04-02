#include "account.h"
#include "string_parser.h"

// Defining the number of threads and the account mutex
#define NUM_WORKERS 10
account** accounts = NULL;
pthread_t thread_id[NUM_WORKERS];
pthread_mutex_t accounts_mutex = PTHREAD_MUTEX_INITIALIZER;

int num_transactions = 0;
int cur_account;

// Defining the struct for process_transaction
typedef struct {
    command_line* command_list;
    int start_index;
    int end_index;
} thread_data;


void* process_transaction(void* arg) {

    // Getting data from thread_data struct
    thread_data* data = (thread_data*)arg;
    command_line* command_list = data->command_list;
    int start_index = data->start_index;
    int end_index = data->end_index;

    // Utilizing the mutex lock and calling the functions from account.c
    for (int i = start_index; i < end_index; i++) {
        command_line trans_tok = command_list[i];
        if (strcmp(trans_tok.command_list[0], "T") == 0) {
            pthread_mutex_lock(&accounts_mutex);
            transfer(accounts, cur_account, trans_tok.command_list[1], trans_tok.command_list[2], trans_tok.command_list[3], trans_tok.command_list[4]);
            pthread_mutex_unlock(&accounts_mutex);
        } else if (strcmp(trans_tok.command_list[0], "C") == 0) {
            pthread_mutex_lock(&accounts_mutex);
            check_bal(accounts, cur_account, trans_tok.command_list[1], trans_tok.command_list[2]);
            pthread_mutex_unlock(&accounts_mutex);
        } else if (strcmp(trans_tok.command_list[0], "D") == 0) {
            pthread_mutex_lock(&accounts_mutex);
            deposit(accounts, cur_account, trans_tok.command_list[1], trans_tok.command_list[2], trans_tok.command_list[3]);
            pthread_mutex_unlock(&accounts_mutex);
        } else if (strcmp(trans_tok.command_list[0], "W") == 0) {
            pthread_mutex_lock(&accounts_mutex);
            withdraw(accounts, cur_account, trans_tok.command_list[1], trans_tok.command_list[2], trans_tok.command_list[3]);
            pthread_mutex_unlock(&accounts_mutex);
        }
    }

    // Exiting the pthread when finished
    pthread_exit(NULL);
}


void* update_balance (void* arg){
    // Opening the file
    FILE* fp = fopen("output.txt", "w");
    if (fp != NULL){
        // Looping through the accounts and printing the data to the file
        for (int i = 0; i < cur_account; i++){
            pthread_mutex_lock(&accounts[i]->ac_lock);
            accounts[i]->balance += ((accounts[i]->reward_rate)*(accounts[i]->transaction_tracker));
            fprintf(fp, "%d balance: %.2lf\n", i, accounts[i]->balance);
            pthread_mutex_unlock(&accounts[i]->ac_lock);
        }
        fclose(fp);
    } else {
        perror("Error opening file");
        pthread_exit(NULL);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv) {

    // Making sure a file name was inputted
    if (argc < 2) {
        perror("No file inputted");
        exit(EXIT_FAILURE);
    }

    // Open the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the number of accounts
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    int num_accounts = atoi(buffer);

    // Allocate memory for accounts
    accounts = (account**) malloc(sizeof(account*) * num_accounts);
    if (accounts == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // Initializing the account and command_list
    cur_account = 0;
    command_line* command_list = NULL;

    // Reading the accounts into the accounts struct
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "index", 5) == 0) {
            accounts[cur_account] = (account*)malloc(sizeof(account));
            if (fgets(buffer, sizeof(buffer), file) == NULL) break;
            strncpy(accounts[cur_account]->account_number, buffer, 16);
            accounts[cur_account]->account_number[16] = '\0';

            if (fgets(buffer, sizeof(buffer), file) == NULL) break;
            strncpy(accounts[cur_account]->password, buffer, 8);
            accounts[cur_account]->password[8] = '\0';

            if (fgets(buffer, sizeof(buffer), file) == NULL) break;
            accounts[cur_account]->balance = atof(buffer);

            if (fgets(buffer, sizeof(buffer), file) == NULL) break;
            accounts[cur_account]->reward_rate = atof(buffer);

            accounts[cur_account]->transaction_tracker = 0.0;
            pthread_mutex_init(&accounts[cur_account]->ac_lock, NULL);

            cur_account++;
            if (cur_account == num_accounts){
                break;
            }
        }
    }



    // Reads the transactions and stores them in the command_list array
    while (fgets(buffer, sizeof(buffer), file)) {
        command_list = realloc(command_list, sizeof(command_line) * (num_transactions + 1));
        if (command_list == NULL) {
            perror("Error reallocating memory");
            exit(EXIT_FAILURE);
        }
        command_list[num_transactions++] = str_filler(buffer, " ");
    }

    // Dividing the transactions among the threads
    int transactions_per_thread = num_transactions / NUM_WORKERS;
    int remaining_transactions = num_transactions % NUM_WORKERS;

    thread_data thread_args[NUM_WORKERS];

    // Creating the threads
    int start_index = 0;
    for (int i = 0; i < NUM_WORKERS; ++i) {
        int end_index = start_index + transactions_per_thread;
        if (i < remaining_transactions) {
            end_index++;
        }

        thread_args[i].command_list = command_list;
        thread_args[i].start_index = start_index;
        thread_args[i].end_index = end_index;

        // Creating the pthread
        pthread_create(&thread_id[i], NULL, process_transaction, &thread_args[i]);
        //printf("\n>>>>> THREAD %d CREATED\n\n", i);

        start_index = end_index;
    }

    // Waiting for all worker threads to finish
    for (int i = 0; i < NUM_WORKERS; ++i) {
        pthread_join(thread_id[i], NULL);
        //printf("\n>>>>> THREAD %d FINISHED\n\n", i);
    }

    // Starting the banker thread
    pthread_t bank_thread_id;
    int rc = pthread_create(&bank_thread_id, NULL, update_balance, NULL);
    if (rc) {
        printf("Error: Unable to create bank thread\n");
        exit(EXIT_FAILURE);
    }

    // Waiting for the bank thread to finish
    pthread_join(bank_thread_id, NULL);


    // Cleaning everyting up and freeing the data
    fclose(file);
    for (int i = 0; i < num_accounts; i++){
        pthread_mutex_destroy(&accounts[i]->ac_lock);
        free(accounts[i]);
    }

    for (int i = 0; i < num_transactions; i++){
        free_command_line(&(command_list[i]));
    }

    free(accounts);
    free(command_list);

    return 0;
}

