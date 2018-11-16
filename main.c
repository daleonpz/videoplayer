#include "headers/parser.h"
#include "headers/video_selector.h"

/* ------------------------------------------- */
/*  
    TODO                                   
*/
/* ------------------------------------------- */



#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILE_LENGTH 100
#define MAX_YES_NO_LENGHT 5

static int valid_format(char *file){
    char *ptr;
     ptr = rindex(file, '.');
     if ((ptr != NULL) &&
                 ((strcmp(ptr, ".MOV") == 0) ||
             (strcmp(ptr, ".mp4") == 0) ||
             (strcmp(ptr, ".MP4") == 0) || 
             (strcmp(ptr, ".mpeg") == 0) ||
             (strcmp(ptr, ".avi") ==0  ))
       )
         return 1;
     else 
        return 0;
}

static int file_select(struct dirent *entry){

    if ((strcmp(entry->d_name, ".")== 0) ||
            (strcmp(entry->d_name, "..") == 0)
            )
	 return 0;

    return ( valid_format(entry->d_name) );
}

static int keepvid(char *filename){

    char yn[MAX_YES_NO_LENGHT];
    char *p = yn;

    while(1){
        puts("---------------------------");
        printf("Do you want to keep this video? [Y/n]: ");

        if( fgets(p, MAX_YES_NO_LENGHT, stdin) != NULL){
            while( *p != '\0') {*p = (char) tolower(*p); p++;}

            if (( strncmp(yn,"n",1) == 0) || ( strncmp(yn,"no",2) == 0)){
                remove(filename);
                printf("file deleted \n");
                break;
            }
            else if(( strncmp(yn,"y",1) == 0) || ( strncmp(yn,"yes",3) == 0))
                break;
            else
                puts("invalid option");

        }
        else{
            fprintf(stderr, "couldn't get any input\n");
            return -1;
        }
    }

    return 0;
}

int main( int argc, char **argv){
    
    struct vplayer vPlayer;

    // TODO: post about passing arg by value, trolling my pointer
    parse_input(argc, argv, &vPlayer);
    vplayer_init(&vPlayer);
    
    struct dirent **eps;
    int n;

    n = scandir (vPlayer.file, &eps, file_select, alphasort);
    if (n >= 0) {
      int cnt;
      char *fullpath = (char *)malloc(MAX_FILE_LENGTH);

      for (cnt = 0; cnt < n; ++cnt){
            sprintf(fullpath,"%s%s", vPlayer.file, eps[cnt]->d_name);
            puts(fullpath);
            play_video(fullpath, &vPlayer);
            keepvid(fullpath);
        }
      free(fullpath);
    }
    else if ( ( access( vPlayer.file, F_OK) == 0 ) && valid_format(vPlayer.file ) ){
                puts(vPlayer.file);
                play_video(vPlayer.file, &vPlayer);
                keepvid(vPlayer.file);
        }       
    else{
        fprintf(stderr,"No valid video file format or couldn't open the directory\n");
        return -1;
    }

    return 0;

}
