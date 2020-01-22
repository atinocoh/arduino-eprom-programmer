//FIXME

unsigned char eprom_id = 0;  // SNES = 0,  ZX = 1
const unsigned long bauds = 9600;
//const unsigned long bauds = 250000;
const unsigned long arrayBauds[6] = {9600,19200,38400,57600,115200,250000};
const int zxoutputEnabled=31;

const int programPin[2]={3,3};
const int readPin[2]={4,4};
const int enablePin[2]={2,39};

//                                SNES, ZX
const long int romSize[2]={1048576, 32768};   

/*
 * snes pinout(some of the pins don't follow the standard pinout)
 * use this pinout for burning eproms for use with snes.
 * You can also use the other pinout and mess up the rom using utilities
 * available online*/
 
 
int adrPins[20]={22,//eprom A0  snes A0
                  23,//eprom A1  snes A1
                  24,//eprom A2  snes A2
                  25,//eprom A3  snes A3
                  26,//eprom A4  snes A4
                  27,//eprom A5  snes A5
                  28,//eprom A6  snes A6
                  29,//eprom A7  snes A7
                  30,//eprom A8  snes A8
                  31,//eprom A9  snes A9
                  32,//eprom A10 snes A10
                  33,//eprom A11 snes A11
                  34,//eprom A12 snes A12
                  35,//eprom A13 snes A13
                  36,//eprom A14 snes A14
                  37,//eprom A15 snes A15
                  40,//eprom A18 snes A16 *
                  41,//eprom A19 snes A17 *
                  38,//eprom A16 snes A18 *
                  39,//eprom A17 snes A19 * 
                  };
int zxadrPins[15]={24,
                   25,
                   26,
                   27,
                   28,
                   29,
                   34,
                   37,
                   36,
                   35,
                   33,
                   30,
                   40,
                   41,
                   38,
                  };

int *addressTable[2]={adrPins,zxadrPins};

                  
/*                  
 *Regular eprom pinout, uncommnet this one and comment the above                   
 *to burn regular eproms for use with other systems.
 *You may have to actually edit this for use with your console/8bit computer
 *The pinout from the eprom is different from the snes pinout
int adrPins[20]={22,//eprom A0  snes A0
                  23,//eprom A1  snes A1
                  24,//eprom A2  snes A2
                  25,//eprom A3  snes A3
                  26,//eprom A4  snes A4
                  27,//eprom A5  snes A5
                  28,//eprom A6  snes A6
                  29,//eprom A7  snes A7
                  30,//eprom A8  snes A8
                  31,//eprom A9  snes A9
                  32,//eprom A10 snes A10
                  33,//eprom A11 snes A11
                  34,//eprom A12 snes A12
                  35,//eprom A13 snes A13
                  36,//eprom A14 snes A14
                  37,//eprom A15 snes A15
                  38,//eprom A16 snes A18 *
                  39,//eprom A17 snes A19 *
                  40,//eprom A18 snes A16 *
                  41 //eprom A19 snes A17 *
                  };*/

char snesdataPins[8]={5,6,7,8,9,10,11,12};
char zxdataPins[8]={23,22,5,10,11,12,2,32};

char *dataPins[2]={snesdataPins,zxdataPins};
char dataBusSize[2]={8,8};

byte inByte=0;
unsigned int secH=0,secL=0;


void setBaudSpeed(byte speed){
 // Serial.print("Entra en actualizar bauds");
    Serial.end();
    
  //Serial.print("Se ha actualizado la velocidad: ");
  //Serial.print(arrayBauds[speed]);
    switch(speed){
          case '0':
              Serial.begin(arrayBauds[0]);
              break;
          case '1':
              Serial.begin(arrayBauds[1]);
              break;
          case '2':
              Serial.begin(arrayBauds[2]);
              break;
          case '3':
              Serial.begin(arrayBauds[3]);
              break;
          case '4':
              Serial.begin(arrayBauds[4]);
              break;
          case '5':
              Serial.begin(arrayBauds[5]);
              break;
    }
}

void setInitialState(){
  // We set the direction:
    pinMode(programPin[eprom_id],OUTPUT);
    pinMode(readPin[eprom_id],OUTPUT);
    pinMode(enablePin[eprom_id],OUTPUT);
  // We set the initial state:
    digitalWrite(programPin[eprom_id],LOW);
    digitalWrite(readPin[eprom_id],LOW);
    digitalWrite(enablePin[eprom_id],HIGH);
    
    if (eprom_id==0){
        for(int i=0;i<20;i++){
          pinMode(addressTable[eprom_id][i],OUTPUT);
        }
    }
    else{
        pinMode(zxoutputEnabled,OUTPUT);
        for(int i=0;i<15;i++){
          pinMode(addressTable[eprom_id][i],OUTPUT);
        //  Serial.print(addressTable[eprom_id][i]);
        }
        digitalWrite(zxoutputEnabled,LOW);    
    }
}


