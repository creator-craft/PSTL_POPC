//#include "Global.h"

#include "hires.h"
#include "grafix.h"
#include "eq.h"
#include "gameeq.h"
#include "bgdata.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h> 




/****************************************
*    D R A W F O R E
*
*  Draw foreground list (using fastmask/fastlay)
 ****************************************/


 void DRAWFORE(SDL_Window *window, SDL_Renderer *renderer, SDL_Surface* screenshot){
    printf("je suis dans DRAWFORE\n");

    if(fgX[1]==999){
        printf("je suis RETURN\n");

        return;
    }

    for( int index=1; index<=maxback;index++){
        IMAGE= fgIMG[index];
        setbgimg(); //extraire BANK,TABLE,IMAGE pour hire call

        XCO= fgX[index];
        YCO=fgY[index];
        OPACITY= fgOP[index];

        //utiliser fastmask mais pas compris


        fastlay(window,renderer,screenshot);
    }



 }

/****************************************
*    *  ADDMID "E-Z" version
*
*  No offset, no mirroring, no cropping
*
*  In: XCO, YCO, IMAGE, TABLE, OPACITY
*      A = midTYP
*
 ****************************************/

void addmidezo(int us){
    int x;
    if(midX[maxmid-1]!=999) return;

    for(int i=1; i< maxback;i++){ 
    
        if (midX[i]==999){
           x=i;
           break;
           }
    }

    midTYP[x]=us;

    midX[x]=XCO;

    midOFF[x]=OFFSET;

    midY[x]=YCO;

    midIMG[x]=IMAGE;

    //midTAB[x]=TABLE;

    midOP[x]=OPACITY;
    midCU[x]=0;
    midCL[x]=0;
    midCR[x]=40;
    midCD[x]=192;

}


/****************************************
*        A D D F O R E
*
*  Add an image to FOREGROUND image list
*
*  In: same as ADDBACK
 *
 ****************************************/

 void addfore(){
    if(IMAGE==0) return;
  if(fgX[maxfore-1]!=999) return ;
    int x;
   for(int i=1; i< maxback;i++){ //equivalent du stx fgx pour avoir l'index de la derniere case rempli
    
     if (fgX[i]==999){
        x=i;
        break;
        }
   }

    fgX[x]=XCO;

    if(YCO>=192) return;
    fgY[x]=YCO;
    fgIMG[x]=IMAGE;

    fgOP[x]=OPACITY;

 }

/****************************************
 *        DRAWALL
 *
 ****************************************/


void drawall(){
    printf("je suis dans DRAWALL\n");
    DOGEN(); //Do general stuff like cls

     /*********************************
     *     *    SDL        *
     * *******************************/

 // Initialisation de SDL
     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return;
    }

    // Créer une fenêtre SDL
    SDL_Window *window = SDL_CreateWindow("Affichage Image avec SDL",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          39*7, 192, SDL_WINDOW_SHOWN);
            
     if (window == NULL) {
        printf("Erreur  %s\n", SDL_GetError());
        SDL_Quit();
     
        return ;
     }

     // Créer un renderer pour dessiner dans la fenêtre 
     SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
     if (renderer == NULL) {
        printf("Erreur  %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return ;
     }


        // Créer une SDL_Surface pour y dessiner l'image
        SDL_Surface*  screenshot = SDL_CreateRGBSurface(0, 39*7, 192, 32, 0, 0, 0, 0);
        if (!screenshot) {
            printf("Erreur  %s\n", SDL_GetError());
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return ;
        }

    /*********************************
     *        FIN SDL
     * 
     * *******************************/

    //if(blackflag!=0) ZEROPEEL();
  
   
   // SNGPEEL(); "Peel off" characters (using the peel list we set up 2 frames ago) pas besoin pour l'nstant
   
    //ZEROPEEL(); //Zero just-used peel list
   
    //DRAWWIPE() ;//Draw wipes
   
    DRAWBACK(window,renderer,screenshot) ;//Draw background plane images
   
    //DRAWMID() ;//Draw middle plane images (& save underlayers to now-clear peel list)
   
    DRAWFORE(window,renderer,screenshot) ;//Draw foreground plane images
   
   // DRAWMSG() ;//Draw mess

 }

/****************************************
 *         DRAWBACK
 * 
 ****************************************/

void DRAWBACK(SDL_Window *window, SDL_Renderer *renderer, SDL_Surface* screenshot){
    printf("je suis dans DRAWBACK\n");
    if(bgX[1]==999) return; // si la première case est vide alors il n'y a aucune image dans le tableau car rempli dans l'ordre 
    printf("JE RETOURNE");

   
        
    for( int index=1; index<=maxback&&bgX[index]!=999;index++){
        IMAGE= bgIMG[index];
        setbgimg(); //extraire BANK,TABLE,IMAGE pour hire call

        XCO= bgX[index];
        YCO=bgY[index];
        OPACITY= bgOP[index];

       fastlay(window,renderer,screenshot);
    }
    


 }
/****************************************
 *         SETBGIMG
 * 
 ****************************************/


