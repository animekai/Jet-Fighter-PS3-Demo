#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io/pad.h>
#include <tiny3d.h>
#include <libfont.h>

extern unsigned char msx[];
#define PI 3.14159265f
u32 jet_off[7], shad_off, white_off, isl_off[6], cloud_off[3];
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
 ResetFont();
 p = (u32 *) AddFontFromBitmapArray((u8 *) msx, (u8 *) p, 0, 254, 8, 8, 1, BIT7_FIRST_PIXEL);
 for(int s=0;s<7;s++){for(int y=0;y<16;y++)for(int x=0;x<16;x++)p[y*16+x]=col_of(jet[s][y][x]);jet_off[s]=tiny3d_TextureOffset(p);p+=256;}
 for(int y=0;y<16;y++)for(int x=0;x<16;x++)p[y*16+x]=(jet[0][y][x]!='.')?0x66081426:0;
 shad_off=tiny3d_TextureOffset(p);p+=256;
 for(int i=0;i<16;i++)p[i]=0xFFFFFFFF;white_off=tiny3d_TextureOffset(p);p+=16;
 for(int s=0;s<6;s++){for(int y=0;y<128;y++)for(int x=0;x<128;x++){
  float dx=(x-64)/64.f,dy=(y-64)/64.f,d=sqrtf(dx*dx+dy*dy)+sinf(x*.09f)*cosf(y*.09f)*.24f+sinf((x-y)*.06f)*.14f;u32 c=0;
  if(s==0&&d<.78f)c=(d>.72f)?0xAAFFFFFF:0xFF48CAE4;else if(s==1&&d<.71f)c=0xFFE9C46A;else if(s==2&&d<.58f)c=(d>.54f)?0xFF52B788:0xFF74C69D;
  else if(s==3&&d<.44f)c=(d>.40f)?0xFF40916C:0xFF52B788;else if(s==4&&d<.30f)c=((x+y)%13==0)?0xFF8D99AE:0xFF40916C;else if(s==5&&d<.36f&&((x*7+y*13)%9==0))c=0xFF1B4332;
  p[y*128+x]=c;}isl_off[s]=tiny3d_TextureOffset(p);p+=16384;}
 for(int s=0;s<3;s++){for(int y=0;y<32;y++)for(int x=0;x<32;x++){float dx=(x-16)/16.f,dy=(y-16)/16.f;p[y*32+x]=(sqrtf(dx*dx+dy*dy)+sinf(x*.4f)*cosf(y*.4f)*.15f<.75f-s*.18f)?(s==0?0xFFD8E2DC:0xFFFFFFFF):0;}cloud_off[s]=tiny3d_TextureOffset(p);p+=1024;}
}
void quad2d(float x,float y,float z,float w,float h,float a,float sx,float sy){
 MATRIX m=MatrixMultiply(MatrixRotationZ(a),MatrixTranslation(x,y,0));tiny3d_SetMatrixModelView(&m);float hw=w*.5f*sx,hh=h*.5f*sy;
 tiny3d_SetPolygon(TINY3D_QUADS);tiny3d_VertexPos(-hw,-hh,z);tiny3d_VertexColor(0xFFFFFFFF);tiny3d_VertexTexture(0,0);
 tiny3d_VertexPos(hw,-hh,z);tiny3d_VertexTexture(.99f,0);tiny3d_VertexPos(hw,hh,z);tiny3d_VertexTexture(.99f,.99f);tiny3d_VertexPos(-hw,hh,z);tiny3d_VertexTexture(0,.99f);tiny3d_End();tiny3d_SetMatrixModelView(NULL);
}
void rect2d(float x,float y,float w,float h,float z,u32 c1,u32 c2){
 tiny3d_SetTexture(0,white_off,4,4,16,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 tiny3d_SetPolygon(TINY3D_QUADS);tiny3d_VertexPos(x,y,z);tiny3d_VertexColor(c1);tiny3d_VertexTexture(0,0);tiny3d_VertexPos(x+w,y,z);tiny3d_VertexColor(c1);tiny3d_VertexTexture(1,0);
 tiny3d_VertexPos(x+w,y+h,z);tiny3d_VertexColor(c2);tiny3d_VertexTexture(1,1);tiny3d_VertexPos(x,y+h,z);tiny3d_VertexColor(c2);tiny3d_VertexTexture(0,1);tiny3d_End();
}
void line2d(float x1,float y1,float x2,float y2,float z,u32 c){
 tiny3d_SetTexture(0,white_off,4,4,16,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);
 tiny3d_SetPolygon(TINY3D_LINES);tiny3d_VertexPos(x1,y1,z);tiny3d_VertexColor(c);tiny3d_VertexTexture(0,0);tiny3d_VertexPos(x2,y2,z);tiny3d_VertexTexture(1,1);tiny3d_End();
}
void quad_persp(float x0,float z0,float x1,float z1,float x2,float z2,float x3,float z3,float yw,float hor,float zbuf){
 float s0=240.f/z0,s1=240.f/z1,s2=240.f/z2,s3=240.f/z3;
 tiny3d_SetPolygon(TINY3D_QUADS);
 tiny3d_VertexPos(424+x0*s0,hor+yw*s0,zbuf);tiny3d_VertexColor(0xFFFFFFFF);tiny3d_VertexTexture(0,0);
 tiny3d_VertexPos(424+x1*s1,hor+yw*s1,zbuf);tiny3d_VertexTexture(.99f,0);
 tiny3d_VertexPos(424+x2*s2,hor+yw*s2,zbuf);tiny3d_VertexTexture(.99f,.99f);
 tiny3d_VertexPos(424+x3*s3,hor+yw*s3,zbuf);tiny3d_VertexTexture(0,.99f);
 tiny3d_End();
}
void draw_world_3d(float px,float py,float ang,float alt,float cp,float cs){
 float ca=-ang-PI*.5f,hor=230+cp*45,co=cosf(ca),si=sinf(ca);
 rect2d(0,0,848,hor,65535,0x4A8FE7FF,0xB8E0FFFF);rect2d(0,hor,848,512-hor,65535,0x1E6BB8FF,0x103B6EFF);
 for(int ix=-2;ix<=2;ix++)for(int iy=-2;iy<=2;iy++){
  float wx=(floorf(px/520)+ix)*520-px,wy=(floorf(py/520)+iy)*520-py;
  float cx=wx*co-wy*si,cz=-(wx*si+wy*co);
  if(cz>160&&cz<950){float h=170.f;
   float x0=cx-h*co+h*si,z0=cz-h*si-h*co,x1=cx+h*co+h*si,z1=cz+h*si-h*co;
   float x2=cx+h*co-h*si,z2=cz+h*si+h*co,x3=cx-h*co-h*si,z3=cz-h*si+h*co;
   if(z0>45&&z1>45&&z2>45&&z3>45){
    for(int s=0;s<6;s++){tiny3d_SetTexture(0,isl_off[s],128,128,512,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad_persp(x0,z0,x1,z1,x2,z2,x3,z3,alt*1.4f-s*3.2f,hor,500-cz-s);}
    for(int c=0;c<3;c++){tiny3d_SetTexture(0,cloud_off[c],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad_persp(x0+70,z0,x1+70,z1,x2+70,z2,x3+70,z3,alt*1.4f-42-c*2.8f,hor,480-cz-c);}
   }}}
}
int main(int argc, const char* argv[]){
 padInfo pi;padData pd;tiny3d_Init(4*1024*1024);ioPadInit(7);init_tex();
 float px=0,py=0,ang=-PI*.5f,alt=38,roll=0,cs=0,cp=0,ct=1;int demo=1,cam=0,bt=0,bs=0,bst=0,bsel=0,fc=0;
 while(1){
  ioPadGetInfo(&pi);float ts=0,tp=0,tt=1;fc++;
  if(demo){ts=sinf(fc*.015f)*.65f;tp=cosf(fc*.02f)*.35f;if(fc%240==0&&roll<=.01f)roll=2*PI;}
  for(int i=0;i<MAX_PADS;i++)if(pi.status[i]){ioPadGetData(i,&pd);
   if(pd.BTN_START&&!bst){if(demo){demo=0;cam=0;}else return 0;}
   if(pd.BTN_SELECT&&!bsel&&!demo){demo=1;cam=0;}
   if(!demo){
    if(pd.ANA_L_H>0||pd.ANA_L_V>0){float lx=(pd.ANA_L_H-128)/128.f,ly=(pd.ANA_L_V-128)/128.f;if(fabsf(lx)>.22f)ts=lx;if(fabsf(ly)>.22f)tp=-ly;}
    if(pd.BTN_LEFT)ts=-1;if(pd.BTN_RIGHT)ts=1;if(pd.BTN_UP)tp=1;if(pd.BTN_DOWN)tp=-1;
    if(pd.BTN_CROSS||pd.BTN_R2)tt=2.3f;if(pd.BTN_SQUARE&&!bs&&roll<=.01f)roll=2*PI;if(pd.BTN_TRIANGLE&&!bt)cam=(cam+1)%3;
   }bst=pd.BTN_START;bsel=pd.BTN_SELECT;bs=pd.BTN_SQUARE;bt=pd.BTN_TRIANGLE;}
  cs+=(ts-cs)*.14f;cp+=(tp-cp)*.14f;ct+=(tt-ct)*.1f;ang+=cs*.042f;alt+=cp*1.1f;
  if(alt<12)alt=12;if(alt>95)alt=95;if(roll>0){roll-=.18f;if(roll<0)roll=0;}
  px+=cosf(ang)*2.6f*ct;py+=sinf(ang)*2.6f*ct;
  if(fc%2==0){float wa=ang+PI*.5f,sp=18*cosf(roll)*(1-fabsf(cs)*.2f);tlx[tidx]=px+cosf(wa)*sp-cosf(ang)*10;tly[tidx]=py+sinf(wa)*sp-sinf(ang)*10;trx[tidx]=px-cosf(wa)*sp-cosf(ang)*10;try_arr[tidx]=py-sinf(wa)*sp-sinf(ang)*10;tidx=(tidx+1)%48;}
  tiny3d_Clear(0xFF2468B4,TINY3D_CLEAR_ALL);tiny3d_AlphaTest(1,0x10,TINY3D_ALPHA_FUNC_GEQUAL);
  tiny3d_BlendFunc(1,TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA|TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA|TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,TINY3D_BLEND_RGB_FUNC_ADD|TINY3D_BLEND_ALPHA_FUNC_ADD);
  tiny3d_Project2D();
  if(cam==0){
   for(int ix=-1;ix<=1;ix++)for(int iy=-1;iy<=1;iy++){
    float wx=(floorf(px/520)+ix)*520-px,wy=(floorf(py/520)+iy)*520-py;
    for(int s=0;s<6;s++){tiny3d_SetTexture(0,isl_off[s],128,128,512,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+wx,256+wy-s*2.2f,100-s,340,340,0,1,1);}
    for(int c=0;c<3;c++){tiny3d_SetTexture(0,cloud_off[c],32,32,128,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+wx+90,256+wy-55-c*2.2f,15-c,72,72,0,1,1);}}
   float ja=-(ang+PI*.5f),bk=cosf(roll)*(1-fabsf(cs)*.25f);
   tiny3d_SetTexture(0,shad_off,16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424+alt*.35f,256+alt*.35f,30,44,44,ja,bk,1);
   for(int k=1;k<48;k++){int i1=(tidx+k-1)%48,i2=(tidx+k)%48;
    float x1=tlx[i1]-px,y1=tly[i1]-py,x2=tlx[i2]-px,y2=tly[i2]-py;if(fabsf(x1-x2)<40&&fabsf(y1-y2)<40)line2d(424+x1,256+y1-alt*.4f,424+x2,256+y2-alt*.4f,22,0xAAFFFFFF);
    x1=trx[i1]-px;y1=try_arr[i1]-py;x2=trx[i2]-px;y2=try_arr[i2]-py;if(fabsf(x1-x2)<40&&fabsf(y1-y2)<40)line2d(424+x1,256+y1-alt*.4f,424+x2,256+y2-alt*.4f,22,0xAAFFFFFF);}
   for(int s=0;s<7;s++){tiny3d_SetTexture(0,jet_off[s],16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424,256-alt*.4f-s*1.9f,20-s,48,48,ja,bk,1);}
  }else if(cam==1){
   draw_world_3d(px,py,ang,alt,cp,cs);
   float jy=365-cp*25,jr=roll+cs*.45f,sp=48*cosf(jr);
   line2d(424-sp,jy,330-sp*1.5f,512,18,0xDDFFFFFF);line2d(424+sp,jy,518+sp*1.5f,512,18,0xDDFFFFFF);
   for(int s=0;s<7;s++){tiny3d_SetTexture(0,jet_off[s],16,16,64,TINY3D_TEX_FORMAT_A8R8G8B8,TEXTURE_NEAREST);quad2d(424,jy-s*3.2f,15-s,96,34,jr,1,1);}
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
  SetCurrentFont(0);
  if(demo){SetFontSize(24,36);SetFontAutoCenter(1);SetFontColor(0xFFFFFFFF,0x00000080);DrawString(0,110,"PS3 FIGHTER STACK");if((fc/25)%2==0){SetFontSize(16,24);SetFontColor(0xFFFCBF49,0x00000080);DrawString(0,410,"PRESIONE START");}SetFontAutoCenter(0);}
  else{SetFontSize(10,16);SetFontColor(0xFFFFFFFF,0x00000060);DrawFormatString(16,14,"ALT:%02dM  CAM:%s  [TRI]VISTA [CUAD]TONEL [SEL]MENU",(int)alt,cam==0?"CENITAL":(cam==1?"3D PERSECUCION":"CABINA"));}
  tiny3d_Flip();}return 0;}
