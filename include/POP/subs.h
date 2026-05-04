#ifndef SUBS_H
#define SUBS_H

/*
 * subs.h
 * Traduction de SUBS.S : Sous-routines de gameplay et scènes cinématiques
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $e000 (main LC bank 1) sur Apple II.
 *
 * SUBS.S est le module le plus hétérogène du projet. Il contient :
 *  1. Des routines de gameplay appelées chaque frame (gravity, addfall,
 *     startkid, addslicers, addtorches, removeobj, setinitials, ...)
 *  2. Toute la machinerie des scènes cinématiques (princess cuts) :
 *     boucle play, routines start* de positionnement, PlayCut0-8
 *  3. Des utilitaires d'affichage (pageflip, doflashon/off, lrclse, pause)
 *  4. La démo automatique (DEMO, DemoProg1)
 *  5. La gestion du temps (showtime, keeptime, getminleft)
 *
 * Pour la traduction C, les sections 2 et 4 (cinématiques et démo)
 * peuvent être traduites de façon moins fidèle car elles dépendent
 * fortement du hardware Apple II (synchronisation VBL, lecteur disque).
 */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"
#include "seqdata.h"
#include "movedata.h"
#include "soundnames.h"

/* -----------------------------------------------------------------------
 * CONSTANTES
 * ----------------------------------------------------------------------- */

/* Physique */
#define TERM_VELOCITY    33   /* vitesse terminale de chute */
#define ACCEL_GRAVITY     3   /* accélération gravitationnelle (pixels/frame²) */
#define WTLESS_TERMVEL    4   /* vitesse terminale sous légèreté */
#define WTLESS_GRAVITY    1   /* accélération sous légèreté */

/* Timer */
#define MIN_FRAMES    1090    /* frames par "minute" de jeu (~11 fps → ~99s) */
#define SEC_FRAMES      (MIN_FRAMES / 60)
#define TIME_LIMIT        60  /* limite de temps en "minutes" de jeu */
#define TIMEMSGTIMER      20  /* durée d'affichage du message de temps */

/* Trancheuses */
#define SLICETIMER        15  /* durée d'un cycle de trancheuse */
#define SLICERSYNC         3  /* décalage de phase entre trancheuses */

/* Miroir */
#define MIRSCRN_SUBS       4
#define MIRX_SUBS          4
#define MIRY_SUBS          0

/* Messages */
#define LEVELMSG_S  1
#define CONTMSG_S   2
#define TIMEMSG_S   3

/* Codes de commandes de la démo */
#define ENDPROG    (-2)
#define ENDDEMO    (-1)
#define CTR          0    /* joystick centré */
#define FWD          1    /* joystick avant */
#define BACK         2    /* joystick arrière */
#define UP           3    /* joystick haut */
#define DOWN_CMD     4    /* joystick bas (DOWN est réservé par movedata) */
#define UPFWD        5    /* haut + avant */
#define PRESS        6    /* bouton appuyé */
#define RELEASE      7    /* bouton relâché */

/* Y de sol de la salle de la princesse */
#define FLOOR_Y_PRINCESS  151


/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES : Jump table à $e000
 * ----------------------------------------------------------------------- */

/* --- Torches et objets --- */

/*
 * ADDTORCHES : Ajoute toutes les torches, flacons et épées de VisScrn
 * à la liste de transition (trans list), pour qu'ils soient animés.
 * Parcourt les 30 blocs de VisScrn et appelle trigtorch/trigflask/trigsword.
 */
void ADDTORCHES(void);

/* --- Affichage --- */

/*
 * DOFLASHON : Affiche un flash lo-res coloré (lightning).
 * Efface l'écran lo-res avec la couleur lightcolor, attend VBL,
 * bascule en mode lo-res. En C : effet SDL2 (overlay coloré).
 */
void DOFLASHON(void);

/*
 * DOFLASHOFF : Revient en mode hi-res après le flash.
 * Attend VBL, rebascule sur la bonne page hi-res.
 * En C : supprime l'overlay coloré.
 */
void DOFLASHOFF(void);

/*
 * PAGEFLIP : Affiche la page cachée et bascule PAGE.
 *
 * Apple II : alterne entre page 1 ($2000) et page 2 ($4000).
 * En C : SDL_RenderPresent() ou équivalent.
 * Gère aussi les "vibes" (double-buffering décalé pour l'effet
 * de vibration de la potion de légèreté).
 */
void PAGEFLIP(void);

/*
 * LRCLSE : Efface l'écran lo-res seulement si la couleur a changé.
 * In: A = nouvelle couleur (byte value)
 * Optimisation : compare avec scrncolor et ne fait rien si identique.
 */
void LRCLSE(uint8_t color);

/*
 * PAUSE : Boucle d'attente active.
 * In: A = durée (1-256, où 256 est codé comme 0 en uint8_t)
 * En C : SDL_Delay() ou boucle équivalente.
 */
void PAUSE(uint8_t duration);

/* --- Scènes cinématiques --- */

