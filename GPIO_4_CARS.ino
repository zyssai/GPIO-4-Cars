/*

  GPIO'4'Cars
                 ATtiny85 Pin map

                    +-\/-+
 NC            PB5 1|O   |8 VCC
 input_pwm  A3 PB3 2|    |7 PB2 A1 input_5v
 ouput_power 4 PB4 3|    |6 PB1 1  input_12v
               GND 4|    |5 PB0 0  output_pwm                 
                    +----+
                    
*/

#define pwm_duty_cycle OCR0A

const int pin_input_5v = A1;
const int pin_input_12v = 1;
const int pin_input_pwm = A3;
const int pin_output_pwm = 0;
const int pin_output_power = 4;

unsigned long boot_time = 0;
const int analog_5v_value = 615;      // When N2 powered is of, 5v drops to 2.1V, so we compare to around 3V with the 615 analog value (0 to 1023 range)
const int boot_time_duration = 30000; // PWM will be generated with low duty cycle during boot process.
const int low_duty_pwm_value = 38;    // Low duty cycle value (around 10%)
int input_pwm = 0;
int input_5v = 0;
bool boot_sequence = true;
bool input_12v = false;
bool power_output = false;
bool old_power_output = true;
 
void setup() {
//   emergency_time = emergency_delay;
  pinMode(pin_input_pwm, INPUT);
  pinMode(pin_input_5v, INPUT);
  pinMode(pin_input_12v, INPUT);
  pinMode(pin_output_power, OUTPUT);
  pinMode(pin_output_pwm, INPUT);        // PWM DISABLED AT START, BY SETTING PWM AS INPUT

  CLKPR = (1<<CLKPCE); // Prescaler enable
  CLKPR = ((0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (1<<CLKPS0));  // Clock division factor by 2  (0001)  
//  CLKPR = _BV(CLKPS0);
}

void pwm_setup() {
  pinMode(pin_output_pwm, OUTPUT);

  TCCR0A = _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
  TCCR0B = _BV(CS00);
  pwm_duty_cycle = low_duty_pwm_value;  // OCR0A
}

void loop() {
  power();
  pwm();  
}

void power() {    // Set output_power pin HIGH if input_12v OR input_5v are HIGH

  input_5v = analogRead(pin_input_5v);
  input_12v = digitalRead(pin_input_12v);
  
  if ((input_12v == true) || (input_5v > analog_5v_value)) {   // input_12v OR input_5v are HIGH
    power_output = true;
  } else {
    power_output = false;
  }

/* if ((input_12v == false) && (input_5v > analog_5v_value)) {   // EMERGENCY CASE: input_12v LOW AND input_5v HIGH, meaning that the OS is not shutting down properly.
     emergency_time--;
    if (emergency_time <= 0); {
      emergency_time = emergency_delay;
      power_output = false;
    }
  }
*/

  if (power_output != old_power_output) {
    if (power_output == true) {
      boot_sequence = true;
      boot_time = millis();
      pwm_setup();                                // CONFIGURE PWM
      digitalWrite(pin_output_power, HIGH);       // ENABLE POWER  
    } else {
      delay(300);
      digitalWrite(pin_output_power, LOW);        // DISABLE POWER
      delay(300);                         
      pinMode(pin_output_pwm, INPUT);             // TO DISABLE PWM OUTPUT, CHANGING PIN MODE TO INPUT
    }
  old_power_output = power_output;
  }
}

void pwm() {                                      // Set PWM according to incoming N2+ PWM
  if (power_output == true) {
    if (boot_sequence == false) {
  
    input_pwm = analogRead(pin_input_pwm);
    pwm_duty_cycle = ((input_pwm >> 4) + (input_pwm >> 3));

   } else {                                       // Set PWM to low duty cycle during boot process
      pwm_duty_cycle = low_duty_pwm_value;
      if ((millis()) >= (boot_time + 10000)) {
        boot_sequence = false;
      }
    }
  }
}
