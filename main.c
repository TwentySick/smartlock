/*

------------- Warning -------------

I fixed the position of registry of EEPROM.
If read code, it should be literally reading. Don't touch.
Bug will appear if change code orders. 
This is bug around logic,it's won't be code
(if you how to code)

Default user password: 123456
Default admin password: 111111

Access to admin mode use: *101#

-----------------------------------

*/

#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define ELEMENTS(x) (sizeof(x) /  sizeof(x[0]))

// Define LED RGB
#define redPin A5
#define bluePin A4
#define greenPin A3

#define resetButton 2

int dataSend;

// Define LCD
LiquidCrystal liquid_crystal_display(A0,A1,10,11,12,13);

// Define password
char passwordAdmin[6];
char password[6]; 
char listPassword[3][6];

int pass1State, pass2State, pass3State;
int passwordState;

// Definde keypad
const byte rows = 4; 
const byte columns = 4; 

char hexaKeys[rows][columns] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte row_pins[rows] = {9,8,7,6};
byte column_pins[columns] = {5,4,3,A2};   

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup(){
  
    // Setup LED RGB
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

  	Serial.begin(9600);
  
    // Display things
    liquid_crystal_display.begin(16,2);
  	pass1State = 0;
  	pass2State = 0;
  	pass3State = 0;
  	preparePassword();
  	// Setup reset button
  	pinMode(resetButton, INPUT_PULLUP);
 	attachInterrupt(digitalPinToInterrupt(resetButton), resetAll, RISING);
  
  	
}

void loop(){
  	
  int count = 0;
  int length = 0;
  char key;
  
  blueLed();
  
  // Reset to default if hold button in 3 seconds
  
  while(true){
    key = keypad_key.getKey();

    // Get input
    if (key){
      password[length++] = key;
      liquid_crystal_display.print("*");
    }

    // Access to admin Mode
    if(strncmp(password,"*101#",5) == 0){
      adminModeLogin();
    }

    // Check password
    if(length == 6){

      delay(200);

      // Password Correct
      for (int i = 0; i < ELEMENTS(listPassword); i++) {
        if(!(strncmp(password, listPassword[i],6))){
          passwordCorrect();
        }
      }

      // Password Incorrect
      passwordIncorrect();
      count++;
      
      	// Lock if password wrong 3 times
        if(count == 3){
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Please wait");
          liquid_crystal_display.setCursor(0, 1);
          liquid_crystal_display.print("1 minute");
          Serial.write('5');
          delay(60000);
          Serial.write('6');
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Enter Password");
          liquid_crystal_display.setCursor(0, 1);
          break;
        }
      
      blueLed();
      liquid_crystal_display.clear();
      liquid_crystal_display.print("Enter Password");
      liquid_crystal_display.setCursor(0,1);
      length = 0;
      
    }
  }
}

void redLed(){
  	Serial.write('1');
	digitalWrite(redPin,HIGH);
  	digitalWrite(bluePin,LOW);
  	digitalWrite(greenPin,LOW);
}

void greenLed(){
	Serial.write('2');    
	digitalWrite(redPin, LOW);
  	digitalWrite(bluePin, LOW);
  	digitalWrite(greenPin, HIGH);
}

void yellowLed(){
  	Serial.write('3');
	digitalWrite(redPin, HIGH);
  	digitalWrite(bluePin, LOW);
  	digitalWrite(greenPin, HIGH);
}

void blueLed(){
  	Serial.write('4');
	digitalWrite(redPin, LOW);
  	digitalWrite(bluePin, HIGH);
  	digitalWrite(greenPin, LOW);
}

void passwordCorrect(){
  	greenLed();
    liquid_crystal_display.clear();
    liquid_crystal_display.print("!!! Welcome !!!");
    liquid_crystal_display.setCursor(0,1);
    liquid_crystal_display.print("Opening the door");  
    exit(0);
}

void passwordIncorrect(){
    redLed();
    liquid_crystal_display.clear();
    liquid_crystal_display.print("Wrong Password !");
    liquid_crystal_display.setCursor(0,1);
  	liquid_crystal_display.print("Try Again");
    delay(2000);
}

void resetBoard(){
  asm volatile ( "jmp 0");  
}

