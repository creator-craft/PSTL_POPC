
//#include "Global.h"
#include <stdio.h>
#include "eq.h"
#include "frameadv_2.h"

#include "topctrl.h"

unsigned char *image_data;
const unsigned char loosed[] = {
    0x15, 0x2C, 0x15, 0x2D, 0x2D, 0x15, 0x15, 0x15, 0x2D, 0x2D, 0x2D
};

int dummy[]={maxpeel,maxpeel};
unsigned char blockd[2] = { 0x86, 0x86 };
// const int initsettings[]={gmaxval,gminval};

unsigned char OPCODE[] = {
    0x31,  // and (oper), Y
    0x11,  // ora
    0x91,  // sta
    0x51,  // eor
    0x31,  // and
    0x91   // sta
};


const unsigned char pieced[] = {
    0x00, 0x15, 0x15, 0x15,
    0x15, 0x18, 0x19, 0x16,
    0x15, 0x00, 0x15, 0x00,
    0x17, 0x15, 0x2E, 0x4C
};

const int Mult10[16] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160};


const int ScrnLeft = 58;
const int ScrnTop = 0;
const int ScrnBot = 191;
const int BlockHeight = 63;

const int Blox1 = BlockHeight;
const int Blox2 = 2*BlockHeight;
const int Blox3 = 3*BlockHeight;
const int Blox4 = 4*BlockHeight;

const int BlockBot[]= {ScrnBot+1-Blox4,
    ScrnBot+1-Blox3,
    ScrnBot+1-Blox2,
    ScrnBot+1-Blox1,
    ScrnBot+1};


int objid;
int BANK;



unsigned char idmask= 0x1F;
int Dy;
int XCO;
int YCO;
int IMAGE;
int OPACITY; //0-5
int state;
int blockxco;

int inbuilder=0;

/*Données blueprint*/
unsigned char **BlueType;
unsigned char **BlueSpec;
unsigned char *Table;

int peelX[maxpeel * 2];

int bgX[maxback];
int bgY[maxback]; 
int bgIMG[maxback];
int bgOP[maxback]; 
int Ay;
int yindex;
int SCRNUM= 1;


/*variables initialisé dans zerolsts()*/
int  genCLS; 
int  wipeX;
// int  midX;
int  objX;
// int  fgX;
int  msgX;


void DoSure(){//numero du screen qu'on souhaite dessiner, on le met à 1
    printf("je suis dans DOSURE\n");
 //SCRNUM = VisScrn; 


 zerolsts();

 sure();

 zeropeels();
 zerored();

 drawall();


}



int main(){
 
    DoSure();

    for(int i=1; i<=50;i++){

        printf("bgIMG %d %d\n ", i,bgIMG[i] );
        printf("bgX %d %d\n ", i,bgX[i] );
        printf("bgY %d %d\n ", i,bgY[i] );
        printf("OPACITYY %d %d\n ", i,bgOP[i] );

     

        // printf("fgIMG %d %d\n ", i,fgIMG[i] );
        // printf("fgX %d %d\n ", i,fgX[i] );
        // printf("fgY %d %d\n ", i,fgY[i] );



    }

    return 1;

}


//gcc topctrl.c FrameAdV.c Grafix.c GameBG.c hires.c BGDATA.c EQ.c GameEQ.c HRtables.c Tables.c  -o s  -lSDL2 -lSDL2_image