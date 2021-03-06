// FB.Light Responsive LED Control
// https://github.com/bombcheck/FB.Light
//
// Forked from doctormord's Responsive Led Control
// https://github.com/doctormord/Responsive_LED_Control
//
// Free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, either version 3 of 
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


// ***************************************************************************
// Color modes
// ***************************************************************************
//#include "definitions.h"

char* listStatusJSON();

extern WebSocketsServer webSocket;

// These functions originally displayed the color using a call to FastLed.show()
// This has been refactored out, theser functions now simply render into the
// leds[] array. The FastLed.show() call happens in the main loop now.
// Furthermore, the 'add glitter' option also refactored out to the main loop.

void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds(random16(NUM_LEDS)) +=
        CRGB(settings.glitter_color.red, settings.glitter_color.green,
             settings.glitter_color.blue);
  }
}

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds[0], NUM_LEDS, gHue, 7);

  // if (settings.glitter_on == true){addGlitter(settings.glitter_density);}
  // frame has been created, now show it
  // FastLED.show();
  // insert a delay to keep the framerate modest
  // FastLED.delay(int(float(500/settings.fps)));
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds[0], NUM_LEDS, settings.ftb_speed);
  for (int x=0; x<settings.confetti_dens; x++) {
    int pos = random16(NUM_LEDS);
    leds(pos) += CHSV(gHue + random8(64), 200, settings.effect_brightness);
  }
  // if (settings.glitter_on == true){addGlitter(settings.glitter_density);}
  // frame has been created, now show it
  // FastLED.show();
  // insert a delay to keep the framerate modest
  // FastLED.delay(int(float(1000/settings.fps)));
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds[0], NUM_LEDS, settings.ftb_speed);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds(pos) += CHSV(gHue, 255, settings.effect_brightness);
  // if (settings.glitter_on == true){addGlitter(settings.glitter_density);}
  // frame has been created, now show it
  // FastLED.show();
  // insert a delay to keep the framerate modest
  // FastLED.delay(int(float(1000/settings.fps)));
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = settings.show_length;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, settings.effect_brightness);
  for (int i = 0; i < NUM_LEDS; i++) {  // 9948
    leds(i) = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  // if (settings.glitter_on == true){addGlitter(settings.glitter_density);}

  // frame has been created, now show it
  // FastLED.show();
  // insert a delay to keep the framerate modest
  // FastLED.delay(int(float(1000/settings.fps)));
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds[0], NUM_LEDS, settings.ftb_speed);
  int dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds(beatsin16(i + 7, 0, NUM_LEDS)) |=
        CHSV(dothue, 200, settings.effect_brightness);
    dothue += 32;
  }
  // if (settings.glitter_on == true){addGlitter(settings.glitter_density);}

  // frame has been created, now show it
  // FastLED.show();
  // insert a delay to keep the framerate modest
  // FastLED.delay(int(float(1000/settings.fps)));
}

void colorWipe() {
  static CRGB prevColor = CHSV(gHue, 255, settings.effect_brightness);
  static CRGB currentColor = CHSV(gHue+60, 255, settings.effect_brightness);

  // Wrap around if necessary
  if (wipePos >= NUM_LEDS) {
    wipePos = 0;
    prevColor = currentColor;
    gHue += 60;
    currentColor = CHSV(gHue, 255, settings.effect_brightness);
  }
  
  // Render the first half of the wipe
  for (int x=0; x<wipePos; x++) {
    leds(x) = currentColor;
  }
  // Render the second half
  for (int x=wipePos; x<NUM_LEDS; x++) {
    leds(x) = prevColor;
  }

  // Advance for next frame
  wipePos+=WIPE_SPEED;
}

//*****************LED RIPPLE*****************************************************

void one_color_allHSV(int ahue,
                      int abright) {  // SET ALL LEDS TO ONE COLOR (HSV)
  for (int i = 0; i < NUM_LEDS; i++) {
    leds(i) = CHSV(ahue, 255, abright);
  }
}

int wrap(int step) {
  if (step < 0) return NUM_LEDS + step;
  if (step > NUM_LEDS - 1) return step - NUM_LEDS;
  return step;
}

