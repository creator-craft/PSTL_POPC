#ifndef HIRES_H
#define HIRES_H

/*
 * hires.h
 * Traduction de HIRES.S — Rendu bas-niveau sur le framebuffer hi-res
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $ee00 (main RAM) sur Apple II.
 *
 * HIRES.S est le seul module qui touche directement aux pixels.
 * En C/SDL2, toutes ces routines sont à réimplémenter en utilisant
 * SDL_RenderCopy, SDL_RenderDrawRect, ou un blitter custom selon
 * le cas. Le format des images et les paramètres d'appel restent
 * les mêmes — seul le backend de rendu change.
 *
 * -----------------------------------------------------------------------
 * FORMAT D'IMAGE (image table)
 *
 * Byte 0   : largeur en octets hi-res (1 octet = 7 pixels)
 * Byte 1   : hauteur en lignes
 * Byte 2-n : données image, lues gauche→droite, haut→bas
 *
 * Sur Apple II, chaque octet hi-res encode 7 pixels (bits 0-6) plus
 * un bit de sélection de palette couleur (bit 7). En SDL2, les images
 * sont pré-décodées en surface RGBA lors du chargement.
 *
 * -----------------------------------------------------------------------
 * PARAMÈTRES PASSÉS AUX ROUTINES DE RENDU
 * (variables globales — définies dans eq.h/gameeq.h)
 *
 *   PAGE      : page hi-res active (0x00 = page 1, 0x20 = page 2)
 *   XCO       : coordonnée X écran (0-39, en octets hi-res)
 *   YCO       : coordonnée Y écran (0-191)
 *   OFFSET    : décalage en bits (0-6) pour l'alignement sub-octet
 *   IMAGE     : numéro d'image dans la table (1-127), puis pointeur
 *               vers les données après appel à setimage()
 *   TABLE     : adresse de base de la table d'images (2 octets)
 *   BANK      : banque mémoire de la table (2=main, 3=aux)
 *   OPACITY   : bits 0-6 = mode de rendu (voir codes ci-dessous)
 *               bit 7 = miroir horizontal
 *   LEFTCUT   : bord gauche de la zone utile (0 = plein écran)
 *   RIGHTCUT  : bord droit + 1 (40 = plein écran)
 *   TOPCUT    : bord supérieur (0 = plein écran)
 *   BOTCUT    : bord inférieur + 1 (192 = plein écran)
 *
 * -----------------------------------------------------------------------
 * CODES D'OPACITÉ (OPACITY bits 0-6)
 */

#define OP_AND    0   /* AND  : combine avec le fond (masque) */
#define OP_ORA    1   /* OR   : superpose sur le fond */
#define OP_STA    2   /* STA  : écrase le fond (opaque) */
#define OP_EOR    3   /* EOR  : XOR spécial (OR + décalage + XOR) — effet ombre */
#define OP_MASK   4   /* MASK : mask/OR — image avec masque de transparence */

/* Bit 7 de OPACITY : miroir horizontal */
#define OP_MIRROR 0x80

#include <stdint.h>

/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES — Jump table à $ee00
 * ----------------------------------------------------------------------- */

/*
 * cls — Efface l'écran hi-res actif (PAGE) en noir (valeur $80 = black2).
 * En SDL2 : SDL_SetRenderDrawColor(0,0,0,255) + SDL_RenderClear()
 */
void cls(void);

/*
 * lay — Routine générale de rendu d'image.
 *
 * Gère : clipping, décalage de bits (OFFSET), miroir (bit 7 d'OPACITY),
 * tous les modes d'opacité. Appelle LayGen, LayMask ou LayXOR selon
 * OPACITY, ou MLAY si le bit miroir est actif.
 *
 * C'est la routine la plus complète mais aussi la plus lente.
 * Utilisée pour les sprites avec effets spéciaux (ombres, transparences).
 * En SDL2 : SDL_RenderCopyEx avec blend mode selon OPACITY.
 */
void lay(void);

/*
 * fastlay — Version allégée de LAY : pas d'offset, pas de clipping
 * (sauf clipping Y limité en haut), pas de miroir, pas de masque,
 * pas de XOR. STA uniquement ou mode configurable via OPACITY.
 *
 * Utilisée pour le rendu rapide du background.
 * En SDL2 : SDL_RenderCopy simple.
 */
void fastlay(void);

/*
 * layrsave — Sauvegarde le fond derrière un sprite dans le peel buffer
 * avant de le dessiner, pour pouvoir l'effacer proprement au frame suivant.
 *
 * In: mêmes paramètres que lay, plus PEELBUF (pointeur dans le peel buffer)
 * Out: PEELIMG, PEELXCO, PEELYCO mis à jour
 *
 * En SDL2 : SDL_RenderReadPixels sur la zone concernée.
 */
void layrsave(void);

/*
 * lrcls — Efface l'écran lo-res/texte (page 1) avec la couleur donnée.
 * In: A = couleur (byte value Apple II lo-res)
 * En SDL2 : overlay coloré semi-transparent pendant les transitions.
 */
void lrcls(uint8_t color);

/*
 * fastmask — Comme fastlay mais applique un masque de transparence
 * (AND avec MASKTAB) avant de dessiner. Utilisé pour les sprites
 * avec zones transparentes.
 * En SDL2 : SDL_SetTextureBlendMode + SDL_RenderCopy.
 */
void fastmask(void);

/*
 * fastblack — Efface un rectangle (width × height) en noir.
 * In: XCO, YCO, width, height, color (via variables globales)
 * En SDL2 : SDL_RenderFillRect.
 */
void fastblack(void);

/*
 * peel — "Décolle" un sprite en restaurant le fond sauvegardé par layrsave.
 * Lit PEELIMG, PEELXCO, PEELYCO et redessine le fond.
 * En SDL2 : SDL_RenderCopy avec la surface sauvegardée.
 */
void peel(void);

/*
 * getwidth — Retourne la largeur et la hauteur d'une image.
 * In: BANK, TABLE, IMAGE
 * Out: A = largeur (octets), X = hauteur (lignes)
 */
uint8_t getwidth(uint8_t *height_out);

/*
 * copyscrnMM — Copie $2000 octets de page hi-res vers une autre
 * (main→main). Utilisé pour dupliquer le fond entre les deux pages.
 * In: IMAGE = page source (hi byte), IMAGE+1 = page dest
 * En SDL2 : SDL_RenderCopy d'une texture vers une autre.
 */
void copyscrnMM(void);

/* copyscrnAA — Copie aux→aux */
void copyscrnAA(void);

/* copyscrnMA — Copie main→aux */
void copyscrnMA(void);

/* copyscrnAM — Copie aux→main */
void copyscrnAM(void);

/*
 * SETFASTAUX — Configure fastlay/fastmask pour lire les tables d'images
 * depuis la mémoire auxiliaire (auxmem). À appeler une fois au démarrage.
 * En C : inutile (les pointeurs sont directs).
 */
void SETFASTAUX(void);

/* SETFASTMAIN — Configure fastlay pour lire depuis la mémoire principale. */
void SETFASTMAIN(void);

/*
 * INVERTY — Inverse les tables YLO/YHI pour retourner l'écran
 * verticalement (effet potion "upside down").
 *
 * Échange les entrées [0..95] et [191..96] dans YLO et YHI.
 * En SDL2 : SDL_RenderCopyEx avec SDL_FLIP_VERTICAL, ou modification
 * de la matrice de transformation du renderer.
 */
void INVERTY(void);


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES (déclarées pour la traduction)
 * ----------------------------------------------------------------------- */

/*
 * setimage — Résout IMAGE (numéro) en IMAGE (pointeur) dans la table.
 * In: TABLE, IMAGE (numéro)
 * Out: IMAGE = adresse des données de l'image dans la table
 */
void setimage(void);

/*
 * PREPREP — Sauvegarde IMAGE/XCO/YCO, résout l'image, lit width/height.
 * Appelé en début de toutes les routines de rendu.
 */
void PREPREP(void);

/*
 * CROP — Calcule les paramètres de clipping selon LEFTCUT/RIGHTCUT/
 * TOPCUT/BOTCUT. Retourne -1 (carry set) si l'image est entièrement
 * hors écran.
 * Out: TOPEDGE, VISWIDTH, OFFLEFT, OFFRIGHT, RMOST, XCO, YCO ajustés
 */
int8_t CROP(void);

/* LayGen  — Rendu général AND/OR/STA (image normale) */
void LayGen(void);

/* LayMask — Rendu avec masque de transparence */
void LayMask(void);

/* LayXOR  — Rendu XOR spécial (OR + décalage d'1 bit + XOR) */
void LayXOR(void);

/* MLAY    — Versions miroir des trois routines ci-dessus */
void MLAY(void);
void MLayGen(void);
void MLayMask(void);
void MLayXOR(void);

#endif /* HIRES_H */
