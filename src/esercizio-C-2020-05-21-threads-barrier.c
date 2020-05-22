/*
 un processo apre un file in scrittura (se esiste già sovrascrive i contenuti del file),
 poi lancia M (=10) threads.

"fase 1" vuol dire: dormire per un intervallo random di tempo compreso tra 0 e 3 secondi,
poi scrivere nel file il messaggio: "fase 1, thread id=, sleep period= secondi"

"fase 2" vuol dire: scrivere nel file il messaggio "fase 2, thread id=, dopo la barriera"
poi dormire per 10 millisecondi, scrivere nel file il messggio "thread id= bye!".

per ogni thread: effettuare "fase 1", poi aspettare che tutti i thread abbiano completato
la fase 1 (barriera: little book of semaphores, pag. 29); poi effettuare "fase 2" e terminare il thread.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>
#include <pthread.h>

#define M 10

pthread_barrier_t thread_barrier;
int count;


void * thread_function(void * arg){

	int* thread_id = (int*)arg;

	time_t t = time(NULL);
	srand(t);
	int sleep_seconds = rand() % 4;

	sleep(sleep_seconds);

	printf("fase 1, thread id= %d, sleep period= %d\n", *thread_id, sleep_seconds);

	int s;

	s = pthread_barrier_wait(&thread_barrier);


	printf("fase 2, thread id=, dopo la barriera\n");

	return NULL;
}

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }

int main() {

	int s;
	int fd;
	pthread_t threads[M];

	if ((fd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) == -1){

	   	perror("open");
	   	exit(EXIT_FAILURE);
	}

	if (dup2(fd, STDOUT_FILENO) == -1) {

	   	perror("problema con dup2");
    	exit(EXIT_FAILURE);
    }

	close(fd);

	s = pthread_barrier_init(&thread_barrier, NULL, M);
	CHECK_ERR(s, "pthread_barrier_init");

	for(int i = 0; i < M; i++){

		s = pthread_create(&threads[i], NULL, thread_function, (void*)(&i));

		if(s != 0){
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	for (int i=0; i < M; i++) {
		s = pthread_join(threads[i], NULL);

		if (s != 0) {
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}
	}

	s = pthread_barrier_destroy(&thread_barrier);
	CHECK_ERR(s,"pthread_barrier_destroy")


	return 0;
}

