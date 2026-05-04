
//#include "Global.h"

#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "hires.h"
#include "eq.h"
#include "HRtables.h"
#include "bgdata.h"
#include "frameadv2.h"



/****************************************
 *         DECLARATIONS
 ****************************************/

unsigned char *image_data;

void fastlay(SDL_Window *window, SDL_Renderer *renderer, SDL_Surface* screenshot);
unsigned char *setimage(void);

/****************************************
 *         FASTLAY
 ****************************************/


 void fastlay(SDL_Window *window,SDL_Renderer *renderer,SDL_Surface*  screenshot){
    printf("je suis dans FASTLAY\n");
    //passer de la memoir aux à la RAM


    unsigned char *image_data = setimage(); // renvoi donné de l'image 

    //if(OPACITY==STA) fastlaySTA();
    int smod= OPACITY&0x7F;

    int smXCO=XCO*7;
    int smYCO= YCO;
    int smWIDTH= image_data[0]; //largeur de l'image 
    int smTOP= image_data[1];// hauteur de l'image 


    int pos_ligne = 2+ ((smTOP - 1) * smWIDTH);




        /*DESSINER L'IMAGE */

        // Pour chaque ligne de l'image
        for (int line_num = smTOP - 1; line_num >= 0; line_num--) {

         if(smYCO<192&&smYCO>=0){//modifier pour le clipping 
          
            // Pour chaque octet de la ligne
            for (int byte = 0; byte < smWIDTH; byte++) {
                unsigned char pixel = image_data[pos_ligne];
                pos_ligne = pos_ligne + 1;
            


                if(smod==2 || smod==5){ //Opacity sta

                    // Pour chaque bit dans l'octet
                    for (int bit = 0; bit < 7; bit++, smXCO++) {
                        
                        if (pixel & (1 << (bit))) {
                         
                            // Dessiner le pixel sur la surface (blanc pour pixel allumé )
                            SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                            SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 255, 255, 255));
                            BASE[smYCO][smXCO]=1;
                            
                        } else {
                            // Dessiner le pixel éteint (noir)
                            SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                            SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 0, 0, 0));
                            BASE[smYCO][smXCO]=0;
                        }
                    }
                 }

                if(smod==0|| smod==4){//Opacity and
                  //  printf(" smYCO %d, smXCO %d , smod %d\n ", smYCO,smXCO,smod);

                     // Pour chaque bit dans l'octet
                    for (int bit = 0; bit < 7; bit++, smXCO++) {
                        if ((pixel & (1 << (bit)))&BASE[smYCO][smXCO]) {
                            // Dessiner le pixel sur la surface (blanc pour pixel allumé )
                            SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                            SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 255, 255, 255));
                           
                            BASE[smYCO][smXCO]=1;
                        } else {
                            // Dessiner le pixel éteint (noir)
                            SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                            SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 0, 0, 0));
                           
                            BASE[smYCO][smXCO]=0;
                        }
                    }

                }

                if(smod==1 ){//or
                   // printf(" smYCO %d, smXCO %d , smod %d\n ", smYCO,smXCO,smod);


                     // Pour chaque bit dans l'octet
                   for (int bit = 0; bit < 7; bit++, smXCO++) {
                        if ((pixel & (1 << (bit)))|(BASE[smYCO][smXCO])) {
                            // Dessiner le pixel sur la surface (blanc pour pixel allumé )
                            SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                            SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 255, 255, 255));
                           
                            BASE[smYCO][smXCO]=1;
                        } else {
                            // Dessiner le pixel éteint (noir)
                            SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                            SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 0, 0, 0));
                           
                            BASE[smYCO][smXCO]=0;
                        }
                     }

                }

                if(smod==3 ){//xor

                    // Pour chaque bit dans l'octet
                    for (int bit = 0; bit < 7; bit++, smXCO++) {
                       if ((pixel & (1 << (bit)))^(BASE[smYCO][smXCO])) {
                           // Dessiner le pixel sur la surface (blanc pour pixel allumé )
                           SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                           SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 255, 255, 255));
                           
                           BASE[smYCO][smXCO]=1;
                       } else {
                           // Dessiner le pixel éteint (noir)
                           SDL_Rect rect = { smXCO, smYCO, 1, 1 };
                           SDL_FillRect(screenshot, &rect, SDL_MapRGB(screenshot->format, 0, 0, 0));
                          
                           BASE[smYCO][smXCO]=0;
                       }
                   }

                }
            }
            }
            pos_ligne = pos_ligne - (2 * smWIDTH);
            smYCO++;
            smXCO=XCO*7;

           

        }
    
           
          // Mise à jour de la fenêtre SDL avec la surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, screenshot);
    if (texture == NULL) {
        printf("Erreur lors de la création de la texture: %s\n", SDL_GetError());
        return;
     }




    // Effacer le renderer
    SDL_RenderClear(renderer);

    // Dessiner la texture
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Mettre à jour l'affichage
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture); // Libérer la texture
    SDL_Delay(200);


   
   
}



/****************************************
 *         SETIAGE
 ****************************************/
unsigned char *setimage(){

    unsigned short bit_pds_faible;// première partie de l'adresse
    unsigned short bit_pds_fort; //2eme partie 
    unsigned short pos_temp;// combinaison bit poids fort et faible
    unsigned short pos_final; // soustraction pos_temp - adresse ou est chargé le fichier
    
    unsigned char pixel;
    
    int nb_oct_par_ligne=0; 
    int nb_lines =0;     // Nombre de lignes
    
    
        printf("image num  %u \n" ,IMAGE&0x7F);
        bit_pds_faible= Table[2*(IMAGE&0x7F) -1];
        bit_pds_fort= Table[2*(IMAGE&0x7F) ];
        printf("IMAGE %d\n",IMAGE&0x7F);
        
    
    
      //  printf("bits de poids faible: %02X\n", bit_pds_faible);
        //printf("bits de poids fort:  %02X\n",bit_pds_fort);
    
        pos_temp = (bit_pds_fort << 8) | bit_pds_faible; //combiner les 2 adresse
    
        //printf("position temp vant soustraction: %02X\n", pos_temp);
    
        pos_final=  pos_temp - 0x6000; 
    
       // printf("pos final en hexa %04X\n", pos_final);
        //printf("pos final en deci %u\n", pos_final);
    
    // lire nombre octets par lgnes et nb lignes
        nb_oct_par_ligne= Table[pos_final];
        nb_lines= Table[pos_final+1];
       // printf("nb_oct_par_ligne %u\n", nb_oct_par_ligne);
       // printf(" nb_lines %u\n", nb_lines);
       // printf("fichier[pos_final] %u\n",Table[pos_final]);
     
        image_data= (unsigned char *)malloc((nb_lines*nb_oct_par_ligne+2));
    
        memcpy(image_data,Table+ pos_final,nb_oct_par_ligne*nb_lines+2);
    
        return image_data;
    
    }
    