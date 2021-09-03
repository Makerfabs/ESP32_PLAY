#ifndef MY_AUDIO_H
#define MY_AUDIO_H

#include <Arduino.h>

void audio_info(const char *info);
void audio_id3data(const char *info);
void audio_showstation(const char *info);
void audio_showstreaminfo(const char *info);
void audio_showstreamtitle(const char *info);
void audio_bitrate(const char *info);
void audio_commercial(const char *info);
void audio_icyurl(const char *info);
void audio_lasthost(const char *info);
void audio_eof_speech(const char *info);

#endif