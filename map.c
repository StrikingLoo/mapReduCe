#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// -------------Benchmark Functions -----------------
//for testing. Multiplies by two and wraps in void* type.
void* twice(void* number){
    int n = *((int*) number);
    int* res = malloc(sizeof(int));
    *res = 2*n;
    return res;
}

//second benchmark: do something more CPU intensive with each number.
void* naivePrime(void* number){
    int n = *((int*) number);
    int* res = malloc(sizeof(int));
    *res = 1;
    for(int i = 2; i < n; i++){
        if(n%i==0){ 
            *res=0;
            return res;
            }
    }
    return res;
}
// ---------------- Begin Map related code.--------------

// generic single-threaded map implementation using void* and pointer arithmetics.
void** map(void** things, void* (*f)(void*), int length){

    void** results = malloc(sizeof(void*)*length);
    
    for(int i = 0; i < length; i++){
        void* thing = things[i];
        void* result = (*f)(thing);
        results[i] = result;
    }
    
    return results;
}

// ------------------------ Concurrent map code ------------

struct map_argument {
    void** things;
    void** results;
    void* (*f)(void*);
    int from;
    int to;
    };

void init_map_argument(struct map_argument* argument, 
            void** things, void** results, void* (*f)(void*),
            int from, int to){
    argument->f = f;
    argument->things = things;
    argument->results = results;
    argument->from = from;
    argument->to = to;
}

void* chunk_map(void* argument){
    struct map_argument* arg = (struct map_argument*) argument;
    for(int i = arg->from; i < arg->to; i++){
        arg->results[i] = (*(arg->f))(arg->things[i]);
    }
    return NULL;
}

void** concurrent_map(void** things, void* (*f)(void*), int length,
                      int nthreads){

    void** results = malloc(sizeof(void*)*length);
    struct map_argument arguments[nthreads];
    pthread_t threads[nthreads];
    int chunk_size = length/nthreads;

    for(int j = 0 ; j < nthreads; j++){
        int from = j*chunk_size;
        struct map_argument* argument = &arguments[j];        
        init_map_argument(argument, things, results, f, from, from+chunk_size);
        pthread_create(&threads[j], NULL, chunk_map, (void*) argument);
    }


    for(int i = 0; i < length % nthreads; i++){
        int idx = chunk_size*nthreads + i;
        results[idx] = (*f)(things[idx]);
    }

    for(int k = 0; k < nthreads; k++){
        pthread_join(threads[k], NULL);
    }

    return results;
}



//printing indirect references helper.
void indirectShow(int** numbers, int N){
    for(int i = 0; i < N; i++){
        printf("%d ", *(numbers[i]));
    }
    printf("\n---\n");
}

int main(int argc, char** argv){
    const char* SIZE = argv[1];
    int N = atoi(SIZE);

    const char* use_threads = argv[2];
    int USE_THREADS = atoi(use_threads);
    int NTHREADS = 0;
    if(USE_THREADS){
        const char* nthreads = argv[3];
        NTHREADS = atoi(nthreads);
    }
    
    int** numbers = malloc(sizeof(int*)*N);
    for(int i = 0 ; i < N ; i++){
        int* n = malloc(sizeof(int));
        *n = i;
        numbers[i] = n;
    }
    
    int** resulting_numbers = NULL;
    if(USE_THREADS){
        void** is_prime = concurrent_map((void**) numbers, twice,N, NTHREADS);
        resulting_numbers = (int**) is_prime;
    } else {
        void** is_prime = map((void**) numbers, twice, N);
        resulting_numbers = (int**) is_prime;
    }
    
    /*
    int screws = 0;
    for(int i = 0; i < N; i++){
        if((*resulting_numbers_b[i])!=(*resultsulting_numbers[i])){
            printf("we screwed up. %d\n", i);
            screws++;
        }
    }
    if(screws==0){printf("all is right in the world.\n");}
    */

    return 0;
}
