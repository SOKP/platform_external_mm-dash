/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DASH_PLAYER_H_

#define DASH_PLAYER_H_

#include "DashPlayerStats.h"
#include <media/MediaPlayerInterface.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <gui/Surface.h>

//Keys for playback modes within TSB
#define KEY_DASH_SEEK_EVENT 7001
#define KEY_DASH_PAUSE_EVENT 7002
#define KEY_DASH_RESUME_EVENT 7003

#define KEY_DASH_ADAPTION_PROPERTIES 8002
#define KEY_DASH_MPD_QUERY           8003
#define KEY_DASH_QOE_EVENT           8004
#define KEY_DASH_QOE_PERIODIC_EVENT  8008
//Keys to get and set mpd properties xml string
#define KEY_DASH_GET_ADAPTION_PROPERTIES 8010
#define KEY_DASH_SET_ADAPTION_PROPERTIES 8011

//Key to query reposition range
#define KEY_DASH_REPOSITION_RANGE    9000

//Key to push blank frame to native window
#define INVOKE_ID_PUSH_BLANK_FRAME 9001

namespace android {

struct MetaData;
struct DashPlayerDriver;

enum {
        kWhatQOE,
        kWhatQOEPlay,
        kWhatQOEStop,
        kWhatQOESwitch,
        kWhatQOEPeriodic,
    };

struct DashPlayer : public AHandler {
    DashPlayer();

    void setUID(uid_t uid);

    void setDriver(const wp<DashPlayerDriver> &driver);

    void setDataSource(const sp<IStreamSource> &source);

    status_t  setDataSource(
            const char *url, const KeyedVector<String8, String8> *headers);

    void setDataSource(int fd, int64_t offset, int64_t length);

    void setVideoSurfaceTexture(const sp<IGraphicBufferProducer> &bufferProducer);

    void setAudioSink(const sp<MediaPlayerBase::AudioSink> &sink);
    void start();

    void pause();
    void resume();

    // Will notify the driver through "notifyResetComplete" once finished.
    void resetAsync();

    // Will notify the driver through "notifySeekComplete" once finished.
    void seekToAsync(int64_t seekTimeUs);

    status_t prepareAsync();
    status_t getParameter(int key, Parcel *reply);
    status_t setParameter(int key, const Parcel &request);
    status_t dump(int fd, const Vector<String16> &args);
    status_t pushBlankBuffersToNativeWindow();

    status_t getSelectedTrack(int32_t type, Parcel* reply);
public:
    struct DASHHTTPLiveSource;

protected:
    virtual ~DashPlayer();

    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    struct Decoder;
    struct CCDecoder;
    struct Renderer;
    struct Source;
    struct Action;
    struct SimpleAction;
    struct SetSurfaceAction;
    struct ShutdownDecoderAction;

    enum {
          // These keys must be in sync with the keys in QCTimedText.java
          KEY_DISPLAY_FLAGS                 = 1, // int
          KEY_STYLE_FLAGS                   = 2, // int
          KEY_BACKGROUND_COLOR_RGBA         = 3, // int
          KEY_HIGHLIGHT_COLOR_RGBA          = 4, // int
          KEY_SCROLL_DELAY                  = 5, // int
          KEY_WRAP_TEXT                     = 6, // int
          KEY_START_TIME                    = 7, // int
          KEY_STRUCT_BLINKING_TEXT_LIST     = 8, // List<CharPos>
          KEY_STRUCT_FONT_LIST              = 9, // List<Font>
          KEY_STRUCT_HIGHLIGHT_LIST         = 10,// List<CharPos>
          KEY_STRUCT_HYPER_TEXT_LIST        = 11,// List<HyperText>
          KEY_STRUCT_KARAOKE_LIST           = 12,// List<Karaoke>
          KEY_STRUCT_STYLE_LIST             = 13,// List<Style>
          KEY_STRUCT_TEXT_POS               = 14,// TextPos
          KEY_STRUCT_JUSTIFICATION          = 15,// Justification
          KEY_STRUCT_TEXT                   = 16,// Text
          KEY_HEIGHT                        = 17,
          KEY_WIDTH                         = 18,
          KEY_DURATION                      = 19,
          KEY_START_OFFSET                  = 20,
          KEY_SUB_ATOM                      = 21,
          KEY_TEXT_FORMAT                   = 22,
          KEY_GLOBAL_SETTING                = 101,
          KEY_LOCAL_SETTING                 = 102,
          KEY_START_CHAR                    = 103,
          KEY_END_CHAR                      = 104,
          KEY_FONT_ID                       = 105,
          KEY_FONT_SIZE                     = 106,
          KEY_TEXT_COLOR_RGBA               = 107,
          KEY_TEXT_EOS                      = 108,
          KEY_TEXT_FLAG_TYPE                = 109,
          KEY_TEXT_DISCONTINUITY            = 110,
    };

    enum {
        kWhatSetDataSource              = '=DaS',
        kWhatSetVideoNativeWindow       = '=NaW',
        kWhatSetAudioSink               = '=AuS',
        kWhatStart                      = 'strt',
        kWhatScanSources                = 'scan',
        kWhatVideoNotify                = 'vidN',
        kWhatAudioNotify                = 'audN',
        kWhatClosedCaptionNotify        = 'capN',
        kWhatTextNotify                 = 'texN',
        kWhatRendererNotify             = 'renN',
        kWhatReset                      = 'rset',
        kWhatSeek                       = 'seek',
        kWhatPause                      = 'paus',
        kWhatResume                     = 'rsme',
        kWhatPrepareAsync               = 'pras',
        kWhatIsPrepareDone              = 'prdn',
        kWhatSourceNotify               = 'snfy',
    };

