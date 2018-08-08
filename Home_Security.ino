  /*home security system by andres hernandez*/

#define HWSERIAL Serial1

int presense_sensor = 15;
int gasSensor = 14;
int fan = 4;
int led = 13;// pin to connect to led or relay
int sensorValue = 0;
int rxByte;
int txByte;

//variables needed for modbus
int slaveID;
int functionID;
int crc;
int rede = 2;
int message;
byte packet[] = {0,0,0,0};
static int listening = 0;
static int flickPresence;
static int flickGas;

void setup() {
  Serial.begin(9600);
  HWSERIAL.begin(9600); 
  pinMode(led,OUTPUT);            // set pin 13 for led
  pinMode(fan,OUTPUT);            //set pin 4 for fan   
  pinMode(rede,OUTPUT);           //drive output enable and receiver output enable pin
}

void clearPacket(){
  slaveID = 0;
  functionID = 0;
  crc = 0;
  message = 0;
}

void listenToSlave() {
  static int field = 0;
  if (HWSERIAL.available() > 0) {
    if(field == 0){
      slaveID = int(HWSERIAL.read());
      //Serial.print("slave ID: ");
      //Serial.println(slaveID);
      field++;
      listening = 1;
    }
    else if (field == 1){
      functionID = int(HWSERIAL.read());    
      //Serial.print("function ID: ");
      //Serial.println(functionID);
      field++;
      listening = 1;
    }
    else if (field == 2){
      message = int(HWSERIAL.read());
      //Serial.print("message: ");
      //Serial.println(message);
      field++;
      listening = 1;
    }
    else if(field == 3){
      crc = int(HWSERIAL.read());
      //Serial.print("crc: ");
      //Serial.println(crc);
      field = 0;
      listening = 0;
    }
    else                    //flush the serial port
    {
      int t = HWSERIAL.read();
      delay(200);
      field = 0;
      listening = 0;
    }
  }
  else                      //flush the serial port
    {
      int t = HWSERIAL.read();
      delay(200);
      field = 0;
    }
}

void readPresence() {
  sensorValue = analogRead(presense_sensor);
  
  if (sensorValue > 500){
    if(flickPresence == 0){
      Serial.println("Room presence detected! Alarm activated!");
      flickPresence = 1;
    }
  }
  else{
    if(flickPresence == 1){
      flickPresence = 0;
    }
  }
  delay(50);
}

void readGas() {
  sensorValue = analogRead(gasSensor);
//  Serial.print("gas sensor value at: ");
//  Serial.println(sensorValue, DEC);

  if (sensorValue > 140) {
    if(flickGas == 0){
      Serial.println("abnormal gas readings! smoke alarm activated! (teensy LED)!");
      digitalWrite(led, HIGH);
      flickGas = 1;
    }
  }
  else {
    if(sensorValue < 130 && flickGas == 1){
      Serial.println("normal gas readings, turning off smoke alarm... (teensy LED)!");
      digitalWrite(led, LOW);
      flickGas = 0;
    }
  }

  delay(50);
  
}

void checkTemperature(int temperature) {
  Serial.print("temperature: ");
  Serial.println(temperature);
  if(temperature >= 27){
    Serial.println("temperature has risen, turning on the HVAC system...");
    digitalWrite(fan, HIGH);
  }
  else {
    Serial.println("temperature has fallen, turning off the HVAC system...");
    digitalWrite(fan, LOW);
    }
 
}

void barGauge() {
  
}

void loop() {
  listenToSlave(); 
  if(listening == 0){
    if(slaveID == 1){ 
      switch(functionID){                                       //checking which type of information hes trying to send
        case 10:                                                  //case for the temperature sensing
          //Serial.println("attempting to turn on the fan: ");
          checkTemperature(message);
          clearPacket();
          break;
        case 20:                                                  //case for the strike plate unlocked
          Serial.println("Front door is open...");
          clearPacket();
          break;
        case 30: 
          Serial.println("no case for the third function yet");
          break;
        default:
          break;
      } 
    }
  }
  readPresence();
  readGas();
}








