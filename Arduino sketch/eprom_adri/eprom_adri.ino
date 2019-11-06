//FIXME

unsigned char iszx = 0;

const int zxoutputEnabled=31;

const int zxprogramPin=3;
const int zxreadPin=4;
const int zxchipEnabled=39;

/*
const int programPin=3;
const int readPin=4;
const int enablePin=2;

*/
const int programPin[2]={3,3};
const int readPin[2]={4,4};
const int enablePin[2]={2,39};





const unsigned int zxromSize=32768;
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

byte inByte=0;
unsigned int secH=0,secL=0;


void setInitialState(){
    pinMode(programPin[iszx],OUTPUT);
    pinMode(readPin[iszx],OUTPUT);
    pinMode(enablePin[iszx],OUTPUT);
    if (iszx==0){
      for(int i=0;i<20;i++){
        pinMode(adrPins[i],OUTPUT);
      }
    }
    else{
      pinMode(zxoutputEnabled,OUTPUT);
      for(int i=0;i<14;i++){
        pinMode(zxadrPins[i],OUTPUT);
      }
      digitalWrite(zxoutputEnabled,LOW);    
    }
    digitalWrite(programPin[iszx],LOW);
    digitalWrite(readPin[iszx],LOW);
    digitalWrite(enablePin[iszx],HIGH);
}


void setup() {
  /**/
  Serial.begin(250000);
  delay(1000);
  programMode();
}
int index=0;
void loop() {
  if(Serial.available()){
    inByte=Serial.read();
    if(inByte==0x55){
      while(Serial.available()==0);
      inByte=Serial.read();
      switch(inByte){
        case 'w':
          /*programMode();
          while(Serial.available()<2);
          secH=Serial.read();
          secL=Serial.read();
          writeSector(secH,secL);*/
          break;
        case 'r':
          iszx=0;
          readMode();
          readROM();
          break;
        case 'z':
          iszx=1;
          readMode();
          readROM();
          break;
      }
    }
  }
}


//low level functions, direct ccontact with hardware pins
void programMode(){
  //data as output
  int sz=sizeof(*(dataPins[iszx]));
  for(int i=0;i<sz;i++){
    pinMode((dataPins[iszx][i]),OUTPUT);
  }
  digitalWrite(readPin[iszx],LOW);
  digitalWrite(programPin[iszx],HIGH);
}
void readMode(){
  //data as input
  int sz=sizeof(*(dataPins[iszx]));
  for(int i=0;i<sz;i++){
    pinMode((dataPins[iszx][i]),INPUT);
  }
  digitalWrite(programPin[iszx],LOW);
  digitalWrite(readPin[iszx],LOW);

}/*
void zxreadMode(){
  //data as input
  for(int i=0;i<8;i++){
    pinMode(zxdataPins[i],INPUT);
  }
  digitalWrite(zxprogramPin,LOW);
  digitalWrite(zxreadPin,LOW);
  

}*/
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
    if (iszx==0)
      setAddress(adr);
    else
      zxsetAddress(adr);
    digitalWrite(enablePin[iszx],LOW);
    delayMicroseconds(1);
    for(int i=7;i>=0;i--){
        data=data<<1;
        data|=digitalRead(dataPins[iszx][i])&1;
    }
    digitalWrite(enablePin[iszx],HIGH);
    return data;
}/*
byte zxreadByte(unsigned long adr){
    byte data=0;
    zxsetAddress(adr);
    digitalWrite(zxchipEnabled,LOW);
    digitalWrite(zxoutputEnabled,LOW);
    delayMicroseconds(50);
    for(int i=7;i>=0;i--){
        data=data<<1;
        data|=digitalRead(zxdataPins[i])&1;
    }
    digitalWrite(zxchipEnabled,HIGH);
    return data;
}*/
void setData(char Data){
  for(int i=0;i<8;i++){
      digitalWrite(dataPins[i],Data&(1<<i));
  }
}
void programByte(byte Data){
  setData(Data);
  //Vpp pulse
  delayMicroseconds(4);
  digitalWrite(enablePin,LOW);
  delayMicroseconds(60);
  digitalWrite(enablePin,HIGH);
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
  digitalWrite(readPin[iszx],LOW);
  digitalWrite(programPin[iszx],LOW);
  for(long i=0;(i<romSize[iszx])&&!cancel;i++){//1048576
    data=readByte(address++);
    Serial.write(data);
    if (Serial.available()>0){
        byte c=Serial.read(); 
        if (c=='c')
          cancel=true; 
    }
    //checksum^=data;
  }
  digitalWrite(readPin[iszx],HIGH);

  //Serial.write(checksum);
  //Serial.write(0xAA);
}/*
void zxreadROM(){
  unsigned long address;
  byte data=0;
  bool cancel=false;
  address=0;
  //read mode
  zxreadMode();
  //start frame
  digitalWrite(zxreadPin,LOW);
  digitalWrite(zxprogramPin,LOW);
  for(long i=0;(i<zxromSize)&&!cancel;i++){//1048576
    data=zxreadByte(address++);
    Serial.write(data);
    if (Serial.available()>0){
        byte c=Serial.read(); 
        if (c=='c')
          cancel=true; 
    }
    //checksum^=data;
  }
  digitalWrite(zxreadPin,HIGH);

  //Serial.write(checksum);
  //Serial.write(0xAA);
}*/
