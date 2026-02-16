/**
 * @file quinn.h
 * @author Mason Dizick (mpdizick@coastal.edu)
 * @brief Quinn's macros for data distribution in parallel programs.
 * 
 * This header provides macros for dividing work among processes or threads
 * in parallel programs. The macros ensure balanced distribution of data
 * even when the number of elements doesn't divide evenly among workers.
 * 
 * Used for distributing rows in matrix operations, array elements, and
 * other parallelizable data structures.
 * 
 * @version 1.0
 * @date 2026-02-16
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#ifndef _QUINN_H_
#define _QUINN_H_

#define MIN(a,b) ((a)<(b)?(a):(b))

/* BLOCK_LOW: Calculate starting index for a given rank/thread
 * id: rank/thread id
 * p:  number of processes/threads
 * n:  number of elements in 1 dimension
 * Returns: starting index for this rank
*/
#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))

/* BLOCK_HIGH: Calculate ending index for a given rank/thread
 * id: rank/thread id
 * p:  number of processes/threads
 * n:  number of elements in 1 dimension
 * Returns: ending index for this rank (inclusive)
*/
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)

/* BLOCK_SIZE: Calculate number of elements for a given rank/thread
 * id: rank/thread id
 * p:  number of processes/threads
 * n:  number of elements in 1 dimension
 * Returns: number of elements this rank is responsible for
*/
#define BLOCK_SIZE(id,p,n) \
   (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

/* BLOCK_OWNER: Determine which rank owns a given element
 * j: data index (from [0, n-1])
 * p: number of processes/threads
 * n: number of elements in 1 dimension
 * Returns: rank/thread id responsible for element j
*/
#define BLOCK_OWNER(j,p,n) (((p)*((j)+1)-1)/(n))

#define PTR_SIZE (sizeof(void*))
#define CEILING(i,j) (((i)+(j)-1)/(j))

#endif /* _QUINN_H_ */
