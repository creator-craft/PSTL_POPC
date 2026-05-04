#ifndef FRAMEDEF_H
#define FRAMEDEF_H

/*
 * framedef.h
 * Traduction de FRAMEDEF.S : Définitions des frames d'animation
 * Source originale : Jordan Mechner, 1989
 *
 * -----------------------------------------------------------------------
 * NATURE DU FICHIER
 *
 * FRAMEDEF.S est un fichier de DONNÉES PURES. Il contient quatre blobs
 * de données alloués à partir de l'adresse $2800 :
 *
 *   Fdef      (1200 octets) : frames principaux : joueur + gardes (parties)
 *   altset1   (200 octets)  : frames alternatifs set 1 : gardes complets
 *   altset2   (450 octets)  : frames alternatifs set 2 : princesse + vizir
 *   swordtab  (192 octets)  : positions des épées pour chaque frame
 *
 * Ces quatre tableaux sont chargés en mémoire par MASTER.S au démarrage
 * et constituent la "base de données d'animation" du jeu. Toute l'animation
 * repose sur eux.
 *
 * -----------------------------------------------------------------------
 * FORMAT D'UN FRAME (5 octets par entrée dans Fdef)
 *
 *   Byte 0 : Fimage  : numéro d'image dans la chtable (bits 0-6)
 *                       bit 7 = sélecteur de table (0=table A, 1=table B)
 *   Byte 1 : Fsword  : numéro de frame d'épée dans swordtab (bits 0-5)
 *                       bits 6-7 = bits de sélection de table pour l'épée
 *   Byte 2 : Fdx     : déplacement X (signé, en pixels 140-res)
 *                       + = vers l'avant, - = vers l'arrière
 *   Byte 3 : Fdy     : déplacement Y (signé, positif = vers le bas)
 *   Byte 4 : Fcheck  : flags du frame :
 *                       bits 0-3 : position du pied (footmark, 0-13 pixels
 *                                  depuis le bord gauche de l'image)
 *                       bit  4   : Fthinmark : image "fine" (CDLeftEj/CDRightEj
 *                                  resserrés de THINNER pixels de chaque côté)
 *                       bits 5   : flag spécial (turning frames)
 *                       bit  7   : parité X (0 = X pair, 1 = X impair)
 *                                  combiné avec FCharFace pour l'alignement
 *                                  sub-pixel sur Apple II
 *
 * Accès : GETFRAMEINFO lit Fdef[(CharPosn - 1) * 5]
 *         (CharPosn commence à 1, donc index 0 = frame 1)
 *
 * -----------------------------------------------------------------------
 * FORMAT D'UNE ENTRÉE SWORDTAB (3 octets par entrée)
 *
 *   Byte 0 : numéro d'image de l'épée dans chtable3 (0 = pas d'épée)
 *   Byte 1 : DX : offset X de l'épée par rapport à FCharX (signé)
 *   Byte 2 : DY : offset Y de l'épée par rapport à FCharY (signé)
 *
 * -----------------------------------------------------------------------
 * SETS ALTERNATIFS
 *
 * Les frames 150-189 ont des définitions différentes selon le personnage :
 *   Fdef[150-189]   = frames de combat du JOUEUR (épée main droite)
 *   altset1[0-39]   = frames de combat des GARDES (épée main gauche, miroir)
 *
 * GETFRAMEINFO dans CTRLSUBS.S choisit automatiquement le bon set selon
 * CharID et CharPosn via usealtsets().
 *
 * altset2[0-84] = frames de la PRINCESSE (Pstand, Plie, Pback...) et
 *                 du VIZIR (Vstand, Vapproach...) : utilisés par les
 *                 scènes cinématiques de SUBS.S.
 *
 * -----------------------------------------------------------------------
 * CORRESPONDANCE CharPosn → nom d'animation (sélection)
 *
 *  1-14   : run-4 à run-17
 *  15     : stand
 *  16-33  : standjump-9 à standjump-26
 *  34-44  : runjump-1 à runjump-11
 *  45-52  : turn-2 à turn-10
 *  53-65  : runturn-8 à runturn-20
 *  67-85  : jumphang-2 à hangdrop-8
 *  87-99  : jumphang-22 à jumphang-34
 *  102-119: jumpfall-2 à jumpfall-19
 *  121-132: stepfwd-1 à stepfwd-12
 *  133-149: sheathe / climbup frames
 *  150-171: combat (strike, block, ready, advance, retreat...)
 *  172-176: jumpfall (versions gardes blessés)
 *  177    : impaled
 *  178    : halves (coupé en deux)
 *  179-183: collapse-15 à collapse-19 (mort par chute)
 *  185    : dead
 *  186-188: mouse-1 à mouse-crouch
 *  191-210: drink (boire potion) / draw (dégainer)
 *  217-228: climbst-2 à climbst-13 (monter escalier)
 *  229-240: sheathe-22 à sheathe-33
 * ----------------------------------------------------------------------- */

