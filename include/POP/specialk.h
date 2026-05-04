#ifndef SPECIALK_H
#define SPECIALK_H

/*
 * specialk.h
 * Traduction de SPECIALK.S — Input, son, temps et utilitaires divers
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $d900 (main LC bank 1) sur Apple II.
 *
 * Malgré son nom, SPECIALK.S n'est pas un module d'effets spéciaux.
 * C'est le module qui regroupe :
 *   1. La gestion des inputs (clavier, joystick)
 *   2. Le système de sons (addsound, zerosound)
 *   3. La gestion du temps (keeptime, getminleft, shortentime)
 *   4. Des utilitaires globaux (cuesong, strobe, dloop)
 *   5. Les touches de développement (cheat codes, debug keys)
 *
 * Beaucoup de routines de ce fichier sont ré-exportées dans la jump table
 * même si elles sont définies ailleurs (decstr, DoSaveGame, LoadLevelX,
 * potioneffect, checkalert...) — SPECIALK.S sert de point d'entrée
 * centralisé depuis la main LC vers les autres modules en aux LC.
 *
 * Note sur la constante min :
 *   SPECIALK.S utilise min=725 frames/"minute", SUBS.S utilise min=1090.
 *   Les deux coexistaient selon le flag CheckTimer. La valeur retenue
 *   pour la traduction C est 725 (valeur de SPECIALK, module actif).
 */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"
#include "soundnames.h"
#include "movedata.h"

/* -----------------------------------------------------------------------
 * CONSTANTES
 * ----------------------------------------------------------------------- */

/* Timing */
#define MIN_FRAMES_SK   725   /* frames par "minute" de jeu */
#define SEC_FRAMES_SK   (MIN_FRAMES_SK / 60)
#define TIME_LIMIT_SK   60    /* limite de temps en "minutes" */

/* Timer du cheat "antimatter" */
#define INIT_AM_TIMER   10

/* Codes ASCII touches de contrôle */
#define KEY_CTRL        0x60
#define KEY_ESC         0x9b
#define KEY_DELETE      0x7f
#define KEY_SHIFT       0x20

/* Touches de jeu légitimes */
#define KLEFT           'j'
#define KDOWN           'k'
#define KRIGHT          'l'
#define KUPLEFT         'u'
#define KUP             'i'
#define KUPRIGHT        'o'

/* Touches spéciales (version finale) */
#define KFREEZE         KEY_ESC
#define KSOUND          ('s' - KEY_CTRL)
#define KMUSIC          ('n' - KEY_CTRL)
#define KSAVEGAME       ('g' - KEY_CTRL)
#define KSHOWTIME       ' '
#define KFLIPX          ('x' - KEY_CTRL)
#define KFLIPY          ('y' - KEY_CTRL)
#define KVERSION        ('v' - KEY_CTRL)


/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES — Jump table à $d900
 * ----------------------------------------------------------------------- */

/*
 * KEYS — Détecte et traite les appuis de touches.
 *
 * Lit le clavier Apple II ($C000), stocke dans keypress et keydown.
 * Appelle KREAD (contrôle clavier du joueur → kbdX, kbdY).
 * Gère la touche ESC (mode pas-à-pas SINGSTEP).
 * Dispatch vers LegitKeys (touches normales) et DevelKeys (debug).
 */
void KEYS(void);

/*
 * CLRJSTK — Met à jour les flags clr (clrF, clrB, clrU, clrD, clrbtn).
 *
 * Système de "smart input" : détecte les appuis frais vs les appuis
 * maintenus. clr = -1 = appui frais non consommé, 0 = relâché,
 * 1 = consommé par GenCtrl.
 *
 * Supposé appelé avec le personnage face à gauche (sens normal).
 * FACEJSTK inverse JSTKX et clrF/clrB avant d'appeler GENCTRL
 * si le personnage est face à droite.
 */
void CLRJSTK(void);

/*
 * ZEROSOUND — Remet la table de sons à zéro (soundtable[0] = 0).
 * Appelé en fin de chaque frame après playback().
 */
void ZEROSOUND(void);

/*
 * ADDSOUND — Ajoute un son à la table d'attente.
 *
 * In: A = numéro de son (voir soundnames.h)
 * La table est lue par playback() en fin de frame.
 * Sans effet si la table est pleine (maxsfx atteint).
 * Préserve X.
 */
void ADDSOUND(uint8_t sound_num);

/*
 * FACEJSTK — Inverse JSTKX et échange clrF/clrB.
 *
 * Appelé par UserCtrl (CTRL.S) avant et après GENCTRL quand le
 * personnage est face à droite (CharFace == 0x00), pour que GENCTRL
 * travaille toujours comme si le personnage regardait à gauche.
 */
void FACEJSTK(void);

/*
 * SAVESELECT / LOADSELECT — Sauvegarde/restaure les flags clr du
 * dispositif sélectionné (joueur humain) dans clrSEL[].
 */
