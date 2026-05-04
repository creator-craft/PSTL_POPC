#ifndef BGDATA_H
#define BGDATA_H

/*
 * bgdata.h
 * Traduction de BGDATA.S — Tables de données du background
 * Source originale : Jordan Mechner, 1989
 *
 * -----------------------------------------------------------------------
 * NATURE DU FICHIER
 *
 * BGDATA.S est un fichier de DONNÉES PURES inclus par FRAMEADV.S via
 * une directive `put bgdata`. Il ne contient aucun code exécutable —
 * uniquement des constantes et des tableaux statiques.
 *
 * Il définit deux catégories de contenu :
 *
 *   1. Les IDs des objets de blueprint (0-29) — déjà définis dans
 *      MOVEDATA.S pour les tests logiques, redéfinis ici avec les mêmes
 *      valeurs pour l'usage dans FRAMEADV. En C, un seul fichier
 *      (movedata.h) suffit — pas de redéfinition nécessaire.
 *
 *   2. Les tables de rendu des sections A/B/C/D/Front de chaque objet :
 *      pour chaque objet (indexé par son ID), ces tables indiquent
 *      quel numéro d'image utiliser, quel offset Y appliquer, etc.
 *      C'est la "carte graphique" du système de rendu de FRAMEADV.S.
 *
 * -----------------------------------------------------------------------
 * LE SYSTÈME DE SECTIONS A/B/C/D/FRONT
 *
 * Chaque bloc du décor est rendu en cinq couches superposées :
 *
 *   Section A  — partie supérieure du bloc courant (arche, colonne, mur)
 *                Origine : (BlockLeft, BlockBot - 3) — plan "milieu"
 *
 *   Section B  — partie mobile/animée du bloc à GAUCHE (portail, pics...)
 *                Origine : même que A
 *
 *   Section C  — coin inférieur du bloc en BAS-À-GAUCHE (jonction)
 *                Origine : (BlockLeft, BlockBot) — plan "bas"
 *
 *   Section D  — le plancher lui-même (dalle horizontale)
 *                Origine : (BlockLeft, BlockBot)
 *
 *   Front      — plan avant, dessiné PAR-DESSUS les personnages
 *                (barreaux de portail, façade de pilier, devant d'arche)
 *
 * Toutes les valeurs X et Y dans ces tables sont relatives à ces origines.
 * ----------------------------------------------------------------------- */

#include <stdint.h>

/* -----------------------------------------------------------------------
 * IDs DES OBJETS DE BLUEPRINT
 *
 * Note : ces valeurs sont identiques à celles de movedata.h.
 * En C, inclure movedata.h suffit — ne pas redéclarer ici.
 * Listées pour référence et correspondance avec les indices des tables.
 * ----------------------------------------------------------------------- */

/*
 * space=0, floor=1, spikes=2, posts=3, gate=4,
 * dpressplate=5, pressplate=6, panelwif=7, pillarbottom=8, pillartop=9,
 * flask=10, loose=11, panelwof=12, mirror=13, rubble=14, upressplate=15,
 * exit=16, exit2=17, slicer=18, torch=19, block=20, bones=21, sword=22,
 * window=23, window2=24, archbot=25, archtop1=26, archtop2=27,
 * archtop3=28, archtop4=29
 */


/* -----------------------------------------------------------------------
 * TABLES DES SECTIONS A/B/C/D/FRONT
 * Indexées par ID d'objet (0-29).
 * ----------------------------------------------------------------------- */

/*
 * maska[30] — masque de la section A
 * Numéro d'image du masque AND à appliquer avant de dessiner la section A
 * du bloc courant (pour éviter que le bloc voisin ne déborde).
 * 0 = pas de masque nécessaire.
 */
extern const uint8_t maska[30];

/*
 * piecea[30] — image principale de la section A
 * Numéro d'image (dans la bgtable courante) de la partie supérieure
 * du bloc. 0 = section A invisible (espace vide, certains panneaux).
 */
extern const uint8_t piecea[30];

