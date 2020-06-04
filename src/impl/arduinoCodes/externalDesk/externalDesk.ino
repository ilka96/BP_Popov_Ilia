/**
 * @file externalDesk.ino
 * @author popovili@fit.cvut.cz
 * @brief Code for external desk in robotic hand
 */

#include<SPI.h>
volatile boolean received;
volatile uint8_t Slavereceived,Slavesend;
int buttonvalue;
int x;
 
void setup()
{
  //Serial.begin(9600);
  
  pinMode(MISO,OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN 
  SPI.setClockDivider(SPI_CLOCK_DIV8); 
  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode
  received = false;

  SPI.attachInterrupt();                  //Interuupt ON is set for SPI commnucation
  
}

/**
* @brief Inerrrput routine function 
*/
ISR (SPI_STC_vect)                 
{
  Slavereceived = SPDR;                  // Value received from master if store in variable slavereceived
  
  switch(Slavereceived) {               // Choose data for send
      case 1:
        x = analogRead( A0 );   
        Slavesend = map(x, 0, 1024, 0, 255);        
       SPDR = Slavesend;
      break;
      case 2:
        x = analogRead( A1 );                         
        Slavesend = map(x, 0, 1024, 0, 255);             
        SPDR = Slavesend;    
      break;
      case 3:
        x = analogRead( A2 );                         
        Slavesend = map(x, 0, 1024, 0, 255);            
        SPDR = Slavesend;
      break;
      case 4:
        x = analogRead( A3 );                          
        Slavesend = map(x, 0, 1024, 0, 255);        
        SPDR = Slavesend;
      break;
      case 5:
        x = analogRead( A4 );                         
        Slavesend = map(x, 0, 1024, 0, 255);      
       SPDR = Slavesend; 
      break;
      case 6:
        x = analogRead( A5 );                         
        Slavesend = map(x, 0, 1024, 0, 255);         
       SPDR = Slavesend;
      }  
 
}

void loop()
{ 
}
