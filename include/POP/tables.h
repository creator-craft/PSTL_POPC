#ifndef TABLES_H
#define TABLES_H

/*
 * tables.h
 * Traduction de TABLES.S : Tables de lookup statiques
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $e00 (main RAM) sur Apple II.
 *
 * Toutes ces tables sont des données constantes calculées à la
 * compilation. En C elles sont déclarées extern const et définies
 * dans tables.c (un seul fichier de définition).
 *
 * Conventions de coordonnées rappelées ici car elles gouvernent
 * toutes les tables :
 *   - X (140px) : 0 = bord gauche écran physique, 58 = bord gauche
 *                 de l'espace de jeu (ScrnLeft), 197 = bord droit
 *   - X (280px) : double de 140px, utilisé pour FCharX
 *   - Y (192px) : 0 = haut, 191 = bas (ScrnBot)
 *   - Blocs X   : 0-9 (de gauche à droite sur l'écran)
 *   - Blocs Y   : 0 = rangée du haut, 1 = milieu, 2 = bas
 *   - Un bloc fait 14 pixels de large, 63 pixels de haut
 */

#include <stdint.h>

/* -----------------------------------------------------------------------
 * CONSTANTES DE GÉOMÉTRIE
 * ----------------------------------------------------------------------- */

#define SCRN_LEFT      58   /* X du bord gauche de l'espace de jeu (140px) */
#define SCRN_TOP        0   /* Y du bord supérieur */
#define SCRN_BOT      191   /* Y du bord inférieur */

#define VERT_DIST      10   /* pixels du bas d'un bloc jusqu'au plan central */
#define BLOCK_HEIGHT   63   /* hauteur d'un bloc en pixels */
#define DHEIGHT         3   /* épaisseur d'un plancher */

/* Coordonnées Y du bas de chaque rangée de blocs */
#define BLOX1   (BLOCK_HEIGHT)
#define BLOX2   (2 * BLOCK_HEIGHT)
#define BLOX3   (3 * BLOCK_HEIGHT)
#define BLOX4   (4 * BLOCK_HEIGHT)


/* -----------------------------------------------------------------------
 * TABLES DE CONVERSION X → BLOC
 * ----------------------------------------------------------------------- */

/*
 * ByteTable[256]
 * Index  : coordonnée X réelle (0-255)
 * Retour : numéro d'octet hi-res (0-36)
 * (Utilisé pour l'adressage du framebuffer Apple II → inutile avec SDL2,
 *  conservé pour fidélité à l'original)
 */
extern const uint8_t ByteTable[256];

/*
 * OffsetTable[256]
 * Index  : même que ByteTable
 * Retour : offset dans l'octet (0-6)
 * (Utilisé pour le rendu hi-res Apple II → inutile avec SDL2)
 */
extern const uint8_t OffsetTable[256];

/*
 * BlockTable[256]
 * Index  : coordonnée X (0-255, système 140px mais stocké sur 256)
 * Retour : numéro de bloc (-5 à 14, cast en int8_t)
 *
 * La plage utile est [0..255] mais les blocs en jeu sont [-5..14].
 * Les valeurs -5 et 14 couvrent les blocs partiellement visibles
 * aux bords de l'écran.
 *
 * Utilisé par GETBLOCKX (foreground) et GETBLOCKXP (plan central).
 * GETBLOCKXP soustrait `angle` (7) avant l'indexation.
 */
extern const int8_t BlockTable[256];

/*
 * PixelTable[256]
 * Index  : même que BlockTable
 * Retour : pixel dans le bloc (0-13)
 *
 * Stocké dans OFFSET après appel à GETBLOCKX/GETBLOCKXP.
 * Utilisé par GETDIST pour calculer la distance au bord du bloc.
 */
extern const uint8_t PixelTable[256];


/* -----------------------------------------------------------------------
 * TABLES DE MULTIPLICATION
 * ----------------------------------------------------------------------- */

/*
 * Mult10[16]
 * Index  : valeur (0-15)
 * Retour : valeur × 10
 *
 * Utilisé pour convertir un numéro de rangée (0-2) en index dans
 * le blueprint (0=0, 1=10, 2=20) → accès au 1er bloc de la rangée.
 */
extern const uint8_t Mult10[16];

/*
 * Mult7[16]
 * Index  : valeur (0-15)
 * Retour : valeur × 7
 *
 * Utilisé pour convertir une largeur en octets hi-res en pixels :
 * chaque octet hi-res = 7 pixels. Utilisé dans GETEDGES pour
 * calculer imwidth.
 */
