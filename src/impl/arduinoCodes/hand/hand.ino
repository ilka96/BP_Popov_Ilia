/**
 * @file hand.ino
 * @author popovili@fit.cvut.cz
 * @brief Code for Arduino in robotic hand
 */

#include <SPI.h>    //  Library for SPI 

/**
* @brief Struct represents fingers of robotic hand
*/
struct SFinger {   
  
  int pinDir;       // Pin for servos
  int pinPwm;
  int fingerPos;      // Last readed servo position
  int fingerPrevPos;  // Previous  servo position
  int fingerID;       // Finger id
  int targetPos;      // Target position for servo
  };


/**
* @brief Servo positions
*/
struct SPositions {  
  
  uint8_t fing[6];
};


/**
* @brief Hand pins initialization
*/
void handInit() { 
  
  pinMode ( 2, OUTPUT );
  pinMode ( 3, OUTPUT );  
  pinMode ( 7, OUTPUT );
  pinMode ( 5, OUTPUT );
  pinMode ( 8, OUTPUT );
  pinMode ( 6, OUTPUT );
  pinMode ( 12, OUTPUT );
  pinMode ( 9, OUTPUT );
  pinMode ( 13, OUTPUT );
  pinMode ( 10, OUTPUT );
  pinMode ( 1, OUTPUT );
  pinMode ( 11, OUTPUT );

  pinMode ( 4, OUTPUT );
  digitalWrite( 4,HIGH );
  }

/**
* @brief  Close finger
*/
void closeFinger(SFinger fing){  
  
  digitalWrite( fing.pinDir,HIGH );
  analogWrite( fing.pinPwm,190 );
}

/**
* @brief  Open finger
*/
void openFinger(SFinger fing){     
  
  digitalWrite( fing.pinDir,LOW );
  analogWrite( fing.pinPwm,190 );
}

/**
* @brief  Stop servo
*/
void stopFingMotor (SFinger fing) { 
  
  digitalWrite( fing.pinDir,HIGH );
  analogWrite( fing.pinPwm,0 );
}  

/**
* @brief  Initialization for all fingers
*/
void initSFingers (SFinger* fingers) {  
  
  fingers[0].pinDir = 2;
  fingers[0].pinPwm = 3;
  fingers[1].pinDir = 7;
  fingers[1].pinPwm = 5;
  fingers[2].pinDir = 8;
  fingers[2].pinPwm = 6;
  fingers[3].pinDir = 12;
  fingers[3].pinPwm = 9;
  fingers[4].pinDir = 13;
  fingers[4].pinPwm = 10;
  fingers[5].pinDir = 1;
  fingers[5].pinPwm = 11;

  for (int i = 0; i < 6; i++) {
    fingers[i].fingerPos = -10;
    fingers[i].fingerPrevPos = -30;    
    fingers[i].fingerID = i;
    fingers[i].targetPos = -1;
  }
}

/**
* @brief  Read data from external desk
*/
byte transferAndWait (const byte what) {  
  
  byte a = SPI.transfer (what);
  delayMicroseconds (40);
  return a;
} 

/**
* @brief  Read actual positions of all servos
*/
void SPIgetServoPositions(SPositions * pos) {  
  
  transferAndWait(1);
  pos->fing[0] = transferAndWait(2);
  pos->fing[1] = transferAndWait(3);
  pos->fing[2] = transferAndWait(4);
  pos->fing[3] = transferAndWait(5);
  pos->fing[4] = transferAndWait(6);
  pos->fing[5] = transferAndWait(6);  
  }


/**
* @brief  Move finger to target position
* @return bool value, true if servo is not moving or in target position and stops servo
* else move serbo and return false
*/
bool fingerToPosition (SFinger finger) { 
                                                                                           
  int diff = finger.fingerPos - finger.targetPos;
  int movingChange = finger.fingerPos - finger.fingerPrevPos;

   if (finger.targetPos < 0) {
      stopFingMotor (finger);
      return true;
   }

   if (finger.targetPos <= 20)
      diff = finger.fingerPos - 20;

   if (movingChange == 0) {
      stopFingMotor (finger);
      return true;
   }
       
 
  if ((diff <= 10) && (diff >= -10)) {
    stopFingMotor (finger);
    return true;
  }

  if (diff < 0)
    openFinger(finger);
  else 
    closeFinger(finger);

  return false;
}

/**
* @brief  Actualizes servos positions
*/
void reNewPosition (SFinger fingers[6], SPositions pos) {  
  
  fingers[0].fingerPrevPos = fingers[0].fingerPos;
  fingers[1].fingerPrevPos = fingers[1].fingerPos;
  fingers[2].fingerPrevPos = fingers[2].fingerPos;
  fingers[3].fingerPrevPos = fingers[3].fingerPos;
  fingers[4].fingerPrevPos = fingers[4].fingerPos;
  fingers[5].fingerPrevPos = fingers[5].fingerPos;
  
  fingers[0].fingerPos = pos.fing[0];
  fingers[1].fingerPos = pos.fing[1];
  fingers[2].fingerPos = pos.fing[2];
  fingers[3].fingerPos = pos.fing[3];
  fingers[4].fingerPos = pos.fing[4];
  fingers[5].fingerPos = pos.fing[5];

}

