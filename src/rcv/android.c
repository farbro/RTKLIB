#include "rtklib.h"
#include "android.h"

extern int input_androidf (raw_t *raw, FILE *fp){
  unsigned char data;

  data = fgetc(fp);

  if (data == EOF) return (int) endOfFile;

  return input_android(raw, data);
}  

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

  trace(5, "input_android, data=%02x\n", data);

   /* Store new byte */
  raw->buff[raw->nbyte++] = data;

  cl_size = sizeof(android_clockd_t);        
  ms_size = sizeof(android_measurements_t);
  msd_size = sizeof(android_measurementsd_t);
  trace(5, "cl_size = %d, ms_size = %d, msd_size = %d\n", cl_size, ms_size, msd_size);

   /* Check if finished receiving android_clockd_t and android_measurements_t */
  if (raw->nbyte == cl_size + ms_size) {
    ms = (android_measurements_t*) &raw->buff[cl_size];

     /* Calculate and store expected total length of message */
    raw->len = cl_size + ms_size + ms->n * msd_size;
    trace(3, "received android_clockd_t + android_measurements_t\n");
  }

   /* Check if complete message is received */
  if (raw->len > 0 && raw->nbyte == raw->len) {

     /* Point the structs */
    cl = (android_clockd_t*) &raw->buff;
    ms = (android_measurements_t*) &raw->buff[cl_size];

    trace(3, "received complete struct\n");

    return convertObservationData(&raw->obs, cl, ms);
  }

  return (int) noMsg; /* Keep buffering */
}

/* Fill obs_t with data from android_clockd_t and andorid_measurements_t */
int convertObservationData(obs_t *obs, android_clockd_t *cl, android_measurements_t *ms) {
  int i;
  obsd_t *obsd;
  android_measurementsd_t *android_obs;
  gtime_t rcv_time, sat_time;
  long msg_time_nanos;
  int rcv_week;

  trace(3, "converting observation data\n");

  /* Set number of observations */
  obs->n = ms->n;
  trace(4, "obsd_t.n = %d\n", obs->n);

  /* Calculate GPS time for message */
  msg_time_nanos = cl->timeNanos - cl->fullBiasNanos;
  trace(4, "msg_time_nanos = %d\n", msg_time_nanos);

  /* Fill obs_t->data */
  for (i = 0; i < ms->n; i++) {
    trace(4, "storing measurement %d\n", i);
    obsd = &obs->data[i];
    android_obs = &ms->measurements[i];

    /* === receiver sampling time (GPST) === */ 
    rcv_time = nano2gtime(msg_time_nanos + android_obs->timeOffsetNanos);
    obsd->time = rcv_time;
    trace(4, "obsd_t.time.time = %d, obsd_t.time.sec = %f\n", obsd->time.time, obsd->time.sec);

    /* === satellite/receiver number === */
    obsd->sat = android_obs->svid;

    /* === satellite/receiver number === */
    /* obsd->rcv      = recId;                    */

    /* === signal strength (0.25 dBHz) === */
    obsd->SNR[0] = android_obs->snrInDb;

    /* === loss of lock indicator === */
    /* obsd->LLI[0]   = lli;                      */

    /* === code indicator (CODE_???) === */
    /* obsd->code[0]  = null;                     */

    /* === quality of carrier phase measurement === */
    /* obsd->qualL[0] = null;                     */

    /* === quality of pseudorange measurement === */
    /* obsd->qualP[0] = null;                     */

    /* === observation data carrier-phase (cycle) === */
    /* obsd->L[0]     = carrierPhase;             */

    /*  === observation data pseudorange (m) === */

    /* Calculate GPST for satellite */
    /* We only receive satellite TOW from Android, so we use week from rcv_time instead */
    time2gpst(rcv_time, &rcv_week); /* Calculate week number from rcv_time */
    sat_time = gpst2time(rcv_week, nano2sec(android_obs->receivedSvTimeNanos));

    obsd->P[0] = calcPseudoRange(rcv_time, sat_time);
    trace(4,"obsd_t.P[0] = %d\n", obsd->P[0]);

    /* === observation data doppler frequency (Hz) === */
    /* obsd->D[0]     = null;                     */
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

double calcPseudoRange(gtime_t rx, gtime_t tx){
  double diff = (rx.time - tx.time) + (rx.sec - tx.sec);

  return diff * CLIGHT;
}
