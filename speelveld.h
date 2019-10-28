////////// Defines ////////////////////////////////////////////////////////////

// Jouw projectgroepnummer. Aanpassen naar het juiste nummer!
#define VELD_PROJECTGROEP 1

////////// Typedefs ///////////////////////////////////////////////////////////

// Struct met latitude en longitude als floats, in decimale graden.
// Bijvoorbeeld: het practicumlokaal = 52.084123,5.168637
typedef struct Veld_Gps {
	float lat, lon;
} VELD_GPS, *VELD_PGPS;

////////// Global Declarations ////////////////////////////////////////////////

extern const VELD_GPS veld_midden;  // middelpunt van het speelveld, referentie voor differentiele upload
extern const VELD_GPS veld_rand[];  // punten die de randen van het speelveld bepalen
extern const VELD_GPS veld_zoek[];  // mogelijke locaties van de verstopper

////////// Function Declarations //////////////////////////////////////////////

unsigned int veld_binnen   (VELD_PGPS);  // check of een punt op het speelveld is, of erbuiten
VELD_GPS     veld_zoekpunt (void);       // vraag de (volgende) positie van de verstopper
