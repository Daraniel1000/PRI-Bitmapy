#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#pragma pack(1)

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

typedef struct tagFileHeader{
        WORD bfType;
        DWORD bfSize;
        WORD bfReserved1;
        WORD bfReserved2;
        DWORD bfOffBits;
    } FileHeader;
    typedef struct tagInfoHeader{
        DWORD biSize;
        LONG biWidth;
        LONG biHeight;
        WORD biPlanes;
        WORD biBitCount;
        DWORD biCompression;
        DWORD biSizeImage;
        LONG biXPelsPerMeter;
        LONG biYPelsPerMeter;
        DWORD biClrUsed;
        DWORD biClrImportant;
     } InfoHeader;
    typedef struct tagRGB24{
        BYTE B;
        BYTE G;
        BYTE R;
     } RGB;
    typedef struct tagPixJasnosc{
    unsigned char jsn;
    int x,y;
    }pixJ;
    typedef struct tagGranice{
    int g,d,l,p;
    }granice;
    struct mask32{
        DWORD R,G,B;
    }maska32;
    struct mask16{
        WORD R,G,B;
    }maska16;
    FileHeader Header;  //zmienne do
    InfoHeader Picture; //trzymania naglowkow
    int padding;        //i wyrownanie

    void bitmapInfo();  //wypisuje informacje o bitmapie, funkcja do debugowania
void czytInHeader();//wczytuje naglowek DIB
void czytmap();     //wczytuje kolory RGB wszystkich pikseli do tablicy struktur
FILE *zapis();      //zapisuje wynikowa bitmape do nowego pliku
void err();         //wypisuje odpowiedni blad
void odszum();      //odszumia/filtruje bitmape
void median();      //aplikuje filtr medianowy na obecny piksel i go wypisuje
void sred();        //aplikuje filtr usredniajacy na obecny piksel i go wypisuje
void minim();       //aplikuje filtr minimalny na obecny piksel i go wypisuje
void maksym();      //aplikuje filtr maksymalny na obecny piksel i go wypisuje
void negatyw();     //zmienia bitmapê w jej negatyw
void szarosc();     //zmienia bitmapê w odcienie szaroœci
int por();          //porownanie do funkcji qsort, u¿ytej w medianie

