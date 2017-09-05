#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;

#define RED "\e[0;31m"
#define BLACK "\e[0;0m"
/* status to return success of failure*/
typedef enum
{
		e_failure,
		e_success
} Status;

//to return type of operation
typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
