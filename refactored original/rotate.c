#include <stdio.h>
#include "allegro.h"

#define scw (SCREEN_W/2)
#define sch (SCREEN_H/2)

typedef struct trgb
{
  int r;
  int g;
  int b;
} trgb;


trgb gra(trgb c1, trgb c2, float i)
{
 trgb c;

 c.r = c1.r * (1-i) + c2.r * (i);
 c.g = c1.g * (1-i) + c2.g * (i);
 c.b = c1.b * (1-i) + c2.b * (i);

 return c;
}


/*trgb grad(trgb col1, trgb col2, int index, int max)
{
  trgb col;  
  float
  r1,g1,b1,
  r2,g2,b2,
  rr,rg,rb,
  mx,indx;

  r1=col1.r; g1=col1.g; b1=col1.b;
  r2=col2.r; g2=col2.g; b2=col2.b;
  mx=max; indx=index;

    if (r1>r2)  rr = r1 - ( (r1-r2) / mx * indx );
    if (r1<r2)  rr = r1 + ( (r2-r1) / mx * indx );
    if (r1==r2) rr = r2;

    if (g1>g2)  rg = g1 - ( (g1-g2) / mx * indx );
    if (g1<g2)  rg = g1 + ( (g2-g1) / mx * indx );
    if (g1==g2) rg = g2;

    if (b1>b2)  rb = b1 - ( (b1-b2) / mx * indx );
    if (b1<b2)  rb = b1 + ( (b2-b1) / mx * indx );
    if (b1==b2) rb = b2;

  if (indx!=0 && indx!=mx)
  {
   col.r=rr;
   col.g=rg;
   col.b=rb;
  } else
  {
     if (indx==0)
     {
       col.r=col1.r;
       col.g=col1.g;
       col.b=col1.b;
     }
     if (indx==mx)
     {
       col.r=col2.r;
       col.g=col2.g;
       col.b=col2.b;
     }
  }

  return col;
}*/




void rot(int x,int y,int cx,int cy,float cosine,float sine,float *rx, float *ry)
{
 float tx,ty;
 float mx,my;

tx=x-cx; ty=y-cy;
//tx = x;
//ty = y;

mx=tx*cosine - ty*sine;
my=tx*sine   + ty*cosine;

 *rx=mx+cx;
 *ry=my+cy;
}

void rotate(BITMAP* bmp,  BITMAP* tmp,float angle)
{


 int i,j,x,y;
 float sine,cosine;
 float rx,ry;
 int w,h;

 trgb p1,p2,p3,p4,hore,dole,p;
 int p1_,p2_,p3_,p4_;



 w=bmp->w;
 h=bmp->h;
 
 clear_to_color(tmp,makecol(255,0,255));
 //tmp = create_bitmap(w,h);
 //blit(bmp,tmp,0,0,0,0,w,h);
 
 sine=sin(-angle*0.01745329252);
 cosine=cos(-angle*0.01745329252); 


 for (i=0; i<w; i++)
 for (j=0; j<h; j++)
 {

  rot(i,j,w/2,h/2,cosine,sine,&rx,&ry);

  //s AA
 /* x=(int)rx;
  y=(int)ry;

  p1_=getpixel(tmp,x  ,y  ); 
  p2_=getpixel(tmp,x+1,y  ); 
  p3_=getpixel(tmp,x+1,y+1); 
  p4_=getpixel(tmp,x  ,y+1); 

  p1.r=getr(p1_); p1.g=getg(p1_); p1.b=getb(p1_);
  p2.r=getr(p2_); p2.g=getg(p2_); p2.b=getb(p2_);
  p3.r=getr(p3_); p3.g=getg(p3_); p3.b=getb(p3_);
  p4.r=getr(p4_); p4.g=getg(p4_); p4.b=getb(p4_);

  hore=gra(p1,p2, rx-x);
  dole=gra(p4,p3, rx-x);

  p=gra(hore,dole,ry-y);

  putpixel(bmp,i,j,makecol(p.r,p.g,p.b));*/
  

  //bez AA
  if(rx < 0 || ry < 0 || rx > w - 1 || ry > h-1)putpixel(tmp,i,j,makecol(255,0,255));
  else
  putpixel(tmp,i,j,getpixel( bmp,(int)rx,(int)ry ));

 }
 
}




/*int main()
{
 int m,i,j;
 BITMAP* bmp;
 BITMAP* bmp2;
 float a;

 int x,y,old_x,old_y;

 allegro_init();
 install_keyboard();

 set_color_depth(32);
 if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0)!=0)
 {
   set_gfx_mode(GFX_TEXT,0,0,0,0);
   allegro_message("Error: %s",allegro_error);
   return 1;
 }

 clear_to_color(screen, makecol(0,0,50));

 bmp2= load_bitmap("s3.bmp",NULL); ;
 bmp = load_bitmap("s3.bmp",NULL); ;

 while (!keypressed())
 {
   a=a+1; if (a>=360) a=a-360;

   blit(bmp2,bmp,0,0,0,0,bmp->w,bmp->h);   

   rotate(bmp,a);
   blit(bmp,screen,0,0,0,0,bmp->w,bmp->h);
   rest(10);
 }

 destroy_bitmap(bmp);

 return 0;
}
END_OF_MAIN();*/
