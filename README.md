A Parallel C implementation of the Map function, using threads.

Though the map related code is generic, this particular executable takes 3 arguments:

- SIZE : how many numbers we'll be mapping.
- USE_THREADS : 0|1 -- (False, True)
- NTHREADS : how many threads, if any, we'll use.
