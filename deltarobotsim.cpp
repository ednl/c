// ********** Bibliotheken **************************************************************

#include <stdio.h>  // printf

// ********** Definities ****************************************************************

/**
 * Deltarobot afmetingen
 *   L_TRI1 = lengte (mm) van driehoekzijde van de basisplaat (die stil staat)
 *   L_TRI2 = lengte (mm) van driehoekzijde van de topplaat (die beweegt)
 *   L_ARM1 = lengte (mm) van de onderste arm (die aan de servo vastzit)
 *   L_ARM2 = lengte (mm) van de bovenste arm (die aan de topplaat vastzit)
 * Wiskundige constanten
 *   WORTEL3 = sqrt(3)
 * Servo waardes (T-hoog)
 *   minimum, maximum, horizontaal, verticaal
 */
#define L_TRI1   175
#define L_TRI2    50  // of ~49.5; nog een keer meten!
#define L_ARM1    50
#define L_ARM2   202
#define WORTEL3    1.73205

#define SERVO0_MIN 620
#define SERVO0_MAX 2200
#define SERVO0_HOR 1700
#define SERVO0_VER 700

#define SERVO1_MIN 700
#define SERVO1_MAX 2300
#define SERVO1_HOR 1700
#define SERVO1_VER 780

#define SERVO2_MIN 660
#define SERVO2_MAX 2350
#define SERVO2_HOR 1700
#define SERVO2_VER 800

// ********** Globale constanten voor snelle berekening *********************************

static const double sinustabel[91] = {            // sinus van 0 t/m 90 graden
	0.0     ,0.017452,0.034899,0.052336,0.069756,0.087156,0.104528,0.121869,0.139173,0.156434,
	0.173648,0.190809,0.207912,0.224951,0.241922,0.258819,0.275637,0.292372,0.309017,0.325568,
	0.342020,0.358368,0.374607,0.390731,0.406737,0.422618,0.438371,0.453990,0.469472,0.484810,
	0.5     ,0.515038,0.529919,0.544639,0.559193,0.573576,0.587785,0.601815,0.615661,0.629320,
	0.642788,0.656059,0.669131,0.681998,0.694658,0.707107,0.719340,0.731354,0.743145,0.754710,
	0.766044,0.777146,0.788011,0.798636,0.809017,0.819152,0.829038,0.838671,0.848048,0.857167,
	0.866025,0.874620,0.882948,0.891007,0.898794,0.906308,0.913545,0.920505,0.927184,0.933580,
	0.939693,0.945519,0.951057,0.956305,0.961262,0.965926,0.970296,0.974370,0.978148,0.981627,
	0.984808,0.987688,0.990268,0.992546,0.994522,0.996195,0.997564,0.998630,0.999391,0.999848,
	1.0
};

// ********** Structs en globale variabelen *********************************************

// een punt of een vector
typedef struct {
	double x, y, z;
} punt;

// kenmerken van alle aangesloten servo's
typedef struct {
	int pin;        // Arduino pinnummer
	int hoek;       // hoek van arm1 met het xy-vlak
	int pwm;        // T-hoog van het PWM-signaal
	int t_min;      // T-hoog minimum
	int t_max;      // T-hoog maximum
	int t_nul;      // basis voor T-hoog berekening
	double t_rico;  // factor voor T-hoog berekening
} servoinfo;

servoinfo robot[3] = {
	{ .pin = 9,
	  .hoek = 0, .hoekmin = -30, .hoekmax = 120,
	  .pwm = SERVO0_HOR, .pwmmin = SERVO0_MIN, .pwmmax = SERVO0_MAX,
	  .pwmnul = SERVO0_HOR, .pwmrico = (SERVO0_HOR - SERVO0_VER) / 90.0 },
	{ .pin = 10,
	  .hoek = 0, .hoekmin = -30, .hoekmax = 120,
	  .pwm = SERVO1_HOR, .pwmmin = SERVO1_MIN, .pwmmax = SERVO1_MAX,
	  .pwmnul = SERVO1_HOR, .pwmrico = (SERVO1_HOR - SERVO1_VER) / 90.0 },
	{ .pin = 11,
	  .hoek = 0, .hoekmin = -30, .hoekmax = 120,
	  .pwm = SERVO2_HOR, .pwmmin = SERVO2_MIN, .pwmmax = SERVO2_MAX,
	  .pwmnul = SERVO2_HOR, .pwmrico = (SERVO2_HOR - SERVO2_VER) / 90.0 }
};

// ********** Functie prototypes ********************************************************

double sinus(int graden);
double cosinus(int graden);
double arm2lengte(int n, int a, punt p);
void zoekhoeken(punt doel);

// ********** Main **********************************************************************

int main(void)
{
	punt positie;
	int i;

	printf("x,y,z,a0,a1,a2\n");

	positie.x = -20.0;
	while (positie.x <= 20.0) {
		positie.y = -20.0;
		while (positie.y <= 20.0) {
			positie.z = 180.0;
			while (positie.z <= 210.0) {

				benadering = zoekhoeken(positie);
				printf("%.0f,%.0f,%.0f,%i,%i,%i\n", positie.x, positie.y, positie.z, benadering.a[0], benadering.a[1], benadering.a[2]);

				// Bewaar hoeken en reken om naar T-hoog
				for (i = 0; i < 3; ++i) {

					//TODO: check eerst of de hoeken wel kunnen/mogen
					robothoek.a[i] = benadering.a[i];

					//TODO: er moet nog een of andere factor tussen
					myservo[i].pwm = 1500 + robothoek.a[i];

					//TODO: zet dit in een functie, vraag aan product owner wat ie precies wil
					/*
					digitalWrite(myservo[i].pin, HIGH);
					delayMicroseconds(myservo[i].pwm);
					digitalWrite(myservo[i].pin, LOW);
					*/
				}
				positie.z += 1.0;
			}
			positie.y += 10.0;
		}
        positie.x += 10.0;
    }
    return 0;
}

