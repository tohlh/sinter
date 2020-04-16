#ifndef SINTER_STACK_H
#define SINTER_STACK_H

#include <stdint.h>

#include "config.h"
#include "nanbox.h"
#include "heap_obj.h"
#include "debug.h"
#include "debug_heap.h"

#ifdef __cplusplus
extern "C" {
#endif

// (Inclusive) Bottom of the current function's operand stack, as an index into
// sistack.
extern sinanbox_t *sistack_bottom;
// (Exclusive) Limit of the current function's operand stack, as an index into
// sistack.
extern sinanbox_t *sistack_limit;
// Index of the next empty entry of the current function's operand stack.
extern sinanbox_t *sistack_top;

SINTER_INLINE void sistack_push(sinanbox_t entry) {
#ifndef SINTER_SEATBELTS_OFF
  if (sistack_top >= sistack_limit) {
    sifault(sinter_fault_stack_overflow);
    return;
  }
#endif

#if SINTER_DEBUG_LEVEL >= 2
    SIDEBUG("Pushed onto stack: ");
    SIDEBUG_NANBOX(entry);
    SIDEBUG("\n");
#endif

  *(sistack_top++) = entry;
}

SINTER_INLINE sinanbox_t sistack_pop(void) {
#ifndef SINTER_SEATBELTS_OFF
  if (sistack_top <= sistack_bottom) {
    sifault(sinter_fault_stack_underflow);
  } else
#endif
  {
#if SINTER_DEBUG_LEVEL >= 2
    SIDEBUG("Popped from stack: ");
    SIDEBUG_NANBOX(*(sistack_top - 1));
    SIDEBUG("\n");
#endif
    return *(--sistack_top);
  }
}

SINTER_INLINE sinanbox_t sistack_peek(unsigned int index) {
  sinanbox_t *v = sistack_top - 1 - index;
#ifndef SINTER_SEATBELTS_OFF
  if (v < sistack_bottom) {
    sifault(sinter_fault_stack_underflow);
  } else
#endif
  {
    return *v;
  }
}

SINTER_INLINE void sistack_new(unsigned int size, const opcode_t *return_address, siheap_env_t *return_env) {
  siheap_frame_t *frame = siframe_new();
  if (!frame) {
    sifault(sinter_fault_out_of_memory);
    return;
  }
  frame->return_address = return_address;
  frame->saved_env = return_env;
  frame->saved_stack_bottom = sistack_bottom;
  frame->saved_stack_limit = sistack_limit;
  frame->saved_stack_top = sistack_top;

  sistack_push(SIHEAP_PTRTONANBOX(frame));

  sistack_bottom = sistack_top;
  sistack_limit = sistack_bottom + size;
}

SINTER_INLINE void sistack_destroy(const opcode_t **return_address, siheap_env_t **return_env) {
  while (sistack_top > sistack_bottom) {
    sinanbox_t v = sistack_pop();
    siheap_derefbox(v);
  }

  siheap_frame_t *frame = (siheap_frame_t *) SIHEAP_NANBOXTOPTR(*(sistack_bottom - 1));
  *return_address = frame->return_address;
  *return_env = frame->saved_env;
  sistack_bottom = frame->saved_stack_bottom;
  sistack_limit = frame->saved_stack_limit;
  sistack_top = frame->saved_stack_top;

  siheap_deref(frame);
}

void sistack_init(void);

#ifdef __cplusplus
}
#endif

#endif
