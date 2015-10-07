/* 
 Turn mechanical key tester into a macro mini keyboard
*/

#include <Bounce.h>

#define KEYCOUNT 6 // Number of keys we have 

/********************************************
 * Set up enums
 */
 
// OS enumeration
enum OSType {
  WINDOWS,
  UBUNTU,
  CENTOS
};

// Make a corresponding enum that creates a directive for each key
// enum index are respective of keys[] index
enum directive {
  REMOTE, // ssh and stuff
  DOCKER, // docker things
  WORK_TOOLS, // work specific macros
  RESERVED_A, // reserved for later
  RESERVED_B, // reserved for later
  OS_SWITCH // change which OS we're on
};

/*
 * End enum setup section
 *******************************************/

/********************************************
 * Array of keys setup here
 */
 
// Create Bounce objects for each key.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
// 10 = 10 ms debounce time
// which is appropriate for most mechanical pushbuttons
// if a key is too "sensitive" to rapid touch, you canincrease this time.
Bounce keys[KEYCOUNT] = {
  Bounce(22, 10),
  Bounce(21, 10),
  Bounce(20, 10),
  Bounce(3, 10),
  Bounce(4, 10),
  Bounce(5, 10)
};

/*
 * End key setup section
 *******************************************/
 
/********************************************
 * State vars setup globally here
 */
 
// Which key is the current modifier
int modKey = -1;

// The current OS profile
OSType currentOS = WINDOWS;

/*
 * State vars end
 *******************************************/

/************************************
 * Handy shortcuts get setup here, not for actual full macros
 */

// Shortcut for hitting win+r
void winR(int sleep) {
  Keyboard.set_modifier(MODIFIERKEY_GUI); // Press and hold win/clover key
  Keyboard.set_key1(KEY_R);
  Keyboard.send_now();
  Keyboard.set_key1(0);
  Keyboard.set_modifier(0); // Clear modifiers
  Keyboard.send_now();
  delay(sleep);
}

// Shortcut for maximizing windows in windows
void winMaximize(boolean withTmux) {
  Keyboard.set_modifier(MODIFIERKEY_GUI); // Press and hold win/clover key
  Keyboard.set_key1(KEY_UP);
  Keyboard.send_now();
  Keyboard.set_key1(0);
  Keyboard.set_modifier(0); // Clear modifiers
  Keyboard.send_now();
  delay(300);
  if (withTmux) {
    Keyboard.println("tmux");
    Keyboard.send_now();
  }
}

/*
 * Shortcut section end
 *******************************************/

/*******************************************
 * Macro functions
 * These are the functions that will actually be triggered by our key presses
 * We will use them in our function array
 */
// Remote stuff
void sshWorkEnv() {
  winR(1000);
  Keyboard.println("\"C:\\Program Files (x86)\\PuTTY\\putty.exe\" -load \"DevEnv\"");
  Keyboard.send_now();
  delay(1500);
  winMaximize(true);
}

// Docker section
void dockerUbuntu() {
  Keyboard.println("docker run --rm -it ubuntu:latest /bin/bash");
  Keyboard.send_now();
}

void dockerCentos() {
  Keyboard.println("docker run --rm -it centos:latest /bin/bash");
  Keyboard.send_now();
}

// Corp macros
void sshVDB() {
  sshWorkEnv();
  delay(700);
  Keyboard.println("vulndb");
  Keyboard.send_now();
}

// OS profile modifiers
void setOSWindows() {
  currentOS = WINDOWS;
}

void setOSUBUNTU() {
  currentOS = UBUNTU;
}

void setOSCENTOS() {
  currentOS = CENTOS;
}

// This function is a place holder for keys that have no function yet
void notImplemented() {}

// This function is used as a place holder for a mod key in it's own row. 
// If you're pressing key 0 to mod, you can't also press it for a function, because you can't press a key that's aleady pressed.
void self() {}

/*
 * Macro section end
 *******************************************/

/*******************************************
 * Here we fill up our array of functions
 * index [0][1] means key 0 is the modkey and then they hit key 1
 *  function key ----------->
 *  modifier key {{ self, f01,  f02 },
 *             |  { f10,  self, f12 },
 *             |  { f20,  f21,  self}}
 *             V     
 */

void (*macros[KEYCOUNT][KEYCOUNT])() = {
  {self,           sshWorkEnv,     notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
  {dockerUbuntu,   self,           dockerCentos,   notImplemented, notImplemented, notImplemented}, // Docker
  {sshVDB,         notImplemented, self,           notImplemented, notImplemented, notImplemented}, // Corp macros
  {notImplemented, notImplemented, notImplemented, self,           notImplemented, notImplemented}, // Reserved
  {notImplemented, notImplemented, notImplemented, notImplemented, self,           notImplemented}, // Reserved
  {setOSWindows,   setOSUBUNTU,    setOSCENTOS,    notImplemented, notImplemented, self          }, // OS Switch
};

/*
 * Array of functions section end
 *******************************************/

/*******************************************
 * Standard arduino setup and loop functions
 */
 
void setup() {
  // Configure the pins for input mode with pullup resistors.
  // The pushbuttons connect from each pin to ground.  When
  // the key is pressed, the pin reads LOW because the key
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
  
}

void loop() {
  // Update the status for each key
  for (int i = 0; i < KEYCOUNT; i = i + 1) {
    keys[i].update();
  }
  
  // Check each key for "falling" edge.
  // Type a message on the Keyboard when each key presses
  // Update the Joystick buttons only upon changes.
  // falling = high (not pressed - voltage from pullup resistor)
  //           to low (pressed - key connects pin to ground)
  for (int key = 0; key < KEYCOUNT; key++) {
    if (keys[key].fallingEdge()) {
      if ((modKey >= 0) && (modKey < KEYCOUNT)) { // If the modKey is valid, run a function.
        macros[modKey][key]();
      }
      else { //If modKey is not valid, set it.
        modKey = key;
      }
    }
  }
  
  // Check each key for "rising" edge
  // Type a message on the Keyboard when each key releases.
  // For many types of projects, you only care when the key
  // is pressed and the release isn't needed.
  // rising = low (pressed - key connects pin to ground)
  //          to high (not pressed - voltage from pullup resistor)
  for (int key = 0; key < KEYCOUNT; key++) {
    if (keys[key].risingEdge()) { // If a key comes up, put modKey into an invalid state so it will need to be made valid again before a function can be run.
      modKey = -1;
    }
  }
}

/*
 * Setup and loop section end
 *******************************************/