// ********** Functie implementaties ****************************************************

/**
 * Sinus via opzoektabel
 *   param graden: hoek in graden
 *   return: sinus van de hoek
 */
double sinus(int graden)
{
	// range = [-179 .. +180]
	while (graden <= -180)
		graden += 360;
	while (graden > 180)
		graden -= 360;

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
double cosinus(int graden)
{
	// range = [-179 .. +180]
	while (graden <= -180)
		graden += 360;
	while (graden > 180)
		graden -= 360;

	if (graden >= 90)
		return -sinustabel[graden - 90];  // 2e kwadrant = sinus 3e kwadrant
	if (graden >= 0)
		return sinustabel[90 - graden];   // 1e kwadrant = sinus 2e kwadrant
	if (graden >= -90)
		return sinustabel[90 + graden];   // 4e kwadrant = sinus 1e kwadrant
	return -sinustabel[-graden - 90];     // 3e kwadrant = sinus 4e kwadrant
}

/**
 * Bereken afwijking in lengte (in het kwadraat) van arm2 voor één servo
 *   param n: servonummer 0/1/2
 *   param a: servohoek in graden
 *   param p: doelpositie (x,y,z) van de deltarobot
 *   return: afwijking in lengte (kwadraat) van de arm2 vector
 */
double arm2lengte(int n, int a, punt p)
{
	static const double k0 = 0.5 * (L_TRI1 - L_TRI2);          // (1/2) . (T1 - T2)
	static const double k1 = WORTEL3 * (L_TRI1 - L_TRI2) / 3;  // (1/3) . sqrt(3) . (T1 - T2)
	static const double k2 = WORTEL3 * (L_TRI1 - L_TRI2) / 6;  // (1/6) . sqrt(3) . (T1 - T2)
	static const double m0 = 0.5 * L_ARM1;                     // (1/2) . A1
	static const double m1 = 0.5 * WORTEL3 * L_ARM1;           // (1/2) . sqrt(3) . A1
	static const double m2 = L_ARM2 * L_ARM2;                  // A2 . A2 = inproduct van arm2 vector

	double cosa = cosinus(a);
	switch (n) {
		case 0:
			p.x -= k1 + L_ARM1 * cosa;
			break;
		case 1:
			p.x += k2 + m0 * cosa;
			p.y -= k0 + m1 * cosa;
			break;
		case 2:
			p.x += k2 + m0 * cosa;
			p.y += k0 + m1 * cosa;
			break;
	}
	p.z -= L_ARM1 * sinus(a);

	return p.x * p.x + p.y * p.y + p.z * p.z - m2;
}

/**
 * Benader de servohoeken en bijbehorende T-hoog
 *   param doel: doelpositie (x,y,z) van de deltarobot
 */
void zoekhoeken(punt doel)
{
	static int richting[3] = { 1, 1, 1 };
	int testhoek;
	double len0, len1, diff;  // benadering

	for (int i = 0; i < 3; ++i) {
		testhoek = robot[i].hoek;

		len0 = arm2lengte(i, testhoek, doel);
		testhoek += richting[i];
		len1 = arm2lengte(i, testhoek, doel);

		if ((*(((byte*) &len0) + 3) & 0x80) ^ (*(((byte*) &len1) + 3) & 0x80)) {

			// signbit verschillend => 1 van de 2 is beste benadering
			*(((byte*) &len0) + 3) &= 0x7f;  // reset signbit = abs()
			*(((byte*) &len1) + 3) &= 0x7f;
			if (len0 < len1)
				testhoek -= richting[i];  // len0 was betere benadering

		} else {

			// signbit hetzelfde => verder op zoek
			if (*(((byte*) &len0) + 3) & 0x80)
				diff = len0 - len1;  // allebei negatief
			else
				diff = len1 - len0;  // allebei positief
			if (diff > 0) {
				// we gaan de verkeerde kant op
				testhoek -= richting[i];     // terug naar beginwaarde
				richting[i] = -richting[i];  // vanaf nu de andere kant op
				len1 = len0;                 // len0 was beter
			}

			do {
				len0 = len1;                           // vorige onthouden
				testhoek += richting[i];               // nieuwe hoek
				len1 = arm2lengte(i, testhoek, doel);  // nieuwe benadering van arm2
			} while (!((*(((byte*) &len0) + 3) & 0x80) ^ (*(((byte*) &len1) + 3) & 0x80)));

			// signbit verschillend => 1 van de 2 is beste benadering
			*(((byte*) &len0) + 3) &= 0x7f;  // reset signbit = abs()
			*(((byte*) &len1) + 3) &= 0x7f;
			if (len0 < len1)
				testhoek -= richting[i];  // vorige was betere benadering
		}
		robot[i].hoek = testhoek;
		robot[i].pwm = robot[i].pwmnul - (robot[i].pwmrico * testhoek);
	}
}
