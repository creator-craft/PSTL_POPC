#ifndef SEQTABLE_H
#define SEQTABLE_H

/*
 * seqtable.h
 * Traduction de SEQTABLE.S — Table des séquences d'animation (bytecode)
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $3000 (aux mem) sur Apple II.
 *
 * -----------------------------------------------------------------------
 * NATURE DU FICHIER
 *
 * SEQTABLE.S est un fichier de DONNÉES PURES — le bytecode d'animation
 * du jeu. Il contient deux structures complémentaires :
 *
 *   1. La table d'index seqtab (114 entrées × 2 octets = 228 octets)
 *      Chaque entrée est une adresse 16 bits pointant vers le début
 *      d'une séquence dans le blob Fdef[]. C'est la table que JUMPSEQ
 *      consulte pour repositionner CharSeq sur une séquence nommée.
 *
 *   2. Le blob de bytecode Fdef (1200 octets alloués dans FRAMEDEF.S)
 *      Séquences d'animation encodées en bytecode interprété par ANIMCHAR.
 *      Chaque séquence est une liste d'octets : numéros de frames positifs
 *      entrecoupés d'instructions négatives (goto, chx, act...).
 *
 * En assembleur, seqtab contient des adresses absolues Apple II.
 * En C, ces adresses deviennent des offsets en octets dans Fdef[].
 *
 * -----------------------------------------------------------------------
 * INSTRUCTIONS DE BYTECODE (valeurs négatives dans le flux)
 *
 *   goto      (-1)  : lire 2 octets → nouveau CharSeq (branchement)
 *   aboutface (-2)  : CharFace ^= 0xFF (demi-tour)
 *   up        (-3)  : CharBlockY-- + addslicers()
 *   down      (-4)  : CharBlockY++ + addslicers()
 *   chx       (-5)  : lire 1 octet → addcharx(octet) → CharX
 *   chy       (-6)  : lire 1 octet → CharY += octet
 *   act       (-7)  : lire 1 octet → CharAction
 *   setfall   (-8)  : lire 2 octets → CharXVel, CharYVel
 *   ifwtless  (-9)  : si weightless!=0 : lire 2 octets → goto, sinon skip
 *   die       (-10) : no-op (ignoré par ANIMCHAR)
 *   jaru      (-11) : jarabove = 1 (secoue planchers au-dessus)
 *   jard      (-12) : jarabove = -1 (secoue planchers en-dessous)
 *   effect    (-13) : lire 1 octet (1 = potioneffect)
 *   tap       (-14) : lire 1 octet → son + alertguard=1
 *   nextlevel (-15) : GoneUpstairs() → NextLevel++
 *
 * -----------------------------------------------------------------------
 * STRUCTURE D'UNE SÉQUENCE
 *
 * Chaque séquence est un flux d'octets lus par GETSEQ() :
 *
 *   - Valeur positive (1-240) = numéro de frame → CharPosn = valeur, stop
 *   - Valeur négative = instruction, exécutée, lecture continue
 *   - Toutes les séquences se terminent sur une boucle goto infinie
 *     ou sur une instruction qui change CharSeq (jumpseq)
 *
 * Exemple (stand) :
 *   act, 0        → CharAction = 0
 *   15            → CharPosn = 15, retour
 *   goto stand    → (boucle infinie, jamais atteint sans jumpseq)
 *
 * -----------------------------------------------------------------------
 * LES 114 SÉQUENCES
 *
 * Numéros 1-93 : séquences du joueur et des gardes (mouvement + combat)
 * Numéros 94-113 : séquences des personnages de cinématique
 *   (Pstand, Vstand, Vwalk, Palert, Pback... princesse et vizir)
 * Numéro 114 : Mraise (souris)
 *
 * Les constantes symboliques sont définies dans seqdata.h.
 * ----------------------------------------------------------------------- */

#include <stdint.h>
#include "framedef.h"   /* pour Fdef[] */

/*
 * seqtab[114] — table d'index des séquences
 *
 * En C : offsets en octets depuis le début de Fdef[].
 * seqtab[seq - 1] = offset du premier octet de la séquence `seq`.
 *
 * Calcul lors de la traduction de SEQTABLE.S :
 *   pour chaque label (stand, startrun, etc.), calculer
 *   (adresse_label - adresse_Fdef) et stocker dans seqtab[].
 *
 * Ces valeurs ne peuvent pas être écrites à la main — elles doivent
 * être calculées automatiquement depuis les labels de SEQTABLE.S,
 * ou bien en compilant le fichier assembleur et en lisant les adresses
 * résolues, puis en soustrayant l'adresse de base de Fdef ($3000 + 228).
 */
extern const uint16_t seqtab[114];

/*
 * Note sur la génération de seqtab[] :
 *
 * Adresse de base de Fdef en mémoire Apple II : $3000 + 228 = $30E4
 * (228 = 114 entrées × 2 octets de la table d'index elle-même)
 *
 * Pour chaque séquence seq (1-114) :
 *   seqtab[seq-1] = adresse_apple2(label_seq) - $30E4
 *
 * Les adresses Apple II des labels sont visibles dans le listing
 * assembleur ou calculables depuis les positions relatives dans le
 * fichier source. Une fois SEQTABLE.S traduit en C, les labels
 * deviennent des constantes de tableau calculées à la compilation.
 */

#endif /* SEQTABLE_H */
