#include "looper.hpp"

Looper looper;

void TIM_LOOPER_CLOCK_HANDLER() {
  auto tim = Tim { TIM_LOOPER_CLOCK };
  if (tim.isState(TimState::update)) {
    // TODO:
    tim.clearState(TimState::update);
  }
}
