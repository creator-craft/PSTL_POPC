#ifndef MOVEDATA_H
#define MOVEDATA_H

/*
 * movedata.h
 * Traduction de MOVEDATA.S — Identifiants des objets de niveau et constantes
 * Source originale : Jordan Mechner, 1989
 *
 * Ce fichier ne contient QUE des #define : aucune variable, aucune fonction.
 * Il est inclus dans tous les modules qui lisent ou comparent BLUETYPE/BLUESPEC.
 *
 * En assembleur, ces constantes sont résolues à l'assemblage (équations Merlin).
 * En C, les #define sont résolus à la compilation — comportement identique.
 */

/* -----------------------------------------------------------------------
 * IDENTIFIANTS DES OBJETS DE NIVEAU (objnames)
 *
 * Ces valeurs sont stockées dans BLUETYPE[] après masquage avec idmask (0x1F).
 * Elles correspondent aux entrées de BGTABLE dans les tables d'images.
 * Chaque bloc de 30 pixels sur l'écran a exactement un de ces identifiants.
 * ----------------------------------------------------------------------- */
#define space          0   /* vide / air */
#define floor_obj      1   /* sol (floor — "floor" est réservé en certains contextes) */
#define spikes         2   /* pics */
#define posts          3   /* colonnes */
#define gate           4   /* portail (mobile) */
#define dpressplate    5   /* plaque de pression vers le bas */
#define pressplate     6   /* plaque de pression vers le haut */
#define panelwif       7   /* panneau avec sol (w/ floor) */
#define pillarbottom   8   /* base de pilier */
#define pillartop      9   /* sommet de pilier */
#define flask         10   /* flacon / potion */
#define loose         11   /* plancher instable */
#define panelwof      12   /* panneau sans sol (w/o floor) */
#define mirror        13   /* miroir */
#define rubble        14   /* décombres (plancher effondré) */
#define upressplate   15   /* plaque de pression (variante haut) */
#define exit_obj      16   /* sortie (porte de sortie) */
#define exit2         17   /* sortie variante */
#define slicer        18   /* trancheuse */
#define torch         19   /* torche */
#define block_obj     20   /* bloc solide */
#define bones         21   /* ossements */
#define sword         22   /* épée au sol */
#define window_obj    23   /* fenêtre */
#define window2       24   /* fenêtre variante */
#define archbot       25   /* base d'arche */
#define archtop1      26   /* sommet d'arche type 1 */
#define archtop2      27   /* sommet d'arche type 2 */
#define archtop3      28   /* sommet d'arche type 3 */
#define archtop4      29   /* sommet d'arche type 4 */

/*
 * Note sur les noms conflictuels en C :
 *   "floor"  → renommé floor_obj  (floor() existe en <math.h>)
 *   "block"  → renommé block_obj  (peut conflictuer selon le contexte)
 *   "exit"   → renommé exit_obj   (exit() existe en <stdlib.h>)
 *   "window" → renommé window_obj (window peut être défini par SDL/GUI)
 *
 * Dans auto.c / frameadv.c, on peut définir des alias locaux si nécessaire :
 *   #define floor  floor_obj
 */


/* -----------------------------------------------------------------------
 * CONSTANTES D'ANIMATION DES OBJETS MOBILES
 * Valeurs issues de BGDATA.S, référencées ici pour usage dans MOVER.S.
 * ----------------------------------------------------------------------- */

/* Torche : indices de la dernière frame d'animation */
#define torchLast   17   /* dernière frame d'animation de la torche */
#define bubbLast     8   /* dernière frame des bulles */

/* Pics : indices de frame dans la séquence d'animation */
#define spikeExt     5   /* frame des pics en position étendue (sortis) */
#define spikeRet     9   /* frame des pics en position rétractée */

/* Trancheuse : indices de frame */
#define slicerExt    2   /* trancheuse en position étendue */
#define slicerRet    6   /* trancheuse en position rétractée */

/* Chute : première frame de l'animation "en chute" dans la séquence */
#define Ffalling    10


/* -----------------------------------------------------------------------
 * CONSTANTES DU JOYSTICK (moveparams)
 *
 * Limites de la valeur du joystick simulé.
 * gmaxval et gminval encadrent la plage de valeurs acceptées.
 * ----------------------------------------------------------------------- */
#define gmaxval   (47 * 4)   /* = 188 : valeur maximale du joystick */
#define gminval   0          /* valeur minimale */

#endif /* MOVEDATA_H */
