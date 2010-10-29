#ifndef __INCLUDE_VAHT_WAV_H__
#define __INCLUDE_VAHT_WAV_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_wav_group Sounds
 * Functions for Sound Resources.
 * These functions can be used to inspect tWAV (sound) resources in the
 * archive. You can also use these to get decompressed sound data.
 * @{
 */

struct vaht_wav_s;

/**
 * \brief the sound data type
 *
 * This data type refers to an individual sound. Use it as a pointer only,
 * because it is always dynamically allocated.
 * \sa vaht_wav_open
 */
typedef struct vaht_wav_s vaht_wav;

/**
 * \brief types of encodings for sounds
 *
 * This enumeration is used whenever the type of encoding used for a sound
 * is needed.
 * \sa vaht_wav_encoding
 */
enum vaht_wav_encoding_t
{
	/** unknown encoding type (unsupported) */
	tWAV_UNKNOWN,
	/** raw PCM sound (not yet supported) */
	tWAV_PCM,
	/** Intel DVI ADPCM encoding */
	tWAV_ADPCM,
	/** MPEG-2 Layer II encoding (not yet supported) */
	tWAV_MP2
};

/**
 * \brief create a sound object
 *
 * This function will create a sound object for the given resource. If the
 * resource is not a sound, or in a format or encoding that libvaht does not
 * understand, it will return NULL. You must close this after you are done with
 * it. This object will keep the resource open while it exists.
 * \param resource the resource you want a sound object of
 * \return a sound object, or NULL
 * \sa vaht_wav_close
 */
vaht_wav* vaht_wav_open(vaht_resource* resource);

/**
 * \brief close a sound object
 *
 * This function will free the memory associated with a sound object. Use
 * it when you are done with a sound.
 * \param wav the sound to close
 * \sa vaht_wav_open
 */
void vaht_wav_close(vaht_wav* wav);

/**
 * \brief the samples per second
 *
 * This function will tell you how many samples per second this sound uses.
 * \param wav the sound to inspect
 * \return the number of samples per second
 * \sa vaht_wav_samplecount vaht_wav_samplesize vaht_wav_channels
 */
uint16_t vaht_wav_samplerate(vaht_wav* wav);

/**
 * \brief the number of samples in the sound
 *
 * This function will tell you how many samples this sound contains, total.
 * \param wav the sound to inspect
 * \return the number of samples
 * \sa vaht_wav_samplerate vaht_wav_samplesize vaht_wav_channels
 */
uint32_t vaht_wav_samplecount(vaht_wav* wav);

/**
 * \brief the number of bits per sample
 *
 * This function will tell you the size of each sample, in bits. It is very
 * likely to be a multiple of 8 (that is, made up of whole bytes). However,
 * this is not required and may very well be different. It is usually 16.
 * \param wav the sound to inspect
 * \return the number of bits per sample
 * \sa vaht_wav_samplerate vaht_wav_samplecount vaht_wav_channels
 */
uint8_t vaht_wav_samplesize(vaht_wav* wav);

/**
 * \brief the number of channels in the sound
 *
 * This function will tell you how many channels this sound has. It is usually
 * 1 (mono) or 2 (stereo). More than that, and libvaht will fail.
 * \param wav the sound to inspect
 * \return the number of channels
 * \sa vaht_wav_samplerate vaht_wav_samplecount vaht_wav_samplesize
 */
uint8_t vaht_wav_channels(vaht_wav* wav);

/**
 * \brief the encoding used in the archive
 *
 * This function will tell you how the sound was encoded in the archive. It
 * is here for information only: this does not affect how the data you get
 * from libvaht is interpreted.
 * \param wav the sound to inspect
 * \return the encoding type
 * \sa vaht_wav_encoding_t
 */
enum vaht_wav_encoding_t vaht_wav_encoding(vaht_wav* wav);

/**
 * \brief read data from the sound
 *
 * This function will read sound data into a buffer, in the format specified
 * by the various sample format calls. In stereo sound, the left buffer is
 * first. Call this multiple times to read the whole sound. It returns the
 * number of bytes read.
 *
 * Size is the space available in the buffer, in bytes. It must be a multiple
 * of 4, or this function will read nothing and return 0. This is required
 * because of a peculiarity in the way ADPCM data is decoded.
 *
 * To start reading from the beginning again, use \ref vaht_wav_reset .
 *
 * \param wav the sound to read data from
 * \param size the amount of data to read (in bytes, multiple of 4)
 * \param buffer the buffer to read into
 * \return the number of bytes read
 * \sa vaht_wav_reset vaht_wav_samplerate vaht_wav_samplecount
 * \sa vaht_wav_samplesize vaht_wav_channels
 */
uint32_t vaht_wav_read(vaht_wav* wav, uint32_t size, void* buffer);

/**
 * \brief reset the sound reading to the beginning
 *
 * Use this function to reset the sound to the beginning. A call to
 * \ref vaht_wav_read after this will read data from the start of the sound.
 * This can be used to reuse sound objects, or to loop a sound.
 * \param wav the sound object to reset
 * \sa vaht_wav_read
 */
void vaht_wav_reset(vaht_wav* wav);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_WAV_H__ */