#include <stdint.h>

/* -----------------------------------------------------------------------
 * TAILLES DES BLOBS
 * ----------------------------------------------------------------------- */

#define FDEF_SIZE      1200   /* octets alloués pour Fdef */
#define ALTSET1_SIZE    200   /* octets pour altset1 (40 frames × 5) */
#define ALTSET2_SIZE    450   /* octets pour altset2 (90 frames × 5) */
#define SWORDTAB_SIZE   192   /* octets pour swordtab (64 épées × 3) */

#define FDEF_FRAME_SIZE   5   /* octets par frame */
#define SWORD_ENTRY_SIZE  3   /* octets par entrée swordtab */

/* Nombre maximal de frames dans chaque set */
#define FDEF_MAX_FRAMES     240   /* Fdef couvre les frames 1-240 */
#define ALTSET1_FRAME_BASE  150   /* altset1 couvre les frames 150-189 */
#define ALTSET2_FRAME_BASE    1   /* altset2 commence au frame 1 */

/* -----------------------------------------------------------------------
 * BLOBS DE DONNÉES D'ANIMATION
 *
 * Ces tableaux sont définis dans framedef.c (ou chargés depuis le disque).
 * Leur contenu est accédé exclusivement via GETFRAMEINFO (CTRLSUBS.S).
 * ----------------------------------------------------------------------- */

/*
 * Fdef[] : table principale des frames d'animation
 *
 * Accès : Fdef[(frame_num - 1) * 5]
 *   [0] = Fimage, [1] = Fsword, [2] = Fdx, [3] = Fdy, [4] = Fcheck
 *
 * Note sur JUMPSEQ : CharSeq est un offset en octets dans Fdef[].
 * seqtab[seq-1] donne l'offset du premier octet du frame de départ
 * de la séquence seq.
 */
extern uint8_t Fdef[FDEF_SIZE];

/*
 * altset1[] : frames alternatifs set 1 (gardes, frames 150-189)
 *
 * Même format que Fdef. Accès : altset1[(frame_num - 150) * 5]
 * Les images font référence à chtable4 (jeu de sprites ennemi).
 * Les gardes sont miroirs du joueur : l'épée est en main gauche.
 */
extern uint8_t altset1[ALTSET1_SIZE];

/*
 * altset2[] : frames alternatifs set 2 (princesse et vizir)
 *
 * Même format. Accès : altset2[(frame_num - 1) * 5]
 * Utilisé pour CharID == 5 (princesse) et CharID == 6 (vizir).
 * Les images font référence à chtable6.
 */
extern uint8_t altset2[ALTSET2_SIZE];

/*
 * swordtab[] : positions des épées
 *
 * Format : { image, dx, dy } × 64
 * Accès : swordtab[(sword_frame - 1) * 3]
 * Fsword (byte 1 de Fdef) donne le numéro de frame d'épée (1-64),
 * ou 0 si pas d'épée pour ce frame.
 */
extern uint8_t swordtab[SWORDTAB_SIZE];


/* -----------------------------------------------------------------------
 * MACROS D'ACCÈS
 * ----------------------------------------------------------------------- */

/* Accès aux champs d'un frame dans Fdef ou un altset */
#define FRAME_IMAGE(buf, n)   ((buf)[((n)-1) * FDEF_FRAME_SIZE + 0])
#define FRAME_SWORD(buf, n)   ((buf)[((n)-1) * FDEF_FRAME_SIZE + 1])
#define FRAME_DX(buf, n)      ((int8_t)(buf)[((n)-1) * FDEF_FRAME_SIZE + 2])
#define FRAME_DY(buf, n)      ((int8_t)(buf)[((n)-1) * FDEF_FRAME_SIZE + 3])
#define FRAME_CHECK(buf, n)   ((buf)[((n)-1) * FDEF_FRAME_SIZE + 4])

/* Extraction des flags de Fcheck */
#define FCHECK_FOOTMARK(fcheck)  ((fcheck) & 0x0F)        /* bits 0-3 */
#define FCHECK_THINMARK(fcheck)  (((fcheck) >> 4) & 0x01) /* bit 4 */
#define FCHECK_PARITY(fcheck)    (((fcheck) >> 7) & 0x01) /* bit 7 */

/* Accès à une entrée de swordtab */
#define SWORD_IMAGE(n)  (swordtab[((n)-1) * SWORD_ENTRY_SIZE + 0])
#define SWORD_DX(n)     ((int8_t)swordtab[((n)-1) * SWORD_ENTRY_SIZE + 1])
#define SWORD_DY(n)     ((int8_t)swordtab[((n)-1) * SWORD_ENTRY_SIZE + 2])

#endif /* FRAMEDEF_H */
