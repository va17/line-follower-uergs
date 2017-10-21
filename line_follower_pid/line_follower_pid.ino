#include <AFMotor.h>
 
AF_DCMotor motor_esq(1); //Seleciona o motor 1
AF_DCMotor motor_dir(4); //Seleciona o motor 4

#define Kp 0.3 // 0.06 // 0.3
#define Kd 4.5 // 0.075 Kp < Kd
#define Ki 0 // TBD
#define rightMaxSpeed 96//96 // max speed
#define leftMaxSpeed 70//70 // max speed
#define rightBaseSpeed 66 // straight line speed
#define leftBaseSpeed 40  // straight line speed
 
int SENSOR1, SENSOR2, SENSOR3;
 
//deslocamentos de calibracao
int leftOffset = 0, rightOffset = 0, centre = 0;
//limiar do sensor
int threshold = 200;

int lastError = 0;
 
//Rotina de calibracao do sensor
void calibrate()
{
 for (int x=0; x<10; x++) //Executa 10 vezes para obter uma media
 {
   delay(100);
   SENSOR1 = analogRead(0);
   SENSOR2 = analogRead(1);
   SENSOR3 = analogRead(2);
   leftOffset = leftOffset + SENSOR1;
   centre = centre + SENSOR2;
   rightOffset = rightOffset + SENSOR3;
   delay(100);
 }
 //obtem a media para cada sensor
 leftOffset = leftOffset /10;
 rightOffset = rightOffset /10;
 centre = centre / 10;
 //calcula os deslocamentos para os sensores esquerdo e direito
 leftOffset = centre - leftOffset;
 rightOffset = centre - rightOffset;
}
 
void setup()
{
  calibrate();
  delay(3000);
  Serial.begin(9600);
  send_speed_to_motor(rightMaxSpeed, leftMaxSpeed);
  delay(100);
}

void loop()
{
  //le os sensores e adiciona os deslocamentos
  SENSOR1 = analogRead(0) + leftOffset;
  SENSOR2 = analogRead(1);
  SENSOR3 = analogRead(2) + rightOffset;

  int position = convert_sensor_to_position();
  
  int error = position - 1000;

  int motorSpeed = Kp * error + Kd * (error - lastError);
  lastError = error;
  
  int rightMotorSpeed = rightBaseSpeed + motorSpeed;
  int leftMotorSpeed = leftBaseSpeed - motorSpeed;

  if (rightMotorSpeed > rightMaxSpeed ) rightMotorSpeed = rightMaxSpeed;
  if (leftMotorSpeed > leftMaxSpeed ) leftMotorSpeed = leftMaxSpeed;
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;
 
  //Envia os valores de velocidade para os motores
  //print_motor_speed(rightMotorSpeed, leftMotorSpeed);
  send_speed_to_motor(rightMotorSpeed, leftMotorSpeed);
  //delay(5);
}

unsigned long convert_sensor_to_position(){
  //print_sensors_readings();
  // normalizar valores;
  //S1 = (unsigned long)(SENSOR1*1000/1023);
  //S2 = (unsigned long)(SENSOR2*1000/1023);
  //S3 = (unsigned long)(SENSOR3*1000/1023);
  unsigned long T = (SENSOR1 + SENSOR2 + SENSOR3);
  unsigned long D = (unsigned long)(0*SENSOR1) + (unsigned long)((unsigned long)1000*SENSOR2) + (unsigned long)((unsigned long)2000*SENSOR3);
  unsigned long position = (D)/T;
  return position;
}

void print_sensors_readings(){
  Serial.print("Sensor 1 ");
  Serial.print(SENSOR1);
  Serial.print(" | Sensor 2 ");
  Serial.print(SENSOR2);
  Serial.print(" | Sensor 3 ");
  Serial.println(SENSOR3);
}

void print_motor_speed(int rightMotorSpeed, int leftMotorSpeed){
  Serial.print("Right motor speed: ");
  Serial.print(rightMotorSpeed);
  Serial.print(" | Left motor speed: ");
  Serial.println(leftMotorSpeed);
}

void send_speed_to_motor(int rightMotorSpeed, int leftMotorSpeed){
  motor_esq.setSpeed(leftMotorSpeed);
  motor_esq.run(FORWARD);
  motor_dir.setSpeed(rightMotorSpeed);
  motor_dir.run(FORWARD);
}