/*
 * pieceay[30] — offset Y de la section A (signé)
 * S'ajoute à la coordonnée Y de base (BlockBot - 3) pour positionner
 * la section A. La plupart des valeurs sont 0 ; exceptions notables :
 * pressplate (+1), archtop2 (-4), archtop3 (-4), archtop4 (-4).
 */
extern const int8_t pieceay[30];

/*
 * maskb[30] — masque de la section B
 * Numéro d'image du masque AND pour la section B du bloc à gauche.
 * 0 = pas de masque.
 */
extern const uint8_t maskb[30];

/*
 * pieceb[30] — image de la section B (partie mobile/animée)
 * Numéro d'image de la partie verticale du bloc à gauche.
 * Certaines valeurs ont le bit 7 set ($9e, $9f) pour indiquer
 * un panneau avec traitement spécial (panelb0/panelc0).
 */
extern const uint8_t pieceb[30];

/*
 * pieceby[30] — offset Y de la section B (signé)
 * S'ajoute à BlockBot - 3. Quelques valeurs négatives pour les
 * sections qui remontent au-dessus de la ligne de sol.
 */
extern const int8_t pieceby[30];

/*
 * bstripe[30] — image de la bande décorative palace (section B)
 * Uniquement pour le jeu de tuiles "palace" (BGset1 == 1).
 * 0 = pas de bande pour cet objet.
 */
extern const uint8_t bstripe[30];

/*
 * piecec[30] — image de la section C (coin bas-gauche)
 * Numéro d'image du coin de jonction entre le bas-gauche et
 * le bloc courant. Visible uniquement si le bloc courant est vide.
 * Certaines valeurs ont bit 7 set pour traitement spécial (panelc0).
 */
extern const uint8_t piecec[30];

/*
 * pieced[30] — image de la section D (plancher)
 * Numéro d'image de la dalle horizontale en bas du bloc.
 * C'est la partie la plus systématiquement présente.
 */
extern const uint8_t pieced[30];

/*
 * fronti[30] — image du plan avant (front)
 * Numéro d'image dessiné par-dessus les personnages.
 * 0 = pas de plan avant pour cet objet.
 */
extern const uint8_t fronti[30];

/*
 * fronty[30] — offset Y du plan avant (signé)
 * Relatif à la section A (BlockBot - 3 + pieceay).
 */
extern const int8_t fronty[30];

/*
 * frontx[30] — offset X du plan avant (en octets hi-res)
 * Relatif à blockxco (bord gauche du bloc courant).
 */
extern const uint8_t frontx[30];


/* -----------------------------------------------------------------------
 * IMAGES SPÉCIALES DES PORTAILS
 * ----------------------------------------------------------------------- */

/* Images des sections du portail */
#define GATEBOT_STA  0x43   /* bas de portail, mode STA (opaque) */
#define GATEBOT_ORA  0x44   /* bas de portail, mode ORA (semi-transparent) */
#define GATE_B1      0x37   /* treillis milieu du portail */
#define GATEC_MASK   0x0d   /* masque section C du portail */

/*
 * gate8c[8] — 8 images de la section C du portail (selon hauteur)
 * gate8b[8] — 8 images du sommet du portail (selon hauteur)
 * Indexées par gateposn % 8.
 */
extern const uint8_t gate8c[8];
extern const uint8_t gate8b[8];

/* Valeurs min/max de l'état d'un portail */
#define GMAXVAL  (47 * 4)   /* portail complètement ouvert */
#define GMINVAL  0          /* portail complètement fermé */


/* -----------------------------------------------------------------------
 * IMAGES DE GRIMPÉE (CLIMBUP)
 * ----------------------------------------------------------------------- */

#define CU_MASK   0x11   /* masque pour la demi-section A lors d'une grimpée */
#define CU_PIECE  0x12   /* image de la demi-section A */
#define CU_POST   0x0e   /* version spéciale pour les piliers (palace) */


/* -----------------------------------------------------------------------
 * IMAGES DE LA SORTIE (STAIRS/EXIT)
 * ----------------------------------------------------------------------- */

#define STAIRS     0x6b   /* image des marches */
#define DOOR       0x6c   /* image de la porte (rendu répété de bas en haut) */
#define DOOR_MASK  0x6d   /* masque AND de la porte */
#define TOP_REPAIR 0x6e   /* réparation du haut de l'encadrement */

