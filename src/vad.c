#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
    "UNDEF", "S", "V", "INIT"};

const char *state2str(VAD_STATE st)
{
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct
{
  float zcr;
  float p;
  float am;

} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N)
{
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  Features feat;
  feat.am = compute_am(x, N);
  feat.p = compute_power(x, N, 0); //=!1 means power will be computed with a square window
  feat.zcr = compute_zcr(x, N, 16000);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA *vad_open(float rate, float alpha0, float alpha1, float alpha2)
{
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->alpha0 = alpha0;
  vad_data->alpha1 = alpha1 / 1000; /*Convert to s*/ //obsolete (default value = 0)
  vad_data->alpha2 = alpha2;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data)
{
  /* 
   * TODO: decide what to do with the last undecided frames
   */

  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data)
{
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x, float *t)
{

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */
  float time_elapsed = *t;
  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state)
  {
  case ST_INIT:
    vad_data->k0 = f.p + vad_data->alpha0;   //set power threshold
    vad_data->k2 = f.zcr + vad_data->alpha2; //set zcr threshold
    vad_data->state = ST_SILENCE;
    break;

  case ST_SILENCE:
    if ((f.p > vad_data->k0) || (f.zcr > vad_data->k2))
      vad_data->state = ST_MAYBE_VOICE;
    break;

  case ST_VOICE:
    if ((f.p < vad_data->k0) && (f.zcr < vad_data->k2))
      vad_data->state = ST_MAYBE_SILENCE;
    break;

  case ST_UNDEF:
    break;

  case ST_MAYBE_SILENCE:
    if ((f.p < vad_data->k0 )&& (vad_data->alpha1 < time_elapsed))
      {
        *t = 0;   //restart time elapsed
        vad_data->state = ST_SILENCE;
      }
    else
    {
      vad_data->state = ST_VOICE;
    }
    break;
  case ST_MAYBE_VOICE:
    if ((f.p > vad_data->k0 ) && (vad_data->alpha1 < time_elapsed))
    {
      *t = 0;   //restart time elapsed
      vad_data->state = ST_VOICE;
    }
    else
    {
      vad_data->state = ST_SILENCE;
    }
    break;
  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE ||
      vad_data->state == ST_MAYBE_SILENCE ||
      vad_data->state == ST_MAYBE_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out)
{
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
