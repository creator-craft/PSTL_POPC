//#include "Global.h"
#include "frameadv_2.h"
#include "eq.h"
#include "bgdata.h"
#include "gameeq.h"
#include "movedata.h"
#include <stdbool.h>
#include <stdio.h>
#include "tables.h"
#include "grafix.h"
#include "gamebg.h"

#include "topctrl.h"


/****************************************************
* DECLARATIONS
*
 **************************************************/
int Dy;
int Ay;
int yindex;
int blockxco;
int objid;
int state;

bool  carry= false;
int pile;
int gateposn;
int blockthr;
int gatebot;
int KidStartScrn=2;
int xsave;
int BASE[192][280];





/****************************************************
* Draw Gate B Front?
* (only if kid is to the left of bars)
*
 **************************************************/
/*----> demander au prof<------*/

 void drawgatebf(){
    setupdgb();

    //Gate bottom

    OPACITY=ORA;

    YCO=gatebot-2;//no 2 blank lines at bottom
    IMAGE=gatebotORA;

    addfore();

    //* Middle pieces

    IMAGE=gateB1;

    YCO= gatebot-12; //loop

    for(;YCO>=0;){

        if(YCO>=192) return;

        if(YCO-7 <blockthr||YCO-7 < 0){ //grill mid piece is 8 lines high--; will it stick up out of block area?
        //;Skip top piece to save a little time
        return;
        }
        //;no, we're still safe--keep going

        addfore();

         YCO=YCO-8;
    }


 }

 void DrawGateBF(int r,int c){
    if(r!=KidBlockY) return;

    if((c-1)!=KidBlockX )return; //is kid in gate block?

    if(scrnRight==KidScrn) return;

    drawgatebf(); //;draw gate bars over char
    
 }

 void maddfore(){
    OPACITY=my_mask;
    addfore();
    OPACITY=ORA;
    addfore();
 }
 
/****************************************************
*    Draw slicer front
*
 **************************************************/

 void drawslicerf(){

    if((state&0x7F)<slicerRet){ 
        xsave=slicerseq[(state&0x7F)]-1;


    }else{
        xsave=slicerseq[slicerRet]-1;
    }

    XCO=blockxco;
    YCO=Ay;

    if(slicerfrnt[xsave]==0) return;
    IMAGE=slicerfrnt[xsave];
    maddfore();
    return;
    
 }
/****************************************************
*    D R A W   F R O N T
*
 **************************************************/

 void drawfrnt(int r,int c){

    if(PRECED!=gate){
        if(objid==slicer){
            drawslicerf();
        }else if(objid==flask){

            if((state&0xE0)==0xA0){
                goto one;
            }else if((state&0xE0)<0x40){
                goto one;
            } else if(specialflask!= 0){goto twelve;}



        } else { goto one;}

        one:
            if(fronti[objid]==0) return;

        twelve:
            IMAGE=fronti[objid];
            YCO= Ay+fronty[objid];
            XCO= blockxco + frontx[objid];

            if(objid>=archtop2){
                OPACITY=STA;
                //Pieces that go to byte boundaries can be STA'd w/o masking
                addfore();
            }else{
                if(BGset1==1){goto n    dunj;}
                else if(objid==posts){ 
                    goto sta;
                    }
                    else{
                        ndunj:
                            if(objid==block) { goto blocka;}
                            else{
                                maddfore();
                            }
                        blocka:
                            if(state<numblox){ goto two;}
                            else{
                                int y=0;
                                two:
                                   IMAGE=blockfr[y];
                                   sta:
                                       OPACITY=STA;
                                       addfore();
                                       return;

                            }
                            

                    }
            }





    }else{
        DrawGateBF(r,c);
    }
 }



/****************************************************
*    Draw flask A (bubbles)
*
 ***************************************************/


 void drawflaska(){
    XCO=blockxco;
    YCO=Ay;
    setupflask(state); //DANS GAMBG
    addmidezo(UseLay);

 }
