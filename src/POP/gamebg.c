
#include "gamebg.h"
#include "eq.h"
#include "bgdata.h"
#include "movedata.h"
#include "grafix.h"




/******************************************
* DECLARATIONS
*******************************************/


unsigned char torchflame[] = {
    0x52, 0x53, 0x54, 0x55, 0x56, 0x61, 0x62, 0x63, 0x64, 0x52, 0x54, 0x56
};

int ptorchflame[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 3, 5, 7, 1, 4, 9, 2, 8, 6
};


unsigned char bubble[] = { 0xB2, 0xAF, 0xB0, 0xB1, 0xB0, 0xAF, 0xB1, 0xB0, 0xAF };


/***********************************************/

void setupflame(int spreced){
     if(spreced >=torchLast+1) return;

     IMAGE=torchflame[spreced];

     XCO++;

     YCO=YCO-43;

     OPACITY= STA;

     //mettre bgtab1 dans TAble
     setbgimg_bis("IMG.BGTAB1.DUN");

}

/******************************************
*  Set up to draw bubbling flask
*
* In/out: same as SETUPFLAME
*******************************************/

void setupflask(int st){

     OFFSET=boffset;

     if((st&0xE0)==EmptyPot){
          IMAGE=bubble[0];
          XCO++;
          XCO++;

          YCO=YCO-14;
          OPACITY=STA;

          setbgimg_bis("IMG.BGTAB2.DUN");

         return;
     }
     else{

          if((st&0xE0)==BoostPot){
          //special flask (taller)
               YCO=YCO-4;
              
           }else if((st&0xE0)>BoostPot){
               OFFSET++;
               YCO=YCO-4; 
           }

           if((st&0x1F)<bubbLast+1){
              IMAGE= bubble[st&0x1F];

           }
           else{

               IMAGE= bubble[0];
           }

           XCO++;
           XCO++;
           YCO=YCO-14;
           OPACITY=STA;

           setbgimg_bis("IMG.BGTAB2.DUN");
           return;
}
}