
//////////---------INCLUDES RELACIONADOS CON LA PANTALLA OLED Y EL KEYPAD-------/////////////////
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>//Librería necesaria para la comunicación I2C conel chip de la pantalla
#include <Adafruit_GFX.h>//Librería gráfica para la pantalla OLED
#include <Adafruit_SSD1306.h>//Librería necesaria para laspantallas OLED

//////////----INCLUDE RTC----/////
#include "RTClib.h"
#define rele 12
 
// Definimos constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED. Valor típico 128
#define ALTO_PANTALLA 64 // alto pantalla OLED. Valor típico son 64 y 32

////////////////----------Variables keypad y OLED-------////////
const byte filas =4;
const byte columnas=4;
char teclas [filas][columnas] ={
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
  };
byte pinesFilas [filas] = {11, 10, 9, 8};//Definimos los pines correspondientes a las filas del teclado matricial
byte pinesColumnas [columnas] = {7, 6, 5, 4};//Definimos los pines correspondientes a las filas del teclado matricial
char tecla_pulsada;
char horariego1[6];
char horariego2[6];
char horariego3[6];

int i=0;
int tecla_numerica;
int tecla_anterior;
char opcion_seleccionada;
bool bandera;
bool bandera_seleccion;

void menu_principal();
void opcion_a();
void opcion_b();
void opcion_c();
void error_numerico();

//////////----Variables RTC y objetos----////

RTC_DS1307 rtc;
DateTime hoy;
int bandera_riego=0;
int bandera_verano=0;

int duracion_riego=10000;

int horariego1=7;
int horariego2=12;
int horariego3=18;
//int horariegoextra=22;
//int hora_actual;
int ano,mes,dia,hora,minuto,segundo;
char ano_c[4];
char mes_c[2];
char dia_c[2];
char hora_c[2];
char min_c[2];
char seg_c[2];

String DiasdelaSemana[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String MesesdelAno[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };

void ajuste_reloj();

//////Configuracion del reloj///////
//SDA A4 Línea I2C de transmisión de datos
//SCL A5 Línea I2C de la señal de reloj
//GND GND 
//VCC VCC
//SQW Este pin sirve para obtener una señal cuadrada, como no lo usaremos no lo declaramos


///////------ Objeto de la clase Adafruit_SSD1306 y del keypa------////////

Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);//&Wire es un puntero de la clase estática Wire. -1 es el pin de Arduino o ESP8266 que se utiliza para resetear la pantalla en caso de que la pantalla tenga un pin RST (no es nuestro caso)
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, filas, columnas); ///Crea el mapa del teclado 
 
void setup() {

  Wire.begin();///Conecta arduino al bus. Si no se especifica dirección, se conecta arduino al bus como maestro.
  rtc.begin();////Iniciamos el RTC
  bandera_seleccion=0;//Bajamos la bandera de selección
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);//Iniciamos la pantalla OLED que se encuentra en la dirección 0x3C

  
  menu_principal();

 
}
 
void loop() {

    tecla_pulsada = teclado.getKey();//Comprobamos que tecla se ha pulsado
    opcion_seleccionada=tecla_pulsada;//La guardamos en una nueva variable
    //Tenemos varias opciones
    if(opcion_seleccionada == 'A'){
      opcion_a();
      bandera_seleccion=1;
    }
    if(opcion_seleccionada == 'B'){
      opcion_b();
      bandera_seleccion=1;
    }
    if(opcion_seleccionada == 'C'){
      opcion_c();
      bandera_seleccion=1;
    }


}