/****************************************************
*    D R A W   M O V A B L E   ' A '
*
 ***************************************************/

 void drawsworda(){

    if(state!=1){
        IMAGE=swordgleam0;

    }else{
        IMAGE=swordgleam1;
    }

    XCO=blockxco;
    YCO=Ay;
    OPACITY=STA;
    addback();

 }


 
 void drawslicera(){

    if((state&0x7f) <slicerRet){
        xsave= slicerseq[state&0x7f]-1;
    }else{
        xsave= slicerseq[slicerRet]-1;
    }

    XCO=blockxco;
    YCO=Ay;

    if(state>0){
        if(slicerbot[xsave]==0){
          
        }else{
            IMAGE= slicerbot[xsave];
            OPACITY=ORA;
            addback();
        }

    }else{
        if(slicerbot2[xsave]!=0) {

            IMAGE=slicerbot2[xsave];
            OPACITY=ORA;
            addback();
 
        }else{
            IMAGE= slicerbot[xsave];
            OPACITY=ORA;
            addback();
      
        }

    }
    if(slicertop[xsave]==0) return;
    IMAGE= slicertop[xsave];
    YCO=Ay-slicergap[xsave];
    OPACITY=ORA;
    addback();

 }


 void drawspikea(){
    if(state>0){
        if(spikea[state]==0) return;
        IMAGE=spikea[state];
    }else{
        if(spikea[spikeExt]==0) return;
        IMAGE=spikea[spikeExt];
    }

    XCO=blockxco;
    YCO=Ay-1;
    OPACITY=ORA;
    addback();
 }


void drawma(){

    if(objid==spikes){
        drawspikea();
    }else if(objid==slicer){
        drawslicera();
    }else if(objid==flask){
        drawflaska();
    }
    else if(objid==sword){
        drawsworda();
    }
    else{
        return;
    }

}
/****************************************************
 
* Get piece "A"
*
* In: state; X = objid
* Out: A = A-section image #
*
 ***************************************************/


 int getpiecea(int objid){

    if(objid==loose){
        int y= getloosey(state);
        return loosea[y];

    }else{
        return piecea[objid];

    }

 }


 /****************************************************
 *      D R A W   ' A '
*
*  (1) If piece to left has intrusive B-section (e.g., panel):
*      MASK A-section
*  (2) OR A-section
 ***************************************************/

 void addamask(){
    
    if(maska[objid]==0) return;
    IMAGE=maska[objid];
    XCO=blockxco;
    YCO=Ay;
    OPACITY= and;

    addback();
 }

 void adda(){
    int result=getpiecea(objid);
    if(result==0) return;
    adda1(result,objid);
}

 void adda1(int arch,int x){
    IMAGE=archpanel;
    XCO= blockxco;
    YCO= Ay + pieceay[x];
    OPACITY=ORA;
    addback();
 
 }


/*--------> prendre exemple sur cette fonction pour demander au prof comments les sauts fonctn<-----*/
 void drawa(){

    if(PRECED==archtop1){//:special
        if(objid!=panelwof){
            adda();

        }else{
           //arch ends to L of panel
            adda1(archpanel,objid);
        }

    }else{if(PRECED==panelwif||PRECED==panelwof||PRECED==pillartop||PRECED==block){ //:needmask
        addamask();

    }
    adda();
    }
}
 
/****************************************************
 *       Draw exit "b" (stairs)
 ***************************************************/

void drawexitb(){
    printf("je suis dans exit");
    IMAGE= stairs;

    YCO= Ay -12;

    if(blockxco>= 36) return;
    XCO= blockxco +1;

    OPACITY= STA;

    if(SCRNUM==KidStartScrn){ // KidStartScrn=2, initialisé dans startKid dans subs, initialise dans ce fichier pour eviter de traduire toute la ftc
        //:nostairs ;assume it's an entrance
        //draw door, bottom to top

        if(Dy-67>=192) return;
        blockthr= Dy-67; //topmost usable line

        gateposn= spreced/4; //gateposn := spreced/4

        doortop= Ay - 14 -gateposn; //;for CROPCHAR

       
        for(;YCO>=blockthr;){
            IMAGE=doormask;
            OPACITY=and;

            addback();

            IMAGE= door;
            OPACITY= ORA;

            addback();

            YCO=YCO-4;
        }

        //repair top

        if((Ay-64)>= 192) return; //;Technically part of C-section but who cares
        YCO=Ay-64;
        IMAGE=toprepair;
        OPACITY= STA;

        addback();

        return;

    }else{
        addback();
    }


}
 /***************************************************
 *       Draw torch B (flame)
 ***************************************************/

