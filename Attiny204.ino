/*        GPIO'4'Cars            

                 ATtiny204 Pin map

                       +-\/-+
                  VCC 1|O   |14 GND
    NC                2|    |13 10    PA3   input_PWM
    NC             1  3|    |12 9     PA2   input_5v
    NC             2  4|    |11 8     RESET (software)           
    ACC            3  5|    |10       UPDI
    NC             4  6|    |9  7     PRES
    output_PWM     5  7|    |8  6     PB1   output_Power
                       +----+
                    
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
int low_duty_cycle = 10;
int hold = 0;
int hold_old = 0;
int on = 0;
int on_old = 0;
int acc = 0;
int acc_old = 0;
uint8_t off_delay = 0;
bool boot_sequence = false;
bool input_reset = false;
bool input_5v = false;
bool input_12v = false;
bool input_acc = false;
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

  if (input_12v == true) {
    on = 1;
  } else {
    on = 0;
  }

  if (input_5v == true) {
    hold = 1;
  } else {
    hold = 0;
  }

  if ((on == 1) && (on_old == 0)) {       // IF N2 IS STARTING, SET PWM LOW DUTY CYCLE FOR BOOT SEQUENCE
    boot_sequence = true;
    duty_cycle = low_duty_cycle;
    pwm_setup();
  }

  if ((on == 0) && (on_old == 1)) {       // FORCE SHUTDOWN AFTER DELAY IF ONLY 5V
	  tm_offdelay = 1000;                   // 1000 = ~25 Seconds
  }
  on_old = on;

  if ((hold == 0) && (hold_old == 1)) {
	  tm_offdelay = 100;
  }
  hold_old = hold;

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
    pwm();
  } else {
    poweroff();
  }
}


void read_input() {                             // READING INPUTS

  input_reset = !digitalRead(pin_reset);        // "!" is here to reverse boolean state as pin_reset is pulled up and short to ground if pressed.
  input_5v_analog = analogRead(pin_input_5v);
  input_12v = digitalRead(pin_input_12v);
//  input_acc = !digitalRead(pin_input_acc);     // "!" is here to reverse boolean state as ACC input is reversed by Q2 transistor.

  if (input_5v_analog > input_5v_trigger) {
    input_5v = true;
  } else {
    input_5v = false;
  }
}

void poweroff() {
  digitalWrite(pin_output_power, LOW);        // DISABLE POWER
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
    duty_cycle = ((input_pwm >> 4) + (input_pwm >> 3));   // Maximum brightness value is reduced to 70% to prevent damage
  } else {                                                // Set PWM to low duty cycle during boot sequence
    if (input_pwm <= 950 ) {                              // Compare maximum duty cycle of boot sequence with lower value at end of boot sequence
      boot_sequence = false;
    }
  }
}

void pwm_setup() {
  pinMode(pin_output_pwm, OUTPUT);

// Set up TCA for PWM
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc;  // Use the system clock directly
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_FRQ_gc;   // Frequency generation mode
  TCA0.SINGLE.PER = 170;                          // Set PER for desired frequency (Giving 23KHz)
  TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;      // Enable the TCA0 timer
//                                                                system clock frequency
//                                                PWM Frequency =  --------------------
//                               
  analogWrite(pin_output_pwm, duty_cycle);
}
