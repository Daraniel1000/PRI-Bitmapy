#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#pragma pack(1)

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;
    typedef struct {
        WORD bfType;
        DWORD bfSize;
        WORD bfReserved1;
        WORD bfReserved2;
        DWORD bfOffBits;
    } FileHeader;
    typedef struct {
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
    typedef struct {
        BYTE B;
        BYTE G;
        BYTE R;
     } RGB;
    typedef struct {
    unsigned char jsn;
    int x,y;
    }pixJ;
    typedef struct {
    int g,d,l,p;
    }granice;
    FileHeader Header;  //zmienne do
    InfoHeader Picture; //trzymania naglowkow
    int padding;        //i wyrownanie
FILE *otw(char c);  //otwiera plik do wczytania/zapisania
void bitmapInfo();  //wypisuje informacje o bitmapie, funkcja do debugowania
void czytmap();     //wczytuje kolory RGB wszystkich pikseli do tablicy struktur
FILE *zapis();      //zapisuje wynikowa bitmape do nowego pliku
void err();         //wypisuje odpowiedni blad
void odszum();      //odszumia/filtruje bitmape
void median();      //aplikuje filtr medianowy na obecny piksel i go wypisuje
void sred();        //aplikuje filtr usredniajacy na obecny piksel i go wypisuje
void minim();       //aplikuje filtr minimalny na obecny piksel i go wypisuje
void maksym();      //aplikuje filtr maksymalny na obecny piksel i go wypisuje
void negatyw();     //zmienia bitmapę w jej negatyw
void szarosc();     //zmienia bitmapę w odcienie szarości
int por();          //porownanie do funkcji qsort, użytej w medianie
int main()
{
    FILE *plik;                                //zmienna pliku wejściowego
    while ((plik=otw('i'))==NULL) puts("Brak wlasciwego pliku");
    fread(&Header,sizeof(FileHeader),1,plik);       //wczytywanie
    fread(&Picture,sizeof(InfoHeader),1,plik);      //naglowkow
    if(Picture.biBitCount<=8)
    {
        puts("Niestety, nie udalo mi sie zmusic programu do dzialania dla malych bitmap");
        fclose(plik);
        return 0;
    }
bitmapInfo(&Picture);
    padding = (4-(Picture.biWidth * sizeof(RGB)) % 4) % 4;
    RGB mapa[Picture.biWidth][abs(Picture.biHeight)];  //utworzenie tablicy
    czytmap(mapa,plik);                             //i wczytanie do niej kolorow wszystkich pikseli
//piszmap(mapa);
    fclose(plik);
    puts("Wybierz, co ma zrobic program:\n1: filtrowanie/odszumianie\n2: negatyw\n3: konwersja na odcienie szarosci\n4: kopiowanie bitmapy do nowego pliku");
    unsigned int d=0;
    while(d<1||d>4)
    {
        while(scanf("%u",&d)!=1)err(1);
        if(d<1||d>4)err(3);
    }
    switch(d)
    {
        case 1:
            odszum(mapa);
            break;
        case 2:
            negatyw(mapa);
            break;
        case 3:
            szarosc(mapa);
            break;
    }
    zapis(mapa,d);                                    //zapis wyjsciowej mapy bitowej w nowym pliku
    return 0;
}
int por (const void * a, const void * b)    //porownanie do funkcji qsort, użytej w medianie
{
    pixJ *pixA=(pixJ *)a;
    pixJ *pixB=(pixJ *)b;
    return (pixA->jsn-pixB->jsn);
}
void median(RGB mapa[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj)//aplikuje filtr medianowy na obecny piksel i go wypisuje
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
    fwrite(&mapa[tab[size/2].x][tab[size/2].y], sizeof(RGB), 1, wyj);
}
void sred(RGB mapa[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj) //aplikuje filtr usredniajacy na obecny piksel i go wypisuje
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
    fwrite(&sred, sizeof(RGB), 1, wyj);
}
void minim(RGB mapa[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj)//aplikuje filtr minimalny na obecny piksel i go wypisuje
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
    fwrite(&mapa[min.x][min.y], sizeof(RGB), 1, wyj);
}
void maksym(RGB mapa[Picture.biWidth][abs(Picture.biHeight)], granice gr, FILE *wyj)//aplikuje filtr maksymalny na obecny piksel i go wypisuje
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
    fwrite(&mapa[max.x][max.y], sizeof(RGB), 1, wyj);
}
void odszum(RGB mapa[Picture.biWidth][abs(Picture.biHeight)])                       //odszumianie/filtruje bitmapę
{
    unsigned int szer=Picture.biWidth+1,wys,rodz=0;
    puts("Podaj wielkosc filtra\nFormat \"x y\" - filtr 2x+1 na 2y+1)");
    while(szer>Picture.biWidth||wys>abs(Picture.biHeight))
    {
        while(scanf("%u%u",&szer,&wys)!=2)err(1); szer=2*szer+1; wys=2*wys+1;
        if(szer>Picture.biWidth||wys>abs(Picture.biHeight)) err(2);
        if(szer*wys>15000000){err(4);szer=Picture.biWidth;}
        //if(wiel%2==0){err(5); wiel=Picture.biWidth;}
        if(szer<=1||wys<=1){err(6); szer=Picture.biWidth;}

    }
    puts("Podaj rodzaj filtra:\n1: medianowy\n2: usredniajacy\n3: minimalny\n4: maksymalny\n");
    while(rodz<1||rodz>4)
    {
        while(scanf("%u",&rodz)!=1)err(1);
        if(rodz<1||rodz>4)err(3);
    }
    int x,y;
    granice gr;
    FILE *wyj=zapis(mapa,0);
    for(y=0; y<abs(Picture.biHeight); y++)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            gr.g=y+wys/2;
            gr.d=y-wys/2;
            gr.l=x-szer/2;
            gr.p=x+szer/2;
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
                    median(mapa,gr,wyj);
                    break;
                case 2:
                    sred(mapa,gr,wyj);
                    break;
                case 3:
                    minim(mapa,gr,wyj);
                    break;
                case 4:
                    maksym(mapa,gr,wyj);
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
void szarosc(RGB mapa[Picture.biWidth][abs(Picture.biHeight)])                      //zmienia bitmapę w odcienie szarości
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
void negatyw(RGB mapa[Picture.biWidth][abs(Picture.biHeight)])                      //zmienia bitmapę w jej negatyw
{
    int x,y;
    for(y=0; y<abs(Picture.biHeight); y++)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            mapa[x][y].R=255-mapa[x][y].R;
            mapa[x][y].G=255-mapa[x][y].G;
            mapa[x][y].B=255-mapa[x][y].B;
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
FILE *zapis(RGB mapa[Picture.biWidth][abs(Picture.biHeight)],int d) //zapisuje wynikowa bitmape do nowego pliku
{
    if(d==1) return NULL;
    if(d==0) Picture.biHeight=-Picture.biHeight;
    FILE *wyj=otw('o');
    int x,y;
    fwrite(&Header, sizeof(FileHeader), 1, wyj);
    fwrite(&Picture, sizeof(InfoHeader), 1, wyj);
    fseek(wyj,Header.bfOffBits,0);
    if(d==0) return wyj;
    //int padding = (4 - (Picture.biWidth * sizeof(RGB)) % 4) % 4;
    for(y=abs(Picture.biHeight)-1; y>=0 ; y--)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            fwrite(&mapa[x][y], sizeof(RGB), 1, wyj);
        }
        for(x=0; x<padding; x++)
        {
            fputc(0x00, wyj);
        }
    }
    fclose(wyj);
    return NULL;
}
void czytmap(RGB mapa[Picture.biWidth][abs(Picture.biHeight)], FILE *plik) //wczytuje kolory RGB wszystkich pikseli do tablicy struktur
{
    fseek(plik,Header.bfOffBits,0);
    RGB temp;
    int x,y;
    for(y=abs(Picture.biHeight)-1; y>=0; y--)
    {
        for(x=0; x<Picture.biWidth; x++)
        {
            fread(&temp, Picture.biBitCount/8, 1, plik);
            mapa[x][y]=temp;
        }
        fseek(plik,padding,1);
    }
    /*for ( i = Header.bfOffBits; i < Header.bfSize;i+=3)
    {
        fread(&temp, sizeof(RGB), 1, plik);
        mapa[x][y]=temp;
        x++; x%=Picture.biWidth+1;
        if(x==0)y--;
    }*/
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
