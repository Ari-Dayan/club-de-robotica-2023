#include <QTRSensors.h>




#define NUM_SENSORS             7  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             13  // emitter is controlled by digital pin 2

// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {A4, A2, A5, A1, A6, A0, A3}, 
  NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
//________________________________________________________________________________________________________________________________________________________________________________________

//////pines del driver//////

int pwm_a = 5;
int pwm_b = 10;



int ain_1 = 6;
int ain_2 = 7;

int bin_2 = 8;
int bin_1 = 9;
int potencia_a;
int potencia_b;

//________________________________________________________________________________________________________________________________________________________________________________________

//////defino los botones de la placa//////

//________________________________________________________________________________________________________________________________________________________________________________________
////// pines inicializacion //////
#define BTN 12

int est_btn;
#define pin_Led 3
//________________________________________________________________________________________________________________________________________________________________________________________

//////defino los leds de la placa//////
#define LED_1 4


//_______________________________________________________________________________________________________________________________________________________________________________________________________
//////defino las variables del control PID//////

int P = 0; //defino el error
int I = 0; // integral
int D = 0; //derivativo
int LAST = 0; //el error anterior
float vel; //defino la velocidad
//velocidad para la line recta
int recto_a = 80; 
int recto_b = 80; 
//_______________________________________________________________________________________________________________________________________________________________________________________________________

void setup()
{
  pinMode(LED_1, OUTPUT);

  pinMode (bin_2, OUTPUT);
  pinMode (bin_1, OUTPUT);
  pinMode (ain_2, OUTPUT);
  pinMode (ain_1, OUTPUT);

  pinMode(BTN, INPUT_PULLUP);
  pinMode(pin_Led, OUTPUT);

  //_______________________________________________________________________________________________________________________________________________________________________________________________________
  //////calibracion//////
  
  delay(500);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode
  for (int i = 0; i < 200; i++)  // make the calibration take about 10 seconds
  {
    qtra.calibrate();       // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
  }
  digitalWrite(13, LOW);     // turn off Arduino's LED to indicate we are through with calibration

  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  
  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
//_______________________________________________________________________________________________________________________________________________________________________________________________________
  ////// configuro el pwm//////
  
  digitalWrite(ain_1, HIGH);
  digitalWrite(ain_2, LOW);
  // se pone el high y low en los pines inversos, ya que cada motor apúnta a lados opuestos
  digitalWrite(bin_1, LOW);
  digitalWrite(bin_2, HIGH);

  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);

//_______________________________________________________________________________________________________________________________________________________________________________________________________

 est_btn = digitalRead(BTN);
while(est_btn==1){
  est_btn = digitalRead(BTN);
  }
  while(est_btn==0){
  est_btn = digitalRead(BTN);
  }
}
void loop(){
 
  unsigned int position = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);// 0 para pista blanca con linea negra, y 1 al revez
 

  //Serial.println(position); // comment this line out if you are using raw values
  
 //_______________________________________________________________________________________________________________________________________________________________________________________________________

    P = (position - 3000);// el error
    //_______________________________________________________________________________________________________________________________________________________________________________________________________

    D =P - LAST; // constante derivativa
    //_______________________________________________________________________________________________________________________________________________________________________________________________________
    I = (P + LAST); // constante integrativa
    //_______________________________________________________________________________________________________________________________________________________________________________________________________

    ////// formula del contol PID//////

    vel = (P * 0.018) + (D * 0.01) + (I * 0.0 );
//    vel = (P * 0.018) + (D * 0.01) + (I * 0.0 ); con 130
//    vel = (P * 0.029) + (D * 0.02) + (I * 0.0 );   170
//    vel = (P * 0.034 o 5) + (D * 0.027) + (I * 0.0 ); 190
//    vel = (P * 0.0405) + (D * 0.03) + (I * 0.0 ); 210


      // a partir de ahora es con aceleracion lenta de 5 en 5

          //vel = (P * 0.041) + (D * 0.0328) + (I * 0.005 ); 220
          //    vel = (P * 0.04) + (D * 0.0335) + (I * 0.01 ); 230
          //     vel = (P * 0.045) + (D * 0.0335) + (I * 0.01 ); 235
          //     vel = (P * 0.049) + (D * 0.0339) + (I * 0.01 ); 240
          //     vel = (P * 0.0525) + (D * 0.0353) + (I * 0.01 ); 245
          //     vel = (P * 0.0525) + (D * 0.0353) + (I * 0.01 ); 255

//Serial.println("corriendo");

    potencia_a = recto_a + vel ;
    potencia_b = recto_b - vel ;

    if (recto_a< 130){
      recto_a = recto_a +5;
      recto_b = recto_b +5;
      }

    if(potencia_a>255){
      potencia_a = 255;
    }
    else if(potencia_a < 0){
      potencia_a = 0;
    }
    
    if(potencia_b>255){
      potencia_b = 255;
    }
    else if(potencia_b < 0){
      potencia_b = 0;
    }

    analogWrite(pwm_a, potencia_a);
    analogWrite(pwm_b, potencia_b);
    LAST = P;
}
