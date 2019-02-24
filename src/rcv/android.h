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
} ReturnCodes;

typedef enum DeltaRangeStates{
  cycleSlip = 4,
  halfCycleResolved = 8,
  halfCycleReported = 16,
  valid             = 1,
  reset             = 2
} DeltaRangeStates;

                        //0 1 2 3 4    5 6 7 8    9 0 11 12 13 14 15 16
const int lliArray[17] = {0,0,0,0,0x01,0,0,0,0x40,0,0,0, 0, 0, 0, 0, 0x02};

typedef struct {
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
  int hasAutomaticGainControlLevelDb;
  int hasCarrierCycles;
  int hasCarrierFrequencyHz;
  int hasCarrierPhase;
  int hasCarrierPhaseUncertainty;
  int hasSnrInDb;

} android_measurementsd_t;

typedef struct {
  int n;
  android_measurementsd_t *measurements;
} android_measurements_t;

typedef struct {
  double biasNanos;
  double biasUncertaintyNanos;
  double driftNanosPerSecond;
  double driftUncertaintyNanosPerSecond;
  long int fullBiasNanos;
  int hardwareClockDiscontinuityCount;
  int leapSecond;
  long int timeNanos;
  double timeUncertaintyNanos;
  int hasBiasNanos;
  int hasBiasUncertaintyNanos;
  int hasDriftNanosPerSecond;
  int hasDriftUncertaintyNanosPerSecond;
  int hasFullBiasNanos;
  int hasLeapSecond;
  int hasTimeUncertaintyNanos;
} android_clockd_t;


long int calcRange(gtime_t t1, gtime_t t2);
double nano2sec(long int t);
gtime_t nano2gtime(long int nanoSec, int week);
double calcPseudoRange(long int t1, long int t2);

