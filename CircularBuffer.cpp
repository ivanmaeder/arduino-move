#include "CircularBuffer.h"

CircularBuffer::CircularBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
      buffer[i] = 0.0f;
  }

  head = 0;
}

void CircularBuffer::clear() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
      buffer[i] = 0.0f;
  }
}    

void CircularBuffer::push(const float val) {
  updatePointers();
  buffer[head] = val;
}

void CircularBuffer::updatePointers() {
  head = (head + 1) % BUFFER_SIZE;
}

float CircularBuffer::get(const unsigned char i) {
  int index = head - i;

  if (index < 0) {
    index = BUFFER_SIZE + index;
  }

  return buffer[index];
}

void CircularBuffer::print() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.println(get(i));
  }
}
