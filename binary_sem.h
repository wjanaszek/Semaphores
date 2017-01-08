#ifndef binary_sem_h
#define binary_sem_h

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include "defines.h"

/* Definiujemy semafor: */
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

/* Alokacja semafora: */
int binary_semaphore_allocation (key_t key, int sem_flags){
	return semget (key, 1, sem_flags);
}

/* Dealokacja: */
int binary_semaphore_deallocate (int semid){
	union semun ignored_argument;
	return semctl (semid, 1, IPC_RMID, ignored_argument);
}

/* Inicjalizacja: */
int binary_semaphore_initialize (int semid){
	union semun argument;
	unsigned short values[1];
	values[0] = 1;
	argument.array = values;
	return semctl (semid, 0, SETALL, argument);
}

int binary_semaphore_setVal(int semid, int value){
	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = value;
	operations[0].sem_flg = SEM_UNDO;
	return semop(semid, operations, 1);
}
/* Lock(): */
int binary_semaphore_wait(int semid){
	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = -1;
	operations[0].sem_flg = SEM_UNDO;
	return semop(semid, operations, 1);
}

/* Unlock(): */
int binary_semaphore_post(int semid){
	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = 1;
	operations[0].sem_flg = SEM_UNDO;
	return semop(semid, operations, 1);
}

int sem_getVal(int semid) {
	return semctl(semid, 0, GETVAL, 0);
}

int zaalokuj_bufory(int * B1_seg_id, int * B2_seg_id, int seg_size){
	if((*B1_seg_id = shmget(B1_key + 1, seg_size, IPC_CREAT | 0666)) < 0){
		printf("Blad podczas tworzenia segmentu B1!\n");
		return 1;
	}
	if((*B2_seg_id = shmget(B2_key + 1, seg_size, IPC_CREAT | 0666)) < 0){
		printf("Blad podczas tworzenia segmentu B2!\n");
		return 1;
	}
	return 0;
}

int zaalokuj_semafory(int * RW_B1_sem_id,
					int * RW_B2_sem_id,
					int * A1_sem_id,
					int * A2_sem_id,
					int * A3_sem_id){
	if((*RW_B1_sem_id = binary_semaphore_allocation(RW_B1_sem_key, IPC_CREAT | 0666)) < 0){
		printf("Blad podczas alokacji semafora dla bufora B1!\n");
		return 1;
	}
	if((*RW_B2_sem_id = binary_semaphore_allocation(RW_B2_sem_key, IPC_CREAT | 0666)) < 0){
		printf("Blad podczas alokacji semafora dla bufora B2!\n");
		return 1;
	}
	if((*A1_sem_id = binary_semaphore_allocation(A1_sem_key, IPC_CREAT | 0666)) < 0){
		printf("Blad podczas alokacji semafora dla procesu A1!\n");
		return 1;
	}
	if((*A2_sem_id = binary_semaphore_allocation(A2_sem_key, IPC_CREAT | 0666)) < 0){
		printf("Blad podcas alokacji semafora dla procesu A2!\n");
		return 1;
	}
	if((*A3_sem_id = binary_semaphore_allocation(A3_sem_key, IPC_CREAT | 0666)) < 0){
		printf("Blad podczas alokacji semafora dla procesu A3!\n");
		return 1;
	}
	return 0;
}

#endif