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

#include <queue.h>
 void q_init(queue_t * q, void (*data_free) (void *)) {
  ll_list_init(q, data_free, NULL);
} int q_destroy(queue_t * q) {

  return ll_destroy(q);
}
void q_enqueue(queue_t * q, void *data) {
  ll_append_elem(q, data);
} void *q_dequeue(queue_t * q) {
  void *d;
  if (q->head == NULL)
    return NULL;
  d = q->head->data;
  ll_rm_elem(q, q->head);
  return d;
}