void ripple() {
  if (currentBg == nextBg) {
    nextBg = random(256);
  } else if (nextBg > currentBg) {
    currentBg++;
  } else {
    currentBg--;
  }
  for (uint16_t l = 0; l < NUM_LEDS; l++) {
    leds(l) = CHSV(currentBg, 255,
                   settings.effect_brightness);  // strip.setPixelColor(l,
                                                 // Wheel(currentBg, 0.1));
  }

  if (step == -1) {
    center = random(NUM_LEDS);
    color = random(256);
    step = 0;
  }

  if (step == 0) {
    leds(center) = CHSV(
        color, 255, settings.effect_brightness);  // strip.setPixelColor(center,
                                                  // Wheel(color, 1));
    step++;
  } else {
    if (step < maxSteps) {
      //Serial.println(pow(fadeRate, step));

      leds(wrap(center + step)) =
          CHSV(color, 255,
               pow(fadeRate, step) * 255);  //   strip.setPixelColor(wrap(center
                                            //   + step), Wheel(color,
                                            //   pow(fadeRate, step)));
      leds(wrap(center - step)) =
          CHSV(color, 255,
               pow(fadeRate, step) * 255);  //   strip.setPixelColor(wrap(center
                                            //   - step), Wheel(color,
                                            //   pow(fadeRate, step)));
      if (step > 3) {
        leds(wrap(center + step - 3)) =
            CHSV(color, 255, pow(fadeRate, step - 2) *
                                 255);  //   strip.setPixelColor(wrap(center +
                                        //   step - 3), Wheel(color,
                                        //   pow(fadeRate, step - 2)));
        leds(wrap(center - step + 3)) =
            CHSV(color, 255, pow(fadeRate, step - 2) *
                                 255);  //   strip.setPixelColor(wrap(center -
                                        //   step + 3), Wheel(color,
                                        //   pow(fadeRate, step - 2)));
      }
      step++;
    } else {
      step = -1;
    }
  }
  // if (settings.glitter_on == true){addGlitter(settings.glitter_density);}

  // frame has been created, now show it
  // FastLED.show();
  // insert a delay to keep the framerate modest
  // FastLED.delay(int(float(1000/settings.fps)));
}

//***************************END LED
//RIPPLE*****************************************************

// Simply fade all LEDs through all colors
void colorflow() {
  if (currentBg == nextBg) {
    nextBg = random(256);
  } else if (nextBg > currentBg) {
    currentBg++;
  } else {
    currentBg--;
  }
  for (uint16_t l = 0; l < NUM_LEDS; l++) {
    leds(l) = CHSV(currentBg, 255,
                   settings.effect_brightness);  // strip.setPixelColor(l,
                                                 // Wheel(currentBg, 0.1));
  }
}

void comet() {
  fadeToBlackBy(leds[0], NUM_LEDS, settings.ftb_speed);
  lead_dot = beatsin16(int(float(settings.fps / 3)), 0, NUM_LEDS);
  leds(lead_dot) = CHSV(dothue, 200, 255);
  dothue += 8;
  // if (settings.glitter_on == true){addGlitter(settings.glitter_density);}
  // FastLED.show();
}

// Theatre-style crawling lights.
void theaterChase() {
  static int8_t frame = 0;

  // turn off the previous frame's led
  for (int i = 0; i < NUM_LEDS; i = i + 3) {
    if (i + frame < NUM_LEDS) {
      leds(i + frame) = CRGB(0, 0, 0);  // turn every third pixel off
    }
  }

  // advance the frame
  frame++;
  if (frame > 2) frame = 0;

  // turn on the current frame's leds
  for (int i = 0; i < NUM_LEDS; i = i + 3) {
    if (i + frame < NUM_LEDS) {
      leds(i + frame) =
          CRGB(settings.main_color.red, settings.main_color.green,
               settings.main_color.blue);  // turn every third pixel on
    }
  }
}


//***********TV
int dipInterval = 10;
int darkTime = 250;
unsigned long currentDipTime;
unsigned long dipStartTime;
unsigned long currentMillis;
int ledState = LOW;
long previousMillis = 0; 
int ledBrightness[NUM_LEDS];
uint16_t ledHue[NUM_LEDS];
int led = 5;
int interval = 2000;
int twitch = 50;
int dipCount = 0;
int analogLevel = 100;
boolean timeToDip = false;

CRGB hsb2rgbAN1(uint16_t index, uint8_t sat, uint8_t bright) {
    // Source: https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
    uint8_t temp[5], n = (index >> 8) % 3;
    temp[0] = temp[3] = (uint8_t)((                                        (sat ^ 255)  * bright) / 255);
    temp[1] = temp[4] = (uint8_t)((((( (index & 255)        * sat) / 255) + (sat ^ 255)) * bright) / 255);
    temp[2] =          (uint8_t)(((((((index & 255) ^ 255) * sat) / 255) + (sat ^ 255)) * bright) / 255);

    return CRGB(temp[n + 2], temp[n + 1], temp[n]);
}

