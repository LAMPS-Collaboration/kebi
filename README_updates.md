//-----------------------------------------------

June 24, 2021 (kimc)

- Added/Modified macros and files:

	a. LAMPS-HighEnergy/macros_tpc/kbpar_g4event.conf
	b. LAMPS-HighEnergy/macros_tpc/input/kbpar_sim.conf
	e. LAMPS-HighEnergy/macros_tpc/input/single_particle.gen
	d. LAMPS-HighEnergy/macros_tpc/input/LHMagneticFieldMapX85m.txt

- Major update/debug:

	a. LAMPS-HighEnergy/geant4/LHMagneticField.cc:
		-> major debug: the field unit will be applied properly when reading text

	b. LAMPS-HighEnergy/geant4/LHDetectorConstruction.cc
		b-1. Added FT prototypes: simple box shape or octants
		b-2. Added ND (neutron detector) based on info from LAMPS January whitepaper

