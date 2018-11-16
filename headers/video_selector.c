#include "video_selector.h"

void vplayer_init(struct vplayer *vPlayer){
    vPlayer->height = 360;
    vPlayer->width  = 640;
    vPlayer->bitsperpixels = 24;
    vPlayer->rect.x = 0;
    vPlayer->rect.y = 0;
    vPlayer->rect.w = vPlayer->width;//pCodecCtx->width;
    vPlayer->rect.h = vPlayer->height;//pCodecCtx->height; 
 }

static int vplayer_start(struct vplayer *vPlayer) {
   
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    vPlayer->screen = SDL_SetVideoMode(
                vPlayer->width,vPlayer->height, vPlayer->bitsperpixels, 0
                ); 

    if(!(vPlayer->screen) ) {
        fprintf(stderr, "SDL: could not set video mode - exiting\n");
        return -1;
    }

    // Allocate a place to put our YUV image on that screen
    vPlayer->bmp = SDL_CreateYUVOverlay(vPlayer->width,//pCodecCtx->width,
                           vPlayer->height, //pCodecCtx->height,
                           SDL_YV12_OVERLAY,
                           vPlayer->screen
                               );

    return 0;

}

static int vplayer_eventManager(struct vplayer *vPlayer){
    SDL_PollEvent(&(vPlayer->event));
    switch(vPlayer->event.type) {
        case SDL_QUIT:
              SDL_Quit();
              return 1;
              break;
        default:
              break;
        }
    return 0;
}

static int vplayer_quit(struct vplayer *vPlayer){
    SDL_Quit();
}

int play_video(char *file,  struct vplayer *vPlayer ) {
        
    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext  *pCodecCtxOrig = NULL;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVFrame         *pFrame = NULL;
    AVPacket        packet;
    int             i; 
    int             videoStream, frameFinished;
    float           aspect_ratio;
    struct SwsContext *sws_ctx = NULL;

    av_register_all(); // Register all formats and codecs
  
    if( avformat_open_input(&pFormatCtx, file, NULL, NULL) != 0 )
        return -1; 
  
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; 

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, file, 0);
  
    // Find the first video stream
    videoStream=-1;
    for( i=0; i < (pFormatCtx->nb_streams); i++ )
        if( pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO ) {
            videoStream=i;
            break;
        }

    if( videoStream == -1 )
        return -1; 
  
    // Get a pointer to the codec context for the video stream
    pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
    
    pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec == NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        return -1;
    }

    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
        fprintf(stderr, "Couldn't copy codec context");
        return -1;
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
        return -1;
  
    pFrame = av_frame_alloc();

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
			   pCodecCtx->height,
			   pCodecCtx->pix_fmt,
			   vPlayer->width,// pCodecCtx->width,
			   vPlayer->height,//pCodecCtx->height,
			   AV_PIX_FMT_YUV420P,
			   SWS_BILINEAR,
			   NULL, NULL, NULL
            );
      
   
    vplayer_start(vPlayer);
    i = 0;
    int closeWindows = 0;
    while( (av_read_frame(pFormatCtx, &packet) >= 0) && !closeWindows  ) {
    // Is this a packet from the video stream?
        if( packet.stream_index == videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
      
           // Did we get a video frame?
            if(frameFinished) {
                SDL_LockYUVOverlay(vPlayer->bmp);

                AVPicture pict;
                pict.data[0] = vPlayer->bmp->pixels[0];
                pict.data[1] = vPlayer->bmp->pixels[2];
                pict.data[2] = vPlayer->bmp->pixels[1];

                pict.linesize[0] = vPlayer->bmp->pitches[0];
                pict.linesize[1] = vPlayer->bmp->pitches[2];
                pict.linesize[2] = vPlayer->bmp->pitches[1];

                // Convert the image into YUV format that SDL uses
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                      pFrame->linesize, 0, pCodecCtx->height,
                      pict.data, pict.linesize
                      );

                SDL_UnlockYUVOverlay(vPlayer->bmp);
                SDL_DisplayYUVOverlay(vPlayer->bmp, &(vPlayer->rect));
 
              }
            }
    
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
        closeWindows =  vplayer_eventManager(vPlayer) ;
    }

    if ( !closeWindows )   vplayer_quit(vPlayer); 

    // Free the YUV frame
    av_frame_free(&pFrame);
  
    // Close the codec
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);
    avformat_close_input(&pFormatCtx);
  
    return 0;
}
