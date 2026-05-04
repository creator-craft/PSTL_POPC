#ifndef AUTO_H
#define AUTO_H

/*
 * auto.h
 * Traduction de AUTO.S : Intelligence artificielle et gestion des transitions
 * Source originale : Jordan Mechner, 1989
 * Module chargé à l'adresse $5400 (aux mem) sur Apple II.
 *
 * Ce fichier déclare :
 *   - Les 8 routines publiques (jump table)
 *   - Les routines internes (IA par personnage, combat, transitions)
 *   - Les constantes de combat et de seuils
 *   - Les tables de paramètres des 12 programmes de garde
 *   - Les positions pré-définies du shadowman
 *   - Le système de playback de séquences
 */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"

/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES : Jump table à $5400
 * Ces 8 fonctions sont appelées depuis les autres modules via la jump table.
 * ----------------------------------------------------------------------- */

/* Dispatcheur principal de l'IA : lit CharID et appelle le bon programme.
 * Doit être appelé une fois par frame pour le personnage adversaire. */
void AUTOCTRL(void);

/* Vérifie le contact entre les épées (frames de strike/block).
 * Travaille sur les positions du frame SUIVANT (upcoming frame). */
void CHECKSTRIKE(void);

/* Applique les effets d'un coup d'épée réussi (StabChar).
 * Gère aussi le cas double-stab (égalité = joueur gagne). */
void CHECKSTAB(void);

/* Rejoue une séquence de commandes pré-enregistrée (frame# + commande).
 * In: A-X = adresse du programme, PlayCount = frame courant. */
void AUTOPLAYBACK(void);

/* Vérifie si le joueur doit transiter vers un écran adjacent.
 * Gère le transfert ou la mise à jour du garde sur l'ancien écran. */
void CUTCHECK(void);

/* Rattrape un garde qui serait tombé hors de l'écran par le bas. */
void CUTGUARD(void);

/* Initialise le garde (ou le shadowman) sur le nouvel écran après un cut. */
void ADDGUARD(void);

/* Déplace un personnage vers l'écran adjacent dans la direction donnée.
 * In: A = direction (0=gauche, 1=droite, 2=haut, 3=bas).
 * Modifie CharScrn, CharX/Y, CharBlockY. */
void CUT(uint8_t direction);


/* -----------------------------------------------------------------------
 * CONSTANTES : Seuils de découpe d'écran (cut edges)
 * Définissent les limites au-delà desquelles un personnage transite.
 * Basés sur ScrnTop/ScrnBottom/ScrnLeft/ScrnRight définis dans eq.h.
 * ----------------------------------------------------------------------- */
#define TopCutEdgePl  (ScrnTop + 10)     /* seuil haut pour transition vers le haut */
#define TopCutEdgeMi  (ScrnTop - 16)     /* seuil haut négatif (wrap) */
#define BotCutEdge    (ScrnBottom + 24)  /* seuil bas pour transition vers le bas */
#define LeftCutEdge   (ScrnLeft - 4)     /* seuil gauche */
#define RightCutEdge  (ScrnRight + 4)    /* seuil droit */


/* -----------------------------------------------------------------------
 * CONSTANTES : Emplacements des objets spéciaux dans les niveaux
 * Codés en dur dans AUTO.S, liés à la structure des niveaux du jeu.
 * ----------------------------------------------------------------------- */
#define FLASK_SCRN   24   /* screen de la potion (niveau 5) */
#define FLASK_X       3
#define FLASK_Y       0

#define MIR_SCRN      4   /* screen du miroir (niveau 4) */
#define MIR_X         4
#define MIR_Y         0

#define SWORD_SCRN   15   /* screen de l'épée (niveau 12) */
#define SWORD_X       1
#define SWORD_Y       0

#define MOUSETIMER  150   /* durée de vie de la souris (frames), défini dans topctrl */


/* -----------------------------------------------------------------------
 * CONSTANTES : Seuils et plages de combat
 * Distances en pixels (système 140px) utilisées dans la logique de combat.
 * ----------------------------------------------------------------------- */

/* Plages de détection du strike/block */
#define STRIKERANGE1   12   /* distance min pour qu'un strike touche */
#define STRIKERANGE2   29   /* distance max pour qu'un strike touche */
#define BLOCKRANGE1     0   /* distance min pour qu'un block soit valide */
#define BLOCKRANGE2    29   /* distance max (from TestStrike) */

/* Seuils de décision de l'IA */
#define SWORDTHRES     90   /* distance à laquelle le garde sort l'épée */
#define ENGARDETHRES   60
#define STRIKETHRES1   STRIKERANGE1
#define STRIKETHRES2   STRIKERANGE2
#define BLOCKTHRES1    10
#define BLOCKTHRES2    BLOCKRANGE2
#define TOOCLOSE       STRIKETHRES1
#define TOOFAR         (STRIKETHRES2 + 6)  /* dist min pour avancer en sécurité */
#define OFFGUARDTHRES   8
#define JUMPTHRES      50   /* si kid saute et est à moins de JUMPTHRES → frapper */
#define RUNTHRES       40
#define BLOCKTIME       4   /* durée d'affaiblissement du blocage après avoir paré */


