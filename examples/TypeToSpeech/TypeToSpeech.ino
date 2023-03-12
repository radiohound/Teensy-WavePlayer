/*
  TypeToSpeech.ino

  Example of using phonemes for Text To Speech sythesis

  Open the Serial Monitor and type in words to hear see the results.

  created March 9 2023
  by Walter Dunckel

  This uses Frank Boesing's WavePlayer library in order to play 22050 Hertz wav files
  


*/
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <play_wav.h>       //https://github.com/FrankBoesing/Teensy-WavePlayer

#if defined(__IMXRT1062__)
#define T4
#include <utility/imxrt_hw.h> // make available set_audioClock() for setting I2S freq on Teensy 4
#else
#define F_I2S ((((I2S0_MCR >> 24) & 0x03) == 3) ? F_PLL : F_CPU) // calculation for I2S freq on Teensy 3
#endif

// GUItool: begin automatically generated code
AudioPlayWav             playWav1;     //xy=210,161
AudioOutputI2S           outputsound;       //xy=417,124
AudioConnection          patchCord1(playWav1, 0, outputsound, 0);
AudioConnection          patchCord2(playWav1, 0, outputsound, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=413,177
// GUItool: end automatically generated code

String txtMsg = "";                               // a string for incoming text
unsigned int lastStringLength = txtMsg.length();  // previous length of the String

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  
  // send an intro:
  //Serial.println("\n\nString  length():");
  //Serial.println();

  AudioMemory(100); //was 50
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  
  if (!(SD.begin(BUILTIN_SDCARD))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(1000);
    }
  }
  delay(1000);
  Serial.println("Type some words for me to try to speak, and press enter");
}

#ifdef T4
#else
// calculate I2S dividers for Teensy 3
uint32_t I2S_dividers( float fsamp, uint32_t nbits, uint32_t tcr2_div )
{

  unsigned fract, divi;
  fract = divi = 1;
  float minfehler = 1e7;

  unsigned x = (nbits * ((tcr2_div + 1) * 2));
  unsigned b = F_I2S / x;

  for (unsigned i = 1; i < 256; i++) {

    unsigned d = round(b / fsamp * i);
    float freq = b * i / (float)d ;
    float fehler = fabs(fsamp - freq);

    if ( fehler < minfehler && d < 4096 ) {
      fract = i;
      divi = d;
      minfehler = fehler;
      if (fehler == 0.0f) break;
    }

  }

  return I2S_MDR_FRACT( (fract - 1) ) | I2S_MDR_DIVIDE( (divi - 1) );
}
#endif

// set I2S samplerate
void setI2SFreq(int freq) {
#if defined(T4)
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (freq * 256 * n1);
  double C = ((double)freq * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, true);
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
       | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
       | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f
#else
  unsigned tcr5 = I2S0_TCR5;
  unsigned word0width = ((tcr5 >> 24) & 0x1f) + 1;
  unsigned wordnwidth = ((tcr5 >> 16) & 0x1f) + 1;
  unsigned framesize = ((I2S0_TCR4 >> 16) & 0x0f) + 1;
  unsigned nbits = word0width + wordnwidth * (framesize - 1 );
  unsigned tcr2div = I2S0_TCR2 & 0xff; //bitclockdiv
  uint32_t MDR = I2S_dividers(freq, nbits, tcr2div);
  if (MDR > 0) {
    while (I2S0_MCR & I2S_MCR_DUF) {
      ;
    }
    I2S0_MDR = MDR;
  }
#endif
}