/**
* @brief  read data from serial line
* @return bool value, true if command from serial in correct format
*/
bool reciveDataFromPC (SFinger fingers[6]) {  
                                              
  String inData,tmp;
  String strS = "S"; 
  String strE = "E"; 
  int tmpIndex,tmpIndex2; 
  int tmpFings[6];
  if (Serial.available()) {
    inData = Serial.readString();   
    tmpIndex = inData.indexOf(':'); 
    tmp = inData.substring(0, tmpIndex);    // S
    if (!tmp.equals(strS))
      return false;
  
    tmpIndex2 = inData.indexOf(',');
    tmp = inData.substring(tmpIndex+1, tmpIndex2);  // Fing1
    tmpFings[0] = tmp.toInt();
    tmpIndex = inData.indexOf(',', tmpIndex2+1);
    tmp = inData.substring(tmpIndex2+1, tmpIndex); // Fing2
    tmpIndex2 = inData.indexOf(',', tmpIndex+1);
    tmpFings[1] = tmp.toInt();
    tmp = inData.substring(tmpIndex+1, tmpIndex2); // Fing3    
    tmpIndex = inData.indexOf(',', tmpIndex2+1);
    tmpFings[2] = tmp.toInt();
    tmp = inData.substring(tmpIndex2+1, tmpIndex); // Fing4             
    tmpIndex2 = inData.indexOf(',', tmpIndex+1);
    tmpFings[3] = tmp.toInt();
    tmp = inData.substring(tmpIndex+1, tmpIndex2); // Fing5    
    tmpIndex = inData.indexOf(':', tmpIndex2+1);
    tmpFings[4] = tmp.toInt();
    tmp = inData.substring(tmpIndex2+1, tmpIndex); // Fing6     
    tmpFings[5] = tmp.toInt();  
    tmp = inData.substring(tmpIndex+1,tmpIndex+2); // E   
    if (!tmp.equals(strE)) 
       return false;
    
  
  for (int i=0; i<6; i++)
    fingers[i].targetPos = tmpFings[i];
 

  return true;
  }
  
return false;       
}

/**
* @brief  Stops all servos
*/
void stopAllServo (SFinger* fingers) {      
  
        for (int i = 0; i <6; i++)
        stopFingMotor( fingers[i]);
}

/**
* @brief Send current positions of servos via serial line
*/
void sendCurrentState (SFinger* fingers) { 
  String currentPosition;
  
  currentPosition = "S:";
      
  for (int i = 0; i <6; i++) {
    
    stopFingMotor( fingers[i]);   
    currentPosition+= String(fingers[i].fingerPos);
    
    if (i != 5)
      currentPosition+= ",";          
    }

  currentPosition+= ":E";
          
  while(Serial.read() >= 0);
  
  Serial.println(currentPosition);
  
}

/**
* @brief Reads and actualizes all servos positions
*/
void updateFingPositions (SPositions* pos, SFinger* fingers) {  
  
        SPIgetServoPositions (pos);
        reNewPosition(fingers, *pos); 
}

/**
* @brief Checks if all servos are in target positions or not moving
*/
bool allFingersToTargetPosition (SFinger* fingers) {  
                                                     
  bool finalPos = true;
  bool fing_position_status[5]; 
  
    for (int i=0;i<6;i++)
      fing_position_status[i] = fingerToPosition(fingers[i]);
          
    for (int i=0;i<6;i++)
      finalPos = finalPos && fing_position_status[i];

  return finalPos;
}

void setup() {  
  
  Serial.begin(9600);                         //Starts Serial Communication at Baud Rate 9600 
  SPI.begin();                                //Begins the SPI commnuication
  SPI.setClockDivider(SPI_CLOCK_DIV8);        //Sets clock for SPI communication at 8 (16/8=2Mhz)
  digitalWrite(SS,HIGH);                      // Setting SlaveSelect as HIGH (So master doesnt connnect with slave)
  handInit();                                 
}

void loop() {
  
  SFinger fingers[6];
  SPositions pos;  
  int n = 6;
  bool allFingersInPosition = false;
  
  initSFingers ( fingers);   

  while (true) {
    
    if (reciveDataFromPC (fingers)) {         

      stopAllServo(fingers);
      
      updateFingPositions (&pos, fingers);

      allFingersInPosition = allFingersToTargetPosition (fingers);

      while (!allFingersInPosition) {
        
        updateFingPositions (&pos, fingers);
        
        allFingersInPosition = allFingersToTargetPosition (fingers);
          
        sendCurrentState (fingers);

      }
    } 
  }
}
  