void adminModeLogin(){
  
  int j = 0;
  char getPassword[6];
  
  // Display things
  yellowLed();
  liquid_crystal_display.clear();
  liquid_crystal_display.print("Admin mode");
  liquid_crystal_display.setCursor(0,1);
  liquid_crystal_display.print("Password: ");
  
  while(true){
  
    char key=keypad_key.getKey();
    
    // Enter admin password
    if(key){
      getPassword[j++]=key;
      liquid_crystal_display.print('*');
    }
    
    // Admin password correct   
    if(j == 6){
      if(strncmp(getPassword,passwordAdmin,6) == 0){
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Password");
          liquid_crystal_display.setCursor(0,1);
          liquid_crystal_display.print("correct !!!");
          delay(1000);
          adminMode();


      } else { // Admin password incorrect
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Password");
          liquid_crystal_display.setCursor(0,1);
          liquid_crystal_display.print("incorrect !!!");
          delay(1000);
          liquid_crystal_display.clear();
          liquid_crystal_display.print("!!! Exit !!!");
          delay(1000);
          resetBoard();
      }
    }
  }
}

void adminMode(){
  
  char key;
  
  liquid_crystal_display.clear();
  liquid_crystal_display.print("Admin mode");
  liquid_crystal_display.setCursor(0,1);
  liquid_crystal_display.print("Choose: ");
  
  while(true){
            
    // Get choice
    while(true){
      key = keypad_key.getKey();

      if(key){
        liquid_crystal_display.print(key);
        delay(200);
        break;
      }
    }  
            
    switch(key){

      // Add password
      case '1':
      	addPassword();
      	break;

      // Update password
      case '2':
      	updatePassword();
      	break;

      // Delete password
      case '3':
      	deletePassword();
      	break;

      // Update admin password
      case 'A':
      	updateAdminPassword();
      	break;

      // Exit Admin mode
      case 'D':
      	liquid_crystal_display.clear();
      	liquid_crystal_display.print("!!! Exit !!!");
      	delay(1000);
      	resetBoard();

      // No option
      default:
        liquid_crystal_display.clear();
        liquid_crystal_display.print("!!! Fail !!!");
        delay(1000);
        liquid_crystal_display.clear();
        liquid_crystal_display.print("Admin mode");
        liquid_crystal_display.setCursor(0,1);
        liquid_crystal_display.print("Choose: ");
    }
  }
}

void addPassword(){
  passwordState = EEPROM.read(0);
  
  // If having 3 passwords
  if(passwordState == 3){
    liquid_crystal_display.clear();
  	liquid_crystal_display.print("Can't create");
    liquid_crystal_display.setCursor(0,1);
    liquid_crystal_display.print("more password");
    delay(2000);

  } else {
    
    char key;
    char newPassword[6];
    int length = 0;
    
    char first[6];
    char second[6];
    char third[6];
       
    //Get Data
    pass1State = EEPROM.read(1);
    pass2State = EEPROM.read(2);
    pass3State = EEPROM.read(3);
    
    // Just for debug
    for(int j=0;j<6;j++){
      first[j] = EEPROM.read(j+10);
    }
    
    for(int j=0;j<6;j++){
      second[j] = EEPROM.read(j+16);
    }
    
    for(int j=0;j<6;j++){
      third[j] = EEPROM.read(j+22);
    }    
    
    liquid_crystal_display.clear();
  	liquid_crystal_display.print("New password");
    liquid_crystal_display.setCursor(0,1);

    // Get input
    while(true){
      key = keypad_key.getKey();
      
      if(key){
        newPassword[length++]=key;
        liquid_crystal_display.print(key);
      }
      
      if(length == 6){
        
        // Add new password
        if(pass1State == 0){
          EEPROM.write(1,1);
          delay(5);
          for(int j=0;j<6;j++){
            EEPROM.write(j+10, newPassword[j]);
            listPassword[0][j] = newPassword[j];
            delay(5);
          }

        } else if(pass2State == 0){
          EEPROM.write(2,1);
          delay(5);
          for(int j=0;j<6;j++){
            EEPROM.write(j+16, newPassword[j]);
            listPassword[1][j] = newPassword[j];
            delay(5);
          }
        
        } else if(pass3State == 0){
          EEPROM.write(3,1);
          delay(5);
          for(int j=0;j<6;j++){
            EEPROM.write(j+22, newPassword[j]);
            listPassword[2][j] = newPassword[j];
            delay(5);
          }  
        }

            
        // Just for debug
        for(int j=0;j<6;j++){
          first[j] = EEPROM.read(j+10);
        }

        for(int j=0;j<6;j++){
          second[j] = EEPROM.read(j+16);
        }

        for(int j=0;j<6;j++){
          third[j] = EEPROM.read(j+22);
        }
        
        passwordState++;
        EEPROM.write(0, passwordState);
        delay(5);
        
        break;
      }
    }
        
    liquid_crystal_display.clear();
    liquid_crystal_display.print("Saving password");
    liquid_crystal_display.setCursor(0,1);
    delay(1000);
  }
  
  adminMode();
}

