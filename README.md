# Shared_Memory_Use_Linux

File: duel_SharedMemory_host.c 

Ex #1a: A duel for the higher value (The host).
==========================================================
Written by: Jacob Bondar.

This program simulates a competitive game between two processes 
 managed by a host process. The host compares between two inputs he 
 recieved from the processes and determine a winner for each round. 
 Scores are updated based on the results, and the game ends when one 
 player wins 120 rounds or both achieve 100 ties.

Compile: gcc -Wall duel_SharedMemory_host.c –o duel_SharedMemory_host
Run: ./duel_SharedMemory_host

Input: void.
 
Output: The winner (if there is one), and the two compiting 
        process summary.

-----------------------------------------------------------

File: duel_SharedMemory_player.c 

Ex #1b: A duel for the higher value (The players).
==========================================================
Written by: Jacob Bondar.

This program implements a process's role in the competitive game. 
 Each process generates random numbers and sends them to the host
 process via memory. It then receives feedback from the host to update
 counters for wins, losses, and ties. The process continues until a
 termination signal is received or a specific condition is met, at 
 which point the logs its summary and exits.

Compile: gcc -Wall duel_SharedMemory_player.c –o duel_SharedMemory_player
Run: ./duel_SharedMemory_player <id> <seed>

Input: In the vector arguments (argv) we recieve 2 things:
		1. The id of the process.
		2. The seed.
 
Output: The summary of the process points.

-----------------------------------------------------------

File: prime.c 

Ex #2a: Prime Number Filter via Memory
==========================================================
Written by: Jacob Bondar.

This program creates a memory to receive numbers from 
 clients, processes the numbers to extract prime numbers, and 
 sends the processed result back to the clients. The program runs 
 continuously until terminated using a SIGINT signal, at which point 
 the memory is safely removed.

Compile: gcc -Wall prime.c –o prime
Run: ./prime

Input: void (getting values threw the memory).
 
Output: void (sending values threw the memory).

-----------------------------------------------------------

File: arithmetic.c 

Ex #2b: Arithmetic Expression Processor via Memory
==========================================================
Written by: Jacob Bondar.

This program receives arithmetic expressions from clients through 
 a memory, evaluates the result of the expressions, and sends 
 the results back to the clients. It operates in an infinite loop 
 until terminated with a SIGINT signal, at which point the memory 
 is safely removed.

Compile: gcc -Wall arithmetic.c –o arithmetic
Run: ./arithmetic

Input: void (getting values threw the memory).
 
Output: void (sending values threw the memory).

-----------------------------------------------------------

File: prime_Arithmetic_client.c 

Ex #2c: Client for Prime and Arithmetic Servers
==========================================================
Written by: Jacob Bondar.

This program acts as a client for two servers: one that processes 
 prime numbers and another that evaluates arithmetic expressions. 
 Based on user input, the program sends requests to the appropriate 
 server via message queues and displays the results received.
 When typing 'p', you need to follow with 'enter', and then for every
 new number, you need to press 'enter' again, until zero.

Compile: gcc -Wall prime_Arithmetic_client.c –o prime_Arithmetic_client
Run: ./prime_Arithmetic_client

Input: User commands ('p' for primes, 'a' for arithmetic) followed 
       by respective data.
 
Output: Processed results (prime numbers or evaluated expressions) 
       from the servers.
