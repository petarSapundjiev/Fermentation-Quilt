
/*
Fermentation_QUILT.ino

This code is intended for the project "Fermentation Quilt"
by Victoria Manganiello & Petar Sapundjiev.

Written: Sep 15 2020.
by Petar Sapundjiev. 
For any additional information regarding the project you can contact us on:
p.v.sapundjiev@gmail.com
vkmanganiello@gmail.com

*/


#define TMP36_VCC      A3         // the PWM pin the LED is attached to
#define TMP_TRESH      45         // 45 degrees Celsius is the FERMENTING TEMPERATURE the regulator will maintan
#define TMP_HYST       1          // 1 degree Celsius is the HYSTERESYS for turning ON and OFF the heaters
#define FERM_PERIOD    28800000   // defines the period after which the regulator will stop the heating process. 28 800 000 is the number of milliseconds in 8 hours.

int heater            = 10;         // the PWM pin the LOOMIA heatrs are attached to
int led_r             = 13;         // the PWM pin the LED is attached to

int brightness        = 30;         // how bright the LED is
int fade_amount       = 1;          // how many points to fade the LED by
int glow_period       = 2000;       // sets the number of "loop" periods for led glow increment
int tmp_sample_period = 5000;       // sets the number of "loop" periods for taking one ADC sample period
int bright_tresh_low  = 10;         // defines lowest brightness during glow
int filt_avg_samples  = 8;          // defines number of averaged sample in filter


float filt_mean       = 0;
float filt_diff       = 0;


void setup() { 
  pinMode(led_r, OUTPUT);             // declare pin 9 to be an output
  pinMode(heater, OUTPUT);             // declare pin 10 to be an output
  pinMode(TMP36_VCC, OUTPUT);          // declare pin 11 to be an output
  pinMode( A2, INPUT );               // sets A1 as input

  digitalWrite( TMP36_VCC, HIGH );    //sets pin A3 HIGH. The pin is used for TMP36 power supply
  Serial.begin(115200);
}

void loop() {

  int   adc_res_1         = 0;
  float temp_1            = 0;
  int   period_A_counter  = 0;        // counts number of "loop" periods for led glow increment
  int   period_B_counter  = 0;        // counts number of "loop" periods for taking one temperature sample

  do{                   //****** loops for 8 hours*********     
    period_A_counter++;
    period_B_counter++;

  if( period_B_counter > tmp_sample_period ){
    period_B_counter = 0;
    adc_res_1 = analogRead( A2 );
    temp_1 = adc_res_1 * 0.32307 - 50;         // ADC value to temperature conversion. 10bit and 3.3V ADC V_ref

/*__________________ filter____________________________________*/    
    filt_diff = (float)temp_1 - filt_mean;
    filt_mean += ( filt_diff / filt_avg_samples );
/*__________________ filter - end______________________________*/    
/*__________________ temperature control_______________________*/    
    if( filt_mean > ( TMP_TRESH + TMP_HYST )){
      digitalWrite( heater, LOW );      
    }else{
      if( filt_mean < ( TMP_TRESH - TMP_HYST )){
        digitalWrite( heater, HIGH );
      }
    }
/*__________________ temperature control - end__________________*/ 
  }
  if ( period_A_counter > glow_period ){
    period_A_counter = 0;
    analogWrite(led_r, brightness);                                    // sets the brightness of LED on pin 13  
    brightness += fade_amount;                                         // change the brightness for next time through the loop  
    if ( brightness < bright_tresh_low || brightness >= 255 ){         // reverse the direction of the fading at the ends of the fade
    fade_amount = -fade_amount;
    }
  }
//**********************************************************************

  }while( millis() < FERM_PERIOD );        // 8 hours after the Fermentation Quilt has been torned ON, the process will stop
  digitalWrite( heater, LOW );             // heaters will be turned OFF
  analogWrite(led_r, 255);                 // LED on pin 13 will go solid red
  
}   //end of "loop"
