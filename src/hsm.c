#include <hsm.h>

void hsm_start(hsm_t * hsm, const hsm_state_t * initial_state)
{
    hsm->current = initial_state;
    hsm->next = NULL;
    hsm->current->handler(HSM_EVENT_ENTRY);
}

void hsm_dispatch(hsm_t * hsm, hsm_event_t evt)
{
    bool was_evt_handled;
    const hsm_state_t * state = hsm->current;
    do
    {
        was_evt_handled = state->handler(evt);
        state = state->parent;
    }
    while (!was_evt_handled && state);

    if (hsm->next)
    {
        hsm->current->handler(HSM_EVENT_EXIT);
        hsm->current = hsm->next;
        hsm->next = NULL;
        hsm->current->handler(HSM_EVENT_ENTRY);
    }
}

void hsm_transition(hsm_t * hsm, const hsm_state_t * next)
{
    hsm->next = next;
}
