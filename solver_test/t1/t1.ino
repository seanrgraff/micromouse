#include <Solver.h>
#include <Button.h>


// Pin Declarations
#define NORTH_SENSOR_PIN 13
#define EAST_SENSOR_PIN 12
#define WEST_SENSOR_PIN 11

#define NORTH_LED_PIN 7
#define EAST_LED_PIN 6
#define WEST_LED_PIN 5
#define SOUTH_LED_PIN 4

#define MODE_LED_PIN 3

#define BUTTON_PIN 2

// Instantiate Diagnostics Object
Solver solver(NORTH_SENSOR_PIN, EAST_SENSOR_PIN, WEST_SENSOR_PIN, \
                        NORTH_LED_PIN,    EAST_LED_PIN,    WEST_LED_PIN, \
                        SOUTH_LED_PIN, MODE_LED_PIN);

Button button(BUTTON_PIN, PULLDOWN);

int count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(button.uniquePress()){
      //Serial.println("button pressed");
      solver.calculateNextMovement();
    count++;
    Serial.println(count);
  }
  solver.getDiagnostics()->update();
  
  delay(10);
}










