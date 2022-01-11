// this welder is an improvement upon mathew's firmware
// this time around, I want to use proper weld timers, including preweld and postweld
// main weld will be a series of pulses, with x weld and y rest

int SSRelay = 15; // relay output pin, active high
int PBStart = 9;  // pushbutton start pin, active low

// #define PREWELD  // comment out to disable preweld
// #define POSTWELD // comment out to disable postweld

// MAIN WELD SETTINGS ******************************
int START_DELAY = 500;      // delay from PBStart on to weld occuring
int NWELDS = 8;             // number of welds in main burst
int WELD_TIME = (int) 16.6 * 1;          // time to do each weld
int REST_TIME = (int) 16.6 * 5;          // time to rest between welds
int WELD_SAFETY_TIME = 1000; // forced time between finishing one weld and starting another
// **************************************

#ifdef PREWELD // PREWELD SETTINGS ******************************
int PREWELD_TIME = 35; // preweld time in ms
int PREWELD_REST = 50; // rest between preweld and main weld
void preweld()
{
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(SSRelay, HIGH);
    delay(PREWELD_TIME);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(SSRelay, LOW);
    delay(PREWELD_REST);
}
#endif

void weld()
{
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(SSRelay, HIGH);
    delay(WELD_TIME);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(SSRelay, LOW);
    delay(REST_TIME);
}

#ifdef POSTWELD
int POSTWELD_REST = 50; // time between main weld and postweld
int POSTWELD_TIME = 30; // time to do postweld
void postweld()
{
    delay(POSTWELD_REST);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(SSRelay, HIGH);
    delay(POSTWELD_TIME);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(SSRelay, LOW);
}
#endif

boolean oneShotBits[8]; // oneshot bits available for use for oneshot or toggle calls
uint8_t ONSTracker;

boolean timerInSession[8];                         // for speed, so we only update timer timers when needed
boolean timerMemory[sizeof(timerInSession)];       // make function calls smaller by remembering previous output state
unsigned long timerTimers[sizeof(timerInSession)]; // debounce timers available for use
uint8_t timerTracker;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PBStart, INPUT);
    pinMode(SSRelay, OUTPUT);
    digitalWrite(SSRelay, LOW);      // active high
    digitalWrite(LED_BUILTIN, HIGH); // active low
    Serial.begin(9600);
}

void loop()
{
    ONSTracker = 0;
    timerTracker = 0;

    if (oneShot(TON(digitalRead(PBStart) == HIGH, START_DELAY, timerTracker++), ONSTracker++))
    {
#ifdef PREWELD
        preweld();
#endif
      Serial.println("Welding " + String(NWELDS) + " cycles for " + String(WELD_TIME) + "ms each.");
        for (int i = 0; i < NWELDS; i++)
        {
            weld();
        }
#ifdef POSTWELD
        postweld();
#endif

        // SAFETY BETWEEN WELDS
        // while (digitalRead(PBStart) == LOW) // wait for the button to release
        //     delay(20);
        delay(WELD_SAFETY_TIME);
    }
}

bool oneShot(boolean precond, uint8_t OSR)
{
    // use global memory to keep track of oneshot bits
    if (precond == 1 && oneShotBits[OSR] == 0)
    {
        oneShotBits[OSR] = 1;
        return 1;
    }
    else if (precond == 0 && oneShotBits[OSR] == 1)
    {
        oneShotBits[OSR] = 0;
        return 0;
    }
    else
        return 0;
}

bool TON(boolean input, int preset, int timerNumber)
{
    if (input && !timerInSession[timerNumber])
        timerTimers[timerNumber] = millis();
    else if (input && timerMemory[timerNumber])
        return 1;
    else if (input && millis() - timerTimers[timerNumber] >= preset)
    {
        timerMemory[timerNumber] = 1;
        return 1;
    }
    else
        ;
    timerMemory[timerNumber] = 0;
    timerInSession[timerNumber] = input;
    return 0;
}