void updatePassword(){
  
  char key;
  
  pass1State = EEPROM.read(1);
  pass2State = EEPROM.read(2);
  pass3State = EEPROM.read(3);

  while(true){
    // Get input
    liquid_crystal_display.clear();
    liquid_crystal_display.print("Update Password");
    liquid_crystal_display.setCursor(0, 1);

    while(true){
      key = keypad_key.getKey();

      if(key){
        liquid_crystal_display.print(key);
        delay(200);
        break;
      }
    }  

    // Update
    switch(key){

      case '1':
      	if(pass1State == 1){

          char currentPassword[6];
          char newPassword[6];
          int lengthCurrentPassword = 0;
          int lengthNewPassword = 0;
          int count = 0;
          
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Current Password");
          liquid_crystal_display.setCursor(0, 1);

          while(true){
            key = keypad_key.getKey();
			
            // Get current password
            if(key){
              currentPassword[lengthCurrentPassword++]=key;
              liquid_crystal_display.print("*");
            }

            if(lengthCurrentPassword == 6){
              if(!strncmp(currentPassword, listPassword[0], 6)){

                liquid_crystal_display.clear();
                liquid_crystal_display.print("New Password");
                liquid_crystal_display.setCursor(0, 1);

                while(true){
                  key = keypad_key.getKey();
                  
                  // Get new password
                  if(key){
                    newPassword[lengthNewPassword++]=key;
                    liquid_crystal_display.print(key);
                  }
                  
                  if(lengthNewPassword == 6){
					break;
                  }
                }
                
                // Save password to EEPROM
                for(int j = 0; j < 6; j++){
                  EEPROM.write(j+10, newPassword[j]);
                  delay(5);
                }
                break;

              } else {
                count++;
                lengthCurrentPassword = 0;
                liquid_crystal_display.clear();
                liquid_crystal_display.print("Wrong Password");
                delay(1000);
                liquid_crystal_display.clear();
                liquid_crystal_display.print("Current Password");
                liquid_crystal_display.setCursor(0, 1);
              }
            }
            
            if(count == 3){
              adminMode();
            }
          }

          liquid_crystal_display.clear();
          liquid_crystal_display.print("Changed Password");
          liquid_crystal_display.setCursor(0,1);
          delay(1000);
          adminMode();

        } else {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Update fail");
          delay(2000);
        }
        break;

      case '2':
        if(pass2State == 1){

          char currentPassword[6];
          char newPassword[6];
          int lengthCurrentPassword = 0;
          int lengthNewPassword = 0;
          int count = 0;
          
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Current Password");
          liquid_crystal_display.setCursor(0, 1);

          while(true){
            key = keypad_key.getKey();
			
            // Get current password
            if(key){
              currentPassword[lengthCurrentPassword++]=key;
              liquid_crystal_display.print("*");
            }

            if(lengthCurrentPassword == 6){
              if(!strncmp(currentPassword, listPassword[1], 6)){

                liquid_crystal_display.clear();
                liquid_crystal_display.print("New Password");
                liquid_crystal_display.setCursor(0, 1);

                while(true){
                  key = keypad_key.getKey();
                  
                  // Get new password
                  if(key){
                    newPassword[lengthNewPassword++]=key;
                    liquid_crystal_display.print(key);
                  }
                  
                  if(lengthNewPassword == 6){
					break;
                  }
                }
                
                // Save password to EEPROM
                for(int j = 0; j < 6; j++){
                  EEPROM.write(j+16, newPassword[j]);
                  delay(5);
                }
                break;

              } else {
                count++;
                lengthCurrentPassword = 0;
                liquid_crystal_display.clear();
                liquid_crystal_display.print("Wrong Password");
                delay(1000);
                liquid_crystal_display.clear();
                liquid_crystal_display.print("Current Password");
                liquid_crystal_display.setCursor(0, 1);
              }
            }
            
            if(count == 3){
              adminMode();
            }
          }

          liquid_crystal_display.clear();
          liquid_crystal_display.print("Changed Password");
          liquid_crystal_display.setCursor(0,1);
          delay(1000);
          adminMode();

        } else {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Update fail");
          delay(2000);
        }
        break;

      case '3':
        if(pass3State == 1){

          char currentPassword[6];
          char newPassword[6];
          int lengthCurrentPassword = 0;
          int lengthNewPassword = 0;
          int count = 0;
          
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Current Password");
          liquid_crystal_display.setCursor(0, 1);

          while(true){
            key = keypad_key.getKey();
			
            // Get current password
            if(key){
              currentPassword[lengthCurrentPassword++]=key;
              liquid_crystal_display.print("*");
            }

            if(lengthCurrentPassword == 6){
              if(!strncmp(currentPassword, listPassword[2], 6)){

                liquid_crystal_display.clear();
                liquid_crystal_display.print("New Password");
                liquid_crystal_display.setCursor(0, 1);

                while(true){
                  key = keypad_key.getKey();
                  
                  // Get new password
                  if(key){
                    newPassword[lengthNewPassword++]=key;
                    liquid_crystal_display.print(key);
                  }
                  
                  if(lengthNewPassword == 6){
					break;
                  }
                }
                
                // Save password to EEPROM
                for(int j = 0; j < 6; j++){
                  EEPROM.write(j+22, newPassword[j]);
                  delay(5);
                }
                break;

              } else {
                count++;
                lengthCurrentPassword = 0;
                liquid_crystal_display.clear();
                liquid_crystal_display.print("Wrong Password");
                delay(1000);
                liquid_crystal_display.clear();
                liquid_crystal_display.print("Current Password");
                liquid_crystal_display.setCursor(0, 1);
              }
            }
            
            if(count == 3){
              adminMode();
            }
          }

          liquid_crystal_display.clear();
          liquid_crystal_display.print("Changed Password");
          liquid_crystal_display.setCursor(0,1);
          delay(1000);
          adminMode();


        } else {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Update fail");
          delay(2000);
        }
        break;

      default:
        liquid_crystal_display.clear();
        liquid_crystal_display.print("Password doesn't");
        liquid_crystal_display.setCursor(0,1);
        liquid_crystal_display.print("exist");
        delay(2000);
    }
  }
}

