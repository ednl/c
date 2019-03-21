// ********** Definities ****************************************************************

#define DEBUG        // print nieuwe positie/hoek/T-hoog, zie onderaan zoekhoeken()
#define BAUD 115200  // snelheid seriele poort in bit/s

// Basis voor de overflow-interrupt teller in 16-bits timer1
// met een klokfrequentie van 16 MHz, een prescaler van 8 en
// een beoogde frequentie van 50 Hz, waarbij één tel overeenkomt
// met 0,5 us, dus 40000 tikken met 0,02 s = 50 Hz
// Berekening: 65536 - 16MHz/8/50Hz = 65536 - 40000 = 25536
#define TELLER50HZ 25536

// Deltarobot afmetingen
#define L_TRI1 175  // lengte (mm) van zijde van basisplaat (die stil staat)
#define L_TRI2  50  // lengte (mm) van zijde van topplaat (die beweegt)
#define L_ARM1  50  // lengte (mm) van onderste arm (die aan servo vastzit)
#define L_ARM2 202  // lengte (mm) van bovenste arm (die aan topplaat vastzit)

// Servo pinnummer en T-hoog-waarde minimum, maximum, horizontaal, verticaal
// TODO: servo's handmatig zo instellen dat SERVOx_HOR ongeveer 1953 is
// zodat het bereik optimaal is. Zie servo-ijking.xlsx
#define SERVO0_PIN    9
#define SERVO0_MIN  620
#define SERVO0_MAX 2390
#define SERVO0_HOR 1945
#define SERVO0_VER 1000

#define SERVO1_PIN   10
#define SERVO1_MIN  545
#define SERVO1_MAX 2415
#define SERVO1_HOR 1910
#define SERVO1_VER 1000

#define SERVO2_PIN   11
#define SERVO2_MIN  525
#define SERVO2_MAX 2340
#define SERVO2_HOR 1905
#define SERVO2_VER 1115

#define ACCEL_X_PIN     A0
#define ACCEL_Y_PIN     A1
#define ACCEL_RANGE      4      // bereik is van -2 tot +2 g (TODO: of van -4 tot +4?)
#define ACCEL_G       9.81
#define ACCEL_BRAKE   0.01      // niet te snel
#define ACCEL_FACTOR 0.0003832  // = BRAKE * G * RANGE / 1024

// Wiskundige constante voor snelle berekening
#define WORTEL3 1.73205  // sqrt(3)

// ********** Typedefs ******************************************************************

// Struct voor een punt of een vector
typedef struct _Punt {
	float x, y, z;
} Punt;

// Struct voor vaste waarden van de Deltarobot servo's
typedef const struct _ServoConst {
	byte pin;             // Arduino pinnummer
	int hoekmin;          // minimale hoek van arm1 met het xy-vlak
	int hoekmax;          // maximale hoek van arm1 met het xy-vlak
	unsigned int pwmnul;  // T-hoog (us) voor hoek = 0
	float pwmrico;        // T-hoog richtingscoefficient (us/graad)
} ServoConst;

// Struct voor variabelen van de Deltarobot servo's
typedef struct _ServoVar {
	int hoek;          // hoek (graden) van arm1 met het xy-vlak
	unsigned int pwm;  // T-hoog (us) van het PWM-signaal
} ServoVar;

// ********** Globale constanten ********************************************************

// Vaste waarden van de Deltarobot servo's
// hoekmin/max berekend uit gemeten T-hoog-min/max en ingesteld nulpunt SERVOx_HOR
// per struct: pin, hoekmin, hoekmax, pwmnul, pwmrico
// ##### TODO: check waar deze hoekmin en hoekmax vandaan komen! Als het goed is van berekening
// ##### met SERVOx_MIN en SERVOx_MAX. Die berekening dan ook hier zetten ipv. de waardes.
static ServoConst servodata[3] = {
	{ SERVO0_PIN, -22, 120, SERVO0_HOR, (SERVO0_HOR - SERVO0_VER) / 90.0f },
	{ SERVO1_PIN, -34, 122, SERVO1_HOR, (SERVO1_HOR - SERVO1_VER) / 90.0f },
	{ SERVO2_PIN, -40, 129, SERVO2_HOR, (SERVO2_HOR - SERVO2_VER) / 90.0f }
};

