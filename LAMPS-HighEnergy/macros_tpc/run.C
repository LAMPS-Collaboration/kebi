int run_g4sim(const char *name);

void run(const char *name="g4event"){

	const bool bG4SIM = false;
	const bool bDIGI = true;
	const bool bRECO = true;

	if ( bG4SIM ){
		if ( !run_g4sim(name) ) return;
	}

	if ( bDIGI || bRECO ){

		auto run = KBRun::GetRun();
		run->SetIOFile(Form("./data/%s.mc",name), Form("./data/%s.reco",name));
		run->AddDetector(new LHTpc());

		if ( bDIGI ){
			auto drift = new LHDriftElectronTask();
			drift->SetPadPersistency(true);
			drift->SetDetID(10);
			run->Add(drift);

			auto electronics = new LHElectronicsTask(true);
			run->Add(electronics);
		}


		if ( bDIGI && bRECO ){
			auto psa = new KBPSATask();
			psa -> SetPSA(new KBPSAFastFit());
			run->Add(psa);

			run->Add(new LHHelixTrackFindingTask());
			//run->Add(new LHVertexFindingTask());
		}

		run->Init();
		run->Run();
	}


}

int run_g4sim(const char *name)
{

	TString fname = Form("kbpar_%s.conf",name);
	gSystem->FindFile("./",fname);

	if ( fname.Length()==0 ){
		cout << "Can not find the macrofile" << endl;
		return 0;
	}else{
		fname = Form("kbpar_%s.conf",name);
		gSystem->Exec(Form("./run_mc.g4sim %s",fname.Data()));
	}

	return 1;
}

