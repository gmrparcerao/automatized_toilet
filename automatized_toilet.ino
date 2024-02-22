/*---------------Header----------------------------
    Sao Paulo State Itaquera Technology College "Professor Miguel Reale"
    Sao Paulo, 12/2020
    Associate of Science - Industrial Automation

    Names of postgraduate students:
      Gabriel Liobino Sampaio
      Guilherme Matheus Rafael Parcerão

    Orienter:
      Fernando Luis de Almeida

    Goals: 
      Embedded system to automatize a public toilet allowing the user to use the toilet without the need of having physical contact with potential virus-infected furniture

    Hardware:
      NodeMCU-32S
      Stepper motor
      Solenoid valve
      LDR
      Magnetic Sensor
      Termistor

    Reviews: 
      R000 - begin

    https://espressif.github.io/arduino-esp32/package_esp32_index.json

*/

#include <AccelStepper.h>

#define sensor_torneira 35
#define torneira 33
#define ldr 26
#define magnetico 13
#define pinTermistor0 34
#define descarga 4

int velocidade_motor = 100; 
int aceleracao_motor = 100;
int sentido_horario = 0;
int sentido_antihorario = 0;
int numero = 0;
 
// Definicao pino ENABLE
#define pino_enable 27

// Definicao pinos STEP e DIR
AccelStepper motor1(1,12,21 );

int sensor_atual = 0;
int pinTermistor = 0;
int LDR = 0;
int MAG =0;

bool verif = 0;
bool ant = 0;
bool aberto=0;

// Parâmetros do termistor
const double beta = 3600.0;
const double r0 = 10000.0;
const double t0 = 273.0 + 25.0;
const double rx = r0 * exp(-beta/t0);
 
// Parâmetros do circuito
const double vcc = 3.5;
const double R = 10000.0;
 
// Numero de amostras na leitura
const int nAmostras = 5;

// millis
unsigned long millisTarefa1 = millis();
unsigned long now = millis();

void setup() {

	Serial.begin(115200);
	pinMode(torneira, OUTPUT);
	pinMode(descarga, OUTPUT);
	pinMode(pino_enable, OUTPUT);
  	
	// Configuracoes iniciais motor de passo
	motor1.setMaxSpeed(velocidade_motor);
	motor1.setSpeed(velocidade_motor);
	motor1.setAcceleration(aceleracao_motor);
}

void loop() {

	LDR = analogRead(ldr);
	MAG = analogRead(magnetico);
	sensor_atual = analogRead(sensor_torneira);
  
  if(sensor_atual <= 1000){

  	digitalWrite(torneira, HIGH);
  	delay(5000);
  	digitalWrite(torneira, LOW);

  }
  
  else{

   	digitalWrite(torneira, LOW);

  }

  int soma = 0;
  
  for (int i = 0; i < nAmostras; i++) {

  	pinTermistor = analogRead(pinTermistor0);
  	soma += pinTermistor;
  	delay (10);

  }
 
  // Determina a resistência do termistor
  double v = (vcc*soma)/(nAmostras*4096.0);
  double rt = (vcc*R)/v - R;
 
  // Calcula a temperatura
  double t = beta / log(rt/rx);
  
  //motor tampa
  if(LDR >= 1000 && MAG >= 1000 && aberto==0){

    digitalWrite(pino_enable, LOW);
    sentido_horario = 1;
        sentido_antihorario = 0;
        aberto =1;
    }

  if(LDR >= 1000 && MAG < 1000 && aberto==1){

    	digitalWrite(pino_enable, LOW);
    	sentido_horario = 0;
    	sentido_antihorario = 1;
    	aberto =0;

  }

 // Move o motor no sentido horario
 if (sentido_horario == 1){

    	motor1.moveTo(10000);
    
	if (ant==0){
    		
		millisTarefa1 = millis();
    		ant=1;

  	}

  }

  // Move o motor no sentido anti-horario
  if (sentido_antihorario == 1){

    	motor1.moveTo(-10000);
    	if (ant==0){

    		millisTarefa1 = millis();
    		ant=1; 

  	}

  }

  now = millis();

  if(sentido_antihorario == 1 || sentido_horario == 1){

  	if((now - millisTarefa1) > 2000){

      		if(sentido_antihorario == 1)
     		{verif =1;}
      		sentido_horario = 0;
      		sentido_antihorario = 0;
      		motor1.moveTo(0);
      		digitalWrite(pino_enable, HIGH);
      		ant=0;

      
   	 }

   }

  // Comando para acionar o motor no sentido especificado
  motor1.run();
  Serial.print("now= ");
  Serial.println(now);
  Serial.println(millisTarefa1);


//descarga
if(verif == 1 && (t-273.0) < 27){

     	digitalWrite(descarga, HIGH);
     	delay(6000);
     	digitalWrite(descarga, LOW);
     	verif = 0;

  }

  if(verif == 1 && (t-273.0) > 27){

     	digitalWrite(descarga, HIGH);
     	delay(3000);
     	digitalWrite(descarga, LOW);
     	verif = 0;

  }
  
}
