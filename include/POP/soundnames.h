#ifndef SOUNDNAMES_H
#define SOUNDNAMES_H

/*
 * soundnames.h
 * Traduction de SOUNDNAMES.S : Identifiants des effets sonores et morceaux musicaux
 * Source originale : Jordan Mechner, 1989
 *
 * Ce fichier ne contient QUE des #define.
 *
 * DEUX GROUPES :
 *
 * 1. Effets sonores (0-19) : indices dans soundtable[] (défini dans GAMEEQ.S).
 *    Utilisés via addsound(sfx_id). Déclenchés à des moments précis du jeu.
 *
 * 2. Morceaux musicaux (préfixe s_) : passés à cuesong(song_id, volume).
 *    Deux sous-groupes : musique de jeu et musique du titre.
 *    Certains indices se chevauchent entre les deux sous-groupes (s_Princess=7
 *    et s_Vict=7 sont des morceaux différents selon le contexte).
 */


/* -----------------------------------------------------------------------
 * EFFETS SONORES
 * Indices dans soundtable[], utilisés avec addsound().
 * ----------------------------------------------------------------------- */
#define PlateDown      0    /* plaque de pression s'abaisse */
#define PlateUp        1    /* plaque de pression se relève */
#define GateDown       2    /* portail descend */
#define SpecialKey1    3    /* son spécial 1 */
#define SpecialKey2    4    /* son spécial 2 */
#define Splat          5    /* chute mortelle / écrasement */
#define MirrorCrack    6    /* miroir se brise */
#define LooseCrash     7    /* plancher instable s'effondre */
#define GotKey         8    /* clé / objet récupéré */
#define Footstep       9    /* pas */
#define RaisingExit   10    /* sortie qui s'ouvre */
#define RaisingGate   11    /* portail qui monte */
#define LoweringGate  12    /* portail qui descend */
#define SmackWall     13    /* choc contre un mur */
#define Impaled       14    /* empalé sur des pics */
#define GateSlam      15    /* portail qui claque */
#define FlashMsg      16    /* message flash */
#define SwordClash1   17    /* choc d'épées 1 */
#define SwordClash2   18    /* choc d'épées 2 */
#define JawsClash     19    /* mâchoires de la trancheuse */


/* -----------------------------------------------------------------------
 * MUSIQUE DE JEU
 * Passés à cuesong(s_XXX, volume) pendant le gameplay.
 * ----------------------------------------------------------------------- */
#define s_Accid        1    /* accident / événement inattendu */
#define s_Heroic       2    /* thème héroïque */
#define s_Danger       3    /* thème de danger */
#define s_Sword        4    /* combat à l'épée */
#define s_Rejoin       5    /* réunion du héros et de son ombre */
#define s_Shadow       6    /* thème du shadow man */
#define s_Vict         7    /* victoire */
#define s_Stairs       8    /* montée des escaliers */
#define s_Upstairs     9    /* en haut des escaliers */
#define s_Jaffar      10    /* thème de Jaffar (vizir) */
#define s_Potion      11    /* potion bue */
#define s_ShortPot    12    /* potion courte */
#define s_Timer       13    /* temps qui s'écoule */
#define s_Tragic      14    /* thème tragique */
#define s_Embrace     15    /* étreinte (scène finale) */
#define s_Heartbeat   16    /* battement de cœur */


/* -----------------------------------------------------------------------
 * MUSIQUE DU TITRE / ÉCRAN D'ACCUEIL
 * Même fonction cuesong(), mais dans un contexte différent.
 * Note : les indices 7-12 chevauchent ceux de la musique de jeu 
 * le contexte d'appel (attract mode vs gameplay) détermine lequel jouer.
 * ----------------------------------------------------------------------- */
#define s_Princess     7    /* thème de la princesse (titre) */
#define s_Squeek       8    /* son aigu (titre) */
#define s_Vizier       9    /* thème du vizir (titre) */
#define s_Buildup     10    /* montée dramatique (titre) */
#define s_Magic       11    /* thème magique (titre) */
#define s_StTimer     12    /* timer du titre */

#endif /* SOUNDNAMES_H */
