/******************************************************************************
**
** FILE NAME    : common_routines.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : common header file
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 27 May 2009   Xu Liang        The first UEIP release
*******************************************************************************/



#ifndef COMMON_ROUTINES_H
#define COMMON_ROUTINES_H



#include <asm/ifx/ifx_types.h>



/*
 *  Array Help Macro
 */
#define NUM_ENTITY(x)                           (sizeof(x) / sizeof(*(x)))

/* Always report fatal error */
#define IFX_KASSERT(exp, msg) do {    \
    if (unlikely(!(exp))) {      \
        printk msg;               \
        BUG();                    \
    }                             \
} while (0)


/*
 *  find first 1 from MSB in a 32-bit word
 *  if all ZERO, return -1
 *  e.g. 0x10000000 => 28
 */
static inline IFX_int32_t clz(IFX_uint32_t x)
{
    __asm__ (
    "       .set    push                                    \n"
    "       .set    mips32                                  \n"
    "       clz     %0, %1                                  \n"
    "       .set    pop                                     \n"
    : "=r" (x)
    : "r" (x));

    return 31 - (IFX_int32_t)x;
}

/*
 *  find first 0 from MSB in a 32-bit word
 *  if all ONE, return -1
 *  e.g. 0xF0000000 => 27
 */
static inline IFX_int32_t clo(IFX_uint32_t x)
{
    __asm__ (
    "       .set    push                                    \n"
    "       .set    mips32                                  \n"
    "       clo     %0, %1                                  \n"
    "       .set    pop                                     \n"
    : "=r" (x)
    : "r" (x));

    return 31 - (IFX_int32_t)x;
}

/* Tail queue declarations */
#define TAILQ_HEAD(name, type)                                     \
struct name {                                                      \
    struct type *tqh_first;     /* first element */                \
    struct type **tqh_last;     /* addr of last next element */    \
}

#define TAILQ_HEAD_INITIALIZER(head)                               \
    { NULL, &(head).tqh_first }

#define TAILQ_ENTRY(type)                                              \
struct {                                                               \
    struct type *tqe_next;      /* next element */                     \
    struct type **tqe_prev;     /* address of previous next element */ \
}
/*
 * Tail queue functions.
 */
#define TAILQ_CONCAT(head1, head2, field) do {                          \
    if (!TAILQ_EMPTY(head2)) {                                          \
        *(head1)->tqh_last = (head2)->tqh_first;                        \
        (head2)->tqh_first->field.tqe_prev = (head1)->tqh_last;         \
        (head1)->tqh_last = (head2)->tqh_last;                          \
        TAILQ_INIT((head2));                                            \
    }                                                                   \
} while (0)

#define TAILQ_EMPTY(head)       ((head)->tqh_first == NULL)

#define TAILQ_FIRST(head)       ((head)->tqh_first)

#define TAILQ_FOREACH(var, head, field)                                  \
     for ((var) = TAILQ_FIRST((head));                                  \
        (var);                                                           \
        (var) = TAILQ_NEXT((var), field))

#define TAILQ_FOREACH_SAFE(var, head, field, tvar)                        \
    for ((var) = TAILQ_FIRST((head));                                   \
        (var) && ((tvar) = TAILQ_NEXT((var), field), 1);                \
        (var) = (tvar))

#define TAILQ_FOREACH_REVERSE(var, head, headname, field)                  \
    for ((var) = TAILQ_LAST((head), headname);                            \
        (var);                                                             \
        (var) = TAILQ_PREV((var), headname, field))

#define TAILQ_FOREACH_REVERSE_SAFE(var, head, headname, field, tvar)        \
    for ((var) = TAILQ_LAST((head), headname);                            \
        (var) && ((tvar) = TAILQ_PREV((var), headname, field), 1);        \
        (var) = (tvar))

#define TAILQ_INIT(head) do {                                               \
    TAILQ_FIRST((head)) = NULL;                                             \
    (head)->tqh_last = &TAILQ_FIRST((head));                                \
} while (0)

#define TAILQ_INSERT_AFTER(head, listelm, elm, field) do {                  \
    if ((TAILQ_NEXT((elm), field) = TAILQ_NEXT((listelm), field)) != NULL)\
        TAILQ_NEXT((elm), field)->field.tqe_prev =                          \
             &TAILQ_NEXT((elm), field);                                     \
    else {                                                                  \
        (head)->tqh_last = &TAILQ_NEXT((elm), field);                       \
    }                                                                       \
    TAILQ_NEXT((listelm), field) = (elm);                                   \
    (elm)->field.tqe_prev = &TAILQ_NEXT((listelm), field);                  \
} while (0)

#define TAILQ_INSERT_BEFORE(listelm, elm, field) do {                       \
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev;                      \
    TAILQ_NEXT((elm), field) = (listelm);                                   \
    *(listelm)->field.tqe_prev = (elm);                                     \
    (listelm)->field.tqe_prev = &TAILQ_NEXT((elm), field);                  \
} while (0)

#define TAILQ_INSERT_HEAD(head, elm, field) do {                        \
    if ((TAILQ_NEXT((elm), field) = TAILQ_FIRST((head))) != NULL)     \
        TAILQ_FIRST((head))->field.tqe_prev =                           \
            &TAILQ_NEXT((elm), field);                                  \
    else                                                                \
        (head)->tqh_last = &TAILQ_NEXT((elm), field);                   \
    TAILQ_FIRST((head)) = (elm);                                        \
    (elm)->field.tqe_prev = &TAILQ_FIRST((head));                       \
} while (0)

#define TAILQ_INSERT_TAIL(head, elm, field) do {                           \
    TAILQ_NEXT((elm), field) = NULL;                                       \
    (elm)->field.tqe_prev = (head)->tqh_last;                              \
    *(head)->tqh_last = (elm);                                             \
    (head)->tqh_last = &TAILQ_NEXT((elm), field);                          \
} while (0)

#define TAILQ_LAST(head, headname)                                         \
    (*(((struct headname *)((head)->tqh_last))->tqh_last))

#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define TAILQ_PREV(elm, headname, field)                                    \
    (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))

#define TAILQ_REMOVE(head, elm, field) do {                                 \
    if ((TAILQ_NEXT((elm), field)) != NULL)                               \
         TAILQ_NEXT((elm), field)->field.tqe_prev =                         \
             (elm)->field.tqe_prev;                                         \
    else {                                                                  \
        (head)->tqh_last = (elm)->field.tqe_prev;                           \
    }                                                                       \
    *(elm)->field.tqe_prev = TAILQ_NEXT((elm), field);                      \
} while (0)


static inline int
ifx_drv_ver(char *buf, char *module, int major, int mid, int minor)
{
    return sprintf(buf, "Lantiq %s driver, version %d.%d.%d, (c) 2001-2011 Lantiq Deutschland GmbH\n",
        module, major, mid, minor);
}

/*
 *  Basic Clock Functions (not for 100% precise usage)
 */
extern unsigned int ifx_get_cpu_hz(void);
extern unsigned int ifx_get_fpi_hz(void);

/*
 *  Output on ASC (before ASC/Console driver is ready)
 */
extern void prom_printf(const char *, ...);

/*
 *  Get Reserved Memory for Voice
 */
extern unsigned int* ifx_get_cp1_base(void);
extern unsigned int ifx_get_cp1_size(void);



#endif  //  COMMON_ROUTINES_H
