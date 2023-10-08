#include <libpynq.h>
#define SAMPLE_SIZE 64*1024

//Sample Left Channel
void audio_record_mono(unsigned int audio_mmap_size, unsigned int *BufAddr, unsigned int nsamples, int uio_index) {
  if (uio_index > 2) {
    pynq_error("audio_record: uio_index outside of range. is %d, should be "
               "below 3. \n",
               uio_index);
  }
  int iic_index = 1;
  unsigned int i, status;
  void *uio_ptr;
  int Data;
  int iic_fd;

  uio_ptr = setUIO(uio_index, audio_mmap_size);
  iic_fd = setI2C(iic_index, IIC_SLAVE_ADDR);
  if (iic_fd < 0) {
    pynq_error("audio_record: unable to set I2C %d, ensure sudo chmod 666 "
               "/dev/i2c-1 has been executed\n",
               iic_index);
  }

  for (i = 0; i < nsamples; i++) {
    do {
      status = *((volatile unsigned *)(((uint8_t *)uio_ptr) + I2S_STATUS_REG));
    } while (status == 0);
    *((volatile unsigned *)(((uint8_t *)uio_ptr) + I2S_STATUS_REG)) =
        0x00000001;

    // Read the sample from the input
    Data = *((volatile int *)(((uint8_t *)uio_ptr) + I2S_DATA_RX_L_REG));

    // Write the sample into memory
    *(BufAddr + i) = Data;
  }

  if (unsetUIO(uio_ptr, audio_mmap_size) < 0) {
    pynq_error("audio_record: unable to free UIO %d, ensure sudo chmod 666 "
               "/dev/i2c-1 has been executed\n",
               uio_index);
  }
  if (unsetI2C(iic_fd) < 0) {
    pynq_error("audio_record: unable to unset I2C %d, ensure sudo chmod 666 "
               "/dev/i2c-1 has been executed\n",
               iic_index);
  }
}

//Play on left channel
void audio_play_mono(unsigned int audio_mmap_size, unsigned int *BufAddr,
                unsigned int nsamples, unsigned int volume, int uio_index) {
  if (uio_index > 2) {
    pynq_error(
        "audio_play: uio_index outside of range. is %d, should be below 3. \n",
        uio_index);
  }
  if (volume > 100) {
    pynq_error("audio_play: volume outside allowed range. Is %d, should be "
               "below 100 \n",
               volume);
  }
  int iic_index = 1;
  unsigned int i, status;
  void *uio_ptr;
  int DataL;
  int iic_fd;

  uio_ptr = setUIO(uio_index, audio_mmap_size);
  iic_fd = setI2C(iic_index, IIC_SLAVE_ADDR);
  if (iic_fd < 0) {
    pynq_error("audio_play: unable to set I2C %d, ensure sudo chmod 666 "
               "/dev/i2c-1 has been executed\n",
               iic_index);
  }

  // Unmute left DAC enable Mixer3
  write_audio_reg(R22_PLAYBACK_MIXER_LEFT_CONTROL_0, 0x21, iic_fd);

  unsigned char vol_register = (unsigned char)volume << 2 | 0x3;
  // Enable Left Headphone out
  write_audio_reg(R29_PLAYBACK_HEADPHONE_LEFT_VOLUME_CONTROL, vol_register,
                  iic_fd);

  for (i = 0; i < nsamples; i++) {
    do {
      status = *((volatile unsigned *)(((uint8_t *)uio_ptr) + I2S_STATUS_REG));
    } while (status == 0);
    *((volatile unsigned *)(((uint8_t *)uio_ptr) + I2S_STATUS_REG)) =
        0x00000001;

    // Read the sample from memory
    DataL = *(BufAddr + i);

    // Write the sample to output
    *((volatile int *)(((uint8_t *)uio_ptr) + I2S_DATA_TX_L_REG)) = DataL;
  }

  // Mute left DAC
  write_audio_reg(R22_PLAYBACK_MIXER_LEFT_CONTROL_0, 0x01, iic_fd);
  // Mute left input to mixer3 (R23)
  write_audio_reg(R23_PLAYBACK_MIXER_LEFT_CONTROL_1, 0x00, iic_fd);

  if (unsetUIO(uio_ptr, audio_mmap_size) < 0) {
    pynq_error("audio_play: unable to free UIO %d, ensure sudo chmod 666 "
               "/dev/i2c-1 has been executed\n",
               uio_index);
  }
  if (unsetI2C(iic_fd) < 0) {
    pynq_error("audio_play: unable to unset I2C %d, ensure sudo chmod 666 "
               "/dev/i2c-1 has been executed\n",
               iic_index);
  }
}

void doAudio()
{
  printf("Doing Init: ");
  audio_init();
  printf("Done\n");
  audio_select_input(LINE_IN);

  printf("Doing Alloc: ");
  unsigned int* ptr_audioBuffer = (unsigned int*) malloc(SAMPLE_SIZE * sizeof(unsigned int));
  printf("Done\n");
  fflush(NULL);

  if (ptr_audioBuffer == NULL) {
    pynq_error("Memory not allocated.\n");
  }

  while (true)
  {
    printf("Doing record: ");
    fflush(NULL);
    audio_record_mono(SAMPLE_SIZE, ptr_audioBuffer, SAMPLE_SIZE, 0);
    printf("Done\n");
    printf("Doing playback loop: ");
    audio_play_mono(SAMPLE_SIZE, ptr_audioBuffer, SAMPLE_SIZE, 90, 0);
    printf("Done\n");

    printf("Confirm recording with \'y\': ");
    fflush(NULL);
    char cmd;
    scanf(" %c", &cmd);
    if (cmd == 'y')
    {
      printf("Recording stored.\n");
      break;
    }
  }
  printf("Press enter for playback: ");
  fflush(NULL);
  char cmd;
  scanf(" %c", &cmd);
  printf("Doing playback loop: ");
  fflush(NULL);
  for (int i = 0; i < 10; i++)
  {
    audio_play_mono(SAMPLE_SIZE, ptr_audioBuffer, SAMPLE_SIZE, 90, 0);
  }
  printf("Done\n");
  fflush(NULL);

  for (int i = 0; i < SAMPLE_SIZE; i++)
  {
    //printf("%d\n", ptr_audioBuffer[i]);
  }

  printf("Freeing alloc: ");
  free(ptr_audioBuffer);
  printf("Done\n");

  //printf("END_DATA");
  deselect();
}

void doBypass(void)
{
  audio_init();
  audio_select_input(LINE_IN);
  while(1) {
    //audio_generate_tone(400, 10000, 50);
    audio_bypass(64*1024, 32*1024, 90, 0);
  }
  deselect();
}

int main(void) {
  pynq_init();

  doAudio();
  //doBypass();

  pynq_destroy();
  return EXIT_SUCCESS;
}