int por (const void * a, const void * b)    //porownanie do funkcji qsort, u¿ytej w medianie
{
    pixJ *pixA=(pixJ *)a;
    pixJ *pixB=(pixJ *)b;
    return (pixA->jsn-pixB->jsn);
}
void median(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj)//aplikuje filtr medianowy na obecny piksel
{
    int x,y,t=0,size;
    size=(gr.g-gr.d+1)*(gr.p-gr.l+1);
    pixJ tab[size];
    for(y=gr.d; y<=gr.g; y++)
    {
        for(x=gr.l; x<=gr.p; x++)
        {
            tab[t].jsn=(mapa[x][y].R+mapa[x][y].G+mapa[x][y].B)/3;
            tab[t].x=x; tab[t].y=y;
            t++;
        }
    }
    qsort(tab,size,sizeof(pixJ),por);
    fwrite(&mapa[tab[size/2].x][tab[size/2].y], 3, 1, wyj);
}
void sred(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj) //aplikuje filtr usredniajacy na obecny piksel i go wypisuje
{
    int x,y,size;
    unsigned int r=0,g=0,b=0; RGB sred;
    size=(gr.g-gr.d+1)*(gr.p-gr.l+1);
    for(y=gr.d; y<=gr.g; y++)
    {
        for(x=gr.l; x<=gr.p; x++)
        {
            r+=mapa[x][y].R;
            g+=mapa[x][y].G;
            b+=mapa[x][y].B;
        }
    }
    sred.R=r/(size);
    sred.G=g/(size);
    sred.B=b/(size);
    fwrite(&sred, 3, 1, wyj);
}
void minim(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj)//aplikuje filtr minimalny na obecny piksel i go wypisuje
{
    int x,y,cj;
    pixJ min; min.jsn=255;
    for(y=gr.d; y<=gr.g; y++)
    {
        for(x=gr.l; x<=gr.p; x++)
        {
            cj=(mapa[x][y].R+mapa[x][y].G+mapa[x][y].B)/3;
            if(cj<=min.jsn)
            {
                min.jsn=cj;
                min.x=x;
                min.y=y;
            }
        }
    }
    fwrite(&mapa[min.x][min.y], 3, 1, wyj);
}
void maksym(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj)//aplikuje filtr maksymalny na obecny piksel i go wypisuje
{
    int x,y,cj;
    pixJ max; max.jsn=0;
    for(y=gr.d; y<=gr.g; y++)
    {
        for(x=gr.l; x<=gr.p; x++)
        {
            cj=(mapa[x][y].R+mapa[x][y].G+mapa[x][y].B)/3;
            if(cj>=max.jsn)
            {
                max.jsn=cj;
                max.x=x;
                max.y=y;
            }
        }
    }
    fwrite(&mapa[max.x][max.y], 3, 1, wyj);
}
void odszum(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)])                       //odszumianie/filtruje bitmapê
{
    unsigned int wiel=Picture.biWidth+1,rodz=0;
    puts("Podaj wielkosc filtra n (2n+1 na 2n+1)");
    while(wiel>Picture.biWidth||wiel>abs(Picture.biHeight))
    {
        while(scanf("%u",&wiel)!=1)err(1); wiel=2*wiel+1;
        if(wiel>Picture.biWidth||wiel>abs(Picture.biHeight)) err(2);
        if(wiel*wiel>15000000){err(4);wiel=Picture.biWidth;}
        //if(wiel%2==0){err(5); wiel=Picture.biWidth;}
        if(wiel<=1){err(6); wiel=Picture.biWidth;}

    }
    puts("Podaj rodzaj filtra:\n1: medianowy\n2: usredniajacy\n3: minimalny\n4: maksymalny\n");
    while(rodz<1||rodz>4)
    {
        while(scanf("%u",&rodz)!=1)err(1);
        if(rodz<1||rodz>4)err(3);
    }
    int x,y;
    granice gr;
    FILE *wyj=zapis(mapa,NULL,NULL,0);
    for(y=0; y<abs(Picture.biHeight); y++)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            gr.g=y+wiel/2;
            gr.d=y-wiel/2;
            gr.l=x-wiel/2;
            gr.p=x+wiel/2;
            if(gr.l<0)
            {
                gr.l=0;
            }
            if(gr.d<0)
            {
                gr.d=0;
            }
            if(gr.p>=Picture.biWidth)
            {
                gr.p=Picture.biWidth-1;
            }
            if(gr.g>=abs(Picture.biHeight))
            {
                gr.g=abs(Picture.biHeight)-1;
            }
            switch(rodz)
            {
                case 1:
                    median(mapa,NULL,NULL,gr,wyj);
                    break;
                case 2:
                    sred(mapa,NULL,NULL,gr,wyj);
                    break;
                case 3:
                    minim(mapa,NULL,NULL,gr,wyj);
                    break;
                case 4:
                    maksym(mapa,NULL,NULL,gr,wyj);
                    break;
            }
        }
        for(x=0; x<padding; x++)
        {
            fputc(0x00, wyj);
        }
    }
    fclose(wyj);
}
void szarosc(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)])                      //zmienia bitmapê w odcienie szaroœci
{
    int x,y; unsigned char szar;
    for(y=0; y<abs(Picture.biHeight); y++)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            szar=(mapa[x][y].R+mapa[x][y].G+mapa[x][y].B)/3;
            mapa[x][y].R=szar;
            mapa[x][y].G=szar;
            mapa[x][y].B=szar;
        }
    }
}
void negatyw(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)])                      //zmienia bitmapê w jej negatyw
{
    int x,y;
    for(y=0; y<abs(Picture.biHeight); y++)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            switch(Picture.biBitCount)
            {
                case 16:
                    mapa16[x][y]=mapa16[x][y]^maska16.R;
                    mapa16[x][y]=mapa16[x][y]^maska16.G;
                    mapa16[x][y]=mapa16[x][y]^maska16.B;
                    break;
                case 32:
                    mapa32[x][y]=mapa32[x][y]^maska32.R;
                    mapa32[x][y]=mapa32[x][y]^maska32.G;
                    mapa32[x][y]=mapa32[x][y]^maska32.B;
                    break;
                default:
                    mapa[x][y].R=255-mapa[x][y].R;
                    mapa[x][y].G=255-mapa[x][y].G;
                    mapa[x][y].B=255-mapa[x][y].B;
                    break;
            }
        }
    }
}
void err(int a)                                                     //wypisuje odpowiedni blad
{
    switch (a)
    {
        case 1:
            puts("Blad wczytywania!\n");
            break;
        case 2:
            puts("Blad: okno nie miesci sie w obrazku!\n");
            break;
        case 3:
            puts("Blad: niewlasciwa liczba\n");
            break;
        case 4:
            puts("Zbyt duze okno!\n");
            break;
        case 5:
            puts("Blad: liczba jest parzysta!\n");
            break;
        case 6:
            puts("Blad: zbyt mala liczba!\n");
            break;
    }
}
FILE *otw(char c)                                                    //otwiera plik do wczytania/zapisania
{
    if(c=='i')
        puts("Podaj nazwe pliku do wczytania (z .bmp):");
    else
        puts("Podaj nazwe pliku wyjsciowego:");
    char nazwa[40];
    scanf("%s",nazwa);
    if(c=='i')
        return fopen(nazwa,"rb");
    else
        return fopen(nazwa,"wb");
}

