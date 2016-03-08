/*
  Modbus Carlo Gavazzi String meter
 
 This program do the following tasks:
 
 - Arduino asks the Meter in Modbus protocol the following data,
 every 2 seconds and save a mean every minute:
     * Active power
 
 created  18 Feb 2016
 by David Calvo
  	 
 */

#include <SD.h>

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4; // Pin for CS SD
const int mode485 = 2; // Pin for mode 485 - High is transmit
// variable to save the reading:
byte response[9]; // Tª response are 7 bytes, V-I response are 9 bytes
word value; // is the reading value, 2 bytes
// variables to save the time of each reading
unsigned long timer = 0;
unsigned long pretimer = 0;
unsigned long segundosencendido=0;
unsigned long presegundosencendido=0;
unsigned long dia=0;
unsigned long hora=0;
unsigned long minuto=0;
unsigned long segundo=0;
unsigned long diainicio=0; // to change each time the program is charged
unsigned long mesinicio =0;
unsigned long anoinicio =0;
unsigned long horainicio =0;
unsigned long minutoinicio=0;
unsigned long segundoinicio=0;

int i;
float temp=0; // Tª to register in SD
float tens=0; // V to register in SD
float inten=0; // I to register in SD

int confi[5][2]; // data to read from the "confi.txt" File
byte valor;
byte delbyte; // used to delete buffer if necessary
int next=0;

void setup()
{ 
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  pinMode(mode485, OUTPUT);   // Pin for mode485 - High is transmit
  digitalWrite(mode485, LOW);
  
  // make sure that the default chip select pin in SD is set to
  // output, even if you don't use it:
  SD.begin(chipSelect);
  pinMode(10, OUTPUT);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File confiFile= SD.open("confi.txt");
  
  if (confiFile) {
    next=0;
    while (confiFile.available()) {
      valor=confiFile.read();
      if (valor==';') {
        next++;
      } else {
        confi[next][0]=int(valor);
        confi[next][1]=int(confiFile.read());
      }
    }
    confiFile.close();
  }
  // Values are the Ascii code in DEC, so minus 48 (is 0 in Ascii code)
  // to obtain the number we need.
  diainicio=(confi[0][0]-48)*10+confi[0][1]-48;
  mesinicio=(confi[1][0]-48)*10+confi[1][1]-48;
  anoinicio=(confi[2][0]-48)*10+confi[2][1]-48;
  horainicio=(confi[3][0]-48)*10+confi[3][1]-48;
  minutoinicio=(confi[4][0]-48)*10+confi[4][1]-48;
  
  
  File dataFile = SD.open("data.csv", FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print("Date");
      dataFile.print(";");
      dataFile.print("Time");
      dataFile.print(";");
      dataFile.print("Tª(ºC)");
      dataFile.print(";");
      dataFile.print("Tension(V)");
      dataFile.print(";");
      dataFile.println("Intensidad(A)");
      dataFile.close();
      delay(2000);
    }
  segundoinicio=diainicio*86400+horainicio*3600+minutoinicio*60;
}

void loop() {

    delay(174000); // Readings every 3minutes
    timer = millis();
    if (timer-pretimer>10) { // esto es por si millis se desborda y vuelve a 0
      segundosencendido=timer/1000;
    }
    else {
      segundosencendido=presegundosencendido+(timer/1000);
    }
    pretimer=timer;
    presegundosencendido=segundosencendido;    

    // Pedimos valor de Tª:
    memset(response, 0,sizeof(response));// borra el contenido de response
    request(0x01,0x03,0x03,0x02,0x00,0x01,0x25,0x8E); // request Tª in HEX
    
    // Response of 7 bytes for Tª:
    i=0;
    if (Serial.available()==7) {
      while (Serial.available()>0) {
        if (i<7) {
          response[i]=Serial.read();
          i++;
        }
      }
    }
    else {
      while (Serial.available()>0) {
        delbyte=Serial.read();
      }
    }
    
    if (i==7) {
      temp = float(word(response[3],response[4]))/10; // Tª in ºC
    }
    delay(1000);

    // Pedimos valor de V-I:
    memset(response, 0,sizeof(response));// borra el contenido de response
    request(0x01,0x03,0x03,0x0A,0x00,0x02,0xE4,0x4D); // request V-I in HEX
    
    // Response of 9 bytes for V-I:
    i=0;
    if (Serial.available()==9) {
      while (Serial.available()>0) {
        if (i<9) {
          response[i]=Serial.read();
          i++;
        }
      }
    }
    else {
      while (Serial.available()>0) {
        delbyte=Serial.read();
      }
    }
    
    if (i==9) {
      tens = float(word(response[3],response[4]))/10; // Tension in V
      inten = float(word(response[5],response[6]))/100; // Intens in A
    }
    delay(1000);
      
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
     File dataFile = SD.open("data.csv", FILE_WRITE);
  
     // if the file is available, write to it:
     if (dataFile) {
        /*
        Lo primero que hacemos es calcular los valores de Timing
        teniendo en cuenta los datos de inicio.
        Los datos desde encencido son como si hubieramos arrancado el 
        equipo a las 0:00:00 del dia 0.
        Lo que hacemos es adelantar los valores segun los segundos de
        inicio:
        */
       segundo = segundosencendido+86400+segundoinicio; // no existe el dia 0, por eso se suma 86400
        
       dia=segundo/86400;
       hora=(segundo/3600)-(dia*24);
       minuto=(segundo/60)-(dia*1440)-(hora*60);
       segundo=segundo-(dia*86400)-(hora*3600)-(minuto*60);
       
       dataFile.print(dia-1);
       dataFile.print("/");
       dataFile.print(mesinicio);
       dataFile.print("/");
       dataFile.print(anoinicio);
       dataFile.print(";");
       dataFile.print(hora);
       dataFile.print(":");
       dataFile.print(minuto);
       dataFile.print(":");
       dataFile.print(segundo);
       dataFile.print(";");
       dataFile.print(temp); // in ºC
       dataFile.print(";");
       dataFile.print(tens); // in V
       dataFile.print(";");
       dataFile.println(inten); // in A  
       dataFile.close();
     }
     // put to 0 acumulative variables:
     temp=0;
     tens=0;
     inten=0;  
}


void request(byte data01,byte data02,byte data03,byte data04,byte data05,byte data06,byte data07, byte data08){ 
  
  digitalWrite(mode485, HIGH); // Transmit by 485:
  delay(10); 
    
  Serial.flush();
  Serial.write(data01);
  Serial.write(data02);
  Serial.write(data03);
  Serial.write(data04);
  Serial.write(data05);
  Serial.write(data06);
  Serial.write(data07);
  Serial.write(data08);
  delay(9);
  digitalWrite(mode485, LOW); // Receive by 485
  delay(1968);
}
