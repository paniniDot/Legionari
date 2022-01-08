
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "DBGpthread.h"
#include "printerror.h"

#define RASOIO 0
#define SCODELLA 1

#define RASOI 1
#define SCODELLE 2
#define LEGIONARI 10

int rasoiRimanenti = RASOI;
int scodelleRimanenti = SCODELLE;
pthread_mutex_t mutexRasoio;
pthread_mutex_t mutexScodella;
pthread_cond_t condRasoio;
pthread_cond_t condScodella;

pthread_mutex_t *mutex( intptr_t indice ) {
    return ( indice == RASOIO ) ? &mutexRasoio : &mutexScodella;
}

pthread_cond_t *filaControllata( intptr_t indice ) {
    return (indice == RASOIO ) ? &condRasoio : &condScodella;
}

int oggettoControllato( intptr_t indice ) {
    return ( indice == RASOIO ) ? rasoiRimanenti : scodelleRimanenti;
}

void *sottoufficiale( void *arg ) {  
    char Slabel[128];
    sprintf( Slabel, "sottoufficiale addetto %s", ( ( intptr_t )arg == RASOIO ) ? "ai rasoii" : "alle scodelle" );
    
    while( 1 ) {
        DBGpthread_mutex_lock( mutex( ( intptr_t )arg ), Slabel );
        if( oggettoControllato( ( intptr_t )arg ) > 0 ) {
            /* printf( "%s: può darlo (rimanenti = %d)\n", Slabel, oggettoControllato( ( intptr_t )arg )); */
            DBGpthread_cond_broadcast( filaControllata( ( intptr_t )arg ), Slabel );
        } else {
            /* printf( "%s: NON può darlo (rimanenti = %d)\n", Slabel, oggettoControllato( ( intptr_t )arg )); */
        }
        DBGpthread_mutex_unlock( mutex( ( intptr_t )arg ), Slabel );
    }
    pthread_exit( NULL );
}

void *legionario( void *arg ) {
    
    char Llabel[128];
    sprintf( Llabel, "legionario %" PRIiPTR "", ( intptr_t )arg );

    while( 1 ) {
        
        DBGpthread_mutex_lock( &mutexRasoio, Llabel );
        printf( "%s: si mette in coda per il rasoio\n", Llabel );
        DBGpthread_cond_wait( &condRasoio, &mutexRasoio, Llabel );
        printf( "%s: ha preso il rasoio\n", Llabel );
        rasoiRimanenti--;
        DBGpthread_mutex_unlock( &mutexRasoio, Llabel );
        
        DBGpthread_mutex_lock( &mutexScodella, Llabel );
        printf( "%s: si mette in coda per la scodella\n", Llabel );
        DBGpthread_cond_wait( &condScodella, &mutexScodella, Llabel );
        printf( "%s: ha preso la scodella\n", Llabel );
        scodelleRimanenti--;
        DBGpthread_mutex_unlock( &mutexScodella, Llabel );

        /* si può radere porcanna la madonna */

        DBGpthread_mutex_lock( &mutexRasoio, Llabel );
        printf( "%s: ha finito di radersi, restituisce le cose\n", Llabel );
        rasoiRimanenti++;
        DBGpthread_cond_broadcast( &condRasoio, Llabel );
        DBGpthread_mutex_unlock( &mutexRasoio, Llabel );
        
        DBGpthread_mutex_lock( &mutexScodella, Llabel );  
        scodelleRimanenti++;
        DBGpthread_cond_broadcast( &condScodella, Llabel );
        DBGpthread_mutex_unlock( &mutexScodella, Llabel );
    }
    pthread_exit( NULL );
}

int main( void ) {
    int rc;
    pthread_t th;
    intptr_t i;

    DBGpthread_mutex_init( &mutexRasoio, NULL, "Main" );
    DBGpthread_mutex_init( &mutexScodella, NULL, "Main" );
    DBGpthread_cond_init( &condRasoio, NULL, "Main" );
    DBGpthread_cond_init( &condScodella, NULL, "Main" );

    rc = pthread_create( &th, NULL, sottoufficiale, (void *)RASOIO );
    if( rc != 0 ) {
        PrintERROR_andExit( rc, "Creazione sottoufficiale" );
    }

    rc = pthread_create( &th, NULL, sottoufficiale, (void *)SCODELLA );
    if( rc != 0 ) {
        PrintERROR_andExit( rc, "Creazione sottoufficiale" );
    }

    for( i = 0; i < LEGIONARI; i++ ) {
        rc = pthread_create( &th, NULL, legionario, ( void *)i );
        if( rc != 0 ) {
            PrintERROR_andExit( rc, "Creazione sottoufficiale" );
        }
    }
    pthread_exit( NULL ); 
}