void drawtorchb(){
    if(blockxco==0) return; //no flame on leftmost torch
    XCO=blockxco;
    YCO=Ay;

    setupflame(spreced); //in gamebg
    addback();
    return;


}
 /***************************************************
 *      D R A W  L O O S E  F L O O R  " B "
 ***************************************************/
void drawlooseb(){
    
    int y=getloosey(spreced);
    IMAGE= looseb;

    YCO= Ay+ looseby[y];
   
    OPACITY= ORA;

    addback();


}
/***************************************************
 *        Draw spikes B
 ***************************************************/

 void drawspikeb(){

    if ((spreced & 0x80) == 0) {//hibit clear --> frame #
        if(spikeb[spreced]==0) return;
        IMAGE=spikeb[spreced];

       
    } else {
        if(spikeb[spikeExt]==0) return;
        IMAGE=spikeb[spikeExt];
    }

    XCO = blockxco;
    YCO = Ay - 1;       
    OPACITY = ORA;

    addback();             
    
 }

/***************************************************
*       DRAWGATEB, D R A W   G A T E   " B "
*
*  Lay down (STA) the gate in sections, bottom
*  to top.  The bottom piece has two blank lines that
*  erase its trail as the gate rises.
*  Topmost section has 8 separate shapes, 1-8 pixels high.
 ***************************************************/


 void restorebot(int colno){

    IMAGE= pieceb[gate];
    YCO=pieceby[gate]+Ay;
    XCO=blockxco;
    OPACITY=STA;
    addback();

    checkc();

    if(carry==false ){drawa();}
    else{ dodrawc(colno );}
}


 void setupdgb(){

    blockthr= Dy-62;

    if(spreced<gmaxval){
        gateposn= spreced/4 +1;
        gatebot= Ay- gateposn;
        return;
    }else{
        gateposn= gmaxval/4 +1;
        gatebot= Ay- gateposn;
        return;
    }

}

/*--------------> POSSIBILITÉ DE BEUG DANS LA BOUCLE AVEC YCO<--------------*/
 void drawgateb(int colno){
    
    setupdgb();

    //First, draw bottom piece

    if(gatebot + 12<Ay){
        //:storit Gate is above my_floor line -- STA it
        YCO= gatebot;
        IMAGE=gatebotSTA;
        OPACITY= STA;
        addback();

        //*  Next, draw as many middle pieces as we need to make up rest of grill

        OPACITY=STA;
        IMAGE=gateB1;

        YCO=gatebot-12; //loop

        for(;YCO>0;){
            
            if(YCO==192) return;

            if(YCO-7<0||YCO-7<blockthr){    //;grill mid piece is 8 lines high--
              //:done
              //now add final piece at top
                if( (YCO-blockthr +1)==0) return;  //desired height (0-8 pixels)
                if((YCO-blockthr +1)>=9) return;
                IMAGE=gate8b[YCO-blockthr];
                addback();
                return;
            }else{   //no, we're still safe--keep going

             addback();
             YCO=YCO-8;

            }
        }
        //fin boucle now add final piece at top

        if( (YCO-blockthr +1)==0) return;  //desired height (0-8 pixels)
        if((YCO-blockthr +1)>=9) return;
        IMAGE=gate8b[YCO-blockthr];
        addback();
        return;


    }
    else{//:orit * Bottom piece is partly below my_floor line -- STA won't work.We need to redraw b.g., then OR gate bottom on top.

        restorebot(colno);

        YCO=gatebot-2; //no 2 blank lines at bottom

        IMAGE=gatebotORA;

        OPACITY= ORA;
        addback();

        //:cont  Next, draw as many middle pieces as we need to make up rest of grill
        OPACITY=STA;
        IMAGE=gateB1;

        YCO=gatebot-12; //loop

        for(;YCO>0;){
            
        
        if(YCO==192) return;
        if(YCO-7<0||YCO-7<blockthr){//;grill mid piece is 8 lines high--
            //:done
            //now add final piece at top
           if( (YCO-blockthr +1)==0) return;  //desired height (0-8 pixels)
           if((YCO-blockthr +1)>=9) return;
           IMAGE=gate8b[YCO-blockthr];
           addback();
           return;



        }else{//no, we're still safe--keep going
             addback();
             YCO=YCO-8;
             }
            }
        //fin boucle now add final piece at top

        if( (YCO-blockthr +1)==0) return;  //desired height (0-8 pixels)
        if((YCO-blockthr +1)>=9) return;
        IMAGE=gate8b[YCO-blockthr];
        addback();
        return;
        }
 }

