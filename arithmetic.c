/*
 * Ex #2b: Arithmetic Expression Processor via Memory
 * ==========================================================
 * Written by: Jacob Bondar.
 *
 * This program receives arithmetic expressions from clients through
 *  a memory, evaluates the result of the expressions, and sends
 *  the results back to the clients. It operates in an infinite loop
 *  until terminated with a SIGINT signal, at which point the memory
 *  is safely removed.
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
    TWO_NUM,
    THREE_NUM,
    FOUR_NUM
};

enum action_t {
    PLUS_ACT = 43,
    MINUS_ACT = 45,
    MULT_ACT = 42,
    DIV_ACT = 47
};

//-------------- global variables --------------------------------------

int shm_id;

//-------------- prototypes section ------------------------------------

void start_run(int shm_id);
int calc_value(int first_value, int action, int second_value);
int do_action(int first_value, int second_value, char action);
void finish_program(int sig);

//-------------- main --------------------------------------------------

int main()
{
    signal(SIGINT, finish_program);

    key_t key;

    if ((key = ftok(".", 'a')) == -1)
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

/* Processes a single client: receives,evaluates, and sends the result.
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

        shm_ptr[FOUR_NUM] =
            calc_value(shm_ptr[ONE_NUM], shm_ptr[TWO_NUM],
                shm_ptr[THREE_NUM]);

        shm_ptr[ZERO_NUM] = ONE_NUM;

        while (shm_ptr[ZERO_NUM] != ZERO_NUM) {}
    }
}

//----------------------------------------------------------------------

/* Evaluates an arithmetic expression in the format "<int><int><int>".
 * The function receives: 2 int values, and 1 int value representing the
 *  action.
 * The function returns: Answer of the action.
 */
int calc_value(int first_value, int action, int second_value)
{
    switch (action)
    {
    case PLUS_ACT:
    {
        return first_value + second_value;
        break;
    }

    case MINUS_ACT:
    {
        return first_value - second_value;
        break;
    }

    case MULT_ACT:
    {
        return first_value * second_value;
        break;
    }

    case DIV_ACT:
    {
        return first_value / second_value;
        break;
    }
    }
    return 0;
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