/* -----------------------------------------------------------------------
 * TABLES DE PARAMÈTRES DES 12 PROGRAMMES DE GARDE
 *
 * Chaque garde possède un "guardprog" (0-11) qui indexe ces tables.
 * Les probabilités sont sur 255 (0 = jamais, 255 = toujours).
 *
 * Programme 0  : garde stupide (ne bloque jamais)
 * Programme 7  : garde très agressif (frappe souvent, bloque bien)
 * Programme 8  : programme spécial (ne fait rien = souris/démo)
 * Programmes 9-11 : variantes avancées (vizier, squelette...)
 * ----------------------------------------------------------------------- */
#define NUM_GUARD_PROGS  12

/* Probabilité de frapper depuis position prête */
extern const uint8_t strikeprob[NUM_GUARD_PROGS];

/* Probabilité de re-frapper après avoir bloqué */
extern const uint8_t restrikeprob[NUM_GUARD_PROGS];

/* Probabilité de bloquer le coup adverse */
extern const uint8_t blockprob[NUM_GUARD_PROGS];

/* Probabilité de bloquer quand déjà affaibli (après avoir bloqué) */
extern const uint8_t impblockprob[NUM_GUARD_PROGS];

/* Probabilité d'avancer vers le joueur */
extern const uint8_t advprob[NUM_GUARD_PROGS];

/* Durée (en frames) de la période réfractaire après avoir reçu un coup */
extern const uint8_t refractimer[NUM_GUARD_PROGS];

/* 0 = couleur de base du niveau, 1 = couleur spéciale */
extern const uint8_t specialcolor[NUM_GUARD_PROGS];

/* Points de force supplémentaires au-dessus de basicstrength du niveau */
extern const uint8_t extrastrength[NUM_GUARD_PROGS];


/* -----------------------------------------------------------------------
 * FORCE ET COULEUR DE BASE PAR NIVEAU
 * Indexées par le numéro de niveau (0-13).
 * La couleur effective = basiccolor[level] XOR specialcolor[guardprog].
 * ----------------------------------------------------------------------- */
extern const uint8_t basicstrength[14];  /* force de base du garde selon le niveau */
extern const uint8_t basiccolor[14];     /* 0 = bleu, 1 = rouge */

#define SHADSTRENGTH  4   /* force fixe du shadowman */


/* -----------------------------------------------------------------------
 * COMMANDES DE PLAYBACK
 *
 * Format d'une séquence pré-enregistrée :
 *   { frame#, commande, frame#, commande, ..., 255, EndProg }
 * Chaque paire indique quelle commande joystick exécuter à partir du frame#.
 * ----------------------------------------------------------------------- */
#define CMD_ENDPROG  (-2)   /* fin de la séquence programmée */
#define CMD_ENDDEMO  (-1)   /* fin de la démo */
#define CMD_CTR        0    /* centrer joystick et relâcher bouton */
#define CMD_FWD        1    /* joystick en avant */
#define CMD_BACK       2    /* joystick en arrière */
#define CMD_UP         3    /* joystick en haut */
#define CMD_DOWN       4    /* joystick en bas */
#define CMD_UPFWD      5    /* joystick haut + avant */
#define CMD_PRESS      6    /* appuyer et maintenir bouton */
#define CMD_RELEASE    7    /* relâcher bouton */

/* Séquence pré-enregistrée : niveau 5 (shadowman vole la potion) */
extern const uint8_t ShadProg5[];


/* -----------------------------------------------------------------------
 * POSITIONS INITIALES DU SHADOWMAN (shadpos)
 *
 * Chaque shadpos est un tableau de 8 octets :
 *   [Posn, X, Y, Face, BlockX, BlockY, Action, SeqEntry]
 * Utilisé par chgshadposn / csps pour téléporter le shadowman.
 * ----------------------------------------------------------------------- */
extern const uint8_t shadpos6a[8];   /* niveau 6 screen 1 : shadowman debout */
extern const uint8_t shadpos5[8];    /* niveau 5 screen 24 : à gauche hors-écran */
extern const uint8_t shadpos12[8];   /* niveau 12 screen 15 : chute depuis le haut */


/* -----------------------------------------------------------------------
 * VARIABLES INTERNES (zero page $f0, locales à AUTO.S)
 *
 * Sur Apple II, ces variables sont dans la zone locale de la zero page.
 * En C, on les déclare comme statiques dans auto.c pour préserver
 * l'encapsulation (elles ne sont pas utilisées hors de ce module).
 * ----------------------------------------------------------------------- */
/* static uint8_t ztemp;       -- variable temporaire générale */
/* static uint8_t prob;        -- valeur de probabilité courante */
/* static uint8_t cutdir;      -- direction du cut courant (0-3) */
/* static uint8_t ProgStart[2];-- adresse du programme de playback courant */


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES : Programmes par personnage
 * Non exposées dans la jump table, mais définies dans auto.c.
 * ----------------------------------------------------------------------- */

