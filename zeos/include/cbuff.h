#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#define CBUFF_SIZE 8


//si head és igual a tail el buffer estarà buit, si head+1 és igual a tail el buffer està ple
struct c_buffer{
	char buff[CBUFF_SIZE];
	int head; //write at head
	int tail; //read from tail
};

void init_cbuff(struct c_buffer *cb);

//afegeix l'element c al buffer si es pot altrament retorna -1
int cb_Push(struct c_buffer *cb, char c);

//llegeix un caràcter del buffer
int cb_Read(struct c_buffer *cb, char *c);


//
int cb_IsEmpty(const struct c_buffer *cb);

int cb_IsFull(const struct c_buffer *cb);

//retorna el numero d'elements del buffer
int cb_count(const struct c_buffer *cb);

#endif