void _tvUpdateLed (int led, int brightness) {
  ledBrightness[led] = brightness;
  for (int i=0; i<NUM_LEDS; i++) {
    uint16_t index = (i%3 == 0) ? 400 : random(0,767);
    ledHue[led] = index;
  }
}

// See: http://forum.mysensors.org/topic/85/phoneytv-for-vera-is-here/13
void tv() {
  if (timeToDip == false) {
    currentMillis = millis();
    if (currentMillis-previousMillis > interval)  {
      previousMillis = currentMillis;
      interval = random(750,4001);//Adjusts the interval for more/less frequent random light changes
      twitch = random(40,100);// Twitch provides motion effect but can be a bit much if too high
      dipCount++;      
    }
    if (currentMillis-previousMillis<twitch) {
      led=random(0, NUM_LEDS-1);
      analogLevel=random(50,255);// set the range of the 3 pwm leds
      ledState = ledState == LOW ? HIGH: LOW; // if the LED is off turn it on and vice-versa:
      
      
      _tvUpdateLed(led, (ledState) ? 255 : 0);
      
      if (dipCount > dipInterval) { 
        //DBG_OUTPUT_PORT.println("dip");
        timeToDip = true;
        dipCount = 0;
        dipStartTime = millis();
        darkTime = random(50,150);
        dipInterval = random(5,250);// cycles of flicker
      }
    } 
  } else {
    //DBG_OUTPUT_PORT.println("Dip Time");
    currentDipTime = millis();
    if (currentDipTime - dipStartTime < darkTime) {
      for (int i=3; i<NUM_LEDS; i++) {
        _tvUpdateLed(i, 0);
      }
    } else {
      timeToDip = false;
    }
  }

  // Render the thing, with a little flicker  
  uint8_t flicker = 255;
  int sat = 200;

  EVERY_N_MILLISECONDS(150) {
    flicker = random(220,255);
    sat = random(180, 220);
  }
  
  for (int i=0; i<NUM_LEDS; i++) {
    uint16_t index = (i%3 == 0) ? 400 : random(0,767);
    //leds(i) = ((index >> 8) % 3, 200, ledBrightness[i]);
    
    leds(i) = hsb2rgbAN1(ledHue[i], sat, ledBrightness[i]).nscale8_video(flicker);
  }
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  80

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 60

bool gReverseDirection = false;

boolean _firerainbow = false; // used for rainbow mode


void fire2012()
{
  
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];
  static byte heat2[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      //heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2)); // original with COOLING
      heat[i] = qsub8( heat[i],  random8(0, ((settings.ftb_speed * 20) / NUM_LEDS) + 2)); // modified with FTBspeed
      heat2[i] = qsub8( heat2[i],  random8(0, ((settings.ftb_speed * 20) / NUM_LEDS) + 2)); // modified with FTBspeed
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
      heat2[k] = (heat2[k - 1] + heat2[k - 2] + heat2[k - 2] ) / 3;
    
    }
        
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    // if( random8() < SPARKING ) { // Original with SPARKING
    if( random8() < settings.show_length ) { // Modified with show_length
      int y = random8(7);
      int z = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
      heat2[z] = qadd8( heat2[z], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < (NUM_LEDS / 2); j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds(pixelnumber) = color;
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < (NUM_LEDS / 2); j++) {
      CRGB color = HeatColor( heat2[j]);
      int pixelnumber;
      pixelnumber = (NUM_LEDS-1) - j;
      leds(pixelnumber) = color;
    }

  if (_firerainbow) {
    for( int j = 0; j < (NUM_LEDS); j++) {
      leds(j) += CHSV(gHue, 255, settings.effect_brightness); // set effect brightness
    }
    _firerainbow = false;
  }

////frame has been created, now show it
//  FastLED.show();  
//  // insert a delay to keep the framerate modest
//  FastLED.delay(int(float(1000/FPS)));
    
}

void fire_rainbow() {
  _firerainbow = true;
  fire2012();
  
}


// Fireworks from WS2812FX

boolean _singlecolor = false; // used for single color mode
boolean _rainbow = false; // used for rainbow mode

void fireworks() {



// fadeToBlackBy( leds[0], NUM_LEDS, ftb_speed);

 
  uint32_t px_rgb = 0;
  byte px_r = 0;
  byte px_g = 0;
  byte px_b = 0;
  byte px_boost = 200;

  for(uint16_t i=0; i < NUM_LEDS; i++) {

    

    //leds(i) /= 2; // fade out (divide by 2)
    leds(i).nscale8(130 - int(float(settings.ftb_speed*0.5)));
    //leds(i).fadeToBlackBy(ftb_speed);

  }

  // first LED has only one neighbour
  leds(0).r = (leds(1).r >> 1) + leds(0).r;
  leds(0).g = (leds(1).g >> 1) + leds(0).g;
  leds(0).b = (leds(1).b >> 1) + leds(0).b;
  //leds(0).setRGB(px_r, px_g, px_b);

// set brightness(i) = ((brightness(i-1)/2 + brightness(i+1)) / 2) + brightness(i)
    for(uint16_t i=1; i < NUM_LEDS-1; i++) {
    leds(i).r = ((
            (leds(i-1).r  >> 1) +
            leds(i+1).r ) >> 1) +
            leds(i).r;

    leds(i).g = ((
            (leds(i-1).g >> 1) +
            leds(i+1).g ) >> 1) +
            leds(i).g;

    leds(i).b = ((
            (leds(i-1).b   >> 1) +
            leds(i+1).b ) >> 1) +
            leds(i).b;
  }

  // last LED has only one neighbour
  leds(NUM_LEDS-1).r = ((leds(NUM_LEDS-2).r >> 2) + leds(NUM_LEDS-1).r);
  leds(NUM_LEDS-1).g = ((leds(NUM_LEDS-2).g >> 2) + leds(NUM_LEDS-1).g);
  leds(NUM_LEDS-1).b = ((leds(NUM_LEDS-2).b >> 2) + leds(NUM_LEDS-1).b);

px_r = random8();
px_g = random8();
px_b = random8();


  
    for(uint16_t i=0; i<_max(1,NUM_LEDS/20); i++) {
      if(random8(settings.show_length + 4) == 0) {
        //Adafruit_NeoPixel::setPixelColor(random(_led_count), _mode_color);
        byte pixel = random(NUM_LEDS);
        if(_singlecolor){
          leds(pixel) = CRGB(settings.main_color.red,settings.main_color.green,settings.main_color.blue); // tails are in single color from set color on web interface
        } else if(_rainbow) {
          leds(pixel) = CHSV( gHue, 255, settings.effect_brightness); // Rainbow cycling color  
        } else if(!_singlecolor && !_rainbow) {
          leds(pixel).setRGB(px_r, px_g, px_b); // Multicolored tale
          
        }
        leds(pixel).maximizeBrightness();


        
      }
    }
  
    _singlecolor = false;
    _rainbow = false;

//  if (GLITTER_ON == true){addGlitter(glitter_density);}
// //frame has been created, now show it
  FastLED.show();  
  // insert a delay to keep the framerate modest
//  FastLED.delay(int(float(1000/FPS)));
}



void fw_single() {
  _singlecolor = true;
  fireworks();
}

void fw_rainbow() {
  _rainbow = true;
  fireworks();
}

// END

//*******************CALEIDOSCOPE**************************************

int XY(int x, int y) {
  if(y > MATRIX_HEIGHT) { y = MATRIX_HEIGHT; }
  if(y < 0) { y = 0; }
  if(x > MATRIX_WIDTH) { x = MATRIX_WIDTH;}
  if(x < 0) { x = 0; }
  if(x % 2 == 1) {  
  return (x * (MATRIX_WIDTH) + (MATRIX_HEIGHT - y -1));
  } else {
    // use that line only, if you have all rows beginning at the same side
    return (x * (MATRIX_WIDTH) + y);  
  }
}
 
// scale the brightness of the screenbuffer down
void DimmAll(byte value)  
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds(i).nscale8(value);
  }
}
 