// Eerste kwadrant van de sinusfunctie, voor hele graden 0..90
static const float sinustabel[91] = {
	0.0f     ,0.017452f,0.034899f,0.052336f,0.069756f,0.087156f,0.104528f,0.121869f,0.139173f,0.156434f,
	0.173648f,0.190809f,0.207912f,0.224951f,0.241922f,0.258819f,0.275637f,0.292372f,0.309017f,0.325568f,
	0.342020f,0.358368f,0.374607f,0.390731f,0.406737f,0.422618f,0.438371f,0.453990f,0.469472f,0.484810f,
	0.5f     ,0.515038f,0.529919f,0.544639f,0.559193f,0.573576f,0.587785f,0.601815f,0.615661f,0.629320f,
	0.642788f,0.656059f,0.669131f,0.681998f,0.694658f,0.707107f,0.719340f,0.731354f,0.743145f,0.754710f,
	0.766044f,0.777146f,0.788011f,0.798636f,0.809017f,0.819152f,0.829038f,0.838671f,0.848048f,0.857167f,
	0.866025f,0.874620f,0.882948f,0.891007f,0.898794f,0.906308f,0.913545f,0.920505f,0.927184f,0.933580f,
	0.939693f,0.945519f,0.951057f,0.956305f,0.961262f,0.965926f,0.970296f,0.974370f,0.978148f,0.981627f,
	0.984808f,0.987688f,0.990268f,0.992546f,0.994522f,0.996195f,0.997564f,0.998630f,0.999391f,0.999848f,
	1.0f
};

// ********** Globale variabelen ********************************************************

// Positie (x,y,z) van de Deltarobot
Punt positie;

// Servo instellingen van de Deltarobot
// waarden worden gezet wordt in zoekhoeken()
// volatile vanwege aanpassing buiten en uitlezing binnen de interrupt-functie
// per struct: hoek, pwm (maal twee, want timer1 resolutie is 0,5 us)
volatile ServoVar servoinstel[3] = {
	{ 0, SERVO0_HOR << 1 },
	{ 0, SERVO1_HOR << 1 },
	{ 0, SERVO2_HOR << 1 }
};

// Nummer van de servo die op dit moment aangestuurd wordt
// waarde is 0..3 waarbij 0..2 = servonummer, 3 = pauze voor 50 Hz signaal
// volatile vanwege aanpassing en uitlezing binnen de interrupt-functie
volatile byte servohoog = 3;

// ********** Functie prototypes ********************************************************

float sinus(int);
float cosinus(int);
float arm2lengte(byte, int, const Punt *);
bool doornulheen(float, float);
bool eersteiskleiner(float, float);
bool zoekhoeken(Punt);
Punt beginpositie(void);
Punt lijn(Punt, Punt, unsigned int);
//void cirkel(Punt startpunt, Punt middelpunt, char richting, unsigned int mmperstap);
//void boog(Punt startpunt, Punt eindpunt, Punt middelpunt, char richting, unsigned int mmperstap);

// ********** Setup *********************************************************************

void setup()
{
	byte i;

	// Maak connectie met seriele poort als er output nodig is
	#ifdef DEBUG
		Serial.begin(BAUD);  // snelheid in bit/s
		while (!Serial) ;    // wacht totdat de interface beschikbaar is
	#endif

	// Maak de servopins outputs en zet op nul
	for (i = 0; i < 3; ++i) {
		pinMode(servodata[i].pin, OUTPUT);
		digitalWrite(servodata[i].pin, LOW);
	}

	// Voor sprint 2
	// Eén punt om het servosignaal weer te geven op een scoop
	positie = beginpositie();

	noInterrupts();
	TCCR1A = 0;              // reset register A
	TCCR1B = 0;              // reset register B
	TCNT1 = TELLER50HZ;       // timer1 counter = 0,02 s tot overflow
	TCCR1B |= (1 << CS11);   // timer1 prescaler 8 = 0,5 us resolutie
	TIMSK1 |= (1 << TOIE1);  // timer1 overflow interrupt enable
	interrupts();
}

// ********** Loop **********************************************************************

