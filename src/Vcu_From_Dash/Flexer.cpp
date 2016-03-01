#include "Flexer.h"

Flexer::Flexer () {
  reset_state();
}

void Flexer::reset_state() {
  for(int i = 0; i < 8; i++) {
    bar_state.state[i] = 0;
  }
}

void Flexer::startForward() {
  forward = true;
  reset_state();
}

void Flexer::startBackward() {
  forward = false;
  reset_state();
}
