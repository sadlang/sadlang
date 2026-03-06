package sad.android.communication.webrtc

import android.content.Context
import android.media.AudioManager
import org.webrtc.*

/**
 * جسر WebRTC للاتصالات الصوتية والمرئية في الوقت الحقيقي
 * WebRTC Bridge - Real-time audio/video communication
 * 
 * @author فريق لغة ص
 */
object SadWebRTCBridge {
    
    private var peerConnectionFactory: PeerConnectionFactory? = null
    private var localPeer: PeerConnection? = null
    private var localVideoTrack: VideoTrack? = null
    private var localAudioTrack: AudioTrack? = null
    private var videoCapturer: CameraVideoCapturer? = null
    private var surfaceTextureHelper: SurfaceTextureHelper? = null
    private var eglBase: EglBase? = null
    
    /**
     * حالة ICE / ICE state
     */
    enum class IceConnectionState {
        NEW, CHECKING, CONNECTED, COMPLETED, 
        DISCONNECTED, FAILED, CLOSED
    }
    
    /**
     * حالة الإشارة / Signaling state
     */
    enum class SignalingState {
        STABLE, HAVE_LOCAL_OFFER, HAVE_REMOTE_OFFER,
        HAVE_LOCAL_PRANSWER, HAVE_REMOTE_PRANSWER, CLOSED
    }
    
    /**
     * إعدادات الفيديو / Video settings
     */
    data class VideoSettings(
        val width: Int = 640,
        val height: Int = 480,
        val fps: Int = 30,
        val useFrontCamera: Boolean = true
    )
    
    /**
     * خادم ICE / ICE server
     */
    data class IceServer(
        val url: String,
        val username: String? = null,
        val password: String? = null
    )
    
