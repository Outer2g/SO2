#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#define CBUFF_SIZE 16


//si head és igual a tail el buffer estarà buit, si head+1 és igual a tail el buffer està ple
struct c_buffer{
	char buff[CBUFF_SIZE];
	int head; //write at head
	int tail; //read from tail
};

void init_cbuff(struct c_buffer *cb);

//afegeix l'element c al buffer si es pot altrament retorna -1
int Push(struct c_buffer *cb, char c);

//llegeix un caràcter del buffer
int Read(struct c_buffer *cb, char c);


//
int IsEmpty(const struct c_buffer *cb);

int IsFull(const struct c_buffer *cb);

//retorna el numero d'elements del buffer
int count(const struct c_buffer *cb);

#endif