    enum {
        kWhatBufferingStart             = 'bfst',
        kWhatBufferingEnd               = 'bfen',
    };

    wp<DashPlayerDriver> mDriver;
    bool mUIDValid;
    uid_t mUID;
    sp<Source> mSource;
    sp<Surface> mNativeWindow;
    sp<MediaPlayerBase::AudioSink> mAudioSink;
    sp<Decoder> mVideoDecoder;
    bool mVideoIsAVC;
    sp<Decoder> mAudioDecoder;
    sp<Decoder> mTextDecoder;
    sp<Renderer> mRenderer;
    sp<CCDecoder> mCCDecoder;
    List<sp<Action> > mDeferredActions;

    bool mAudioEOS;
    bool mVideoEOS;

    bool mScanSourcesPending;
    bool isSetSurfaceTexturePending;
    int32_t mScanSourcesGeneration;
    bool mBufferingNotification;

    enum TrackName {
        kVideo = 0,
        kAudio,
        kText,
        kTrackAll,
    };

    enum FlushStatus {
        NONE,
        AWAITING_DISCONTINUITY,
        FLUSHING_DECODER,
        FLUSHING_DECODER_SHUTDOWN,
        SHUTTING_DOWN_DECODER,
        FLUSHED,
        SHUT_DOWN,
    };

    //Should be in sync with QCTimedText.java
    enum FrameFlags {
         TIMED_TEXT_FLAG_FRAME = 0x00,
         TIMED_TEXT_FLAG_CODEC_CONFIG,
         TIMED_TEXT_FLAG_EOS,
         TIMED_TEXT_FLAG_END = TIMED_TEXT_FLAG_EOS,
    };

    //Currently we only support SMPTE and CEA. Today sendTextPacket() has default parameter as SMPTE
    enum TimedTextType {
        TIMED_TEXT_SMPTE,
        TIMED_TEXT_CEA,
        TIMED_TEXT_UNKNOWN,
    };

    // Once the current flush is complete this indicates whether the
    // notion of time has changed.
    bool mTimeDiscontinuityPending;

    FlushStatus mFlushingAudio;
    FlushStatus mFlushingVideo;
    bool mResetInProgress;
    bool mResetPostponed;
    bool mSetVideoSize;

    int64_t mSkipRenderingAudioUntilMediaTimeUs;
    int64_t mSkipRenderingVideoUntilMediaTimeUs;

    int64_t mVideoLateByUs;

    bool mPauseIndication;

    Mutex mLock;

    char *mTrackName;
    sp<AMessage> mTextNotify;
    sp<AMessage> mSourceNotify;
    sp<AMessage> mQOENotify;

    int32_t mSRid;

    status_t instantiateDecoder(int track, sp<Decoder> *decoder);

    status_t feedDecoderInputData(int track, const sp<AMessage> &msg);
    void renderBuffer(bool audio, const sp<AMessage> &msg);

    void notifyListener(int msg, int ext1, int ext2, const Parcel *obj=NULL);

    void finishFlushIfPossible();

    void flushDecoder(bool audio, bool needShutdown);

    status_t selectTrack(size_t trackIndex, bool select);

    static bool IsFlushingState(FlushStatus state, bool *needShutdown = NULL);

    void finishReset();
    void postScanSources();

    sp<Source> LoadCreateSource(const char * uri, const KeyedVector<String8,
                                 String8> *headers, bool uidValid, uid_t uid);

    void postIsPrepareDone();

    // for qualcomm statistics profiling
    sp<DashPlayerStats> mStats;

    void sendTextPacket(sp<ABuffer> accessUnit, status_t err, DashPlayer::TimedTextType eTimedTextType = TIMED_TEXT_SMPTE);
    void getTrackName(int track, char* name);
    void prepareSource();

    void processDeferredActions();

    void performDecoderShutdown(bool audio, bool video);
    void performScanSources();
    void performSetSurface(const sp<Surface> &wrapper);

    int mLogLevel;
    bool mTimedTextCEAPresent;

    //Set and reset in cases of seek/resume-out-of-tsb to signal discontinuity in CEA timedtextsamples
    bool mTimedTextCEASamplesDisc;

    void onClosedCaptionNotify(const sp<AMessage> &msg);
    void sendSubtitleData(const sp<ABuffer> &buffer, int32_t baseIndex);
    void writeTrackInfo(Parcel* reply, const sp<AMessage> format) const;

    int32_t mCurrentWidth;
    int32_t mCurrentHeight;
    int32_t mColorFormat;

    int mDPBSize;
    bool mDPBCheckToDelayRendering;
    int64_t mVideoDecoderStartTimeUs;
    int64_t mVideoDecoderSetupTimeUs;
    int64_t mDelayRenderingUs;
    bool    mStarted;
    int64_t mFirstVideoSampleUs;
    int64_t mVideoSampleDurationUs;

    int64_t mLastReadAudioMediaTimeUs;
    int64_t mLastReadAudioRealTimeUs;

    DISALLOW_EVIL_CONSTRUCTORS(DashPlayer);
};

}  // namespace android

#endif  // DASH_PLAYER_H_
