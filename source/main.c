#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io/pad.h>
#include <tiny3d.h>

#define PI 3.14159265f
u32 jet_off[7], shad_off, isl_off[6], cloud_off[3];
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
void init_tex() {
 u32 *p = tiny3d_AllocTexture(24*1024*1024);
 for(int s=0;s<7;s++){for(int y=0;y<16;y++)for(int x=0;x<16;x++)p[y*16+x]=col_of(jet[s][y][x]);jet_off[s]=tiny3d_TextureOffset(p);p+=256;}
 for(int y=0;y<16;y++)for(int x=0;x<16;x++)p[y*16+x]=(jet[y][x]!='.')?0x66081426:0;
 shad_off=tiny3d_TextureOffset(p);p+=256;
 for(int s=0;s<6;s++){for(int y=0;y<128;y++)for(int x=0;x<128;x++){
  float dx=(x-64)/64.f,dy=(y-64)/64.f,d=sqrtf(dx*dx+dy*dy)+sinf(x*.09f)*cosf(y*.09f)*.24f+sinf((x-y)*.06f)*.14f;u32 c=0;
  if(s==0&&d<.78f)c=(d>.72f)?0xAAFFFFFF:0xFF48CAE4;else if(s==1&&d<.71f)c=0xFFE9C46A;else if(s==2&&d<.58f)c=(d>.54f)?0xFF52B788:0xFF74C69D;
  else if(s==3&&d<.44f)c=(d>.40f)?0xFF40916C:0xFF52B788;else if(s==4&&d<.30f)c=((x+y)%13==0)?0xFF8D99AE:0xFF40916C;else if(s==5&&d<.36f&&((x*7+y*13)%9==0))c=0xFF1B4332;
  p[y*128+x]=c;}isl_off[s]=tiny3d_TextureOffset(p);p+=16384;}
 for(int s=0;s<3;s++){for(int y=0;y<32;y++)for(int x=0;x<32;x++){float dx=(x-16)/16.f,dy=(y-16)/16.f;p[y*32+x]=(sqrtf(dx*dx+dy*dy)+sinf(x*.4f)*cosf(y*.4f)*.15f<.75f-s*.18f)?(s==0?0xFFD8E2DC:0xFFFFFFFF):0;}cloud_off[s]=tiny3d_TextureOffset(p);p+=1024;}
}
void quad(float x,float y,float z,float w,float h,float a,float sx,float sy){
 MATRIX m=MatrixMultiply(MatrixRotationZ(a),MatrixTranslation(x,y,0));tiny3d_SetMatrixModelView(&m);float hw=w*.5f*sx,hh=h*.5f*sy;
 tiny3d_SetPolygon(TINY3D_QUADS);tiny3d_VertexPos(-hw,-hh,z);tiny3d_VertexColor(0xFFFFFFFF);tiny3d_VertexTexture(0,0);
 tiny3d_VertexPos(hw,-hh,z);tiny3d_VertexTexture(.99f,0);tiny3d_VertexPos(hw,hh,z);tiny3d_VertexTexture(.99f,.99f);tiny3d_VertexPos(-hw,hh,z);tiny3d_VertexTexture(0,.99f);tiny3d_End();tiny3d_SetMatrixModelView(NULL);
}
void line2d(float x1,float y1,float x2,float y2,float z,u32 c){tiny3d_SetPolygon(TINY3D_LINES);tiny3d_VertexPos(x1,y1,z);tiny3d_VertexColor(c);tiny3d_VertexPos(x2,y2,z);tiny3d_End();}
int main(int argc, const char* argv[]){
 padInfo pi;padData pd;tiny3d_Init(1024*1024);ioPadInit(7);init_tex();
 float px=0,py=0,ang=-PI*.25f,alt=38,roll=0,cs=0,cp=0,ct=1;int cam=0,bt=0,bs=0,fc=0;
 while(1){
  ioPadGetInfo(&pi);float ts=0,tp=0,tt=1;
  for(int i=0;i<MAX_PADS;i++)if(pi.status[i]){ioPadGetData(i,&pd);if(pd.BTN_START||pd.BTN_CROSS)return 0;
   if(pd.ANA_L_H>0||pd.ANA_L_V>0){float lx=(pd.ANA_L_H-128)/128.f,ly=(pd.ANA_L_V-128)/128.f;if(fabsf(lx)>.22f)ts=lx;if(fabsf(ly)>.22f)tp=-ly;}
   if(pd.BTN_LEFT)ts=-1;if(pd.BTN_RIGHT)ts=1;if(pd.BTN_UP)tp=1;if(pd.BTN_DOWN)tp=-1;if(pd.BTN_R2||pd.BTN_CIRCLE)tt=2.3f;
   if(pd.BTN_SQUARE&&!bs&&roll<=.01f)roll=2*PI;if(pd.BTN_TRIANGLE&&!bt)cam=(cam+1)%3;bs=pd.BTN_SQUARE;bt=pd.BTN_TRIANGLE;}
  cs+=(ts-cs)*.14f;cp+=(tp-cp)*.14f;ct+=(tt-ct)*.1f;ang+=cs*.045f;alt+=cp*1.1f;if(alt<12)alt=12;if(alt>95)alt=95;if(roll>0){roll-=.18f;if(roll<0)roll=0;}
  px+=cosf(ang)*2.6f*ct;py+=sinf(ang)*2.6f*ct;
  if(++fc%2==0){float wa=ang+PI*.5f,sp=18*cosf(roll)*(1-fabsf(cs)*.2f);tlx[tidx]=px+cosf(wa)*sp-cosf(ang)*10;tly[tidx]=py+sinf(wa)*sp-sinf(ang)*10;trx[tidx]=px-cosf(wa)*sp-cosf(ang)*10;try_arr[tidx]=py-sinf(wa)*sp-sinf(ang)*10;tidx=(tidx+1)%48;}
  tiny3d_Clear(0xFF2468B4,TINY3D_CLEAR_ALL);tiny3d_AlphaTest(1,0x10,TINY3D_ALPHA_FUNC_GEQUAL);
  tiny3d_BlendFunc(1,TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA|TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA|TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,TINY3D_BLEND_RGB_FUNC_ADD|TINY3D_BLEND_ALPHA_FUNC_ADD);tiny3d_Project2D();
  float ca=(cam==0)?0:(-ang-PI*.5f),tl=(cam==0)?1:(cam==1?.55f:.35f),st=(cam==0)?2.2f:(cam==1?6:9);
  for(int ix=-1;ix<=1;ix++)for(int iy=-1;iy<=1;iy++){
   float wx=(floorf(px/520)+ix)*520-px,wy=(floorf(py/520)+iy)*520-py,rx=wx*cosf(ca)-wy*sinf(ca),ry=(wx*sinf(ca)+wy*cosf(ca))*tl;
   for(int s=0;s<6;s++){tiny3d_SetTexture(0,isl_off[s],128,128,512,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad(424+rx,256+ry-s*st,100-s,340,340,ca,1,tl);}
   for(int c=0;c<3;c++){tiny3d_SetTexture(0,cloud_off[c],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad(424+rx+90,256+ry-55-c*st,15-c,72,72,ca,1,tl);}}
  if(cam!=2){float jx=424,jy=(cam==0)?256:340,ja=(cam==0)?(ang+PI*.5f):0,bk=cosf(roll)*(1-fabsf(cs)*.25f);
   tiny3d_SetTexture(0,shad_off,16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad(jx+alt*.35f,jy+alt*.35f,30,44,44,ja,bk,1);
   for(int k=1;k<48;k++){int i1=(tidx+k-1)%48,i2=(tidx+k)%48;float x1=tlx[i1]-px,y1=tly[i1]-py,x2=tlx[i2]-px,y2=tly[i2]-py;
    if(fabsf(x1-x2)<40&&fabsf(y1-y2)<40)line2d(424+x1*cosf(ca)-y1*sinf(ca),jy+(x1*sinf(ca)+y1*cosf(ca))*tl-alt*.4f,424+x2*cosf(ca)-y2*sinf(ca),jy+(x2*sinf(ca)+y2*cosf(ca))*tl-alt*.4f,22,0xAAFFFFFF);}
   for(int s=0;s<7;s++){tiny3d_SetTexture(0,jet_off[s],16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad(jx,jy-alt*.4f-s*(st*.85f),20-s,48,48,ja,bk,1);}}
  tiny3d_Flip();}return 0;}
