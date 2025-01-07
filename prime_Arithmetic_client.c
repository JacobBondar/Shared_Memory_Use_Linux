/*
 * Ex #2c: Client for Prime and Arithmetic Servers
 * ===============================================
 * Written by: Jacob Bondar.
 *
 * This program acts as a client for two servers: one that processes
 * prime numbers and another that evaluates arithmetic expressions.
 * Based on user input, the program sends requests to the appropriate
 * server via memory and displays the results received.
 *
 * Input: User commands ('p' for primes, 'a' for arithmetic) followed
 *        by respective data.
 * Output: Processed results (prime numbers or evaluated expressions)
 *         from the servers.
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
#include <unistd.h>

//-------------- const and enum section --------------------------------

const int MAX_INPUT = 100;
const int SIZE_OF_MEMORY = 102;

enum numbers_t {
	ZERO_NUM,
	ONE_NUM,
	TWO_NUM,
	THREE_NUM,
	FOUR_NUM
};

//-------------- prototypes section ------------------------------------

void do_prime(int** shm_ptr, pid_t pid);
void do_math(int** shm_ptr, pid_t pid);
int create_memory(char type);
int* connect_to_memory(int shm_id);
void read_data(int* first_value, int* second_value, int* action);
void insert_data(int** shm_ptr, int first_value, int second_value,
	int action);
void finish_program(int sig);

//-------------- main --------------------------------------------------

int main()
{
	signal(SIGINT, finish_program);

	int shm_id_act = create_memory('a');
	int shm_id_prime = create_memory('p');

	int* shm_ptr_act = connect_to_memory(shm_id_act);
	int* shm_ptr_prime = connect_to_memory(shm_id_prime);

	pid_t pid = getpid();

	while (1)
	{
		char c;
		scanf("%c", &c);

		if (c == 'p')
		{
			do_prime(&shm_ptr_prime, pid);
		}

		else if (c == 'a')
		{
			do_math(&shm_ptr_act, pid);
		}
	}

	exit(EXIT_SUCCESS);
}

//----------------------------------------------------------------------

/* Sends a numbers to the prime number server for processing.
 * The function receives: a pointer to a pointer to the memory and pid.
 * The function returns: void.
 */
void do_prime(int** shm_ptr, pid_t pid)
{
	int index = ONE_NUM, value = 0;
	scanf("%d", &value);

	for (int index = 1; value > 0 && index <= MAX_INPUT; index++)
	{
		(*shm_ptr)[index] = value;
		scanf("%d", &value);
	}
	(*shm_ptr)[ZERO_NUM] = pid;

	while ((*shm_ptr)[ZERO_NUM] != 1) {}

	index = 1;
	while ((*shm_ptr)[index] != 0)
	{
		if ((*shm_ptr)[index] != -1)
		{
			printf("%d ", (*shm_ptr)[index]);
		}
		index++;
	}
	puts("");
	memset(*shm_ptr, 0, SIZE_OF_MEMORY);
}

//----------------------------------------------------------------------

/* Sends an arithmetic expression to the arithmetic server for
 * 	processing.
 * The function receives: a pointer to a pointer to the memory and pid.
 * The function returns: void.
 */
void do_math(int** shm_ptr, pid_t pid)
{
	int first_value, second_value, action;

	read_data(&first_value, &second_value, &action);

	insert_data(shm_ptr, first_value, second_value, action);

	(*shm_ptr)[ZERO_NUM] = pid;
	while (*shm_ptr[ZERO_NUM] != 1) {}

	printf("%d\n", (*shm_ptr)[FOUR_NUM]);

	memset(*shm_ptr, 0, SIZE_OF_MEMORY);
}

//----------------------------------------------------------------------

/* Creates the memory.
 * The function receives: char type.
 * The function returns: memory id.
 */
int create_memory(char type)
{
	int shm_id;
	key_t key;

	if ((key = ftok(".", type)) == -1)
	{
		perror("Can't fork\n");
		exit(EXIT_FAILURE);
	}

	if ((shm_id = shmget(key, SIZE_OF_MEMORY, 0600)) == -1)
	{
		perror("Cant shmget\n");
		exit(EXIT_FAILURE);
	}
	return shm_id;
}

//----------------------------------------------------------------------

/* Connects to the memory.
 * The function receives: ID of the memory.
 * The function returns: pointer to the memory.
 */
int* connect_to_memory(int shm_id)
{
	int* shm_ptr = (int*)shmat(shm_id, NULL, 0);

	if (shm_ptr == (int*)-1)
	{
		perror("shmat failed: ");
		exit(EXIT_FAILURE);
	}
	return shm_ptr;
}

//----------------------------------------------------------------------

/* Read the data from the user.
 * The function receives: 3 int pointer.
 * The function returns: void.
 */
void read_data(int* first_value, int* second_value, int* action)
{
	char temp;

	scanf("%d", first_value);
	getchar(); // to cancel enter
	scanf("%c", &temp);
	scanf("%d", second_value);

	*action = temp;
}

//----------------------------------------------------------------------

/* Inserts the data to the memory.
 * The function receives: 3 integeres and a pointer to a pointer to the
 *  memory.
 * The function returns: void.
 */
void insert_data(int** shm_ptr, int first_value, int second_value,
	int action)
{
	(*shm_ptr)[ONE_NUM] = first_value;
	(*shm_ptr)[TWO_NUM] = action;
	(*shm_ptr)[THREE_NUM] = second_value;
}

//----------------------------------------------------------------------

/* Handles program termination by exiting.
 * The function receives: the signal number (SIGINT).
 * The function returns: void.
 */
void finish_program(int sig)
{
	exit(EXIT_SUCCESS);
}
