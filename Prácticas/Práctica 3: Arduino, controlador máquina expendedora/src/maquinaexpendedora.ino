// LIBRARIES
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include <LiquidCrystal.h>
#include <DHT.h>

// ARDUINO THREAD
class LedThread: public Thread {
public:
    int pin;
    int num_blinks;
    bool state;
    LedThread(int _pin): Thread() {
        pin = _pin;
        state = true;
        num_blinks = 0;
        pinMode(pin, OUTPUT);
    }
    bool shouldRun(unsigned long time){
        return Thread::shouldRun(time);
    }
    void run(){
        Thread::run();
        digitalWrite(pin, state ? HIGH : LOW);
        state = !state;
        num_blinks ++;
    }
};

// PIN CONSTANTS AND SENSORS DATA
#define PIN_LED1 9
#define PIN_LED2 8
#define PIN_BUTTON 13
#define PIN_TRIGGER 7
#define PIN_ECHO 6
#define PIN_TEMPERATURE 10
#define PIN_X A1
#define PIN_Y A0
#define PIN_SWITCH 1
DHT dht(PIN_TEMPERATURE, DHT11);
LiquidCrystal lcd(12,11,5,4,3,2);
ThreadController controller = ThreadController();
LedThread* ledThread = new LedThread(PIN_LED1);
Thread button_Thread = Thread();
int distance, x_value, y_value, button_value, intensity, button_pressed, first_button_pressed;
float temperature, humidity;

// NENU OPTIONS AND STATES
#define NUMBER_PRODUCTS_OPTIONS 5
#define NUMBER_ADMIN_OPTIONS 4
#define START 1
#define WAITING_CUSTOMER 2
#define CUSTOMER_FOUND 3
#define PREPARING_COFFEE 4
#define ADMINISTRATOR 5
int state, number_products_options, number_admin_options;
String products[] = {"Cafe Solo", "Cafe Cortado", "Cafe Doble", "Cafe Premium", "Chocolate"};
float prizes[] = {1.00, 1.10, 1.25, 1.50, 2.00};
String admin_menu_options[] = {"Ver temperatura" , "Ver distancia sensor", "Ver contador", "Modificar precio"};
byte euro[] {0x07,0x0C,0x08,0x1E,0x1E,0x08,0x0C,0x07,};

// TIMES (SECONDS)
int time, time_preparation, time_button_pressed;

// TIMES (MILLIS() FUNCTION)
unsigned long time_switch, time_dht, time_preparing, time_intensity, time_removedrink_message, time_lcd, time_press_button, time_hold_button;

// METHODS
void administration_menu(int number_admin_options) {
  number_products_options = 0;
  time_switch = millis();
  do {
    y_value = analogRead(PIN_Y);
    switch (number_admin_options) {
      case 0:
        if ((millis() - time_lcd) > 300) {
          humidity  = dht.readHumidity();
          temperature = dht.readTemperature();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.write("Temp: ");
          lcd.print(temperature);
          lcd.print((char)223);
          lcd.write("C");
          lcd.setCursor(0, 1);
          lcd.write("Hum: ");
          lcd.print(humidity);
          lcd.write("%");
          time_lcd = millis();
        }
        break;
      case 1:
        if ((millis() - time_switch) > 200) {
          digitalWrite(PIN_TRIGGER, HIGH);
          delayMicroseconds(10);
          digitalWrite(PIN_TRIGGER, LOW);
          time = pulseIn(PIN_ECHO, HIGH);
          distance = time/59;
          lcd.clear();
          lcd.print(distance);
          lcd.write(" cm");
          time_switch = millis();
        }
        break;
      case 2:
        if ((millis() - time_switch) > 200) {
          lcd.clear();
          lcd.print(millis() / 1000);
          time_switch = millis();
        }
        break;
      case 3:
        if ((millis() - time_switch) > 200) {
          button_value = digitalRead(PIN_SWITCH);
          number_products_options = read_joystick_values(number_products_options, NUMBER_PRODUCTS_OPTIONS);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(products[number_products_options]);
          lcd.setCursor(0,1);
          lcd.print(prizes[number_products_options]);
          lcd.write(byte(0));         
          if (button_value == 0) {
            time_switch = millis();
            float new_price = prizes[number_products_options];
            do {
              if ((millis() - time_switch) > 200) {
                y_value = analogRead(PIN_Y);
                x_value = analogRead(PIN_X);
                button_value = digitalRead(PIN_SWITCH);
                if (x_value <= 100) {
                  new_price += 0.05;
                }
                if (x_value >= 1000) {
                  if (new_price > 0.05) {
                    new_price -= 0.05;
                  }
                }
                if (button_value == 0) {
                  prizes[number_products_options] = new_price;        
                }
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print(products[number_products_options]);
                lcd.setCursor(0,1);
                lcd.print(new_price);
                lcd.write(byte(0));
                time_switch = millis();
              }
            } while (y_value < 950);
          }
          time_switch = millis();
        }
        break;        
    }
  } while (y_value < 950);    
}

