
#ifndef WEBRTC_NET_DEFAULTS_H_
#define WEBRTC_NET_DEFAULTS_H_
#pragma once

#include "webrtc/media/base/videocapturer.h"
#include "webrtc/media/base/yuvframegenerator.h"
#include "webrtc/api/mediastreaminterface.h"

namespace Native
{
	class Conductor;

	class YuvFramesCapturer2 : public cricket::VideoCapturer
	{
	public:
		YuvFramesCapturer2(Conductor & c);
		virtual ~YuvFramesCapturer2();

		// Override virtual methods of parent class VideoCapturer.
		virtual cricket::CaptureState Start(const cricket::VideoFormat& capture_format);
		virtual void Stop();
		virtual bool IsRunning();
		virtual bool IsScreencast() const
		{
			return false;
		}

	protected:
		// Override virtual methods of parent class VideoCapturer.
		virtual bool GetPreferredFourccs(std::vector<uint32_t>* fourccs);

		// Read a frame and determine how long to wait for the next frame.
		void ReadFrame(bool first_frame);

	private:
		class YuvFramesThread;  // Forward declaration, defined in .cc.

		Conductor * con;
		YuvFramesThread* frames_generator_thread;
		cricket::YuvFrameGenerator* frame_generator_;

		rtc::scoped_refptr<webrtc::I420Buffer> video_buffer;
		cricket::VideoFrame * video_frame;

		int width_;
		int height_;
		uint32_t frame_data_size_;
		uint32_t frame_index_;

		int64_t barcode_reference_timestamp_millis_;
		int32_t barcode_interval_;

		RTC_DISALLOW_COPY_AND_ASSIGN(YuvFramesCapturer2);
	};

	class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame>
	{
	public:
		VideoRenderer(Conductor & c, bool remote, webrtc::VideoTrackInterface * track_to_render) :
			rendered_track_(track_to_render), con(&c), remote(remote)
		{
			rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
		}
		virtual ~VideoRenderer()
		{
			rendered_track_->RemoveSink(this);
		}

		// VideoSinkInterface implementation
		void OnFrame(const webrtc::VideoFrame& frame) override;

	protected:

		bool remote;
		Conductor * con;
		rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;		
	};

	class AudioRenderer : public webrtc::AudioTrackSinkInterface
	{
	public:
		AudioRenderer(Conductor & c, bool remote, webrtc::AudioTrackInterface * track_to_render) :
			audio_track_(track_to_render), con(&c), remote(remote)
		{
			audio_track_->AddSink(this);
		}
		virtual ~AudioRenderer()
		{
			audio_track_->RemoveSink(this);
		}

		virtual void OnData(const void* audio_data,
							int bits_per_sample,
							int sample_rate,
							size_t number_of_channels,
							size_t number_of_frames) override;

	protected:

		bool remote;
		Conductor * con;
		rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track_;
	};	
}
#endif  // WEBRTC_NET_DEFAULTS_H_
