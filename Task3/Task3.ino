#define keypadColumnSize 2
#define MotorAplus 6
#define MotorBplus 5
#define MotorAminus 4
#define MotorBminus 1
#define switchTrigger 2
#define MotorStepsPerRevolution 32 
//keypad varibles
bool iskeyread = 0;
int keypad[] = {0,1,2,3,4,5,6,7};
//Motor variables
int currentStep = 0;
int revolutionSteps = MotorStepsPerRevolution;
//Elevator variables
int currentFloor = 0;
int targetFloor = 0;
  //Representing the current elevator status
bool isGoingup = 1;  
bool isIdle = 1;
  //These arrays represent the status of the floors called or not while going up or going down
  //Initially all the floors are not called
bool goinguptargets[] = {0,0,0,0,0,0,0,0};
bool goingdowntargets[] = {0,0,0,0,0,0,0,0};
bool isDestinationReached = 0;
//variables for delays
bool isWaitEnterExitSet = 0;
unsigned long waitEnterExitStart = 0;
unsigned long waitEnterExitcurrent = 0; 
unsigned long CurrentTime = 0;
//variables for switches
unsigned long switchWaitStart = 0;
int pushedSwitch = 0;
bool isSwitchwaitset = 0;
bool switchwaitSet = 0;
bool switchwaitCheck = 0;

void setup() {
//keypad pins and switches
  for(int i = 14; i <= 19; i++) 
  (i< 18)? pinMode(i, OUTPUT): pinMode(i, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchTrigger), pushSwitch, CHANGE);
//Motor pins
  for(int i = MotorAminus; i <= MotorAplus; i++) pinMode(i, OUTPUT);
  pinMode(MotorBminus, OUTPUT);  
//BCD seven segment pins
  Serial.begin(9600);
  for(int i = 10; i <= 12; i++) pinMode(i, OUTPUT);
  }