void button_actions() {
  if (state != START) {
    button_pressed = digitalRead(PIN_BUTTON);
    if (button_pressed == 1) {
      if (first_button_pressed == 1) {
        time_press_button = millis();
        first_button_pressed = 0;
      }
      else {
        time_hold_button = millis();
      }
    }
    else {
      first_button_pressed = 1;
      time_button_pressed = time_hold_button - time_press_button;
    }
    if ((time_button_pressed) < (3 * 1000) && (time_button_pressed) > (2 * 1000)) {
      state = WAITING_CUSTOMER;
      time_hold_button = 0;
      time_press_button = 0;
    }
    if ((time_button_pressed) > (5 * 1000)) {
      if (state == ADMINISTRATOR) {
        state = WAITING_CUSTOMER;
      }
      else {
        state = ADMINISTRATOR;       
      }
      time_hold_button = 0;
      time_press_button = 0;
    }
  }
}

int read_joystick_values(int index, int num_elements) {
  x_value = analogRead(PIN_X);
  if (x_value <= 100) {
    if (index == 0) {
      index = num_elements - 1;
    }
    else {
      index -= 1;
    }
  }
  if (x_value >= 1000) {
    if (index == num_elements - 1) {
      index = 0;
    }
    else {
      index += 1;
    }
  }
  return index;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_X, INPUT);
  pinMode(PIN_Y, INPUT);
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_SWITCH, HIGH);
  dht.begin();
  lcd.begin(16,2);
  lcd.createChar(0, euro);
  ledThread->setInterval(1000);
  button_Thread.enabled = true;
  button_Thread.setInterval(300);
  button_Thread.onRun(button_actions);
  controller.add(ledThread);
  controller.add(&button_Thread);
  state = START;
  number_products_options = 0;
  number_admin_options = 0;
  time_switch = millis();
  time_lcd = millis();
  time_hold_button = millis();
  first_button_pressed = 0;
  time_button_pressed = 0;
  lcd.clear();
}

void loop() {
  controller.run();
  if (state == START) {
    if (ledThread->num_blinks >= 6) {
      ledThread->num_blinks = 0;
      controller.remove(ledThread);
      state = WAITING_CUSTOMER;
    }
    else {
      controller.add(ledThread);
      if ((millis() - time_lcd) > 300) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write("  CARGANDO ...  ");
        lcd.setCursor(0,1);
        lcd.write("");
        time_lcd = millis();
      }  
    }
  }
  else if (state == WAITING_CUSTOMER) {
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);
    if ((millis() - time_lcd) > 300) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write("   ESPERANDO    ");
      lcd.setCursor(0,1);
      lcd.write("    CLIENTE     ");
      time_lcd = millis();
    }
    digitalWrite(PIN_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIGGER, LOW);
    time = pulseIn(PIN_ECHO, HIGH);
    distance = time/59;
    if (distance < 100) {
      state = CUSTOMER_FOUND;
      time_dht = millis();
    }
  }
  else if (state == CUSTOMER_FOUND) {
    if ((millis() - time_dht) < 5000) {
      if ((millis() - time_lcd) > 300) {
        humidity  = dht.readHumidity();
        temperature = dht.readTemperature();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("Temp: ");
        lcd.print(temperature);
        lcd.print((char)223);
        lcd.write("C");
        lcd.setCursor(0, 1);
        lcd.write("Hum: ");
        lcd.print(humidity);
        lcd.write("%");
        time_lcd = millis();
      }
      time_switch = millis();
      number_products_options = 0;
    }
    else {   
      if ((millis() - time_switch) > 200) {
        button_value = digitalRead(PIN_SWITCH);
        number_products_options = read_joystick_values(number_products_options, NUMBER_PRODUCTS_OPTIONS);
        if (button_value == 0) {
          state = PREPARING_COFFEE;
          time_preparation = random(4,8) * 1000;
          time_preparing = millis();
          time_intensity = millis();
          intensity = 0;
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(products[number_products_options]);
        lcd.setCursor(0,1);
        lcd.print(prizes[number_products_options]);
        lcd.write(byte(0));
        time_switch = millis();
      }      
    }
  }
  else if (state == PREPARING_COFFEE) {
    if ((millis() - time_preparing) < time_preparation) {
      if ((millis() - time_lcd) > 300) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write("   PREPARANDO   ");
        lcd.setCursor(0,1);
        lcd.write("    CAFE ...    ");
        time_lcd = millis();
      }
      if ((millis() - time_intensity) > (time_preparation / 255)) {
        intensity += 1;
        analogWrite(PIN_LED2, intensity);
        time_intensity = millis();
        time_removedrink_message = millis();
      }
    }
    else {
      if ((millis() - time_removedrink_message) < 3000) {        
        if ((millis() - time_lcd) > 300) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.write("     RETIRE     ");
          lcd.setCursor(0,1);
          lcd.write("     BEBIDA     ");
          time_lcd = millis();
        }
      }
      else {
        digitalWrite(PIN_LED2, LOW);
        state = WAITING_CUSTOMER;
      }
    }
  }
  else if (state == ADMINISTRATOR) {
    digitalWrite(PIN_LED1, HIGH);
    digitalWrite(PIN_LED2, HIGH);
    if ((millis() - time_switch) > 200) {
      number_admin_options = read_joystick_values(number_admin_options, NUMBER_ADMIN_OPTIONS);
      button_value = digitalRead(PIN_SWITCH);
      lcd.clear();
      if (number_admin_options != 1) {
        lcd.setCursor(0,0);      
        lcd.print(admin_menu_options[number_admin_options]);
      }
      else {
        if ((millis() - time_lcd) > 300) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.write("Ver distancia");
          lcd.setCursor(0,1);
          lcd.write("sensor");
          time_lcd = millis();
        }
      }
      if (button_value == 0) {
        administration_menu(number_admin_options);
      }
      time_switch = millis();
    }
  }
}