/*
 * Ex #1a: A duel for the higher value (The host).
 * ==========================================================
 * Written by: Jacob Bondar.
 *
 * This program simulates a competitive game between two processes
 * managed by a host process. The host compares between two inputs he
 * recieved from the processes and determine a winner for each round.
 * Scores are updated based on the results, and the game ends when one
 * player wins 120 rounds or both achieve 100 ties.
 *
 * Input: void.
 *
 * Output: The winner (if there is one), and the process who competes
 *         summary.
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
#include <unistd.h>
#include <sys/shm.h> 

//-------------- const and enum section --------------------------------
const int EQUAL = 0;
const int HIGHER = 1;
const int LOWER = -1;
const int WON = 120;
const int FINISED = 100;
const int DIVIDE = 10;
const int NUM_OF_PLAYERS = 2;
const int ALLOWED = 0;
const long FIRST_PROCESS = 0;
const long SECOND_PROCESS = 1;
const int SHM_SIZE = 5;
const int ONE = 1;
const int MINUS_ONE = -1;
const int START_ROUND = -1;
const int TIE = 0;
const int FIRST_WON = 1;
const int SECOND_WON = 2;

enum numbers_t {
	ZERO_NUM,
	ONE_NUM,
	TWO_NUM,
	THREE_NUM,
	FOUR_NUM
};



//-------------- prototypes section ------------------------------------

void run(int msgid);
void finish_program(int sig);
void update_values(int* won, int* result, int insert);
void print_won(int id, pid_t first_process, pid_t second_process);

bool finish = false;

//-------------- main --------------------------------------------------

int main()
{
	key_t key;
	int shm_id;

	if ((key = ftok(".", '5')) == -1)
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

	run(shm_id);

	exit(EXIT_SUCCESS);
}

//----------------------------------------------------------------------

/* The function updates the values of the result and win counters for
 *  the two process who compete based on the game outcome.
 * The function receives: pointers to the win counter and the results
 *  of the  two process who compete.
 * The function returns: void.
 */
void update_values(int* won, int* result, int insert)
{
	(*won)++;
	*result = insert;
}

//----------------------------------------------------------------------

/* The function prints a message when a process wins and sends
 *  a termination signal to both processes.
 * The function receives: two process IDs and the ID of the
 *  winning process.
 * The function returns: void.
 */
void print_won(int id, pid_t first_process, pid_t second_process)
{
	printf("player #%d won\n", id);
	kill(first_process, SIGUSR1);
	kill(second_process, SIGUSR1);
	finish = true;
}

//----------------------------------------------------------------------

/* The function handles the actions of the host process, including
 *  reading data from processes and determining the winner.
 * The function receives: id of the memory section.
 * The function returns: void.
 */
void run(int shm_id)
{
	int* shm_ptr = (int*)shmat(shm_id, NULL, 0);
	if (shm_ptr == (int*)-1)
	{
		perror("shmat failed: ");
		exit(EXIT_FAILURE);
	}
	memset(shm_ptr, 0, SHM_SIZE);

	while (shm_ptr[THREE_NUM] == 0 || shm_ptr[FOUR_NUM] == 0) {}

	pid_t first_process = shm_ptr[THREE_NUM];
	pid_t second_process = shm_ptr[FOUR_NUM];
	shm_ptr[THREE_NUM] = shm_ptr[FOUR_NUM] = 0;

	int first_won = 0, second_won = 0,
		first_value = 0, second_value = 0, result = 0;

	while (!finish)
	{
		result = 0;

		shm_ptr[ZERO_NUM] = START_ROUND;

		while (shm_ptr[ONE_NUM] == 0 || shm_ptr[TWO_NUM] == 0) {}
		if (shm_ptr[ONE_NUM] == MINUS_ONE)
		{
			break;
		}

		first_value = shm_ptr[ONE_NUM];
		second_value = shm_ptr[TWO_NUM];

		if (second_value > first_value)
		{
			update_values(&second_won, &result, SECOND_WON);
		}

		else if (second_value < first_value)
		{
			update_values(&first_won, &result, FIRST_WON);
		}

		if (first_won == WON)
		{
			print_won(FIRST_PROCESS, first_process, second_process);
		}

		else if (second_won == WON)
		{
			print_won(SECOND_PROCESS, first_process, second_process);
		}

		shm_ptr[ZERO_NUM] = result;
		while (shm_ptr[THREE_NUM] != ONE || shm_ptr[FOUR_NUM] != ONE) {}

		shm_ptr[ONE_NUM] = shm_ptr[TWO_NUM] = shm_ptr[THREE_NUM] =
			shm_ptr[FOUR_NUM] = 0;
	}

	sleep(1);
	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
	{
		perror("shmctl failed");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

//----------------------------------------------------------------------

/* The function handles SIGUSR1 signals by setting a global flag to
 * indicate termination.
 * The function receives: the signal number.
 * The function returns: void.
 */
void finish_program(int sig)
{
	finish = true;
}