void setbgimg_bis(char ImgTab[15]){

    printf("je suis dans SETIMGBIS\n");
    FILE *file;


    
    file = fopen(ImgTab, "rb");

    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        exit(1);
    }

    // Déplace l'offset à la fin du fichier
    fseek(file, 0, SEEK_END);

    // Renvoie la position de l'offset et donc la taille du fichier
    size_t file_size = ftell(file);

    if (file_size == -1) {
        perror("Erreur lors de la lecture de la taille du fichier");
        fclose(file);
        exit(1);
    }

    // Tableau qui va contenir le fichier
    Table= (unsigned char *)malloc(file_size * sizeof(unsigned char));

    if (Table == NULL) {
        perror("Erreur d'allocation de mémoire");
        fclose(file);
        exit(1);
    }

    // Offset remis au début du fichier pour charger le fichier
    rewind(file);

    // Charger le fichier dans le tableau
    size_t res = fread(Table, 1, file_size, file);

    if (res != file_size) {
        perror("Lecture du fichier échouée");
        free(Table);
        fclose(file);
        exit(1);
    }

    
    fclose(file);


   
}


void setbgimg(){ //retourner la bonne bgtable.
    printf("je suis dans SETBGIMG\n");
    BANK= 3; // aux mem
    

    if(IMAGE&0x80){ //Bit 7: 0 = bgtable1, 1 = bgtable2
        setbgimg_bis("IMG.BGTAB2.DUN");
        printf("BG2\n");
        
    }
    else{
        setbgimg_bis("IMG.BGTAB1.DUN");
        printf("BG1\n");


}
    

}


/****************************************
 *         ZERORED
 * 
 ****************************************/

void zerored(){
    printf("je suis dans ZERORED\n");
    for(int y=29;y>0; y--){
     fredbuf[y]= 0;
     floorbuf[y]=0;
     halfbuf[y]=0;
     movebuf[y]= 0;
     objbuf[y]= 0;   
    }

    for(int y=9;y>0; y--){
        topbuf[y]= 0;
    }
 }


/****************************************
 *         ZEROPEELS
 * 
 ****************************************/

void zeropeels(){
    peelX[0]=0;
    peelX[maxpeel]=0;
 }



/****************************************
 *         DOGEN
 ****************************************/


void DOGEN(){

    printf("je suis dans DOGEN\n");
    if(genCLS==0){
        if(BGset1!=1) return;
        dummy[BGset1-1]=0;

    }else{
        //cls(); //clear screen avec des pixels noir
    }


 }


/****************************************
 *         ZEROLSTS
 ****************************************/


void zerolsts(){//remet plusieurs variables à 0
    printf("je suis ds zerolsts\n");

 genCLS=0;
 //wipeX=0;


    for(int i=0; i< maxback;i++){
    bgX[i]= 999;
    }
    for(int i=0; i< maxmid;i++){
        midX[i]= 999;
        }
 //objX=0;
    for(int i=0; i< maxfore;i++){
        fgX[i]= 999;
    }
    
 //msgX=0;

}

/****************************************
 *         ADDBACK
 ****************************************/

void addback(){
    if(IMAGE==0) return;
    printf("je suis dans ADDBACK\n");
    int x=0;
   if(bgX[maxback-1]!=999) return ;

   for(int i=1; i< maxback;i++){
    if (bgX[i]==999){
        x=i;
        break;
    }
   }

   bgX[x]= XCO;
   if(YCO==192) return;

   bgY[x]= YCO;

   bgIMG[x]= IMAGE;
   bgOP[x]= OPACITY;

   printf("Fin\n");

}

/****************************************
 *         CALCLBLUE
 ****************************************/

/*retourne le debut du blutype et du bluespec*/
void calcblue(char ImgTab[7]){
    //if(SCRNUM==0) calcmenue() on s'en occupe pas maintenant
    FILE *file;


    
    file = fopen(ImgTab, "rb");

    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        exit(1);
    }

    // Déplace l'offset à la fin du fichier
    fseek(file, 0, SEEK_END);

    // Renvoie la position de l'offset et donc la taille du fichier
    size_t file_size = ftell(file);

    if (file_size == -1) {
        perror("Erreur lors de la lecture de la taille du fichier");
        fclose(file);
        exit(1);
    }

    // Tableau qui va contenir le fichier
    unsigned char *fichier = (unsigned char *)malloc(file_size * sizeof(unsigned char));

    if (fichier == NULL) {
        perror("Erreur d'allocation de mémoire");
        fclose(file);
        exit(1);
    }

    // Offset remis au début du fichier pour charger le fichier
    rewind(file);

    // Charger le fichier dans le tableau
    size_t res = fread(fichier, 1, file_size, file);

    if (res != file_size) {
        perror("Lecture du fichier échouée");
        free(fichier);
        fclose(file);
        exit(1);
    }

    
    fclose(file);


    BlueType=(unsigned char**)malloc(25*sizeof(unsigned char*)); // car 24 screens
    BlueSpec=(unsigned char**)malloc(25*sizeof(unsigned char*));
    LINKMAP= (unsigned char*)malloc(256*sizeof(unsigned char*));


    for(int i = 1; i <= 24; i++) {
        BlueType[i] = (unsigned char*)malloc(30 * sizeof(unsigned char));
    }

    for(int i = 1; i <= 24; i++) {
        BlueSpec[i] = (unsigned char*)malloc(30 * sizeof(unsigned char));
    }

    int octets=0;
    for(int i=1;i<=24;i++){
        for(int bp=0;bp<30;bp++){
        BlueType[i][bp]= fichier[octets++];
    
        }
    }

    printf("octets = %d\n",octets);
   
    for(int i=1;i<=24;i++){
        for(int bp=0;bp<30;bp++){
        BlueSpec[i][bp]= fichier[octets++];
    
        }
    }


    
    printf("octets = %d\n",octets);

    octets= octets + 256 ;//placer à la bonne position pour recuperer linkmap(skiplinkloc)

    for(int i=0; i<256;i++){
        LINKMAP[i]=fichier[octets++];

    }
    free(fichier);

    printf("calclblue fini\n");
  
   
}