#include <stddef.h>
#include <stdio.h>

#include <hsm.h>

hsm_t hsm;

enum events
{
    EVENT_SWITCH_STATES,
};

static bool s1_handler(hsm_event_t evt);
static bool s2_handler(hsm_event_t evt);

static const hsm_state_t s1 = {.handler = s1_handler, .parent = NULL};
static const hsm_state_t s2 = {.handler = s2_handler, .parent = NULL};

bool s1_handler(hsm_event_t evt)
{
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            printf("S1 entry\n");
            break;
        case EVENT_SWITCH_STATES:
            hsm_transition(&hsm, &s2);
            break;
        case HSM_EVENT_EXIT:
            printf("S1 exit\n");
            break;
        default:
            return false;
    }
    return true;
}

bool s2_handler(hsm_event_t evt)
{
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            printf("S2 entry\n");
            break;
        case EVENT_SWITCH_STATES:
            hsm_transition(&hsm, &s1);
            break;
        case HSM_EVENT_EXIT:
            printf("S2 exit\n");
            break;
        default:
            return false;
    }
    return true;
}

int main(void)
{
    hsm_start(&hsm, &s1);
    hsm_dispatch(&hsm, EVENT_SWITCH_STATES);
    hsm_dispatch(&hsm, EVENT_SWITCH_STATES);
    hsm_dispatch(&hsm, EVENT_SWITCH_STATES);
    return 0;
}
