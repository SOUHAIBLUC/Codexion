#include "codexion.h"

void heap_insert(t_dongle *d, int coder_id, long deadline)
{
    int          i;
    int          parent;
    t_heap_entry tmp;

    d->heap[d->heap_size].coder_id = coder_id;
    d->heap[d->heap_size].deadline = deadline;
    d->heap_size++;
    i = d->heap_size - 1;
    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (d->heap[i].deadline < d->heap[parent].deadline)
        {
            tmp             = d->heap[i];
            d->heap[i]      = d->heap[parent];
            d->heap[parent] = tmp;
            i               = parent;
        }
        else
            break;
    }
}

void heap_remove(t_dongle *d)
{
    int          i = 0;
    int          child;
    t_heap_entry tmp;
    d->heap[0] = d->heap[d->heap_size - 1];
    d->heap_size--;
    while (1)
    {
        int left  = 2 * i + 1;
        int right = 2 * i + 2;
        child     = left;
		if (left >= d->heap_size)
			break;
        if (right < d->heap_size &&
            d->heap[right].deadline < d->heap[left].deadline)
            child = right;
        if (d->heap[i].deadline > d->heap[child].deadline)
        {
            tmp            = d->heap[i];
            d->heap[i]     = d->heap[child];
            d->heap[child] = tmp;
            i              = child;
        }
		else 
			break;
    }
}

int heap_peek(t_dongle *d) 
{
	return d->heap[0].coder_id; 
}