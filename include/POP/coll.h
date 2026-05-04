#ifndef COLL_H
#define COLL_H

/*
 * coll.h
 * Traduction de COLL.S : Détection et résolution des collisions
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $4500 (aux mem) sur Apple II.
 *
 * Ce fichier gère deux types de collisions distincts :
 *   1. Collisions avec les barrières verticales (murs, portails,
 *      miroirs, trancheuses) : CHECKBARR / COLLISIONS
 *   2. Collision avec le sol (gérée dans CTRL.S : CHECKFLOOR)
 *
 * Le système de détection utilise des buffers de "collision data" (CD)
 * et "screen number" (SN) sur deux frames consécutifs pour détecter
 * le moment exact où le personnage franchit une barrière.
 */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"
#include "movedata.h"
#include "seqdata.h"
#include "soundnames.h"

/* -----------------------------------------------------------------------
 * CONSTANTES
 * ----------------------------------------------------------------------- */

/*
 * Distances en pixels depuis le bord gauche d'un bloc jusqu'à la
 * barrière (BarL) et depuis le bord droit jusqu'à la barrière (BarR).
 * Indexées par le code de barrière retourné par CMPBARR :
 *   0 = libre, 1 = panneau/portail, 2 = flacon, 3 = miroir/trancheuse,
 *   4 = bloc solide
 * Invariant : BarL[i] + BarR[i] + largeur_barriere == 14
 */
extern const uint8_t BarL[5]; /* = {0, 12, 2,  0, 0} */
extern const uint8_t BarR[5]; /* = {0,  0, 9, 11, 0} */

#define DEATH_VELOCITY_COLL  33
#define OOF_VELOCITY_COLL    22

/*
 * GATEMARGIN : marge en pixels ajoutée à l'ouverture du portail
 * pour décider s'il bloque le passage.
 * Plus élevé = portail plus généreux (le personnage passe plus facilement).
 */
#define GATEMARGIN  6


/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES : Jump table à $4500
 * ----------------------------------------------------------------------- */

/*
 * CHECKBARR : Détecte les collisions avec les barrières verticales.
 *
 * Remplit les buffers CD (collision data) et SN (screen number) pour
 * les blocs dans la plage [CDLeftEj .. CDRightEj] sur les trois niveaux
 * de blocs (BlockYthis, dessus, dessous).
 *
 * Compare les données du frame courant (CDthisframe) avec le frame
 * précédent (CDlastframe) pour détecter un franchissement.
 *
 * Out: collideL = numéro de bloc (0-9) si collision avec bord gauche
 *                 d'une barrière, 0xFF (-1) sinon
 *      collideR = idem pour bord droit
 *
 * Note : "bord gauche" signifie que le personnage heurte le côté gauche
 * de la barrière (il vient de la gauche). Le sens de la collision est
 * dans le référentiel de l'écran, pas du personnage.
 */
void CHECKBARR(void);

/*
 * COLLISIONS : Résout les collisions détectées par CHECKBARR.
 *
 * In: collideL, collideR (résultats de CHECKBARR)
 *
 * Si une collision est détectée et que le personnage n'est pas dans
 * un état exempt (grimpée, suspension), appelle leftcoll ou rightcoll
 * qui appellent à leur tour collide().
 *
 * Gère aussi l'AMtimer (antimatter timer) : si actif, ignore la
 * collision et émet un son.
 */
void COLLISIONS(void);

/*
 * GETFWDDIST : Distance disponible pour un "pas prudent" vers l'avant.
 *
 * Analyse les blocs devant le personnage et retourne :
 *   A = distance en pixels (0-14) jusqu'à l'obstacle ou le bord
 *   X = nature de l'obstacle : 0=bord (edge), 1=barrière, 2=libre
 *   + résultats RDBLOCK pour le bloc concerné
 *
 * Utilisé par DoStepfwd et DoStartrun dans CTRL.S pour calibrer
 * l'amplitude d'un pas.
 */
uint8_t GETFWDDIST(uint8_t *obstacle_type);

/*
 * CHECKCOLL : Vérifie si un bloc donné constitue réellement une collision.
 *
 * In: résultats RDBLOCK (A = objid)
 * Out: carry set = collision effective, carry clear = pas de collision
 *      (La distinction est faite selon le type de bloc : portail ouvert,
 *       trancheuse ouverte, miroir franchissable en courant, etc.)
 *
 * Gère les cas spéciaux :
 *  - Miroir : le joueur peut le traverser avec un saut en course (runjump)
 *             → appelle smashmirror() et positionne le shadowman
 *  - Trancheuse : ne bloque que si fermée (slicerExt)
 *  - Portail : ne bloque que si assez bas (voir gatebarr?)
 */
uint8_t CHECKCOLL(uint8_t objid); /* retourne 1 si collision, 0 sinon */