    /**
     * مستمع الاتصال / Connection listener
     */
    interface WebRTCListener {
        fun onLocalSdp(sdp: String, type: String)
        fun onIceCandidate(candidate: String, sdpMid: String?, sdpMLineIndex: Int)
        fun onIceConnectionStateChange(state: IceConnectionState)
        fun onAddStream(streamId: String)
        fun onError(message: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة WebRTC / Initialize WebRTC
     */
    fun initialize(context: Context) {
        // تهيئة EGL
        eglBase = EglBase.create()
        
        // تهيئة مصنع الاتصالات
        PeerConnectionFactory.initialize(
            PeerConnectionFactory.InitializationOptions.builder(context)
                .setEnableInternalTracer(true)
                .createInitializationOptions()
        )
        
        // إنشاء المصنع
        peerConnectionFactory = PeerConnectionFactory.builder()
            .setVideoEncoderFactory(DefaultVideoEncoderFactory(
                eglBase?.eglBaseContext, true, true
            ))
            .setVideoDecoderFactory(DefaultVideoDecoderFactory(eglBase?.eglBaseContext))
            .createPeerConnectionFactory()
    }
    
    /**
     * إنشاء اتصال / Create peer connection
     */
    fun createPeerConnection(
        context: Context,
        iceServers: List<IceServer>,
        listener: WebRTCListener
    ): Boolean {
        val factory = peerConnectionFactory ?: return false
        
        val rtcConfig = PeerConnection.RTCConfiguration(
            iceServers.map { server ->
                PeerConnection.IceServer.builder(server.url).apply {
                    server.username?.let { setUsername(it) }
                    server.password?.let { setPassword(it) }
                }.createIceServer()
            }
        ).apply {
            bundlePolicy = PeerConnection.BundlePolicy.MAXBUNDLE
            rtcpMuxPolicy = PeerConnection.RtcpMuxPolicy.REQUIRE
            continualGatheringPolicy = PeerConnection.ContinualGatheringPolicy.GATHER_CONTINUALLY
        }
        
        val observer = object : PeerConnection.Observer {
            override fun onSignalingChange(state: PeerConnection.SignalingState?) {}
            
            override fun onIceConnectionChange(state: PeerConnection.IceConnectionState?) {
                state?.let {
                    listener.onIceConnectionStateChange(
                        IceConnectionState.valueOf(it.name)
                    )
                }
            }
            
            override fun onIceConnectionReceivingChange(receiving: Boolean) {}
            
            override fun onIceGatheringChange(state: PeerConnection.IceGatheringState?) {}
            
            override fun onIceCandidate(candidate: IceCandidate?) {
                candidate?.let {
                    listener.onIceCandidate(it.sdp, it.sdpMid, it.sdpMLineIndex)
                }
            }
            
            override fun onIceCandidatesRemoved(candidates: Array<out IceCandidate>?) {}
            
            override fun onAddStream(stream: MediaStream?) {
                stream?.let { listener.onAddStream(it.id) }
            }
            
            override fun onRemoveStream(stream: MediaStream?) {}
            
            override fun onDataChannel(channel: DataChannel?) {}
            
            override fun onRenegotiationNeeded() {}
            
            override fun onAddTrack(receiver: RtpReceiver?, streams: Array<out MediaStream>?) {}
        }
        
        localPeer = factory.createPeerConnection(rtcConfig, observer)
        return localPeer != null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الكاميرا
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تشغيل الكاميرا / Start camera
     */
    fun startCamera(
        context: Context,
        settings: VideoSettings = VideoSettings(),
        localView: SurfaceViewRenderer? = null
    ): Boolean {
        val factory = peerConnectionFactory ?: return false
        
        // إنشاء مصدر الفيديو
        val videoSource = factory.createVideoSource(false)
        localVideoTrack = factory.createVideoTrack("video_track", videoSource)
        
        // إنشاء ملتقط الفيديو
        videoCapturer = createCameraCapturer(context, settings.useFrontCamera)
        
        surfaceTextureHelper = SurfaceTextureHelper.create(
            "CaptureThread",
            eglBase?.eglBaseContext
        )
        
        videoCapturer?.initialize(surfaceTextureHelper, context, videoSource.capturerObserver)
        videoCapturer?.startCapture(settings.width, settings.height, settings.fps)
        
        // عرض محلي
        localView?.let {
            it.init(eglBase?.eglBaseContext, null)
            it.setMirror(settings.useFrontCamera)
            localVideoTrack?.addSink(it)
        }
        
        return true
    }
    
    /**
     * إيقاف الكاميرا / Stop camera
     */
    fun stopCamera() {
        videoCapturer?.stopCapture()
        videoCapturer?.dispose()
        videoCapturer = null
        
        localVideoTrack?.dispose()
        localVideoTrack = null
    }
    
    /**
     * تبديل الكاميرا / Switch camera
     */
    fun switchCamera() {
        videoCapturer?.switchCamera(null)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الصوت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تشغيل الصوت / Start audio
     */
    fun startAudio(context: Context): Boolean {
        val factory = peerConnectionFactory ?: return false
        
        val audioSource = factory.createAudioSource(MediaConstraints())
        localAudioTrack = factory.createAudioTrack("audio_track", audioSource)
        
        // تفعيل وضع الاتصال
        val audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        audioManager.mode = AudioManager.MODE_IN_COMMUNICATION
        
        return true
    }
    
    /**
     * إيقاف الصوت / Stop audio
     */
    fun stopAudio(context: Context) {
        localAudioTrack?.dispose()
        localAudioTrack = null
        
        val audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        audioManager.mode = AudioManager.MODE_NORMAL
    }
    
    /**
     * كتم الصوت / Mute audio
     */
    fun setAudioEnabled(enabled: Boolean) {
        localAudioTrack?.setEnabled(enabled)
    }
    
    /**
     * تفعيل الفيديو / Enable video
     */
    fun setVideoEnabled(enabled: Boolean) {
        localVideoTrack?.setEnabled(enabled)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإشارة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء عرض / Create offer
     */
    fun createOffer(listener: WebRTCListener) {
        val peer = localPeer ?: return
        
        // إضافة المسارات
        addLocalTracks()
        
        peer.createOffer(object : SdpObserver {
            override fun onCreateSuccess(sdp: SessionDescription?) {
                sdp?.let {
                    peer.setLocalDescription(object : SdpObserver {
                        override fun onCreateSuccess(p0: SessionDescription?) {}
                        override fun onSetSuccess() {
                            listener.onLocalSdp(it.description, "offer")
                        }
                        override fun onCreateFailure(p0: String?) {}
                        override fun onSetFailure(p0: String?) {
                            listener.onError(p0 ?: "Set local description failed")
                        }
                    }, it)
                }
            }
            
            override fun onSetSuccess() {}
            override fun onCreateFailure(error: String?) {
                listener.onError(error ?: "Create offer failed")
            }
            override fun onSetFailure(p0: String?) {}
        }, MediaConstraints().apply {
            mandatory.add(MediaConstraints.KeyValuePair("OfferToReceiveVideo", "true"))
            mandatory.add(MediaConstraints.KeyValuePair("OfferToReceiveAudio", "true"))
        })
    }
    
    /**
     * إنشاء جواب / Create answer
     */
    fun createAnswer(remoteSdp: String, listener: WebRTCListener) {
        val peer = localPeer ?: return
        
        // تعيين SDP البعيد
        val remoteDescription = SessionDescription(SessionDescription.Type.OFFER, remoteSdp)
        peer.setRemoteDescription(object : SdpObserver {
            override fun onCreateSuccess(p0: SessionDescription?) {}
            override fun onSetSuccess() {
                // إنشاء الجواب
                addLocalTracks()
                
                peer.createAnswer(object : SdpObserver {
                    override fun onCreateSuccess(sdp: SessionDescription?) {
                        sdp?.let {
                            peer.setLocalDescription(object : SdpObserver {
                                override fun onCreateSuccess(p0: SessionDescription?) {}
                                override fun onSetSuccess() {
                                    listener.onLocalSdp(it.description, "answer")
                                }
                                override fun onCreateFailure(p0: String?) {}
                                override fun onSetFailure(p0: String?) {
                                    listener.onError(p0 ?: "Set local description failed")
                                }
                            }, it)
                        }
                    }
                    override fun onSetSuccess() {}
                    override fun onCreateFailure(error: String?) {
                        listener.onError(error ?: "Create answer failed")
                    }
                    override fun onSetFailure(p0: String?) {}
                }, MediaConstraints())
            }
            override fun onCreateFailure(p0: String?) {}
            override fun onSetFailure(error: String?) {
                listener.onError(error ?: "Set remote description failed")
            }
        }, remoteDescription)
    }
    
    /**
     * تعيين الجواب / Set answer
     */
    fun setRemoteAnswer(remoteSdp: String, listener: WebRTCListener) {
        val peer = localPeer ?: return
        
        val remoteDescription = SessionDescription(SessionDescription.Type.ANSWER, remoteSdp)
        peer.setRemoteDescription(object : SdpObserver {
            override fun onCreateSuccess(p0: SessionDescription?) {}
            override fun onSetSuccess() {}
            override fun onCreateFailure(p0: String?) {}
            override fun onSetFailure(error: String?) {
                listener.onError(error ?: "Set remote answer failed")
            }
        }, remoteDescription)
    }
    
    /**
     * إضافة مرشح ICE / Add ICE candidate
     */
    fun addIceCandidate(candidate: String, sdpMid: String?, sdpMLineIndex: Int) {
        val iceCandidate = IceCandidate(sdpMid ?: "", sdpMLineIndex, candidate)
        localPeer?.addIceCandidate(iceCandidate)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنظيف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إغلاق الاتصال / Close connection
     */
    fun close() {
        localPeer?.close()
        localPeer = null
    }
    
    /**
     * تحرير الموارد / Dispose resources
     */
    fun dispose() {
        close()
        stopCamera()
        
        surfaceTextureHelper?.dispose()
        surfaceTextureHelper = null
        
        peerConnectionFactory?.dispose()
        peerConnectionFactory = null
        
        eglBase?.release()
        eglBase = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun addLocalTracks() {
        localVideoTrack?.let { video ->
            val stream = peerConnectionFactory?.createLocalMediaStream("local_stream")
            stream?.addTrack(video)
            localAudioTrack?.let { audio -> stream?.addTrack(audio) }
            stream?.let { localPeer?.addStream(it) }
        }
    }
    
    private fun createCameraCapturer(context: Context, useFront: Boolean): CameraVideoCapturer? {
        val enumerator = if (Camera2Enumerator.isSupported(context)) {
            Camera2Enumerator(context)
        } else {
            Camera1Enumerator(true)
        }
        
        val deviceNames = enumerator.deviceNames
        
        // البحث عن الكاميرا المطلوبة
        for (deviceName in deviceNames) {
            val isFront = enumerator.isFrontFacing(deviceName)
            if (isFront == useFront) {
                val capturer = enumerator.createCapturer(deviceName, null)
                if (capturer != null) return capturer
            }
        }
        
        // إرجاع أي كاميرا متاحة
        for (deviceName in deviceNames) {
            val capturer = enumerator.createCapturer(deviceName, null)
            if (capturer != null) return capturer
        }
        
        return null
    }
}