#define ARCHTOP3SP 0xa1   /* archtop3 spécial (arch se terminant sur panneau) */


/* -----------------------------------------------------------------------
 * ANIMATION DES PICS
 * ----------------------------------------------------------------------- */

/*
 * spikea[10] — images de la section A des pics (frames 0-9)
 * spikeb[10] — images de la section B des pics
 * Frame 0 = rétractés, frame 5 (spikeExt) = complètement sortis.
 */
extern const uint8_t spikea[10];
extern const uint8_t spikeb[10];

#define SPIKE_EXT  5   /* frame des pics complètement étendus */
#define SPIKE_RET  9   /* frame des pics complètement rétractés */


/* -----------------------------------------------------------------------
 * ANIMATION DE LA TRANCHEUSE
 * ----------------------------------------------------------------------- */

/*
 * slicerseq[7] — séquence des frames de la trancheuse
 * Chaque valeur est un index dans slicertop/slicerbot (1-5).
 */
extern const uint8_t slicerseq[7];

#define SLICER_EXT  2   /* état "étendu" (lame sortie) */
#define SLICER_RET  6   /* état "rétracté" (lame rentrée) */

extern const uint8_t slicertop[5];    /* images du haut de la trancheuse */
extern const uint8_t slicerbot[5];    /* images du bas */
extern const uint8_t slicerbot2[5];   /* images du bas avec traînée de sang */
extern const int8_t  slicergap[5];    /* espacement vertical haut/bas */
extern const uint8_t slicerfrnt[5];   /* images du plan avant */


/* -----------------------------------------------------------------------
 * ANIMATION DU PLANCHER INSTABLE
 * ----------------------------------------------------------------------- */

#define LOOSE_B    0x1b   /* image de la section B du plancher instable */

extern const uint8_t loosea[11];    /* images section A selon état (0-10) */
extern const int8_t  looseby[11];   /* offsets Y section B */
extern const uint8_t loosed[11];    /* images section D */

#define FFALLING   10   /* premier frame "en chute" du plancher */


/* -----------------------------------------------------------------------
 * IMAGES DIVERSES
 * ----------------------------------------------------------------------- */

#define SPECIAL_FLASK  0x95   /* image spéciale pour certaines potions */

#define SWORD_GLEAM0   0x99   /* épée au sol, frame 0 */
#define SWORD_GLEAM1   0xb3   /* épée au sol, frame 1 (scintillement) */


/* -----------------------------------------------------------------------
 * PANNEAUX (PANELS)
 * ----------------------------------------------------------------------- */

#define PANEL_B0    0x9e   /* image section B des panneaux (traitement spécial) */
#define PANEL_C0    0x9f   /* image section C des panneaux */
#define NUM_PANS    3      /* nombre de variantes de panneaux */

extern const uint8_t panelb[3];   /* images section B selon état panneau (0-2) */
extern const uint8_t panelc[3];   /* images section C */

#define ARCH_PANEL  0xa1   /* arche se terminant contre un panneau */


/* -----------------------------------------------------------------------
 * MURS DE FOND (SPACE & FLOOR)
 * ----------------------------------------------------------------------- */

#define NUM_BPANS  3   /* nombre de variantes de murs de fond */

extern const uint8_t spaceb[4];    /* images section B pour espace vide (palace) */
extern const int8_t  spaceby[4];   /* offsets Y */
extern const uint8_t floorb[4];    /* images section B pour sol (palace) */
extern const int8_t  floorby[4];   /* offsets Y */


/* -----------------------------------------------------------------------
 * BLOCS SOLIDES
 * ----------------------------------------------------------------------- */

#define NUM_BLOX  2   /* nombre de variantes de blocs solides */

extern const uint8_t blockb[2];    /* images section B des blocs */
extern const uint8_t blockc[2];    /* images section C */
extern const uint8_t blockd[2];    /* images section D */
extern const uint8_t blockfr[2];   /* images plan avant */

#endif /* BGDATA_H */
