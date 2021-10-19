Oct. 19, 2021 (ckim)

- Added tilted angle detectors (SC tilt and AT-TPC, active volume only)
- Minor tweak/cleanup on kbpar_TB22H.conf

Oct. 15, 2021 (ckim)

- Minor updates to check 12C(p, 2p)11B scattering angles from truth level

	a. ~kebi/LAMPS-HighEnergy/geant4/TB22HDetectorConstruction.cc
	b. ~kebi/LAMPS-HighEnergy/macros_tpc/kbpar_TB22H.conf

//-----------------------------------------------

Sep. 23, 2021 (ckim)

- ~kebi/source/geant4/KBPrimaryGeneratorAction.cc: added uncertainty option in beam energy
	a. To invoke it, use parameter " G4InputEnergyError " in exact number (i.e., 0.01 for 1 %)
	b. This function only activated when the above parameter is given

- ~kebi/LAMPS-HighEnergy/macros_tpc/geant4_run.mac : added random seeding by SHLim (/random/setSeeds 1 1)
- ~kebi/LAMPS-HighEnergy/geant4/TB22HDetectorConstruction.cc : modified TOF and ND arrangement

Sep. 17, 2021 (ckim)

- ~kebi/source/geant4/KBPrimaryGeneratorAction.cc: debugged randomizing beam XY (both radial and square shape)

//-----------------------------------------------

Sep. 13, 2021 (kimc)

- New project added: TB22H (Test Beam 2022 HIMAC)
	a.	By default aims 12C (p, 2p) 11B
	b.	Default beam: 12C, 100 MeV/A, 5.0 x 10^5 PPS, r = 5 mm (circular)
	c.	Subsystems of interest: SC, BDC, C2H4 target, BTOF, AT-TPC, and ND

- Newly added (compared to TB20A):
    a.  ~kebi/LAMPS-HighEnergy/geant4/TB22HDetectorConstruction.hh
    b.  ~kebi/LAMPS-HighEnergy/geant4/TB22HDetectorConstruction.cc
    c.  ~kebi/LAMPS-HighEnergy/macros_tpc/kbpar_TB22H.conf
    d.  ~kebi/LAMPS-HighEnergy/macros_tpc/run_TB22Hmc.cc

- Modified:
    a.  ~kebi/LAMPS-HighEnergy/CMakeLists.txt
    b.  ~kebi/source/geant4/KBPrimaryGeneratorAction.cc
		b-1. Added circular beam shape feature
			 b-1-A. To invoke it, require parameters " G4InputCircular (bool) " and " G4InputRadius (double) "
			 b-1-B. If not invoked, square beam shape will be called as default
        b-2. Added user defined beam insertion position in z
			 b-2-A. To invoke it, require parameter " G4InputPosZ (double) "
			 b-2-B. If not invoked or given "G4InputPosZ" is out of world, default (-worldDz) will be called
	c.	~kebi/LAMPS-HighEnergy/macros_tpc/geant4_vis.mac

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