void loop()
{
  // Beweeeg continu in een vierkantje op dezelfde z-hoogte
  positie = lijn(positie, (Punt){50, 50, 185}, 5);
  positie = lijn(positie, (Punt){50, -50, 185}, 5);
  positie = lijn(positie, (Punt){-50, -50, 185}, 5);
  positie = lijn(positie, (Punt){-50, 50, 185}, 5);
  delay(1000);  // 1 seconde wachten voor nieuw vierkantje

  /** 
   * Bewegingsvergelijkingen voor versnellingssensor
   * Dit stuk niet gebruiken indien die sensor niet is aangesloten!
	 *
	float ax, ay;
	static float vx = 0.0f, vy = 0.0f;
	static unsigned long t_prev = 0;
	unsigned long dt, t_now = millis();

	if (t_prev) {
		dt = t_now - t_prev;
		ax = ACCEL_FACTOR * (analogRead(ACCEL_X_PIN) - 512);  // TODO: factor 2? Zie bovenaan bij ACCEL_RANGE
		ay = ACCEL_FACTOR * (analogRead(ACCEL_Y_PIN) - 512);
		vx += ax * dt;
		vy += ay * dt;
		positie.x += vx * dt;
		positie.y += vy * dt;
		zoekhoeken(positie);
	}
	t_prev = t_now;
   *
   * (einde van de begingsvergelijkingen)
   */
}

ISR(TIMER1_OVF_vect)
{
	// Eerst de huidige servopuls uitzetten (0..2 = servonummer, 3 = pauze voor 50 Hz)
	// Sneller maar minder flexibel alternatief voor digitalWrite:
	//PORTB &= B11110001;  // reset pin 9 en 10 en 11
	if (servohoog != 3)
		digitalWrite(servodata[servohoog].pin, LOW);

	// Ga naar de volgende servo (3 servo's en 1 pauze, dus modulo 4)
	++servohoog %= 4;

	// Dan die volgende servo aanzetten, of extra pauze voor 50 Hz signaal
	if (servohoog != 3) {
		// Servopuls aanzetten voor servonummer 0..2
		// aftellen tot de ingestelde T-hoog is bereikt
		// (factor 2 vanwege timer resolutie van 0,5 us zit er al in)
		// Waarschijnlijk sneller, maar minder flexibel alternatief voor digitalWrite:
		//PORTB |= 2 << servohoog;  // set pin 9 of 10 of 11
		digitalWrite(servodata[servohoog].pin, HIGH);
		TCNT1 = 65535 - servoinstel[servohoog].pwm + 1;
	} else {
		// Pauze voor 50 Hz signaal = volmaken tot 20000 us
		// maal twee, want timer resolutie is 0,5 us, dus begin te tellen bij
		// 65536 - 40000 + wat er al af is vanwege 3x T-hoog van de 3 servo's
		TCNT1 = TELLER50HZ + servoinstel[0].pwm + servoinstel[1].pwm + servoinstel[2].pwm;
	}
}

// ********** Functie implementaties ****************************************************

/**
 * Sinus via opzoektabel
 *   param graden: hoek in graden
 *   return: sinus van de hoek
 */
float sinus(int graden)
{
	// domein = [-179..180]
	while (graden <= -180)
		graden += 360;
	while (graden > 180)
		graden -= 360;

	// bekijk per kwadrant
	if (graden >= 90)
		return sinustabel[180 - graden];  // 2e kwadrant
	if (graden >= 0)
		return sinustabel[graden];        // 1e kwadrant
	if (graden >= -90)
		return -sinustabel[-graden];      // 4e kwadrant
	return -sinustabel[180 + graden];     // 3e kwadrant
}

/**
 * Cosinus via opzoektabel
 *   param graden: hoek in graden
 *   return: cosinus van de hoek
 */
float cosinus(int graden)
{
	// domein = [-179..180]
	while (graden <= -180)
		graden += 360;
	while (graden > 180)
		graden -= 360;

	// bekijk per kwadrant
	if (graden >= 90)
		return -sinustabel[graden - 90];  // 2e kwadrant = sinus 3e kwadrant
	if (graden >= 0)
		return sinustabel[90 - graden];   // 1e kwadrant = sinus 2e kwadrant
	if (graden >= -90)
		return sinustabel[90 + graden];   // 4e kwadrant = sinus 1e kwadrant
	return -sinustabel[-graden - 90];     // 3e kwadrant = sinus 4e kwadrant
}

/**
 * Bereken afwijking in arm2-lengte (in het kwadraat) voor één servo
 *   param n: servonummer 0/1/2
 *   param a: servohoek in graden
 *   param p: doelpositie (x,y,z) van de deltarobot
 *   return: afwijking in lengte (kwadraat) van de arm2 vector
 */
