////////// Includes ///////////////////////////////////////////////////////////

#include <stdio.h>      // printf, niet nodig op ARM-bord maar kan geen kwaad
#include "speelveld.h"

////////// Globals ////////////////////////////////////////////////////////////

// Middelpunt van het speelveld. Gebruik dit als referentiepunt om je positie
// te uploaden. Upload alleen het verschil met deze locatie, tot de 5e decimaal
// nauwkeurig, maar dan als (signed) char. Stel: jouw lat = 52.080751, dan is
// het verschil +0.000151, daar maak je van: +15, oftewel in hex: 0F.
// Negatieve verschillen eerst unsigned maken, dan pas naar hex. Dus stel dat
// het verschil -4 is, dat is unsigned: 252, en hex: FC. Je complete locatie
// past daarmee in 2 (signed) chars = 4 hex digits = 4 data bytes voor LoRa.
const VELD_GPS veld_midden = { 52.08060f, 5.17386f };

// Randpunten van het speelveld. De punten definieren een gesloten veelhoek
// (polygoon). Gebruik de functie veld_binnen() om te testen of een bepaald
// punt binnen of buiten de grenzen valt.
const VELD_GPS veld_rand[] = {
	{ 52.081190f, 5.172981f },  // NW, bij het hek op de weg
	{ 52.080570f, 5.173020f },  // W, begin van de sloot
	{ 52.080040f, 5.174160f },
	{ 52.080010f, 5.174420f },  // Z, einde van de sloot
	{ 52.080240f, 5.174745f },
	{ 52.080764f, 5.173961f },  // centraal, hoekpunt van het terrein van de kinderopvang
	{ 52.081145f, 5.174594f },
	{ 52.081190f, 5.174594f }   // NO, langs de weg
};

// Mogelijke locaties van de verstopper. Vraag een locatie aan
// d.m.v. de functie veld_zoekpunt(). Resultaat is afhankelijk
// van het aantal aanroepen en je groepsnummer, zie speelveld.h.
const VELD_GPS veld_zoek[] = {
	{ 52.079216f, 5.170761f },  // ZW (ver weg van het speelveld)
	{ 52.080728f, 5.173576f },  // centraal (open)
	{ 52.081118f, 5.174240f },  // NO (open)
	{ 52.081050f, 5.173570f },  // NW (bomen)
	{ 52.080261f, 5.174495f },  // ZO (boom, dichtbij rand)
	{ 52.080293f, 5.173786f }   // Z  (bomen, naast sloot)
};

////////// Function Definitions ///////////////////////////////////////////////

