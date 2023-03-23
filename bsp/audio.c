#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include "audio.h"
#include "gpio.h"
#include "pwm.h"
#include "adc.h"
#include "lib.h"

void delay_ms(uint32_t ms);

/* Initialize speaker and mic */
void audio_init(uint32_t speaker_pin, uint32_t mic_pin, uint32_t run_mic_pin)
{
    /* Speaker is connected to a PWM pin. We can use it to play tones by
     * generating a square wave at a specific frequency.
     */
    pwm_init(speaker_pin);

    if (run_mic_pin != 0)    // should we turn on the mic?
    {
        /* Turn on the microphone circuit */
        gpio_out(run_mic_pin, DRIVE_HIGH);  // supplies voltage to circuit, hence
                                            // configured for high drive current
        gpio_set(run_mic_pin);              // turn on the mic

        /* Initialize ADC for receiving microphone samples. */
        adc_init(mic_pin);
    }

    /* Set volume */
    audio_volume(50);      // 50 normal, 25 low
}

/* Duty cycle can be used to control volume. It is not linear but we can
   turn the volume low during debugging. */
static int duty_cycle;

void audio_volume(uint32_t volume)
{
    duty_cycle = volume;
}

/* Play a beep at the specified frequency for the specified duration. */
void audio_beep(int freq, int duration_ms)
{
    /* Generate pwm with the specified frequency */
    pwm_out(freq, duty_cycle, duration_ms);
}

/* Play all the frequencies from start to end for a given duration. */
void audio_sweep(int fstart, int fend, int duration_ms)
{
    int step_duration = 20;     // in milliseconds
    int steps = duration_ms / step_duration;
    int fdelta = (fend - fstart) / steps;
    int f;

    /* Play the sweep as small steps of increasing / decreasing frequencies. */
    f = fstart;
    while (steps > 0)
    {
        audio_beep(f, step_duration);

        f += fdelta;
        steps--;
    }
}

/* An octave is made up of 12 notes C C# D D# E F F# G G# A A# B.
 * The array below shows position of notes A to G in the octave.
 * This is required because the order is CDEFGAB, and not ABCDEFG.
 */
static char note_pos[] = {9, 11, 0, 2, 4, 5, 7};
                       // A  B   C  D  E  F  G

#define C0_FREQ     16.35
#define NUM_OCTAVES 7
#define NUM_NOTES   12

/* Calculate and store frequencies of all notes in all 7 octaves.
 * Starts with C0 and ends with C7, hence total notes are octave*note + 1.
 */
static uint16_t scale_frequencies[NUM_OCTAVES * NUM_NOTES + 1];
static void scale_generate(void);

void music_init(void)
{
    /* Compute the frequencies for all the notes. Do this one-time as it
     * involves floating point computations.
     */
    scale_generate();
}

/*
 * An octave is made up of 12 notes: C-C#-D-D#-E-F-G-G#-A-A#-B-[C].
 * The last C is an octave higher than the first C and has twice the frequency.
 * The remaining notes are exponentially equispaced within x and 2x frequencies, * hence each 2^(1/12) times the earlier one.
 */
static void scale_generate(void)
{
    int i;

    scale_frequencies[0] = C0_FREQ;
    for (i = 1; i <= NUM_OCTAVES * NUM_NOTES; i++)
        scale_frequencies[i] = C0_FREQ * pow(2, i/12.0);
}

/*
 * Play a string made up of notes. Each note has same duration.
 * A dash is used to continue the previous note and a dot for a silence.
 */
void music_play(char tunes[])
{
    char *p;
    int note, sharp, octave;
    int index, freq = 0;

    p = tunes;
    while (*p)  // till we hit the end of the string
    {
        note = tolower(*p) - 'a';

        note = note_pos[note];

        /* repeat previous */
        if (*p == '-')
        {
            p++;
            audio_beep(freq, 100);  // repeat the previous frequency
            continue;
        }

        /* silence */
        if (*p == '.')
        {
            p++;
            delay_ms (100);         // silence
            continue;
        }

        p++;

        /* sharp */
        if (*p == '#')
        {
            sharp = 1;
            p++;
        }
        else
            sharp = 0;

        octave = *p++ - '0';

        /* compute index and look up the frequency table */
        index = octave*12 + note + sharp;
        freq = scale_frequencies[index];

        audio_beep(freq, 100);
    }
}

/*
 * Clap is detected using a crude algorithm of collecting samples
 * for 20ms, computing its energy and comparing against a threshold.
 * The algorighm just ensures there is silence before and after the
 * clap to distinguish it from a continuous loud noise.
 *
 * The algorithm is not accurate. For example, one could miss a clap
 * if it falls half into the previous frame and half into the current frame.
 * Also it's just an energy-based algorithm. Hence, all pulse-like noises
 * are detected as clap.
 */
#define CLAP_WINDOW 20  // 20 ms window to collect samples
#define CLAP_FRAMELEN   ((CLAP_WINDOW * MIC_SAMPLE_RATE) / 1000)

#define CLAP_THRESHOLD  0x100000    // by trial and error

static uint16_t samples[CLAP_FRAMELEN];     // to collect the samples

uint32_t clap_detect(void)
{
    uint32_t dc;    // ADC provides unsigned samples with DC somewhere
                    //  around the middle. We need to compute the DC value
                    //  and subtract before computing the energy.
    uint32_t energy;

    // Previous two decisions and the current decision to decide if
    // there was a clap or not.
    static uint32_t prev[2];    // previous two energy decisions
    uint32_t current;           // current energy decisions
    uint32_t clap;              // clap decision
    int i;

    /* Read a buffer of samples from the mic */
    adc_read(samples, CLAP_FRAMELEN);

    /* printf("%x\n", adc_in()); */

    /* Compute DC */
    dc = 0;
    for (i = 0; i < CLAP_FRAMELEN; i++)
        dc += samples[i];
    dc /= CLAP_FRAMELEN;

    /* printf("dc = %x\n", (unsigned int) dc); */

    /* Compute energy */
    energy = 0;
    for (i = 0; i < CLAP_FRAMELEN; i++)
    {
        uint32_t s = samples[i] - dc;

        energy += s * s;
    }
    /* printf("energy = %x\n", (unsigned int) energy); */

    /* Current threshold detection */
    if (energy > CLAP_THRESHOLD)
        current = 1;
    else
        current = 0;

    /* It's a clap if prev1-prev0-current decisions are 0-1-0 */
    clap = ((prev[1] == 0) && (prev[0] == 1) && (current == 0));

    /* Save for the next time. */
    prev[1] = prev[0];
    prev[0] = current;

    return clap;
}
