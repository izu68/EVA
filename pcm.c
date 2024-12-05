#include "pcm.h"

void dac1 (uint8_t ch1_sample)
{

}

void dac2 (uint8_t ch1_sample, uint8_t ch2_sample)
{

}

sound_cache_entry sound_cache[MAX_SOUND_BANKS];

uint8_t init_pcm_system (void)
{
	if (Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf ("(EVA ERROR -> SDL) SDL Mixel could not initialize\n");
		return 1;
	}
	Mix_AllocateChannels (MAX_CHANNELS);
	return 0;
}

void load_sound_bank (uint8_t sound_bank)
{
	char path[256];
	FILE* file = fopen ("flash/sound_table.txt", "r");
	if (!file)
	{
		printf ("(EVA ERROR) Sound table not found in filesystem\n");
		return;
	}

	uint8_t id;
	while (fscanf (file, "%hhx %s", &id, path) == 2)
	{
		if (id == sound_bank)
		{
			fclose (file);

			// Load and cache sound
			Mix_Chunk* bank = Mix_LoadWAV (path);
			if (!bank)
			{
				printf ("(EVA ERROR) Failed to cache sound\n");
				return;		
			}

       			if (sound_cache[sound_bank].bank == NULL) 
			{
				sound_cache[sound_bank].bank = bank;
				printf ("(EVA) Loaded sound bank %X\n", sound_bank);
				return;
                	}

            		// Cache full, abort
			printf ("(EVA ERROR) Sound bank used, ignoring load\n");
            		return;
		}
	}
	fclose(file);
    	printf("(EVA ERROR) Sound bank %X not found\n", sound_bank);
    	return;
}

void free_sound_bank (uint8_t sound_bank)
{
	sound_cache[sound_bank].bank = NULL;
}

void play_sound_bank (uint8_t sound_bank, uint8_t channel)
{
	if (sound_cache[sound_bank].bank)
	{
		if (Mix_PlayChannel(channel, sound_cache[sound_bank].bank, 0) == -1) 
		{
            		printf("Failed to play sound: %s\n", Mix_GetError());
        	}
    	} 
	else 
	{
        	printf("Sound ID %d not loaded.\n", sound_bank);
    	}
}

void halt_channel (int8_t channel)
{
	Mix_HaltChannel (channel);
}
