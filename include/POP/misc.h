#ifndef MISC_H
#define MISC_H

/*
 * misc.h
 * Traduction de MISC.S — Routines diverses de haut niveau
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $f900 (aux LC bank 1) sur Apple II.
 *
 * Ce fichier contient des routines de gameplay qui ne rentrent
 * pas dans les autres catégories : gestion des personnages spéciaux
 * (squelette, souris, miroir, reflet), force du personnage, sauvegarde,
 * chargement de niveau, et calcul de l'alerte ennemie.
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

/* Effets des potions */
#define WTLESSTIMER  200    /* durée de la potion de légèreté (frames) */
#define VIBETIMER      3    /* durée des vibrations de légèreté */

/* Paramètres du squelette (level 3) */
#define SKELSCRN   1
#define SKELX      5
#define SKELY      1
#define SKELTRIG   2    /* blockX qui déclenche l'apparition */
#define SKELPROG   2    /* programme de garde du squelette */

/* Seuil d'alerte : distance max pour le combat (en pixels × 4) */
#define GFIGHTTHRES  (28 * 4)

/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES — Jump table à $f900
 * ----------------------------------------------------------------------- */

/*
 * VANISHCHAR — Fait disparaître le personnage courant (Char).
 *
 * Met CharFace=86 (code "pas de personnage"), CharAction=0,
 * CharLife=0. Calcule ChgOppStr = -OppStrength (tue l'adversaire).
 * Utilisé quand le shadowman est tué sur level 12 (il disparaît
 * au lieu de tomber).
 */
void VANISHCHAR(void);

/*
 * MOVEMUSIC — Déplace 1K de données musicales de $5000 main vers aux LC.
 * Opération de chargement Apple II → ignorée en C (les données sont
 * déjà en mémoire à l'initialisation).
 */
void MOVEMUSIC(void);

/*
 * FIRSTGUARD — Empêche le joueur de passer à travers un garde en garde.
 *
 * Si EnemyAlert >= 2 et l'adversaire est en garde sur le même plan,
 * face au joueur, à moins de 15 pixels derrière lui :
 * aligne CharY sur le sol et déclenche la séquence "bump".
 *
 * Appelé depuis DoKid (TOPCTRL) avant le contrôle du joueur.
 */
void FIRSTGUARD(void);

/*
 * MARKMETERS — Marque les 3 blocs de la barre de force du joueur
 * et les 2 blocs de la barre de l'adversaire pour redraw.
 */
void MARKMETERS(void);

/* Marque les 3 blocs de la barre de force du joueur (blocs 20-22) */
void MARKKIDMETER(void);

/* Marque les 2 blocs de la barre de l'adversaire (blocs 28-29) */
void MARKOPPMETER(void);

/*
 * POTIONEFFECT — Applique l'effet d'une potion bue.
 *
 * Ne s'applique que si CharID == 0 (le joueur) et lastpotion != 0.
 *
 * Effets selon lastpotion :
 *  -1 : épée → gotsword=1, 3 flashs blancs, musique s_Sword
 *   1 : recharge → ChgKidStr+1, 2 flashs orange, musique s_ShortPot
 *   2 : boost → MaxKidStr+1 + recharge, 5 flashs orange, musique s_Potion
 *   3 : légèreté → weightless=200, vibes=3, musique s_ShortPot
 *   4 : inversion → invert ^= 0xFF, redrawflg=2, inverty()
 *   5 : mauvaise → ChgKidStr=-1, son Splat
 */
void POTIONEFFECT(void);

/*
 * MOUSERESCUE — Fait apparaître la souris pour sauver le joueur (level 8).
 *
 * Crée la souris (CharID=24) à droite de l'écran, la met en marche
 * vers la gauche, et la sauvegarde dans Shad. Appelé depuis misctimers
 * quand exitopen >= mousetimer.
 */
void MOUSERESCUE(void);

/*
 * STABCHAR — Réaction d'un personnage à une attaque d'épée.
 *
 * Si CharSword==2 (en garde) : décrémente la force (decstr).
 *   → Si force tombe à 0 : tue le personnage (stabkill ou fightfall)
 *   → Sinon : séquence "stabbed" (blessé)
 * Si CharSword != 2 (sans défense) : -100 force → stabkill
 *
 * Gère le cas spécial du squelette (CharID==4) : invincible.
 * Si le personnage est tué au bord d'un précipice, le projette dans
 * le vide (fightfall).
 */
void STABCHAR(void);

/*
 * UNHOLY — Si le shadowman meurt, le joueur meurt aussi (et vice versa).
 *
 * Uniquement sur level 12 quand CharID+OpID == 1 (kid vs shadow).
 * Applique -100 force + 5 flashs blancs + son Splat.
 */
