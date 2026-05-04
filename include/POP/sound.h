#ifndef SOUND_H
#define SOUND_H

/*
 * sound.h
 * Traduction de SOUND.S — Moteur sonore Apple II
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $ea00 (main LC bank 1) sur Apple II.
 *
 * -----------------------------------------------------------------------
 * NATURE DU FICHIER
 *
 * SOUND.S est le moteur audio bas-niveau du jeu. Il gère exclusivement
 * les effets sonores (SFX) via le haut-parleur 1-bit de l'Apple II
 * ($C030). La musique est gérée séparément par un système dédié
 * (MINIT/MPLAY dans GRAFIX.S, données dans la LC bank 1).
 *
 * L'Apple II ne possède qu'un seul haut-parleur piloté par un seul bit :
 * écrire en $C030 inverse la membrane. Tous les sons sont donc des
 * trains d'impulsions à fréquence et durée variables, produits par
 * la routine `tone` (boucles d'attente imbriquées).
 *
 * -----------------------------------------------------------------------
 * ARCHITECTURE
 *
 * Le système fonctionne en deux temps :
 *
 *   1. Durant le frame : ADDSOUND accumule les sons à jouer dans
 *      soundtable[] (max maxsfx entrées). Plusieurs sons peuvent
 *      être enregistrés dans le même frame.
 *
 *   2. En fin de frame : PLAYBACK lit soundtable[] et appelle
 *      makesound() pour chaque entrée, via la table d'adresses
 *      lookup[]. ZEROSOUND remet le compteur à 0.
 *
 * Note importante : SPECIALK.S redéfinit ADDSOUND et ZEROSOUND dans
 * sa propre jump table (et les implémente de façon identique). En pratique
 * tout le code du jeu appelle la version de SPECIALK.S via la jump table
 * de la main LC. SOUND.S est la définition originale, potentiellement
 * utilisée différemment selon la configuration mémoire.
 *
 * -----------------------------------------------------------------------
 * TRADUCTION C / SDL2
 *
 * `tone` génère des sons par busy-loop — incompatible avec SDL2.
 * En C, remplacer par SDL_QueueAudio ou des samples pré-générés.
 * Chaque DoXxx peut être remplacé par la lecture d'un fichier WAV
 * correspondant, ou par une génération procédurale avec SDL_AudioSpec.
 *
 * Les paramètres pitch/duration donnent des indications sur le son :
 *   pitch bas (1-10)    = son grave, court
 *   pitch élevé (1000+) = son aigu, très court (claquement)
 *   duration élevée     = son long
 * ----------------------------------------------------------------------- */

#include <stdint.h>
#include "soundnames.h"

/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES
 * ----------------------------------------------------------------------- */

/*
 * PLAYBACK — Joue tous les sons accumulés dans soundtable[].
 *
 * Parcourt soundtable[1..soundtable[0]] et appelle makesound()
 * pour chaque entrée. Sans effet si soundon == 0.
 * Appelé depuis MainLoop (TOPCTRL) en fin de chaque frame.
 */
void PLAYBACK(void);

/*
 * ADDSOUND — Ajoute un son à la file d'attente.
 *
 * In: A = numéro de son (voir soundnames.h, constantes PlateDown..JawsClash)
 * Sans effet si la file est pleine (soundtable[0] == maxsfx).
 * Préserve le registre X.
 *
 * Note : la version active en jeu est celle de SPECIALK.S (même code).
 */
void ADDSOUND(uint8_t sound_num);

/*
 * ZEROSOUND — Remet le compteur de la file à zéro (soundtable[0] = 0).
 * Appelé depuis MainLoop après PLAYBACK.
 *
 * Note : la version active en jeu est celle de SPECIALK.S (même code).
 */
void ZEROSOUND(void);


/* -----------------------------------------------------------------------
 * TABLE DE DISPATCH
 *
 * lookup[] mappe chaque numéro de son (0-19) vers sa routine.
 * Indexation : lookup[son * 2] = adresse de DoXxx.
 * En C : remplacer par un tableau de pointeurs de fonctions.
 * ----------------------------------------------------------------------- */

/* Prototype commun des routines de son */
typedef void (*SoundRoutine)(void);

/* Table de dispatch (20 entrées, indices 0-19) */
extern const SoundRoutine sound_lookup[20];


/* -----------------------------------------------------------------------
 * ROUTINES DE SON INDIVIDUELLES
 *
 * Chaque routine génère un son spécifique via tone().
 * Paramètres typiques : ldy=pitch_lo, ldx=pitch_hi, lda=duration.
 *
 * En C/SDL2 : remplacer par SDL_QueueAudio avec un sample approprié.
 * ----------------------------------------------------------------------- */

void DoPlateDown(void);      /* 0  : plaque de pression enfoncée */
void DoPlateUp(void);        /* 1  : plaque de pression remontée */
void DoGateDown(void);       /* 2  : portail frappe le sol */
void DoSpecialKey1(void);    /* 3  : confirmation touche spéciale */
void DoSpecialKey2(void);    /* 4  : confirmation touche spéciale (variante) */
void DoSplat(void);          /* 5  : chute mortelle / coupure / bruit sourd */
void DoMirrorCrack(void);    /* 6  : miroir brisé (alias DoSplat) */
void DoLooseCrash(void);     /* 7  : plancher qui s'écrase (alias DoSplat) */
void DoGotKey(void);         /* 8  : objet ramassé / flash message */
void DoFootstep(void);       /* 9  : pas du personnage */
void DoRaisingExit(void);    /* 10 : sortie qui s'ouvre */
void DoRaisingGate(void);    /* 11 : portail qui monte */
void DoLowerGate(void);      /* 12 : portail qui descend */
void DoSmackWall(void);      /* 13 : impact contre un mur */
void DoImpaled(void);        /* 14 : empalement (alias DoSmackWall) */
void DoGateSlam(void);       /* 15 : portail claque (alias DoSmackWall) */
void DoFlashMsg(void);       /* 16 : bip double (message clignotant) */
void DoSwordClash1(void);    /* 17 : choc d'épées (joueur) */
void DoSwordClash2(void);    /* 18 : choc d'épées (adversaire) */
void DoJawsClash(void);      /* 19 : trancheuse se ferme */


/* -----------------------------------------------------------------------
 * ROUTINE INTERNE
 * ----------------------------------------------------------------------- */

/*
 * tone — Génère un son par busy-loop sur le haut-parleur Apple II.
 *
 * In: y-x = pitch (lo-hi), a = durée
 *
 * Sur Apple II : boucle sur $C030 (toggle speaker) avec délai calculé
 * à partir de pitch et duration.
 *
 * En C/SDL2 : cette routine n'a pas d'équivalent direct. Remplacer
 * l'ensemble du module par un système basé sur SDL_OpenAudio.
 */
void tone(uint16_t pitch, uint8_t duration);

#endif /* SOUND_H */
