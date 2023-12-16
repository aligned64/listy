#include "../include/listy.h"

typedef struct LISTY_node
{
    struct LISTY_node* _next;
    struct LISTY_node* _prev;

    void* _data;
} LISTY_node_t;

typedef struct LISTY_list
{
    LISTY_node_h _head;
    LISTY_node_h _tail;

    size_t _size;
    LISTY_order_t _order;

    void* (*_copy)(void const*);
    void (*_del)(void*);
    int (*_cmp)(void const*, void const*);
    void const* (*_get_key)(void const*);
} LISTY_list_t;

LISTY_list_h LISTY_new_list(void* (*copy)(void const*), void (*del)(void*), 
    int (*cmp)(void const*, void const*), void const* (*get_key)(void const*),
    LISTY_order_t ordering)
{
    LISTY_list_h list = malloc(sizeof(LISTY_list_t));
    if (!list)
    {
        return NULL;
    }

    list->_head = NULL;
    list->_tail = NULL;
    list->_size = 0;
    list->_order = ordering;

    list->_copy = copy;
    list->_del = del;
    list->_cmp = cmp;
    list->_get_key = get_key;
    
    return list;
}

LISTY_find_ret_t LISTY_find(LISTY_const_list_h list, void const* key)
{
    LISTY_find_ret_t ret;
    ret.target = NULL;
    ret.prev = NULL;
    LISTY_node_h node = list->_head;

    while (node && 
        list->_cmp(list->_get_key(node->_data), key) == list->_order)
    {
        ret.prev = node;
        node = node->_next;
    }

    if (node && list->_cmp(list->_get_key(node->_data), key) == 0)
    {
        ret.target = node;
    }

    return ret;
}

LISTY_node_h LISTY_add_node(LISTY_list_h list, LISTY_node_h node, 
    LISTY_node_h prev)
{
    if (!prev) // front
    {
        node->_next = list->_head;
        list->_head = node;
        node->_prev = NULL;
    }
    else // anywhere else
    {
        node->_next = prev->_next;
        node->_prev = prev;
        prev->_next = node;
    }

    if (node->_next)
    {
        node->_next->_prev = node;
    }
    else
    {
        list->_tail = node;
    }

    ++(list->_size);

    return node;
}

LISTY_node_h LISTY_insert(LISTY_list_h list, void const* data)
{
    LISTY_find_ret_t ret = LISTY_find(list, list->_get_key(data));
    if (ret.target)
    {
        return NULL; // a node with the same key is already there
    }

    LISTY_node_h node = malloc(sizeof(LISTY_node_t));
    if (!node)
    {
        return NULL;
    }

    node->_data = list->_copy(data);

    return LISTY_add_node(list, node, ret.prev);
}

LISTY_node_h LISTY_emplace(LISTY_list_h list, void** data)
{
    LISTY_find_ret_t ret = LISTY_find(list, list->_get_key(*data));
    if (ret.target)
    {
        return NULL; // a node with the same key is already there
    }

    LISTY_node_h node = malloc(sizeof(LISTY_node_t));
    if (!node)
    {
        return NULL;
    }

    node->_data = *data;
    *data = NULL;

    return LISTY_add_node(list, node, ret.prev);
}

void LISTY_traverse(LISTY_const_list_h list, 
    void (*func)(void*, void**), void** other, LISTY_iter_t iter)
{
    LISTY_const_node_h node = (iter == LISTY_FROM_HEAD 
                                   ? list->_head 
                                   : list->_tail);
    while (node)
    {
        func(node->_data, other);
        node = (iter == LISTY_FROM_HEAD ? node->_next : node->_prev);
    }

}

void* LISTY_get_data(LISTY_node_h node)
{
    return node->_data;
}

void LISTY_delete_list(LISTY_list_h list)
{
    LISTY_node_h node = list->_head;
    while(node)
    {
        LISTY_node_h tmp = node->_next;
        free(node->_data);
        free(node);
        node = tmp;
    }

    free(list);
}
