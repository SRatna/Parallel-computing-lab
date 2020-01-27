#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define KILO (1024)
#define MEGA (1024*1024)
#define MAX_ITEMS (64*MEGA)

#define THREADS 8
#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

static int *v;
static pthread_t pids[THREADS];
static int args[THREADS];

struct info {
    int low;
    int * data_set ;
    int high;
};

static void print_array(void) {
    for (int i = 0; i < MAX_ITEMS; i++)
        printf("%d ", v[i]);
    printf("\n");
}

static void init_array_line(int *line) {
    int i, max;
    i = *line;
    max = i + MAX_ITEMS;
    v = (int *) malloc(MAX_ITEMS * sizeof(int));
    for (i; i < max; i++)
        v[i] = rand() % 10000 + 1;
}

static void init_array() {
    int i;
    for (i = 0; i < THREADS; i++) {
        pthread_create(pids + i, NULL, &init_array_line, args + i);
    }
}

static unsigned partition(int *v, unsigned low, unsigned high,  unsigned pivot_index) {
    if (pivot_index != low)
        swap(v, low, pivot_index);

    pivot_index = low;
    low++;

    while (low <= high) {
        if (v[low] <= v[pivot_index])
            low++;
        else if (v[high] > v[pivot_index])
            high--;
        else
            swap(v, low, high);
    }

    if (high != pivot_index)
        swap(v, pivot_index, high);
    return high;
}

static void quick_sort(void *data){
    struct info *info = data;
    struct info *info1 = data;

    int low = info->low;
    int high = info->high;
    pthread_attr_t attr;
    pthread_t thread_id1;
    pthread_attr_init(&attr);
    unsigned pivot_index;

    if (low >= high)
        return;

    pivot_index = (low + high) / 2;
    pivot_index = partition(v, low, high, pivot_index);
    info1->data_set = info->data_set;

    if (low < pivot_index) {
        info1->low = low;
        info1->high = pivot_index - 1;
        pthread_create(pids, NULL, &quick_sort, &info);
    }

    if (pivot_index < high) {
        info->low = pivot_index + 1;
        info->high = high;
        pthread_create(pids, NULL, &quick_sort, &info);
    }

}

static wait_all() {
    int i;
    for (i = 0; i < THREADS; i++) {
        pthread_join(pids[i], NULL);
    }
}

int main(int argc, char **argv) {
    init_array();
    wait_all();

    struct info data1;
    data1.low = 0;
    data1.high = MAX_ITEMS - 1;
    data1.data_set = v;

    quick_sort(&data1);
    wait_all();
}