float arm2lengte(byte n, int a, const Punt *doel)
{
	// constanten voor snellere berekening
	static const float k0 = 0.5 * (L_TRI1 - L_TRI2);          // (1/2) . (T1 - T2)
	static const float k1 = WORTEL3 * (L_TRI1 - L_TRI2) / 3;  // (1/3) . sqrt(3) . (T1 - T2)
	static const float k2 = WORTEL3 * (L_TRI1 - L_TRI2) / 6;  // (1/6) . sqrt(3) . (T1 - T2)
	static const float m0 = 0.5 * L_ARM1;                     // (1/2) . A1
	static const float m1 = 0.5 * WORTEL3 * L_ARM1;           // (1/2) . sqrt(3) . A1
	static const float m2 = 1.0 * L_ARM2 * L_ARM2;            // A2 . A2 = inproduct van arm2 vector
	// lokale variabelen
	Punt p;  // (dx,dy,dz) = verschilvector tussen doelpositie en daadwerkelijke lengte
	float cosa = cosinus(a);

	switch (n) {
		case 0:
			p.x = doel->x - k1 - L_ARM1 * cosa;
			p.y = doel->y;
			break;
		case 1:
			p.x = doel->x + k2 + m0 * cosa;
			p.y = doel->y - k0 - m1 * cosa;
			break;
		case 2:
			p.x = doel->x + k2 + m0 * cosa;
			p.y = doel->x + k0 + m1 * cosa;
			break;
	}
	p.z = doel->z - L_ARM1 * sinus(a);
	
	// afwijking in lengte dx^2 + dy^2 + dz^2
	return p.x * p.x + p.y * p.y - m2 + p.z * p.z;
}

/**
 * Hebben twee getallen een verschillend teken?
 * (de ene negatief, de andere nul of positief)
 * Eenvoudiger maar minder snel: (f0 < 0) != (f1 < 0)
 */
bool doornulheen(float f0, float f1)
{
	// vergelijk (xor) de signbits van de twee floats
	return ((*(((byte*) &f0) + 3) & 0x80) ^ (*(((byte*) &f1) + 3) & 0x80));
}

/**
 * Welke absolute waarde is kleiner?
 * (ligt de eerste dichter bij nul?)
 * Eenvoudiger maar minder snel: fabsf(f0) < fabsf(f1)
 */
bool eersteiskleiner(float f0, float f1)
{
	// zet de signbits op nul, vergelijk grootte
	*(((byte*) &f0) + 3) &= 0x7f;  // reset signbit = abs(f0)
	*(((byte*) &f1) + 3) &= 0x7f;  // reset signbit = abs(f1)
	return (f0 < f1);
}

/**
 * Benader de servohoeken en bijbehorende T-hoog
 * param pos: doelpositie (x,y,z) van de Deltarobot
 * return: kan het punt bereikt worden?
 *   indien ja: resultaat hoek+pwm in globale variabele servoinstel[]
 */
