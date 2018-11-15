#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

static struct list_head *merge(struct list_head *left,
        struct list_head *right,
        bool (*pred)(struct list_head *,
            struct list_head *)) {
    if (left == NULL) return right;
    if (right == NULL) return left;
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
    return head_add(head, merge(left, tail, pred));
}

struct mylist {
    int data;
    char c;
    struct list_head list;
};

bool predicate(struct list_head *hdl, struct list_head *hdr) {
    struct mylist *l = list_entry(hdl, struct mylist, list);
    struct mylist *r = list_entry(hdr, struct mylist, list);
    return l->data < r->data;
}

struct list_head *merge_sort(struct list_head *head) {
    if (head->next == head) {
        return head;
    }
    int c = COUNT(head);
    struct list_head *last_few = SLICE_BW(c / 2, head);
    struct list_head *first_few = SLICE(c % 2 ? c / 2 + 1 : c / 2, head);
    return merge(merge_sort(first_few), merge_sort(last_few), predicate);
}

void deinit(struct list_head *head) {
    struct mylist *item = list_entry(head, struct mylist, list);
    free(item);
}

int main() {
    struct mylist *temp;
    struct list_head *new_head;
    temp = malloc(sizeof *temp);
    init_list_head(&temp->list);
    temp->data = 700;
    temp->c = 'a';
    new_head = &temp->list;
    for (int i = 1; i < 13; i++) {
        temp = malloc(sizeof *temp);
        init_list_head(&temp->list);
        temp->data = rand() % 1000;
        temp->c = 'a' + i;
        tail_add(&temp->list, new_head);
    }
    struct list_head *curr;
    curr = new_head;
    do {
        printf("%d(%c) ", list_entry(curr, struct mylist, list)->data, list_entry(curr, struct mylist, list)->c);
        curr = curr->next;
    } while(curr != new_head);
    printf("\n*************\n");

    new_head = merge_sort(new_head);

    curr = new_head;
    do {
        printf("%d(%c) ", list_entry(curr, struct mylist, list)->data, list_entry(curr, struct mylist, list)->c);
        curr = curr->next;
    } while(curr != new_head);
    printf("\n*************\n");

    clear(new_head, deinit);
    return 0;
}