// Is deze positie op het speelveld? (= binnen de grenzen van de gesloten polygoon?)
// Benodigd globaal: veld_rand[] met meer dan 2 punten
// Arg: pointer naar GPS locatie
// Ret: 0 = nee = buiten het speelveld, 1 = ja = binnen het speelveld
unsigned int veld_binnen(VELD_PGPS pos)
{
	// Array-lengte van de polygoon die de rand van het speelveld beschrijft.
	static const unsigned int randpunten = (sizeof veld_rand) / (sizeof veld_rand[0]);

	// Deze initiele waardes worden de eerste keer aangepast.
	static unsigned int firstrun = 1;
	static VELD_GPS veldlim[] = {
		{  90.0f,  180.0f },  // index 0 = minimale waardes van lat/lon
		{ -90.0f, -180.0f }   // index 1 = maximale waardes van lat/lon
	};

	// Test het aantal kruisingen met de rand van het speelveld
	// naar 1 kant toe (in dit geval naar lat = 0 = evenaar).
	// Als dit oneven is, dan is de positie wel op het veld.
	unsigned int oneven = 0;

	// Indices van twee naastliggende punten, dus i<->j is altijd een lijnstuk
	unsigned int i, j = randpunten - 1;
	float f;  // hulpvariabele

	// Null pointer = geen locatie => niet op het veld
	if (!pos)
		return 0;

	// Uiterste limieten van het speelveld (= de omsluitende rechthoek).
	// De beginwaardes worden de 1e keer aangepast, anders werkt de
	// volgende check niet.
	if (firstrun)
	{
		firstrun = 0;
		for (i = 0; i < randpunten; ++i)  // bepaal min/max van alle punten
		{
			f = veld_rand[i].lat;          // huidige lat
			if (f < veldlim[0].lat)       // kleiner?
				veldlim[0].lat = f;       // minimale lat
			else if (f > veldlim[1].lat)  // groter?
				veldlim[1].lat = f;       // maximale lat
			f = veld_rand[i].lon;          // huidige lon
			if (f < veldlim[0].lon)       // kleiner?
				veldlim[0].lon = f;       // minimale lon
			else if (f > veldlim[1].lon)  // groter?
				veldlim[1].lon = f;       // maximale lon
		}
	}

	// Niet op het veld als het buiten de buitenste randen is
	if (pos->lat < veldlim[0].lat ||
		pos->lat > veldlim[1].lat ||
		pos->lon < veldlim[0].lon ||
		pos->lon > veldlim[1].lon)
		return 0;

	for (i = 0; i < randpunten; ++i)
	{
		// Binnen de lon range van dit lijnstuk?
		if ((pos->lon > veld_rand[i].lon && pos->lon <= veld_rand[j].lon) ||
			(pos->lon > veld_rand[j].lon && pos->lon <= veld_rand[i].lon))
		{
			// Aan de +lat kant van het lijnstuk? 
			if (veld_rand[i].lat +
				(pos->lon - veld_rand[i].lon) /
				(veld_rand[j].lon - veld_rand[i].lon) *
				(veld_rand[j].lat - veld_rand[i].lat) < pos->lat)
			{
				// Dit lijnstuk moet gekruist worden richting evenaar
				// TODO: werkt verkeerd om op het zuidelijk halfrond
				++oneven;
			}
		}
		j = i;  // volgende lijnstuk
	}
	return oneven % 2;  // 0=niet op het speelveld, 1=wel op het speelveld
}

// Vraag de (volgende) positie van de verstopper. De waarde die je krijgt, is
// afhankelijk van je groepsnummer (aanpassen in speelveld.h!) en hoe vaak je
// deze functie aanroept: telkens het volgende punt.
// Benodigd globaal: veld_zoek[] met 1 of meer punten
// Ret: GPS locatie (struct met 2 floats)
VELD_GPS veld_zoekpunt(void)
{
	static const unsigned int aantalpunten = (sizeof veld_zoek) / (sizeof veld_zoek[0]);
	static unsigned int n = VELD_PROJECTGROEP - 1;  // begin met 0 want meteen ++

	++n;                // volgende punt
	n %= aantalpunten;  // blijf binnen de array

	return (VELD_GPS){ veld_zoek[n].lat, veld_zoek[n].lon };
}

////////// Main ///////////////////////////////////////////////////////////////

// Voorbeeldprogramma hoe je de gegevens en functies in deze bestanden kunt
// gebruiken. Verwijder deze hele main() functie zodra je de bestanden in je
// programma op het ARM-bord zet! Anders zijn er twee mains.
int main(void)
{
	VELD_GPS zoeker, verstopper;
	float staplat, staplon;
	unsigned int i, stappen = 10;

	verstopper = veld_zoekpunt();  // 1e locatie van de verstopper
	zoeker     = veld_midden;       // beginlocatie van de zoeker

	staplat = (verstopper.lat - zoeker.lat) / (float)stappen;
	staplon = (verstopper.lon - zoeker.lon) / (float)stappen;
	for (i = 0; i <= stappen; ++i)
	{
		printf("%2u: %.6f,%.6f = %s\n", i, zoeker.lat, zoeker.lon, veld_binnen(&zoeker) ? "binnen" : "buiten");
		zoeker.lat += staplat;
		zoeker.lon += staplon;
	}

	return 0;
}