void loop() {
  //always store the current milliseconds at the beggining of each loop cycle
  CurrentTime = millis();
  
  if(!isSwitchwaitset && switchwaitSet){
    //take a snap of the current time
    switchWaitStart = CurrentTime;
    isSwitchwaitset = 1;
  }
  if(switchwaitCheck){
    //if the time passed 200 ms set the new target
    if(CurrentTime - switchWaitStart > 200) settargetFloor(pushedSwitch);
    //resest the flags
    isSwitchwaitset = 0;
    switchwaitSet = 0;
    switchwaitCheck = 0;
  }
  
  //testing the keypad
  int keyPressed = scanKeypad();
  if(keyPressed != -1){
    //set the new target floor
    Serial.println(keyPressed);
    settargetFloor(keyPressed);
  }
  
  //printing on sevensegments
  Display(currentFloor);

  //Motor movement
  //The elevator hasn't reached its destination
  if(currentFloor != targetFloor){
    //If the elevator reached its destination it should wait for one second
    if((waitEnterExitcurrent - waitEnterExitStart < 1000) && isDestinationReached){
      waitEnterExitcurrent = millis();
    }
    //The elevator is moving
    else{
      isDestinationReached = 0;
      isWaitEnterExitSet = 0;
      
      if(revolutionSteps != 0){
        if(isGoingup == 1){
          currentStep++;
        }
        else{
          (currentStep >= 1)? currentStep--: currentStep = 3;
        }
        isIdle = 0;
        moveMotor(MotorAplus, MotorAminus, MotorBplus, MotorBminus, currentStep, 20000);
        revolutionSteps--;           
      }
      else{
        //compeleted one revolution either going up or down
        (isGoingup == 1)? currentFloor++ : currentFloor--;
        //reset counting the steps for a new revolution
        revolutionSteps = MotorStepsPerRevolution;
      }
    }
  }
  
  //The elevator has reached its destination
  else{
    if(!isWaitEnterExitSet){
      //Start waiting for one second
      waitEnterExitStart = millis();
      waitEnterExitcurrent = millis();
      isDestinationReached = 1;
      isWaitEnterExitSet = 1;
      isIdle = 1;
      //Reset the currentfloor state to not called
      (isGoingup == 1)? goinguptargets[currentFloor] = 0: goingdowntargets[currentFloor] = 0; 
    }
        
    //Check the new target of the elevator
    if(isGoingup){
      //Check if there are more floors called above
      for(int i = currentFloor + 1; i <= 7; i++){
        if(goinguptargets[i] == 1){
          targetFloor = i;
          isIdle = 0;
          break; 
        }
      }
      //Didn't find an upper target so search for a lower target  
      if(currentFloor == targetFloor){
        for(int i = currentFloor - 1; i >= 0; i--){
          if(goingdowntargets[i] == 1){
            targetFloor = i;
            isIdle = 0;
            //set the status to going down
            isGoingup = 0;
            break; 
          }   
        }
      }
   }
  //The elevator is going down
  else{
    //check if there are more floors called below the currentfloor
    for(int i = currentFloor - 1; i >= 0; i--){
      if(goingdowntargets[i] == 1){
        targetFloor = i;
        isIdle = 0;
        break; 
      }   
    }
    //if there are no targets below see if there is a called target above
    if(currentFloor == targetFloor){
      for(int i = currentFloor + 1; i <= 7; i++){
        if(goinguptargets[i] == 1){
          targetFloor = i;
          isIdle = 0;
          //set the status to going up
          isGoingup = 1;
          break;
        }
      }      
    }

  }
  }
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
// Dealing with switches isr
//-----------------------------------------------------------------------------------------------------------------------------------------
void pushSwitch(){
  //on the first change set bool to take a snap of the current time and set the currently pushed switch
  if(!switchwaitSet){
    switchwaitSet = 1;
    pushedSwitch = 1 * int(digitalRead(9)) + 2 * int(digitalRead(8)) + 4 * int(digitalRead(7));
  }
  //on the second change set a bool to calculate the difference
  else{
    switchwaitCheck = 1;
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
//-----------------------------------------------------------------------------------------------------------------------------------------
//Displaying on sevenSegment
//-----------------------------------------------------------------------------------------------------------------------------------------
void Display(int currentFloor){
  //BCD 10 11 12   encoder 7 8 9 
   digitalWrite(12, currentFloor & 1);
   digitalWrite(11, currentFloor & 2);
   digitalWrite(10, currentFloor & 4);  
}
//-----------------------------------------------------------------------------------------------------------------------------------------
//Setting the targetfloor according to the switches
//-----------------------------------------------------------------------------------------------------------------------------------------
void settargetFloor(int target){
  if(isIdle){
    if(target < currentFloor){
      goingdowntargets[target] = 1;
      targetFloor = target;
      isGoingup = 0;
    }
    else if (target > currentFloor){
      goinguptargets[target] = 1;
      targetFloor = target;
      isGoingup = 1;
    }
  }
  else if(isGoingup){
    //if the elevator didn't pass the targetfloor yet set its status to called to be set as the new target
    if(target > currentFloor && target < targetFloor){
      goinguptargets[target] = 1;
      //set the new target to stop at
      targetFloor = target;
      isGoingup = 1;
    }
    else if(target > currentFloor && target > targetFloor){
      goinguptargets[target] = 1;
      isGoingup = 1;
    }
    else if(target <= currentFloor){
      goingdowntargets[target] = 1;
    }
  }
  //if the elevator is going down
  else{
    if(target < currentFloor && target > targetFloor){
      //set the new target to stop at
      goingdowntargets[target] = 1;
      targetFloor = target;
      isGoingup = 0;
    }
    else if (target < currentFloor && target < targetFloor){
      goingdowntargets[target] = 1;
      isGoingup = 0;
    }
    else if(target >= currentFloor){
      goinguptargets[target] = 1;
    }
  }
}
