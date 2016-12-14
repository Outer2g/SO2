#include <cbuff.h>

/*typedef struct {
	char buff[BUFF_SIZE];
	int head; //write at head
	int tail; //read from tail
}c_buffer;*/

void init_cbuff(struct c_buffer *cb){
	cb->head = 0;
	cb->tail = 0;
	cb->count = 0;
}

int cb_Push(struct c_buffer *cb, char *c){
	if(cb->count == CBUFF_SIZE) return -1;
	cb->buff[cb->head] = c;
	++cb->head;
	++cb->count;
	if(cb->head == CBUFF_SIZE)cb->head = 0;
	return 0;
}

int cb_Read(struct c_buffer *cb, char *c){
	if(cb->count == 0) return -1;
	//if(cb->head == cb->tail) return -1;
	c = cb->buff[cb->tail];
	++cb->tail;
	--cb->count;
	if(cb->tail == CBUFF_SIZE)cb->tail = 0;
	return 0;
}

int cb_IsEmpty(const struct c_buffer *cb){
	if (cb->count == 0) return 1;
	else return 0;
	/*if (cb->head == cb->tail) return 1;
	else return 0;*/
}

int cb_IsFull(const struct c_buffer *cb){
	if ((cb->count) == CBUFF_SIZE) return 1;
	else return 0;
	/*if ((cb->head+1) == cb->tail) return 1;
	else if ((cb->head == CBUFF_SIZE-1) && cb->tail == 0) return 1;
	else return 0;*/
}

int cb_count(const struct c_buffer *cb){
	return cb->count;
}
