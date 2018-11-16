#include "parser.h"

/* ------------------------------------------- */
/*      extra functions                        */
/* ------------------------------------------- */
static int check_resolution(int res){
    switch(res){
        case 360:
        case 480:
        case 720:
        case 1080:
            return res;
        default:
            return 360;
    }
}

static int check_fps(int fps){
    return ( (fps <= 0)?120:fps );
}


/* ------------------------------------  */
/*           input parser                */
/* ------------------------------------  */
struct option opts_list[] = {
  {"file",1,0,'f'},
  {"resolution",1,0,'r'},
  {"fps",1,0,'s'},
  {"help",0,0,'h'},
  {0,0,0,0}
};

int parse_input(int argc, char **argv, struct vplayer *vPlayer)
{
    int c;

    vPlayer->height = 360;
    vPlayer->width  = 640;
    vPlayer->bitsperpixels = 24;
    vPlayer->rect.x = 0;
    vPlayer->rect.y = 0;
    vPlayer->rect.w = vPlayer->width;
    vPlayer->rect.h = vPlayer->height;
    vPlayer->fps = 120;
    vPlayer->file = NULL;
 
    while ( (c = getopt_long(argc, argv, "f:r:s:h", opts_list, NULL)) != EOF){
        switch(c){
            case 'f':
                vPlayer->file = optarg;
                break;

            case 'r':
                vPlayer->height = check_resolution( atoi(optarg) ) ;
                vPlayer->width  = (vPlayer->height*16/9) ;
                break;

            case 's':
                vPlayer->fps = check_fps( atoi(optarg) );
                break;

            case 'h':
                puts("-f, --file:\t  file or folder name of the video(s)\n"
                     "-r, --resolution:\t resolution of the video player\n"
                     "-s, --fps:\t video playback speed (frames per seconds)\n"
                     "-h, --help:\t help"
                     );
                exit(0);
                break;

            default:
                 exit(1);
        }
    }

    if (vPlayer->file == NULL){
        fprintf(stderr,"no input file... exiting\n" );
        exit(1);
    }
 
    return 1;  
}



