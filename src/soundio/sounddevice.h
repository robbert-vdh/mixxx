#pragma once

#include <QString>
#include <QList>

#include "util/types.h"
#include "preferences/usersettings.h"
#include "soundio/sounddeviceerror.h"
#include "soundio/soundmanagerutil.h"

class SoundDevice;
class SoundManager;
class AudioOutput;
class AudioInput;
class AudioOutputBuffer;
class AudioInputBuffer;

const QString kNetworkDeviceInternalName = "Network stream";

class SoundDevice {
  public:
    SoundDevice(UserSettingsPointer config, SoundManager* sm);
    virtual ~SoundDevice() = default;

    inline const SoundDeviceId& getDeviceId() const {
        return m_deviceId;
    }
    inline const QString& getDisplayName() const {
        return m_strDisplayName;
    }
    inline const QString& getHostAPI() const {
        return m_hostAPI;
    }
    void setSampleRate(double sampleRate);
    void setFramesPerBuffer(unsigned int framesPerBuffer);
    virtual SoundDeviceError open(bool isClkRefDevice, int syncBuffers) = 0;
    virtual bool isOpen() const = 0;
    virtual SoundDeviceError close() = 0;
    virtual void readProcess(SINT framesPerBuffer) = 0;
    virtual void writeProcess(SINT framesPerBuffer) = 0;
    virtual QString getError() const = 0;
    virtual unsigned int getDefaultSampleRate() const = 0;
    int getNumOutputChannels() const;
    int getNumInputChannels() const;
    SoundDeviceError addOutput(const AudioOutputBuffer& out);
    SoundDeviceError addInput(const AudioInputBuffer& in);
    const QList<AudioInputBuffer>& inputs() const {
        return m_audioInputs;
    }
    const QList<AudioOutputBuffer>& outputs() const {
        return m_audioOutputs;
    }

    void clearOutputs();
    void clearInputs();
    bool operator==(const SoundDevice &other) const;
    bool operator==(const QString &other) const;

  protected:
    /**
     * Broadcast the buffer size in milliseconds to the GUI if it has changed.
     * This affects the waveform visualizers and a couple other components. This
     * should be called at the start of a process callback.
     */
    void maybeUpdateBufferSize(double bufferSizeMs);

    void composeOutputBuffer(CSAMPLE* outputBuffer,
                             const SINT iFramesPerBuffer,
                             const SINT readOffset,
                             const int iFrameSize);

    void composeInputBuffer(const CSAMPLE* inputBuffer,
                            const SINT framesToPush,
                            const SINT framesWriteOffset,
                            const int iFrameSize);

    void clearInputBuffer(const SINT framesToPush,
                          const SINT framesWriteOffset);

    SoundDeviceId m_deviceId;
    UserSettingsPointer m_pConfig;
    // Pointer to the SoundManager object which we'll request audio from.
    SoundManager* m_pSoundManager;
    // The name of the soundcard, as displayed to the user
    QString m_strDisplayName;
    // The number of output channels that the soundcard has
    int m_iNumOutputChannels;
    // The number of input channels that the soundcard has
    int m_iNumInputChannels;
    // The current samplerate for the sound device.
    double m_dSampleRate;
    // The name of the audio API used by this device.
    QString m_hostAPI;
    // The **configured** number of frames per buffer. We'll tell PortAudio we
    // want this many frames in a buffer, but PortAudio may still give us have a
    // differently sized buffers. As such this value should only be used for
    // configuring the audio devices. The actual runtime buffer size should be
    // used for any computations working with audio.
    SINT m_framesPerBuffer;
    QList<AudioOutputBuffer> m_audioOutputs;
    QList<AudioInputBuffer> m_audioInputs;

  private:
    /// The last buffer size that was sent to the `[Master],audio_buffer_size`
    /// control object. This is set in `maybeUpdateBufferSize()`.
    double m_lastBufferSizeMs = 0;
};

typedef QSharedPointer<SoundDevice> SoundDevicePointer;
