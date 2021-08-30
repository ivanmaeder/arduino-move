#ifndef CircularBuffer_h
#define CircularBuffer_h

#define BUFFER_SIZE 64

#include <Arduino.h>

class CircularBuffer {
  private:
    float buffer[BUFFER_SIZE];
    unsigned char capacity;
    unsigned char head;

    void updatePointers();
    void print(int index);

  public:
    CircularBuffer();

    void push(const float val);
    void clear();
    float get(unsigned char index);
    void print();
};

#endif
