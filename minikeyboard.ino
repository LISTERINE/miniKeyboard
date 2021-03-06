/* 
 Turn mechanical key tester into a macro mini keyboard
*/

/********************************************
 * Display imports and setup
 */
 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


// The display seem to not want to start up when you aren't programming it
// fire this reboot with a key combo to fix that
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

/*
 * End display imports and init
 *******************************************/

/********************************************
 * Keyboard imports and setup
 */
 
#include <Bounce.h>
#include <stdarg.h>

#define KEYCOUNT 6 // Number of keys we have 
#define PROFILECOUNT 6 // Number of operating systems we can choose from

/*
 * End Keyboard imports and init
 *******************************************/

/********************************************
 * Set up profiles
 */
 
char *profiles[KEYCOUNT] = {
  "WINDOWS_1",
  "UBUNTU_1",
  "CENTOS_1",
  "OSX_1",
  "RESERVED_1",
  "WINDOWS_2"
};

enum ProfileType {
  WINDOWS_1,
  UBUNTU_1,
  CENTOS_1,
  OSX_1,
  RESERVED_1,
  WINDOWS_2
};

/*
 * End profile section
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
  Bounce(0, 10),
  Bounce(1, 10),
  Bounce(6, 10),
  Bounce(3, 10), // burned out pin 3 desoldering...oops...
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
ProfileType currentProfile = OSX_1;

// Display data array. +1 to display profile type
char *displayData[KEYCOUNT+1] = {
  "", // Profile
  "", // Key 1
  "", // Key 2
  "", // Key 3
  "", // Key 4
  "", // Key 5
  "" // Key 6
};

enum displayVal {
  PROFILE,
  LINE_1,
  LINE_2,
  LINE_3,
  LINE_4,
  LINE_5,
  LINE_6
};

/*
 * State vars end
 *******************************************/

/************************************
 * Handy shortcuts get setup here, not for actual full macros
 */

// Shortcut for hitting win+r or command+space
void guiGetProgram(int sleep) {
  Keyboard.set_modifier(MODIFIERKEY_GUI); // Press and hold win/clover key
  if ((currentProfile == WINDOWS_1) || (currentProfile == WINDOWS_2)) {
    Keyboard.set_key1(KEY_R);
  }
  if(currentProfile == OSX_1) {
    Keyboard.set_key1(KEY_SPACE);
  }
  Keyboard.send_now();
  Keyboard.set_key1(0);
  Keyboard.set_modifier(0); // Clear modifiers
  Keyboard.send_now();
  delay(sleep);
}

// Hits ctrl or command (based on profile) and then a key ex. ctrl/command+c
void ctrlCommand(int key) {
  if ((currentProfile == WINDOWS_1) || (currentProfile == WINDOWS_2)) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
  }
  if(currentProfile == OSX_1) {
    Keyboard.set_modifier(MODIFIERKEY_GUI); // Press and hold win/clover key
  }
  Keyboard.set_key1(key);
  Keyboard.send_now();
  Keyboard.set_key1(0);
  Keyboard.set_modifier(0); // Clear modifiers
  Keyboard.send_now();
  delay(50);
}

// Launches an interpreter
void guiPrompt() {
  guiGetProgram(300);
  if ((currentProfile == WINDOWS_1) || (currentProfile == WINDOWS_2)) {
    Keyboard.print("cmd");
  }
  if(currentProfile == OSX_1) {
    Keyboard.print("iterm"); // Press and hold win/clover key
  }
  Keyboard.send_now();
  delay(500);
  Keyboard.press(KEY_ENTER);
  Keyboard.release(KEY_ENTER);
  delay(800);
}

