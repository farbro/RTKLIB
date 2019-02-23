// Recieved time is in Nano seconds (from sattelite)
//
#include "rtklib.h"
#define SEC 1000000000
#define C   299792458
#define UN (unsigned char)

typedef enum ReturnCodes {
  endOfFile         = -2,
  errorMsg          = -1,
  noMsg             = 0,
  observationData   = 1,
  inEphemeris       = 2,
  inSbasMsg         = 3,
  antennaPosParam   = 5,
  dgpsCorrection    = 7,
  inIonUtcParam     = 9,
  ssrMessage        = 10,
  lexMsg            = 31
} ReturneCodes;

typdef enum DeltaRangeStates{
  cycleSlip = 4,
  halfCycleResolved = 8,
  halfCycleReported = 16,
  valid             = 1,
  reset             = 2
} DeltaRangeStates;
                        //0 1 2 3 4    5 6 7 8    9 0 11 12 13 14 15 16
const int lliArray[17] = (0,0,0,0,0x01,0,0,0,0x40,0,0,0, 0, 0, 0, 0, 0x02);

typedef{
  double biasNanos;
  double biasUncertaintyNanos;
  double driftNanosPerSecond;
  double driftUncertaintyNanosPerSecond;
  long int fullBiasNanos;
  int hardwareClockDiscontinuityCount;
  int leapSecond;
  long int timeNanos;
  double timeUncertaintyNanos;
  int biasNanos;
  int biasUncertaintyNanos;
  int driftNanosPerSecond;
  int driftUncertaintyNanosPerSecond;
  int fullBiasNanos;
  int LeapSecond;
  int TimeUncertaintyNanos;
  int NUM_MEASUREMENTS
  double accumulatedDeltaRangeMeters;
  int accumulatedDeltaRangeState;
  double accumulatedDeltaRangeUncertaintyMeters;
  double automaticGainControlLevelDb;
  long int carrierCycles;
  float carrierFrequencyHz;
  double carrierPhase;
  double carrierPhaseUncertainty;
  double cn0DbHz;
  int constellationType;
  int multipathIndicator;
  double pseudorangeRateUncertaintyMetersPerSecond;
  long int receivedSvTimeNanos;
  long int receivedSvTimeUncertaintyNanos;
  double snrInDb;
  int state;
  int svid;
  double timeOffsetNanos;
  int hasautomaticGainControlLevelDb;
  int hascarrierCycles;
  int hasCarrierFrequencyHz;
  int hascarrierPhase;
  int hascarrierPhaseUncertainty;
  int snrInDb;
}androidStruct2;


long int calcRange(gtime_t t1, gtime_t t2);
double nano2sec(long int t);
gtime_t nano2gtime(long int nanoSec, int week);
double calcPseudoRange(long int t1, long int t2);


/* TODO: Same for file? */
extern int input_androidf (raw_t *raw, FILE *fp){}  


extern int input_android (raw_t *raw,  unsigned char data){

  /* === TODO: NOTES === /

   0. Should be an array of data. Add loop for length of raw->buff.

   1. Some data is represented as unsigned chars in obsd_t but as doubles in androidStruct.
      How shall a double be represented in an unsigned char? The decimals are lost and a
      number greater than 255 causes overflow.

   2. TOW or gps week needs to be fetched and updated. it is currently set to a week 
      corresponding the week which starts 2019-02-25

   3. LLI (loss of lock indicator). getAccumulatedDeltaRangeState indicates the "loss of lock"
      state however the states are not the same in android as in rtklib. the current "conversion"
      between the states are probably WRONG.

   4. CODE_??? <--- what, que, vafan? rtklib_h, row number 283-339

   5. add: android.data ? android.data : 0; 
      Or is it fine if some values is null?

   6. Fill in all "null" values

   7. add return 'returncode'

  / ==== NOTES END ==== */

  androidStruct2 ourData;
  gtime_t recTime;
  ourData               = (androidStruct)(raw->buff[0]);
  int tow               = 2042;                        
  long int t2           = ourData.timeNanos;
  long int t1           = ourData.receivedSvTimeNanos;
  unsigned char satId   = UN(ourData.svid);
  unsigned char recId   = UN(ourData.svid); /* TODO: WRONG VAR!  */
  unsigned char snr     = UN(ourData.snrInDb);
  unsigned char lli     = UN(lliArray[ourData.accumulatedDeltaRangeState]);
  double carrierPhase    = (ourData.carrierPhase);

  /* TODO: Insert observation data */
  raw->obs.data[0].time     = t1;                       /* receiver sampling time (GPST) */ 
  raw->obs.data[0].sat      = satId;                    /* satellite/receiver number */
  raw->obs.data[0].rcv      = recId;                    /* satellite/receiver number */
  raw->obs.data[0].SNR[0]   = snr;                      /* signal strength (0.25 dBHz) */
  raw->obs.data[0].LLI[0]   = lli;                      /* loss of lock indicator */
  raw->obs.data[0].code[0]  = null;                     /* code indicator (CODE_???) */
  raw->obs.data[0].qualL[0] = null;                     /* quality of carrier phase measurement */
  raw->obs.data[0].qualP[0] = null;                     /* quality of pseudorange measurement */
  raw->obs.data[0].L[0]     = carrierPhase;                     /* observation data carrier-phase (cycle) */
  raw->obs.data[0].P[0]     = calcPseudoRange(t1,t2);   /* observation data pseudorange (m) */
  raw->obs.data[0].D[0]     = null;                     /* observation data doppler frequency (Hz) */

}


/* ========= ============ ========= */ 
/* ========= Calculations ========= */ 
/* ========= ============ ========= */ 
long int calcRange(gtime_t t1, gtime_t t2){
  long int pseudurange;

  return pseudurange;
}

double nano2sec(long int t){
  return ((double)(t)/(SEC));
}

gtime_t nano2gtime(long int nanoSec, int week){
  gtime_t temp;
  double sec;
  sec = (double)(((double)(nanoSec))/SEC);
  temp = gpst2time(week, sec);
  return temp;
}

double calcPseudoRange(long int t1, long int t2){
  double pseudoRange =  C*(nano2sec(t2) - nano2sec(t1));
}



