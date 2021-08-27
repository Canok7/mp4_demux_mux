#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "demux/mp4.h"
#include "input/my_file_stream.h"
#include "vlc_demux.h"
int main(int argc, const char* argv[]){
    printf("[%s%d] hello world\n",__FUNCTION__,__LINE__);
    demux_t mp4demuxer;
    memset(&mp4demuxer,0,sizeof(mp4demuxer));
    mp4demuxer.s = CreateFileStream(NULL,"new_noBf.mp4");
    Open(&mp4demuxer);

    int count = 100;
    int ret =0;

    demux_Control( &mp4demuxer, DEMUX_SET_TIME, 0, true );
    while(count-->0){
         ret = Demux(&mp4demuxer);
        printf("[%s%d] demux:%d count:%d\n",__FUNCTION__,__LINE__,ret,count);
    }

}
