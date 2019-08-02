#ifndef _HSM_H
#define _HSM_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define HSM_EVENT_ENTRY 0xFE
#define HSM_EVENT_EXIT  0xFF

typedef uint8_t hsm_event_t;

typedef struct hsm_state_t hsm_state_t;

typedef bool (* hsm_handler_t)(hsm_event_t evt);

struct hsm_state_t
{
    hsm_handler_t       handler;
    const hsm_state_t * parent;
};

typedef struct
{
    const hsm_state_t * current;
    const hsm_state_t * next;
} hsm_t;

void hsm_start(hsm_t * hsm, const hsm_state_t * initial_state);

void hsm_dispatch(hsm_t * hsm, hsm_event_t evt);

void hsm_transition(hsm_t * hsm, const hsm_state_t * next);

#endif // _HSM_H
