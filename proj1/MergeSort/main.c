#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#define list_entry(ptr, type, member) \
    ((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))
#define TRAVERSE(HEAD, POS) \
    for (POS = (HEAD); POS; \
        (POS = (POS->next)), POS = (POS != (HEAD) ? POS : NULL))
#define COUNT(HEAD) \
    ({ \
     int __i = 0; \
     for (struct list_head *pos = (HEAD); pos; \
             (pos = (pos->next)), pos = (pos != (HEAD) ? pos : NULL), __i++) \
     ; \
     __i; \
     })

struct list_head {
    struct list_head *next, *prev;
};

#define SLICE(N, HEAD) \
    ({ \
     struct list_head *pos = (HEAD); \
     for (int __i = 1; (__i < (N)); (pos = (pos->next)), __i++) \
     ; \
     pos->next = (HEAD); \
     (HEAD)->prev = pos; \
     HEAD; \
     })
#define SLICE_BW(N, HEAD) \
    ({ \
     struct list_head *tail = (HEAD)->prev; \
     struct list_head *pos = tail; \
     for (int __i = 1; (__i < (N)); (pos = (pos->prev)), __i++) \
     ; \
     pos->prev = tail; \
     tail->next = pos; \
     pos; \
     })
#define TRAVERSE_BW(HEAD, POS) \
    for (POS = ((HEAD)->prev); POS; \
            (POS = (POS->prev)), POS = (POS != ((HEAD)->prev) ? POS : NULL))

int stack_size = 0;
int max = 0;

void init_list_head(struct list_head *hd) {
    hd->next = hd;
    hd->prev = hd;
}

void clear(struct list_head *head, void (*deinit)(struct list_head *)) {
    struct list_head *prev, *pos = head->next;
    while (pos != head) {
        prev = pos;
        pos = pos->next;
        init_list_head(prev);
        deinit(prev);
    }

    init_list_head(head);
    deinit(head);
}

void tail_add(struct list_head *tail, struct list_head *list) {
    tail->next = list;
    tail->prev = list->prev;
    list->prev->next = tail;
    list->prev = tail;
}

struct list_head *head_add(struct list_head *head, struct list_head *list) {
    struct list_head *temp = list->prev;
    list->prev->next = head;
    list->prev = head;
    head->prev = temp;
    head->next = list;
    return head;
}

static struct list_head *head_remove(struct list_head *list) {
    if (list->next == list)
        return NULL;
    struct list_head *new_head = list->next;
    list->prev->next = new_head;
    new_head->prev = list->prev;
    init_list_head(list);
    return new_head;
}

struct mylist {
    int data;
    char c;
    struct list_head list;
};


static struct list_head *merge(struct list_head *left,
        struct list_head *right,
        bool (*pred)(struct list_head *,
            struct list_head *)) {
    stack_size += (char *)__builtin_frame_address(1) - (char *)__builtin_frame_address(0);
    max = max > stack_size ? max : stack_size;
    if (left == NULL) {
        stack_size -= (char *)__builtin_frame_address(1) - (char *)__builtin_frame_address(0);
        return right;
    }
    if (right == NULL) {
        stack_size -= (char *)__builtin_frame_address(1) - (char *)__builtin_frame_address(0);
        return left;
    }
    struct list_head *head, *tail;
    /* Fill your solution here */
    if(pred(left, right)) {
        head = left;
        left = right;
        right = head;
    } else {
        head = right;
    }

    tail = head_remove(right);
    struct list_head *tmp_head_add = head_add(head, merge(left, tail, pred));
    stack_size -= (char *)__builtin_frame_address(1) - (char *)__builtin_frame_address(0);
    return tmp_head_add;
}

bool predicate(struct list_head *hdl, struct list_head *hdr) {
    struct mylist *l = list_entry(hdl, struct mylist, list);
    struct mylist *r = list_entry(hdr, struct mylist, list);
    return l->data < r->data;
}

struct list_head *merge_sort(struct list_head *head) {
    stack_size += (char *)__builtin_frame_address(1) - (char *)__builtin_frame_address(0);
    max = max > stack_size ? max : stack_size;
    if (head->next == head) {
        stack_size -= (char *)__builtin_frame_address(1) - (char *)__builtin_frame_address(0);
        return head;
    }
    int c = COUNT(head);
    struct list_head *last_few = SLICE_BW(c / 2, head);
    struct list_head *first_few = SLICE(c % 2 ? c / 2 + 1 : c / 2, head);
    struct list_head *tmp;
    tmp = merge(merge_sort(first_few), merge_sort(last_few), predicate);
    stack_size -= (char *)__builtin_frame_address(1) - (char *)__builtin_frame_address(0);

    return tmp;
}

void deinit(struct list_head *head) {
    struct mylist *item = list_entry(head, struct mylist, list);
    free(item);
}

int main() {

    struct mylist *temp;
    struct list_head *new_head;
    int num = 1024;
    temp = malloc(sizeof *temp);
    init_list_head(&temp->list);
    temp->data = num;
    temp->c = 'a';
    new_head = &temp->list;

    for (int i = 1; i < num; i++) {
        temp = malloc(sizeof *temp);
        init_list_head(&temp->list);
        /* temp->data = rand() % 1000000; */
        temp->data = num - i - 1;
        temp->c = 'a' + i;
        tail_add(&temp->list, new_head);
    }
    /* struct list_head *curr; */
    /* curr = new_head; */
    /* do { */
    /*     printf("%d ", list_entry(curr, struct mylist, list)->data); */
    /*     curr = curr->next; */
    /* } while(curr != new_head); */
    /* printf("\n*************\n"); */

    new_head = merge_sort(new_head);

    /* curr = new_head; */
    /* do { */
    /*     #<{(| printf("%d(%c) ", list_entry(curr, struct mylist, list)->data, list_entry(curr, struct mylist, list)->c); |)}># */
    /*     printf("%d ", list_entry(curr, struct mylist, list)->data); */
    /*     curr = curr->next; */
    /* } while(curr != new_head); */
    /* printf("\n*************\n"); */

    printf("%d\n", stack_size);
    printf("%d\n", max);
    clear(new_head, deinit);
    return 0;
}
