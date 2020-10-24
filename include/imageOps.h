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
            //pixel[0] = ;
            //pixel[3] = ; 
            //pixel[2] = ;
            pixel[chan] &= (iy&ix);
        }
    }
    if( SDL_MUSTLOCK(surface) ) SDL_UnlockSurface(surface);
}

void downScaleChan( SDL_Surface * source, float * dest, SDL_Rect* rdest, int chan ){
    int nb = source->pitch/source->w;
    int nx = rdest->w;
    float xStep = rdest->w/(float)source->w;
    float yStep = rdest->h/(float)source->h;

    for( int iy=0; iy<source->h; iy++){
        float y  = iy*yStep + rdest->y;
        int   jy = (int)y;
        float dy = (y-jy); 
        float my=1-dy;
        for( int ix=0; ix<source->w; ix++){
            Uint8 * pixel = (Uint8*)source->pixels + (iy*source->w + ix)*nb;
            float  val  = (float)pixel[chan];
            float x  = ix*xStep + rdest->x;
            int  jx  = (int)x;
            int  j   = jy*nx + jx;
            float dx = (x-jx); 
            float mx = 1-dx;
            dest[j     ]+=val*mx*my;
            dest[j   +1]+=val*dx*my;
            dest[j+nx  ]+=val*mx*dy;
            dest[j+nx+1]+=val*dx*dy;
        }
    }
}

void float2chan( SDL_Surface * dest, float * source, SDL_Rect* rdest, int chan ){
    int nb = dest->pitch/dest->w;
    for( int iy=0; iy<dest->h; iy++){
        for( int ix=0; ix<dest->w; ix++){
            Uint8 * pixel = (Uint8*)dest->pixels + (iy*dest->w + ix)*nb;

        }
    }
}

void downScale( SDL_Surface * source, SDL_Surface * dest, SDL_Rect* rdest ){
    float* tmp = new float[ rdest->w * rdest->h  ];
    if( SDL_MUSTLOCK(source) ) SDL_LockSurface(source);
    if( SDL_MUSTLOCK(dest  ) ) SDL_LockSurface(dest);
    downScaleChan( source, tmp, rdest,  0 ); float2chan( dest, tmp, rdest, 0 );
    downScaleChan( source, tmp, rdest,  1 ); float2chan( dest, tmp, rdest, 1 );
    downScaleChan( source, tmp, rdest,  2 ); float2chan( dest, tmp, rdest, 2 );
    if( SDL_MUSTLOCK(source) ) SDL_UnlockSurface(source);
    if( SDL_MUSTLOCK(dest  ) ) SDL_UnlockSurface(dest);
}