/***************************************************
 *         DRAWMB  D R A W   M O V A B L E   ' B '
 ***************************************************/

 void drawmb(int colno){

    if(PRECED==gate){
        drawgateb(colno);//draw B-section of moving gate 

    }else if(PRECED==spikes) {
        drawspikeb();
        
    }else if(PRECED==loose){
        drawlooseb();
    }else if(PRECED==torch){
        drawtorchb();
    }else if(PRECED==exita){
        drawexitb();
    }
    else{ return;}

 }

 /*********************************************
 *       Draw B-section of piece to left
 **********************************************/



 void drawb(){

    if(objid==block) return; //B-section hidden by solid block
    if(PRECED==space){

        if(spreced>=numbpans+1){return;}
        else if(spaceb[spreced]==0){return;}
        else{
            IMAGE= spaceb[spreced];
            YCO=spaceby[spreced] + Ay; //clearcary avant donc on ajoute pas 1, parti :cont
            XCO=blockxco;
            OPACITY=ORA;
            addback();
        }

    }
    else if(PRECED==my_floor){
        if(spreced>=numbpans+1){
            if(floorb[spreced]==0){return;}
            else{ IMAGE=floorb[spreced];
            
                YCO=floorby[spreced] + Ay;
                XCO=blockxco;
                OPACITY=ORA;
                addback();
            }

        }else{
            if(floorb[0]==0){return;}
            else{ 
                IMAGE=floorb[0];
                YCO=floorby[0] + Ay;
                XCO=blockxco;
                OPACITY=ORA;
                addback();

            }

        }

    }
    else if(PRECED==block){

        if(spreced<numblox){
           IMAGE= blockb[spreced];//:cont1
           YCO= pieceby[PRECED]+Ay;
           XCO=blockxco;
           OPACITY=ORA;
           addback();
        }else{
            IMAGE= blockb[0];
            YCO= pieceby[PRECED]+Ay;
           XCO=blockxco;
           OPACITY=ORA;
           addback();
        }


    }
    else if(pieceb[PRECED]==0){//:stripe * Add stripe (palace bg set only)

        if(BGset1!=1){return;} //pal
        else if(bstripe[PRECED]==0){return;}//str1
             
             else{
                IMAGE=bstripe[PRECED];
                carry=true;
                YCO= Ay-32;
                XCO=blockxco;
                OPACITY=ORA;
                addback();
             }




    } else if(objid==panelb0){ //:panel * Special panel handling
        if(spreced>=numpans){return;}
        else if (panelb[spreced] !=0){
            IMAGE= panelb[spreced];
            YCO=pieceby[PRECED]+Ay;
            XCO=blockxco;
            OPACITY=ORA;
            addback();
        }else{return;}

    } else{//* draw regular B-section :cont1
        IMAGE=pieceb[PRECED];
        YCO=pieceby[PRECED]+Ay;
        XCO=blockxco;
        OPACITY=ORA;
        addback();

    }


 }


/*********************************************
 *      D R A W   G A T E   " C "
 **********************************************/

 void drawgatec(int colno){
    YCO=Dy;
    IMAGE=gatecmask;
    OPACITY=and;
    addback();
    if(SBELOW[colno]<gmaxval){

        gateposn=SBELOW[colno]/4;

        int y = -(gateposn & 0xF8);  

        IMAGE= gate8c[gateposn+y];

        OPACITY=ORA;

        addback();

    }

    gateposn=gmaxval/4;

    int negation = -(gateposn & 0xF8);  

    IMAGE= gate8c[gateposn+negation];

    OPACITY=ORA;

    addback();

 }

 /*********************************************
 *       drawmc, D R A W  M O V A B L E  ' C '
 **********************************************/

void drawmc(int colno){
    if(objid==space||objid==panelwof||objid==pillartop){//;is there a piece here?

       if( BELOW[colno]!=gate) return;
       //that piece is a gate
       drawgatec(colno);

    }
    return; //sera couvert par la section A


}


 /****************************************
 *         DRAWC et ses 3 fonctions
 ****************************************/

