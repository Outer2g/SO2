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

int Push(struct c_buffer *cb, char c){
	if((cb->head+1)%CBUFF_SIZE == cb->tail) return -1;
	cb->buff[cb->head] = c;
	(++cb->head)%CBUFF_SIZE;
	if(cb->head == CBUFF_SIZE)cb->head = 0;
	return 0;
}

int Read(struct c_buffer *cb, char c){
	if((cb->head)%CBUFF_SIZE == cb->tail) return -1;
	c = cb->buff[cb->tail];
	(++cb->tail)%CBUFF_SIZE;
	return 0;
}

int IsEmpty(const struct c_buffer *cb){
	return (cb->head == cb->tail);
}

int IsFull(const struct c_buffer *cb){
	return ((cb->head+1) == cb->tail);
}

int count(const struct c_buffer *cb){
	int ret;
	ret = cb->head + 1 - CBUFF_SIZE + cb->tail;
	return ret; 
}
