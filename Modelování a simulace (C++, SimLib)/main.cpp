/*
	IMS projekt - Minecraft server
	Marek Sipos (xsipos03)
*/


#include <iostream>
#include <cmath>
#include "simlib.h"

using namespace std;

/* PARAMETRY */
const int PLAYERS = 10; // Pocet hracu na serveru
const int MAX_THREADS = 1000; // Maximum async vlaken pro zpracovani pluginu
const int SIMULATION_LENGTH = 1000; // Horni mez casu simulace (sekund)

/* GLOBALNI PROMENNE */
bool nerfed_entities = false; // Je aktivni rezim Nerfed entities?
bool genmap_tick = false; // Probehlo jiz generovani mapy v ticku?
bool loadmap_tick = false; // Probehlo jiz nacteni mapy v ticku?

/* MODELOVY CAS */
unsigned current_second = 0; // Aktualni sekunda (od nulte)
double tps[SIMULATION_LENGTH]; // Namerene hodnoty v case (po sekundach)
double cpu_time = 0.0; // Spotrebovany cas CPU v aktualni sekunde


/* LINKY */
Facility linka_sync("Sync");
Store linka_async("Async", MAX_THREADS);

/***********************************/

// Zaokrouhli cislo nahoru na dany nasobek (napr. aligned = 142.3, aligner = 50.0, vysledek = 150.0)
double Align_to_upper(double aligned, double aligner) {
	return ceil(aligned/aligner)*aligner;
}

// Zaznamena cas CPU do statistik
void mark_cpu(double time_to_mark) {
	cpu_time += time_to_mark;
}

// Pokud uz cas pokrocil, zpracuje novy tick a sekundu
void check_time() {
	if (current_second >= SIMULATION_LENGTH) return;

	static double last_time = 0.0;
	static unsigned last_percent = 0;

	// Cas se uz prehoupl o tick, takze mapu lze znovu zpracovat
	if (Time >= Align_to_upper(last_time, 50.0)) {
		genmap_tick = false;
		loadmap_tick = false;
	}

	// Cas se uz prehoupl pres sekundu, takze vypocitame TPS
	while (Time >= Align_to_upper(last_time, 1000.0)) {
		tps[current_second] = 20.0 * (1000.0 / cpu_time);
		if (tps[current_second] > 20.0) tps[current_second] = 20.0;

		nerfed_entities = (tps[current_second] < 19.0);

		cpu_time = 0.0;
		last_time += 1000.0;
		current_second++;
	}

	unsigned actual_percent = ceil((Time / (SIMULATION_LENGTH*1000)) * 100);
	if (actual_percent > last_percent) {
		last_percent = actual_percent;
		cout << actual_percent << "% ";
		std::cout.flush();
	}

	last_time = Time;
}

// Vytiskne prubeh TPS v case
void Print_tps() {
	cout << "------- TPS ------" << endl;
	for (int i = 0; i < SIMULATION_LENGTH; i++) {
		if (i < SIMULATION_LENGTH-1)
			cout << tps[i] << ", ";
		else
			cout << tps[i] << endl;
	}
	cout << "------------------" << endl;
}

/***********************************/

class Pozadavek_entita : public Process {
	void Behavior() {
		Seize(linka_sync);
		Wait(nerfed_entities ? 0.035 : 0.002);
		mark_cpu(nerfed_entities ? 0.035 : 0.002);
		Release(linka_sync);
		check_time();
	}
};

class Pozadavek_genmap : public Process {
	void Behavior() {
		if (Uniform(0,100) <= 1) {
			if (!genmap_tick && PLAYERS > 0) {
				genmap_tick = true;
				Seize(linka_sync);
				Wait(7.0+(3.0*PLAYERS));
				mark_cpu(7.0+(3.0*PLAYERS));
				Release(linka_sync);
				check_time();
			}
		}
	}
};

class Pozadavek_loadmap : public Process {
	void Behavior() {
		if (Uniform(0,100) <= 10) {
			if (!loadmap_tick && PLAYERS > 0) {
				loadmap_tick = true;
				Seize(linka_sync);
				Wait(0.02+(0.01*PLAYERS));
				mark_cpu(0.02+(0.01*PLAYERS));
				Release(linka_sync);
				check_time();
			}
		}
	}
};

class Pozadavek_plugin : public Process {
	void Behavior() {
		if (!linka_async.Full()) { // Async p=1
			Enter(linka_async, 1);
			Wait(0.005*PLAYERS);
			Leave(linka_async, 1);
		} else { // Sync p=0
			Seize(linka_sync, 1);
			Wait(0.005*PLAYERS);
			mark_cpu(0.005*PLAYERS);
			Release(linka_sync);
			check_time();
		}
	}
};

class Pozadavek_gc : public Process {
	void Behavior() {
		Priority = 99; // GC ma pravo zablokovat prednostne
		Seize(linka_sync);
		Enter(linka_async, MAX_THREADS);
		Wait(500.0);
		mark_cpu(500.0);
		Leave(linka_async, MAX_THREADS);
		Release(linka_sync);
		check_time();
	}
};

/***********************************/

class Generator_entita : public Event {
	void Behavior() {
		(new Pozadavek_entita)->Activate();
		Activate(Time+Exponential(nerfed_entities ? 1.8 : 0.2));
	}
};

class Generator_genmap : public Event {
	void Behavior() {
		(new Pozadavek_genmap)->Activate();
		Activate(Time+1.0);
	}
};

class Generator_loadmap : public Event {
	void Behavior() {
		(new Pozadavek_loadmap)->Activate();
		Activate(Time+1.0);
	}
};

class Generator_plugin : public Event {
	void Behavior() {
		(new Pozadavek_plugin)->Activate();
		Activate(Time+Exponential(1.0));
	}
};

class Generator_gc : public Event {
	void Behavior() {
		(new Pozadavek_gc)->Activate();
		Activate(Time+200000);
	}
};

/***********************************/

int main()
{

	cout << "Simulace spustena: ";
	std::cout.flush();
	/* --------- JEDNA SIMULACE --------- */

	RandomSeed(time(NULL));
	Init(0.0, SIMULATION_LENGTH*1000); // modelovy cas 0-1000 sekund

	/* Spousteni generatoru */
	(new Generator_entita)->Activate(Time+Exponential(nerfed_entities ? 1.8 : 0.2));
	(new Generator_genmap)->Activate(Time+1.0);
	(new Generator_loadmap)->Activate(Time+1.0);
	(new Generator_plugin)->Activate(Time+Exponential(1.0));
	(new Generator_gc)->Activate(Time+200000);


	Run();

	linka_sync.Output();
	linka_async.Output();

	Print_tps();
}