//Aquí tenemos las distintas funciones en las que podemos programar la hora de riego
void opcion_a(){
    if(bandera_seleccion==1){


    //Limpiamos el display, situamos el cursor en la posición adecuada e imprimimos por pantalla el mensaje
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Opcion A seleccionada");
    display.display();

    //Creamos un bucle en el que cada vez que recibimos una tecla, la guardamos en el vector corresponiente
    for(i=0;i<6;i++){
      tecla_pulsada=teclado.waitForKey();////Esta función es importante, ya que hasta que no pulsemos una tecla, el micro se queda parado en está instrucción. La unica manera de avanzar es pulsar una tecla o salir de bucle for
      if(tecla_pulsada=='A'||tecla_pulsada=='B'||tecla_pulsada=='C'||tecla_pulsada=='D'||tecla_pulsada=='*'||tecla_pulsada=='#'){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("ERROR");
        display.setCursor(0,10);
        display.println("La tecla pulsada no es numerica");
        display.display();
        delay(2000);
        bandera_seleccion=0;
        menu_principal();
        break;
      }
      else{
        horariego1[i]=tecla_pulsada;
        tecla_numerica=horariego1[i]-48;
        if(i==0&&tecla_numerica>2){          
          error_numerico();
          menu_principal();
          break;
        }
        if(i==0&&tecla_numerica==2){
          tecla_anterior=tecla_numerica;
        }
        if(i==1&&tecla_anterior==2&&tecla_numerica>4){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==2&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==4&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        
        if(i==5){//Si ya hemos terminado de meter la hora completa, esta ocupará 6 espacios, por lo que una vez estén rellenos imprimiremos por pantalla lo que tenemos en el vector
          display.clearDisplay();
          display.setCursor(3,0);  
          display.println("Se ha programado la hora de riego a las ");
          display.write(horariego1[0]);
          display.write(horariego1[1]);
          display.print(':');
          display.write(horariego1[2]);
          display.write(horariego1[3]);
          display.print(':');
          display.write(horariego1[4]);
          display.write(horariego1[5]); 
          display.display();
          delay(5000);
          display.clearDisplay();
          menu_principal();
          break;
         }
    }
    }
    }
}

//Aquí tenemos las distintas funciones en las que podemos programar la hora de riego
void opcion_b(){
    if(bandera_seleccion==1){
    bandera_seleccion=0;//Lo primero que hacemos es bajar la bandera de seleccion

    //Limpiamos el display, situamos el cursor en la posición adecuada e imprimimos por pantalla el mensaje
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Opcion B seleccionada");
    display.display();

    //Creamos un bucle en el que cada vez que recibimos una tecla, la guardamos en el vector corresponiente
    for(i=0;i<6;i++){
      tecla_pulsada=teclado.waitForKey();////Esta función es importante, ya que hasta que no pulsemos una tecla, el micro se queda parado en está instrucción. La unica manera de avanzar es pulsar una tecla o salir de bucle for
      if(tecla_pulsada=='A'||tecla_pulsada=='B'||tecla_pulsada=='C'||tecla_pulsada=='D'||tecla_pulsada=='*'||tecla_pulsada=='#'){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("ERROR");
        display.setCursor(0,10);
        display.println("La tecla pulsada no es numerica");
        display.display();
        delay(2000);
        menu_principal();
        break;
      }
      else{
        horariego1[i]=tecla_pulsada;
        tecla_numerica=horariego1[i]-48;
        if(i==0&&tecla_numerica>2){          
          error_numerico();
          menu_principal();
          break;
        }
        if(i==0&&tecla_numerica==2){
          tecla_anterior=tecla_numerica;
        }
        if(i==1&&tecla_anterior==2&&tecla_numerica>4){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==2&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==4&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
      horariego2[i]=tecla_pulsada;
      if(i==5){//Si ya hemos terminado de meter la hora completa, esta ocupará 6 espacios, por lo que una vez estén rellenos imprimiremos por pantalla lo que tenemos en el vector
        display.clearDisplay();
        display.setCursor(3,0);  
        display.println("Se ha programado la hora de riego a las ");
        display.write(horariego2[0]);
        display.write(horariego2[1]);
        display.print(':');
        display.write(horariego2[2]);
        display.write(horariego2[3]);
        display.print(':');
        display.write(horariego2[4]);
        display.write(horariego2[5]); 
        display.display();
        delay(5000);
        display.clearDisplay();
        menu_principal();
        break;
      }
    }
    

    }
}
}

//Aquí tenemos las distintas funciones en las que podemos programar la hora de riego
void opcion_c(){
    if(bandera_seleccion==1){
    bandera_seleccion=0;//Lo primero que hacemos es bajar la bandera de seleccion

    //Limpiamos el display, situamos el cursor en la posición adecuada e imprimimos por pantalla el mensaje
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Opcion C seleccionada");
    display.display();

    //Creamos un bucle en el que cada vez que recibimos una tecla, la guardamos en el vector corresponiente
    for(i=0;i<6;i++){
      tecla_pulsada=teclado.waitForKey();////Esta función es importante, ya que hasta que no pulsemos una tecla, el micro se queda parado en está instrucción. La unica manera de avanzar es pulsar una tecla o salir de bucle for
      if(tecla_pulsada=='A'||tecla_pulsada=='B'||tecla_pulsada=='C'||tecla_pulsada=='D'||tecla_pulsada=='*'||tecla_pulsada=='#'){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("ERROR");
        display.setCursor(0,10);
        display.println("La tecla pulsada no es numerica");
        display.display();
        delay(2000);
        menu_principal();
        break;
      }
      else{
        horariego1[i]=tecla_pulsada;
        tecla_numerica=horariego1[i]-48;
        if(i==0&&tecla_numerica>2){          
          error_numerico();
          menu_principal();
          break;
        }
        if(i==0&&tecla_numerica==2){
          tecla_anterior=tecla_numerica;
        }
        if(i==1&&tecla_anterior==2&&tecla_numerica>4){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==2&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==4&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
      if(i==5){//Si ya hemos terminado de meter la hora completa, esta ocupará 6 espacios, por lo que una vez estén rellenos imprimiremos por pantalla lo que tenemos en el vector
        display.clearDisplay();
        display.setCursor(3,0);  
        display.println("Se ha programado la hora de riego a las ");
        display.write(horariego3[0]);
        display.write(horariego3[1]);
        display.print(':');
        display.write(horariego3[2]);
        display.write(horariego3[3]);
        display.print(':');
        display.write(horariego3[4]);
        display.write(horariego3[5]);
        display.display();
        delay(5000);
        display.clearDisplay();
        menu_principal();
        break;
      }
      
    }
    

    }
}
}

void menu_principal(){

  i=0;

    // Limpiar buffer
  display.clearDisplay();
 
  // Tamaño del texto
  display.setTextSize(2);
  // Color del texto
  display.setTextColor(SSD1306_WHITE);
  // Posición del texto
  display.setCursor(40, 16);
  
  // Escribir texto
  display.println("Menu");
 
  // Enviar a pantalla
  display.display();

  //Seleccionamos el lugar donde pondremos el cursor, imprimimos por pantalla y lo mandamos al display
  display.setCursor(10,32);
  display.println("Principal");
  display.display();
  
  delay(2000);
  
  // Limpiamos la pantalla, cambiamos el tamaño del texto y resituamos el cursor para escribir el nuevo mensaje. Luego lo mandamos
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Pulse A, B y C para  cambiar las horas de riego 1, 2 y 3 respectivamente");
  display.display();
}


void error_numerico(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("La hora introducida no es valida");
  display.display();
  delay(2000);
}


void ajuste_reloj(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Por favor, introduzca el año, el mes, el día, y la hora respectivamente")
  display.display();
  int j;
  int k=0;
  if(k==0){
    for (j=3;j<0;j--){
      ano_c = teclado.WaitForKey();
    if(j=0){
      k++;
    }
  }
  }

  if(k==1){
    for(j=2;j<0;j--){
      mes_c = teclado.WaitForKey();
    }
    if(j=0){
      k++;
    }
  }
  if(k==2){
    for (j=2;j<0;j--){
      dia_c = teclado.WaitForKey();
    if(j=0){
      k++;
  }
  if(k==3){
    for (j=2;j<0;j--){
      hora_c = teclado.WaitForKey();
    if(j=0){
      k++;
  }
  if(k==4){
    for (j=2;j<0;j--){
     minuto_c = teclado.WaitForKey();
    if(j=0){
      k++;
  }
  if(k==5){
  for (j=2;j<0;j--){
      segundo_c = teclado.WaitForKey();
    if(j=0){
      k++;
  }
  if(k==6){
    rtc.adjust(DateTime(ano_c, mes_c, dia_c, hora_c, minuto_c, segundo_c));
    recuperar_fecha();
  }
  

}

void recuperar_fecha(){
  hoy = rtc.now
  ano=hoy.year();
  mes=hoy.month();
  dia=hoy.day();
  hora=hoy.hour();
  minuto=hoy.minute();
  segundo=hoy.second();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("La fecha actual es:");
  display.write(ano);
  display.print("/");
  display.write(mes);
  display.print("/");
  display.write(dia);
  display.print("/");
  display.write(hora);
  display.print("/");
  display.write(minuto);
  display.print("/");
  display.write(segundo);
  display.print("/");

}