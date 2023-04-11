/**
  ******************************************************************************
  * @file    bsp_list.h
  * @author  Rougga
  * @version V0.1
  * @date    15-March-2023
  * @brief   This file contains all the functions prototypes for the devices firmware 
  *          library.
  ******************************************************************************/
 #ifndef BSP_LIST_H
 #define BSP_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Get offset of a member variable.
 *
 * @param[in]   type     the type of the struct this is embedded in.
 * @param[in]   member   the name of the variable within the struct.
 */
#define offsetof(type, member)   ((size_t)&(((type *)0)->member))

/**
 * Get the struct for this entry.
 *
 * @param[in]   ptr     the list head to take the element from.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the variable within the struct.
 */
#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))


/**
* Iterate over list of given type.
*
* @param[in]   queue   he head for your list.
* @param[in]   node    the type * to use as a loop cursor.
* @param[in]   type    the type of the struct this is embedded in.
* @param[in]   member  the name of the slist_t within the struct.
*/
#define slist_for_each_entry(queue, node, type, member)        \
    for (node = (queue)->next? container_of((queue)->next, type, member) : NULL; \
         node;                                        \
         node = node->member.next ? container_of(node->member.next, type, member) : NULL)

/**
 * Get the struct for this entry.
 * @param[in]   ptr     the list head to take the element from.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the variable within the struct.
 */
#define list_entry(ptr, type, member) \
        container_of(ptr, type, member)


/**
 * @brief for single link list
 * 
 */
typedef struct _slist_s {
    struct _slist_s *next;
} slist_t;

/**
 * @brief list init
 * 
 * @param[in] head point to list head
 */
static inline void slist_init(slist_t *head)
{
    head->next = NULL;
}

/**
 * @brief list add new node
 * 
 * @param[in] node new node need to add
 * @param[in] head point to current local list
 */
static inline void slist_add(slist_t *node, slist_t *head)
{
    node->next = head->next;
    head->next = node;
}

/**
 * @brief  The slist is empty or not
 * 
 * @param[in] head 
 * @return true slist is empty
 * @return false slist is not empty
 */
static inline bool slist_empty(slist_t *head)
{
    return !head->next;
}

/**
 * @brief list remove a node
 * 
 * @param[in] head point to list head
 * @param[in] n the remove node
 * @return slist_t* list head
 */
static inline slist_t *slist_remove(slist_t *head, slist_t *n)
{
    slist_t *node = head;
    /* find remove node */
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if(node->next != (slist_t *)0) node->next = node->next->next;

    return head;
}

/**
 * @brief obtain list first node
 * 
 * @param[in] l point to the list head
 * @return slist_t* first node
 */
static inline slist_t *slist_first(slist_t *l)
{
    return l->next;
}

/**
 * @brief obtain list of tail
 * 
 * @param l point to the list head
 * @return slist_t* tail node 
 */
static inline slist_t *slist_tail(slist_t *l)
{
    while (l->next) l = l->next;

    return l;
}

/**
 * @brief obtain the node of next node
 * 
 * @param n the current node
 * @return slist_t* next node
 */
static inline slist_t *slist_next(slist_t *n)
{
    return n->next;
}

/**
 * @brief add node to list tail
 * 
 * @param node will add node
 * @param head the list head
 */
void slist_add_tail(slist_t *node, slist_t *head);

/**
 * @brief delete node of list
 * 
 * @param node the delete node
 * @param head the list head
 */
void slist_del(slist_t *node, slist_t *head);

/**
 * @brief obtain the num of list node
 * 
 * @param queue the list head
 * @return int the number of node
 */
int slist_entry_number(slist_t *queue);

/**********************************************************************/

#define dlist_for_each_entry(queue, node, type, member) \
    for (node = container_of((queue)->next, type, member); \
         &node->member != (queue); \
         node = container_of(node->member.next, type, member))

/**
 * @brief for double link list
 * 
 */
typedef struct dlist_s {
    struct dlist_s *prev;
    struct dlist_s *next;
} dlist_t;

static inline void dlist_init(dlist_t *node)
{
    node->next = node->prev = node;
}

static inline void __dlist_add(dlist_t *node, dlist_t *prev, dlist_t *next)
{
    node->next = next;
    node->prev = prev;

    prev->next = node;
    next->prev = node;
}

static inline void dlist_add_tail(dlist_t *node, dlist_t *queue)
{
    __dlist_add(node, queue->prev, queue);
}

static inline void dlist_del(dlist_t *node)
{
    dlist_t *prev = node->prev;
    dlist_t *next = node->next;

    prev->next = next;
    next->prev = prev;
}

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */

