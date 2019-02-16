// ********** Bibliotheken **************************************************************

#include <stdio.h>  // printf
#include <math.h>   // sqrt

// ********** Definities ****************************************************************

#define DEBUG 1  // print tussenstappen ja (1) of nee (0)

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

// Wiskundige constante
#define WORTEL3 1.73205  // sqrt(3)

// ********** Typedefs ******************************************************************

// Arduino type voor simulatie
typedef unsigned char byte;

// Struct voor een punt of een vector
typedef struct {
	float x, y, z;
} punt;

// Struct voor vaste waarden van de Deltarobot servo's
typedef const struct {
	byte pin;             // Arduino pinnummer
	int hoekmin;          // minimale hoek van arm1 met het xy-vlak
	int hoekmax;          // maximale hoek van arm1 met het xy-vlak
	unsigned int pwmnul;  // T-hoog (us) voor hoek = 0
	float pwmrico;        // T-hoog richtingscoefficient (us/graad)
} servo_const;

// Struct voor variabelen van de Deltarobot servo's
typedef struct {
	int hoek;          // hoek van arm1 met het xy-vlak
	unsigned int pwm;  // T-hoog van het PWM-signaal
} servo_var;

// ********** Globale constanten ********************************************************

// Vaste waarden van de Deltarobot servo's
servo_const servodata[3] = {
	{ .pin = 9,
		.hoekmin = -30, .hoekmax = 120,
		.pwmnul = SERVO0_HOR, .pwmrico = (SERVO0_HOR - SERVO0_VER) / 90.0f },
	{ .pin = 10,
		.hoekmin = -30, .hoekmax = 120,
		.pwmnul = SERVO1_HOR, .pwmrico = (SERVO1_HOR - SERVO1_VER) / 90.0f },
	{ .pin = 11,
		.hoekmin = -30, .hoekmax = 120,
		.pwmnul = SERVO2_HOR, .pwmrico = (SERVO2_HOR - SERVO2_VER) / 90.0f }
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
servo_var servoinstel[3] = {
	{ .hoek = 0, .pwm = SERVO0_HOR },
	{ .hoek = 0, .pwm = SERVO1_HOR },
	{ .hoek = 0, .pwm = SERVO2_HOR }
};

// ********** Functie prototypes ********************************************************

float sinus(int graden);
float cosinus(int graden);
float arm2lengte(byte n, int a, punt p);
bool doornulheen(float f0, float f1);
bool eersteiskleiner(float f0, float f1);
void zoekhoeken(punt pos);
void lijn(punt startpunt, punt eindpunt, unsigned int mmperstap);
//void cirkel(punt startpunt, punt middelpunt, char richting, unsigned int mmperstap);
//void boog(punt startpunt, punt eindpunt, punt middelpunt, char richting, unsigned int mmperstap);

// ********** Main **********************************************************************

int main(void)
{
	punt p0 = { -20, -20, 190 };
	punt p1 = { 20, -20, 190 };
	punt p2 = { 20, 20, 190 };
	punt p3 = { -20, 20, 190 };

	printf("Vierkant:\n");
	printf("\nx,y,z,a0,a1,a2\n");
	lijn(p0, p1, 2);
	printf("\nx,y,z,a0,a1,a2\n");
	lijn(p1, p2, 2);
	printf("\nx,y,z,a0,a1,a2\n");
	lijn(p2, p3, 2);
	printf("\nx,y,z,a0,a1,a2\n");
	lijn(p3, p0, 2);

	return 0;
}

// ********** Functie implementaties ****************************************************

/**
 * Sinus via opzoektabel
 *   param graden: hoek in graden
 *   return: sinus van de hoek
 */
float sinus(int graden)
{
	// bereik = [-179..180]
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
	return -sinustabel[180 + graden];   // 3e kwadrant
}

/**
 * Cosinus via opzoektabel
 *   param graden: hoek in graden
 *   return: cosinus van de hoek
 */
float cosinus(int graden)
{
	// bereik = [-179..180]
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
	return -sinustabel[-graden - 90];   // 3e kwadrant = sinus 4e kwadrant
}

/**
 * Bereken afwijking in lengte (in het kwadraat) van arm2 voor één servo
 *   param n: servonummer 0/1/2
 *   param a: servohoek in graden
 *   param p: doelpositie (x,y,z) van de deltarobot
 *   return: afwijking in lengte (kwadraat) van de arm2 vector
 */
float arm2lengte(byte n, int a, punt p)
{
	static const float k0 = 0.5 * (L_TRI1 - L_TRI2);          // (1/2) . (T1 - T2)
	static const float k1 = WORTEL3 * (L_TRI1 - L_TRI2) / 3;  // (1/3) . sqrt(3) . (T1 - T2)
	static const float k2 = WORTEL3 * (L_TRI1 - L_TRI2) / 6;  // (1/6) . sqrt(3) . (T1 - T2)
	static const float m0 = 0.5 * L_ARM1;                     // (1/2) . A1
	static const float m1 = 0.5 * WORTEL3 * L_ARM1;           // (1/2) . sqrt(3) . A1
	static const float m2 = 1.0 * L_ARM2 * L_ARM2;            // A2 . A2 = inproduct van arm2 vector

	float cosa = cosinus(a);
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
 * Hebben ze een verschillend teken (de ene negatief, de andere nul of positief)
 */
bool doornulheen(float f0, float f1)
{
	return ((*(((byte*) &f0) + 3) & 0x80) ^ (*(((byte*) &f1) + 3) & 0x80));
}

/**
 * Welke absolute waarde is kleiner (dichter bij nul is beter)
 */
bool eersteiskleiner(float f0, float f1)
{
	*(((byte*) &f0) + 3) &= 0x7f;  // reset signbit = abs()
	*(((byte*) &f1) + 3) &= 0x7f;
	return (f0 < f1);
}

/**
 * Benader de servohoeken en bijbehorende T-hoog
 * param pos: doelpositie (x,y,z) van de Deltarobot
 * resultaat in globale variabele servoinstel[]
 */
void zoekhoeken(punt pos)
{
	static int richting[3] = { 1, 1, 1 };   // onthoud de richting voor nieuwe hoek
	int hoek0, hoek1, bestehoek, randhoek;  // probeer hoeken
	float len0, len1;                       // benadering van arm2
	bool klaar;                             // geen iteratie meer nodig

	for (byte i = 0; i < 3; ++i) {
		
		// Beginwaarden
		klaar = false;
		hoek0 = servoinstel[i].hoek;
		
		// Check randen, doe 1e poging
		if (hoek0 <= servodata[i].hoekmin) {

			// De eerste hoek is meteen al op of onder het minimum
			richting[i] = 1;
			hoek0 = servodata[i].hoekmin;
			hoek1 = hoek0 + 1;
			len0 = arm2lengte(i, hoek0, pos);
			len1 = arm2lengte(i, hoek1, pos);
			if (eersteiskleiner(len0, len1)) {
				// Eerste was beter, maar die hoek was al minimaal, dus klaar
				bestehoek = hoek0;
				klaar = true;
			} else if (doornulheen(len0, len1)) {
				// Benaderingen hebben verschillend teken: 1 van de 2 is de beste
				bestehoek = eersteiskleiner(len0, len1) ? hoek0 : hoek1;
				klaar = true;
			} else {
				// We moeten verder zoeken in deze richting
				bestehoek = hoek1;
			}
			

		} else if (hoek0 >= servodata[i].hoekmax) {

			// De eerste hoek is meteen al op of boven het maximum
			richting[i] = -1;
			hoek0 = servodata[i].hoekmax;
			hoek1 = hoek0 - 1;
			len0 = arm2lengte(i, hoek0, pos);
			len1 = arm2lengte(i, hoek1, pos);
			if (eersteiskleiner(len0, len1)) {
				// Eerste was beter, maar die hoek was al maximaal, dus klaar
				bestehoek = hoek0;
				klaar = true;
			} else if (doornulheen(len0, len1)) {
				// Benaderingen hebben verschillend teken: 1 van de 2 is de beste
				bestehoek = eersteiskleiner(len0, len1) ? hoek0 : hoek1;
				klaar = true;
			} else {
				// We moeten verder zoeken in deze richting
				bestehoek = hoek1;
			}

		} else {

			// We zitten (nog) niet op een rand dus probeer eerst de oude richting
			hoek1 = hoek0 + richting[i];
			len0 = arm2lengte(i, hoek0, pos);
			len1 = arm2lengte(i, hoek1, pos);
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
				len0 = len1;                           // vorige onthouden
				bestehoek += richting[i];              // nieuwe hoek poging
				len1 = arm2lengte(i, bestehoek, pos);  // nieuwe benadering van arm2
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
		servoinstel[i].pwm = servodata[i].pwmnul - (servodata[i].pwmrico * bestehoek);
	}
	if (DEBUG)
		printf("%.0f,%.0f,%.0f,%i,%i,%i\n", pos.x, pos.y, pos.z, servoinstel[0].hoek, servoinstel[1].hoek, servoinstel[2].hoek);
}

/**
 * Beweeg de Deltarobot in een rechte lijn van A naar B
 * param mmperstap: verplaatsing in mm per stap in de 3D-beweging (0 => in 1 keer)
 */
void lijn(punt startpunt, punt eindpunt, unsigned int mmperstap)
{
	float dx, dy, dz;
	unsigned int i, stappen = 0;

	if (mmperstap) {
		dx = eindpunt.x - startpunt.x;
		dy = eindpunt.y - startpunt.y;
		dz = eindpunt.z - startpunt.z;
		stappen = 0.5 + sqrt(dx*dx + dy*dy + dz*dz) / mmperstap;
		if (stappen > 1) {
			// Verplaatsing per stap in x/y/z-richting
			dx /= stappen;
			dy /= stappen;
			dz /= stappen;
		}
	}

	zoekhoeken(startpunt);  // begin hier
	for (i = 1; i < stappen; ++i) {  // vanaf 1 want neem alleen tussenpunten
		startpunt.x += dx;
		startpunt.y += dy;
		startpunt.z += dz;
		zoekhoeken(startpunt);
	}
	zoekhoeken(eindpunt);  // eindig hier
}