/*
 * PLAYCUT : Joue une scène cinématique pré-enregistrée.
 * In: A = numéro de scène (0-8)
 * Appelle PlayCut0 à PlayCut8 via table d'adresses self-modifiante.
 * En C : switch/case ou table de pointeurs de fonctions.
 */
void PLAYCUT(uint8_t cut_num);

/* Scènes individuelles (appelées via PLAYCUT) */
void PlayCut0(void);   /* scène d'ouverture : princesse et vizir */
void PlayCut1(void);   /* princesse debout */
void PlayCut2(void);   /* princesse allongée */
void PlayCut3(void);   /* alias de PlayCut1 */
void PlayCut4(void);   /* souris revient vers la princesse */
void PlayCut5(void);   /* princesse entend quelque chose */
void PlayCut6(void);   /* fin tragique (temps écoulé) */
void PlayCut7(void);   /* happy ending */
void PlayCut8(void);   /* princesse envoie la souris */

/* --- Sons --- */

/*
 * ADDLOWERSOUND : Ajoute le son d'un portail qui se ferme,
 * seulement si le portail est visible à l'écran.
 * In: A = état du portail (pair = pas de son, impair = son)
 */
void ADDLOWERSOUND(uint8_t state);

/* --- Objets --- */

/*
 * REMOVEOBJ : Retire un objet du blueprint (remplace par floor).
 *
 * In: A = type de potion (lastpotion) : négatif = épée, 1-5 = potions
 * Met le bloc courant (BlueType),y à "floor", BlueSpec à 0,
 * marque le bloc pour redraw (markwipe + markred).
 * Appelé depuis PickItUp (CTRL.S).
 */
void REMOVEOBJ(int8_t lastpotion_type);

/* --- Physique --- */

/*
 * ADDFALL : Applique la vitesse de chute à la position du personnage.
 *
 * CharY += CharYVel
 * Si CharAction == 4 (freefall) : applique aussi CharXVel via addcharx,
 * puis appelle rereadblocks().
 *
 * Appelé à chaque frame dans DoKid/DoShad après GRAVITY.
 */
void ADDFALL(void);

/*
 * GRAVITY : Accélère la chute si CharAction == 4 (freefall).
 *
 * Normal    : CharYVel += AccelGravity (3), plafonné à TermVelocity (33)
 * Légèreté  : CharYVel += WtlessGravity (1), plafonné à WtlessTermVel (4)
 *
 * Note : "vers le bas" = CharYVel positif (Y croît vers le bas sur Apple II).
 * Appelé à chaque frame dans DoKid/DoShad avant ADDFALL.
 */
void GRAVITY(void);

/* --- Initialisation --- */

/*
 * SETINITIALS : Définit l'état initial de tous les gadgets du niveau.
 *
 * Parcourt tous les écrans du niveau (INFO screens) et appelle
 * getinitobj() pour chaque bloc. Écrit l'état initial dans BlueSpec.
 * Appelé une fois depuis RESTART avant de démarrer le niveau.
 */
void SETINITIALS(void);

/*
 * STARTKID : Place le joueur à sa position de départ pour ce niveau.
 *
 * Lit KidStartScrn, KidStartBlock, KidStartFace depuis le blueprint (INFO).
 * Gère les cas spéciaux : level 1 (pas d'épée, stepfall), level 3
 * (milestone), level 13 (running start).
 * Appelle STARTKID1 pour initialiser les variables communes.
 */
void STARTKID(void);

/*
 * STARTKID1 : Fin de l'initialisation du joueur (appelable séparément).
 *
 * Aligne CharY sur FloorY[CharBlockY+1].
 * Remet à zéro : CharXVel, CharYVel, weightless, invert, jarabove,
 *   droppedout, CharSword, offguard, waitingtojump.
 * CharLife = 0xFF (vivant), CharID = 0 (kid).
 * Appelle animchar() pour obtenir le premier frame.
 */
void STARTKID1(void);

/*
 * INITIALGUARDS : Calcule les positions X initiales de tous les gardes
 * pour le niveau courant (GdStartX[]).
 *
 * Parcourt les 24 écrans possibles. Pour chaque garde dont GdStartBlock
 * est valide (< 30), calcule la coordonnée X depuis getblockej + angle + 7.
 * Appelé une fois depuis RESTART.
 */
void INITIALGUARDS(void);

/*
 * MIRAPPEAR : Fait apparaître le miroir dans le blueprint (level 4).
 *
 * Appelé par MOVER quand l'exit est ouvert sur level 4.
 * Remplace le bloc (MIRSCRN, MIRX, MIRY) par "mirror".
 */
void MIRAPPEAR(void);

/* --- Trancheuses --- */

/*
 * ADDSLICERS : Ajoute toutes les trancheuses de CharBlockY au niveau courant
 * à la liste de transition, en les synchronisant.
 *
 * Gère le timer de chaque trancheuse : ne re-déclenche pas une trancheuse
 * qui est en mid-slice (état < slicerRet).
 * Chaque trancheuse suivante est décalée de SLICERSYNC frames.
 */
