#include <SDL2/SDL.h>

/*
void PutPixel24_nolock(SDL_Surface * surface, int x, int y, Uint32 color){
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    pixel[0] = (color >> 24) & 0xFF;
    pixel[1] = (color >> 16) & 0xFF;
    pixel[2] = (color >> 8) & 0xFF;
#else
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
#endif
}


void PutPixel32(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel32_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);
}
*/


void modifyChanel(  SDL_Surface * surface, int chan  ){
    if( SDL_MUSTLOCK(surface) ) SDL_LockSurface(surface);
    int nb = surface->pitch/surface->w;
    for( int iy=0; iy<surface->h; iy++){
        for( int ix=0; ix<surface->w; ix++){
            Uint8 * pixel = (Uint8*)surface->pixels + (iy*surface->w + ix)*nb;

            pixel[chan] &= (iy&ix);
        }
    }
    if( SDL_MUSTLOCK(surface) ) SDL_UnlockSurface(surface);
}

void setBuff(int n, float* buff, float val){ for(int i=0; i<n; i++){ buff[i]=val; }; };

void setImg(SDL_Surface * img, float val, int chan){
    int n  = img->w * img->h;
    int nb = img->pitch/img->w;
    Uint8 * buff = (Uint8*)img->pixels;
    //printf( "n %i nb %i \n", n, nb );
    for(int i=0; i<n; i++){ buff[i*nb+chan]=val; };
}

void fillImg(SDL_Surface * img, SDL_Rect* rdest, float val, int chan ){
    int n  = img->w * img->h;
    int nb = img->pitch/img->w;
    //printf( "n %i nb %i \n", n, nb );
    for(int iy=rdest->y; iy<rdest->h; iy++){
        for(int ix=rdest->x; ix<rdest->w; ix++){
            Uint8 * buff = ((Uint8*)img->pixels) + (iy*img->w + ix )*nb; 
            buff[chan]=val; 
        }
    };
}

void downScaleChan( SDL_Surface * source, float * dest, SDL_Rect* rdest, int chan ){
    int nb = source->pitch/source->w;
    int nx = rdest->w;
    float xStep = rdest->w/(float)source->w;
    float yStep = rdest->h/(float)source->h;
    float dV = xStep*yStep;
    printf( "downScaleChan nb %i xyStep %g %g dV %g \n", nb, xStep, yStep, dV );
    for( int iy=0; iy<source->h; iy++){
        float y  = iy*yStep + rdest->y;
        int   jy = (int)y;
        float dy = (y-jy); 
        float my=1-dy;
        for( int ix=0; ix<source->w; ix++){
            Uint8 * pixel = ((Uint8*)source->pixels) + (iy*source->w + ix)*nb;
            float  val  = pixel[chan] * dV;
            float x  = ix*xStep + rdest->x;
            int  jx  = (int)x;
            float dx = (x-jx); 
            float mx = 1-dx;
            int  j   = jy*nx + jx;
            if( (jx<(rdest->w-1)) &  (jy<(rdest->h-1)) ){
                //printf( "ix,iy %i %i dx,dy %g %g\n", ix,iy, dx, dy );
                dest[j     ]+=val*mx*my;
                dest[j   +1]+=val*dx*my;
                dest[j+nx  ]+=val*mx*dy;
                dest[j+nx+1]+=val*dx*dy;
            }
        }
    }
}

void float2chan( SDL_Surface * dest, float * source, SDL_Rect* rdest, int chan ){
    int nb = dest->pitch/dest->w;
    for( int iy=rdest->y; iy<rdest->h; iy++){
        for( int ix=rdest->x; ix<rdest->w; ix++){
            int i = iy*dest->w + ix;
            Uint8 * pixel = ((Uint8*)dest->pixels) + i*nb;
            Uint8 b = (Uint8)source[i];
            pixel[chan] = b;
            //pixel[chan] = ix&iy;
        }
    }
}

void downScale( SDL_Surface * source, SDL_Surface * dest, SDL_Rect* rdest ){
    int n = rdest->w * rdest->h ;
    float* tmp = new float[ n ];
    if( SDL_MUSTLOCK(source) ) SDL_LockSurface(source);
    if( SDL_MUSTLOCK(dest  ) ) SDL_LockSurface(dest);
    setBuff(n,tmp,0.); downScaleChan( source, tmp, rdest,  0 ); float2chan( dest, tmp, rdest, 0 );
    setBuff(n,tmp,0.); downScaleChan( source, tmp, rdest,  1 ); float2chan( dest, tmp, rdest, 1 );
    setBuff(n,tmp,0.); downScaleChan( source, tmp, rdest,  2 ); float2chan( dest, tmp, rdest, 2 );
    //float2chan( dest, tmp, rdest, 2 );
    //setImg( dest, 0x8f, 0);
    //setImg( dest, 0x3f, 1);
    //setImg( dest, 0x6f, 2);
    fillImg( dest, rdest, 0xff, 3);
    if( SDL_MUSTLOCK(source) ) SDL_UnlockSurface(source);
    if( SDL_MUSTLOCK(dest  ) ) SDL_UnlockSurface(dest);
    delete [] tmp; 
}