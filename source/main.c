#include <sys/thread.h>
#include <audio/audio.h>
#include <time.h>
#include <stdarg.h>
#include <sysutil/video.h>
#include <sys/systime.h>
#include <sysutil/sysutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io/pad.h>
#include <tiny3d.h>
#include <libfont.h>

extern unsigned char msx[];
#define PI 3.14159265f
u32 jet_off[7], shad_off, white_off, font_off, ui_bg_off, sky_off, sea_off, cab_dark_off, cab_org_off, scr_off, hud_grn_off, isl_off[6], cloud_off[3];
float tlx[48], tly[48], trx[48], try_arr[48];
int tidx = 0;
const char *jet[7][16] = {
 {".......RR.......","......RRRR......","......RRRR......",".....RRRRRR.....",".....BBBBBB.....","....BBBBBBBB....","...BBBBBBBBBB...","..BBBBBBBBBBBB..",".RBBBBBBBBBBBBR.","RRBBBBBBBBBBBBRR","..BBBBBBBBBBBB..","....BBBBBBBB....",".....BBBBBB.....",".....OO..OO.....","................","................"},
 {".......RR.......","......RRRR......","......RRRR......",".....DDDDDD.....",".....DDDDDD.....","....DDDDDDDD....","...DDDDDDDDDD...","..DDDDDDDDDDDD..",".RDDDDDDDDDDDDR.","RRDDDDDDDDDDDDRR","..DDDDDDDDDDDD..","....DDDDDDDD....",".....DDDDDD.....",".....YY..YY.....","................","................"},
 {"................",".......RR.......","......RRRR......",".....DDDDDD.....",".....CCCCCC.....","....CCCCCCCC....","...DDDDDDDDDD...","..DDDDDDDDDDDD..",".WDDDDDDDDDDDDW.","WDDDDDDDDDDDDDDW","..DDDDDDDDDDDD..","....DDDDDDDD....",".....DDDDDD.....","................","................","................"},
 {"................","................","................",".....CCCCCC.....","....CCCCCCCC....","....CCCCCCCC....",".....DDDDDD.....","....DDDDDDDD....","...DDDDDDDDDD...","....DDDDDDDD....",".....DDDDDD.....","....DDDDDDDD....","................","................","................","................"},
 {"................","................","................","................",".....CCCCCC.....",".....CCCCCC.....","......DDDD......","......DDDD......","......DDDD......","......DDDD......",".....DDDDDD.....",".....DDDDDD.....","................","................","................","................"},
 {"................","................","................","................","................","................","................",".......DD.......",".......DD.......",".......DD.......","......DDDD......","......DDDD......","................","................","................","................"},
 {"................","................","................","................","................","................","................","................","................",".......RR.......",".......RR.......",".......RR.......","................","................","................","................"}
};
u32 col_of(char c){
 if(c=='R')return 0xFFE63946;if(c=='B')return 0xFF3A404A;if(c=='D')return 0xFF23272E;
 if(c=='C')return 0xFF4CC9F0;if(c=='W')return 0xFFF1FAEE;if(c=='O')return 0xFFF77F00;
 if(c=='Y')return 0xFFFCBF49;return 0;
}
int log_init=0;
void wlog(const char *fmt, ...){
 if(!log_init){remove("/dev_hdd0/ps3_fighter.log");log_init=1;}
 FILE *f=fopen("/dev_hdd0/ps3_fighter.log","a");
 if(f){
  time_t now=time(NULL);struct tm *tm_i=localtime(&now);
  u64 s,ns;sysGetCurrentTime(&s,&ns);
  fprintf(f,"[%04d-%02d-%02d %02d:%02d:%02d.%03ld] ",tm_i->tm_year+1900,tm_i->tm_mon+1,tm_i->tm_mday,tm_i->tm_hour,tm_i->tm_min,tm_i->tm_sec,(long)(ns/1000000));
  va_list ap;va_start(ap,fmt);vfprintf(f,fmt,ap);va_end(ap);
  fprintf(f,"\n");fclose(f);
 }
}
void sys_cb(u64 st,u64 prm,void *usr){wlog("SYSUTIL_EVENT: 0x%llx",(unsigned long long)st);if(st==SYSUTIL_EXIT_GAME){exit(0);}}
void log_hw_info(){
 videoState vs;videoResolution vr;
 if(videoGetState(0,0,&vs)==0){
  videoGetResolution(vs.displayMode.resolution,&vr);
  wlog("VIDEO: state=%d scan=%d aspect=%d res=%dx%d (ID:%d)",vs.state,vs.displayMode.scanMode,vs.displayMode.aspect,vr.width,vr.height,vs.displayMode.resolution);
 }else wlog("VIDEO: videoGetState fallo");
 padInfo pi;ioPadGetInfo(&pi);
 wlog("PADS: max=%d connected=%d status0=%d",pi.max,pi.connected,pi.status[0]);
}
u32* al128(u32 *p){uintptr_t a=(uintptr_t)p;return (u32*)((a+127)&~((uintptr_t)127));}
void init_tex() {
 u32 *vram = tiny3d_AllocTexture(24*1024*1024);
 u32 *p = (u32 *)(((uintptr_t)vram + 255) & ~((uintptr_t)255));
 u32 *tmp = (u32 *)malloc(128*128*4);
 for(int s=0;s<7;s++){
  memset(tmp,0,32*32*4);
  for(int y=0;y<32;y++)for(int x=0;x<32;x++){
   int dx=abs(x-16); u32 c=0;
   if(s<=1){
    if(y>=1&&y<=28&&dx<=1+(y/7)) c=(y<5)?0xFF161A20:0xFF4A5568;
    if(y>=11&&y<=23&&dx<=(y-9)) c=(dx>=(y-10))?0xFFE63946:0xFF3A4454;
    if(y>=24&&y<=29&&dx>=2&&dx<=(y-18)) c=(dx>=(y-19))?0xFFE63946:0xFF2D3748;
    if(y>=29&&y<=31&&(dx==2||dx==3)) c=(s==0)?0xFFF77F00:0xFFFCBF49;
   }else if(s<=3){
    if(y>=4&&y<=27&&dx<=2) c=0xFF5A677D;
    if(y>=13&&y<=22&&dx<=(y-11)/2) c=0xFF4A5568;
    if(y>=7&&y<=16&&dx<=1&&s==3) c=0xFF4CC9F0;
   }else{
    if(y>=8&&y<=16&&dx<=1&&s==4) c=0xFF90E0EF;
    if(y>=17&&y<=26&&dx==1) c=0xFF3A4454;
    if(y>=22&&y<=29&&dx==(3+(s-4))) c=0xFFE63946;
   }
   tmp[y*32+x]=c;
  }
  RSX_MEMCPY(p,tmp,32*32*4);jet_off[s]=tiny3d_TextureOffset(p);p=al128(p+1024);
 }
 memset(tmp,0,32*32*4);
 for(int y=0;y<32;y++)for(int x=0;x<32;x++){
  int dx=abs(x-16);
  if((y>=1&&y<=28&&dx<=1+(y/7))||(y>=11&&y<=23&&dx<=(y-9))||(y>=24&&y<=29&&dx>=2&&dx<=(y-18))) tmp[y*32+x]=0x66081426;
 }
 RSX_MEMCPY(p,tmp,32*32*4);shad_off=tiny3d_TextureOffset(p);p=al128(p+1024);
 for(int k=0;k<256;k++)tmp[k]=0xFFFFFFFF;
 RSX_MEMCPY(p, tmp, 16*16*4); white_off=tiny3d_TextureOffset(p); p=al128(p+256);
 for(int s=0;s<6;s++){
  for(int y=0;y<128;y++)for(int x=0;x<128;x++){
   float dx=(x-64)/64.f,dy=(y-64)/64.f,d=sqrtf(dx*dx+dy*dy)+sinf(x*.09f)*cosf(y*.09f)*.24f+sinf((x-y)*.06f)*.14f;u32 c=0;
   if(s==0&&d<.78f)c=(d>.72f)?0xAAFFFFFF:0xFF48CAE4;else if(s==1&&d<.71f)c=0xFFE9C46A;else if(s==2&&d<.58f)c=(d>.54f)?0xFF52B788:0xFF74C69D;
   else if(s==3&&d<.44f)c=(d>.40f)?0xFF40916C:0xFF52B788;else if(s==4&&d<.30f)c=((x+y)%13==0)?0xFF8D99AE:0xFF40916C;else if(s==5&&d<.36f&&((x*7+y*13)%9==0))c=0xFF1B4332;
   tmp[y*128+x]=c;
  }
  RSX_MEMCPY(p, tmp, 128*128*4); isl_off[s]=tiny3d_TextureOffset(p); p=al128(p+16384);
 }
 for(int s=0;s<3;s++){
  for(int y=0;y<32;y++)for(int x=0;x<32;x++){float dx=(x-16)/16.f,dy=(y-16)/16.f;tmp[y*32+x]=(sqrtf(dx*dx+dy*dy)+sinf(x*.4f)*cosf(y*.4f)*.15f<.75f-s*.18f)?(s==0?0xFFD8E2DC:0xFFFFFFFF):0;}
  RSX_MEMCPY(p, tmp, 32*32*4); cloud_off[s]=tiny3d_TextureOffset(p); p=al128(p+1024);
 }
  for(int k=0;k<256;k++)tmp[k]=0xAA0A101A;
 RSX_MEMCPY(p, tmp, 16*16*4); ui_bg_off=tiny3d_TextureOffset(p); p=al128(p+256);
  for(int y=0;y<16;y++){u32 c=(y<8)?0xFF2B6CB0:0xFF7CC4FA;for(int x=0;x<16;x++)tmp[y*16+x]=c;}
 RSX_MEMCPY(p, tmp, 16*16*4); sky_off=tiny3d_TextureOffset(p); p=al128(p+256);
 for(int y=0;y<16;y++){u32 c=(y<8)?0xFF1A5699:0xFF0E3463;for(int x=0;x<16;x++)tmp[y*16+x]=c;}
 RSX_MEMCPY(p, tmp, 16*16*4); sea_off=tiny3d_TextureOffset(p); p=al128(p+256);
 for(int k=0;k<256;k++)tmp[k]=0xFF1B222C;
 RSX_MEMCPY(p, tmp, 16*16*4); cab_dark_off=tiny3d_TextureOffset(p); p=al128(p+256);
 for(int k=0;k<256;k++)tmp[k]=0xFFD96B27;
 RSX_MEMCPY(p, tmp, 16*16*4); cab_org_off=tiny3d_TextureOffset(p); p=al128(p+256);
 for(int k=0;k<256;k++)tmp[k]=0xFF0A2618;
 RSX_MEMCPY(p, tmp, 16*16*4); scr_off=tiny3d_TextureOffset(p); p=al128(p+256);
 for(int k=0;k<256;k++)tmp[k]=0xFF38D975;
 RSX_MEMCPY(p, tmp, 16*16*4); hud_grn_off=tiny3d_TextureOffset(p); p=al128(p+256);
 memset(tmp, 0, 128*128*4);
 for(int ch=32;ch<128;ch++){
  int gx=(ch%16)*8, gy=((ch-32)/16)*8;
  for(int r=0;r<8;r++){unsigned char b=msx[ch*8+r];
   for(int c=0;c<8;c++) if(b&(0x80>>c)) tmp[(gy+r)*128+(gx+c)]=0xFFFFFFFF;
  }
 }
 RSX_MEMCPY(p, tmp, 128*128*4); font_off=tiny3d_TextureOffset(p); p=al128(p+16384);
 free(tmp);
 wlog("VRAM base:%p | Offsets -> jet0:0x%x isl0:0x%x white:0x%x", vram, jet_off[0], isl_off[0], white_off);
}
void quad2d(float x,float y,float z,float w,float h,float a,float sx,float sy){
 float hw=w*.5f*sx,hh=h*.5f*sy,c=cosf(a),s=sinf(a);
 float x0=-hw*c+hh*s,y0=-hw*s-hh*c,x1=hw*c+hh*s,y1=hw*s-hh*c;
 float x2=hw*c-hh*s,y2=hw*s+hh*c,x3=-hw*c-hh*s,y3=-hw*s+hh*c;
 tiny3d_SetPolygon(TINY3D_QUADS);
 tiny3d_VertexPos(x+x0,y+y0,z);tiny3d_VertexTexture(0.0f,0.0f);
 tiny3d_VertexPos(x+x1,y+y1,z);tiny3d_VertexTexture(0.99f,0.0f);
 tiny3d_VertexPos(x+x2,y+y2,z);tiny3d_VertexTexture(0.99f,0.99f);
 tiny3d_VertexPos(x+x3,y+y3,z);tiny3d_VertexTexture(0.0f,0.99f);
 tiny3d_End();
}
void rect2d(float x,float y,float w,float h,float z,u32 c1,u32 c2){
 tiny3d_SetTexture(0,white_off,16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 tiny3d_SetPolygon(TINY3D_QUADS);
 tiny3d_VertexPos(x,y,z);tiny3d_VertexTexture(0,0);
 tiny3d_VertexPos(x+w,y,z);tiny3d_VertexTexture(1,0);
 tiny3d_VertexPos(x+w,y+h,z);tiny3d_VertexTexture(1,1);
 tiny3d_VertexPos(x,y+h,z);tiny3d_VertexTexture(0,1);
 tiny3d_End();
 return;
 tiny3d_SetTexture(0,white_off,16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 tiny3d_SetPolygon(TINY3D_QUADS);tiny3d_VertexPos(x,y,z);tiny3d_VertexColor(c1);tiny3d_VertexTexture(0,0);tiny3d_VertexPos(x+w,y,z);tiny3d_VertexColor(c1);tiny3d_VertexTexture(1,0);
 tiny3d_VertexPos(x+w,y+h,z);tiny3d_VertexColor(c2);tiny3d_VertexTexture(1,1);tiny3d_VertexPos(x,y+h,z);tiny3d_VertexColor(c2);tiny3d_VertexTexture(0,1);tiny3d_End();
}
void line2d(float x1,float y1,float x2,float y2,float z,u32 c){
 tiny3d_SetTexture(0,white_off,16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 float dx=x2-x1,dy=y2-y1,l=sqrtf(dx*dx+dy*dy);if(l>0.1f){float nx=-dy/l*1.5f,ny=dx/l*1.5f;tiny3d_SetPolygon(TINY3D_QUADS);tiny3d_VertexPos(x1+nx,y1+ny,z);tiny3d_VertexTexture(0,0);tiny3d_VertexPos(x2+nx,y2+ny,z);tiny3d_VertexTexture(1,0);tiny3d_VertexPos(x2-nx,y2-ny,z);tiny3d_VertexTexture(1,1);tiny3d_VertexPos(x1-nx,y1-ny,z);tiny3d_VertexTexture(0,1);tiny3d_End();}
}
void quad_persp(float x0,float z0,float x1,float z1,float x2,float z2,float x3,float z3,float yw,float hor,float zbuf){
 float s0=240.f/z0,s1=240.f/z1,s2=240.f/z2,s3=240.f/z3;
 tiny3d_SetPolygon(TINY3D_QUADS);
 tiny3d_VertexPos(424+x0*s0,hor+yw*s0,zbuf);tiny3d_VertexTexture(0,0);
 tiny3d_VertexPos(424+x1*s1,hor+yw*s1,zbuf);tiny3d_VertexTexture(.99f,0);
 tiny3d_VertexPos(424+x2*s2,hor+yw*s2,zbuf);tiny3d_VertexTexture(.99f,.99f);
 tiny3d_VertexPos(424+x3*s3,hor+yw*s3,zbuf);tiny3d_VertexTexture(0,.99f);
 tiny3d_End();
}
void tex_box(u32 tex,float x,float y,float w,float h,float z){
 tiny3d_SetTexture(0,tex,16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 tiny3d_SetPolygon(TINY3D_QUADS);
 tiny3d_VertexPos(x,y,z);tiny3d_VertexTexture(0,0);tiny3d_VertexPos(x+w,y,z);tiny3d_VertexTexture(1,0);
 tiny3d_VertexPos(x+w,y+h,z);tiny3d_VertexTexture(1,1);tiny3d_VertexPos(x,y+h,z);tiny3d_VertexTexture(0,1);
 tiny3d_End();
}
void draw_world_3d(float px,float py,float ang,float alt,float cp,float cs){
 float ca=-ang-PI*.5f,hor=230+cp*40,co=cosf(ca),si=sinf(ca);
 tex_box(sky_off,0,0,848,hor,65535);
 tex_box(sea_off,0,hor,848,512-hor,65535);
 for(int ix=-2;ix<=2;ix++)for(int iy=-2;iy<=2;iy++){
  float wx=(floorf(px/520)+ix)*520-px,wy=(floorf(py/520)+iy)*520-py;
  float cx=wx*co-wy*si,cz=-(wx*si+wy*co);
  if(cz>140&&cz<950){float h=170.f;
   float x0=cx-h*co+h*si,z0=cz-h*si-h*co,x1=cx+h*co+h*si,z1=cz+h*si-h*co;
   float x2=cx+h*co-h*si,z2=cz+h*si+h*co,x3=cx-h*co-h*si,z3=cz-h*si+h*co;
   if(z0>45&&z1>45&&z2>45&&z3>45){
    for(int s=0;s<6;s++){tiny3d_SetTexture(0,isl_off[s],128,128,512,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad_persp(x0,z0,x1,z1,x2,z2,x3,z3,alt*1.5f-s*3.5f,hor,500-cz-s);}
    for(int c=0;c<3;c++){tiny3d_SetTexture(0,cloud_off[c],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad_persp(x0+70,z0,x1+70,z1,x2+70,z2,x3+70,z3,alt*1.5f-45-c*3.0f,hor,480-cz-c);}
   }}}
}
void draw_text(float x, float y, float z, float sc, const char *s){
 tiny3d_SetTexture(0,font_off,128,128,512,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 float cw=8.0f*sc, ch=8.0f*sc;
 for(int i=0;s[i];i++){
  unsigned char c=(unsigned char)s[i]; if(c<32||c>127)continue;
  float u0=((c%16)*8)/128.0f, v0=(((c-32)/16)*8)/128.0f, u1=u0+7.8f/128.0f, v1=v0+7.8f/128.0f;
  float px=x+i*(cw+sc);
  tiny3d_SetPolygon(TINY3D_QUADS);
  tiny3d_VertexPos(px,y,z);tiny3d_VertexTexture(u0,v0);
  tiny3d_VertexPos(px+cw,y,z);tiny3d_VertexTexture(u1,v0);
  tiny3d_VertexPos(px+cw,y+ch,z);tiny3d_VertexTexture(u1,v1);
  tiny3d_VertexPos(px,y+ch,z);tiny3d_VertexTexture(u0,v1);
  tiny3d_End();
 }
}
void ui_box(float x,float y,float w,float h,float z){
 tiny3d_SetTexture(0,ui_bg_off,16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 tiny3d_SetPolygon(TINY3D_QUADS);
 tiny3d_VertexPos(x,y,z);tiny3d_VertexTexture(0,0);tiny3d_VertexPos(x+w,y,z);tiny3d_VertexTexture(1,0);
 tiny3d_VertexPos(x+w,y+h,z);tiny3d_VertexTexture(1,1);tiny3d_VertexPos(x,y+h,z);tiny3d_VertexTexture(0,1);
 tiny3d_End();
}
u32 snd_port=0;audioPortConfig snd_cfg;int snd_ok=0;u64 snd_key;sys_event_queue_t snd_q;
float ph_l=0,ph_b1=0,ph_b2=0,ph_b3=0,ph_p1=0,ph_p2=0,ph_p3=0,ph_arp=0,ph_kick=0,ph_snare=0,ph_hat=0,ph_roll=0;
float env_l=0,env_b=0,lp_b=0,lp_l=0,lp_r=0,sin_tab[256],dly_l[9600]={0},dly_r[9600]={0};int dly_p=0;
u32 n_seed=987654321,m_seed=135792468;int last_step=-1,snd_fc=0,prog_idx=0;
float cur_fl=0,tgt_fl=0,cur_fb=55.f,cur_fa=0,cur_fp1=220.f,cur_fp2=261.63f,cur_fp3=329.63f;
volatile float v_ct=1.f,v_cs=0.f,v_roll=0.f;volatile int v_demo=1;
const float sw_scale[10]={220.00f,246.94f,261.63f,293.66f,329.63f,349.23f,392.00f,440.00f,493.88f,523.25f};
const float sw_roots[16]={110.00f,87.31f,130.81f,98.00f, 146.83f,110.00f,87.31f,82.41f, 98.00f,110.00f,87.31f,130.81f, 130.81f,98.00f,110.00f,87.31f};
const float sw_thirds[16]={130.81f,110.00f,164.81f,123.47f, 174.61f,130.81f,110.00f,103.83f, 123.47f,130.81f,110.00f,164.81f, 164.81f,123.47f,130.81f,110.00f};
const float sw_fifths[16]={164.81f,130.81f,196.00f,146.83f, 220.00f,164.81f,130.81f,123.47f, 146.83f,164.81f,130.81f,196.00f, 196.00f,146.83f,164.81f,130.81f};
u32 next_rng(){m_seed=m_seed*1664525u+1013904223u;return m_seed;}
float saw(float p){return 2.f*p-1.f;}
void audio_thread(void *arg){
 sys_event_t ev;
 while(snd_ok){
  if(sysEventQueueReceive(snd_q,&ev,20000)!=0)continue;
  u64 cur=*(u64*)((u64)snd_cfg.readIndex);u32 nxt=(cur+1)%snd_cfg.numBlocks;
  f32 *buf=(f32*)((u64)snd_cfg.audioDataStart)+snd_cfg.channelCount*AUDIO_BLOCK_SAMPLES*nxt;
  snd_fc++;int step=snd_fc/32;
  if(step!=last_step){
   last_step=step;u32 r=next_rng();int bar=(step/16)%8;if((step%16)==0)prog_idx=(r>>14)&3;
   int ci=(prog_idx*4)+(bar%4);cur_fb=sw_roots[ci]*((step&1)?1.f:.5f);env_b=1.f;
   cur_fp1=sw_roots[ci]*2.f;cur_fp2=sw_thirds[ci]*2.f;cur_fp3=sw_fifths[ci]*2.f;
   cur_fa=(step%4==0)?cur_fp1:((step%4==2)?cur_fp3:cur_fp2);
   if((step%2)==0){if(((r>>4)&7)<5){tgt_fl=sw_scale[(r&7)+((r>>10)&1)*2]*(((r>>18)&3)==0?2.f:(((r>>18)&3)==1?0.5f:1.f));if(cur_fl<1.f)cur_fl=tgt_fl;env_l=1.f;}else if(((r>>8)&3)==0)tgt_fl=0.f;}
   if((step%4)==0)ph_kick=1.f;if((step%8)==4||((step%32)>=28&&(r&1)))ph_snare=1.f;if((step%2)==1)ph_hat=1.f;
  }
  float ct=v_ct,cs=v_cs,roll=v_roll;int demo=v_demo;
  for(int i=0;i<AUDIO_BLOCK_SAMPLES;i++){
   float lead=0;if(tgt_fl>1.f){cur_fl+=(tgt_fl-cur_fl)*.0018f;float vib=1.f+.0035f*sin_tab[(snd_fc*5+i/20)&255];ph_l+=(cur_fl*vib)/48000.f;if(ph_l>=1.f)ph_l-=1.f;float p2=ph_l*1.004f;if(p2>=1.f)p2-=1.f;lead=(saw(ph_l)*.55f+saw(p2)*.45f)*.12f*env_l;env_l*=.99991f;}
   ph_b1+=cur_fb/48000.f;if(ph_b1>=1.f)ph_b1-=1.f;ph_b2+=(cur_fb*1.005f)/48000.f;if(ph_b2>=1.f)ph_b2-=1.f;ph_b3+=(cur_fb*.995f)/48000.f;if(ph_b3>=1.f)ph_b3-=1.f;
   lp_b+=((saw(ph_b1)+saw(ph_b2)+saw(ph_b3))*.33f-lp_b)*(.06f+.24f*env_b);env_b*=.9994f;float bass=lp_b*.36f;
   ph_p1+=cur_fp1/48000.f;if(ph_p1>=1.f)ph_p1-=1.f;ph_p2+=(cur_fp2*1.002f)/48000.f;if(ph_p2>=1.f)ph_p2-=1.f;ph_p3+=(cur_fp3*.998f)/48000.f;if(ph_p3>=1.f)ph_p3-=1.f;
   float pad=(sin_tab[(int)(ph_p1*256.f)&255]+sin_tab[(int)(ph_p2*256.f)&255]+sin_tab[(int)(ph_p3*256.f)&255])*.048f;
   ph_arp+=cur_fa/48000.f;if(ph_arp>=1.f)ph_arp-=1.f;float arp=(saw(ph_arp)*.4f+sin_tab[(int)(ph_arp*256.f)&255]*.6f)*.042f;
   n_seed=n_seed*1664525u+1013904223u;float nz=((float)(n_seed>>16)/32768.f)-1.f;
   float kick=(ph_kick>.01f)?sin_tab[(int)(ph_kick*1250.f)&255]*ph_kick*.36f:0.f;ph_kick*=.9987f;
   float snare=(ph_snare>.01f)?(nz*.78f+sin_tab[(int)(ph_snare*720.f)&255]*.22f)*ph_snare*.21f:0.f;ph_snare*=.9989f;
   float hat=(ph_hat>.01f)?nz*ph_hat*.042f:0.f;ph_hat*=.992f;
   float eng=nz*(.02f+(ct-1.f)*.055f+fabsf(cs)*.022f),wh=0;if(roll>.05f){ph_roll+=(340.f+sinf(roll)*160.f)/48000.f;if(ph_roll>=1.f)ph_roll-=1.f;wh=sin_tab[(int)(ph_roll*256.f)&255]*.1f;}
   float mid=bass+kick+snare+pad+(demo?0.f:(eng+wh)),sl=mid+lead*.85f+arp*.35f+hat*.65f,sr=mid+lead*.35f+arp*.85f+hat*.95f;
   int dr=(dly_p+3600)%9600;float ol=sl+dly_r[dly_p]*.32f,or_=sr+dly_l[dr]*.32f;dly_l[dly_p]=sl;dly_r[dly_p]=sr;dly_p=(dly_p+1)%9600;
   lp_l+=(ol-lp_l)*.48f;lp_r+=(or_-lp_r)*.48f;buf[i*2]=lp_l>.92f?.92f:(lp_l<-.92f?-.92f:lp_l);buf[i*2+1]=lp_r>.92f?.92f:(lp_r<-.92f?-.92f:lp_r);
  }
 }
}
void init_snd(){for(int k=0;k<256;k++)sin_tab[k]=sinf((k/256.f)*6.2831853f);audioPortParam pr={AUDIO_PORT_2CH,AUDIO_BLOCK_8,AUDIO_PORT_INITLEVEL,0.95f};if(audioInit()==0&&audioPortOpen(&pr,&snd_port)==0&&audioGetPortConfig(snd_port,&snd_cfg)==0&&audioCreateNotifyEventQueue(&snd_q,&snd_key)==0&&audioSetNotifyEventQueue(snd_key)==0){u64 ts,tns;sysGetCurrentTime(&ts,&tns);m_seed^=(u32)(ts^tns);sysEventQueueDrain(snd_q);audioPortStart(snd_port);snd_ok=1;sys_ppu_thread_t tid;sysThreadCreate(&tid,audio_thread,NULL,1000,0x4000,THREAD_JOINABLE,"snd_sw");wlog("AUDIO: Synthwave OK");}}
void update_snd(int fc,float ct,float cs,float roll,int demo){v_ct=ct;v_cs=cs;v_roll=roll;v_demo=demo;}
int main(int argc, const char* argv[]){
 padInfo pi;padData pd;wlog("1. Inicio main (TS:%llu)",(unsigned long long)time(NULL));sysUtilRegisterCallback(0,sys_cb,NULL);int r_t3d=tiny3d_Init(4*1024*1024);wlog("tiny3d_Init ret: 0x%x", r_t3d);wlog("2. Tiny3D Init OK");log_hw_info();for(int k=0;k<3;k++){tiny3d_Clear(0xFF2468B4,TINY3D_CLEAR_ALL);tiny3d_Flip();}wlog("3. HDMI Flip OK");int r_pad=ioPadInit(7);init_snd();wlog("ioPadInit ret: 0x%x", r_pad);init_tex();wlog("4. Texturas OK");
 float px=0,py=0,ang=-PI*.5f,alt=38,roll=0,cs=0,cp=0,ct=1;int demo=1,cam=0,bt=0,bs=0,bst=0,bsel=0,fc=0;
 wlog("5. Entrando a bucle principal");while(1){sysUtilCheckCallback();
  ioPadGetInfo(&pi);float ts=0,tp=0,tt=1;fc++;
  if(demo){ts=sinf(fc*.015f)*.65f;tp=cosf(fc*.02f)*.35f;if(fc%240==0&&roll<=.01f)roll=2*PI;}
  for(int i=0;i<MAX_PADS;i++)if(pi.status[i]){ioPadGetData(i,&pd);
   if(pd.BTN_START&&!bst){if(demo){demo=0;cam=0;}else return 0;}
   if(pd.BTN_SELECT&&!bsel&&!demo){demo=1;cam=0;}
   if(!demo){
    if(pd.ANA_L_H>0||pd.ANA_L_V>0){float lx=(pd.ANA_L_H-128)/128.f,ly=(pd.ANA_L_V-128)/128.f;if(fabsf(lx)>.22f)ts=lx;if(fabsf(ly)>.22f)tp=-ly;}
    if(pd.BTN_LEFT)ts=-1;if(pd.BTN_RIGHT)ts=1;if(pd.BTN_UP)tp=1;if(pd.BTN_DOWN)tp=-1;
    if(pd.BTN_CROSS||pd.BTN_R2)tt=2.3f;if(pd.BTN_SQUARE&&!bs&&roll<=.01f)roll=2*PI;if(pd.BTN_TRIANGLE&&!bt){cam=(cam+1)%3;wlog("CAMBIO DE VISTA A: %d", cam);}
   }bst=pd.BTN_START;bsel=pd.BTN_SELECT;bs=pd.BTN_SQUARE;bt=pd.BTN_TRIANGLE;}
  cs+=(ts-cs)*.14f;cp+=(tp-cp)*.14f;ct+=(tt-ct)*.1f;ang+=cs*.042f;alt+=cp*1.1f;
  if(alt<12)alt=12;if(alt>95)alt=95;if(roll>0){roll-=.18f;if(roll<0)roll=0;}
  px+=cosf(ang)*2.6f*ct;py+=sinf(ang)*2.6f*ct;
  if(fc%2==0){float wa=ang+PI*.5f,sp=18*cosf(roll)*(1-fabsf(cs)*.2f);tlx[tidx]=px+cosf(wa)*sp-cosf(ang)*10;tly[tidx]=py+sinf(wa)*sp-sinf(ang)*10;trx[tidx]=px-cosf(wa)*sp-cosf(ang)*10;try_arr[tidx]=py-sinf(wa)*sp-sinf(ang)*10;tidx=(tidx+1)%48;}
  if(fc==1)wlog("6A. Clear inicio");tiny3d_Clear(0x2468B4FF,TINY3D_CLEAR_ALL);tiny3d_AlphaTest(1,0x10,TINY3D_ALPHA_FUNC_GEQUAL);
  tiny3d_BlendFunc(1,TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA|TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA|TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,TINY3D_BLEND_RGB_FUNC_ADD|TINY3D_BLEND_ALPHA_FUNC_ADD);
  tiny3d_Project2D();
  if(cam==0){if(fc==1)wlog("6A1. Islas");if(fc==1)wlog("6A1. Dibujando islas");
   for(int ix=-1;ix<=1;ix++)for(int iy=-1;iy<=1;iy++){
    float wx=(floorf(px/520)+ix)*520-px,wy=(floorf(py/520)+iy)*520-py;
    for(int s=0;s<6;s++){tiny3d_SetTexture(0,isl_off[s],128,128,512,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+wx,256+wy-s*2.2f,100-s,340,340,0,1,1);if(fc==1&&ix==-1&&iy==-1)wlog("6A1b. Capa isla %d OK", s);}
    for(int c=0;c<3;c++){tiny3d_SetTexture(0,cloud_off[c],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+wx+90,256+wy-55-c*2.2f,15-c,72,72,0,1,1);}}
   if(fc==1)wlog("6A2. Sombra");float ja=ang+PI*.5f,bk=cosf(roll)*(1-fabsf(cs)*.25f);
   tiny3d_SetTexture(0,shad_off,32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+alt*.35f,256+alt*.35f,30,44,44,ja,bk,1);
   if(fc==1)wlog("6A3. Estelas");for(int k=1;k<48;k++){int i1=(tidx+k-1)%48,i2=(tidx+k)%48;
    float x1=tlx[i1]-px,y1=tly[i1]-py,x2=tlx[i2]-px,y2=tly[i2]-py;if(fabsf(x1-x2)<40&&fabsf(y1-y2)<40)line2d(424+x1,256+y1-alt*.4f,424+x2,256+y2-alt*.4f,22,0xAAFFFFFF);
    x1=trx[i1]-px;y1=try_arr[i1]-py;x2=trx[i2]-px;y2=try_arr[i2]-py;if(fabsf(x1-x2)<40&&fabsf(y1-y2)<40)line2d(424+x1,256+y1-alt*.4f,424+x2,256+y2-alt*.4f,22,0xAAFFFFFF);}
   if(fc==1)wlog("6A4. Avion");for(int s=0;s<7;s++){tiny3d_SetTexture(0,jet_off[s],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424,256-alt*.4f-s*1.9f,20-s,48,48,ja,bk,1);}
  }else if(cam==1){
   draw_world_3d(px,py,ang,alt,cp,cs);
   float jy=365-cp*25,jr=roll+cs*.45f,sp=48*cosf(jr);
   line2d(424-sp,jy,330-sp*1.5f,512,18,0xDDFFFFFF);line2d(424+sp,jy,518+sp*1.5f,512,18,0xDDFFFFFF);
   if(fc==1)wlog("6A4. Avion");for(int s=0;s<7;s++){tiny3d_SetTexture(0,jet_off[s],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424,jy-s*3.2f,15-s,96,34,jr,1,1);}
   rect2d(412,jy+6,8,14+ct*6,12,0xFFAA00FF,0xFF4500FF);rect2d(428,jy+6,8,14+ct*6,12,0xFFAA00FF,0xFF4500FF);
  }else{
   draw_world_3d(px,py,ang,alt,cp,cs);
   /* Mira HUD central y lineas de horizonte artificial */
   tex_box(hud_grn_off,335,230+cs*30,178,3,13);tex_box(hud_grn_off,423,205,3,52,13);
   tex_box(hud_grn_off,355,190,14,3,13);tex_box(hud_grn_off,479,190,14,3,13);
   /* Pilares laterales del parabrisas */
   tex_box(cab_dark_off,0,0,54,512,12);tex_box(cab_dark_off,794,0,54,512,12);
   tex_box(cab_org_off,50,0,12,512,11);tex_box(cab_org_off,786,0,12,512,11);
   /* Tablero principal y visera naranja */
   tex_box(cab_dark_off,0,356,848,156,10);tex_box(cab_org_off,0,350,848,8,9);
   /* Pantalla izquierda (Radar) y derecha (Sensores motor/combustible) */
   tex_box(scr_off,74,376,195,112,8);tex_box(scr_off,579,376,195,112,8);
   tex_box(hud_grn_off,168,386,3,92,7);tex_box(hud_grn_off,88,431,168,3,7);
   tex_box(hud_grn_off,145+sinf(fc*.05f)*35,412+cosf(fc*.05f)*20,8,8,6);
   /* Barras de medidores en pantalla derecha (Altitud, Velocidad, Combustible) */
   tex_box(hud_grn_off,600,475-alt*.8f,24,alt*.8f,7);
   tex_box(hud_grn_off,645,475-ct*32.f,24,ct*32.f,7);
   tex_box(hud_grn_off,690,405,24,70,7);
   /* Horizonte artificial central */
   tex_box(sky_off,370,374,108,52,8);tex_box(cab_org_off,370,426+cp*18,108,52-cp*18,8);
   tex_box(white_off,370,425+cp*18,108,3,7);
   /* Palanca de mando (Joystick) central dinamica */
   float jx=414+cs*26, jy=446-cp*14;
   tex_box(cab_dark_off,jx,jy,20,66,6);tex_box(cab_org_off,jx-3,jy-8,26,18,5);
  }
    if(demo){ts=sinf(fc*.015f)*.65f;tp=cosf(fc*.02f)*.35f;if(fc%240==0&&roll<=.01f)roll=2*PI;}
  for(int i=0;i<MAX_PADS;i++)if(pi.status[i]){ioPadGetData(i,&pd);
   if(pd.BTN_START&&!bst){if(demo){demo=0;cam=0;}else return 0;}
   if(pd.BTN_SELECT&&!bsel&&!demo){demo=1;cam=0;}
   if(!demo){
    if(pd.ANA_L_H>0||pd.ANA_L_V>0){float lx=(pd.ANA_L_H-128)/128.f,ly=(pd.ANA_L_V-128)/128.f;if(fabsf(lx)>.22f)ts=lx;if(fabsf(ly)>.22f)tp=-ly;}
    if(pd.BTN_LEFT)ts=-1;if(pd.BTN_RIGHT)ts=1;if(pd.BTN_UP)tp=1;if(pd.BTN_DOWN)tp=-1;
    if(pd.BTN_CROSS||pd.BTN_R2)tt=2.3f;if(pd.BTN_SQUARE&&!bs&&roll<=.01f)roll=2*PI;if(pd.BTN_TRIANGLE&&!bt){cam=(cam+1)%3;wlog("CAMBIO DE VISTA A: %d", cam);}
   }bst=pd.BTN_START;bsel=pd.BTN_SELECT;bs=pd.BTN_SQUARE;bt=pd.BTN_TRIANGLE;}
  cs+=(ts-cs)*.14f;cp+=(tp-cp)*.14f;ct+=(tt-ct)*.1f;ang+=cs*.042f;alt+=cp*1.1f;
  if(alt<12)alt=12;if(alt>95)alt=95;if(roll>0){roll-=.18f;if(roll<0)roll=0;}
  px+=cosf(ang)*2.6f*ct;py+=sinf(ang)*2.6f*ct;
  if(fc%2==0){float wa=ang+PI*.5f,sp=18*cosf(roll)*(1-fabsf(cs)*.2f);tlx[tidx]=px+cosf(wa)*sp-cosf(ang)*10;tly[tidx]=py+sinf(wa)*sp-sinf(ang)*10;trx[tidx]=px-cosf(wa)*sp-cosf(ang)*10;try_arr[tidx]=py-sinf(wa)*sp-sinf(ang)*10;tidx=(tidx+1)%48;}
  if(fc==1)wlog("6A. Clear inicio");tiny3d_Clear(0x2468B4FF,TINY3D_CLEAR_ALL);tiny3d_AlphaTest(1,0x10,TINY3D_ALPHA_FUNC_GEQUAL);
  tiny3d_BlendFunc(1,TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA|TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA|TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,TINY3D_BLEND_RGB_FUNC_ADD|TINY3D_BLEND_ALPHA_FUNC_ADD);
  tiny3d_Project2D();
  if(cam==0){if(fc==1)wlog("6A1. Islas");if(fc==1)wlog("6A1. Dibujando islas");
   for(int ix=-1;ix<=1;ix++)for(int iy=-1;iy<=1;iy++){
    float wx=(floorf(px/520)+ix)*520-px,wy=(floorf(py/520)+iy)*520-py;
    for(int s=0;s<6;s++){tiny3d_SetTexture(0,isl_off[s],128,128,512,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+wx,256+wy-s*2.2f,100-s,340,340,0,1,1);if(fc==1&&ix==-1&&iy==-1)wlog("6A1b. Capa isla %d OK", s);}
    for(int c=0;c<3;c++){tiny3d_SetTexture(0,cloud_off[c],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+wx+90,256+wy-55-c*2.2f,15-c,72,72,0,1,1);}}
   if(fc==1)wlog("6A2. Sombra");float ja=ang+PI*.5f,bk=cosf(roll)*(1-fabsf(cs)*.25f);
   tiny3d_SetTexture(0,shad_off,32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+alt*.35f,256+alt*.35f,30,44,44,ja,bk,1);
   if(fc==1)wlog("6A3. Estelas");for(int k=1;k<48;k++){int i1=(tidx+k-1)%48,i2=(tidx+k)%48;
    float x1=tlx[i1]-px,y1=tly[i1]-py,x2=tlx[i2]-px,y2=tly[i2]-py;if(fabsf(x1-x2)<40&&fabsf(y1-y2)<40)line2d(424+x1,256+y1-alt*.4f,424+x2,256+y2-alt*.4f,22,0xAAFFFFFF);
    x1=trx[i1]-px;y1=try_arr[i1]-py;x2=trx[i2]-px;y2=try_arr[i2]-py;if(fabsf(x1-x2)<40&&fabsf(y1-y2)<40)line2d(424+x1,256+y1-alt*.4f,424+x2,256+y2-alt*.4f,22,0xAAFFFFFF);}
   if(fc==1)wlog("6A4. Avion");for(int s=0;s<7;s++){tiny3d_SetTexture(0,jet_off[s],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424,256-alt*.4f-s*1.9f,20-s,48,48,ja,bk,1);}
  }else if(cam==1){
   draw_world_3d(px,py,ang,alt,cp,cs);
   float jy=365-cp*25,jr=roll+cs*.45f,sp=48*cosf(jr);
   line2d(424-sp,jy,330-sp*1.5f,512,18,0xDDFFFFFF);line2d(424+sp,jy,518+sp*1.5f,512,18,0xDDFFFFFF);
   if(fc==1)wlog("6A4. Avion");for(int s=0;s<7;s++){tiny3d_SetTexture(0,jet_off[s],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424,jy-s*3.2f,15-s,96,34,jr,1,1);}
   rect2d(412,jy+6,8,14+ct*6,12,0xFFAA00FF,0xFF4500FF);rect2d(428,jy+6,8,14+ct*6,12,0xFFAA00FF,0xFF4500FF);
  }else{
   draw_world_3d(px,py,ang,alt,cp,cs);
   rect2d(310,155,228,155,14,0x88D8FF22,0x88D8FF22);
   line2d(345,232+cs*35,503,232-cs*35,13,0x48BB78FF);line2d(424,215,424,250,13,0x48BB78FF);
   rect2d(0,0,58,512,12,0x1A202CFF,0x2D3748FF);rect2d(790,0,58,512,12,0x1A202CFF,0x2D3748FF);
   rect2d(52,0,14,512,11,0xDD6B20FF,0xED8936FF);rect2d(782,0,14,512,11,0xDD6B20FF,0xED8936FF);
   rect2d(0,365,848,147,10,0x1C2833FF,0x2E4053FF);rect2d(0,360,848,8,9,0xDD6B20FF,0xED8936FF);
   rect2d(72,384,185,108,8,0x092416FF,0x123D26FF);rect2d(591,384,185,108,8,0x092416FF,0x123D26FF);
   rect2d(374,382,100,100,8,0x2B6CB0FF,0x9C4221FF);line2d(374,432+cp*20,474,432+cp*20,7,0xFFFFFFFF);
   rect2d(416,445,16,67,6,0x11161DFF,0x232B38FF);
     }
  if(fc==1)wlog("6B. Geometria dibujada, llamando Flip directo");if(demo){
   ui_box(134,82,580,54,4);
   draw_text(154,96,3,3.0f,"FIGHTER JET PS3 DEMO");
   if((fc/25)%2==0){
    ui_box(214,395,420,48,4);
    draw_text(236,408,3,3.0f,"PRESIONE START");
   }
  }else{
   ui_box(12,10,560,26,4);
   char buf[96];
   sprintf(buf,"ALT:%02dM  CAM:%s  OST:SYNTHWAVE",(int)alt,cam==0?"CENITAL":(cam==1?"3D":"CABINA"));
   draw_text(20,17,3,1.5f,buf);
  }
  update_snd(fc,ct,cs,roll,demo);tiny3d_Flip();if(fc==1)wlog("6D. Primer Flip RSX COMPLETADO!");if(fc==10)wlog("7. 10 FRAMES FLUIDOS EN PS3 REAL!");}return 0;}
