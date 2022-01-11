int ACRelay = 15;
int PBStart = 9;
int PBLight = 8;

unsigned long TimeDelayON = 150; // weld time
unsigned long TotalTimeON = 0;
int DelayON = LOW;

// ms count between cycles
int DelayOFF = 1000;

int JustOFF = LOW;
int lastButtonState = LOW;


void setup() {
  pinMode(ACRelay, OUTPUT);
  pinMode(PBLight, OUTPUT);
  pinMode(PBStart, INPUT);
  digitalWrite(ACRelay, LOW);

  Serial.begin(9600);
}

void loop() {
  delay(1);
  int reading = digitalRead(PBStart);
  digitalWrite(PBLight, HIGH);
  
  if (reading == LOW && DelayON == LOW){
      JustOFF = LOW;  
  }
  
  if (JustOFF == LOW && reading == HIGH || DelayON == HIGH){
    if (DelayON == LOW){
      TotalTimeON = millis();
    }
    digitalWrite(PBLight, LOW);
    JustOFF = HIGH;
    DelayON = HIGH;
    if(millis() - TotalTimeON <= TimeDelayON){
      digitalWrite(ACRelay, HIGH); 
      Serial.println(millis() - TotalTimeON);
      Serial.println(TimeDelayON);     
    }
    if(millis() - TotalTimeON > TimeDelayON){
      digitalWrite(ACRelay, LOW);
      DelayON = LOW;
      Serial.println("fin");
      delay(DelayOFF);
      digitalWrite(PBLight, HIGH);
    }
  }  
}
