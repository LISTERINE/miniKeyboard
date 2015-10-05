/* Buttons to USB Keyboard Example

   You must select Keyboard from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

#include <Bounce.h>

// Create Bounce objects for each button.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
Bounce button0 = Bounce(22, 10);
Bounce button1 = Bounce(21, 10);  // 10 = 10 ms debounce time
Bounce button2 = Bounce(20, 10);  // which is appropriate for
Bounce button3 = Bounce(3, 10);  // most mechanical pushbuttons
Bounce button4 = Bounce(4, 10);
Bounce button5 = Bounce(5, 10);  // if a button is too "sensitive"
Bounce button6 = Bounce(6, 10);  // to rapid touch, you can
Bounce button7 = Bounce(7, 10);  // increase this time.
Bounce button8 = Bounce(8, 10);
Bounce button9 = Bounce(9, 10);


int modKey = -1; // Which button is the current modifier

void winR() {
  Keyboard.set_modifier(MODIFIERKEY_GUI); // Press and hold win/clover key
  Keyboard.set_key1(KEY_R);
  Keyboard.send_now();
  Keyboard.set_key1(0);
  Keyboard.set_modifier(0); // Clear modifiers
  Keyboard.send_now();
  delay(300);
}

void setup() {
  // Configure the pins for input mode with pullup resistors.
  // The pushbuttons connect from each pin to ground.  When
  // the button is pressed, the pin reads LOW because the button
  // shorts it to ground.  When released, the pin reads HIGH
  // because the pullup resistor connects to +5 volts inside
  // the chip.  LOW for "on", and HIGH for "off" may seem
  // backwards, but using the on-chip pullup resistors is very
  // convenient.  The scheme is called "active low", and it's
  // very commonly used in electronics... so much that the chip
  // has built-in pullup resistors!
  pinMode(22, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);  // Teensy++ LED, may need 1k resistor pullup
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
}

void loop() {
  // Update all the buttons.  There should not be any long
  // delays in loop(), so this runs repetitively at a rate
  // faster than the buttons could be pressed and released.
  button0.update();
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();
  button6.update();
  button7.update();
  button8.update();
  button9.update();

  // Check each button for "falling" edge.
  // Type a message on the Keyboard when each button presses
  // Update the Joystick buttons only upon changes.
  // falling = high (not pressed - voltage from pullup resistor)
  //           to low (pressed - button connects pin to ground)
  if (button0.fallingEdge()) {
    //
    // Modifier goal: remote (putty, ssh, RD, etc...)
    //
    switch (modKey) {
      case 1:
        // Docker
        Keyboard.println("docker run --rm -it ubuntu:latest /bin/bash");
        Keyboard.send_now();
        break;
      case 2:
        // Corp special
        winR();
        Keyboard.println("putty.exe -load \"DevEnv\"");
        Keyboard.send_now();
        delay(1500);
        Keyboard.println("vulndb")
        Keyboard.send_now();
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      default:
        modKey = 0;
        break;
    }
  }
  if (button1.fallingEdge()) {
    //
    // Modifier goal: docker
    //
    switch (modKey) {
      case 0:
        // ssh/putty
        winR();
        Keyboard.println("putty.exe -load \"DevEnv\"");
        Keyboard.send_now();
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      default:
        modKey = 1;
        break;
    }
  }
  if (button2.fallingEdge()) {
    //
    // Corp special shortcuts for work
    //
    switch (modKey) {
      case 0:
        // ssh/putty
        break;
      case 1:
        // Docker
        Keyboard.println("docker run --rm -it centos:latest /bin/bash");
        Keyboard.send_now();
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      default:
        modKey = 2;
        break;
    }
  }
  if (button3.fallingEdge()) {
    switch (modKey) {
      case 0:
        // ssh/putty
        break;
      case 1:
        // Docker
        break;
      case 2:
        // Corp special
        break;
      case 4:
        break;
      case 5:
        break;
      default:
        modKey = 3;
        break;
    }
  }
  if (button4.fallingEdge()) {
    switch (modKey) {
      case 0:
        // ssh/putty
        break;
      case 1:
        // Docker
        break;
      case 2:
        // Corp special
        break;
      case 3:
        break;
      case 5:
        break;
      default:
        modKey = 4;
        break;
    }
  }
  if (button5.fallingEdge()) {
    switch (modKey) {
      case 0:
        // ssh/putty
        winR();
        Keyboard.println("putty.exe -load \"vultr_biggest\"");
        Keyboard.send_now();
        break;
      case 1:
        // Docker
        break;
      case 2:
        // Corp special
        break;
      case 3:
        break;
      case 4:
        break;
      default:
        modKey = 5;
        break;
    }
  }
  

  // Check each button for "rising" edge
  // Type a message on the Keyboard when each button releases.
  // For many types of projects, you only care when the button
  // is pressed and the release isn't needed.
  // rising = low (pressed - button connects pin to ground)
  //          to high (not pressed - voltage from pullup resistor)
  if (button0.risingEdge()) {
    modKey = -1;
  }
  if (button1.risingEdge()) {
    modKey = -1;
  }
  if (button2.risingEdge()) {
    modKey = -1;
  }
  if (button3.risingEdge()) {
    modKey = -1;
  }
  if (button4.risingEdge()) {
    modKey = -1;
  }
  if (button5.risingEdge()) {
    modKey = -1;
  }
}