extern const uint8_t Mult7[16];

/*
 * Mult30[32] : table 16 bits (uint16_t)
 * Index  : numéro d'écran (0-31)
 * Retour : offset en octets dans le blueprint (valeur × 30)
 *
 * Utilisé par calcblue() pour localiser le début des données
 * d'un écran dans BlueType/BlueSpec.
 */
extern const uint16_t Mult30[32];


/* -----------------------------------------------------------------------
 * TABLES DE GÉOMÉTRIE DES BLOCS
 * ----------------------------------------------------------------------- */

/*
 * BlockEdge[20]
 * Index  : numéro de bloc + 5 (pour couvrir les blocs -5 à 14)
 * Retour : coordonnée X (140px) du bord gauche du bloc
 *
 * Formule : BlockEdge[i] = ScrnLeft + (i-5) * 14 - 12
 * (le -12 initial correspond au bord gauche du bloc -5)
 *
 * Valeurs (bloc -5 à 14) :
 *   -5 → -12, -4 → 2, -3 → 16, -2 → 30, -1 → 44,
 *    0 → 58,   1 → 72,  2 → 86,  3 → 100, ..., 14 → 254
 *
 * Utilisé par GETBLOCKEJ, GETFWDDIST, CHECKALERT, etc.
 */
extern const uint8_t BlockEdge[20];

/*
 * BlockTop[5]
 * Index  : rangée de blocs + 1 (couvre -1 à 3)
 * Retour : Y du bord supérieur de la rangée
 *
 * Valeurs (rangée -1 à 3) :
 *   ScrnBot+1-Blox4, ScrnBot+1-Blox3, ScrnBot+1-Blox2,
 *   ScrnBot+1-Blox1, ScrnBot+1
 *   = 3, 66, 129, 192, 255 (avec ScrnBot=191, BlockHeight=63)
 *
 * Utilisé par GETBLOCKY (plan foreground) et CROPCHAR.
 */
extern const uint8_t BlockTop[5];

/*
 * BlockBot[5]
 * Index  : rangée de blocs + 1
 * Retour : Y du bord inférieur de la rangée
 *
 * Valeurs : ScrnBot-Blox3, ScrnBot-Blox2, ScrnBot-Blox1,
 *           ScrnBot, ScrnBot+Blox1
 *           = 2, 65, 128, 191, 254
 */
extern const uint8_t BlockBot[5];

/*
 * FloorY[5]
 * Index  : rangée de blocs + 1
 * Retour : Y du plan central (où se tiennent les personnages)
 *          = Y du bord inférieur - VertDist
 *
 * Valeurs : BlockBot[i] - VertDist (10)
 *           = -8, 55, 118, 181, 244
 *
 * C'est la coordonnée Y à laquelle un personnage debout sur la
 * rangée i a ses pieds. Référencé partout sous la forme
 * FloorY[CharBlockY + 1] (l'indice +1 gère la rangée -1).
 *
 * Utilisé par : CHECKFLOOR (hitflr), STARTKID, DOIMPALE, etc.
 */
extern const uint8_t FloorY[5];

/*
 * BlockAy[5]
 * Index  : rangée de blocs + 1
 * Retour : Y du bord inférieur visible d'un plancher (sans DHeight)
 *          = BlockBot[i] - DHeight (3)
 *
 * Utilisé pour le dessin des planchers dans GRAFIX.
 */
extern const uint8_t BlockAy[5];


/* -----------------------------------------------------------------------
 * VALEURS CALCULÉES (pour référence lors de la traduction)
 *
 * Avec ScrnBot=191, BlockHeight=63, VertDist=10, DHeight=3 :
 *
 * Rangée  BlockTop  BlockBot  FloorY  BlockAy
 *  -1       3          2       -8*     -1*
 *   0      66         65       55      62
 *   1     129        128      118     125
 *   2     192        191      181     188
 *   3     255        254      244     251
 *
 * (*) Les valeurs pour la rangée -1 dépassent les limites uint8_t
 *     et wrappent (comportement attendu en 8 bits : 256-8=248, 256-1=255)
 *
 * En C, FloorY est utilisé avec l'index (CharBlockY + 1) :
 *   CharBlockY=0 → FloorY[1] = 55  (rangée du haut)
 *   CharBlockY=1 → FloorY[2] = 118 (rangée du milieu)
 *   CharBlockY=2 → FloorY[3] = 181 (rangée du bas)
 * ----------------------------------------------------------------------- */

#endif /* TABLES_H */
