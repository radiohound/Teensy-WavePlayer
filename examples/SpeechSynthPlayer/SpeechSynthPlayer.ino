//SpeechSynthPlayer.ino
//This code produces speech output to I2S when the correct phonemes are played in the correct order.

//Parts of Frank Boesing's WaveFilePlayerI2SSampleRate code was used to play 22050 htz wav files. 
//phoneme names format from SAM TTS manual. This manual has a helpful word dictionary
//however, it includes numbers for pronounciation that I have not implemented here
//http://www.retrobits.net/atari/sam.shtml

//The phoneme sounds files need to be copied to a SDCARD in the builtin SDCard on a Teensy 4.1 or 3.6 board, 
//with a Teensy Audio Shield Rev D for 4.1 or previous Rev for a Teensy 3.6

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <play_wav.h>

#if defined(__IMXRT1062__)
#define T4
#include <utility/imxrt_hw.h> // make available set_audioClock() for setting I2S freq on Teensy 4
#else
#define F_I2S ((((I2S0_MCR >> 24) & 0x03) == 3) ? F_PLL : F_CPU) // calculation for I2S freq on Teensy 3
#endif

// GUItool: begin automatically generated code
AudioPlayWav             playWav1;     //xy=210,161
AudioOutputI2S           outputsound;       //xy=417,124
//AudioOutputUSB           usb1;           //xy=402,185
AudioConnection          patchCord1(playWav1, 0, outputsound, 0);
//AudioConnection          patchCord3(playWav1, 0, usb1, 0);
//AudioConnection          patchCord4(playWav1, 0, usb1, 1);
AudioConnection          patchCord2(playWav1, 0, outputsound, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=413,177
// GUItool: end automatically generated code


//These phonemes were all generated from espeak-ng using the wav recording feature:
// espeak-ng -w AA.wav -ven-us [[o]]. (More info in the pdf file located in docs)
// in espeak-ng the phoneme that matches SAM's AA.wav is the phoneme o, AW is aU, OW is oU, UW is u etc..
// To do list: make chart for Sam to espeak alophone conversion
// Some of the phonemes here had to be cut from example words in order for them to match the correct sound. 
// This is because for some phonemes, espeak-ng needs to know the context of the letter, in order to know how to 
// pronounce it. SAM phonemes are a much reduced list of phonemes than espeak. 
// Some of these wav files may need to be re-recorded, trimmed/extended, and or volume increased/decreased
// to improve the sound. But this seems to produce promising results, and results that can be manipulated by the 
// sound library.  


void setup() {
  Serial.begin(9600);
  AudioMemory(50);
  delay(500);
  if (CrashReport) {
    pinMode(13, OUTPUT);
    digitalWriteFast(13, 1);
    Serial.println(CrashReport);
    CrashReport.clear();
    delay(30000);
  }

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  
  if (!(SD.begin(BUILTIN_SDCARD))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  
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
      //Serial.printf("%fHz<->%fHz(%d/%d) Fehler:%f\n", fsamp, freq, fract, divi, minfehler);
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
  setI2SFreq(22050); //wav files at 22050 htz
  delay(500);
  // Play phonemes to create words
  setI2SFreq(22050);
  playWav1.play("T.wav");
  delay(75);
  playWav1.play("IY.wav");
  delay(75);
  playWav1.play("N.wav");
  delay(75);
  playWav1.play("S.wav");
  delay(75);
  playWav1.play("IY.wav");
  delay(300);  
  playWav1.play("K.wav");
  delay(75);
  playWav1.play("AE.wav");
  delay(75);
  playWav1.play("N.wav");
  delay(300);
  playWav1.play("S.wav");
  delay(75);
  playWav1.play("P.wav");
  delay(75);
  playWav1.play("IY.wav");
  delay(75);
  playWav1.play("K.wav");
  delay(5000);
}
