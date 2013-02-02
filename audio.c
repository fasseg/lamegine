/*
 * audio.c
 *
 *  Created on: Feb 2, 2013
 *      Author: ruckus
 */

#include "audio.h"

int init_sound() {
	long loops;
	int rc;
	int size;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;

	/* Open PCM device for playback. */
	rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(handle, params, 2);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

	/* Set period size to 32 frames. */
	frames = 32;
	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params, &val, &dir);
	/* 5 seconds in microseconds divided by
	 * period time */
	char *name = "resources/audio/sample.wav";

	FILE *fp = fopen(name, "r");
	fseek(fp, 0L, SEEK_END);
	long bufsize = ftell(fp);
	buffer =  malloc(sizeof(char) * (bufsize + 1));
	fseek(fp, 0L, SEEK_SET);
	fread(buffer, sizeof(char), bufsize, fp);
	fclose(fp);
	printf("size: %d",bufsize);

	rc = snd_pcm_writei(handle, buffer, bufsize);

	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
	return 0;
}
