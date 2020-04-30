#define keypadColumnSize 2
#define MotorAplus 6
#define MotorBplus 5
#define MotorAminus 4
#define MotorBminus 1
#define MotorStepsPerRevolution 32 
//keypad varibles
bool iskeyread = 0;
int keypad[] = {0,1,2,3,4,5,6,7};
//Motor variables
int currentStep = 0;
//Elevator variables
int currentFloor = 1;

void setup() {
//keypad pins
  for(int i = 14; i <= 19; i++) 
  (i< 18)? pinMode(i, OUTPUT): pinMode(i, INPUT_PULLUP);
//Motor pins
  for(int i = MotorAminus; i <= MotorAplus; i++) pinMode(i, OUTPUT);
  pinMode(MotorBminus, OUTPUT);  
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
   digitalWrite(12, currentFloor & 1);
   digitalWrite(11, currentFloor & 2);
   digitalWrite(10, currentFloor & 4);
  //Motor testing
  currentStep++;
  moveMotor(MotorAplus, MotorAminus, MotorBplus, MotorBminus, currentStep, 20000); 
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
// Stepper Motor function
//-----------------------------------------------------------------------------------------------------------------------------------------
//Move the stepper motor
void moveMotor(int Ap, int Am, int Bp, int Bm, int currentStep, int stepWait){
  currentStep %= 4;
  unsigned long waitStart = micros();
  while(micros() - waitStart < stepWait) {} 
  switch(currentStep){
    case 0:
    digitalWrite(Ap, 1);
    digitalWrite(Bp, 1);
    digitalWrite(Am, 0);
    digitalWrite(Bm, 0);
    break;
  
    case 1:
    digitalWrite(Ap, 1);
    digitalWrite(Bp, 0);
    digitalWrite(Am, 0);
    digitalWrite(Bm, 1);
    break;
  
    case 2:
    digitalWrite(Ap, 0);
    digitalWrite(Bp, 0);
    digitalWrite(Am, 1);
    digitalWrite(Bm, 1);
    break;
  
    case 3:
    digitalWrite(Ap, 0);
    digitalWrite(Bp, 1);
    digitalWrite(Am, 1);
    digitalWrite(Bm, 0);
   }
}
