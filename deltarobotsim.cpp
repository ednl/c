// ********** Bibliotheken **************************************************************

#ifndef ARDUINO
	// Voor simulatie, niet op een Arduino
	#include <stdio.h>  // printf
	#include <math.h>   // sqrt, fabsf
#endif

// ********** Definities ****************************************************************

#define DEBUG  // print tussenstappen, zie onderaan zoekhoeken()

// Deltarobot afmetingen
#define L_TRI1 175  // lengte (mm) van zijde van basisplaat (die stil staat)
#define L_TRI2  50  // lengte (mm) van zijde van topplaat (die beweegt)
#define L_ARM1  50  // lengte (mm) van onderste arm (die aan servo vastzit)
#define L_ARM2 202  // lengte (mm) van bovenste arm (die aan topplaat vastzit)

// Servowaarde (T-hoog) minimum, maximum, horizontaal, verticaal
#define SERVO0_MIN  620
#define SERVO0_MAX 2200
#define SERVO0_HOR 1700
#define SERVO0_VER  700

#define SERVO1_MIN  700
#define SERVO1_MAX 2300
#define SERVO1_HOR 1700
#define SERVO1_VER  780

#define SERVO2_MIN  660
#define SERVO2_MAX 2350
#define SERVO2_HOR 1700
#define SERVO2_VER  800

// TODO: servo's handmatig zo instellen dat SERVOx_HOR ongeveer 1953 is
// zodat het bereik optimaal is. Zie servo-ijking.xlsx

// Wiskundige constante voor berekening
#define WORTEL3 1.73205  // sqrt(3)

// ********** Typedefs ******************************************************************

#ifndef ARDUINO
	// Voor simulatie, niet op een Arduino
	typedef unsigned char byte;
#endif

// Struct voor een punt of een vector
typedef struct Punt {
	float x, y, z;
} Punt;

// Struct voor vaste waarden van de Deltarobot servo's
typedef const struct ServoConst {
	byte pin;             // Arduino pinnummer
	int hoekmin;          // minimale hoek van arm1 met het xy-vlak
	int hoekmax;          // maximale hoek van arm1 met het xy-vlak
	unsigned int pwmnul;  // T-hoog (us) voor hoek = 0
	float pwmrico;        // T-hoog richtingscoefficient (us/graad)
} ServoConst;

// Struct voor variabelen van de Deltarobot servo's
typedef struct ServoVar {
	int hoek;          // hoek (graden) van arm1 met het xy-vlak
	unsigned int pwm;  // T-hoog (us) van het PWM-signaal
} ServoVar;

// ********** Globale constanten ********************************************************