void loop() {
  int delayTime = 95;
  setI2SFreq(22050);
  // add any incoming characters to the String:
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    txtMsg += inChar;
  }

  // print the text in
  if (txtMsg.length() != lastStringLength) {
    Serial.println(txtMsg);

    for (int i=0; i<(txtMsg.length()-2); i++) {  //read each character without cariage return
      int letter = txtMsg.charAt(i);

      switch (letter) {

         case '0':
            Serial.print("Z");
            playWav1.play("Z.wav");
            delay(delayTime);
            Serial.print("IY");
            playWav1.play("IY.wav");
            delay(delayTime);
            Serial.print("R");
            playWav1.play("R.wav");
            delay(delayTime);      
            Serial.print("OW");
            playWav1.play("OW.wav");
            delay(delayTime);        
        break;       

        case '1':
            Serial.print("W");
            playWav1.play("W.wav");
            delay(delayTime);
            Serial.print("AH");
            playWav1.play("AH.wav");
            delay(delayTime);
            Serial.print("N");
            playWav1.play("N.wav");
            delay(delayTime);            
        break;

        case '2':
            Serial.print("T");
            playWav1.play("T.wav");
            delay(delayTime);
            Serial.print("UW");
            playWav1.play("UW.wav");
            delay(delayTime);
        break;
        
        case '3':
            Serial.print("TH");
            playWav1.play("TH.wav");
            delay(delayTime);
            Serial.print("R");
            playWav1.play("R.wav");
            delay(delayTime);
            Serial.print("IY");
            playWav1.play("IY.wav");
            delay(delayTime);            
        break;

        case '4':
            Serial.print("F");
            playWav1.play("F.wav");
            delay(delayTime);
            Serial.print("OH");
            playWav1.play("OH.wav");
            delay(delayTime);
            Serial.print("R");
            playWav1.play("R.wav");
            delay(delayTime);            
        break;

        case '5':
            Serial.print("F");
            playWav1.play("F.wav");
            delay(delayTime);
            Serial.print("AY");
            playWav1.play("AY.wav");
            delay(delayTime);
            Serial.print("V");
            playWav1.play("V.wav");
            delay(delayTime);            
        break;

        case '6':
            Serial.print("S");
            playWav1.play("S.wav");
            delay(delayTime);
            Serial.print("IH");
            playWav1.play("IH.wav");
            delay(delayTime);
            Serial.print("K");
            playWav1.play("K.wav");
            delay(delayTime);            
            Serial.print("S");
            playWav1.play("S.wav");
            delay(delayTime); 
        break;

        case '7':
            Serial.print("S");
            playWav1.play("S.wav");
            delay(delayTime);
            Serial.print("EH");
            playWav1.play("EH.wav");
            delay(delayTime);
            Serial.print("V");
            playWav1.play("V.wav");
            delay(delayTime);            
            Serial.print("IX");
            playWav1.play("IX.wav");
            delay(delayTime); 
            Serial.print("N");
            playWav1.play("N.wav");
            delay(delayTime); 
        break;
        
        case '8':
            Serial.print("EY");
            playWav1.play("EY.wav");
            delay(delayTime);
            Serial.print("T");
            playWav1.play("T.wav");
            delay(delayTime);
        break;

        case '9':
            Serial.print("N");
            playWav1.play("N.wav");
            delay(delayTime);
            Serial.print("AY");
            playWav1.play("AY.wav");
            delay(delayTime);
            Serial.print("N");
            playWav1.play("N.wav");
            delay(delayTime);            
        break;       
  
        case 'a':
          if (txtMsg.charAt(i+1)=='i') { //this checks for ai
            i++; //skip i sound, because it is included in phoneme below
            Serial.print("EY");
            playWav1.play("EY.wav");
          } else if (((txtMsg.charAt(i+2))=='e') || ((txtMsg.charAt(i+2))=='i')) { 
              Serial.print("EY");  
              playWav1.play("EY.wav");
          } else if (((txtMsg.charAt(i-1))=='g') && ((txtMsg.charAt(i+1))=='l')) {
              Serial.print("AX");
              playWav1.play("AX.wav");
          } else if (((txtMsg.charAt(i+1))=='r') && ((txtMsg.charAt(i+2))=='y')) {
              i=i+2;
              Serial.print("EH");
              playWav1.play("EH.wav");   
              delay(delayTime);
              Serial.print("R");
              playWav1.play("R.wav");   
              delay(delayTime);  
              Serial.print("IY");
              playWav1.play("IY.wav");
          } else if ((txtMsg.charAt(i+1))=='l') { 
              Serial.print("AO");
              playWav1.play("AO.wav");
          } else {
              Serial.print("AE"); 
              playWav1.play("AE.wav");
          }
          delay(delayTime);  
        break;

        case 'b':
          Serial.print("B");
          playWav1.play("B.wav");
          delay(delayTime);  
        break;

        case 'c':
          if ((txtMsg.charAt(i+1)=='i') && (txtMsg.charAt(i+2)=='a') && (txtMsg.charAt(i+3)=='l'))  {
              i=i+2;
              Serial.print("SH");
              playWav1.play("SH.wav");
              delay(delayTime);
              Serial.print("UL");
              playWav1.play("UL.wav"); 

          } else if ((txtMsg.charAt(i+1)=='i') && ((txtMsg.charAt(i+2)=='e') || (txtMsg.charAt(i+2)=='a'))) { 
              Serial.print("SH");
              playWav1.play("SH.wav");   
          } else if (txtMsg.charAt(i-1)=='x') {  
              delay(1);
          } else if ((txtMsg.charAt(i+1)=='e') || (txtMsg.charAt(i+1)=='i')) {  
            Serial.print("S");
            playWav1.play("S.wav");
          } else if (txtMsg.charAt(i+1)=='h') {  
              i++;
              Serial.print("CH");
              playWav1.play("CH.wav");      
          } else if (txtMsg.charAt(i+1)=='y') {  
              Serial.print("S");
              playWav1.play("S.wav");                        
          } else {  
              Serial.print("K"); 
              playWav1.play("K.wav"); 
          }
          delay(delayTime);  
        break;

        case 'd':
          Serial.print("D");
          playWav1.play("D.wav");
          delay(delayTime);  
        break;

        case 'e':
        if ((txtMsg.charAt(i+1)=='o') && (txtMsg.charAt(i+2)=='u')) { 
              i=i+2; //skip sound, because it is included in phoneme below
              Serial.print("IY");
              playWav1.play("IY.wav");
              delay(delayTime); 
              Serial.print("AX");
              playWav1.play("AX.wav");
          } else if ((txtMsg.charAt(i+1)=='t') && (txtMsg.charAt(i-1)=='i') && (txtMsg.charAt(i+2)=='y')) { 
              i=i+2; //skip sound, because it is included in phoneme below
              Serial.print("IX");
              playWav1.play("IX.wav");
              delay(delayTime);
              Serial.print("T");
              playWav1.play("T.wav");
              delay(delayTime); 
              Serial.print("IY");
              playWav1.play("IY.wav");
          } else if ((txtMsg.charAt(i+1)=='t') && (txtMsg.charAt(i+2)=='y')) { 
              i=i+2; 
              Serial.print("T");
              playWav1.play("T.wav");
              delay(delayTime); 
              Serial.print("IY");
              playWav1.play("IY.wav");
          } else if ((txtMsg.charAt(i+1)=='a') && ((txtMsg.charAt(i+2)==' ') || (txtMsg.charAt(i+2)=='\r') || (txtMsg.charAt(i+2)=='s'))) { 
              i++; 
              Serial.print("IY");
              playWav1.play("IY.wav");
              delay(delayTime); 
              Serial.print("AX");
              playWav1.play("AX.wav");
          } else if ((txtMsg.charAt(i+1)=='c') && (txtMsg.charAt(i+2)=='i')) { 
              Serial.print("EH");
              playWav1.play("EH.wav");
          } else if (txtMsg.charAt(i+1)=='o') { 
            i++; 
            Serial.print("IY");
            playWav1.play("IY.wav");
            delay(delayTime);
            Serial.print("OW");
            playWav1.play("OW.wav");
          } else if (txtMsg.charAt(i+1)=='w') { 
            i++; 
            Serial.print("UW");
            playWav1.play("UW.wav") ;
          } else if ((txtMsg.charAt(i+2)=='e') || (txtMsg.charAt(i+2)=='i') || (txtMsg.charAt(i+2)=='y') || (txtMsg.charAt(i+2)=='o') || (txtMsg.charAt(i+2)=='u') || (txtMsg.charAt(i+2)=='a'))  { 
              Serial.print("IY");
              playWav1.play("IY.wav");
          } else if (txtMsg.charAt(i+1)=='e')   { 
              i++; 
              Serial.print("IY");
              playWav1.play("IY.wav");
          } else if (txtMsg.charAt(i+1)=='a') { 
              i++; 
              Serial.print("IY");
              playWav1.play("IY.wav");
          } else if (txtMsg.charAt(i+1)=='i') { 
              i++; 
              Serial.print("EY");
              playWav1.play("EY.wav");
          } else if (txtMsg.charAt(i+1)=='r') { 
              Serial.print("E");
              playWav1.play("E.wav");
          } else if (((txtMsg.charAt(i-2))=='t') && ((txtMsg.charAt(i-1))=='h')) {
              Serial.print("UH");
             playWav1.play("UH.wav");
          } else if (txtMsg.charAt(i+1)=='w') { 
              i++; 
              Serial.print("UW");
              playWav1.play("UW.wav");
          } else if (txtMsg.charAt(i+1)=='\r') { 
              Serial.print(" ");
              delay(delayTime); 
          } else if (txtMsg.charAt(i+1)==' ') { 
              Serial.print(" ");
 
          } else { 
              Serial.print("EH");
              playWav1.play("EH.wav");
          }
          delay(delayTime);  
        break;

        case 'f':
          Serial.print("F");
          playWav1.play("F.wav");
          delay(delayTime);  
        break;         

        case 'g':
          if ((txtMsg.charAt(i+1)=='e') || (txtMsg.charAt(i+1)=='i'))  { 
            //i++; //
            Serial.print("J");
            playWav1.play("J.wav");
          } else if (txtMsg.charAt(i+1)=='y') { 
              i++; //
              Serial.print("J");
              playWav1.play("J.wav");
              delay(delayTime);  
              Serial.print("IY");
              playWav1.play("IY.wav"); 
          } else { 
              Serial.print("G"); 
              playWav1.play("G.wav");
          }
          delay(delayTime);  
        break;
        
        case 'h':
          Serial.print("_H");
          playWav1.play("_H.wav");
          delay(delayTime); 
        break;   

        case 'I':
          Serial.print("AY");
          playWav1.play("AY.wav");
          delay(delayTime); 
        break;   

        case 'i':
          if ((txtMsg.charAt(i+1)=='c') && (txtMsg.charAt(i+2)=='e')) { 
              Serial.print("AY");
              playWav1.play("AY.wav"); 
          } else if ((txtMsg.charAt(i+1)=='t') && (txtMsg.charAt(i+2)=='y')) { 
              Serial.print("IX");
              playWav1.play("IX.wav");  
          } else if ((txtMsg.charAt(i+1)=='e') && (txtMsg.charAt(i+2)=='f')) {
              i++;
              Serial.print("IY");
              playWav1.play("IY.wav");  
          } else if ((txtMsg.charAt(i+1)=='e') && (txtMsg.charAt(i+2)=='w')) {
              Serial.print("Y");
              playWav1.play("Y.wav");  
          }  else if ((txtMsg.charAt(i+2)=='e') || (txtMsg.charAt(i+2)=='y') || (txtMsg.charAt(i+2)=='o')) { 
            Serial.print("AY");
            playWav1.play("AY.wav");
          } else if (txtMsg.charAt(i+1)=='r') { 
              i++; 
              Serial.print("ER");
              playWav1.play("ER.wav");
          } else if ((txtMsg.charAt(i+1)=='e') && (txtMsg.charAt(i+2)=='s')) { 
              i++;
              Serial.print("IY");
              playWav1.play("IY.wav");  
              delay(delayTime);
              Serial.print("S");
              playWav1.play("S.wav");  
          } else if ((txtMsg.charAt(i+1)=='e') && (txtMsg.charAt(i-1)=='r')) { 
              i++;
              Serial.print("IY");
              playWav1.play("IY.wav");  
          } else if (txtMsg.charAt(i+1)=='e') { 
              //i++;
              Serial.print("AY");
              playWav1.play("AY.wav");   
          } else if ((txtMsg.charAt(i+1)=='g') && (txtMsg.charAt(i+2)=='h')) { 
              i=i+2;
              Serial.print("AY");
              playWav1.play("AY.wav");     
               
          } else { 
              Serial.print("IH");
              playWav1.play("IH.wav");
          }
          delay(delayTime);  
        break;

        case 'j':
          Serial.print("J");
          playWav1.play("J.wav");
          delay(delayTime);  
        break;   

        case 'k':
          Serial.print("K");
          playWav1.play("K.wav");
          delay(delayTime);  
        break;   

        case 'l':
          Serial.print("L");
          playWav1.play("L.wav");
          delay(delayTime);  
        break;   

        case 'm':
          Serial.print("M");
          playWav1.play("M.wav");
          delay(delayTime); 
        break;           

        case 'n':
          if ((txtMsg.charAt(i+1)=='g') && ((txtMsg.charAt(i+2)=='e') || (txtMsg.charAt(i+2)=='i')) && (txtMsg.charAt(i-1)!='i'))  { 
            i++; //skip sound, because it is included in phoneme below
            Serial.print("N");
            playWav1.play("N.wav");
            delay(delayTime);
            Serial.print("J");
            playWav1.play("J.wav");
          } else if (txtMsg.charAt(i+1)=='g') { 
            i++; 
            Serial.print("NX");
            playWav1.play("NX.wav");
          } else { 
            Serial.print("N");
            playWav1.play("N.wav");
          }
          delay(delayTime); 
        break;   

        case 'o':
          if ((txtMsg.charAt(i-1)=='y') && (txtMsg.charAt(i+1)=='u'))  { 
              i++; 
              Serial.print("UW"); //changed from OW sound
              playWav1.play("UW.wav");    
          } else if (txtMsg.charAt(i+1)=='u') { 
            i++; 
            Serial.print("OW");
            playWav1.play("OW.wav");
          } else if ((txtMsg.charAt(i-1)=='l') && (txtMsg.charAt(i+1)=='w')){ 
              i++; 
              Serial.print("OW");
              playWav1.play("OW.wav");
          } else if ((txtMsg.charAt(i-1)=='m') && (txtMsg.charAt(i+1)=='w')){ 
              i++; 
              Serial.print("OW");
              playWav1.play("OW.wav");
          } else if ((txtMsg.charAt(i-1)=='h') && (txtMsg.charAt(i+1)=='w')){ 
              i++; 
              Serial.print("AW"); 
              playWav1.play("AW.wav");
          } else if ((txtMsg.charAt(i-1)=='w') && ((txtMsg.charAt(i+1)==' ') || (txtMsg.charAt(i+1)=='\r'))) { 
              Serial.print("UW"); 
              playWav1.play("UW.wav");   
          } else if (txtMsg.charAt(i+1)=='y'){ 
              i++;
              Serial.print("OY");
              playWav1.play("OY.wav");
          } else if ((txtMsg.charAt(i-1)=='g') && ((txtMsg.charAt(i+1)==' ') || (txtMsg.charAt(i+1)=='\r'))) { 
              Serial.print("OH");
              playWav1.play("OH.wav");
          } else if ((txtMsg.charAt(i-1)=='t') && ((txtMsg.charAt(i+1)==' ') || (txtMsg.charAt(i+1)=='\r'))) { 
              Serial.print("UX");
              playWav1.play("UX.wav");
          } else if ((txtMsg.charAt(i+1)=='o') && (txtMsg.charAt(i+2)=='k')){ 
              i++; 
              Serial.print("UH");
              playWav1.play("UH.wav");
          } else if (txtMsg.charAt(i+1)=='o') { 
              i++; 
              Serial.print("UX");
              playWav1.play("UX.wav");
          } else if ((txtMsg.charAt(i+1)=='n') && (txtMsg.charAt(i+2)!='e')){ 
              Serial.print("AA");
              playWav1.play("AA.wav");
          } else if (txtMsg.charAt(i+1)=='m') { 
              Serial.print("AH"); 
              playWav1.play("AH.wav");
          } else if ((txtMsg.charAt(i+2)=='e') || (txtMsg.charAt(i+2)=='i')|| (txtMsg.charAt(i+2)=='y') || (txtMsg.charAt(i+1)==' ') || (txtMsg.charAt(i+1)=='\r'))  { 
              Serial.print("OW");
              playWav1.play("OW.wav");
          } else if (txtMsg.charAt(i+1)=='a') {
              i++; 
              Serial.print("OH");
              playWav1.play("OH.wav");
          } else {
              Serial.print("AA"); 
              playWav1.play("AA.wav"); 
          }
          delay(delayTime);  
        break;

        case 'p':
          if (txtMsg.charAt(i+1)=='h') { 
              i++; 
              Serial.print("F");
              playWav1.play("F.wav");
          } else {
              Serial.print("P");              
              playWav1.play("P.wav");
          }
          delay(delayTime);  
        break;

        case 'q':
          Serial.print("K");
          playWav1.play("K.wav");
          delay(delayTime);   
        break; 

        case 'r':
          Serial.print("R");
          playWav1.play("R.wav");
          delay(delayTime);  
        break;  

        case 's':
          if (txtMsg.charAt(i+1)=='h') { 
            i++; 
            Serial.print("SH");
            playWav1.play("SH.wav");
          } else {
              Serial.print("S");
              playWav1.play("S.wav");
          }
          delay(delayTime);  
        break;    

        case 't':
          if ((txtMsg.charAt(i+1)=='i') && (txtMsg.charAt(i+2)=='o') && (txtMsg.charAt(i+3)=='n')){ 
              i=i+3; 
              Serial.print("SH");
              playWav1.play("SH.wav");
              delay(delayTime); 
              Serial.print("UN");
              playWav1.play("UN.wav");
          } else if ((txtMsg.charAt(i+1)=='i') && ((txtMsg.charAt(i+2)=='o') || (txtMsg.charAt(i+2)=='e'))) { 
              Serial.print("SH");
              playWav1.play("SH.wav");
          } else if ((txtMsg.charAt(i+1)=='u') && (txtMsg.charAt(i+2)=='a')) { 
              i++;
              Serial.print("CH");
              playWav1.play("CH.wav"); 
              delay(delayTime);
              Serial.print("EW");
              playWav1.play("EW.wav");  

              } else if ((txtMsg.charAt(i+1)=='i') && (txtMsg.charAt(i+2)=='a')) { 
              i++;
              Serial.print("SH");
              playWav1.play("SH.wav"); 
              delay(delayTime);
              Serial.print("IY");
              playWav1.play("IY.wav");    
          } else if (txtMsg.charAt(i+1)=='h') { 
              i++; 
              Serial.print("TH");
              playWav1.play("TH.wav");
          } else {
              Serial.print("T");
              playWav1.play("T.wav");
          }
          delay(delayTime);  
        break;   

        case 'u':
          if ((txtMsg.charAt(i-1)=='q') && (txtMsg.charAt(i+1)=='a'))  { 
              i++; 
              Serial.print("W");
              playWav1.play("W.wav");
              delay(delayTime); 
              Serial.print("AA");
              playWav1.play("AA.wav");
          } else if ((txtMsg.charAt(i-1)=='q') && (txtMsg.charAt(i+1)=='e'))  { 
              i++; 
              Serial.print("W");
              playWav1.play("W.wav");
              delay(delayTime);  
              Serial.print("EH");
              playWav1.play("EH.wav");
          } else if ((txtMsg.charAt(i-1)=='q') && (txtMsg.charAt(i+1)=='i'))  { 
              i++; 
              Serial.print("W");
              playWav1.play("W.wav");
              delay(delayTime);             
              Serial.print("IH");
              playWav1.play("IH.wav");  
          } else if ((txtMsg.charAt(i-1)=='q') && (txtMsg.charAt(i+1)=='o'))  { 
              i++; 
              Serial.print("W");
              playWav1.play("W.wav");
              delay(delayTime);               
              Serial.print("OW");
              playWav1.play("OW.wav");  
          } else if ((txtMsg.charAt(i+2)=='e') || (txtMsg.charAt(i+2)=='i') || (txtMsg.charAt(i+2)=='y')) { 
              Serial.print("UW");
              playWav1.play("UW.wav");
          } else if ((txtMsg.charAt(i+1)=='e') || ((txtMsg.charAt(i+1))=='i')){ 
              i++;
              Serial.print("Y");
              playWav1.play("Y.wav");  
              Serial.print("UW");
              playWav1.play("UW.wav");  
          } else {
              Serial.print("UH");
              playWav1.play("UH.wav");   
          }            
          delay(delayTime);           
        break;  

        case 'v':
          Serial.print("V");
          playWav1.play("V.wav");
          delay(delayTime);          
        break;   

        case 'w':
          if (txtMsg.charAt(i+1)=='h') {
            i++;
            Serial.print("WH");
            playWav1.play("WH.wav");
          } else {
          Serial.print("W");
          playWav1.play("W.wav");
          }
          delay(delayTime);          
        break;

        case 'x':
          Serial.print("K");
          playWav1.play("K.wav");
          delay(delayTime);  
          
          Serial.print("S");
          playWav1.play("S.wav");
          delay(delayTime);           
        break;

        case 'y':
          if (((txtMsg.charAt(i+1)==' ') || (txtMsg.charAt(i+1)=='/r')) && ((txtMsg.charAt(i-1)=='r') || (txtMsg.charAt(i-1)=='n'))) {
            Serial.print("IY");
            playWav1.play("IY.wav"); 
          } else if ((txtMsg.charAt(i-2)=='o') || (txtMsg.charAt(i-2)=='a') || (txtMsg.charAt(i-2)=='i') || (txtMsg.charAt(i-2)=='u') || (txtMsg.charAt(i-2)=='e')) {
            Serial.print("IY");
            playWav1.play("IY.wav"); 
          } else if ((txtMsg.charAt(i-1)=='l') || (txtMsg.charAt(i-1)=='r')) {
            Serial.print("AY");
            playWav1.play("AY.wav"); 
          } else {
            Serial.print("Y");
            playWav1.play("Y.wav");
          }
          delay(delayTime);           
        break;

        case 'z':
          Serial.print("Z");
          playWav1.play("Z.wav");
          delay(delayTime);            
        break;   

        case '\r':  // cariage return
          Serial.print(" ");
        break;    

        case ' ':  //space between words
          Serial.print(" ");
          delay(200);  //longer delay between words
        break;   

        default:
          Serial.println("Couldn't locate a matching CASE!");        
        break;               
      }
    }


  }
  txtMsg="";
  
  delay(delayTime); //delay between each sound
}
