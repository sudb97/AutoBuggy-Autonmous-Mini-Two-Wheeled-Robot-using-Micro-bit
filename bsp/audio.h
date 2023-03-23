#ifndef AUDIO_H
#define AUDIO_H
#include <stdint.h>

/* Audio */
void audio_init(uint32_t speaker_pin, uint32_t mic_pin, uint32_t run_mic_pin);
void audio_beep(int freq, int duration_ms);
void audio_sweep(int fstart, int fend, int duration_ms);
void audio_volume(uint32_t volume);
    /* crude volume control based on duty cycle. 50 for normal, 25 for low. */

/* Music */
void music_init(void);
void music_play(char tunes[]);

/* Mic */
uint32_t clap_detect(void);

#endif /* AUDIO_H */
