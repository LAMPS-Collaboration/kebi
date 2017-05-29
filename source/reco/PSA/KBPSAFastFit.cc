#include "KBPSAFastFit.hh"

ClassImp(KBPSAFastFit)

KBPSAFastFit::KBPSAFastFit()
: KBPulseGenerator()
{
  fTbStartCut = 512 - fNDFTbs - 1;
  fThresholdOneTbStep = fThresholdTbStep * fThreshold;
}

void 
KBPSAFastFit::AnalyzeChannel(Double_t *buffer, vector<KBChannelHit> *hitArray)
{
  Double_t adc[512] = {0};
  memcpy(&adc, buffer, sizeof(Double_t)*512);

  // Found peak information
  Int_t tbCurrent = fTbStart;
  Int_t tbStart;

  // Fitted hit information
  Double_t tbHit;
  Double_t amplitude;
  Double_t squareSum;
     Int_t ndf = fNDFTbs;

  // Previous hit information
  Double_t tbHitPre = fTbStart;
  Double_t amplitudePre = 0;

  while (FindPeak(adc, tbCurrent, tbStart)) 
  {
    if (tbStart > fTbStartCut - 1)
      break;

    if (FitPulse(adc, tbStart, tbCurrent, tbHit, amplitude, squareSum, ndf) == false)
      continue;

    if (TestPulse(adc, tbHitPre, amplitudePre, tbHit, amplitude)) 
    {
      hitArray -> push_back(KBChannelHit(0, tbHit, amplitude));

      tbHitPre = tbHit;
      amplitudePre = amplitude;

      tbCurrent = Int_t(tbHit) + 9;
    }
  }
}

Bool_t
KBPSAFastFit::FindPeak(Double_t *adc, 
                          Int_t &tbCurrent, 
                          Int_t &tbStart)
{
  Int_t countAscending      = 0;
  Int_t countAscendingBelow = 0;

  for (; tbCurrent < fTbEnd; tbCurrent++)
  {
    Double_t diff = adc[tbCurrent] - adc[tbCurrent - 1];

    // If adc difference of step is above threshold
    if (diff > fThresholdOneTbStep) 
    {
      if (adc[tbCurrent] > fThreshold) countAscending++;
      else countAscendingBelow++;
    }
    else 
    {
      // If acended step is below 5, 
      // or negative pulse is bigger than the found pulse, continue
      if (countAscending < fNumAscending || ((countAscendingBelow >= countAscending) && (-adc[tbCurrent - 1 - countAscending - countAscendingBelow] > adc[tbCurrent - 1]))) 
      {
        countAscending = 0;
        countAscendingBelow = 0;
        continue;
      }

      tbCurrent -= 1;
      if (adc[tbCurrent] < fThreshold)
        continue;

      // Peak is found!
      tbStart = tbCurrent - countAscending;
      while (adc[tbStart] < adc[tbCurrent] * 0.05)
        tbStart++;

      return true;
    }
  }

  return false;
}

