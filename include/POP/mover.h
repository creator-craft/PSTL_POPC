#ifndef MOVER_H
#define MOVER_H

/*
 * mover.h
 * Traduction de MOVER.S — Objets en transition (TROBs) et objets mobiles (MOBs)
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $ee00 (aux mem) sur Apple II.
 *
 * MOVER.S gère deux systèmes d'objets dynamiques :
 *
 * TROBs (Transitional Objects) — objets qui changent d'état progressivement :
 *   portails, trancheuses, plaques de pression, pics, planchers instables,
 *   torches, flacons, épées, sorties. Stockés dans les tableaux trloc[],
 *   trscrn[], trdirec[] (taille : trobspace).
 *   Mis à jour par ANIMTRANS chaque frame.
 *
 * MOBs (Mobile Objects) — objets qui se déplacent physiquement :
 *   uniquement les planchers détachés (falling floors). Stockés dans
 *   mobx[], moby[], mobscrn[], mobvel[], mobtype[], moblevel[]
 *   (taille : mobspace).
 *   Mis à jour par ANIMMOBS chaque frame.
 *
 * La distinction : un TROB change d'état dans le blueprint (BlueSpec),
 * un MOB a une position physique indépendante du blueprint.
 */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"
#include "movedata.h"
#include "soundnames.h"

/* -----------------------------------------------------------------------
 * CONSTANTES
 * ----------------------------------------------------------------------- */

/* Timers */
#define PPTIMER       5    /* frames avant qu'une plaque de pression remonte */
#define SPIKETIMER    (15 + 128) /* timer des pics (bit 7 = mode timer) */
#define SLICETIMER    15   /* frames entre deux passages de trancheuse */
#define GATETIMER     (GMAXVAL + 50) /* frames avant qu'un portail se referme */
#define LOOSETIMER    FFALLING  /* frames avant qu'un plancher se détache */

/* Physique des planchers qui tombent */
#define WIGGLETIME    4    /* frames de vibration avant détachement */
#define FF_ACCEL      3    /* accélération de chute */
#define FF_TERMVEL    29   /* vitesse terminale */
#define CRUMBLETIME   2    /* frames d'effondrement */
#define CRUMBLETIME2  10
#define DISAPPEARTIME 2
#define FF_HEIGHT     17   /* hauteur estimée d'un plancher (pixels) */
#define CRUSH_DIST    30   /* distance max pour écraser un personnage */

/* Hauteurs de wipe (zones à effacer lors du redraw) */
#define LOOSEWIPE     31
#define SPIKEWIPE     31
#define SLICERWIPE    63
#define PLATEWIPE     16

/* Vitesse de fermeture du portail (indexée par trdirec 0-8) */
extern const uint8_t gatevel[9]; /* = {0,0,0,20,40,60,80,100,120} */
#define MAXGATEVEL    8

/* Incréments d'état du portail selon trdirec (0=bas, 1=haut, 2=haut+bloqué) */
extern const int8_t gateinc[3]; /* = {-1, 4, 4} */

#define EXIT_INC      4
#define EMAXVAL       (43 * 4)  /* état max de la sortie (complètement ouverte) */


/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES — Jump table à $ee00
 * ----------------------------------------------------------------------- */

/*
 * ANIMTRANS — Met à jour tous les TROBs de la liste de transition.
 *
 * Pour chaque TROB, appelle animobj() qui dispatch vers la routine
 * d'animation appropriée (animgate, animplate, animspikes...).
 * Incrémente trobcount pour chaque TROB qui génère du mouvement visible
 * (utilisé par songcues pour éviter de jouer la musique pendant
 * qu'un objet bouge).
 * Supprime de la liste les TROBs dont trdirec est passé à -1 (stoppés).
 *
 * Appelé depuis NextFrame (TOPCTRL) en début de chaque frame.
 */
void ANIMTRANS(void);

/*
 * TRIGSPIKES — Déclenche les pics d'un bloc.
 *
 * In: résultats de rdblock (tempscrn, trloc depuis Y, BlueSpec)
 * Si les pics sont rétractés (state=0) : les ajoute à la trans list.
 * Si les pics sont étendus (hibit set) : remet le timer à spiketimer.
 * Si les pics sont en mouvement : ne fait rien.
 */
void TRIGSPIKES(void);

/*
 * PUSHPP — Appuie sur une plaque de pression.
 *
 * In: résultats de rdblock pour la plaque
 * Ajoute la plaque à la trans list, déclenche les objets liés
 * (portails, sorties) via trigger() → LINKLOC/LINKMAP.
 * Son: PlateDown. alertguard = 1.
 */
void PUSHPP(void);

/*
 * BREAKLOOSE1 — Déclenche un plancher instable avec un état initial donné.
 * In: A = état initial (1 = normal, ou valeur de timer)
 * Ne déclenche pas si le plancher est "required" (bit reqmask dans BlueType).
 */
void BREAKLOOSE1(uint8_t initial_state);

/*
 * BREAKLOOSE — Déclenche un plancher instable (état initial = 1).
 * Raccourci pour BREAKLOOSE1(1).
 */
void BREAKLOOSE(void);