/*
Caleidoscope1 mirrors from source to A, B and C
 
y
 
|       |
|   B   |   C
|_______________
|       |
|source |   A
|_______________ x
 
*/
void Caleidoscope1() {
  for(int x = 0; x < MATRIX_WIDTH / 2 ; x++) {
    for(int y = 0; y < MATRIX_HEIGHT / 2; y++) {
      leds(XY( MATRIX_WIDTH - 1 - x, y )) = leds(XY( x, y ));              // copy to A
      leds(XY( x, MATRIX_HEIGHT - 1 - y )) = leds(XY( x, y ));             // copy to B
      leds(XY( MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y )) = leds(XY( x, y )); // copy to C
     
    }
  }
}
 
/*
Caleidoscope2 rotates from source to A, B and C
 
y
 
|       |
|   C   |   B
|_______________
|       |
|source |   A
|_______________ x
 
*/
void Caleidoscope2() {
  for(int x = 0; x < MATRIX_WIDTH / 2 ; x++) {
    for(int y = 0; y < MATRIX_HEIGHT / 2; y++) {
      leds(XY( MATRIX_WIDTH - 1 - x, y )) = leds(XY( y, x ));    // rotate to A
      leds(XY( MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y )) = leds(XY( x, y ));    // rotate to B
      leds(XY( x, MATRIX_HEIGHT - 1 - y )) = leds(XY( y, x ));    // rotate to C
     
     
    }
  }
}
 