void setup() {
  /**/
  Serial.begin(bauds);
  delay(1000);
 
}
int index=0;
void loop() {
  if(Serial.available()){
    inByte=Serial.read();
    if(inByte==0x55){   //'U'
      while(Serial.available()==0);
      inByte=Serial.read();
      switch(inByte){
        case 'w':
          setInitialState();
          programMode();
          while(Serial.available()<2);
          secH=Serial.read();
          secL=Serial.read();
          writeSector(secH,secL);
          break;
        case 'r':
          eprom_id=0;
          setInitialState();
          readMode();
          readROM();
          break;
        case 'z':
          eprom_id=1;
          setInitialState();
          readMode();
          readROM();
          break;
      }
    }
    if(inByte==0x62){  //'b'
        while(Serial.available()==0);
        inByte=Serial.read();
          if(inByte>='0' && inByte<='5')
            setBaudSpeed(inByte);
    }
  }
}


//low level functions, direct ccontact with hardware pins
void programMode(){
  //data as output
  int sz=dataBusSize[eprom_id];
  for(int i=0;i<sz;i++){
      pinMode((dataPins[eprom_id][i]),OUTPUT);
  }
  digitalWrite(readPin[eprom_id],LOW);
  digitalWrite(programPin[eprom_id],HIGH);
}
void readMode(){
  //data as input
  //int sz=sizeof(dataPins[eprom_id]);
  int sz=dataBusSize[eprom_id];
  for(int i=0;i<sz;i++){
      pinMode((dataPins[eprom_id][i]),INPUT);
      //Serial.print((int)dataPins[eprom_id][i]);
  }
  digitalWrite(programPin[eprom_id],LOW);
  digitalWrite(readPin[eprom_id],LOW);

}
void setAddress(uint32_t Addr){
    for(int i=0;i<8;i++){
      digitalWrite(adrPins[i],Addr&(1<<i));
    }
    Addr=Addr>>8;
    for(int i=0;i<8;i++){
      digitalWrite(adrPins[i+8],Addr&(1<<i));
    }
    Addr=Addr>>8;
    for(int i=0;i<4;i++){
      digitalWrite(adrPins[i+16],Addr&(1<<i));
    }
}

void zxsetAddress(uint32_t Addr){
    for(int i=0;i<8;i++){
      digitalWrite(zxadrPins[i],Addr&(1<<i));
    }
    Addr=Addr>>8;
    for(int i=0;i<7;i++){
      digitalWrite(zxadrPins[i+8],Addr&(1<<i));
    }

}
byte readByte(unsigned long adr){
    byte data=0;
    if (eprom_id==0){
      setAddress(adr);
    }
    else{
      zxsetAddress(adr);
    }
    digitalWrite(enablePin[eprom_id],LOW);
    delayMicroseconds(1);
    for(int i=7;i>=0;i--){
        data=data<<1;
        data|=(digitalRead(dataPins[eprom_id][i]))&1;
    }
    digitalWrite(enablePin[eprom_id],HIGH);
    return data;
}
void setData(char Data){
  for(int i=0;i<8;i++){
      digitalWrite(dataPins[eprom_id][i],Data&(1<<i));
  }
}
void programByte(byte Data){
  setData(Data);
  //Vpp pulse
  delayMicroseconds(4);
  digitalWrite(enablePin[eprom_id],LOW);
  delayMicroseconds(60);
  digitalWrite(enablePin[eprom_id],HIGH);
}

void writeSector(unsigned char sectorH,unsigned char sectorL){
  byte dataBuffer[128];
  unsigned long address=0;
  byte CHK=sectorH,CHKreceived;
  CHK^=sectorL;

  address=sectorH;
  address=(address<<8)|sectorL;
  address*=128;

  for(int i=0;i<128;i++){
        while(Serial.available()==0);
        dataBuffer[i]=Serial.read();
        CHK ^= dataBuffer[i];
  }
  while(Serial.available()==0);
  CHKreceived=Serial.read();
  programMode();
  //only program the bytes if the checksum is equal to the one received
  if(CHKreceived==CHK){
    for (int i = 0; i < 128; i++){
      setAddress(address++);
      programByte(dataBuffer[i]);
    }
  Serial.write(CHK);
  }
  readMode();

}
void readROM(){
  unsigned long address;
  byte data=0;
  bool cancel=false;
  address=0;
  //read mode
  readMode();
  //start frame
  digitalWrite(readPin[eprom_id],LOW);
  digitalWrite(programPin[eprom_id],LOW);
  for(long i=0;(i<romSize[eprom_id])&&!cancel;i++){//1048576
    data=readByte(address++);
    Serial.write(data);
    if (Serial.available()>0){
        byte c=Serial.read(); 
        if (c=='c')
          cancel=true; 
    }
    //checksum^=data;
  }
  digitalWrite(readPin[eprom_id],HIGH);

  //Serial.write(checksum);
  //Serial.write(0xAA);
}
