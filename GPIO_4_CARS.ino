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

const int analog_5v_value = 615; // When N2 powered is of, 5v drops to 2.1V, so we compare to around 3V with the 615 analog value.
const int boot_time_duration = 1000; // PWM will be generated with low duty cycle during boot process.
const int low_duty_pwm_value = 38; // Low duty cycle value (around 10%)
const int emergency_delay = 30000; // Time remaining before forced shutdown (if for some reason OS is not shutting down properly)
int emergency_time = emergency_delay;
int input_pwm = 0;
int input_5v = 0;
int boot_time = 0;
bool boot_sequence = true;
bool input_12v = false;
bool power_output = false;
bool old_power_output = false;
 
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
  if ((input_12v == true) || (input_5v > analog_5v_value)) {   // input_12v OR input_5v are HIGH
    power_output = true;
  } else {
    power_output = false;
  }
  if ((input_12v == false) && (input_5v > analog_5v_value)) {   // EMERGENCY CASE: input_12v LOW AND input_5v HIGH, meaning that the OS is not shutting down properly.
    emergency_time = (emergency_delay - 100);
    if (emergency_time <= 0); {
      emergency_time = emergency_delay;
      power_output = false;
    }
  }
  if (power_output != old_power_output) {
    if (power_output == true) {
      boot_sequence = true;
      digitalWrite(pin_n2_power, HIGH);  
    } else {
      delay(300);
      digitalWrite(pin_n2_power, LOW);
    }
  old_power_output = power_output;
  }
}

void pwm() {                                      // Set PWM according to incoming N2+ PWM
  if (boot_sequence == false) {
    
    input_pwm = analogRead(pin_input_pwm);
    input_pwm = ((input_pwm * 10) /57);
    analogWrite(pin_pwm_output, input_pwm);
  } else {                                        // Set PWM to low duty cycle during boot process
    if (boot_time >= 0) {
      analogWrite(pin_pwm_output, low_duty_pwm_value);
      boot_time = (boot_time - 100);
      if (boot_time <= 0) {
        boot_time = boot_time_duration;
        boot_sequence = false;
      }
    }
  }
}