void cdHome() {
  if ((currentProfile == WINDOWS_1) || (currentProfile == WINDOWS_2)) {
    Keyboard.println("cd %USERPROFILE%");
  }
  else {
    Keyboard.println("cd ~");
  }
  Keyboard.send_now();
  delay(50);
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

void gotoGitRoot() {
  Keyboard.println("cd $(git rev-parse --show-toplevel)");
  Keyboard.send_now();
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
  guiGetProgram(1000);
  Keyboard.println("\"C:\\Program Files (x86)\\PuTTY\\putty.exe\" -load \"DevEnv\"");
  Keyboard.send_now();
  delay(1500);
  winMaximize(true);
  setProfile_1(); // enter ubuntu profile
  delay(1000);
  Keyboard.println("tmux");
  Keyboard.send_now();
}

void sshVDB() {
  sshWorkEnv();
  delay(700);
  Keyboard.println("vulndb");
  Keyboard.send_now();
}

// VM/Docker section
void dockerRun() {
  gotoGitRoot();
  delay(500);
  Keyboard.println("make dockerrun");
  Keyboard.send_now();
}

void dockerExec() {
  gotoGitRoot();
  delay(500);
  Keyboard.println("make dockerexec");
  Keyboard.send_now();
}

void dockerBuild() {
  gotoGitRoot();
  delay(500);
  Keyboard.println("make dockerbuild");
  Keyboard.send_now();
}

void dockerUbuntu() {
  Keyboard.println("docker run --rm -it ubuntu:latest /bin/bash");
  Keyboard.send_now();
}

void dockerCentos() {
  Keyboard.println("docker run --rm -it centos:latest /bin/bash");
  Keyboard.send_now();
}


void rainbowDocker() {
  Keyboard.println("docker run --rm -it ubuntu:latest /bin/bash | lolcat");
  Keyboard.send_now();
}

void vagrantUp() {
  Keyboard.println("vagrant up");
  Keyboard.send_now();
}

void vagrantSsh() {
  Keyboard.println("vagrant ssh");
  Keyboard.send_now();
}

// Programming functions
void quickStack() {
  guiPrompt();
  cdHome();
  Keyboard.println("cookiecutter git@gitlab.com:LISTERINE/quickstack.git");
  Keyboard.send_now();
  delay(500);
  Keyboard.println("yes");
  Keyboard.send_now();
}

void makeTest() {
  Keyboard.println("make test");
  Keyboard.send_now();
}

void makeUnitTest() {
  Keyboard.println("make -C $(git rev-parse --show-toplevel) unittest");
  Keyboard.send_now();
}

// Shortcuts
void screenshot() {
  if(currentProfile == OSX_1) {
    Keyboard.set_modifier(MODIFIERKEY_SHIFT);
    Keyboard.send_now();
    Keyboard.set_modifier(MODIFIERKEY_SHIFT | MODIFIERKEY_GUI);
    Keyboard.send_now();
    Keyboard.print(KEY_4);
    Keyboard.send_now();
    
  }
  else {
    Keyboard.set_key1(KEY_PRINTSCREEN);
    Keyboard.send_now();
  }
  delay(100);
  Keyboard.set_key1(0);  // Clear key1
  Keyboard.set_modifier(0); // Clear modifiers
  Keyboard.send_now(); // Push cleared key state.
}

void shrug() {
  Keyboard.print("¯\_(ツ)_/¯");
  Keyboard.send_now();
}

void jog() {
  Keyboard.print("ᕕ( ᐛ )ᕗ");
  Keyboard.send_now();
}

// Restart the teensy
void reboot() {
  CPU_RESTART
}

/*
 * Macro section end
 *******************************************/

 /*******************************************
 * Here we fill up our array of functions and set their corresponding string representations
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
 *  Make sure when you update a function you update the corresponding string to keep track of it
 *  
 */

// String representations of functions for display
// * keys are one button press keys
char *macroTitles[PROFILECOUNT][KEYCOUNT+1][KEYCOUNT] = { // +1 for row types

  // Windows functions
  {
    {"Remote",     "VM/Docker", "Programming", "Meta Keys", "General",    "Profile Switch"    }, // Titles
    {"*Dev Env",   "SSH VDB",   "",            "",         "",            ""                  }, // Remote
    {"",           "*",         "",            "",         "",            ""                  }, // VM/Docker
    {"",           "",          "*",           "",         "",            ""                  }, // Programming
    {"",           "",          "",            "*REBOOT",  "",            ""                  }, // Meta keys
    {"shrug",      "jog",       "",            "",         "*screenshot", ""                  }, // Reserved
    {"Windows 1", "Ubuntu",    "Centos",       "OSX",      "Reserved 1",  "*Windows 2"        }  // Profile Switch
  },
  // Ubuntu functions
  {
    {"Remote",         "VM/Docker","Programming", "Meta Keys", "General",    "Profile Switch"    }, // Titles
    {"*",              "",         "",            "",          "",            ""                 }, // Remote
    {"D.exec",         "*D.run",   "V.up",        "V.ssh",     "",            "D.build"          }, // VM/Docker
    {"",               "",         "*",           "",          "",            ""                 }, // Programming
    {"",               "",         "",            "*REBOOT",   "",            ""                 }, // Meta keys
    {"shrug",          "jog",      "",            "",          "*screenshot", ""                 }, // Reserved
    {"Windows 1",      "Ubuntu",   "Centos",      "OSX",       "Reserved 1",  "*Windows 2"       }  // Profile Switch
  },
  // Centos functions
  {
    {"Remote",     "VM/Docker", "Programming", "Meta Keys", "General",    "Profile Switch"    }, // Titles
    {"*",           "",         "",            "",         "",            ""                  }, // Remote
    {"D.exec",      "*D.run",   "V.up",        "V.ssh",     "",           "D.build"         }, // VM/Docker
    {"",            "",         "*",           "",         "",            ""                  }, // Programming
    {"",            "",         "",            "*REBOOT",  "",            ""                  }, // Meta keys
    {"shrug",       "jog",      "",            "",         "*screenshot", ""                  }, // Reserved
    {"Windows 1",   "Ubuntu",   "Centos",      "OSX",      "Reserved 1",  "*Windows 2"       }  // Profile Switch
  },
  // OSX functions
  {
    {"Remote",        "VM/Docker","Programming", "Meta Keys", "General",  "Profile Switch"    }, // Titles
    {"*",             "",         "",            "",         "",            ""                  }, // Remote
    {"D.exec",        "*D.run",   "V.up",        "V.ssh",     "",           "D.build"         }, // VM/Docker
    {"goto git root", "unit test","*TERM",       "Quickstack","",           ""                  }, // Programming
    {"",              "",         "",            "*REBOOT",  "",            ""                  }, // Meta keys
    {"shrug",         "jog",      "",            "",         "*screenshot", ""                  }, // Reserved
    {"Windows 1",     "Ubuntu",   "Centos",      "OSX",      "Reserved 1",  "*Windows 2"       }  // Profile Switch
  },
  // RESERVED_1 functions
  {
    {"Remote",     "VM/Docker", "Programming", "Meta Keys", "General",    "Profile Switch"    }, // Titles
    {"*",          "",          "",            "",         "",            ""                  }, // Remote
    {"",           "*",         "",            "",         "",            ""                  }, // VM/Docker
    {"",           "",          "*",           "",         "",            ""                  }, // Programming
    {"",           "",          "",            "*REBOOT",  "",            ""                  }, // Meta keys
    {"shrug",      "jog",       "",            "",         "*screenshot", ""                  }, // Reserved
    {"Windows 1",  "Ubuntu",    "Centos",      "OSX",      "Reserved 1",  "*Windows 2"       }  // Profile Switch
  },
  // WINDOWS_2 functions
  {
    {"Remote",     "VM/Docker", "Programming", "Meta Keys", "General",    "Profile Switch"    }, // Titles
    {"*",          "",          "",            "",         "",            ""                  }, // Remote
    {"",           "*",         "",            "",         "",            ""                  }, // VM/Docker
    {"",           "",          "*",           "",         "",            ""                  }, // Programming
    {"",           "",          "",            "*REBOOT",  "",            ""                  }, // Meta keys
    {"shrug",      "jog",       "",            "",         "*screenshot", ""                  }, // Reserved
    {"Windows 1",  "Ubuntu",    "Centos",      "OSX",      "Reserved 1",  "*Windows 2"       }  // Profile Switch
  }
};
 
/*******************************************
 * Display update convenience functions
 */


void formatDisplayStr(char *fmt, ...){
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  display.println(buf);
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  formatDisplayStr("Profile: %s", displayData[PROFILE]);
  display.setCursor(0,9);
  formatDisplayStr("1: %s", displayData[LINE_1]);
  display.setCursor(0,18);
  formatDisplayStr("2: %s", displayData[LINE_2]);
  display.setCursor(0,27);
  formatDisplayStr("3: %s", displayData[LINE_3]);
  display.setCursor(0,36);
  formatDisplayStr("4: %s", displayData[LINE_4]);
  display.setCursor(0,45);
  formatDisplayStr("5: %s", displayData[LINE_5]);
  display.setCursor(0,54);
  formatDisplayStr("6: %s", displayData[LINE_6]);
  display.display();
}

void updateDisplayArray() {
  for (int keyString=0; keyString < KEYCOUNT; keyString++) {
    displayData[keyString+1] = macroTitles[currentProfile][modKey+1][keyString];
  }
  updateDisplay();
}

/*
 * Shortcut section end
 *******************************************/
 
void (*macros[PROFILECOUNT][KEYCOUNT][KEYCOUNT])() = {
  // Windows functions
  {
    {sshWorkEnv,     sshVDB,         notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // VM/Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Programming
    {notImplemented, notImplemented, notImplemented, reboot,         notImplemented, notImplemented}, // Meta keys
    {shrug,          jog,            notImplemented, notImplemented, screenshot,     notImplemented}, // General
    {setProfile_0,   setProfile_1,   setProfile_2,   setProfile_3,   setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // Ubuntu functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {dockerExec,     dockerRun,      vagrantUp,      vagrantSsh,     notImplemented, dockerBuild   }, // VM/Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Programming
    {notImplemented, notImplemented, notImplemented, reboot,         notImplemented, notImplemented}, // Meta keys
    {shrug,          jog,            notImplemented, notImplemented, screenshot,     notImplemented}, // General
    {setProfile_0,   setProfile_1,   setProfile_2,   setProfile_3,   setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // Centos functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {dockerExec,     dockerRun,      vagrantUp,      vagrantSsh,     notImplemented, dockerBuild   }, // VM/Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Programming
    {notImplemented, notImplemented, notImplemented, reboot,         notImplemented, notImplemented}, // Meta keys
    {shrug,          jog,            notImplemented, notImplemented, screenshot,     notImplemented}, // General
    {setProfile_0,   setProfile_1,   setProfile_2,   setProfile_3,   setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // OSX functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {dockerExec,     dockerRun,      vagrantUp,      vagrantSsh,     notImplemented, dockerBuild   }, // VM/Docker
    {gotoGitRoot,    makeUnitTest,   guiPrompt,      quickStack,     notImplemented, notImplemented}, // Programming
    {notImplemented, notImplemented, notImplemented, reboot,         notImplemented, notImplemented}, // Meta keys
    {shrug,          jog,            notImplemented, notImplemented, screenshot,     notImplemented}, // General
    {setProfile_0,   setProfile_1,   setProfile_2,   setProfile_3,   setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // RESERVED_1 functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // VM/Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Programming
    {notImplemented, notImplemented, notImplemented, reboot,         notImplemented, notImplemented}, // Meta keys
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // General
    {setProfile_0,   setProfile_1,   setProfile_2,   setProfile_3,   setProfile_4,   setProfile_5  }  // Profile Switch
  },
  // WINDOWS_2 functions
  {
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Remote
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // VM/Docker
    {notImplemented, notImplemented, notImplemented, notImplemented, notImplemented, notImplemented}, // Programming
    {notImplemented, notImplemented, notImplemented, reboot,         notImplemented, notImplemented}, // Meta keys
    {shrug,          jog,            notImplemented, notImplemented, screenshot,     notImplemented}, // General
    {setProfile_0,   setProfile_1,   setProfile_2,   setProfile_3,   setProfile_4,   setProfile_5  }  // Profile Switch
  }
};

/*
 * Array of functions section end
 *******************************************/

/*******************************************
 * OS/profile modifier functions
 */
 
void setProfile_0() {
  currentProfile = WINDOWS_1;
  displayData[PROFILE] = profiles[WINDOWS_1];
  updateDisplayArray();
}

void setProfile_1() {
  currentProfile = UBUNTU_1;
  displayData[PROFILE] = profiles[UBUNTU_1];
  updateDisplayArray();
}

void setProfile_2() {
  currentProfile = CENTOS_1;
  displayData[PROFILE] = profiles[CENTOS_1];
  updateDisplayArray();
}

void setProfile_3() {
  currentProfile = OSX_1;
  displayData[PROFILE] = profiles[OSX_1];
  updateDisplayArray();
}

void setProfile_4() {
  currentProfile = RESERVED_1;
  displayData[PROFILE] = profiles[RESERVED_1];
  updateDisplayArray();
}

void setProfile_5() {
  currentProfile = WINDOWS_2;
  displayData[PROFILE] = profiles[WINDOWS_2];
  updateDisplayArray();
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
 * Standard arduino setup and loop functions
 */
 
void setup() {

  /*##########################################
   # Setup pins for keyboard IO
   #*/
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
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  /*
   # End setup pins for keyboard IO
   ##########################################*/

  /*##########################################
   # Display init
   #*/
  Serial.begin(9600);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  /*
   # End display init
   ##########################################*/

   setProfile_3();
  
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
        updateDisplayArray();
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
        updateDisplayArray();
      }
    }
  }
}

/*
 * Setup and loop section end
 *******************************************/

