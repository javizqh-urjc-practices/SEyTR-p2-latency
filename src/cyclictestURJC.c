#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>

#define THREAD_ITERATIONS 5000
#define THREAD_PRIORITY 99

#define EXEC_TIME_SEC 60
#define SEC_TO_NSEC 1000000000
#define MSEC_TO_NSEC 1000000

struct timespec init;

void * thread_function(void * arg);
long int get_nanoseconds(struct timespec start, struct timespec end);
long int get_seconds(struct timespec start, struct timespec end);
int write_legend(int csv_fd);
int write_to_csv(int csv_fd, int cpu, int iterations, long latency);

// ---------------- Linked List --------------------------------

struct latency_data {
    long avg;
    long max;
};

struct info {
    int cpu;
    int iteration;
    long latency;
    struct info * next;
};

struct info * new_info(int cpu);
struct info * add_info(struct info *old_info);
void print_info(int csv_fd, struct info *first_info, struct latency_data *data);

struct info * new_info(int cpu) {
    struct info *create_info = malloc(sizeof(struct info));
    if (create_info == NULL) {
        err(EXIT_FAILURE, "Failed to allocate info");
    }
    memset(create_info, 0, sizeof(struct info));

    create_info->cpu = cpu;
    create_info->iteration = 0;
    create_info->latency = 0;
    create_info->next = NULL;

    return create_info;
}

struct info * add_info(struct info *last_info) {
    struct info *last = last_info;
    struct info *new = new_info(last_info->cpu);

    last->next = new;
    new->iteration = last->iteration + 1;
    
    return new;
}

void print_info(int csv_fd, struct info *first_info, struct latency_data *data) {
    long long total = 0, avg = 0, max = 0, n_values = 0;
    int cpu = first_info->cpu;
    struct info *curr = first_info;
    struct info *to_free = curr;

    for (;curr != NULL;) {
        if (curr->latency != 0) {
            n_values++;
            total += curr->latency;
            if (curr->latency > max) max = curr->latency;
            write_to_csv(csv_fd, curr->cpu, curr->iteration, curr->latency);
            to_free = curr;
            curr = curr->next;
            free(to_free);
        } else {
            free(curr);
            break;
        }
    }
    avg = total / n_values;
    printf("[%d]\tlatencia media = %.9ld ns. | max = %.9ld ns\n",cpu, avg, max);
    data->avg += avg;
    if (max > data->max) data->max = max;
    return;
}

// -------------------------------------------------------------

int main() {
    int N_CORES= (int) sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[N_CORES];
    int thread_ids[N_CORES];
    struct sched_param priority;
    cpu_set_t cpuset;
    void *data_thread;
    static int32_t latency_target_value = 0;
    int csv_fd = open("cyclictestURJC.csv", O_CREAT | O_RDWR | O_TRUNC, 0777);
    int latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    struct latency_data *lt_data = malloc(sizeof(struct latency_data));

    if (latency_target_fd < 0 || csv_fd < 0) {
        err(EXIT_FAILURE, "Couldn't open the necessary files");
    }

    if (lt_data == NULL) {
        err(EXIT_FAILURE, "Memory allocation failed\n");
    }

    lt_data->avg = 0;
    lt_data->max = 0;
    write_legend(csv_fd);
    write(latency_target_fd, &latency_target_value, 4);

    CPU_ZERO(&cpuset);
    priority.sched_priority = THREAD_PRIORITY;

    clock_gettime(CLOCK_MONOTONIC, &init);
    for (int i = 0; i < N_CORES; i++) {
        thread_ids[i] = i + 1;
        CPU_SET(i, &cpuset);
        threads[i] = pthread_self();
        if (pthread_setschedparam(threads[i], SCHED_FIFO, &priority) == 0) {
            err(EXIT_FAILURE, "Could not set scheduler priority");
        }
        if (pthread_setaffinity_np(threads[i], sizeof(cpuset), &cpuset) == 0) {
            err(EXIT_FAILURE, "Could not set cpu affinity");
        }
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }

    for (int i = 0; i < N_CORES; i++) {
        pthread_join(threads[i], &data_thread);
        struct info * data = (struct info *) data_thread;
        print_info(csv_fd, data, lt_data);
    }

    printf("\nTotal\tlatencia media = %.9ld ns. | max = %.9ld ns\n", 
           lt_data->avg / N_CORES, lt_data->max);

    close(csv_fd);
    close(latency_target_fd);

    return 0;
}

void * thread_function(void * arg) {
    int thread_id = *(int *) arg;
    struct timespec start, end, sleep;
    struct info * data = new_info(thread_id - 1);
    struct info * last_data = data;

    sleep.tv_sec = 0;
    sleep.tv_nsec = MSEC_TO_NSEC;
    
    do {
        clock_gettime(CLOCK_MONOTONIC, &start);
        nanosleep(&sleep, NULL);
        clock_gettime(CLOCK_MONOTONIC, &end);
        last_data->latency = get_nanoseconds(start, end) - sleep.tv_nsec;
        last_data = add_info(last_data);
    } while (get_seconds(init, end) < EXEC_TIME_SEC);
    
    pthread_exit(data);
}

// Returns the number of nanoseconds passed
long int get_nanoseconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * SEC_TO_NSEC + (end.tv_nsec - start.tv_nsec);
}

// Returns the number of seconds passed
long int get_seconds(struct timespec start, struct timespec end) {
    return ((end.tv_sec - start.tv_sec) * 100 + (end.tv_nsec - start.tv_nsec)
        / 10000000) / 100;
}

int write_legend(int csv_fd) {
    char * buf = malloc(1024);
    int msg_len = 0;

    sprintf(buf, "CPU,NUMERO_ITERACION,LATENCIA\n");
    msg_len = strlen(buf);
    write(csv_fd, buf, msg_len);
    free(buf);
    return 1;
}

int write_to_csv(int csv_fd, int cpu, int iterations, long latency) {
    char * buf = malloc(1024);
    int msg_len = 0;

    sprintf(buf, "%d,%d,%ld\n", cpu, iterations, latency);
    msg_len = strlen(buf);
    write(csv_fd, buf, msg_len);
    free(buf);
    return 1;
}
