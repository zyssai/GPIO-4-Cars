/*

  GPIO'4'Cars
                 ATtiny85 Pin map

                    +-\/-+
   NC         PB5  1|O   |8  VCC
   input_5v     3  2|    |7  2   pwm_input
   input_12v   A2  3|    |6  1   n2_power
              GND  4|    |5  0   pwm_output
                    +----+
                    
*/
 
const int pin_input_12v = A2;
const int pin_input_5v = 3;
const int pin_pwm_relay = 2;
const int pin_n2_power = 1;
const int pin_pwm_output = 0;

const int analog_value = 800;
int input_5v = 0;
bool input_12v = false;
 
void setup() {
  pinMode(pin_input_12v, INPUT);
  pinMode(pin_input_5v, INPUT);
  pinMode(pin_pwm_relay, OUTPUT);
  pinMode(pin_n2_power, OUTPUT);
  pinMode(pin_pwm_output, OUTPUT);
}

void loop() {
  power_rail();
}

void power_rail() {    // Set n2_power pin HIGH if input_12v OR input_5v are HIGH
  input_12v = digitalRead(pin_input_12v);
  input_5v = analogRead(pin_input_5v);
  if ((input_12v = true) || (input_5v > analog_value)) {
    pwm_screen();
    digitalWrite(pin_n2_power, HIGH);
  }
  else{
    delay(1000);
    digitalWrite(pin_n2_power, LOW);
  }
}

void pwm_screen() {    // Set pwm pin ON or OFF.
  analogWrite(pin_pwm_output, 38);
  digitalWrite(pin_pwm_relay, HIGH);
  delay(200);
}