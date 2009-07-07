/*****************************************
   Copyright 2008 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************/

/** @file queue.h
  * @brief Interface for the FIFO queue library.
  */

#include <crush/linklist.h>

#ifndef QUEUE_H
#define QUEUE_H
typedef llist_t queue_t;

/** @brief returns whether or not a queue is empty.
  * @param q a queue_t pointer
  */
#define q_empty( q ) \
  ((q)->nnodes == 0)
/** @brief initializes a queue.
  *
  * This function must be called before a queue is ready for use.
  * @param q a pointer to a pre-allocated queue.
  * @param data_free function to free the data held in the queue.
  */
void q_init(queue_t * q, void (*data_free) (void *));

/** @brief frees all memory used by a queue.
  *
  * After this function is called, the queue is unusable until q_init() is
  * called on it again.
  *
  * @param q a pointer to the queue to be destroyed.
  * @return 0 on success.
  */
int q_destroy(queue_t * q);

/** @brief pushes an item onto the back of the queue
  * @param q the queue.
  * @param data the item to be enqueued.
  */
void q_enqueue(queue_t * q, void *data);

/** @brief pops an item out of the front of the queue.
  * @param q the queue from which the next element should be popped.
  * @return the data from the front of the queue, or NULL if the queue is empty.
  */
void *q_dequeue(queue_t * q);

#endif  /* QUEUE_H */
