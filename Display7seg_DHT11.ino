#include <DHT.h>    // importa la Librerias DHT
#include <DHT_U.h>
#include <TM1637Display.h>    // incluye libreria TM1637

#define CLK 4       // CLK a pin 2
#define DIO 5       // DIO a pin 3

TM1637Display display(CLK, DIO);  // crea objeto
const uint8_t alto[] = {    // array para almacenar caracteres especiales
  SEG_F | SEG_G | SEG_B | SEG_F | SEG_E | SEG_C,    // H
  SEG_B | SEG_C      // I
};

int SENSOR = 6;     // pin DATA de DHT11 a pin digital 6
int TEMPERATURA;
int HUMEDAD;
int PULSADOR1 = 9;
int PULSADOR2 = 10;
int LED = 2;
int alarma = 24;
int muestra_temp1;
int muestra_temp2;
int muestra_temp3;
int temp_final;
bool flag1 = true;
bool flag2 = true;
bool flag3 = false;
unsigned long tiempo_ant_millis;
unsigned long tiempo_act_millis;
int Tpo_espera_on = 0;
int Tpo_espera_off = 0;
int hysteresis = 2; // Hysteresis para sumar/restar a set alarma

DHT dht(SENSOR, DHT11);   // creacion del objeto, cambiar segundo parametro
        // por DHT11 si se utiliza en lugar del DHT22
void setup(){
  Serial.begin(9600);   // inicializacion de monitor serial
  dht.begin();      // inicializacion de sensor
  display.setBrightness(1);   // establece nivel de brillo
  Serial.println("******* IR caloventor Baño *******");
  Serial.println(F("START " __FILE__ " from " __DATE__ ));

  pinMode(PULSADOR1, INPUT);
  pinMode(PULSADOR2, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  display.showNumberDec(temp_final, false, 2, 0); // muestra valor temp_final
  display.showNumberDec(alarma,false, 2, 2);   // muestra set alarma


}

//************* LOOP principal ***********
void loop()
{
  if (digitalRead(PULSADOR1) == HIGH)  // evaluo si la entrada esta en nivel alto
{
    while (digitalRead(PULSADOR1) == HIGH) //retraso antirebote
     {
       delay(10);
     }
    //digitalWrite(LED,HIGH); // pulsador presionado, encender LED
    alarma++;   //aumento set alarma de temperatura
    display.showNumberDec(alarma,false, 2, 2);   // muestra set alarma
    delay(50);
    //digitalWrite(LED,LOW);  // pulsador no presionado, apagar LED

}

 if (digitalRead(PULSADOR2) == HIGH)  // evaluo si la entrada esta en nivel alto
{
  while (digitalRead(PULSADOR2) == HIGH) //retraso antirebote
   {
    delay(10);
   }
    //digitalWrite(LED,HIGH); // pulsador presionado, encender LED
    alarma--;  //bajo set alarma de temperatura
    display.showNumberDec(alarma,false, 2, 2);   // muestra set alarma
    delay(50);
    //digitalWrite(LED,LOW);  // pulsador no presionado, apagar LED
}

 //Summ
 tiempo_act_millis = millis();
if ( tiempo_act_millis - tiempo_ant_millis >= 3000)
  {

    muestra_temperatura(); // Ir a muestrear
    toma_muestras(); //Va a tomar muestras de temperatura
    tiempo_ant_millis = tiempo_act_millis;//delay(10);
  }

} //*********** Fin LOOP ***************

// *********** Tomar temperatura ***************
void muestra_temperatura()
{
  if (flag1==true && flag2 == true && flag3 == true)
  {
      temp_final = (muestra_temp1+muestra_temp2+muestra_temp3) / 3;
      flag1 = true;
      flag2 = true;
      flag3 = false;

      display.showNumberDec(temp_final, false, 2, 0); // muestra valor temp_final

      Serial.print("Temperatura: ");  // escritura en monitor serial de los valores
      Serial.print(TEMPERATURA);
      Serial.print(" Temperatura final: ");
      Serial.print(temp_final);
      Serial.print(" Humedad: ");
      Serial.println(HUMEDAD);
      Serial.println("M1: "+String(muestra_temp1) +" M2: "+String(muestra_temp2)+" M3: "+String(muestra_temp3)+" FINAL: "+String(temp_final));
      Serial.println("*********************************** ");
      //Serial.println(" ");

//******     Rutina para apagar caloventor  *************
      if (temp_final >= alarma + hysteresis)
        {
          Tpo_espera_off++;  //si esta alta suma 1 a la espera para apagar
          Tpo_espera_on = 0;
          if (Tpo_espera_off >=2) // si pasaron 2 seg muestra HI
           {
            Serial.println("Apagado " + String(Tpo_espera_off));
            digitalWrite(LED,LOW); // Caloventor apagado
            display.setSegments(alto, 2, 2); // muestra set alarma
           }
        }

//******    Rutina para encender caloventor  ************
      if ( temp_final < alarma )
        {
          Tpo_espera_on++;
          Tpo_espera_off = 0;
          if (Tpo_espera_on >= 2)
          {
            Serial.println("Encendido " + String(Tpo_espera_on));
            digitalWrite(LED,HIGH); // pulsador presionado, encender LED
            display.showNumberDec(alarma,false, 2, 2);   // muestra set alarma
          }
        }
  }
}

void toma_muestras()
{
    Serial.println("********   Tomando muestra ********");
    TEMPERATURA = dht.readTemperature();  // obtencion de valor de temperatura
    HUMEDAD = dht.readHumidity();   // obtencion de valor de humedad
    if (flag1 == false)
    {
        muestra_temp1 = TEMPERATURA;
        flag1 = true;
        Serial.println("M1: "+String(muestra_temp1));

    }

    if (flag2 == false)
    {
        muestra_temp2 = TEMPERATURA;
        flag2 = true;
        flag1 = false;
        Serial.println("M2: "+String(muestra_temp2));

    }

    if (flag3 == false)
    {
        muestra_temp3 = TEMPERATURA;
        flag3 = true;
        flag2 = false;
        Serial.println("M3: "+String(muestra_temp3));

    }
}
