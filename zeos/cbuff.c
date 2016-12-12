#include <cbuff.h>

/*typedef struct {
	char buff[BUFF_SIZE];
	int head; //write at head
	int tail; //read from tail
}c_buffer;*/

void init_cbuff(struct c_buffer *cb){
	cb->head = 0;
	cb->tail = 0;
}

int cb_Push(struct c_buffer *cb, char c){
	if((cb->head+1)%CBUFF_SIZE == cb->tail) return -1;
	cb->buff[cb->head] = c;
	++cb->head;
	if(cb->head == CBUFF_SIZE)cb->head = 0;
	return 0;
}

int cb_Read(struct c_buffer *cb, char *c){
	if(cb->head == cb->tail) return -1;
	c = cb->buff[cb->tail];
	++cb->tail;
	return 0;
}

int cb_IsEmpty(const struct c_buffer *cb){
	return (cb->head == cb->tail);
}

int cb_IsFull(const struct c_buffer *cb){
	return ((cb->head+1) == cb->tail);
}

int cb_count(const struct c_buffer *cb){
	if (cb->head > cb->tail) return (cb->head - cb->tail);
	else if (cb->head  == cb->tail) return 0;
	else if (cb->head +1 == cb->tail) return CBUFF_SIZE;
	else return  cb->head + CBUFF_SIZE - cb->tail;
}
