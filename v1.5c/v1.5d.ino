/*        GPIO'4'Cars v1.5c & 1.5d
|
|                 ATtiny204 Pin map
|
|                            +-\/-+
|                      VCC  1|O   |14  GND
|    NC           PA4       2|    |13  10    PA3    input_PWM
|    NC           PA5    1  3|    |12  9     PA2    input_5v
|    NC           PA6    2  4|    |11  8     PA1    RESET  (software)           
|    ACC          PA7    3  5|    |10        PA0    UPDI
|    NC           PB3    4  6|    |9   7     PB0    PRES
|    output_PWM   PB2    5  7|    |8   6     PB1    output_Power
|                            +----+                   
*/

const int pin_input_5v = 9;
const int pin_input_12v = 7;
const int pin_input_acc = 3;
const int pin_input_pwm = 10;
const int pin_output_pwm = 5;
const int pin_output_power = 6;
const int pin_reset = 8;

const int input_5v_trigger = 615;      // When N2 powered is of, 5v drops to 2.1V, so we compare to around 3V with the 615 analog value (0 to 1023 range)
int input_5v_analog = 0;
int input_pwm = 0;
int duty_cycle = 0;
int low_duty_cycle = 5;
int hold = 0;
int hold_old = 0;
int on = 0;
int on_old = 0;
int acc = 0;
int acc_old = 0;
uint8_t off_delay = 0;
bool boot_sequence = false;
bool input_reset = false;
bool power_output = false;

uint16_t tm_offdelay = 0;


void setup() {
  pinMode(pin_reset, INPUT_PULLUP);
  pinMode(pin_input_5v, INPUT);
  pinMode(pin_input_12v, INPUT);
  pinMode(pin_input_acc, INPUT);
  pinMode(pin_input_pwm, INPUT);
  pinMode(pin_output_pwm, INPUT);        // PWM DISABLED AT START, BY SETTING PWM AS INPUT
  pinMode(pin_output_power, OUTPUT);

  // Set the system clock frequency (adjust according to your setup)
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm);

  digitalWrite(pin_output_power, LOW);
}

void loop() {

  power_output = false;

  tm_offdelay--;
  if (tm_offdelay == 0xffff) {
  	tm_offdelay = 0;
  }

  delay(100);
  read_input();

  if ((on == 1) && (on_old == 0)) {       // IF 12V IS GOING ON, POWER ON AND SET PWM LOW DUTY CYCLE FOR BOOT SEQUENCE
    boot_sequence = true;
    duty_cycle = low_duty_cycle;
    pwm_setup();
  }

  if ((on == 0) && (on_old == 1)) {       // FORCE SHUTDOWN AFTER DELAY IF ONLY 5V
	  tm_offdelay = 10000;                   // 1000 = ~5 Seconds
  }
  
  if ((hold == 0) && (hold_old == 1)) {   // SHUTDOWN WHEN 5V IS GOING OFF
	  tm_offdelay = 100;
  }

  if ((acc == 1) && (acc_old == 0) && (on == 1) && (hold == 0)) {
    input_reset = true;
  }

  if (tm_offdelay > 0) {
	  off_delay = 1;
  } else {
	  off_delay = 0;
    hold = 0;
    hold_old = 0;
  }


  power_output |= hold;
  power_output |= on;
  power_output |= off_delay;

  if (input_reset == true) {
    power_output = 0;
  }

  if (power_output > 0) {
    poweron();
    if (on_old == 0) {
      delay(1000);
    }
    pwm();
  } else {
    poweroff();
  }

  on_old = on;
  hold_old = hold;
  acc_old = acc;
}

void read_input() {

  input_reset = !digitalRead(pin_reset);        // "!" is here to reverse boolean state as pin_reset is pulled up and short to ground if pressed.
  on = !digitalRead(pin_input_12v);             // "!" is here to reverse boolean state as pin_input_12v is high when car is off due to Q1.
  acc = !digitalRead(pin_input_acc);            // "!" is here to reverse boolean state as pin_input_acc is high when car is off due to Q2.
  input_5v_analog = analogRead(pin_input_5v);

  if (input_5v_analog > input_5v_trigger) {
    hold = true;
  } else {
    hold = false;
  }
  
}

void poweroff() {
  digitalWrite(pin_output_power, LOW);        // DISABLE POWER
  pinMode(pin_output_pwm, INPUT);             // PWM DISABLED, BY SETTING PWM AS INPUT

  if (input_reset == true) {
    delay(20000);
  } else {
    delay(3000);
  }
}

void poweron() {
  digitalWrite(pin_output_power, HIGH);       // ENABLE POWER
}

void pwm() {
  input_pwm = analogRead(pin_input_pwm);                  // Read incoming N2 PWM duty cycle
  if (boot_sequence == false) {                           // Set PWM duty cycle according to incoming N2+ PWM
    duty_cycle = (((input_pwm >> 4) + (input_pwm >> 3)) +1);   // Maximum brightness value is reduced to 70% to prevent damage
    analogWrite(pin_output_pwm, duty_cycle);
  } else {                                                // Set PWM to low duty cycle during boot sequence
    if ((input_pwm <= 400 ) && (input_pwm >= 250)) {   // Check incoming duty cycle of boot sequence 
      boot_sequence = false;                           // Voltage is going around 1.10V during boot. Once found 1.10V, exit boot sequence.
    }
  }
}

void pwm_setup() {
  pinMode(pin_output_pwm, OUTPUT);

  // Set up TCA for PWM
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc;  // Use the system clock directly
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_FRQ_gc;   // Frequency generation mode
  TCA0.SINGLE.PER = 250;                          // Set PER for desired frequency (170 giving 23KHz for 1MHz system clock)
  TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;      // Enable the TCA0 timer
  //                                                                system clock frequency
  //                                                PWM Frequency =  --------------------
  //                                                                        PER
  analogWrite(pin_output_pwm, low_duty_cycle);
}
