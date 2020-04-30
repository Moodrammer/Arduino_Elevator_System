#define keypadColumnSize 2
bool iskeyread = 0;
int keypad[] = {0,1,2,3,4,5,6,7};

void setup() {
//keypad pins
  for(int i = 14; i <= 19; i++) 
  (i< 18)? pinMode(i, OUTPUT): pinMode(i, INPUT_PULLUP);
//BCD seven segment pins
  Serial.begin(9600);
  for(int i = 10; i <= 12; i++) pinMode(i, OUTPUT);
  }

void loop() {
  //testing the keypad
  int keyPressed = scanKeypad();
  if(keyPressed != -1){
    //Do some logic 
    Serial.print(keyPressed);
  }
  //BCD 10 11 12   encoder 7 8 9 
   digitalWrite(12, digitalRead(9));
   digitalWrite(11, digitalRead(8));
   digitalWrite(10, digitalRead(7));
  //Todo: -Setting an interrupt that is activated when the user pushes a button to start counting the number of milliseconds passed 
}
//-----------------------------------------------------------------------------------------------------------------------------------------
// Dealing with Keypad functions
//-----------------------------------------------------------------------------------------------------------------------------------------
//scans the keypad for a pressed key if no pressed key found it returns -1
int scanKeypad(){
    int columnNumber;
    if(!digitalRead(18)) columnNumber = 0;
    else if(!digitalRead(19)) columnNumber = 1;
    else{
      columnNumber = -1;
      if(iskeyread) iskeyread = 0;    
    }
    //loop on the rows and output 1 then check the columns
    // if the key is already read don't read again
    if(columnNumber != -1){
      if(!iskeyread){
          int rowNumber = scanRows(columnNumber);
          //Get the number pressed
          return keypad[rowNumber * keypadColumnSize + columnNumber];      
       }      
    }
    //return a flag that indicates that no new key was pressed
    return -1;
}
//Scans the rows of a certain column the rownumber of the pressed key
int scanRows(int columnNumber){
  for(int i = 14; i <= 17; i++){
    digitalWrite(i, 1);
    if(digitalRead(columnNumber + 18) == 1){
      digitalWrite(i, 0);
      iskeyread = 1;
      return i - 14;
    }
    else digitalWrite(i, 0);
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------