void SAVESELECT(void);
void LOADSELECT(void);

/*
 * SAVEDESEL / LOADDESEL — Idem pour le dispositif non-sélectionné
 * (adversaire en mode manuel) dans clrDESEL[].
 */
void SAVEDESEL(void);
void LOADDESEL(void);

/*
 * INITINPUT — Remet à zéro clrSEL[] et clrDESEL[].
 * Appelé depuis RESTART.
 */
void INITINPUT(void);

/*
 * DEMOKEYS — Vérifie si une touche ou un bouton interrompt la démo.
 *
 * Out: 0xFF si interruption (retour dans START), 0x00 sinon.
 * Appelé depuis MainLoop juste après KEYS.
 * Laisse passer ESC et CTRL-S sans interrompre.
 */
uint8_t DEMOKEYS(void);

/*
 * LISTTORCHES — Construit la liste des torches visibles sur VisScrn
 * dans torchx[], torchy[], torchstate[], torchclip[].
 *
 * Appelé avant la boucle musicale (songcues) pour que BURN puisse
 * animer les flammes pendant que la musique joue, sans toucher
 * au système normal de rendu.
 */
void LISTTORCHES(void);

/*
 * BURN — Anime une flamme de torche directement sur la page affichée.
 *
 * Contourne le système de listes d'images : appelle fastlay ou lay
 * directement sur la page courante pendant la musique.
 * Avance d'un index dans torchx[] à chaque appel.
 */
void BURN(void);

/*
 * GETMINLEFT — Calcule le temps restant en minutes et secondes (BCD).
 *
 * In: FrameCount (compteur de frames, 16 bits)
 * Out: MinLeft = minutes restantes (BCD $00-$99)
 *      SecLeft = secondes restantes (pendant la dernière minute)
 *
 * Utilise MIN_FRAMES_SK = 725 frames par "minute" (~11fps réel).
 * Le temps limite est TIME_LIMIT_SK = 60 "minutes" (≈ 65 minutes réelles).
 */
void GETMINLEFT(void);

/*
 * KEEPTIME — Incrémente FrameCount et déclenche les messages de temps.
 *
 * Appelé chaque frame depuis NextFrame (TOPCTRL) quand le timer tourne.
 * Compare FrameCount avec timetable[] pour savoir si c'est le moment
 * d'afficher un message ("X minutes left").
 * Ne fait rien si level==0 (démo) ou si le joueur est mort.
 */
void KEEPTIME(void);

/*
 * SHORTENTIME — Raccourcit le temps restant à 15 "minutes".
 *
 * Appelé quand le joueur triche (skip level). Modifie FrameCount
 * et NextTimeMsg pour simuler qu'il ne reste que 15 minutes.
 * N'a pas d'effet si le temps est déjà inférieur à 15 minutes.
 */
void SHORTENTIME(void);

/*
 * CUESONG — Programme la musique à jouer.
 *
 * In: A = numéro de chanson (voir soundnames.h)
 *     X = nombre de cycles dans lesquels la chanson doit être jouée
 *
 * Stocke dans SongCue et SongCount. La routine songcues() (TOPCTRL)
 * vérifie ces valeurs chaque frame et lance la musique quand les
 * conditions sont remplies (personnages statiques, pas de MOBs...).
 */
void CUESONG(uint8_t song_num, uint8_t song_count);

/*
 * STROBE / DLOOP — Lit le clavier et le joystick.
 *
 * STROBE appelle keys() puis controller().
 * DLOOP est un alias de STROBE.
 * Appelé depuis MainLoop à chaque frame.
 */
void STROBE(void);
void DLOOP(void);


/* -----------------------------------------------------------------------
 * ROUTINES INTERNES (déclarées pour la traduction)
 * ----------------------------------------------------------------------- */

/*
 * KREAD — Lit les touches de contrôle du joueur (j/k/l/u/i/o et
 * leurs équivalents majuscules) et met à jour kbdX, kbdY.
 */
void KREAD(void);

/* Touches légitimes (accessibles en version finale) */
void LegitKeys(void);

/* Touches de développement (cheat codes, debug) */
void DevelKeys(void);
void TempDevel(void);

/*
 * addkey — Ajoute une touche au buffer circulaire keybuf[].
 * Utilisé par checkcode() pour détecter les séquences secrètes.
 */
void addkey(uint8_t key);

/*
 * checkcode — Compare les dernières touches du buffer avec une séquence.
 * In: adresse de la séquence (chaîne ASCII terminée par 0, en reverse)
 * Out: 0 si correspondance, 0xFF sinon
 */
uint8_t checkcode(const char *sequence);

/*
 * Table des messages de temps (timetable[]) :
 * Valeurs de FrameCount auxquelles afficher les messages.
 * 17 entrées + sentinelle 65535.
 */
extern const uint16_t timetable[];

#endif /* SPECIALK_H */
