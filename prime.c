/*
 * Ex #2a: Prime Number Filter via Memory
 * ==========================================================
 * Written by: Jacob Bondar.
 *
 * This program creates a memory to receive numbers from
 * clients, processes the numbers to extract prime numbers, and
 * sends the processed result back to the clients. The program runs
 * continuously until terminated using a SIGINT signal, at which point
 * the memory is safely removed.
 *
 * Input: void (getting values threw the memory).
 *
 * Output: void (sending values threw the memory).
 */

 //-------------- include section ---------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/shm.h>

//-------------- const and enum section --------------------------------

const int SHM_SIZE = 102;

enum numbers_t {
    ZERO_NUM,
    ONE_NUM,
};

//-------------- global variables --------------------------------------

int shm_id;

//-------------- prototypes section ------------------------------------

void start_run(int shm_id);
void finish_program(int sig);
bool is_prime(int number);
void update_prime(int** shm_ptr);

//-------------- main --------------------------------------------------

int main()
{
    signal(SIGINT, finish_program);

    key_t key;

    if ((key = ftok(".", 'p')) == -1)
    {
        perror("Can't fork\n");
        exit(EXIT_FAILURE);
    }

    if ((shm_id = shmget(key, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0600))
        == -1)
    {
        perror("Cant shmget\n");
        exit(EXIT_FAILURE);
    }

    start_run(shm_id);

    exit(EXIT_SUCCESS);
}

//----------------------------------------------------------------------

/* The function recieves and sends the result of the prime answer.
 * The function receives: id of the memory.
 * The function returns: void.
 */
void start_run(int shm_id)
{
    int* shm_ptr = (int*)shmat(shm_id, NULL, 0);

    if (shm_ptr == (int*)-1)
    {
        perror("shmat failed: ");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        while (shm_ptr[ZERO_NUM] <= ZERO_NUM) {}

        update_prime(&shm_ptr);
        shm_ptr[ZERO_NUM] = ONE_NUM;

        while (shm_ptr[ZERO_NUM] != ZERO_NUM) {}
    }
}

//----------------------------------------------------------------------

/* Extracts prime numbers from the memory.
 * The function receives: a pointer to a pointer to the memory.
 * The function returns: void.
 */
void update_prime(int** shm_ptr)
{
    int index = 1;

    while ((*shm_ptr)[index] != 0)
    {
        if (!is_prime((*shm_ptr)[index]))
        {
            (*shm_ptr)[index] = -1;
        }
        index++;
    }
}

//----------------------------------------------------------------------

/* Determines whether a number is prime.
 * The function receives: an integer to check.
 * The function returns: true if the number is prime, false otherwise.
 */
bool is_prime(int number)
{
    for (int i = 2; i * i <= number; i++)
    {
        if (number % i == 0)
        {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------

/* Handles program termination by removing the memory.
 * The function receives: the signal number (SIGINT).
 * The function returns: void.
 */
void finish_program(int sig)
{
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl IPC_RMID failed: ");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