Bool_t
KBPSAFastFit::FitPulse(Double_t *adc, 
                          Int_t tbStart,
                          Int_t tbPeak,
                       Double_t &tbHit, 
                       Double_t &amplitude,
                       Double_t &squareSum,
                          Int_t &ndf)
{
  Double_t adcPeak = adc[tbPeak];

  if (adcPeak > 3500) // if peak value is larger than 3500(mostly saturated)
  {
    ndf = tbPeak - tbStart;
    if (ndf > fNDFTbs) ndf = fNDFTbs;
  }

  Double_t alpha   = fAlpha   / (adcPeak * adcPeak); // Weight of time-bucket step
  Double_t betaCut = fBetaCut * (adcPeak * adcPeak); // Effective cut for beta

  Double_t lsPre; // Least-squares of previous fit
  Double_t lsCur; // Least-squares of current fit

  Double_t beta = 0;    // -(lsCur-lsPre)/(tbCur-tbPre)/ndf.
  Double_t dTb = - 0.1; // Time-bucket step to next fit

  Double_t tbPre = tbStart + 1; // Pulse starting time-bucket of previous fit
  Double_t tbCur = tbPre + dTb; // Pulse starting time-bucket of current fit

  LSFitPulse(adc, tbPre, ndf, lsPre, amplitude);
  LSFitPulse(adc, tbCur, ndf, lsCur, amplitude);
  beta = -(lsCur - lsPre) / (tbCur - tbPre) / ndf;

  Int_t numIteration = 1;
  Bool_t doubleCheckFlag = false; // Checking flag to apply cut twice in a row

  while (dTb != 0 && lsCur != lsPre)
  {
    lsPre = lsCur;
    tbPre = tbCur;

    dTb = alpha * beta;
    if (dTb > 1) dTb = 1;
    if (dTb < -1) dTb = -1;

    tbCur = tbPre + dTb;
    if (tbCur < 0 || tbCur > fTbStartCut)
    {
      return false;
    }

    LSFitPulse(adc, tbCur, ndf, lsCur, amplitude);
    beta = -(lsCur - lsPre) / (tbCur - tbPre) / ndf;

    numIteration++;

    if (abs(beta) < betaCut)
    {
      if (doubleCheckFlag == true)
        break;
      else
        doubleCheckFlag = true;
    }
    else
      doubleCheckFlag = false;

    if (numIteration >= fIterMax)
      break;
  }

  if (beta > 0) {
    tbHit = tbPre;
    squareSum = lsPre;
  }
  else {
    tbHit = tbCur;
    squareSum = lsCur;
  }

  return true;
}

void 
KBPSAFastFit::LSFitPulse(Double_t *buffer, Double_t tbStart, Int_t ndf, Double_t &chi2, Double_t &amplitude)
{
  Double_t refy = 0;
  Double_t ref2 = 0;

  for (Int_t iTbPulse = 0; iTbPulse < ndf; iTbPulse++) {
    Int_t tb = tbStart + iTbPulse;
    Double_t y = buffer[tb];

    Double_t ref = Pulse(tb + 0.5, 1, tbStart);
    refy += ref * y;
    ref2 += ref * ref;
  }

  if (ref2 == 0)
  {
    chi2 = 1.e10;
    return;
  }

  amplitude = refy / ref2;

  chi2 = 0;
  for (Int_t iTbPulse = 0; iTbPulse < ndf; iTbPulse++) {
    Int_t tb = tbStart + iTbPulse;
    Double_t val = buffer[tb];
    Double_t ref = Pulse(tb + 0.5, amplitude, tbStart);
    chi2 += (val - ref) * (val - ref);
  }
}

Bool_t
KBPSAFastFit::TestPulse(Double_t *adc, 
                        Double_t tbHitPre,
                        Double_t amplitudePre, 
                        Double_t tbHit, 
                        Double_t amplitude)
{
  Int_t numTbsCorrection = fNumTbsCorrection;

  if (numTbsCorrection + Int_t(tbHit) >= 512)
    numTbsCorrection = 512 - Int_t(tbHit);

  if (amplitude < fThreshold) 
  {
    return false;
  }

  if (amplitude < Pulse(tbHit + 9, amplitudePre, tbHitPre) / 2.5) 
  {
    for (Int_t iTbPulse = -1; iTbPulse < numTbsCorrection; iTbPulse++) {
      Int_t tb = Int_t(tbHit) + iTbPulse;
      //adc[tb] -= Pulse(tb, amplitude, tbHit);
      adc[tb] = 0;
    }

    return false;
  }

  for (Int_t iTbPulse = -1; iTbPulse < numTbsCorrection; iTbPulse++) {
    Int_t tb = Int_t(tbHit) + iTbPulse;
    //adc[tb] -= Pulse(tb, amplitude, tbHit);
    adc[tb] = 0;
  }


  return true;
}
