#include <pthread.h>        //Create POSIX threads.
#include <time.h>           //Wait for a random time.
#include <unistd.h>         //Thread calls sleep for specified number of seconds.
#include <semaphore.h>      //To create semaphores
#include <stdlib.h>         
#include <stdio.h>          //Input Output

pthread_t *Students;        //N threads running as Students.
pthread_t TA;               //Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;
int students_helped = 0;
int total_students = 0;

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

// Declared Semaphores and Mutex Lock
sem_t TA_sleep;
sem_t Chairs[3];
sem_t TA_signal;
pthread_mutex_t Chair_Mutex;

//Declared Functions
void *TA_Activity();
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
    int number_of_students;     //Variable taken from the user to create student threads. Default is 5 student threads.
    int id;
    srand(time(NULL));

    /*TODO
	//Initializing Mutex Lock and Semaphores.
	
     //hint: use sem_init() and pthread_mutex_init()
     
     */

    // Initializing Mutex Lock and Semaphores
    pthread_mutex_init(&Chair_Mutex, NULL);
    sem_init(&TA_sleep, 0, 0);
    sem_init(&TA_signal, 0, 0);
    for (int i = 0; i < 3; i++) {
        sem_init(&Chairs[i], 0, 0);
    }

    if (argc < 2)
    {
        printf("Number of Students not specified. Using default (5) students.\n");
        number_of_students = 5;
    }
    else
    {
        printf("Number of Students specified. Creating %d threads.\n", number_of_students);
        number_of_students = atoi(argv[1]);
    }
	
	total_students = number_of_students;
        
    //Allocate memory for Students
    Students = (pthread_t*) malloc(sizeof(pthread_t) * number_of_students);

    /*TODO
	//Creating one TA thread and N Student threads.
     //hint: use pthread_create

	//Waiting for TA thread and N Student threads.
     //hint: use pthread_join
     
     */

    // Creating one TA thread and N Student threads
    pthread_create(&TA, NULL, TA_Activity, NULL);
    for (id = 0; id < number_of_students; id++) {
        pthread_create(&Students[id], NULL, Student_Activity, (void*)&id);
        usleep(100);
    }

    // Waiting for TA thread and N Student threads
    pthread_join(TA, NULL);
    for (int i = 0; i < number_of_students; i++) {
        pthread_join(Students[i], NULL);
    }

    //Free allocated memory
    free(Students);
	printf("Program finished successfully.\n");

    return 0;
}

void *TA_Activity()
{
    //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()     
    do {
        printf("TA is sleeping.\n");
        sem_wait(&TA_sleep);

        while (1) {
            //lock chair count
            pthread_mutex_lock(&Chair_Mutex);

            //TA gets next student on chair
            if (ChairsCount == 0) {
                pthread_mutex_unlock(&Chair_Mutex);
                break;
            }

            // Help the next student
            printf("TA is helping a student.\n");
            sem_post(&Chairs[CurrentIndex]); // Signal the student on the next chair
            ChairsCount--;
            CurrentIndex = (CurrentIndex + 1) % 3;

            pthread_mutex_unlock(&Chair_Mutex);

            // Simulate helping a student
            sleep(2);
            printf("TA finished helping a student.\n");
            sem_post(&TA_signal); // Signal the student that help is complete
			
			pthread_mutex_lock(&Chair_Mutex);
			students_helped++;
			pthread_mutex_unlock(&Chair_Mutex);
			
			if (students_helped == total_students) {
				printf("All students have been helped, TA is done for the day.\n");
				return NULL;
			}
		}

    } while (1);

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

    while (1) {
        printf("Student %d needs help from the TA.\n", id);

        pthread_mutex_lock(&Chair_Mutex);
        if (ChairsCount < 3) {
            
            int chair_index = (CurrentIndex + ChairsCount) % 3;
            ChairsCount++;
            printf("Student %d is waiting on chair %d.\n", id, chair_index);
            pthread_mutex_unlock(&Chair_Mutex);

            sem_post(&TA_sleep);
            sem_wait(&Chairs[chair_index]);
            printf("Student %d leaves after getting help.\n", id);
            break;
        
        } else {
            pthread_mutex_unlock(&Chair_Mutex);
            printf("Student %d didn't find a chair. Will return later.\n", id);
            sleep(rand() % 5 + 1);
        }
    }

    return NULL;
}
