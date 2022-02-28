/*
 * 
 * 
 */
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#define N 23

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

char linia1[16], linia2[16];
unsigned char bufor [N];
int PM25 = 20, PM10 = 20;
int wartoscPM25(unsigned char *thebuf);
int wartoscPM10(unsigned char *thebuf);
char sprawdzLancuch(unsigned char *thebuf, char leng);

void setup(){
   Serial.begin(9600);             // rozpoczęcie komunikacji przez SerialPort baud 9600 (bitów na sekund)
   lcd.init();// inicjalizacja ekranu
   lcd.backlight(); //podświetlenie ekraunu
   lcd.setCursor(0, 0);         // move cursor to   (0, 0)
   lcd.print("Witaj w");        // print message at (0, 0)
   lcd.setCursor(2, 1);         // move cursor to   (0, 0)
   lcd.print("PoliNosie");        // print message at (0, 0)
   delay(1000); //opoznienie 1 sekunda
   Serial.println("PM10,PM2.5"); //wydrukowanie wartosci do pomiarow poprzez serial port

}
 
void loop(){
  if(Serial.find(0x42))    // odnalezienie bitu inicjującego transfer bajtow po serial porcie patrz strona www
    Serial.readBytes(bufor,N); // odczyt ramki bajtow wynoszący n czyli 24 bajty [23 + znak 0 = 24 sprawdz zapis tablic w programowaniu 0 to 1 wartosc]
     if(bufor[0] == 0x4d){ // jezeli wartosc bufora to 0x4d, czyli 2 bajt z czujnika to:
      if(sprawdzLancuch(bufor,N)){  //sprawdz poprawnosc lancucha, patrz funkcja sprawdzLancuch
        PM25=wartoscPM25(bufor); // odczyt pm2.5
        PM10=wartoscPM10(bufor);  //odczyt pm10
     }
    }
   lcd.clear(); //wyczyszczenie ekranu lcd
   lcd.setCursor(0,0); //ustawienie kursora na lcd na linii1
   sprintf(linia1,"PM2.5=%d ug/m3",PM25); //wydrukowanie wartosci pm2.5 na lcd
   lcd.print(linia1);
   lcd.setCursor(0,1); //ustawienie kursora na lcd na linii 2
   sprintf(linia2,"PM10=%d ug/m3",PM10); //wydrukowanie wartosci pm10 na lcd
   lcd.print(linia2);

   Serial.print(PM10);
   Serial.print(",");
   Serial.println(PM25); //wyswietlaie obu wartosci w serial porcie
   delay(1000); //opoznienie 1 sekunda
}

int wartoscPM25(unsigned char *buf) // generacja wartosci PM2,5
{
  int PM25v;
  PM25v=((buf[11]<<8) + buf[12]);  //odczyt wartosci pm2.5, patrz bajt 11,12 w ramce bajtow na stronie z przesunieciem bitowym 8 znakow
  return PM25v;
}

int wartoscPM10(unsigned char *buf) // generacja wartosci PM10
{
  int PM10v;
  PM10v=((buf[13]<<8) + buf[14]);  //odczyt wartosci pm10, patrz bajt 13,14 w ramce bajtow na stronie z przesunieciem bitowym 8 znakow
  return PM10v;
}

bool sprawdzLancuch(unsigned char *buf, int dlugosc) // sprawdzenie poprawności lancucha, tj. jego dlugosci wynoszacej n 24 bajty, jezeli jest poprawny funkcja zwraca 1
{  
  bool flaga=0;
  int suma=0;

  for(int i=0; i<(dlugosc-2); i++){
  suma+=buf[i];
  }
 suma=suma + 0x42;
 
  if(suma == ((buf[dlugosc-2]<<8)+buf[dlugosc-1]))  
  {
    suma = 0;
    flaga = 1;
  }
  return flaga;
}
