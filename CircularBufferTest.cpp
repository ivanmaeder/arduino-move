#include "CircularBufferTest.h"

CircularBufferTest::CircularBufferTest() {
}

void CircularBufferTest::runAllTests() {
    Serial.println("Tests starting...");
    test1();
    test2();
    test3();
    test4();
    test5();
    Serial.println("Tests finished.");
}

void CircularBufferTest::test1() {
  CircularBuffer buffer;

  if (buffer.get(0) != 0.0f ||
      buffer.get(1) != 0.0f ||
      buffer.get(2) != 0.0f ||
      buffer.get(3) != 0.0f ||
      buffer.get(4) != 0.0f) {
    Serial.println("Empty list test failed");
  }
}

void CircularBufferTest::test2() {
  CircularBuffer buffer;

  buffer.push(1.0);
  buffer.push(2.0);

  if (buffer.get(0) != 2.0f ||
      buffer.get(1) != 1.0f ||
      buffer.get(2) != 0.0f ||
      buffer.get(3) != 0.0f ||
      buffer.get(4) != 0.0f) {
    Serial.println("Full list test failed");
  }
}

void CircularBufferTest::test3() {
  CircularBuffer buffer;

  buffer.push(1.0);
  buffer.push(2.0);
  buffer.push(3.0);
  buffer.push(4.0);
  buffer.push(5.0);

  if (buffer.get(0) != 5.0f ||
      buffer.get(1) != 4.0f ||
      buffer.get(2) != 3.0f ||
      buffer.get(3) != 2.0f ||
      buffer.get(4) != 1.0f) {
    Serial.println("Full list test failed");
  }
}

void CircularBufferTest::test4() {
  CircularBuffer buffer;

  buffer.push(1.0);
  buffer.push(2.0);
  buffer.push(3.0);
  buffer.push(4.0);
  buffer.push(5.0);
  buffer.push(6.0);
  buffer.push(7.0);

  if (buffer.get(0) != 7.0f ||
      buffer.get(1) != 6.0f ||
      buffer.get(2) != 5.0f ||
      buffer.get(3) != 4.0f ||
      buffer.get(4) != 3.0f) {
    Serial.println("Overflow test failed");
  }
}

void CircularBufferTest::test5() {
  CircularBuffer buffer;

  buffer.push( 1.0);
  buffer.push( 2.0);
  buffer.push( 3.0);
  buffer.push( 4.0);
  buffer.push( 5.0);
  buffer.push( 6.0);
  buffer.push( 7.0);
  buffer.push( 8.0);
  buffer.push( 9.0);
  buffer.push(10.0);
  buffer.push(11.0);
  buffer.push(12.0);
  buffer.push(13.0);
  buffer.push(14.0);

  if (buffer.get(0) != 14.0f ||
      buffer.get(1) != 13.0f ||
      buffer.get(2) != 12.0f ||
      buffer.get(3) != 11.0f ||
      buffer.get(4) != 10.0f) {
    Serial.println("Double overflow test failed");
  }
}
