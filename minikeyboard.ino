/* 
 Turn mechanical key tester into a macro mini keyboard
*/

#include <Bounce.h>

#define KEYCOUNT 6 // Number of keys we have 
#define PROFILECOUNT 6 // Number of operating systems we can choose from

/********************************************
 * Set up enums
 */
 
// Profile enumeration
enum ProfileType {
  WINDOWS_0,
  UBUNTU_0,
  CENTOS_0,
  OSX_0,
  RESERVED_0,
  WINDOWS_1
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

// Which was the pressed after the modifier, if any
int secondaryKey = -1;

// The initial profile
ProfileType currentProfile = WINDOWS_0;

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

/*
 * Macro section end
 *******************************************/

/*******************************************
 * OS/profile modifier functions
 */
 
void setProfile_0() {
  currentProfile = WINDOWS_0;
}

void setProfile_1() {
  currentProfile = UBUNTU_0;
}

void setProfile_2() {
  currentProfile = CENTOS_0;
}

void setProfile_3() {
  currentProfile = OSX_0;
}

void setProfile_4() {
  currentProfile = RESERVED_0;
}

void setProfile_5() {
  currentProfile = WINDOWS_1;
}

/*
 * Profile switch functions section end
 *******************************************/

/*******************************************
 * Filler functions
 */
 
// This function is a place holder for keys that have no function yet
void notImplemented() {}

// This function is used as a place holder for a mod key in it's own row. 
// If you're pressing key 0 to mod, you can't also press it for a function, because you can't press a key that's aleady pressed.
void self() {}

/*
 * Filler function section end
 *******************************************/

/*******************************************
 * Here we fill up our array of functions
 * index [0][1] means key 0 is the modkey and then they hit key 1
 * function key ----------->
 * modifier key {{ self, f01,  f02 },
 *            |  { f10,  self, f12 },
 *            |  { f20,  f21,  self}}
 *            V
 *  The array is grouped into sections, based on the currently selected OS/profile
 *  
 *  functions at n,n (example [0][0]) require only one key press and take no modifier other than the current profile
 *  
 */

void (*macros[PROFILECOUNT][KEYCOUNT][KEYCOUNT])() = {
  // Windows functions
  {
    {sshWorkEnv,     notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Docker
    {sshVDB,         notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Corp macros
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {setProfile_0,   setProfile_1,    setProfile_2,    setProfile_3, setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // Ubuntu functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {dockerUbuntu,   notImplemented, dockerCentos,   notImplemented, notImplemented, notImplemented}, // Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {setProfile_0,   setProfile_1,    setProfile_2,    setProfile_3, setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // Centos functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {dockerUbuntu,   notImplemented, dockerCentos,   notImplemented, notImplemented, notImplemented}, // Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {setProfile_0,   setProfile_1,    setProfile_2,    setProfile_3, setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // OSX functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {dockerUbuntu,   notImplemented, dockerCentos,   notImplemented, notImplemented, notImplemented}, // Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {setProfile_0,   setProfile_1,    setProfile_2,    setProfile_3, setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // RESERVED_0 functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {setProfile_0,   setProfile_1,    setProfile_2,    setProfile_3, setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // WINDOWS_2 functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Reserved
    {setProfile_0,   setProfile_1,    setProfile_2,    setProfile_3, setProfile_4,   setProfile_5  }  // Profile Switch
  }
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
  //
  for (int key = 0; key < KEYCOUNT; key++) {
    // Loop over all our buttons to check their status
    if (keys[key].fallingEdge()) {
      // If we find a key that has been pressed...
      if ((modKey >= 0) && (modKey < KEYCOUNT)) {
        // Check if the modKey is already set to a valid option, if it is that means that
        // this is a secondary key press so record as secondary and execute the associated function.
        secondaryKey = key;
        macros[currentProfile][modKey][key]();
      }
      else { 
        // If modKey is not valid, set it, and clear any existing secondary key.
        // We clear the secondary key here to avoid complications if you are only hitting a single key (using no modifier)
        modKey = key;
        secondaryKey = -1;
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
    // Loop over all our buttons to check their status
    if (keys[key].risingEdge()) { 
      // If a key comes up, that means that the modKey was either not used to modify or it already did it's job; put modKey into an invalid state. 
      if (key == modKey) {
        // If the rising key is the modKey, reset modKey to an invalid state to disable it
        if (secondaryKey < 0) { 
          // If no secondary key was hit, that means we want a one button shortcut; execute the function at modKey,modKey
          macros[currentProfile][modKey][modKey]();
        }
        modKey = -1;
      }
    }
  }
}

/*
 * Setup and loop section end
 *******************************************/

