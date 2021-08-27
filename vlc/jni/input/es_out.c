//create by canok
//20210826 make a es_out which write h264 to file
//i had destory the es_out moudle, this is only work es_out can do!!!
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <assert.h>
#include <vlc_common.h>

#include <vlc_input.h>
#include <vlc_es_out.h>
#include <vlc_block.h>
#include <vlc_aout.h>
#include <vlc_fourcc.h>
#include <vlc_meta.h>
#include <libmp4.h>
#define MAKE64_LEFT(d0,d1,d2,d3,d4,d5,d6,d7) ((int64_t)(d0)<<56)|((int64_t)(d1)<<48)|((int64_t)(d2)<<40)|((int64_t)(d3)<<32)|((int64_t)(d4)<<24)|((int64_t)(d5)<<16)|((int64_t)(d6)<<8)|(int64_t)(d7)
#define MAKE64_LEFT_FROM_ARRY(d) MAKE64_LEFT(d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7])
#define MAKE32_LEFT(d0,d1,d2,d3) ((int32_t)(d0)<<24)|((int32_t)(d1)<<16)|((int32_t)(d2)<<8)|(int32_t)(d3)
#define MAKE16_LEFT(d0,d1) ((int16_t)(d0)<<8)|(int16_t)(d1)

struct es_out_id_t {
    int type;
    FILE *v_fp_out ;
    FILE *a_fp_out ;

    //存储一些我们想要的信息
    unsigned int   i_bitrate; /**< bitrate of this ES */
    int      i_profile;       /**< codec specific information (like real audio flavor, mpeg audio layer, h264 profile ...) */
    int      i_level;         /**< codec specific information: indicates maximum restrictions on the stream (resolution, bitrate, codec features ...) */
    bool     b_packetized;  /**< whether the data is packetized (ie. not truncated) */
    int     i_extra;        /**< length in bytes of extra data pointer */
    void    *p_extra;       /**< extra data needed by some decoders or muxers */

};

int es_out_Send( es_out_t *out, es_out_id_t *id,
                               block_t *p_block ){
    msg_Dbg(out, "es_type:%c datalen:%zu, %ld, %ld","UVASD"[id->type],p_block->i_buffer,p_block->i_dts,p_block->i_pts);
    if(id->type == VIDEO_ES && id->v_fp_out!=NULL){
       //仅仅是对 avcC box
        MP4_Box_data_avcC_t *p_avcC = (MP4_Box_data_avcC_t*)id->p_extra;
        unsigned char *p = p_block->p_buffer;
        unsigned char *end = p+p_block->i_buffer;
        while(end-p>0){
            char nal[4]={0x00,0x00,0x00,0x01};
            int slicelen=0;
            switch(p_avcC->i_length_size){
                case 2:
                   slicelen = MAKE16_LEFT(p[0], p[1]);
                    p+=2;
                    break;
                case 4:
                    slicelen = MAKE32_LEFT(p[0], p[1],p[2],p[3]);
                    p+=4;
                    break;
            }
            //第一个sample 里面揉和SEI + I帧。  无语
            unsigned char nalType = p[0]; 
            char type = nalType & ((1<<5)-1);
            msg_Dbg(out, "naltype:%d  slicelen:%d", type,slicelen);
            if(type == 5){//I帧，在前面加上 sps pps
                fwrite(nal,1,sizeof(nal),id->v_fp_out);
                fwrite(p_avcC->sps[0],1,p_avcC->i_sps_length[0],id->v_fp_out);
                fwrite(nal,1,sizeof(nal),id->v_fp_out);
                fwrite(p_avcC->pps[0],1,p_avcC->i_pps_length[0],id->v_fp_out);
            }
            fwrite(nal,1,sizeof(nal),id->v_fp_out);
            fwrite(p,1,slicelen,id->v_fp_out);

            p+=slicelen;
        }
    }else{
        msg_Dbg(out, "can unknow id->type:%d",id->type);
    }

    block_Release( p_block );
    return VLC_SUCCESS;
}

es_out_id_t * es_out_Add( es_out_t *out, const es_format_t *fmt ){
   msg_Dbg(p_demux, "can track es_out add");
   es_out_id_t   *es = malloc( sizeof( * es ) );
   es->type = fmt->i_cat;
   if(fmt->i_cat == VIDEO_ES){
        es->v_fp_out = fopen("es_out_video","w+");
        if(es->v_fp_out == NULL){
            msg_Dbg(out, "fopen err!!");
        }
        es->i_extra = fmt->i_extra;
        es->p_extra = malloc(fmt->i_extra);
        if(es->p_extra!=NULL){
            memcpy( es->p_extra,fmt->p_extra,fmt->i_extra);
        }
        es->i_bitrate = fmt->i_bitrate;
        es->i_level = fmt->i_level;
        es->i_profile = fmt->i_profile;

   }else if(fmt->i_cat == VIDEO_ES){
        es->a_fp_out = fopen("es_out_audio","w+");
        if(es->a_fp_out == NULL){
            msg_Dbg(out, "fopen err!!");
        }
   }
   return es;
}

void es_out_Del( es_out_t *out, es_out_id_t *id ){
    if(id!=NULL){
        if(id->p_extra!=NULL){
            free(id->p_extra);
            id->p_extra = NULL;
        }
        if(id->v_fp_out!=NULL){
            fclose(id->v_fp_out);
            id->v_fp_out = NULL;
        }
        free(id);
        id=NULL;
    }
}

