// Joytstick pins and output
int analogPin0 = A0;  // A0 on the arduino and VRx on the joystick
int analogPin1 = A1;  // A1 on the arduino and VRy on the joystick

float forward_back_joystick_val = 0;  // read values from analogPin0
float left_right_joystick_val = 0;    // read values from analogPin1

// Ideal speed we'd like to set the motors to:
// -1 full speed backwards, +1, full speed forward
float left_motor_speed = 0;
float right_motor_speed = 0;
float steering = 0; // Signal to add to the left/right motor to turn. -1 to +1

// Motor pins - these all need to be "PWM" pins - they have a tilde
// printed next to the pin-number on the arduino board.
int motor_left_pin_A = 5; // Marked "in1" on the H-bridge, ~3 on the arduino
int motor_left_pin_B = 3; // Marked "in2" on the H-bridge, ~5 on the arduino
int motor_right_pin_A = 9; // Marked "in3" on the H-bridge, ~6 on the arduino
int motor_right_pin_B = 6; // Marked "in4" on the H-bridge, ~9 on the arduino

// The input sensor (the joystick) gives a bit of a noisy signal.
// We ignore a small bit of deflection of the joystick as probably noise.
float input_dead_band = 0.05;



void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read and normalize the joystick input.
  forward_back_joystick_val = normalize_joystick_input_signal(analogPin0);
  // To debug: print out the values on the serial output.
  // Serial.print("analogPin0 (forward-back): ");
  // Serial.print(forward_back_joystick_val);
  // Serial.println("");
  
  left_right_joystick_val = normalize_joystick_input_signal(analogPin1);
  // Serial.print("analogPin1 (left-right): ");
  // Serial.print(left_right_joystick_val);
  // Serial.println("");
  
  // Note that we constrain the motor speeds after adding in the steering value.
  steering = left_right_joystick_val;
  left_motor_speed = constrain(forward_back_joystick_val + steering, -1.0, 1.0);
  right_motor_speed = constrain(forward_back_joystick_val - steering, -1.0, 1.0);
  // Serial.print("Left motor speed: ");
  // Serial.print(left_motor_speed);
  // Serial.print("    ");
  // Serial.print("Right motor speed: ");
  // Serial.print(right_motor_speed);
  // Serial.println("");
  
  // Start your motor running
  set_motor_speed(motor_left_pin_A, motor_left_pin_B, left_motor_speed);
  set_motor_speed(motor_right_pin_A, motor_right_pin_B, right_motor_speed);

  
  // Don't really need more - and makes the serial easier to read.
  delay(50);
}


// normalize_joystick_input_signal reads from a pin whose value varies between
// 0-1024, and returns a signal from -1 to +1.
float normalize_joystick_input_signal(int pin) {
  return map(analogRead(pin), 0, 1024, -1024.0, 1024.0) / 1024.0;
}


// set the speed of one motor's pins. 
// Signal should be a float of -1 to +1 where -1 is full
// speed backwards and +1 is full speed forwards.
void set_motor_speed(int pin_A, int pin_B, float input_signal) {
  // If we've only been asked to move under "input_dead_band", don't
  // try to move at all. The motors won't turn anyway, and the signal
  // is likely just input noise.
  if (input_signal < input_dead_band && input_signal > -input_dead_band) {
    analogWrite(pin_A, 0);
    analogWrite(pin_B, 0);
  }
  
  // The output is expected on the pin is 0-255
  int output_signal = int(255)*abs(input_signal);

  // Forward vs backward is selected based on which pin on the
  // H-bridge gets the signal. Pin A, and we go forward.
  // Pin B and we go backward. Both on at the same time? Who knows?
  if (input_signal > 0) {
    analogWrite(pin_A, output_signal);
    analogWrite(pin_B, 0);

  } else {
    analogWrite(pin_B, output_signal);
    analogWrite(pin_A, 0);

  }
}