// adds the color of one quarter to the other 3
void Caleidoscope3() {
  for(int x = 0; x < MATRIX_WIDTH / 2 ; x++) {
    for(int y = 0; y < MATRIX_HEIGHT / 2; y++) {
      leds(XY( MATRIX_WIDTH - 1 - x, y )) += leds(XY( y, x ));    // rotate to A
      leds(XY( MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y )) += leds(XY( x, y ));    // rotate to B
      leds(XY( x, MATRIX_HEIGHT - 1 - y )) += leds(XY( y, x ));    // rotate to C
     
     
    }
  }
}
 
// add the complete buffer 3 times while rotating
void Caleidoscope4() {
  for(int x = 0; x < MATRIX_WIDTH ; x++) {
    for(int y = 0; y < MATRIX_HEIGHT ; y++) {
      buffer(XY( MATRIX_WIDTH - 1 - x, y )) += buffer(XY( y, x ));    // rotate to A
      buffer(XY( MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y )) += buffer(XY( x, y ));    // rotate to B
      buffer(XY( x, MATRIX_HEIGHT - 1 - y )) += buffer(XY( y, x ));    // rotate to C
     
     
    }
  }
}
 
void ShowBuffer() {
  for(int i = 0; i < NUM_LEDS ; i++) {
    leds(i) += buffer(i);
  }
}
 
void ClearBuffer() {
  for(int i = 0; i < NUM_LEDS ; i++) {
    buffer(i) = 0;
  }
}
 
void Spiral(int x,int y, int r, byte dimm) {  
  for(int d = r; d >= 0; d--) {                // from the outside to the inside
    for(int i = x-d; i <= x+d; i++) {
       leds(XY(i,y-d)) += leds(XY(i+1,y-d));   // lowest row to the right
       leds(XY(i,y-d)).nscale8( dimm );}
    for(int i = y-d; i <= y+d; i++) {
       leds(XY(x+d,i)) += leds(XY(x+d,i+1));   // right colum up
       leds(XY(x+d,i)).nscale8( dimm );}
    for(int i = x+d; i >= x-d; i--) {
       leds(XY(i,y+d)) += leds(XY(i-1,y+d));   // upper row to the left
       leds(XY(i,y+d)).nscale8( dimm );}
    for(int i = y+d; i >= y-d; i--) {
       leds(XY(x-d,i)) += leds(XY(x-d,i-1));   // left colum down
       leds(XY(x-d,i)).nscale8( dimm );}
  }
}

void caleidoscope(uint8_t calmode)
{
  calcount=calcount+5;
   
  // first plant the seed into the buffer
  buffer(XY(sin8(calcount)/17, cos8(calcount)/17)) = CHSV (160 , 255, 255); // the circle  
  buffer(XY(quadwave8(calcount)/17, 4)) = CHSV (0 , 255, 255); // lines following different wave fonctions
  buffer(XY(cubicwave8(calcount)/17, 6)) = CHSV (40 , 255, 255);
  buffer(XY(triwave8(calcount)/17, 8)) = CHSV (80 , 255, 255);
 
  // duplicate the seed in the buffer
  if (calmode == 1) Caleidoscope1();
  else if (calmode == 2) Caleidoscope2();
  else if (calmode == 3) Caleidoscope3();
  else if (calmode == 4) Caleidoscope4();
  else Caleidoscope4();
 
  // add buffer to leds
  ShowBuffer();
 
  // clear buffer
  ClearBuffer();
 
  // rotate leds
  Spiral(7,7,8,110);
 
  // do not delete the current leds, just fade them down for the tail effect
  //DimmAll(220);
}

//*******************END CALEIDOSCOPE**********************************

//*******************************ARRAY OF SHOW ANIMATIONS FOR MIXED SHOW
//MODE***********************
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, confetti, sinelon, juggle,
                               bpm, ripple, comet};
//**************************************************************************************************