/*
 * ANIMCHAR : Avance d'un frame dans la séquence d'animation du personnage.
 *
 * Lit les octets de Fdef[CharSeq] via GETSEQ jusqu'à trouver un numéro
 * de frame (valeur positive non-instruction), en exécutant au passage
 * les instructions rencontrées :
 *
 *   chx        : lire delta-X, appeler addcharx → CharX
 *   chy        : lire delta-Y → CharY
 *   aboutface  : CharFace ^= 0xFF (demi-tour)
 *   goto       : lire 2 octets → CharSeq (branchement absolu dans Fdef)
 *   up         : dec CharBlockY + addslicers
 *   down       : inc CharBlockY + addslicers
 *   act        : lire octet → CharAction
 *   setfall    : lire 2 octets → CharXVel, CharYVel
 *   ifwtless   : si weightless != 0, goto (sinon sauter 2 octets)
 *   die        : ignoré (passage en revue)
 *   jaru       : jarabove = 1  (secoue planchers au-dessus)
 *   jard       : jarabove = -1 (secoue planchers en-dessous)
 *   tap        : son + alertguard=1 (0=alerte, 1=pas, 2=choc mur)
 *   nextlevel  : appelle GoneUpstairs() → inc NextLevel
 *   effect     : lire octet (1 = potioneffect)
 *
 * Quand une valeur positive non-instruction est trouvée :
 *   CharPosn = cette valeur  (numéro du frame courant)
 *   retour
 *
 * C'est LA routine centrale de l'animation. Elle est appelée par DoKid,
 * DoShad, hitflr, fallon, et toutes les routines de collision.
 */
void ANIMCHAR(void);

/*
 * CHECKSLICE : Détecte si le personnage (joueur) est coupé par une trancheuse.
 *
 * Utilise les buffers CD (CDthisframe) pour détecter un chevauchement
 * complet avec une trancheuse fermée (CDthisframe[x] == 0xFF).
 * Si coupé : aligne le personnage sur la trancheuse, décrémente la vie,
 * joue le son, déclenche la séquence "halve" (coupé en deux).
 *
 * In: Char data, CDthisframe[], SNthisframe[]
 */
void CHECKSLICE(void);

/*
 * CHECKSLICE2 : Version pour l'adversaire (pas de buffers CD).
 *
 * Vérifie directement les blocs sous les pieds et devant le personnage.
 * Utilisé pour les gardes qui n'ont pas de données CD complètes.
 */
void CHECKSLICE2(void);

/*
 * CHECKGATE : Si le personnage est debout sous un portail qui se ferme,
 * le projette latéralement.
 *
 * Actif seulement pour CharPosn=15 (debout), 108-110 (accroupi),
 * ou CharAction=7 (demi-tour).
 */
void CHECKGATE(void);

/*
 * ENEMYCOLL : Détection de collision simplifiée pour l'adversaire.
 *
 * Vérifie si l'adversaire en garde recule dans un mur ou un portail.
 * Appelle bumpengback si c'est le cas.
 * Ne s'applique que si CharAction=1 (au sol), CharLife vivant,
 * CharSword=2 (en garde), et AMtimer=0.
 */
void ENEMYCOLL(void);


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES (déclarées ici pour la traduction)
 * ----------------------------------------------------------------------- */

/*
 * Réinitialise les buffers CD/SN pour un nouveau frame.
 * Copie CDthisframe → CDlastframe selon le BlockY courant.
 */
void initCDbufs(void);

/*
 * Remplit CDthisframe et SNthisframe pour un niveau de blocs (blocky).
 * Appelée trois fois par CHECKBARR (niveau courant, dessus, dessous).
 */
void getCData(uint8_t sn_buf, uint8_t cd_buf);

/*
 * Retourne la coordonnée X du bord gauche de la barrière dans un bloc.
 * In: scrn/bx/by via rdblock, blockedge
 * Out: coordonnée X (140px), ou 255 si pas de barrière
 */
uint8_t getleftbar(uint8_t scrn, uint8_t bx, uint8_t by);

/*
 * Retourne la coordonnée X du bord droit de la barrière dans un bloc.
 * Out: coordonnée X, ou 0 si pas de barrière
 */
uint8_t getrightbar(uint8_t scrn, uint8_t bx, uint8_t by);

/*
 * Ajuste une coordonnée X de tempscrn vers VisScrn.
 * Si tempscrn est à gauche de VisScrn : soustrait ScrnWidth.
 * Si tempscrn est à droite             : ajoute ScrnWidth.
 */
uint8_t AdjustScrn(uint8_t x);

/*
 * Applique physiquement le déplacement suite à une collision.
 * In: A = distance barrière→personnage, X = direction (-1=gauche, 0=droite)
 * Distingue AirBump (dans les airs) de GroundBump (au sol).
 */
void collide(int8_t dist, int8_t direction);

/*
 * Distance en pixels entre le personnage et une barrière dans tempblockx.
 * Négatif si la barrière est derrière le personnage.
 */
int8_t DBarr(void);

/*
 * Version de DBarr pour ENEMYCOLL (sens inversé car l'ennemi recule).
 */
int8_t DBarr2(void);

/*
 * Le portail bloque-t-il le passage ?
 * Out: carry clear = bloque (fermé), carry set = laisse passer (ouvert)
 * Utilise BlueSpec du portail et imheight du personnage.
 */
uint8_t gatebarr_check(void); /* gatebarr? en assembleur */

/*
 * Personnage monté en haut des escaliers → incrémente NextLevel,
 * déclenche la musique de victoire.
 */
void GoneUpstairs(void);

#endif /* COLL_H */
