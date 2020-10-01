#include <AFMotor.h>

// ----------------------- Constantes

AF_DCMotor motor_esq(1);       // Seleciona o motor 1
AF_DCMotor motor_dir(4);       // Seleciona o motor 4

#define Kp 0.3                 // 0.06 // 0.3
#define Kd 4.5                 // 0.075 Kp < Kd
#define Ki 3.1                 //
#define rightMaxSpeed 96//96   // Velocidade máxima (roda direita)
#define leftMaxSpeed 70//70    // Velocidade máxima (roda esquerda)
#define rightBaseSpeed 66      // Velocidade para andar em linha reta (roda direita)
#define leftBaseSpeed 40       // Velocidade para andar em linha reta (roda esquerda)

// ----------------------- Inicialização de variáveis globais
int SENSOR1, SENSOR2, SENSOR3;

int leftOffset = 0, rightOffset = 0, centre = 0; // Deslocamentos de calibração
int threshold = 200;                             // Limiar do sensor
int lastError = 0;                               // Último erro
int I = 0;                                       // Inicializa I
unsigned long time;                              // Variável para controlar o tempo corrente
unsigned long lastTime = 0;                      // Inicializa variável que armazena o tempo da última leitura

// ----------------------- SETUP
void setup()
{
  calibrate(); // Execute rotina de calibração
  delay(3000);
  Serial.begin(9600);
  send_speed_to_motor(rightMaxSpeed, leftMaxSpeed);
  delay(100);
}

// ----------------------- MAIN LOOP
void loop()
{
  // Lê os sensores e adiciona os deslocamentos (offsets)
  SENSOR1 = analogRead(0) + leftOffset;
  SENSOR2 = analogRead(1);
  SENSOR3 = analogRead(2) + rightOffset;

  int position = convert_sensor_to_position();        // Lê posição atual (1000 = centro da fita)
  
  int error = position - 1000;                        // Subtrai 1000 do erro para que o mesmo seja deslocado para 0

  time = millis();                                    // millis() retorna o temp atual em uma timestamp (epoch)
  int E = error - lastError;                          // Calcula diferença entre erro atual e anterior
  if(lastTime == 0) lastTime = time;                  // Se lastTime = 0 significa que não houve uma leitura ainda, logo, ultimo_tempo = tempo_atual
  int deltaTime = time - lastTime;                    // Calcula variação de tempo (delta)
  lastTime = time;                                    // Armazena tempo atual em lastTime para ser utilizado no próximo ciclo

  int P = error;                                      // Componente Proporcional
  int I = I + (E*deltaTime);//I + error;              // Componente Integral
  int D = (error - lastError);                        // Componente Derivativa
  int motorSpeed = (Kp * P) + (Kd * D) + (Ki * I);    // Velocidade do motor = P + I + D
  lastError = error;                                  // Armazena o erro para ser utilizado como erro anterior no próximo ciclo
  
  int rightMotorSpeed = rightBaseSpeed + motorSpeed;  // Velocidade do motor direito = velocidade base + velocidade cálculada
  int leftMotorSpeed = leftBaseSpeed - motorSpeed;    // Velocidade do motor esquero = velocidade base - velocidade cálculada

  // Condicionais para impedir que os motores passem das velocidades máximas definidas e também não recebam velocidades < 0
  if (rightMotorSpeed > rightMaxSpeed ) rightMotorSpeed = rightMaxSpeed;
  if (leftMotorSpeed > leftMaxSpeed ) leftMotorSpeed = leftMaxSpeed;
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;
 
  // Envia os valores de velocidade para os motores
  //print_motor_speed(rightMotorSpeed, leftMotorSpeed);
  send_speed_to_motor(rightMotorSpeed, leftMotorSpeed);
  //delay(5);
}

//Rotina de calibracao do sensor
void calibrate()
{
 // Rotina executada 10 vezes para se obter uma média
 for (int x=0; x<10; x++)
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
 // Calcula a média de cada sensor
 leftOffset = leftOffset /10;
 rightOffset = rightOffset /10;
 centre = centre / 10;
 // Calcula os deslocamentos para os sensores esquerdo e direito a partir do sensor central
 leftOffset = centre - leftOffset;
 rightOffset = centre - rightOffset;
}

// Função que converte a leitura dos sensores (0 - 1023) para um valor de posição ~(0-1000)
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

// Função auxiliar para debugar a leitura dos sensores através do monitor serial
void print_sensors_readings(){
  Serial.print("Sensor 1 ");
  Serial.print(SENSOR1);
  Serial.print(" | Sensor 2 ");
  Serial.print(SENSOR2);
  Serial.print(" | Sensor 3 ");
  Serial.println(SENSOR3);
}

// Função auxiliar para debugar velocidade dos motores através do monitor serial
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


