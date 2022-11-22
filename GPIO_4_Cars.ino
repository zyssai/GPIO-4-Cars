/*

  GPIO'4'Cars
                 ATtiny85 Pin map

                    +-\/-+
 NC            PB5 1|O   |8 VCC
 input_pwm  A3 PB3 2|    |7 PB2 A1 input_5v
 n2_power    4 PB4 3|    |6 PB1 1  input_12v
               GND 4|    |5 PB0 0  pwm_output                  
                    +----+
                    
*/
 
const int pin_input_pwm = A3;
const int pin_input_5v = A1;
const int pin_input_12v = 1;
const int pin_n2_power = 4;
const int pin_pwm_output = 0;
const int analog_value = 800;
int input_pwm = 0;
int input_5v = 0;
bool input_12v = false;
bool power_output = false;
 
void setup() {
  pinMode(pin_input_pwm, INPUT);
  pinMode(pin_input_5v, INPUT);
  pinMode(pin_input_12v, INPUT);
  pinMode(pin_n2_power, OUTPUT);
  pinMode(pin_pwm_output, OUTPUT);
}

void loop() {
  power();
  pwm();
}

void power() {    // Set n2_power pin HIGH if input_12v OR input_5v are HIGH
  input_5v = analogRead(pin_input_5v);
  input_12v = digitalRead(pin_input_12v);
  if ((input_12v = true) || (input_5v > analog_value)) {
    power_output = true;
    analogWrite(pin_pwm_output, 38);
    digitalWrite(pin_n2_power, HIGH);
  }
  else{
    power_output = false;
    delay(1000);
    digitalWrite(pin_n2_power, LOW);
  }
}

void pwm() {    // Set PWM
  if (power_output = true) {
    input_pwm = analogRead(pin_input_pwm);
    analogWrite(pin_pwm_output, pin_input_pwm);
  }
}