/*
 * ANIMMOBS — Met à jour tous les MOBs (planchers qui tombent).
 *
 * Pour chaque MOB : applique la gravité, détecte collision avec le sol,
 * crée éventuellement un nouveau MOB (plancher rebondit sur un autre),
 * vérifie si un personnage est écrasé (checkcrush).
 * Supprime les MOBs terminés (mobvel == 0xFF).
 *
 * Appelé depuis NextFrame (TOPCTRL) après ANIMTRANS.
 */
void ANIMMOBS(void);

/*
 * ADDMOBS — Ajoute tous les MOBs visibles à la table d'objets pour l'affichage.
 * Appelé depuis DoFast (TOPCTRL) lors de l'assemblage du frame.
 */
void ADDMOBS(void);

/*
 * CLOSEEXIT — Ouvre la sortie en grand puis la laisse se refermer.
 * In: Y = trloc (position du bloc exit), A = trscrn
 * Utilisé pour l'animation d'entrée (slam shut) au début du niveau.
 */
void CLOSEEXIT(void);

/*
 * GETSPIKES — Retourne l'état des pics.
 * Out: 0 = rétractés (sûrs), 1 = étendus, 2 = en extension
 */
uint8_t GETSPIKES(void);

/*
 * SHAKEM — Secoue tous les planchers instables visibles sur un niveau.
 * In: A = CharBlockY (niveau à secouer)
 * Appelé depuis shakeloose (TOPCTRL) quand le joueur saute.
 */
void SHAKEM(uint8_t block_y);

/*
 * TRIGSLICER — Déclenche une trancheuse.
 * In: A = état initial (avec hibit = smear flag)
 *     Y = trloc, BlueSpec déjà pointé
 */
void TRIGSLICER(uint8_t initial_state);

/*
 * TRIGTORCH — Déclenche l'animation d'une torche.
 * In: Y = trloc, A = trscrn
 */
void TRIGTORCH(void);

/*
 * GETFLAMEFRAME — Avance d'un frame dans l'animation de flamme.
 * In/Out: A = état courant de la flamme (0 à torchLast)
 */
uint8_t GETFLAMEFRAME(uint8_t state);

/*
 * SMASHMIRROR — Casse le miroir (met BlueSpec à 86).
 * Appelé depuis CHECKCOLL quand le joueur traverse le miroir en courant.
 */
void SMASHMIRROR(void);

/*
 * JAMSPIKES — Bloque les pics dans l'état étendu (corps empalé).
 * Met BlueSpec à 0xFF et retire les pics de la trans list (trdirec=-1).
 */
void JAMSPIKES(void);

/*
 * TRIGFLASK — Déclenche l'animation d'un flacon.
 * Choisit un frame de départ aléatoire.
 */
void TRIGFLASK(void);

/*
 * GETFLASKFRAME — Avance d'un frame dans l'animation d'un flacon.
 * In/Out: A = frame courant (1 à bubbLast)
 */
uint8_t GETFLASKFRAME(uint8_t frame);

/*
 * TRIGSWORD — Déclenche l'animation d'une épée (scintillement).
 */
void TRIGSWORD(void);

/*
 * JAMPP — Bloque une plaque de pression sous un poids mort.
 * Transforme pressplate en dpressplate (permanently depressed),
 * ou upressplate en rubble.
 */
void JAMPP(void);


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES (déclarées pour la traduction)
 * ----------------------------------------------------------------------- */

/* Recherche un TROB dans la liste par (trloc, trscrn).
 * Out: X = index (0 si non trouvé) */
uint8_t searchtrob(void);

/* Ajoute un TROB à la liste, ou met à jour trdirec s'il y est déjà. */
void addtrob(void);

/* Anime le TROB d'index X (dispatch vers la routine appropriée). */
void animobj(uint8_t index);

/* Arrête un TROB (trdirec = -1). */
void stopobj(void);

/* Déclenche les objets liés via LINKLOC/LINKMAP. */
void trigger(void);

/* Anime chaque type d'objet */
void animgate(void);
void animplate(void);
void animspikes(void);
void animfloor(void);
void animslicer(void);
void animexit(void);
void animflask(void);
void animtorch(void);
void animsword(void);

/* Marque les blocs d'un TROB pour redraw */
void redgate(void);
void redplate(void);
void redspikes(void);
void redloose(void);
void redslicer(void);
void redexit(void);
void redflask(void);
void redtorch(void);
void redsword(void);

/* Gestion des MOBs */
void addamob(void);
void savemob(uint8_t index);
void loadmob(uint8_t index);
void animmob(void);      /* anime un MOB (plancher qui tombe) */
void mobfloor(void);     /* physique du plancher */
void checkcrush(void);   /* vérifie si un personnage est écrasé */
void crushchar(void);    /* écrase le personnage */
void makerubble(void);   /* transforme le sol en gravats */
void passthru(void);     /* passe au niveau de blocs suivant */
void knockloose(void);   /* un plancher tombe sur un autre plancher instable */
void makespace(void);    /* change l'objid en "espace vide" */

/* Lecture de LINKLOC/LINKMAP */
uint8_t gettimer(uint8_t link_index);
void    chgtimer(uint8_t link_index, uint8_t new_timer);
uint8_t getloc(uint8_t link_index);
uint8_t getlastflag(uint8_t link_index);
uint8_t getscrn(uint8_t link_index);

/* Vérifie si un bloc est visible et retourne son index dans redbuf */
uint8_t check(void);
uint8_t checkright(void);
uint8_t checkleft(void);
uint8_t checkabover(void);

#endif /* MOVER_H */
