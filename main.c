#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include "binary_sem.h"
#include "defines.h"	
#include "queue.h"
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/select.h>

/* ---------------------------------- 
Postac wejsciowych danych:
 | argv nr:                 |
 | 4 |       1      | 2 | 3 |
 |--------------------------|
 | Z |  buffer_size | X | Y |
 |--------------------------|

	 					---> Proces inicjalizujacy bufory, semafory i procesy, wykonujacy scenariusz testowy nr 1 <---
 */

int main(int argc, char ** argv){
	printf("================================================================\n");
	printf("\t\tTest\n");
	printf("================================================================\n");

	/* Rozmiar bufora: */
	int buffer_size;

	/* Identyfikatory semaforów: */
	int RW_B1_sem_id;
	int RW_B2_sem_id;
	int A1_sem_id;
	int A2_sem_id;
	int A3_sem_id;

	double pocz, kon, licznik, licznik2, licznik3;

	/* Rozmiar segmentu pamieci */
	int seg_size;

	/* Identyfiaktory dzielonych segmentów pamieci dla procesow: */
	int B1_seg_id = 0;
	int B2_seg_id = 0;

	int * B1_shared_memory;
	int * B2_shared_memory;

	if(argc < 2){
		printf("Za malo argumentow!\n");
		return 1;
	}
	else if(argc == 2){
		/* Wczytaj argumenty wejsciowe: */
		buffer_size = atoi(argv[1]);
		seg_size = (buffer_size + 1)*sizeof(int);
	}
	else	{
		return 2;
	}

	/* Zaalokuj pamieci dla buforow: */
	if(zaalokuj_bufory(&B1_seg_id, &B2_seg_id, seg_size) == 1){
		printf("Blad alokacji buforow!\n");
		return 1;
	}

	/* Zamontuj zaalokowane segmenty: */
	B1_shared_memory = (int *) shmat(B1_seg_id, 0, 0);
	B2_shared_memory = (int *) shmat(B2_seg_id, 0, 0);

	/* Ustal ich poczatkowy rozmiar rowny 0 */
	B1_shared_memory[0] = 0;
	B2_shared_memory[0] = 0;

	/* Zaalokuj semafory: */
	if(zaalokuj_semafory(&RW_B1_sem_id, &RW_B2_sem_id, &A1_sem_id, &A2_sem_id, &A3_sem_id) == 1){
		return 1;
	}

	/* Zainicjalizuj semafory: */
	binary_semaphore_initialize(RW_B1_sem_id);
	binary_semaphore_initialize(RW_B2_sem_id);
	binary_semaphore_initialize(A1_sem_id);
	binary_semaphore_initialize(A2_sem_id);
	binary_semaphore_initialize(A3_sem_id);

	setbuf(stdout, NULL);		//wylacz buforowanie wyjscia 
/*================================================================================================*/
/*							scenariusz testowy nr 1												  */
/*================================================================================================*/

	int tmp;
	const double N = 5;			//domyslna dlugosc dzialania main w minutach

	pocz = clock()/CLOCKS_PER_SEC;
	licznik2 = clock()/CLOCKS_PER_SEC;

	while(1){

		binary_semaphore_wait(RW_B1_sem_id);
		sleep(1);		// dla czytelniejszego wyswietlania dzialan testu
		printf("-> B1.size = %d\n", B1_shared_memory[0]);		// wyswietl rozmiar i zawartosc B1
		for(int j = 1; j < B1_shared_memory[0] + 1; j++){
			printf("%d, ", B1_shared_memory[j]);
		}
		binary_semaphore_post(RW_B1_sem_id);
		printf("\n");
		binary_semaphore_wait(RW_B2_sem_id);
		printf("-> B2.size = %d\n", B2_shared_memory[0]);		// wyswietl rozmiar i zawartosc B2
		for(int j = 1; j < B2_shared_memory[0] + 1; j++){
			printf("%d, ", B2_shared_memory[j]);
		}
		binary_semaphore_post(RW_B2_sem_id);
		printf("\n");

		/* Jesli uplynelo N minut to koniec testu */
		kon = clock()/CLOCKS_PER_SEC;
		if(kon - pocz > 60*N){
			printf("\t\tKoniec testu\n");
			system("sudo killall proc_A1");
			system("sudo killall proc_A2");
			system("sudo killall proc_A3");
			break;
		}
	}

	/* Zwolnij zasoby przed zamknieciem programu */
	shmdt(B1_shared_memory);
	shmdt(B2_shared_memory);
	shmctl(B1_seg_id, IPC_RMID, 0);
	shmctl(B2_seg_id, IPC_RMID, 0);
	binary_semaphore_deallocate(RW_B1_sem_id);
	binary_semaphore_deallocate(RW_B2_sem_id);
	binary_semaphore_deallocate(A1_sem_id);
	binary_semaphore_deallocate(A2_sem_id);
	binary_semaphore_deallocate(A3_sem_id);

	return 0;
}