void UNHOLY(void);

/*
 * REFLECTION — Gère le reflet du joueur dans le miroir (level 4).
 *
 * Si createshad == 0xFF : le reflet prend vie comme shadowman
 * (appelle CreateShad). Sinon, si le joueur est devant le miroir,
 * calcule la position du reflet et l'ajoute à la table d'objets.
 *
 * Le reflet est l'image miroir du joueur : Face inversé, X symétrique
 * par rapport au centre du miroir.
 */
void REFLECTION(void);

/*
 * BONESRISE — Fait apparaître le squelette vivant (level 3).
 *
 * Déclenché quand le joueur atteint le bloc SKELTRIG sur SKELSCRN.
 * Remplace les os (bones) par un sol normal, crée un garde de type
 * squelette (CharID=4, CharSword=2) et le sauvegarde dans Shad.
 */
void BONESRISE(void);

/*
 * DECSTR — Décrémente la force du personnage courant de A.
 *
 * In: A = quantité à retrancher (1-100, non nul)
 * Out: non-zéro si le personnage survit, zéro s'il meurt
 *      ChgKidStr ou ChgOppStr mis à jour (valeur négative)
 *
 * Si A >= force actuelle : tue le personnage (killkid ou killopp).
 * La force elle-même (KidStrength/OppStrength) n'est PAS modifiée ici ;
 * c'est chgmeters() (TOPCTRL) qui l'applique en lisant ChgKidStr.
 */
uint8_t DECSTR(uint8_t amount);

/*
 * DOSAVEGAME — Sauvegarde la partie sur disque.
 *
 * Ne sauvegarde que si level >= FirstSideB (3).
 * Copie origstrength, FrameCount, NextTimeMsg dans les zones Sav*
 * puis appelle savegame() (MASTER).
 * En C : à remplacer par une écriture dans un fichier.
 */
void DOSAVEGAME(void);

/*
 * LOADLEVELX — Charge un niveau depuis le disque.
 *
 * In: X = numéro de niveau (0-14)
 * Consulte les tables bgset1/bgset2/chset/bluepTRKlst/bluepREGlst
 * pour déterminer les jeux de tuiles et la piste blueprint à charger,
 * puis appelle loadlevel() (MASTER).
 * En C : à remplacer par un chargement de fichier.
 */
void LOADLEVELX(uint8_t level_num);

/*
 * CHECKALERT — Calcule EnemyAlert (visibilité mutuelle joueur/garde).
 *
 * Out: EnemyAlert =
 *   2 : ligne de vue dégagée (même écran, même rangée, pas d'obstacle)
 *   1 : obstacles (trancheuse, espace, portail fermé) mais vue dégagée
 *   0 : obstacle solide (mur, panneau) ou conditions non remplies
 *
 * Conditions pour EnemyAlert > 0 :
 *  - Même écran et même BlockY que le joueur
 *  - Ni l'un ni l'autre n'est mort
 *  - ShadFace != 86 (un adversaire existe)
 *  - KidPosn != 0 et pas sur l'escalier (219-228)
 *
 * La routine scanne les blocs entre KidBlockX et ShadBlockX
 * à la hauteur KidBlockY.
 */
void CHECKALERT(void);

/*
 * DISPVERSION — Affiche le numéro de version sur l'écran texte
 * et attend une touche. Utile uniquement en développement.
 */
void DISPVERSION(void);


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES (déclarées ici pour la traduction)
 * ----------------------------------------------------------------------- */

/*
 * getreflect — Calcule les données du reflet dans le miroir.
 * Met à jour CharX, CharFace et dmirr (distance au miroir).
 * dmirr < 0 = joueur du mauvais côté (ne pas dessiner le reflet).
 */
void getreflect(void);

/*
 * CreateShad — Fait naître le shadowman depuis le miroir.
 * Appelé quand createshad == 0xFF.
 * Réinitialise CharID=1, positionne le shadowman, appelle SaveShad.
 * Initialise MaxOppStr = MaxKidStr, KidStrength = 1.
 */
void CreateShad(void);

/* Tables de sélection des jeux de tuiles par niveau */
extern const uint8_t bgset1[15];   /* jeu de bg (A) par niveau */
extern const uint8_t bgset2[15];   /* jeu de bg (B) par niveau */
extern const uint8_t chset[15];    /* jeu de sprites par niveau */

/* Tables de localisation des blueprints sur le disque */
extern const uint8_t bluepTRKlst[15]; /* piste par niveau */
extern const uint8_t bluepREGlst[15]; /* région par niveau */

#endif /* MISC_H */
