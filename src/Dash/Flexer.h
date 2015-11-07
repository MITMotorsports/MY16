#ifndef FLEXER_H
#define FLEXER_H

typedef struct BarState {
  unsigned char state[8];
} BarState;

class Flexer {
  private:
    BarState bar_state;
    char inner = 0;
    char outer = 0;
    bool forward;
    void reset_state();
  public:
    Flexer();
    void startForward();
    void startBackward();
    bool hasNext();
    BarState* next();
};
#endif // FLEXER_H
