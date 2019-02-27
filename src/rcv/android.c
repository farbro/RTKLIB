#include "rtklib.h"
#include "android.h"

/* TODO: Same for file? */
extern int input_androidf (raw_t *raw, FILE *fp){ return 0; }  

extern int input_android (raw_t *raw,  unsigned char data){

  /* === TODO: NOTES === /

   1. Some data is represented as unsigned chars in obsd_t but as doubles in androidStruct.
      How shall a double be represented in an unsigned char? The decimals are lost and a
      number greater than 255 causes overflow.

   2. TOW or gps week needs to be fetched and updated. it is currently set to a week 
      corresponding the week which starts 2019-02-25

   3. LLI (loss of lock indicator). getAccumulatedDeltaRangeState indicates the "loss of lock"
      state however the states are not the same in android as in rtklib. the current "conversion"
      between the states are probably WRONG.

   4. CODE_??? <--- what, que, vafan? rtklib_h, row number 283-339

   5. add return 'returncode'

   --------------- New additions-02/25 ------------------------------------ 

   6. Fix sizeof() error. 

  / ==== NOTES END ==== */

  android_clockd_t *cl;
  android_measurements_t *ms;
  int cl_size, ms_size, msd_size;

   /* Store new byte */
  raw->buff[raw->nbyte++] = data;

  cl_size = sizeof(android_clockd_t);        
  ms_size = sizeof(android_measurements_t);
  msd_size = sizeof(android_measurementsd_t);

   /* Check if finished receiving android_clockd_t and android_measurements_t */
  if (raw->nbyte == cl_size + ms_size) {
    ms = (android_measurements_t*) &raw->buff[cl_size];

     /* Calculate and store expected total length of message */
    raw->len = cl_size + ms_size + ms->n * msd_size;
  }

   /* Check if complete message is received */
  if (raw->len > 0 && raw->nbyte == raw->len) {

     /* Point the structs */
    cl = (android_clockd_t*) &raw->buff;
    ms = (android_measurements_t*) &raw->buff[cl_size];

    /* TODO Convert raw data */
    return convertObservationData(&raw->obs, cl, ms);
  }

  return (int) noMsg; /* Keep buffering */
}

/* Fill obs_t with data from android_clockd_t and andorid_measurements_t */
int convertObservationData(obs_t *obs, android_clockd_t *cl, android_measurements_t *ms) {
  int i;
  obsd_t *obsd;
  android_measurementsd_t *android_obs;
  gtime_t cl_time;
  int cl_week;
  double cl_tow;

  /* Set number of observations */
  obs->n = ms->n;

  /* Convert GPS time in nanoseconds to gtime_t */
  cl_time = nano2gtime(cl->timeNanos - cl->fullBiasNanos);

  /* Get current week and tow */
  cl_tow = time2gpst(cl_time, &cl_week);

  /* Fill obs_t->data */
  for (i = 0; i < ms->n; i++) {
    obsd = &obs->data[i];
    android_obs = &ms->measurements[i];

    obsd->time     = gpst2time(cl_week, android_obs->receivedSvTimeNanos / (double)SEC); /* receiver sampling time (GPST) */ 
    /* obsd->sat      = android_obs->svid;        */ /* satellite/receiver number */
    /* obsd->rcv      = recId;                    */ /* satellite/receiver number */
    /* obsd->SNR[0]   = snr;                      */ /* signal strength (0.25 dBHz) */
    /* obsd->LLI[0]   = lli;                      */ /* loss of lock indicator */
    /* obsd->code[0]  = null;                     */ /* code indicator (CODE_???) */
    /* obsd->qualL[0] = null;                     */ /* quality of carrier phase measurement */
    /* obsd->qualP[0] = null;                     */ /* quality of pseudorange measurement */
    /* obsd->L[0]     = carrierPhase;             */ /* observation data carrier-phase (cycle) */
    /* obsd->P[0]     = calcPseudoRange(t1,t2);   */ /* observation data pseudorange (m) */
    /* obsd->D[0]     = null;                     */ /* observation data doppler frequency (Hz) */
  }

  return (int) observationData;
}


/* ========= ============ ========= */ 
/* ========= Calculations ========= */ 
/* ========= ============ ========= */ 
double nano2sec(long int t){
  return ((double)(t)/(SEC));
}

gtime_t nano2gtime(long int nanoSec){
  gtime_t gtime;
  gtime.time = nanoSec / SEC;
  gtime.sec = (gtime.time - nanoSec) / (double)SEC;

  return gtime;
}

double calcPseudoRange(long int t1, long int t2){
  double pseudoRange =  C*(nano2sec(t2) - nano2sec(t1));
  return pseudoRange;
}
