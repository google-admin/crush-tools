#include <queue.h>


void q_init( queue_t *q,  void (* data_free)(void *)){
	ll_list_init(q, data_free, NULL);
}

int q_destroy( queue_t *q ) {
	return ll_destroy( q );
}

void q_enqueue( queue_t *q, void *data ){
	ll_append_elem( q, data );
}

void * q_dequeue(queue_t *q){
	void *d = q->head->data;
	ll_rm_elem(q, q->head);
	return d;
}
