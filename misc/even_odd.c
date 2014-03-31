#include<stdio.h>
#include<pthread.h>

#define MAX 20
int n = 0;

pthread_mutex_t m;
pthread_cond_t  c;

void *even(void *d)
{
	while (1) {
		pthread_mutex_lock(&m);
		while( n %2 == 0) {
			pthread_cond_wait(&c, &m);
		}

		printf("Even : %d\n", ++n);

		pthread_cond_signal(&c);
		pthread_mutex_unlock(&m);

		if (n >= MAX) break;
	}

	return 0;
}

void *odd(void *d)
{
	int i = 0;

	while (1) {
		pthread_mutex_lock(&m);
		while( n %2 == 1) {
			pthread_cond_wait(&c, &m);
		}

		printf("Odd : %d\n", ++n);

		pthread_cond_signal(&c);
		pthread_mutex_unlock(&m);

		if (n >= MAX) break;
	}

	return 0;
}


int main()
{
	pthread_t e, o;
	pthread_attr_t attr;

//	pthread_attr_init(&attr);

	pthread_mutex_init(&m,NULL);
	pthread_cond_init(&c,NULL);

	pthread_create(&e, NULL, even, NULL );
	pthread_create(&o, NULL, odd, NULL );

	pthread_join(e, NULL);	
	pthread_join(o, NULL);	
}
