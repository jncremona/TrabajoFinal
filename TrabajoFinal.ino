//G 0 0 0 15 105 1 F

/////////////////////////////////VARIABLES DEL SISTEMA/////////////////////////////////////
/////////////////////////////////VARIABLES LASER///////////////////////////////////////////
int Potencia_laser=0; //esto esta en microlitros

///////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////VARIABLES BIORREACTOR/////////////////////////////////////
int minutos=2; //cada cuanto entrega la bomba
float flujobomba=0.4; //ul/pulso  aca va lo que entrega la bomba por pulso de encoder
int volumen_total=0; //esto esta en microlitros
int D100=1;//esta va a ser la dilución 1/h
float dxmin=0;//medida
float OD=0;//medida
float Tension1=0;//dilucion por minuto
float Tension2=0;//dilucion por minuto
float Potencia1=0;//dilucion por minuto
float Potencia2=0;//dilucion por minuto
///////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////VARIABLES ENCODER/////////////////////////////////////////
double contador_de_pulsos=0;//va a contar cada ranura del encoder
int vueltas;
double pulsos;
///////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////VARIABLES MOTORES Y BOMBAS PERISTALTICAS/////////////////
int velaire=150;
int velstir=100;
int varstir=25;
int velocidad = 3;  // velocidad de la bomba (0~10);
unsigned long timer,timer2,timer3,timer4;
float tiempo=0.5;
int rep=1;
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////VARIABLES SENSORES//////////////////////////////////////
int analogPin1 = 1;     // pin 2 mido la potencia de entrada
int analogPin2 = 0;     // pin 3 mido la potencia de salida 
int val11 = 0;           // entrada
int val21 = 0;           // salida
int val12 = 0;           // entrada
int val22 = 0;           // salida
int flag=0;
//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////VARIABLES CONTROL//////////////////////////////////////

char cadena[130];
int Comenzar=0;

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////Funciones /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void setup() {              
  Serial.begin(9600);
  Serial.flush();
  pinMode(9, OUTPUT);//mezclador magnetico
  pinMode(10, OUTPUT);//bomba sin encoder
  pinMode(11, OUTPUT);//bomba con encoder
  pinMode(12, OUTPUT);//
  pinMode(6, OUTPUT);//aireador
  pinMode(5, OUTPUT);//laser
  attachInterrupt(digitalPinToInterrupt(2),contar,FALLING);//contador encoder  //cada vez que haya un cambio en el pin 3 va a llamar a la funcion contar
}

/////////////////////////////////////////////////////////////////////////////////////////
void calculos() {  
  if(timer-timer2>=2000){             //esta en milisegundos esto es cada cuanto manda
    
    if(flag==0){ 
    analogWrite(5,0);
    val11 = analogRead(analogPin1);    // leo los pines
    val21 = analogRead(analogPin2);
    flag=1;
    }
    
  analogWrite(5,Potencia_laser);            //mando una señal para prender el laser
    if(timer-timer2>=3000){
    if(flag==1){ 
    flag=0;
    timer2=timer; 
    val12 = analogRead(analogPin1);    // leo los pines
    val22 = analogRead(analogPin2);
      }
    delay(10);
   analogWrite(5,0);
   Serial.print("S,");
   Serial.print(val11);
   Serial.print(",");
   Serial.print(val12);
   Serial.print(",");
   Serial.print(val21);
   Serial.print(",");
   Serial.print(val22);
   Serial.print(",");
   Serial.print(D100);
   Serial.print(",");
   Serial.print(velaire);
   Serial.print(",");
   Serial.print(varstir);
   Serial.print(",");
   Serial.print(volumen_total);
   Serial.print(",");
   Serial.print(Potencia_laser);
   Serial.println(",E");

   
   delay(10);
   Tension1=val12*0.0049;
   Tension2=val22*0.0049;
   OD=(Tension1*7.2)/(Tension2*20.46);
   delay(10);
   OD=log(Tension1*20.46)-log(Tension2*7.2);
    delay(10);
    Serial.println("//////////////////////////////////");
   Serial.print("T1: ");
   Serial.println(Tension1);
   Serial.print("T2: ");
   Serial.println(Tension2);
   Serial.print("Dilucion: ");
   Serial.println(OD);
   Serial.println("//////////////////////////////////");
    }             //apago el laser
}}

////////////////////////////////////////////////////////////////////////////////////////
float leerserial() {  
const int serStrLen = 200; char serInStr[ serStrLen ];
char a[10],b[10],c[10],d[10],e[10],f[10];

delay(10); // espero 30ms
memset( serInStr, 0, sizeof(serInStr) ); //pongo en 0
int i=0;

while(Serial.available() && i<serStrLen){
  serInStr[i] = Serial.read(); 
  i++;
}

sscanf(serInStr,"G %s %s %s %s %s %s F",a,b,c,d,e,f);
D100=atoi(a);
velaire=atoi(b);
velstir=atoi(c);
volumen_total=atoi(d);
Potencia_laser=atoi(e);
Comenzar=atoi(f);
return (0);

}
//////////////////////////////////////////////////////////////////////////////////////////
void contar(){
contador_de_pulsos++;
//Serial.println(contador_de_pulsos);
}
//////////////////////////////////////////////////////////////////////////////////////////
void diluir(){//va a diluir una determinada cantidad de pulsos (6ul(+-0.3ul)-->36pulsos).
if(contador_de_pulsos<pulsos){
//Serial.println(pulsos);
//Serial.println(contador_de_pulsos);
analogWrite(11,200);
analogWrite(10,150);}
if(contador_de_pulsos>pulsos){
analogWrite(11,0);
analogWrite(10,0);
/*Serial.print("Cambios:");
 Serial.print(contador_de_pulsos);
Serial.print(" Flujo diluído[ul]:");
Serial.println(contador_de_pulsos*6);  */

}}
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Fin Funciones ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void loop() {

//2//////////////////////////////////////////////
delay(100);
  if(Serial.available()){
     leerserial();

  }
/////////////////////////////////////////////////  

if(Comenzar==1){
//1//////////////////////////////////////////////
timer=millis();
calculos();// mide los sensores y envia datos
/////////////////////////////////////////////////  

  
//3//////////////////////////////////////////////
dxmin=D100*volumen_total/6;//D=1 ----> osea 15ml/h entrega---->1000*D100*volumen_total/(60)(100) porque D100=D*100 y dxmin en ul
pulsos=minutos*dxmin/flujobomba;//   dxmin es la dilucion por minuto necesaria en ul y el flujo de labomba esta en volumen/pulso
timer = millis();
if(timer-timer4>minutos*60000){//cada 1 minuto reseteo los pulsos
timer4=timer;
contador_de_pulsos = 0;
}
diluir();
/////////////////////////////////////////////////


//4//////////////////////////////////////////////
if (varstir<velstir){
varstir=varstir+1;}
if(velstir<varstir){
  varstir=varstir-5;
  }
analogWrite(9,varstir);

analogWrite(6,velaire);
/////////////////////////////////////////////////


}
if(Comenzar!=1){
 
      delay(3000);
   Serial.print("S,");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.print(",");
   Serial.print(1);
   Serial.println(",E");
  
  
  
  }
}
