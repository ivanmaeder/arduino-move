//#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h> 

#include "CircularBuffer.h"
#include "CircularBufferTest.h"

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);
//Stick
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);
//Grid
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, PIN, NEO_RGB + NEO_KHZ800);


bool emptyBufferFlag = false;
CircularBuffer buffer;

void setup(void) {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("Ooops, no LSM303 detected... Check your wiring!");
    while(1);
  }

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
/*

float runEvent_down = 36.8;
float runEvent_up = 36.8;
bool runEvent_accelerometerOnItsWayUp = false;
int runEvent_deltas = 0;
float fs[] = { 36.8, 39.1, 40.1, 40.7, 37.9, 34.3, 31.1, 27.7, 24.9, 26.1, 27.6, 39.7, 42.7, 41.7, 39.7, 29.5, 29.7, 42.6, 54.3, 60.0, 43.4, 53.7, 40.3, 45.1, 46.0, 44.0, 36.4, 27.3, 39.7, 49.6, 47.9, 43.8 };

for (int i = 0; i < 32; i++) {
  int value = fs[i];

  Serial.print("up=");
  Serial.println(runEvent_up);
  Serial.print("down=");
  Serial.println(runEvent_down);
  Serial.print("value=");
  Serial.println(value);
  if (runEvent_accelerometerOnItsWayUp) {
    Serial.println("+");
  }
  else {
    Serial.println("-");
  }
  Serial.println();

  if (value > runEvent_up) {
    runEvent_accelerometerOnItsWayUp = true;
    runEvent_up = value;
  }
  else {
    if (runEvent_accelerometerOnItsWayUp) { //we're no longer on the way up
      if (runEvent_down + 9 < runEvent_up) {
        runEvent_deltas++;
      }
      
      runEvent_accelerometerOnItsWayUp = false;
      runEvent_down = value;
    }
    else {
      if (runEvent_up - 9 > runEvent_down) {
        runEvent_deltas++;
      }

      runEvent_accelerometerOnItsWayUp = true;
      runEvent_up = value;
    }
  }
}
Serial.print("deltas=");
Serial.println(runEvent_deltas);
Serial.println("Done");
while(1);
*/
  
}

void loop(void) {
  sensors_event_t event;

  int i = 0;
  while (1) {
//    accel.begin();
//    Serial.println("Top of while");
    if (emptyBufferFlag) {
      buffer.clear();
      emptyBufferFlag = false;
    }
    accel.getEvent(&event);
 
    float x = abs(event.acceleration.x);
    float y = abs(event.acceleration.y);
    float z = abs(event.acceleration.z);

    float value = x + y + z;
//    Serial.print("pushing..................................");
//    Serial.println(value);
    buffer.push(value);

    analyse();

    delay(20);

//    if (i++ == 256) {
//      buffer.print();
//      while(1);
//    }
  }
}

void analyse() {
//  Serial.println("analyse()");
  int shakeEvent_peaks = 0;

  float jumpEvent_average1 = buffer.get(0);
  float jumpEvent_average2 = buffer.get(13);
  float jumpEvent_average3 = buffer.get(26);

  float runEvent_down = buffer.get(0);
  float runEvent_up = buffer.get(0);
  bool runEvent_accelerometerOnItsWayUp = false;
  int runEvent_deltas = 0;

//  Serial.print("First value analysed: ");
//  Serial.println(buffer.get(0));
  for (char i = 0; i < 32; i++) {
    float value = buffer.get(i);

    /* Shake is detected by counting the number of values at threshold (60.2). Within
     the last 32 samples, more than 5 peak values is fairly accurate for detecting
     shake.
     */
    if (value > 60.0f) {
      shakeEvent_peaks++;
    }

    /* The time for a jump is a little bit less than 32 samples. At the beginning and
     end of the jump, values are low. In the middle, quite high. So we average the first,
     middle and last six samples to see if they fit this pattern.
     */
     if (i > 0 && i < 6) {
       jumpEvent_average1 += value;
       jumpEvent_average1 /= 2;
     }

     if (i > 13 && i < 19) {
       jumpEvent_average2 += value;
       jumpEvent_average2 /= 2;
     }

     if (i > 26 && i < 32) {
       jumpEvent_average3 += value;
       jumpEvent_average3 /= 2;
     }

     /* Running involves a delta of at least nine, in a sample of 32, about three to
      four up AND down. The data below shows three up movements of at least nine, and
      four down movements:

        Value   Delta      Up    Down
           36
           40       4       
           24     -16               Y
           42      18       Y
           29     -13               Y
           60      31       Y
           40     -20               Y
           46       6
           27     -19               Y
           49      22       Y
      */
      if (value > runEvent_up) {
        runEvent_accelerometerOnItsWayUp = true;
        runEvent_up = value;
      }
      else {
        if (runEvent_accelerometerOnItsWayUp) { //we're no longer on the way up
          if (runEvent_down + 9 < runEvent_up) {
            runEvent_deltas++;
          }
          
          runEvent_accelerometerOnItsWayUp = false;
          runEvent_down = value;
        }
        else {
          if (runEvent_up - 9 > runEvent_down) {
            runEvent_deltas++;
          }

          runEvent_accelerometerOnItsWayUp = true;
          runEvent_up = value;
        }
      }
  }

  if (shakeEvent_peaks > 6) {
    shake();
  }

  if (jumpEvent_average1 + 30 < jumpEvent_average2 && jumpEvent_average3 + 30 < jumpEvent_average2) {
    jump();
  }

  if (runEvent_deltas > 2 && runEvent_deltas < 5) {
//    run();
  }
//  delay(500);
}

void shake() {
//  Serial.print("Shake! ");
//  Serial.println(random(1, 100000));
  Serial.print("Shake! ");
  Serial.println(random(1, 100000));
  emptyBufferFlag = true;
  pulse(5);
//  Serial.print("Shake! ");
//  Serial.println(random(1, 100000));

}

void jump() {
  Serial.print("JuuuuuuUUUUUUuuuuuump ");
  Serial.println(random(1, 100000));
  singleChase(strip.Color(255,   0,   255), 50); 
  emptyBufferFlag = true;
}

void run() {
  Serial.print("Run run run ");
  Serial.println(random(1, 100000));
  blink(50);
  emptyBufferFlag = true;
}

void runAllTests() {
//  To run tests redefine BUFFER_SIZE as 5

//  CircularBufferTest pt;
//  pt.runAllTests();
//  while(1);
}

void singleChase(uint32_t c, uint8_t wait) {
  Serial.println("Begin singleChase");
  strip.setBrightness(255);
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }

  turnOff();
}

void pulse(uint8_t wait) { 
  Serial.println("Begin pulse");
  for (int ii = 0; ii < 255; ++ii) {
    Serial.println(255 - ii);
    for (int i = 0; i < 12; i++) {
      strip.setPixelColor(i, 232, 136, 19);
    }
    strip.setBrightness(255 - ii);

    strip.show();
    delay(wait);
  }

  turnOff();
}

void blink(uint8_t wait) {
  Serial.println("Begin blink");
  for (int j = 0; j < 5; j++) {
//    strip.show();
    strip.setBrightness(255);
Serial.println("setting bright");
    for (int i = 0; i < 12; i++) {
      strip.setPixelColor(i, 0, 255, 170);
    }

    strip.show();
    delay(wait);
    turnOff();
    delay(wait);
  }
}

void turnOff() {
  strip.setBrightness(0);
  strip.show();
}