void deletePassword(){
  passwordState = EEPROM.read(0);
  
  if(passwordState == 1){
  	liquid_crystal_display.clear();
    liquid_crystal_display.print("Can't delete");
    liquid_crystal_display.setCursor(0,1);
    liquid_crystal_display.print("password");
    delay(2000);
    adminMode();
  } else {
    
    char key;
    
    pass1State = EEPROM.read(1);
    pass2State = EEPROM.read(2);
    pass3State = EEPROM.read(3);
    
    while(true){
      // Get input
      liquid_crystal_display.clear();
      liquid_crystal_display.print("Delete Password");
      liquid_crystal_display.setCursor(0, 1);

      while(true){
        key = keypad_key.getKey();

        if(key){
          liquid_crystal_display.print(key);
          delay(200);
          break;
        }
      }  

      // Delete
      switch(key){

        case '1':
        if(pass1State == 1){
          
		  passwordState--;
          EEPROM.write(0, passwordState);
          pass1State = 0;
          EEPROM.write(1,0);

          for(int j = 0; j < 6; j++){
            EEPROM.write(j+10, 0);
            delay(5);
          }

          liquid_crystal_display.clear();
          liquid_crystal_display.print("Deleted password");
          liquid_crystal_display.setCursor(0,1);
          delay(1000);
          adminMode();

        } else {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Delete fail");
          delay(2000);
        }
        break;

        case '2':
        if(pass2State == 1){
          
		  passwordState--;
          EEPROM.write(0, passwordState);
          pass2State = 0;
          EEPROM.write(2,0);

          for(int j = 0; j < 6; j++){
            EEPROM.write(j+16, 0);
            delay(5);
          }

          liquid_crystal_display.clear();
          liquid_crystal_display.print("Deleted password");
          liquid_crystal_display.setCursor(0,1);
          delay(1000);
          adminMode();

        } else {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Delete fail");
          delay(2000);
        }
        break;

        case '3':
        if(pass3State == 1){
          
		  passwordState--;
          EEPROM.write(0, passwordState);
          pass3State = 0;
          EEPROM.write(3,0);

          for(int j = 0; j < 6; j++){
            EEPROM.write(j+22, 0);
            delay(5);
          }

          liquid_crystal_display.clear();
          liquid_crystal_display.print("Deleted password");
          liquid_crystal_display.setCursor(0,1);
          delay(1000);
          adminMode();

        } else {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Delete fail");
          delay(2000);
        }
        break;

        default:
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Password doesn't");
          liquid_crystal_display.setCursor(0,1);
          liquid_crystal_display.print("exist");
          delay(2000);
      }
    }
  }
}