FILE *zapis(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)],int d) //zapisuje wynikowa bitmape do nowego pliku
{
    if(d==1) return NULL;
    if(d==0) Picture.biHeight=-Picture.biHeight;
    FILE *wyj=otw('o');
    int x,y;
    fwrite(&Header, sizeof(FileHeader), 1, wyj);
    fwrite(&Picture, sizeof(InfoHeader), 1, wyj);
    switch(Picture.biBitCount)
    {
        case 16:
            fwrite(&maska16,6,1,wyj);
            break;
        case 32:
            fwrite(&maska32,12,1,wyj);
            break;
    }
    fseek(wyj,Header.bfOffBits,0);
    if(d==0) return wyj;
    //int padding = (4 - (Picture.biWidth * 3) % 4) % 4;
    for(y=abs(Picture.biHeight)-1; y>=0 ; y--)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            switch(Picture.biBitCount)
            {
                case 16:
                    fwrite(&mapa16[x][y], 2, 1, wyj);
                    break;
                case 32:
                    fwrite(&mapa32[x][y], 4, 1, wyj);
                    break;
                default:
                    fwrite(&mapa[x][y], 3, 1, wyj);
                    break;
            }
        }
        for(x=0; x<padding; x++)
        {
            fputc(0x00, wyj);
        }
    }
    fclose(wyj);
    return NULL;
}
void czytmap(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],WORD mapa16[Picture.biWidth][abs(Picture.biHeight)],DWORD mapa32[Picture.biWidth][abs(Picture.biHeight)], FILE *plik) //wczytuje kolory RGB wszystkich pikseli do tablicy struktur
{
    fseek(plik,Header.bfOffBits,0);
    RGB temp; WORD temp16; DWORD temp32;
    int x,y;
    for(y=abs(Picture.biHeight)-1; y>=0; y--)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            switch(Picture.biBitCount)
            {
                case 16:
                    fread(&temp16,2,1,plik);
                    mapa16[x][y]=temp16;
                    break;
                case 32:
                    fread(&temp32,4,1,plik);
                    mapa32[x][y]=temp32;
                    break;
                default:
                    fread(&temp, 3, 1, plik);
                    mapa[x][y]=temp;
                    break;
            }
        }
        fseek(plik,padding,1);
    }
}
void czytInHeader(FILE *plik)
{
    int stFormat,bity;
    fread(&Picture.biSize,4,1,plik);
    bity=4;
    if(Picture.biSize<=12)
        stFormat=1;
    else
        stFormat=0;
    if (stFormat)
    {
        fread(&Picture.biWidth,2,1,plik);
        bity+=2;
    }
    else
    {
        fread(&Picture.biWidth,4,1,plik);
        bity+=4;
    }
    if (bity>=Picture.biSize)
        return;
    if(stFormat)
    {
        fread(&Picture.biHeight,2,1,plik);
        bity+=2;
    }
    else
    {
        fread(&Picture.biHeight,4,1,plik);
        bity+=4;
    }
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biPlanes,2,1,plik);
    bity+=2;
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biBitCount,2,1,plik);
    bity+=2;
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biCompression,4,1,plik);
    bity+=4;
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biSizeImage,4,1,plik);
    bity+=4;
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biXPelsPerMeter,4,1,plik);
    bity+=4;
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biYPelsPerMeter,4,1,plik);
    bity+=4;
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biClrUsed,4,1,plik);
    bity+=4;
    if(bity>=Picture.biSize)
        return;
    fread(&Picture.biClrImportant,4,1,plik);
    bity+=4;
    if(bity>=Picture.biSize)
        return;
    switch(Picture.biBitCount)
    {
        case 16:
            fread(&maska16,6,1,plik);
            break;
        case 32:
            fread(&maska32,12,1,plik);
            break;
    }
    fseek(plik, (Picture.biSize-bity), SEEK_CUR);
    return;
}
void bitmapInfo()                                                     //wypisuje informacje o bitmapie, funkcja do debugowania
{
    printf("\n Szerokosc: %d pikseli",Picture.biWidth);
    printf("\n Wysokosc: %d pikseli",Picture.biHeight);
    printf("\n Liczba bitow na piksel: %d (1, 4, 8, or 24)",Picture.biBitCount);
    printf("\n Kompresja: %d (0=none, 1=RLE-8, 2=RLE-4)",Picture.biCompression);
     printf("\n Rozmiar samego rysunku: %d",Picture.biSizeImage);
     printf("\n Rozdzielczosc pozioma: %d",Picture.biXPelsPerMeter);
     printf("\n Rozdzielczosc pionowa: %d",Picture.biYPelsPerMeter);
     printf("\n Liczba kolorow w palecie: %d\n",Picture.biClrUsed);
}
