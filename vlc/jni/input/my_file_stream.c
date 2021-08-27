//created by canok
//20210826 make a simple stream moudle which read from file
#include "stream.h"
#include "my_file_stream.h"
struct stream_sys_t
{
    FILE *fp;
    uint8_t  *p_buffer;
};

static ssize_t Read( stream_t *, void *p_read, size_t i_read );
static int Seek( stream_t *, uint64_t );
static int  Control( stream_t *, int i_query, va_list );

static void stream_fileDestroy(stream_t *s)
{
    stream_sys_t *sys = s->p_sys;

    fclose(sys->fp);
    if(sys->p_buffer !=NULL){
        free(sys->p_buffer);
    }
}

stream_t *CreateFileStream(vlc_object_t *p_parent,const char* file)
{
    msg_Dbg(p_parent, "can track CreateFileStream");
    stream_t *s = vlc_stream_CommonNew( p_parent,stream_fileDestroy );
    stream_sys_t *p_sys;

    if( !s )
        return NULL;

    s->p_sys = p_sys = malloc( sizeof( stream_sys_t ) );
    if( !s->p_sys )
    {
        stream_CommonDelete( s );
        return NULL;
    }
    p_sys->fp = fopen(file,"r");
    if(p_sys->fp == NULL){
         msg_Dbg(s, "openfile err!:%s",file);
    }
    p_sys->p_buffer = NULL;

    s->pf_read    = Read;
    s->pf_seek    = Seek;
    s->pf_control = Control;
    return s;
}

/****************************************************************************
 * AStreamControl:
 ****************************************************************************/
static int Control( stream_t *s, int cmd, va_list args )
{
    msg_Dbg(s, "vlc_stream_vaContrl: cmd:%d",cmd);
       stream_sys_t *sys = s->p_sys;
       bool    *pb_bool;
       int64_t *pi_64;
       switch( cmd )
         {
             case STREAM_CAN_SEEK:
             case STREAM_CAN_FASTSEEK:
                 pb_bool = va_arg( args, bool * );
                 *pb_bool = true;
                 break;
             case STREAM_CAN_PAUSE:
             case STREAM_CAN_CONTROL_PACE:
                 pb_bool = va_arg( args, bool * );
                 *pb_bool = true;
                 break;
             case STREAM_GET_SIZE:
             {
                 //struct stat st;
                 //if (fstat (g_fp, &st) || !S_ISREG(st.st_mode))
                 //    return VLC_EGENERIC;
                 //*va_arg( args, uint64_t * ) = st.st_size;
                 int cur=ftell(sys->fp);
                 fseek(sys->fp,0L,SEEK_END);
                 int size=ftell(sys->fp);
                 fseek(sys->fp,cur,SEEK_SET);
                 *va_arg( args, uint64_t * ) = size;
                 break;
             }
             case STREAM_GET_PTS_DELAY:
                 pi_64 = va_arg( args, int64_t * );
                 *pi_64 *= 0;
                 break;
             case STREAM_SET_PAUSE_STATE:
               break;
               default:
               break;
           }
    return VLC_SUCCESS;
}

static ssize_t Read( stream_t *s, void *p_read, size_t i_read )
{
    stream_sys_t *p_sys = s->p_sys;

    if(p_sys->p_buffer!=NULL){
        free(p_sys->p_buffer);
    }
    p_sys->p_buffer= malloc(i_read);
    if(p_sys->p_buffer==NULL){
        msg_Dbg(s, "malloc err");
        return 0;
    }
    int ret = fread(p_sys->p_buffer,1,i_read,p_sys->fp);
    if(ret <i_read){
        msg_Err(s, "fread_errr");
        return 0;
    }
    
    if ( p_read )
        memcpy( p_read, p_sys->p_buffer, i_read );
    return i_read;
}

static int Seek( stream_t *s, uint64_t offset )
{
     stream_sys_t *p_sys = s->p_sys;
     msg_Dbg(s, "seek:%ld",offset);
     fseek(p_sys->fp,offset,SEEK_SET);
     return VLC_SUCCESS;
}

