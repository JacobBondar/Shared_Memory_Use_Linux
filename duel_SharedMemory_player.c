/*
 * Ex #1b: A duel for the higher value (The players).
 * ==========================================================
 * Written by: Jacob Bondar.
 *
 * This program implements a process's role in the competitive game.
 * Each process generates random numbers and sends them to the host
 * process via memory. It then receives feedback from the host to update
 * counters for wins, losses, and ties. The process continues until a
 * termination signal is received or a specific condition is met, at
 * which point the process logs its summary and exits.
 *
 * Input: In the vector arguments (argv) we recieve 2 things:
 * 			1. The id of the process.
 * 			2. The seed.
 *
 * Output: The summary of the process points.
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

//-------------- global variables --------------------------------------

bool finish = false;

//-------------- const and enum section --------------------------------

const int EQUAL = 0;
const int HIGHER = 1;
const int LOWER = -1;
const int FINISED = 100;
const int DIVIDE = 10;
const long ALLOWED = 1;
const int SHM_SIZE = 5;
const int START_ROUND = -1;
const int FIRST_PROCESS = 0;
const int SECOND_PROCESS = 1;

enum result_t {
	EQUAL_RES,
	FIRST_HIGHER_RES,
	SECOND_HIGHER_RES
};

enum numbers_t {
	ZERO_NUM,
	ONE_NUM,
	TWO_NUM,
	THREE_NUM,
	FOUR_NUM
};

//-------------- prototypes section ------------------------------------

void start_run(int shm_id, int id);
void finish_program(int sig);
void update_counters_after_read(int num_got, int* zero_count,
	int* one_count, int* minus_one_count, int id);
void print_killed(int id, int minus_one_count, int zero_count,
	int one_count, int** shm_ptr);

//-------------- main --------------------------------------------------

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		perror("Not enough arguments!");
		exit(EXIT_FAILURE);
	}
	signal(SIGUSR1, finish_program);

	key_t key;
	int shm_id;

	if ((key = ftok(".", '5')) == -1)
	{
		perror("Can't fork\n");
		exit(EXIT_FAILURE);
	}

	if ((shm_id = shmget(key, SHM_SIZE, 0600)) == -1)
	{
		perror("Cant shmget\n");
		exit(EXIT_FAILURE);
	}

	int id = atoi(argv[1]);
	srand(atoi(argv[2]));

	start_run(shm_id, id);

	exit(EXIT_SUCCESS);
}

//----------------------------------------------------------------------

/* The function updates counters based on the value received from the
 *  main process.
 * The function receives: the received value and pointers to three
 *  counters for -1, 0, and 1.
 * The function returns: void.
 */
void update_counters_after_read(int num_got, int* zero_count,
	int* one_count, int* minus_one_count, int id)
{
	switch (num_got)
	{
	case EQUAL_RES:
	{
		(*zero_count)++;
		break;
	}

	case FIRST_HIGHER_RES:
	{
		if (id == FIRST_PROCESS)
		{
			(*one_count)++;
		}

		else if (id == SECOND_PROCESS)
		{
			(*minus_one_count)++;
		}

		break;
	}

	case SECOND_HIGHER_RES:
	{
		if (id == SECOND_PROCESS)
		{
			(*one_count)++;
		}

		else if (id == FIRST_PROCESS)
		{
			(*minus_one_count)++;
		}
		break;
	}
	}
}

//----------------------------------------------------------------------

/* The function runs the whole game, generates random number, recieves
 * input, and continues until certain condition.
 * The function receives: name of the queue, and the id.
 * The function returns: void.
 */
void start_run(int shm_id, int id)
{
	int* shm_ptr = (int*)shmat(shm_id, NULL, 0);
	if (shm_ptr == (int*)-1)
	{
		perror("shmat failed: ");
		exit(EXIT_FAILURE);
	}
	shm_ptr[id + THREE_NUM] = getpid();

	int zero_count = 0, minus_one_count = 0, one_count = 0, num_got = 0;

	while (true)
	{
		while (shm_ptr[ZERO_NUM] != START_ROUND) {}

		shm_ptr[id + ONE_NUM] = rand() % 9 + 1;

		while (shm_ptr[ZERO_NUM] == START_ROUND) {}

		num_got = shm_ptr[ZERO_NUM];
		update_counters_after_read(num_got, &zero_count, &one_count,
			&minus_one_count, id);

		shm_ptr[id + THREE_NUM] = 1;

		if (finish || zero_count == FINISED)
		{
			printf("Player #%d was killed: %d %d %d\n", id,
				minus_one_count, zero_count, one_count);
			shm_ptr[id + ONE_NUM] = -1;
			exit(EXIT_SUCCESS);
		}
	}
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