/* Dispatcher interne appelé par AUTOCTRL */
void GuardProg(void);     /* garde standard + vizier + squelette en garde */
void ShadowProg(void);    /* shadowman (logique spéciale par niveau) */
void SkelProg(void);      /* squelette : force CharSword=2 puis GuardProg */
void VizierProg(void);    /* vizier : alias de GuardProg */
void KidProg(void);       /* joueur en mode démo : alias de GuardProg */
void MouseProg(void);     /* souris : avance jusqu'à x=200 puis disparaît */

/* Logiques du shadowman par niveau */
void ShadLevel4(void);    /* niveau 4 (miroir) : courir hors-écran */
void ShadLevel5(void);    /* niveau 5 (voleur) : playback pour voler potion */
void ShadLevel6(void);    /* niveau 6 (plongeon) : appuyer sur plaque */
void FinalShad(void);     /* niveau 12 (combat final) : logique de fusion */


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES : Combat
 * ----------------------------------------------------------------------- */

/* Teste si le coup d'épée de Char touche Op (frames 153/154) */
void TestStrike(void);

/* Logique générale de combat en garde (face à face, distance correcte) */
void EnGarde(void);
void GenFight(void);

/* Décisions conditionnelles de l'IA en combat */
void Alert(void);         /* pré-garde : se retourner / passer en garde */
void InRange(void);       /* à portée de frappe */
void FollowKid(void);     /* poursuivre le joueur qui a sauté en bas */
void MaybeAdvance(void);  /* décide d'avancer selon advprob */
void MaybeBlock(void);    /* décide de bloquer selon blockprob */
void MaybeStrike(void);   /* décide de frapper selon strikeprob */


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES : Commandes joystick simulées
 *
 * L'IA simule un joueur humain en écrivant directement dans les variables
 * de joystick (JSTKX, JSTKY, btn, clrF, clrB, clrU, clrD, clrbtn).
 * Ces fonctions sont les "touches" que l'IA peut "appuyer".
 * ----------------------------------------------------------------------- */
void DoRelease(void);     /* tout relâcher (JSTKX=0, JSTKY=0, btn=0...) */
void DoAdvance(void);     /* = DoFwd : avancer (clrF=-1, JSTKX=-1) */
void DoFwd(void);
void DoRetreat(void);     /* = DoBack : reculer (clrB=-1, JSTKX=1) */
void DoBack(void);
void DoBlock(void);       /* = DoUp : joystick haut (clrU=-1, JSTKY=-1) */
void DoUp(void);
void DoTurn(void);        /* = DoDown : joystick bas (clrD=-1, JSTKY=1) */
void DoDown(void);
void DoStandup(void);     /* haut + reculer */
void DoDropguard(void);   /* = DoRunaway : bas + reculer */
void DoRunaway(void);
void DoEngarde(void);     /* bas + avancer : passer en garde */
void DoStrike(void);      /* = DoPress : appuyer bouton (clrbtn=-1, btn=-1) */
void DoPress(void);
void DoRelBtn(void);      /* relâcher bouton (btn=0) */


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES : Transitions d'écran
 * ----------------------------------------------------------------------- */

/* Vérifie si le personnage est en train de sortir de l'écran courant.
 * Retourne la direction du cut (-1 si aucun cut). */
int8_t cutchar(void);

/* Transfère le garde vers le nouvel écran (supprime ses données de l'ancien) */
void transferguard(void);

/* Met à jour les coordonnées du garde sur l'ancien écran avant de partir */
void updateguard(void);

/* Initialise un garde normal à partir des données GdStart* du blueprint */
void AddNormalGd(void);

/* Récupère la force du garde selon le niveau et son programme */
void getgdstrength(void);


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES : Utilitaires
 * ----------------------------------------------------------------------- */

/* Retourne X = guardprog, A = nombre aléatoire (appelle rnd) */
void rndp(void);

/* Change la position du shadowman vers une shadpos donnée */
void chgshadposn(const uint8_t *shadpos);

/* Change la position du shadowman ET initialise guardprog, OppStrength */
void csps(const uint8_t *shadpos);

/* Vérifie si on est au milestone du niveau 3 (passage vers le screen 7) */
void milestone3(void);

/* Niveau 12 : le shadow vole l'épée quand le joueur passe sous sword_scrn */
void stealsword(void);

/* Niveau 13 : joue le thème de Jaffar lors de l'entrée screen 3 */
void jaffmusic(void);

/* Niveau 4 : joue le thème de danger près du miroir */
void mirrmusic(void);

/* Niveau 12 : hook vide (chute infinie désactivée) */
void infinity(void);

/* Sous-routines de déplacement dans CUT */
void Cleft(void);
void Cright(void);
void Cup(void);
void Cdown(void);


/* -----------------------------------------------------------------------
 * DIRECTION CONSTANTS : utilisées par CUT et cutchar
 * ----------------------------------------------------------------------- */
#define CUT_LEFT   0
#define CUT_RIGHT  1
#define CUT_UP     2
#define CUT_DOWN   3

#endif /* AUTO_H */
