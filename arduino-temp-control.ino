#define Button 7
#define motor 13
#define sensorTemp 0
#define buttonMais 4
#define buttonMenos 3
#define sensorTemp A0

//////// GENERAL

int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 1;

float temperatura = 24;
float temp_atual = 0;
float temp_anterior = 0;

//////// TEMPO RESISTENCIA TOTAL

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long tempoTotal = 0;
unsigned long Time = 0;

///////// BUTTON -
int buttonStateMenos = 0;

int buttonStatePrevious = LOW;                      // previousstate of the switch
unsigned long minButtonLongPressDuration = 1000;    // Time we wait before we see the press as a long press
unsigned long buttonLongPressMillis;                // Time in ms when we the button was pressed
bool buttonStateLongPress = false;                  // True if it is a long press
const int intervalButton = 100;                      // Time between two readings of the button state
unsigned long previousButtonMillis;                 // Timestamp of the latest reading
unsigned long buttonPressDuration;  
unsigned long currentMillis;  

///////// BUTTON +
int buttonStateMais = 0;

static const int buttonPin = 7;                         // switch pin
int buttonMaisStatePrevious = LOW;                      // previousstate of the switch
unsigned long minButtonMaisLongPressDuration = 1000;    // Time we wait before we see the press as a long press
unsigned long buttonMaisLongPressMillis;                // Time in ms when we the button was pressed
bool buttonMaisStateLongPress = false;                  // True if it is a long press
unsigned long previousMaisButtonMillis;                 // Timestamp of the latest reading
unsigned long buttonMaisPressDuration;  				// 

///////// ESTADO RESISTENCIA

int motorState = 0;
int lastMotorState = 0;

//////////

unsigned long tempoInicio = 0;
char char_pressed;


void setup()
{
  Serial.begin(9600);
  pinMode(Button,INPUT);
  pinMode(motor,OUTPUT);
  pinMode(buttonMais,INPUT);
  pinMode(buttonMenos,INPUT);
}

void loop()
{
  
  if (Serial.available() > 0)
    {
        char_pressed = Serial.read();
        if (char_pressed == 'a')
            Serial.println(temperatura);
    }
  
 tempoInicio = millis();

 buttonState = digitalRead(Button);
  
   if (buttonState != lastButtonState) {
    liga_resistencia(false);
    if (buttonState == HIGH) {    
      if(buttonPushCounter == 4){
      	buttonPushCounter = 1;
      }else{
      	buttonPushCounter++;
      }  
        if(buttonPushCounter == 1){
			//Modo de Funcionamento 1
          	//Serial.println("Modo de Funcionamento 1");
			mensagem(1);
        }
        if(buttonPushCounter == 2){
			//Modo de Funcionamento 2
            //Serial.println("Modo de Funcionamento 2");
			mensagem(2);
        }
        if(buttonPushCounter == 3){
			//Modo de Funcionamento 3
            //Serial.println("Modo de Funcionamento 3");
			mensagem(3);   
        }
        if(buttonPushCounter == 4){
			//Modo de Funcionamento 4
            //Serial.println("Modo de Funcionamento 4");
			mensagem(4);
        }
	
  	}
    delay(500);  
   }
  
  

  switch(buttonPushCounter){
  	case 1:
        liga_resistencia(true);
  		trinta_segundos();
        break;
  	case 2:
        temp_atual = get_temperatura();
        if(temp_atual <= temperatura){
          liga_resistencia(true);
        }else{
          liga_resistencia(false); 
        }
        // Se houver alteração de temp mostrar serial
        if(temp_atual != temp_anterior){
          mensagem(2);
        }
        temp_anterior = temp_atual;
    	break;
  	case 3:
    	trinta_segundos();
    	break;
  	case 4:
        liga_resistencia(false);

        buttonStateMais = digitalRead(buttonMais); 
        buttonStateMenos = digitalRead(buttonMenos);  
    
       	button_menos();
    	delay(200);
  		button_mais();
    
    	if(buttonStateMenos == HIGH && buttonStateLongPress){
            temperatura = temperatura - 0.10;
            mensagem(4);
      	}
        if(buttonStateMais == HIGH && buttonMaisStateLongPress){
            temperatura = temperatura + 0.10;
            mensagem(4);
      	}

    	break;
	default:
    	break;
  }
  
  
    motorState = digitalRead(motor);
    if (motorState != lastMotorState) {
      delay(100);
    if (motorState == HIGH) {
        //Serial.println("MOTOR HIGH");
      	startTime = millis();
    } 
    if (motorState == LOW){
      	//Serial.println("MOTOR LOW");
        endTime = millis();
        Time = endTime - startTime;
        tempoTotal = tempoTotal + Time;      	
    }
    delay(50);
  }
  lastMotorState = motorState;
}