// Vaste waarden van de Deltarobot servo's
// hoekmin/max berekend uit gemeten T-hoog-min/max en ingesteld nulpunt SERVOx_HOR
// per struct: pin, hoekmin, hoekmax, pwmnul, pwmrico
static ServoConst servodata[3] = {
	{  9, -22, 120, SERVO0_HOR, (SERVO0_HOR - SERVO0_VER) / 90.0f },
	{ 10, -34, 122, SERVO1_HOR, (SERVO1_HOR - SERVO1_VER) / 90.0f },
	{ 11, -40, 129, SERVO2_HOR, (SERVO2_HOR - SERVO2_VER) / 90.0f }
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
void zoekhoeken(Punt);
void lijn(Punt, Punt, unsigned int, bool);
//void cirkel(Punt startpunt, Punt middelpunt, char richting, unsigned int mmperstap);
//void boog(Punt startpunt, Punt eindpunt, Punt middelpunt, char richting, unsigned int mmperstap);

// ********** Setup *********************************************************************

#ifdef ARDUINO
	// Alleen op Arduino
	void setup()
	{
		byte i;

		// Maak connectie met seriele poort als er output nodig is
		#ifdef DEBUG
			Serial.begin(115200);  // snelheid 115200 baud
			while (!Serial) ;  // wacht totdat de interface beschikbaar is
		#endif

		// Maak de servopins outputs en zet op nul
		for (i = 0; i < 3; ++i) {
			pinMode(servodata[i].pin, OUTPUT);
			digitalWrite(servodata[i].pin, LOW);
		}

		// Voor sprint 2
		// Eén punt om het servosignaal weer te geven op een scoop
		Punt p = { 0, 0, 185 };
		zoekhoeken(p);
		#ifdef DEBUG
			// Instellingen van alle drie de servo's weergeven
			// TODO: weergeven op oscilloscoop en screenshot
			for (i = 0; i < 3; ++i ) {
				Serial.print("servo[");
				Serial.print(i);
				Serial.print("].hoek = ");
				Serial.println(servoinstel[i].hoek, 0);  // geen decimalen
				Serial.print("servo[");
				Serial.print(i);
				Serial.print("].pwm = ");
				Serial.println(servoinstel[i].pwm);
			}
		#endif

		noInterrupts();
		TCCR1A = 0;              // reset register A
		TCCR1B = 0;              // reset register B
		TCNT1 = 25536;           // timer1 counter = 65536 - 16MHz/8/50Hz = 0,02 s tot overflow
		TCCR1B |= (1 << CS11);   // timer1 prescaler 8 = 0,5 us resolutie
		TIMSK1 |= (1 << TOIE1);  // timer1 overflow interrupt enable
		interrupts();
	}
#endif

// ********** Loop **********************************************************************

#ifdef ARDUINO
	// Alleen op Arduino
	void loop()
	{
		delay(100);
	}

	ISR(TIMER1_OVF_vect)
	{
		// Eerst de huidige servopuls uitzetten (0..2 = servonummer, 3 = pauze voor 50 Hz)
		if (servohoog != 3)
			digitalWrite(servodata[servohoog].pin, LOW);
		//PORTB &= B11110001;  // reset pin 9 en 10 en 11

		// Ga naar de volgende servo (3 servo's en 1 pauze, dus modulo 4)
		++servohoog %= 4;

		// Dan die volgende servo aanzetten, of extra pauze voor 50 Hz signaal
		if (servohoog != 3) {
			// Servopuls aanzetten voor servonummer 0..2
			// aftellen tot de ingestelde T-hoog is bereikt
			// (factor 2 vanwege timer resolutie van 0,5 us zit er al in)
			digitalWrite(servodata[servohoog].pin, HIGH);
			//PORTB |= 2 << servohoog;  // set pin 9 of 10 of 11
			TCNT1 = 65535 - servoinstel[servohoog].pwm + 1;
		} else {
			// Pauze voor 50 Hz signaal = volmaken tot 20000 us
			// maal twee, want timer resolutie is 0,5 us, dus begin te tellen bij
			// 65536 - 40000 + wat er al af is vanwege 3x T-hoog van de 3 servo's
			TCNT1 = 25536 + servoinstel[0].pwm + servoinstel[1].pwm + servoinstel[2].pwm;
		}
	}
#endif

// ********** Main **********************************************************************

#ifndef ARDUINO
	// Voor simulatie, niet op een Arduino
	int main(void)
	{
		Punt p0 = { -50, -50, 185 };
		Punt p1 = {  50, -50, 185 };
		Punt p2 = {  50,  50, 185 };
		Punt p3 = { -50,  50, 185 };

		printf("Vierkant:\n");
		printf("\nx,y,z,a0,a1,a2\n");
		lijn(p0, p1, 10, true);
		printf("\nx,y,z,a0,a1,a2\n");
		lijn(p1, p2, 10, false);
		printf("\nx,y,z,a0,a1,a2\n");
		lijn(p2, p3, 10, false);
		printf("\nx,y,z,a0,a1,a2\n");
		lijn(p3, p0, 10, false);

		return 0;
	}
#endif

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
 */
bool doornulheen(float f0, float f1)
{
	#ifdef ARDUINO
		// vergelijk (xor) de signbits van de twee floats
		return ((*(((byte*) &f0) + 3) & 0x80) ^ (*(((byte*) &f1) + 3) & 0x80));
	#else
		// zelfde functionaliteit, snel genoeg voor simulatie
		return ((f0 < 0) != (f1 < 0));
	#endif
}

/**
 * Welke absolute waarde is kleiner?
 * (ligt de eerste dichter bij nul?)
 */
bool eersteiskleiner(float f0, float f1)
{
	#ifdef ARDUINO
		// zet de signbits op nul, vergelijk grootte
		*(((byte*) &f0) + 3) &= 0x7f;  // reset signbit = abs()
		*(((byte*) &f1) + 3) &= 0x7f;
		return (f0 < f1);
	#else
		// zelfde functionaliteit, snel genoeg voor simulatie
		return (fabsf(f0) < fabsf(f1));
	#endif
}

/**
 * Benader de servohoeken en bijbehorende T-hoog
 * param pos: doelpositie (x,y,z) van de Deltarobot
 * resultaat in globale variabele servoinstel[]
 */
void zoekhoeken(Punt pos)
{
	static int richting[3] = { 1, 1, 1 };   // onthoud de richting voor nieuwe hoek
	int hoek0, hoek1, bestehoek, randhoek;  // probeer hoeken, min/max
	float len0, len1;                       // benadering van arm2
	bool klaar;                             // geen iteratie meer nodig

	for (byte i = 0; i < 3; ++i) {
		
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
				bestehoek = hoek0;
				klaar = true;
			} else if (doornulheen(len0, len1)) {
				// Benaderingen hebben verschillend teken: 1 van de 2 is de beste
				bestehoek = eersteiskleiner(len0, len1) ? hoek0 : hoek1;
				klaar = true;
			} else {
				// We moeten verder zoeken in de ingeslagen richting
				bestehoek = hoek1;
			}

		} else {

			// We zitten (nog) niet op een rand dus probeer eerst de oude richting
			hoek1 = hoek0 + richting[i];
			len0 = arm2lengte(i, hoek0, &pos);
			len1 = arm2lengte(i, hoek1, &pos);
			if (doornulheen(len0, len1)) {
				// Benaderingen hebben verschillend teken: 1 van de 2 is de beste
				bestehoek = eersteiskleiner(len0, len1) ? hoek0 : hoek1;
				klaar = true;
			} else if (eersteiskleiner(len0, len1)) {
				// De vorige was dichter bij nul: we gaan de verkeerde kant op!
				richting[i] = -richting[i];  // vanaf nu de andere kant op
				bestehoek = hoek0;           // begin opnieuw vanaf beginwaarde
				len1 = len0;                 // len0 was beter, gebruik als laatste benadering
			} else {
				// We moeten verder zoeken in deze richting
				bestehoek = hoek1;
			}

		}

		if (!klaar) {

			// Houd minimum of maximum in de gaten
			randhoek = (richting[i] > 0) ? servodata[i].hoekmax : servodata[i].hoekmin;

			// Doorzoeken zolang niet op de rand, en benaderingen met hetzelfde teken
			do {
				len0 = len1;                            // vorige onthouden
				bestehoek += richting[i];               // nieuwe hoek poging
				len1 = arm2lengte(i, bestehoek, &pos);  // nieuwe benadering van arm2
			} while (bestehoek != randhoek && !doornulheen(len0, len1));

			// We zitten nu of op de rand, of we hebben de beste benadering gevonden
			if (bestehoek != randhoek)
				// Niet op de rand, dus de benaderingen hebben verschillend teken:
				// 1 van de 2 is de beste, check welke
				if (eersteiskleiner(len0, len1))
					bestehoek -= richting[i];
		}

		// Nieuwe servo instelling
		servoinstel[i].hoek = bestehoek;
		// Voor het zetten van .pwm geen interrupts, want de waarde wordt gebruikt in de ISR
		// en het is een int dus de ene byte kan veranderd zijn en de andere niet terwijl er
		// net een interrupt afgaat (en het zijn sowieso twee statements)
		#ifdef ARDUINO
			noInterrupts();
		#endif
		servoinstel[i].pwm = servodata[i].pwmnul - (servodata[i].pwmrico * bestehoek);
		servoinstel[i].pwm <<= 1;  // maal twee want timer resolutie is 0,5 us.
		#ifdef ARDUINO
			interrupts();
		#endif
	}
	#ifdef DEBUG
		#ifdef ARDUINO
			// Alleen op Arduino
			Serial.print(pos.x, 0);
			Serial.print(",");
			Serial.print(pos.y, 0);
			Serial.print(",");
			Serial.print(pos.z, 0);
			Serial.print(",");
			Serial.print(servoinstel[0].hoek);
			Serial.print(",");
			Serial.print(servoinstel[1].hoek);
			Serial.print(",");
			Serial.println(servoinstel[2].hoek);
		#else
			// Voor simulatie, niet op een Arduino
			printf("%.0f,%.0f,%.0f,%i,%i,%i\n",
				pos.x, pos.y, pos.z,
				servoinstel[0].hoek, servoinstel[1].hoek, servoinstel[2].hoek);
		#endif
	#endif
}

/**
 * Beweeg de Deltarobot in een rechte lijn van A naar B
 * param startpunt: van Punt A (x,y,z)
 * param eindpunt: naar Punt B (x,y,z)
 * param mmperstap: verplaatsing in mm per stap in de 3D-beweging (0 = in 1 keer)
 * param eerstnaarstart: eerst naar het startpunt bewegen, of is dit een voortzetting?
 */
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
