
#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>			//Input Output

pthread_t *Students;		//N threads running as Students.
pthread_t TA;				//Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;

/*TODO
 
//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
//An array of 3 semaphores to signal and wait chair to wait for the TA.
//A semaphore to signal and wait for TA's next student.
 
 //Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
 
 //hint: use sem_t and pthread_mutex_t
 
 */
sem_t TA_sleep;
sem_t Chairs[3];
sem_t TA_signal;
pthread_mutex_t Chair_Mutex;

//Declared Functions
void *TA_Activity();
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

    /*TODO
	//Initializing Mutex Lock and Semaphores.
	
     //hint: use sem_init() and pthread_mutex_init()
     
     */
	
	if(argc<2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);
		number_of_students = atoi(argv[1]);
	}
		
	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

    /*TODO
	//Creating one TA thread and N Student threads.
     //hint: use pthread_create

	//Waiting for TA thread and N Student threads.
     //hint: use pthread_join
     
     */

	//Free allocated memory
	free(Students); 
	return 0;
}

void *TA_Activity()
{     
     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	do {
		//TA is currently sleeping.
		sem_wait(&TA_sleep);
		// lock
		pthread_mutex_lock(&Chair_Mutex);
		//TA gets next student on chair.
		if (CurrentIndex > 0) {
			CurrentIndex--;
			// Signal next student
			sem_post(&TA_signal);
		}
		//unlock
		pthread_mutex_unlock(&Chair_Mutex);
		 //if chairs are empty, break the loop.
	} while(1);
	return NULL;
}

void *Student_Activity(void *threadID) 
{
    /*TODO
     
	//Student  needs help from the TA
	//Student tried to sit on a chair.
	//wake up the TA.
    // lock
     
	// unlock
    //Student leaves his/her chair.
	//Student  is getting help from the TA
	//Student waits to go next.
	//Student left TA room
    
    //If student didn't find any chair to sit on.
    //Student will return at another time
     
     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
			
	*/

	int id = *(int*) threadID;
	printf("Student %d needs help from the TA.\n", id);

	if (ChairsCount < 3) {
		ChairsCount++;
		printf("Student %d sat on a chair. Waiting for the TA to be free.\n", id);

		sem_post(&TA_sleep);

		pthread_mutex_unlock(&Chair_Mutex);

		sem_wait(&Students[id]);

		pthread_mutex_lock(&Chair_Mutex);
		ChairsCount--;
		printf("Student %d left his/her chair. Chairs now taken: %d\n", id, ChairsCount);
		pthread_mutex_unlock(&Chair_Mutex);

		sem_post(&TA_signal);
	}
	else {
		printf("Student %d didn't find any chair to sit on. Will return later.\n", id);
		pthread_mutex_unlock(&Chair_Mutex);
	}

}