bool zoekhoeken(Punt pos)
{
	static int richting[3] = { 1, 1, 1 };  // onthoud de richting voor nieuwe hoek
  // Om nieuwe hoekbenaderingen lokaal bij te houden, want de berekening en opslag in de
  // globale variabele wordt eventueel afgebroken als 1 van de 3 benaderingen niet kan/mag:
  int bestehoek[3] = { servoinstel[0].hoek, servoinstel[1].hoek, servoinstel[2].hoek };
  int hoek0, hoek1, randhoek;            // probeer hoeken, min/max
	float len0, len1;                      // benadering van arm2
	bool bestegevonden;                    // beste benadering gevonden (verschil door nul heen)
	bool klaar;                            // geen iteratie meer nodig
  byte i;

	for (i = 0; i < 3; ++i) {

		// Beginwaarde
		klaar = false;
		hoek0 = servoinstel[i].hoek;
		
		// Check randen, doe 1e poging
		if (hoek0 <= servodata[i].hoekmin || hoek0 >= servodata[i].hoekmax) {

			if (hoek0 <= servodata[i].hoekmin) {
				// De eerste hoek is meteen al op of onder het minimum
				richting[i] = 1;               // omhoog
				hoek0 = servodata[i].hoekmin;  // voor de zekerheid
			} else {
				// De eerste hoek is meteen al op of boven het maximum
				richting[i] = -1;              // omlaag
				hoek0 = servodata[i].hoekmax;  // voor de zekerheid
			}
			hoek1 = hoek0 + richting[i];
			len0 = arm2lengte(i, hoek0, &pos);
			len1 = arm2lengte(i, hoek1, &pos);
			if (eersteiskleiner(len0, len1)) {
				// Eerste was beter, maar die hoek was al min/maximaal, dus klaar
				//bestehoek = hoek0;
				//klaar = true;
        return false;  // nieuw algoritme: als één niet kan, dan meteen klaar!
			} else if (doornulheen(len0, len1)) {
				// Benaderingen hebben verschillend teken: 1 van de 2 is de beste
				bestehoek[i] = eersteiskleiner(len0, len1) ? hoek0 : hoek1;
				klaar = true;
			} else {
				// Gaat de goede kant op, we moeten verder zoeken in de ingeslagen richting
				bestehoek[i] = hoek1;
			}

		} else {

			// We zitten (nog) niet op een rand dus probeer eerst de oude richting
			hoek1 = hoek0 + richting[i];
			len0 = arm2lengte(i, hoek0, &pos);
			len1 = arm2lengte(i, hoek1, &pos);
			if (doornulheen(len0, len1)) {
				// Benaderingen hebben verschillend teken: 1 van de 2 is de beste
				bestehoek[i] = eersteiskleiner(len0, len1) ? hoek0 : hoek1;
				klaar = true;
			} else if (eersteiskleiner(len0, len1)) {
				// De vorige was dichter bij nul: we gaan de verkeerde kant op!
				richting[i] = -richting[i];  // vanaf nu de andere kant op
				bestehoek[i] = hoek0;        // begin opnieuw vanaf beginwaarde
				len1 = len0;                 // len0 was beter, gebruik als laatste benadering
			} else {
				// Gaat de goede kant op, we moeten verder zoeken in deze richting
				bestehoek[i] = hoek1;
			}

		}

		if (!klaar) {

			// Houd minimum of maximum in de gaten
			randhoek = (richting[i] > 0) ? servodata[i].hoekmax : servodata[i].hoekmin;

			// Doorzoeken zolang niet op de rand, en benaderingen met hetzelfde teken
			do {
				len0 = len1;                               // vorige onthouden
				bestehoek[i] += richting[i];               // nieuwe hoek poging
				len1 = arm2lengte(i, bestehoek[i], &pos);  // nieuwe benadering van arm2
        bestegevonden = doornulheen(len0, len1);   // is de ene groter en de andere kleiner dan nul?
			} while (!bestegevonden && bestehoek[i] != randhoek);

			// We zitten nu of op de rand, en/of we hebben de beste benadering gevonden
      if (bestegevonden) {
        // Wel de kleinste pakken
        if (eersteiskleiner(len0, len1))
          bestehoek[i] -= richting[i];
      } else
        // Niet door nul heen, dus geen optimale benadering gevonden,
        // dus niks aanpassen en meteen klaar
        return false;
		} // if !klaar
	} // for i = [0..2]

  // Alledrie gelukt, dus nieuwe servo instelling
  // Tijdens het zetten van .pwm geen interrupts, want de waarde wordt gebruikt
  // in de ISR() en het is een int dus de ene byte kan veranderd zijn en de andere
  // niet terwijl er dan net een interrupt afgaat (en het zijn sowieso 2 statements)
  noInterrupts();
  for (i = 0; i < 3; ++i) {
    servoinstel[i].hoek = bestehoek[i];
    servoinstel[i].pwm = servodata[i].pwmnul - (servodata[i].pwmrico * bestehoek[i]);
    servoinstel[i].pwm <<= 1;  // maal twee, want de timer resolutie is 0,5 us.
  }
  interrupts();

	#ifdef DEBUG
		String s = "pos=("
			+ String(pos.x, 0) + ","
			+ String(pos.y, 0) + ","
			+ String(pos.z, 0) + ") deg=("
			+ servoinstel[0].hoek + ","
			+ servoinstel[1].hoek + ","
			+ servoinstel[2].hoek + ") pwm=("
			+ (servoinstel[0].pwm >> 1) + ","
			+ (servoinstel[1].pwm >> 1) + ","
			+ (servoinstel[2].pwm >> 1) + ")";
		Serial.println(s);
	#endif
  
  // Alles klar
  return true;
}

/**
 * Beweeg de Deltarobot naar de beginpositie
 * Dit is in het midden van het xy-vlak en op een zodanige hoogte dat het
 * bereik in het xy-vlak maximaal is, dwz. zodat ie het verst opzij kan
 */
