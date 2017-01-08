#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include "binary_sem.h"
#include "defines.h"	
#include "queue.h"

/* ========= */
/* Proces A3 */
/* ========= */
int main(int argc, char ** argv){
	int X, Y, Z;
	int buffer_size;
	int B1_seg_id;
	int B2_seg_id;
	int * queue_B1;
	int * queue_B2;

	if(argc == 5){
		buffer_size = atoi(argv[1]);
		X = atoi(argv[2]);
		Y = atoi(argv[3]);
		Z = atoi(argv[4]);
	}
	else	{
		printf("Blad!\n");
		return 1;
	}

	int seg_size = (buffer_size + 1)*sizeof(int);
	/* Deklaracje semaforow do podlaczenia do programu testujacego */
	int RW_B1_sem_id;
	int RW_B2_sem_id;
	int A1_sem_id;
	int A2_sem_id;
	int A3_sem_id;

	/* Podlaczenie do bufora B1: */
	if((B1_seg_id = shmget(B1_key + 1, seg_size, 0666)) < 0){
		printf("A3 - Blad przy podlaczaniu do B1!\n");
		return 1;
	}
	queue_B1 = (int *) shmat(B1_seg_id, 0, 0);
	//printf("A3 - B1_seg_id = %d\n", B1_seg_id);
	if((B2_seg_id = shmget(B2_key + 1, seg_size, 0666)) < 0){
		printf("A3 - Blad przy podlaczaniu do B2!\n");
		return 1;
	}
	queue_B2 = (int *) shmat(B2_seg_id, 0, 0);
	/* Alokacja semaforow: */
	if((RW_B1_sem_id = binary_semaphore_allocation(RW_B1_sem_key, 0666)) < 0){
		printf("A3 - Blad przy podlaczaniu do RW_B1_sem!\n");
		return 1;
	}
	if((RW_B2_sem_id = binary_semaphore_allocation(RW_B2_sem_key, 0666)) < 0){
		printf("A3 - Blad przy podlaczaniu do RW_B2_sem!\n");
		return 1;
	}
	if((A1_sem_id = binary_semaphore_allocation(A1_sem_key, 0666)) < 0){
		printf("A3 - Blad przy podlaczaniu do A3.1_sem!\n");
		return 1;
	}
	if((A2_sem_id = binary_semaphore_allocation(A2_sem_key, 0666)) < 0){
		printf("A3 - Blad przy podlaczaniu do A3.2_sem!\n");
		return 1;
	}
	if((A3_sem_id = binary_semaphore_allocation(A3_sem_key, 0666)) < 0){
		printf("A3 - Blad przy podlaczaniu do A3.3_sem!\n");
		return 1;
	}


	int item = 0;
	int B1_size, B2_size = 0;
	setbuf(stdout, NULL);
/*================================================================================================*/
/*											work												  */
/*================================================================================================*/
	while(1){
		/* Zablokuj dostep do buforow B1, B2 */
		binary_semaphore_wait(RW_B1_sem_id);
		binary_semaphore_wait(RW_B2_sem_id);

		/* Sprawdz warunek dla procesu A3 */
		if(queue_B2[0] > Z){
			binary_semaphore_wait(A3_sem_id);
			item = queuePop(queue_B2);
			if(item % 3 == 0 && queue_B1[0] < buffer_size){
				queueInsert(queue_B1, item);
			}
			binary_semaphore_post(A3_sem_id);
		}

		/* Sprawdz warunek dla procesu A1 */
		if(queue_B1[0] < X){
			binary_semaphore_post(A1_sem_id);
		}

		/* Sprawdz warunek dla procesu A2 */
		if(queue_B1[0] > 0 && queue_B2[0] < Y){
			binary_semaphore_post(A2_sem_id);
		}

		/* Zwolnij dostep do buforow */
		binary_semaphore_post(RW_B2_sem_id);
		binary_semaphore_post(RW_B1_sem_id);
	}

	shmdt(queue_B1);
	shmdt(queue_B2);
	return 0;
}