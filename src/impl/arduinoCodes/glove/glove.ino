/**
 * @file glove.ino
 * @author popovili@fit.cvut.cz
 * @brief Code for Arduino in glove
 */


void setup() {
  Serial.begin(9600); // initialize serial communication at 9600 bits per second:
}

/**
* @brief Read data on serial line  
* @return bool value, true if recived data is equal to "RDY"
*
*/
bool readSerial () {       
                           
  String inData;
  String strSend = "RD"; 
  int lastSym;
  if (Serial.available()) {
    inData = Serial.readString();   
    lastSym = inData.indexOf('Y'); 
    inData = inData.substring(0, lastSym);   
    if (inData.equals(strSend))
      return true;
  }
  return false;
}

/**
* @brief The loop routine runs over and over again forever
* 
*
*/
void loop() {
  
  String resStr;
  int sensorValue;
  char ch;
        Serial.flush();

  while (true) {

      resStr = "S:";
      sensorValue = analogRead(A0);                       // read data from pin
      sensorValue = map(sensorValue, 400, 850, 250, 0); 
      resStr+= String(sensorValue);
      resStr+= ",";
      sensorValue = analogRead(A1);
      sensorValue = map(sensorValue, 590, 810, 250, 0); 
      resStr += String(sensorValue);
      resStr+= ",";
      sensorValue = analogRead(A2);
      sensorValue = map(sensorValue, 400, 550, 250, 0);       
      resStr += String(sensorValue);
      resStr+= ",";
      sensorValue = analogRead(A3);
      sensorValue = map(sensorValue, 470, 710, 250, 0);       
      resStr += String(sensorValue);
      resStr+= ",";
      sensorValue = analogRead(A4);
      sensorValue = map(sensorValue, 400, 800, 250, 0);       
      resStr += String(sensorValue);
      resStr+= ",-1";
      resStr+= ":E";

      if (readSerial()) {
      Serial.println(resStr);                          // send data from all pins
  
  }
 }
}
