#include<pthread.h>

/* pthread implementation of 1st reader/writer problem
 *  based on mutex and conditional variable
 */
typedef struct read_write_lock_s {
	pthread_mutex_t mutex;
	pthread_cond_t cv;
	int read_ctr;
	int  write_ctr;
}read_write_lock;

void rw_lock_init(read_write_lock *rw_lock)
{
	rw_lock->read_ctr = 0;
	rw_lock->write_ctr = 0;
	pthread_cond_init(&rw_lock->cv);
	pthread_mutex_init(&rw_lock->mutex);
}

void rw_lock_acquire_read(read_write_lock *rw_lock)
{
	pthread_mutex_lock(&rw_lock->mutex);
	while(rw_lock->write_ctr != 0) {  /* wait for writer */
		pthread_cond_wait(&rw_lock->cv, &rw_lock->mutex);
	}
	rw_lock->read_ctr++;
	pthread_mutex_unlock(&rw_lock->mutex);
}

void rw_lock_acquire_write(read_write_lock *rw_lock)
{
	pthread_mutex_lock(&rw_lock->mutex);
	while(rw_lock->write_ctr !=0 || rw_lock->read_ctr>0) {
		pthread_cond_wait(&rw_lock->cv, &rw_lock->mutex);
	}
	rw_lock->write_ctr=1;
	pthread_mutex_unlock(&rw_lock->mutex);
}

void rw_lock_release_read(read_write_lock *rw_lock)
{
	pthread_mutex_lock(&rw_lock->mutex);
	if(--rw_lock->read_ctr == 0) { /* no one is reading now; writer please hurry! */
		pthread_cond_broadcast(&rw_lock->cv);
	}
	pthread_mutex_unlock(&rw_lock->mutex);
}

void rw_lock_release_write(read_write_lock *rw_lock)
{
	pthread_mutex_lock(&rw_lock->mutex);
	rw_lock->write_ctr = 0;
	pthread_cond_broadcast(&rw_lock->cv); /* writer is done; readers in... */
	pthread_mutex_unlock(&rw_lock->mutex);
}
