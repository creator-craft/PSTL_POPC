#ifndef TOPCTRL_H
#define TOPCTRL_H

/*
 * topctrl.h
 * Traduction de TOPCTRL.S — Boucle principale et orchestration du jeu
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $2000 (aux mem) sur Apple II.
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

/* Disques */
#define POPSIDE1     0xa9   /* ID octet side 1 */
#define POPSIDE2     0xad   /* ID octet side 2 */
#define FIRSTSIDEB   3      /* premier niveau sur side B */
#define LASTSIDEB    14     /* dernier niveau sur side B */

/* Paramètres initiaux */
#define INITMAXSTR   3      /* force initiale du joueur */

/* Timers */
#define BTLTIMER     20     /* frames "back to life" */
#define WTLFLASH     15     /* frames flash de fin de weightlessness */
#define MOUSETIMER   150    /* durée de vie de la souris */

/* Messages */
#define LEVELMSG     1      /* message "Level #" */
#define CONTMSG      2      /* message "Press button to continue" */
#define TIMEMSG      3      /* message "# minutes left" */

#define LEVELTIMER   20     /* durée d'affichage du message de niveau */
#define CONTFLASH    95     /* timer à partir duquel le message clignote */
#define CONTOFF      15
#define DEADENOUGH   4      /* frames après mort avant message continue */

/* Miroir */
#define MIRLEVEL     4
#define MIRSCRN_TOP  4
#define MIRX_TOP     4
#define MIRY_TOP     0

/* Soft switches Apple II (simulés, non utilisés en C) */
/* ALTZPon = $c009, ALTZPoff = $c008, etc. */


/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES — Jump table à $2000
 * ----------------------------------------------------------------------- */

/* Démarrer une nouvelle partie. In: level = numéro de niveau (0=démo, 1=jeu) */
void START(uint8_t level);

/* Redémarrer le niveau courant */
void RESTART(void);

/* Reprendre une partie sauvegardée */
void STARTRESUME(void);

/* Initialisation système au démarrage */
void INITSYSTEM(void);

/* Aller en mode démo (attract mode) */
void GOATTRACT(void);

/* Lancer une cinématique "coupe" entre deux scènes */
void docrosscut(void);


/* -----------------------------------------------------------------------
 * BOUCLE PRINCIPALE
 * ----------------------------------------------------------------------- */

/* Boucle principale du jeu (ne retourne jamais) */
void MainLoop(void);

/* Calcule ce que doit être le prochain frame (logique pure, pas d'affichage) */
void NextFrame(void);

/* Dessine le prochain frame sur la page cachée et l'affiche */
void FrameAdv(void);

/* Génère et affiche le tout premier frame d'un niveau */
void FirstFrame(void);


/* -----------------------------------------------------------------------
 * MISE À JOUR DES PERSONNAGES
 * ----------------------------------------------------------------------- */

/* Met à jour le joueur pour ce frame */
void DoKid(void);

/* Met à jour l'adversaire (shadowman/garde) pour ce frame */
void DoShad(void);

/* Ajoute tous les personnages visibles à la table d'objets */
void addchars(void);

/* Ajoute le joueur à la table d'objets (crop, index, mark) */
void setupkid(void);

/* Ajoute l'adversaire à la table d'objets */
void setupshad(void);


/* -----------------------------------------------------------------------
 * GESTION DES ÉCRANS
 * ----------------------------------------------------------------------- */

/* Dessine le background du nouvel écran (avant les personnages) */
void drawbg(void);

/* Redessine entièrement le background depuis le blueprint */
void DoSure(void);

/* Mise à jour rapide : redessine seulement les blocs modifiés */
void DoFast(void);

/* Transition vers un nouvel écran (version propre, attend la fin du frame) */
void DoCleanCut(void);

/* Prépare la transition vers cutscrn si différent de VisScrn */
void PrepCut(void);

/* Charge le niveau suivant */
void LoadNextLevel(void);


/* -----------------------------------------------------------------------
 * CONTRÔLE DU JOUEUR (routines internes à TOPCTRL)
 * ----------------------------------------------------------------------- */

/* Gère l'état du joueur : mort progressive, message continue */
void ctrlplayer(void);

/* Si le personnage est sur l'écran 0, le tue */
void kill0(void);

/* Joue la musique de mort appropriée */
void deathsong(void);


/* -----------------------------------------------------------------------
 * FIN DE PARTIE
 * ----------------------------------------------------------------------- */

/* Le joueur a gagné */
void YouWin(void);

/* Le temps est écoulé, le joueur a perdu */
void YouLose(void);


/* -----------------------------------------------------------------------
 * UTILITAIRES INTERNES
 * ----------------------------------------------------------------------- */

/* Initialise les variables avant de démarrer une partie */
void initgame(void);

/* Démarre une partie. In: A = level # */
void StartGame(uint8_t level);

/* Reprend une partie sauvegardée */
void ResumeGame(void);

/* Initialise les buffers de collision detection */
void initCDbuf(void);

/* Ferme la porte d'entrée du niveau */
void entrance(void);

/* Joue les musiques en attente (SongCue) */
void songcues(void);

/* Ajoute les effets sonores de choc d'épées si nécessaire */
void addsfx(void);

/* Affiche le message courant (niveau, continue, temps) */
void dispmsg(void);

/* Timers divers appelés à chaque frame (mergetimer, souris...) */
void misctimers(void);

/* Gestion des planchers instables (secoue si le joueur saute) */
void shakeloose(void);

/* Marque les blocs des barres de force pour redraw si changées */
void checkmeters(void);

/* Applique les changements de force (ChgKidStr, ChgOppStr) */
void chgmeters(void);

/* Affiche un flash lumineux lors d'un impact */
void flashon(void);
void flashoff(void);

/* Flash de fin de potion de légèreté */
void wtlessflash(void);

/* Vérification de protection contre la copie (yellow) */
void yellowcheck(uint8_t next_level);

/* Patch de redéveloppement (redraw forcé) */
void develpatch(void);

/* Demande au joueur de retourner le disque */
void flipdisk(void);

/*
 * Test si un personnage est statique (ne bouge pas)
 * In: A = CharPosn
 * Out: 0 si statique, 1 si en mouvement
 */
uint8_t static_check(uint8_t posn);

/*
 * Test si un personnage est "froid" (état de mort terminal)
 * In: A = CharPosn
 * Out: 0 si oui, 1 si non
 */
uint8_t cold_check(uint8_t posn);

/* Efface les flags de direction joystick */
void clearjoy(void);

/* Gestion de la réflexion dans le miroir */
void reflection_draw(void);

#endif /* TOPCTRL_H */
