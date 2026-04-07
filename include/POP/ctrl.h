#ifndef CTRL_H
#define CTRL_H

/*
 * ctrl.h
 * Traduction de CTRL.S — Contrôle du personnage joueur et physique
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $3a00 (aux mem) sur Apple II.
 */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"
#include "seqdata.h"
#include "movedata.h"
#include "soundnames.h"

/* -----------------------------------------------------------------------
 * CONSTANTES PHYSIQUES
 * ----------------------------------------------------------------------- */

/* Vitesses de chute */
#define DEATH_VELOCITY  33  /* vitesse Y au-delà de laquelle la chute est mortelle */
#define OOF_VELOCITY    22  /* vitesse Y au-delà de laquelle la chute fait mal */

/* Paramètres d'accrochage à un rebord */
#define GRABREACH    (-8)   /* offset X pour tester si un rebord est atteignable */
#define GRABSPEED    32     /* vitesse Y max pour pouvoir s'accrocher */
#define GRABLEAD     25     /* anticipation verticale pour l'accrochage */
#define STUNTIME     12     /* frames de paralysie après accrochage */

/* Paramètres de saut */
#define JUMPUPREACH  0
#define JUMPUPANGLE  (-6)
#define JUMPBACKTHRES 6     /* distance min pour faire un "jump back" */

/* Paramètres de pas en avant / descente de rebord */
#define STEPOFF_FWD  3      /* distance max pour "step off" vers l'avant */
#define STEPOFF_BACK 8      /* distance max pour descendre d'un rebord */

/* Seuils de combat */
#define SWORDTHRES    90    /* distance pour passer en garde (face à l'ennemi) */
#define SWORDTHRES_N  (-10) /* distance pour passer en garde (ennemi derrière) */
#define BLOCKTHRES    32    /* distance max pour bloquer */
#define GRACEPERIOD   9     /* frames de grâce après avoir baissé la garde */
#define GDPATIENCE    15    /* timer de patience du garde */

/* Paramètre d'escalier */
#define GCLIMBTHRES  6      /* ouverture min d'un portail pour passer dessous */
#define STAIRTHRES   30     /* état min d'un bloc exit pour grimper les escaliers */

/* Saut en course */
#define RJCHANGE     4      /* variation X projetée lors d'un runjump */
#define RJLOOKAHEAD  1      /* nombre de blocs de look-ahead */
#define RJLEADDIST   14     /* distance de lancement requise en pixels */
#define RJMAXFUJBAK  8      /* pixels de marge de correction arrière */
#define RJMAXFUJFWD  2      /* pixels de marge de correction avant */


/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES — Jump table à $3a00
 * ----------------------------------------------------------------------- */

/* Contrôle du joueur humain (lit le joystick sélectionné) */
void PLAYERCTRL(void);

/* Vérifie s'il y a du sol sous les pieds, déclenche la chute si non */
void CHECKFLOOR(void);

/* Contrôle de l'adversaire (appelle AutoCtrl puis GenCtrl) */
void SHADCTRL(void);

/* Relit les blocs autour du personnage (GetFrameInfo + GetBaseBlock) */
void REREADBLOCKS(void);

/* Vérifie si le personnage est sur une plaque de pression ou un plancher instable */
void CHECKPRESS(void);

/* Empale le personnage sur des pics */
void DOIMPALE(void);

/* Contrôle général : lit les inputs et met à jour CharSeq selon l'état courant */
void GENCTRL(void);

/* Vérifie si le personnage est dans une zone de pics (empalement en course/saut) */
void CHECKIMPALE(void);


/* -----------------------------------------------------------------------
 * PHYSIQUE DE CHUTE
 * ----------------------------------------------------------------------- */

/* Gère la physique quand le personnage est en chute libre (Action=4) */
void falling(void);

/* Le personnage touche le sol : choisit l'animation d'atterrissage */
void hitflr(void);

/* Le personnage est encore en l'air : peut-il s'accrocher à un rebord ? */
void fallon(void);

/* Déclenche la séquence de chute depuis le sol (startfall) */
void startfall(void);

/* Personnage "à l'intérieur" d'un bloc : le repose d'un côté ou de l'autre */
void InsideBlock(void);


/* -----------------------------------------------------------------------
 * MACHINE À ÉTATS — GENCTRL
 * Ces routines sont appelées par GENCTRL selon CharPosn/CharAction.
 * ----------------------------------------------------------------------- */

void standing(void);    /* debout : répond aux inputs de base */
void starting(void);    /* début de course : détecte le saut */
void stjumpup(void);    /* début de saut vers le haut */
void arunning(void);    /* en course : saut, virage, arrêt, roulade */
void hanging(void);     /* suspendu : grimper, lâcher, basculer */
void turning(void);     /* en train de se retourner */
void crouching(void);   /* accroupi : ramper, se relever, ramasser */

/* Contrôle spécial pour les gardes (alertstand → en garde) */
void GuardCtrl(void);

/* Contrôle en mode combat (CharSword=2) */
void FightCtrl(void);


/* -----------------------------------------------------------------------
 * ACTIONS DÉCLENCHÉES PAR GENCTRL
 * Ces routines appellent jumpseq avec la bonne séquence.
 * ----------------------------------------------------------------------- */

void DoStartrun(void);    /* commencer à courir */
void DoTurn(void);        /* demi-tour */
void DoStandjump(void);   /* saut depuis l'arrêt */
void DoRunjump(void);     /* saut en course (avec calibration X) */
void DoSdiveroll(void);   /* roulade plongeon depuis l'arrêt */
void DoCrouch(void);      /* s'accroupir */
void DoEngarde(void);     /* passer en garde */
void DoJumpup(void);      /* saut vers le haut (avec détection rebord) */
void DoJumphang(void);    /* saut + accrochage */
void DoJumphigh(void);    /* grand saut (pas de rebord) */
void DoJumpedge(void);    /* saut en arrière pour attraper un rebord */
void DoStepfwd(void);     /* pas en avant */
void Stairs(void);        /* monter un escalier */

/* Actions de combat */
void DoBlock(void);       /* parer */
void DoStrike(void);      /* frapper */
void DoRetreat(void);     /* reculer */
void DoAdvance(void);     /* avancer */
void DoTurnEng(void);     /* demi-tour en garde */

/* Contrôle en mode démo */
void DemoCtrl(void);

/* Contrôle utilisateur (tient compte de CharFace pour inverser les inputs) */
void UserCtrl(uint8_t param);

/* Efface tous les flags de direction */
void clrall(void);


/* -----------------------------------------------------------------------
 * RAMASSAGE D'OBJETS
 * ----------------------------------------------------------------------- */

/*
 * Tente de ramasser un objet (flask ou sword).
 * Return: 0 si rien, 1 si objet ramassé.
 */
uint8_t TryPickup(void);

/* Ramasse effectivement l'objet devant le personnage */
void PickItUp(void);

#endif /* CTRL_H */