void ADDSLICERS(void);

/* --- Divers --- */

/*
 * CRUMBLE : Sur level 13 : déclenche l'effondrement des planchers
 * instables au-dessus des écrans 23 et 16 à l'entrée du joueur.
 */
void CRUMBLE(void);

/*
 * DEADENEMY : Réaction à la mort de l'adversaire.
 *
 * Level 0 (démo) : milestone=1 (démarre partie 2 de la démo)
 * Level 13 (vizir) : lance la musique de victoire, ouvre l'exit,
 *   pousse la plaque de pression, flashs blancs
 * Autres : joue s_Vict (musique de victoire)
 */
void DEADENEMY(void);

/*
 * SHOWTIME : Affiche le temps restant si timerequest != 0.
 *
 * Gère le compte à rebours de la dernière minute (affichage secondes).
 * Met message=TimeMsg et msgtimer pour déclencher l'affichage.
 */
void SHOWTIME(void);

/*
 * DEMO : Contrôle le joueur pendant la démo automatique.
 *
 * Appelé depuis PLAYERCTRL (CTRL.S) quand level==0.
 * Lit DemoProg1 (table de commandes avec timestamps) via AutoPlayback.
 */
void DEMO(void);


/* -----------------------------------------------------------------------
 * BOUCLE DE JEU DES SCÈNES CINÉMATIQUES
 * ----------------------------------------------------------------------- */

/*
 * play : Boucle de playback pour les scènes (version simplifiée de MainLoop).
 *
 * In: A = nombre de frames à jouer
 * Appelle dans l'ordre : rnd, pause(SPEED), strobe, NextFrame, flashon,
 * FrameAdv, flashoff, playback, zerosound.
 * Interrompue par bouton ou touche (en dehors du level 0).
 *
 * Note : NextFrame et FrameAdv dans SUBS.S sont des versions allégées
 * spécifiques aux cinématiques (DoKid/DoShad sans physique complète).
 */
void play(uint8_t frames);


/* -----------------------------------------------------------------------
 * UTILITAIRES DE SCÈNES : Positionnement des personnages
 *
 * Ces routines placent les personnages dans leur position de départ
 * pour chaque scène cinématique. "P" = princess (Shad), "K" = kid,
 * "V" = vizier (Kid), "M" = mouse (Kid).
 * ----------------------------------------------------------------------- */
void startP0(void);  /* princesse position neutre */
void startP1(void);  /* princesse debout, face droite */
void startP2(void);  /* princesse allongée */
void startP4(void);  /* princesse attend la souris */
void startP5(void);  /* princesse alerte */
void startP7(void);  /* princesse attend le prince */
void startP8(void);  /* princesse caresse la souris */
void startV0(void);  /* vizier entre en scène */
void startK7(void);  /* joueur entre en scène (happy ending) */
void startM4(void);  /* souris court vers la droite */
void startM7(void);  /* souris au sol */
void startM8(void);  /* souris à mi-chemin */

/* jumpseq pour personnages de scène */
void pjumpseq(uint8_t seq);  /* pour la princesse (Shad) */
void vjumpseq(uint8_t seq);  /* pour le vizier/kid/souris (Kid) */
void mjumpseq(uint8_t seq);  /* alias de vjumpseq */
void kjumpseq(uint8_t seq);  /* alias de vjumpseq */


/* -----------------------------------------------------------------------
 * MUSIQUE DES SCÈNES
 * ----------------------------------------------------------------------- */

/*
 * PlaySong : Joue une chanson dans la salle de la princesse.
 * In: A = numéro de chanson
 * Interrompue par bouton ou touche.
 */
void PlaySong(uint8_t song_num);

/*
 * PlaySongX : Joue une chanson OU attend X frames si le son est coupé.
 * In: A = numéro de chanson, X = frames d'attente si son coupé
 */
void PlaySongX(uint8_t song_num, uint8_t silent_frames);

/*
 * PlaySongI : Comme PlaySongX mais une touche démarre une nouvelle partie.
 */
void PlaySongI(uint8_t song_num, uint8_t silent_frames);

/* Sablier */
void getglass(void);    /* In: FrameCount → Out: X = état du sablier (0-7) */
void addglass(void);    /* Ajoute le sablier à la scène (GlassState=X) */
void addglass1(uint8_t state); /* Ajoute le sablier avec état explicite */


/* -----------------------------------------------------------------------
 * TABLE DE COMMANDES DE LA DÉMO
 * ----------------------------------------------------------------------- */

/*
 * DemoProg1 : Séquence de commandes pré-enregistrées pour la démo.
 *
 * Format : paires (frame#, commande) terminées par ENDPROG ou ENDDEMO.
 * frame# = numéro de frame absolu depuis le début de la démo.
 * commande = CTR/FWD/BACK/UP/DOWN/UPFWD/PRESS/RELEASE
 */
extern const int8_t DemoProg1[];

#endif /* SUBS_H */