//*  Mask B-section of piece to left
 void domaskb(){
   if(maskb[PRECED]==0) return;
   IMAGE= maskb[PRECED];

   YCO=Dy;
   OPACITY= and;
   addback();

 }

//Draw C-section of piece below & to left
void dodrawc(int colno){


    if(BELOW[colno]== block) {//objid of piece below & to left
       if( SBELOW[colno]<numblox){//block ou pas block, l'ajouter si c'est le cas 
       IMAGE= blockc[SBELOW[colno]];
       XCO=blockxco;
       YCO=Dy;
       OPACITY= ORA;
       addback();

       }

    } 
    else{
        if(piecec[BELOW[colno]]==0) return;//pas de section c
        if(piecec[BELOW[colno]]==panelc0){
            //* Special panel handling
           if(SBELOW[colno]>=numpans ){ //num of different panel
            return;

        }else{
           if(panelc[SBELOW[colno]]!=0){
            IMAGE= panelc[SBELOW[colno]];
            XCO=blockxco;
            YCO=Dy;
            OPACITY= ORA;
            addback();
            
           }
           else return;
        }
    } 

    IMAGE=piecec[BELOW[colno]];
    XCO=blockxco;
    YCO=Dy;
    OPACITY= ORA;
    addback();
}
}

void checkc(){
    if(objid==0||objid==pillartop||
        objid==panelwof||
        objid>=archtop1){//section C visible
        carry=true;
        return;
    }
    else{ carry=false;
    return;
}

}



void drawc(int colno){
    checkc();
    if(carry== false) return;
    dodrawc(colno);
    domaskb();

}
/****************************************
 *         GETBELOW
 ****************************************/



/*ON ne gere que les cas avec l'ecran du bas null*/
void getbelow(int rowno){

    // printf("GETBELOW");
    // if(rowno<2){//* "ONSCREEN": Looking below top or middle row
    //     BELOW[0]= PREV[rowno+1];
    //     SBELOW[0]= sprev[rowno+1];
    //     carry=false;
    //     int  y=yindex;
    //     for(int xsave=1;xsave<10;xsave++){
           
            
    //         BELOW[xsave]=getobjid(y+10);//ce que va renvoyer getobjid
    //         SBELOW[xsave]= state;
    //         y++;
    //     }
    //     return;
    
    // }
    //null screen
    for(int i= 1;i<10;i++){

        BELOW[i]=1;
    }

    if(level==12){BELOW[0]= space;}
    else{ BELOW[0]=block;}


}

/****************************************
 *         GetPREV
 ****************************************/

 /*Pour l'instant, on cherche à dessiner les écran indépendemment des screens droite/gauche/bas/haut
 Donc on ne traduit que la partie de getprev qui gere le cas avec le screen d'avant null*/

 void getprev(){ 
    printf("fin getprev");

    PREV[0]=block; 
    PREV[1]=block;
    PREV[2]=block;
    sprev[0] =0;
    sprev[1] =0;
    sprev[2] =0;

 }
/****************************************
 *         GONULL
 ****************************************/

int Gonull(){
    int inmenu=0;
    if(inmenu==0){return getobjbldr();}
    return space;

}


/****************************************
 *         GETOBJID1
 ****************************************/

int getobjid1(int yindex){ //implanter linkmap
   
   if(inbuilder!=0) return getobjbldr();

   
    state= BlueSpec[2][yindex];

    if((BlueType[2][yindex]&idmask)== PRESSPLATE){
         if((LINKMAP[state]&idmask)<2){
            return PRESSPLATE;
            } 
         return dpressplate;
        }else if((BlueType[2][yindex]&idmask)== upressplate){
            if((LINKMAP[state]&idmask) < 2){
                 return upressplate;

        }
         //state=0;
         //return my_floor;
        }
        return BlueType[2][yindex]&idmask;
}



/****************************************
 *         GETINITOBJ1
 ****************************************/

int getinitobj1(){
    if((BlueType[1][yindex]&idmask)==gate){
      carry=true;
      return   initsettings[BlueSpec[1][yindex]-1];
    

    }
    else if((BlueType[1][yindex]&idmask)==loose){
        return 0;
    }
    else if((BlueType[1][yindex]&idmask)==flask){
        carry=true;
        return BlueSpec[1][yindex]*32;

    }

    return 0;

}

