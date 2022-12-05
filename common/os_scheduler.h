/*
 * Copyright (c) 2021-2022 Silicon Laboratories Inc. (www.silabs.com)
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of the Silicon Labs Master Software License
 * Agreement (MSLA) available at [1].  This software is distributed to you in
 * Object Code format and/or Source Code format and is governed by the sections
 * of the MSLA applicable to Object Code, Source Code and Modified Open Source
 * Code. By using this software, you agree to the terms of the MSLA.
 *
 * [1]: https://www.silabs.com/about-us/legal/master-software-license-agreement
 */
#ifndef OS_SCHEDULER_H
#define OS_SCHEDULER_H
#include <stdbool.h>
#include <stdint.h>
#include "common/ns_list.h"

struct os_ctxt;

/**
 * \brief Initialise event scheduler.
 *
 */
void eventOS_scheduler_init(struct os_ctxt *ctxt);

/**
 * Process one event from event queue.
 * Do not call this directly from application. Requires to be public so that simulator can call this.
 * Use eventOS_scheduler_run() or eventOS_scheduler_run_until_idle().
 * \return true If there was event processed, false if the event queue was empty.
 */
bool eventOS_scheduler_dispatch_event(void);

/**
 * \brief Process events until no more events to process.
 */
void eventOS_scheduler_run_until_idle(void);

/**
 * \brief Read current active Tasklet ID
 *
 * This function not return valid information called inside interrupt
 *
 * \return curret active tasklet id
 *
 * */
int8_t eventOS_scheduler_get_active_tasklet(void);

/**
 * \brief Set manually Active Tasklet ID
 *
 * \param tasklet requested tasklet ID
 *
 * */
 void eventOS_scheduler_set_active_tasklet(int8_t tasklet);

/**
 * \brief This function will be called when stack receives an event.
 */
void eventOS_scheduler_signal(void);

/**
 * \enum arm_library_event_priority_e
 * \brief Event Priority level.
 */
typedef enum arm_library_event_priority {
    ARM_LIB_HIGH_PRIORITY_EVENT = 0, /**< High Priority Event (Function CB) */
    ARM_LIB_MED_PRIORITY_EVENT = 1, /**< Medium Priority (Timer) */
    ARM_LIB_LOW_PRIORITY_EVENT = 2, /*!*< Normal Event and ECC / Security */
} arm_library_event_priority_e;

typedef struct arm_event {
    int8_t receiver; /**< Event handler Tasklet ID */
    int8_t sender; /**< Event sender Tasklet ID */
    uint8_t event_type; /**< This will be typecast arm_library_event_type_e, arm_internal_event_type_e or application specific define */
    uint8_t event_id; /**< Timer ID, NWK interface ID or application specific ID */
    void *data_ptr; /**< Application could share data pointer tasklet to tasklet */
    arm_library_event_priority_e priority;
    uintptr_t event_data;
} arm_event_t;

typedef struct arm_event_storage {
    arm_event_t data;
    enum {
        ARM_LIB_EVENT_DYNAMIC,
        ARM_LIB_EVENT_USER,
        ARM_LIB_EVENT_TIMER,
    } allocator;
    enum {
        ARM_LIB_EVENT_UNQUEUED,
        ARM_LIB_EVENT_QUEUED,
        ARM_LIB_EVENT_RUNNING,
    } state;
    ns_list_link_t link;
} arm_event_storage_t;

/**
 * \brief Send event to event scheduler.
 *
 * \param event pointer to pushed event.
 *
 * Event data is copied by the call, and this copy persists until the
 * recipient's callback function returns. The callback function is passed
 * a pointer to a copy of the data, not the original pointer.
 *
 * \return 0 Event push OK
 * \return -1 Memory allocation Fail
 */
int8_t eventOS_event_send(const arm_event_t *event);

/**
 * \brief Send user-allocated event to event scheduler.
 *
 * \param event pointer to pushed event storage.
 *
 * The event structure is not copied by the call, the event system takes
 * ownership and it is threaded directly into the event queue. This avoids the
 * possibility of event sending failing due to memory exhaustion.
 *
 * event->data must be filled in on entry - the rest of the structure (link and
 * allocator) need not be.
 *
 * The structure must remain valid until the recipient is called - the
 * event system passes ownership to the receiving event handler, who may then
 * invalidate it, or send it again.
 *
 * The recipient receives a pointer to the arm_event_t data member of the
 * event - it can use container_of() to get a pointer to the original
 * event passed to this call, or to its outer container.
 *
 * It is a program error to send a user-allocated event to a non-existent task.
 */
void eventOS_event_send_user_allocated(arm_event_storage_t *event);

/**
 * \brief Event handler callback register
 *
 * Function will register and allocate unique event id handler
 *
 * \param handler_func_ptr function pointer for event handler
 * \param init_event_type generated event type for init purpose
 *
 * \return >= 0 Unique event ID for this handler
 * \return < 0 Register fail
 *
 * */
int8_t eventOS_event_handler_create(void (*handler_func_ptr)(arm_event_t *), uint8_t init_event_type);

/**
 * Cancel an event.
 *
 * Queued events are removed from the event-loop queue and/or the timer queue.
 *
 * Passing a NULL pointer is allowed, and does nothing.
 *
 * Event pointers are valid from the time they are queued until the event
 * has finished running or is cancelled.
 *
 * Cancelling a currently-running event is only useful to stop scheduling
 * it if it is on a periodic timer; it has no other effect.
 *
 * Cancelling an already-cancelled or already-run single-shot event
 * is undefined behaviour.
 *
 * \param event Pointer to event handle or NULL.
 */
void eventOS_event_cancel(arm_event_storage_t *event);

#endif
