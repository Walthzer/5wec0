#include <audio.h>
void audio_init(void){};
void audio_select_input(int input){};
void write_audio_reg(unsigned char u8RegAddr, unsigned char u8Data, int iic_fd){};
void config_audio_pll(void){};
void config_audio_codec(void){};
void select_line_in(void){};
void select_mic(void){};
void deselect(void){};
void audio_bypass(unsigned int audio_mmap_size, unsigned int nsamples,
                  unsigned int volume, int uio_index){};
void audio_record(unsigned int audio_mmap_size, unsigned int *BufAddr,
                  unsigned int nsamples, int uio_index){};
void audio_play(unsigned int audio_mmap_size, unsigned int *BufAddr,
                unsigned int nsamples, unsigned int volume, int uio_index){};
void audio_repeat_play(unsigned int audio_mmap_size, unsigned int *BufAddr,
                       unsigned int nsamples, unsigned int volume,
                       unsigned int repetitions){};
void audio_generate_tone(unsigned int frequency, uint32_t time_ms,
                         unsigned int volume){};

