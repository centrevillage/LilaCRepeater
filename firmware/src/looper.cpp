#include "looper.hpp"

Looper looper;

void TIM_LOOPER_CLOCK_HANDLER() {
  auto tim = Tim { TIM_LOOPER_CLOCK };
  if (tim.isState(TimState::UPDATE)) {
    // TODO:
    tim.clearState(TimState::UPDATE);
  }
}