int get_temperatura(){ 
   int reading = analogRead(sensorTemp);
   float voltage = reading * 5.0;
   voltage /= 1024.0;
   float temperatureC = (voltage - 0.5) * 100 ;
   return temperatureC;
}

int liga_resistencia(bool modo){  
  if(modo == true){
  	digitalWrite(motor,HIGH); 
  }
  if(modo == false){
  	digitalWrite(motor,LOW);
  }
  
}

int mensagem(int modo){
	Serial.println("-------------------------");
	Serial.print("Modo de Funcionamento: ");Serial.println(modo);
  	Serial.print("Temp_ref: "); Serial.println(temperatura);
    Serial.print("Temp_atual: "); Serial.println(get_temperatura());
	Serial.print("Resistencia OFF: "); Serial.println((tempoTotal)/1000.0,2);
	Serial.print("Resistencia ON: "); Serial.println((tempoInicio-tempoTotal)/1000.0,2);
}

void trinta_segundos(){
  		buttonState = digitalRead(Button);
	    buttonStateMais = digitalRead(buttonMais); 
        buttonStateMenos = digitalRead(buttonMenos);
          
       if(buttonStateMais == HIGH){
            do{
              liga_resistencia(true);
            }while(!digitalRead(Button) && !digitalRead(buttonMenos));
       }
       if(buttonStateMenos == HIGH){
            do{
              liga_resistencia(false);
            }while(!digitalRead(Button) && !digitalRead(buttonMais));
       }
}

void button_menos(){
	  
  currentMillis = millis(); 

  if(currentMillis - previousButtonMillis > intervalButton) {
    int buttonState = digitalRead(buttonMenos);    
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
      //Serial.println("Button pressed");
    }
    buttonPressDuration = currentMillis - buttonLongPressMillis;
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      //Serial.println("Button long pressed");
    }
    if (buttonState == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;
      buttonStateLongPress = false;
      //Serial.println("Button released");
          temperatura = temperatura - 0.10;
          mensagem(4);
    }
    previousButtonMillis = currentMillis;

  }
}

void button_mais(){
	  
  currentMillis = millis(); 

  if(currentMillis - previousMaisButtonMillis > intervalButton) {
    int buttonMaisState = digitalRead(buttonMais);    
    if (buttonMaisState == HIGH && buttonMaisStatePrevious == LOW && !buttonMaisStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonMaisStatePrevious = HIGH;
      //Serial.println("Button pressed");
    }
    buttonMaisPressDuration = currentMillis - buttonLongPressMillis;
    if (buttonMaisState == HIGH && !buttonMaisStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonMaisStateLongPress = true;
      //Serial.println("Button long pressed");
    }
    if (buttonMaisState == LOW && buttonMaisStatePrevious == HIGH) {
      buttonMaisStatePrevious = LOW;
      buttonMaisStateLongPress = false;
      //Serial.println("Button released");
          temperatura = temperatura + 0.10;
          mensagem(4);
    }
    previousMaisButtonMillis = currentMillis;

  }
}