Punt beginpositie(void)
{
	Punt p = { 0.0f, 0.0f, 185.0f };
	if (zoekhoeken(p))
  	return p;
  else {
    // Ja, wat nu? Als ie de beginpositie al niet kan vinden, dan is er iets mis!
    #ifdef DEBUG
      Serial.println("Fout: kan de beginpositie niet vinden.");
    #endif
    while (true) ;
  }
}

/**
 * Beweeg de Deltarobot in een rechte lijn van A naar B
 * param startpunt: van Punt A (x,y,z)
 * param eindpunt: naar Punt B (x,y,z)
 * param mmperstap: verplaatsing in mm per stap in de 3D-beweging (0 = in 1 keer)
 * param eerstnaarstart: eerst naar het startpunt bewegen, of is dit een voortzetting?
 */
/* OUDE VERSIE
void lijn(Punt startpunt, Punt eindpunt, unsigned int mmperstap, bool eerstnaarstart)
{
	float dx, dy, dz;
	unsigned int stappen = 0;

	if (mmperstap) {  // begrensde snelheid
		// verschilvector
		dx = eindpunt.x - startpunt.x;
		dy = eindpunt.y - startpunt.y;
		dz = eindpunt.z - startpunt.z;
		// aantal stappen afhankelijk van lengte van verschilvector, 0.5 voor afronding
		stappen = 0.5 + sqrt(dx*dx + dy*dy + dz*dz) / mmperstap;
		if (stappen) {  // 1 of meer stappen
			if (stappen != 1) {  // 2 of meer stappen
				// Bereken verplaatsing per stap in x/y/z-richting
				dx /= stappen;
				dy /= stappen;
				dz /= stappen;
			}
			--stappen;  // gebruik alleen tussenpunten, dus 1 minder dan aantal stappen
		}
	}

	// Ga eerst naar het beginpunt, indien gevraagd
	if (eerstnaarstart)
		zoekhoeken(startpunt);  // begin hier

	// Maak de beweging met tussenstappen, indien gevraagd en nodig
	while (stappen) {
		startpunt.x += dx;
		startpunt.y += dy;
		startpunt.z += dz;
		zoekhoeken(startpunt);  // tussenpunt
		--stappen;
	}

	// Ga naar het eindpunt
	zoekhoeken(eindpunt);  // eindig hier
}
*/

/**
 * NIEUWE VERSIE
 * Beweeg de Deltarobot in een rechte lijn naar punt A
 * param laatstepunt: van huidige bekende positie (x,y,z)
 * param eindpunt: naar Punt A (x,y,z)
 * param mmperstap: verplaatsing in mm per stap in de 3D-beweging (0 = in 1 keer)
 * return: laatste positie die bereikt kon worden = nieuwe huidige positie
 */
Punt lijn(Punt laatstepunt, Punt eindpunt, unsigned int mmperstap)
{
  Punt volgendepunt = laatstepunt;
  float dx, dy, dz;
  unsigned int stappen = 0;

  if (mmperstap) {  // begrensde snelheid
    // verschilvector
    dx = eindpunt.x - laatstepunt.x;
    dy = eindpunt.y - laatstepunt.y;
    dz = eindpunt.z - laatstepunt.z;
    // aantal stappen afhankelijk van lengte van verschilvector, 0.5 voor afronding
    stappen = 0.5 + sqrt(dx*dx + dy*dy + dz*dz) / mmperstap;
    if (stappen) {  // 1 of meer stappen
      if (stappen != 1) {  // 2 of meer stappen
        // Bereken verplaatsing per stap in x/y/z-richting
        dx /= stappen;
        dy /= stappen;
        dz /= stappen;
      }
      --stappen;  // gebruik alleen tussenpunten, dus 1 minder dan aantal stappen
    }
  }

  // Maak de beweging met tussenstappen, indien gevraagd en nodig
  while (stappen) {
    // Probeer naar het volgende tussenpunt te gaan
    volgendepunt.x += dx;
    volgendepunt.y += dy;
    volgendepunt.z += dz;
    if (zoekhoeken(volgendepunt))
      laatstepunt = volgendepunt;
    else
      return laatstepunt;
    --stappen;
  }

  // Nu nog naar het eindpunt
  return (zoekhoeken(eindpunt) ? eindpunt : laatstepunt);
}