/****************************************
 *         GETOBJBLDR
 ****************************************/

 int getobjbldr(){
    pile= BlueType[1][yindex]&idmask;//recuperer apres avec pla
    state =getinitobj1();
    if(carry!=true) state= BlueSpec[1][yindex];

    return pile;

}
/****************************************
 *         GETLOOSEY
 ****************************************/

int getloosey(int accu){
   
    if(accu>=0) return state;

    if((state&0x7F)<Ffalling+1) { return state&0x7F; }
    else{ return 1;}
       
   

}


 /****************************************
 *         DRAWLOOSED
 ****************************************/

 void drawloosed(){
    int y= getloosey(state);
    if(loosed[y]==0) return;
    IMAGE= loosed[y];

    XCO=blockxco;
    YCO=Dy;
    OPACITY=STA;

    addback();
}


/****************************************
 *         DRAWD
 ****************************************/

 void cont1(unsigned char blockd){
    IMAGE=blockd;
    XCO=blockxco;
    YCO=Dy;
    addback();

}

void drawd(){

    OPACITY= STA;
    if(objid==block){

        if (state<numblox){ cont1(blockd[state]);
        }
        else{cont1(blockd[0]);
            }

    }
    else if (objid!=panelwof){
        if(pieced[objid]==0){
            return;
        }
    }
    OPACITY=ORA;
    if(pieced[objid]==0){}
    IMAGE= pieced[objid];
    XCO= blockxco;
    YCO=Dy;
    addback();
    

}




/****************************************
 *         DRAWMD
 ****************************************/

void drawmd(){
    if(objid==loose) return;
    drawloosed();

}

/****************************************
 *         REDBLOCKSURE
 ****************************************/

void RedBlockSure(int rowno,int colno){
     drawc(colno) ;//C-section of piece below & to left
     drawmc(colno);
  
     drawb() ;//B-section of piece to left
     drawmb(colno);
  
     drawd() ;//D-section
     drawmd();

    drawa() ;//A-section
    drawma();
    drawfrnt(rowno,colno) ;//A-section frontpiece (Note: This is necessary in case we do a layersave before we get to f.g. plane)
  

}

/****************************************
 *         GETOBJID
 ****************************************/


int getobjid(int yindex){
    if(SCRNUM==0) return Gonull();
    return getobjid1(yindex);
}


/****************************************
 *         SETBACK
 ****************************************/


void setback(){
    printf("je suis dans SETBACK\n");
    addback();// pas vraiment, ptr de fonction vers add back pour la prochaine fois qu'on appelle add
}

/****************************************
 *         SURE
 ****************************************/

void sure(){
   
    for(int y=0; y<192;y++){
        for(int x=0;x<280; x++){
            BASE[y][x]=0;
        }
    }
    printf("je suis dans SURE\n");
    genCLS=1;//clear screen
   
    setback();
   
    getprev(); //get 3 rightmost blocks of screen to left Pas besoin pour l'instant
   
    
    calcblue("LEVEL1");//get blueprint base addr en fonction de SCRNUM
   
    /* Draw 3 rows of 10 blocks (L-R, T-B)*/
   
       for(int rowno=2;rowno>=0;rowno--){  //0 = top row, 2 = bottom row

        printf("boucle SURE");
           Dy= BlockBot[rowno+1]; //get Y-coord for bottom of D-section
           Ay=Dy-3; //& A-section
   
           yindex=Mult10[rowno]; //block # (0-29)
           PRECED= PREV[rowno];
           spreced= sprev[rowno];//pour recuperer l'etat du block d'avant
   
           getbelow(rowno);
       
              
           for(int colno=0;colno<10;colno++){//0 = leftmost column, 9 = rightmost
          
               XCO= colno*4;
               blockxco= colno*4; //get X-coord for A-section
               objid =getobjid(yindex);// car notre SCRNUM!=0
   
                RedBlockSure(rowno,colno);//Redraw entire block,colno pour dodrawc
   
                PRECED= objid;
                spreced= state;
   
                yindex++;
   
           }
   
       //reste dessiner les parties des autres screens
    }
    }