void updateAdminPassword(){
  
  char currentPassword[6];
  char newPassword[6];
  char key;
  int length = 0;
  int count = 0;
  
  // Get admin password
  for(int j = 0; j < 6; j++){
  	passwordAdmin[j] = EEPROM.read(j + 4);
  }

  liquid_crystal_display.clear();
  liquid_crystal_display.print("Change admin");
  liquid_crystal_display.setCursor(0, 1);
  liquid_crystal_display.print("password");
  delay(2000);
  liquid_crystal_display.clear();
  liquid_crystal_display.print("Current Password");
  liquid_crystal_display.setCursor(0, 1);
  
  while(true){
    key = keypad_key.getKey();

    if(key){
	  currentPassword[length++] = key;
      liquid_crystal_display.print("*");
    }	
	
    if(length == 6){
      if(!strncmp(currentPassword, passwordAdmin, 6)){
        
        length = 0;
        
        liquid_crystal_display.clear();
        liquid_crystal_display.print("New Password");
        liquid_crystal_display.setCursor(0 ,1);
        
        // Get new password
        while(true){
          key = keypad_key.getKey();
          	
          if(key){
          	newPassword[length++] = key;
            liquid_crystal_display.print(key);
          }
          
          if(length == 6){
          	break;
          }
        }
        
        for(int j = 0; j < 6; j++){
          EEPROM.write(j+4, newPassword[j]);
          delay(5);
        }
        
        liquid_crystal_display.clear();
        liquid_crystal_display.print("Changed Password");
        delay(1000);
		break;

      } else {
        length = 0;
        liquid_crystal_display.clear();
        liquid_crystal_display.print("Wrong password");
        delay(2000);
        liquid_crystal_display.clear();
        liquid_crystal_display.print("Current Password");
        liquid_crystal_display.setCursor(0, 1);
        count++;
      }
    }
    
    if(count == 3){
      break;
    }
  }
    
  adminMode();
}

void preparePassword(){
  int position = 0;
  
  // Get password State
  passwordState = EEPROM.read(position++);
  pass1State = EEPROM.read(position++);
  pass2State = EEPROM.read(position++);
  pass3State = EEPROM.read(position++);
  
  // Get admin password
  for(int j = 0; j < 6; j++){
  	passwordAdmin[j] = EEPROM.read(position++);
  }
  
  if(passwordState == 0){
    resetAll();
  } else {
  // User password
    for(int j=0; j <6; j++){
      listPassword[0][j] = EEPROM.read(position++);
    }

    for(int j=0; j <6; j++){
      listPassword[1][j] = EEPROM.read(position++);        
    }
      
    for(int j=0; j <6; j++){
      listPassword[2][j] = EEPROM.read(position++);        
    }

    liquid_crystal_display.print("Team 1");
    liquid_crystal_display.setCursor(0,1);
    liquid_crystal_display.print("Smart Lock");
    delay(1000);
    liquid_crystal_display.clear();
    liquid_crystal_display.print("Enter Password");
    liquid_crystal_display.setCursor(0,1);
  }
}

void resetAll(){

  int position = 0;
  
  // Clear data in EEPROM
  for (int j = 0; j < EEPROM.length(); j++) {
    EEPROM.write(j, 0);
    delay(5);
  }
  
  // Write State of all password
  EEPROM.write(position++, 1);
  delay(5);
  EEPROM.write(position++, 1);
  delay(5);
  EEPROM.write(position++, 0);
  delay(5);
  EEPROM.write(position++, 0);
  delay(5);
  
   
  // Create admin password
  for(int j = 0; j < 6; j++){
    EEPROM.write(position++, '1');
    delay(5);
  }
  
  // Create default first password
  for(int j = 0; j < 6; j++){
    EEPROM.write(position++, j + 49);
    delay(5);
  }
  
  for(int j = 0; j < 6; j++){
  	EEPROM.write(position++, 0);
    delay(5);
  }
  for(int j = 0; j < 6; j++){
  	EEPROM.write(position++, 0);
    delay(5);
  }
  
  resetBoard();
}