#include <unity.h>
#include <string.h>

#include <hsm.c>

/* Test helpers */

#define TEST_EVENTS_MAX_COUNT 16

typedef struct
{
    const hsm_state_t * state;
    hsm_event_t         evt;
} test_event_t;

static test_event_t test_events[TEST_EVENTS_MAX_COUNT];
static uint8_t rd_idx = 0;
static uint8_t wr_idx = 0;

static void test_event_register(const hsm_state_t * state, hsm_event_t evt)
{
    test_events[wr_idx].state = state;
    test_events[wr_idx].evt   = evt;
    wr_idx++;
}

static test_event_t * test_event_pop(void)
{
    if (rd_idx < wr_idx)
    {
        return &test_events[rd_idx++];
    }
    else
    {
        return NULL;
    }
}

static void test_event_list_clear(void)
{
    memset(test_events, 0, sizeof(test_events) / sizeof(test_events[0]));
    rd_idx = 0;
    wr_idx = 0;
}

/* Declarations for HSM module */

hsm_t hsm;

enum test_events
{
    HSM_TEST_EVENT_NEXT,
    HSM_TEST_EVENT_GO_TO_S2,
    HSM_TEST_EVENT_PARENT_ONLY,
};

static bool state_s1_handler(hsm_event_t evt);
static bool state_s11_handler(hsm_event_t evt);
static bool state_s2_handler(hsm_event_t evt);

static const hsm_state_t state_s1 =  {state_s1_handler, NULL};
static const hsm_state_t state_s11 = {state_s11_handler, &state_s1};
static const hsm_state_t state_s2 =  {state_s2_handler, NULL};

static bool state_s1_handler(hsm_event_t evt)
{
    test_event_register(&state_s1, evt);
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            break;
        case HSM_TEST_EVENT_NEXT:
            hsm_transition(&hsm, &state_s11);
            break;
        case HSM_TEST_EVENT_GO_TO_S2:
            hsm_transition(&hsm, &state_s2);
            break;
        case HSM_TEST_EVENT_PARENT_ONLY:
            break;
        case HSM_EVENT_EXIT:
            break;
        default:
            return false;
    }
    return true;
}

static bool state_s11_handler(hsm_event_t evt)
{
    test_event_register(&state_s11, evt);
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            break;
        case HSM_TEST_EVENT_NEXT:
            hsm_transition(&hsm, &state_s2);
            break;
        case HSM_TEST_EVENT_GO_TO_S2:
            hsm_transition(&hsm, &state_s2);
            break;
        case HSM_EVENT_EXIT:
            break;
        default:
            return false;
    }
    return true;
}

static bool state_s2_handler(hsm_event_t evt)
{
    test_event_register(&state_s2, evt);
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            break;
        case HSM_TEST_EVENT_NEXT:
            hsm_transition(&hsm, &state_s1);
            break;
        case HSM_EVENT_EXIT:
            break;
        default:
            return false;
    }
    return true;
}

/* Actual tests */

static char * state_name_get(const hsm_state_t * p_state)
{
    uintptr_t p_st = (uintptr_t)p_state;
    if (p_st == (uintptr_t)&state_s1)
    {
        return "STATE_S1";
    }
    else if (p_st == (uintptr_t)&state_s11)
    {
        return "STATE_S11";
    }
    else if (p_st == (uintptr_t)&state_s2)
    {
        return "STATE_S2";
    }
    else
    {
        TEST_FAIL_MESSAGE("Provided state not handled in name getter.");
        return NULL;
    }
}

static char * event_name_get(hsm_event_t evt)
{
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            return "ENTRY";
        case HSM_EVENT_EXIT:
            return "EXIT";
        case HSM_TEST_EVENT_NEXT:
            return "NEXT";
        case HSM_TEST_EVENT_PARENT_ONLY:
            return "PARENT_ONLY";
        case HSM_TEST_EVENT_GO_TO_S2:
            return "GO_TO_S2";
        default:
            TEST_FAIL_MESSAGE("Provided event not handled in name getter.");
            return NULL;
    }
}

static void verify_test_events(test_event_t * expected_events, size_t evt_num)
{
    for (size_t i = 0; i < evt_num; i++)
    {
        test_event_t * received_evt = test_event_pop();
        test_event_t * expected_evt = &expected_events[i];

        TEST_ASSERT(received_evt);

        char string[128];

        snprintf(string,
                 sizeof(string),
                 "Expected: %s. Got: %s",
                  event_name_get(expected_evt->evt),
                  event_name_get(received_evt->evt));
        TEST_ASSERT_EQUAL_HEX32_MESSAGE(expected_evt->evt, received_evt->evt, string);

        snprintf(string,
                 sizeof(string),
                 "Expected: %s. Got: %s",
                  state_name_get(expected_evt->state),
                  state_name_get(received_evt->state));
        TEST_ASSERT_EQUAL_HEX32_MESSAGE((uintptr_t)expected_evt->state,
                                        (uintptr_t)received_evt->state,
                                        string);
    }
}

void setUp(void)
{
    memset(&hsm, 0, sizeof(hsm));
    test_event_list_clear();
}

void tearDown(void)
{
    /* Check if event queue is empty */
    test_event_t * evt = test_event_pop();
    if (evt)
    {
        char string[128];
        snprintf(string, 128, "Event queue is not empty - contain event: %d", evt->evt);
        TEST_FAIL_MESSAGE(string);
    }
}

void test_start(void)
{
    test_event_t expected_events[] =
    {
        {&state_s1, HSM_EVENT_ENTRY},
    };

    hsm_start(&hsm, &state_s1);

    verify_test_events(expected_events, sizeof(expected_events) / sizeof(expected_events[0]));
}

void test_simple_transition(void)
{
    test_event_t expected_events[] =
    {
        {&state_s1, HSM_EVENT_ENTRY},
        {&state_s1, HSM_TEST_EVENT_NEXT},
        {&state_s1, HSM_EVENT_EXIT},
        {&state_s11, HSM_EVENT_ENTRY},
    };

    hsm_start(&hsm, &state_s1);
    hsm_dispatch(&hsm, HSM_TEST_EVENT_NEXT);

    verify_test_events(expected_events, sizeof(expected_events) / sizeof(expected_events[0]));
}

void test_event_pass_to_parent_state(void)
{
    test_event_t expected_events[] =
    {
        {&state_s11, HSM_EVENT_ENTRY},
        {&state_s11, HSM_TEST_EVENT_PARENT_ONLY},
        {&state_s1,  HSM_TEST_EVENT_PARENT_ONLY},
    };

    hsm_start(&hsm, &state_s11);
    hsm_dispatch(&hsm, HSM_TEST_EVENT_PARENT_ONLY);

    verify_test_events(expected_events, sizeof(expected_events) / sizeof(expected_events[0]));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_start);
    RUN_TEST(test_simple_transition);
    RUN_TEST(test_event_pass_to_parent_state);

    return UNITY_END();
}
