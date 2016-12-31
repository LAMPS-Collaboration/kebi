#include "KBPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

KBPrimaryGeneratorAction::KBPrimaryGeneratorAction(const char *fileName)
{
  fParticleGun = new G4ParticleGun();
  fEventGenerator = new KBMCEventGenerator(fileName);
}

KBPrimaryGeneratorAction::KBPrimaryGeneratorAction(G4String fileName)
:KBPrimaryGeneratorAction(fileName.data())
{
}

KBPrimaryGeneratorAction::~KBPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void KBPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4int pdg;
  G4double vx, vy, vz, px, py, pz;

  fEventGenerator -> ReadNextEvent(vx, vy, vz);
  fParticleGun -> SetParticlePosition(G4ThreeVector(vx,vy,vz));

  while (fEventGenerator -> ReadNextTrack(pdg, px, py, pz))
  {
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable() -> FindParticle(pdg);
    fParticleGun -> SetParticleDefinition(particle);

    G4ThreeVector momentum(px,py,pz);
    fParticleGun -> SetParticleMomentum(momentum.mag()*MeV);
    fParticleGun -> SetParticleMomentumDirection(momentum.unit());
    fParticleGun -> GeneratePrimaryVertex(anEvent);
  }
}
