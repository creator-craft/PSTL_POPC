#ifndef GAMEBG_H
#define GAMEBG_H

/*
 * gamebg.h
 * Traduction de GAMEBG.S : Rendu des personnages et des éléments d'interface
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $4c00 (aux mem) sur Apple II.
 *
 * -----------------------------------------------------------------------
 * NATURE DU FICHIER
 *
 * GAMEBG.S est le module de RENDU DES PERSONNAGES ET DE L'INTERFACE.
 * Malgré son nom ("game background"), il ne dessine pas le background :
 * c'est FRAMEADV.S qui s'en charge. GAMEBG.S est responsable de :
 *
 *   1. L'ajout des personnages à la table d'objets (ADDCHAROBJ)
 *      et leur dessin effectif (DrawKid, DrawShad, DrawGuard)
 *
 *   2. Le rendu des barres de force (DRAWKIDMETER, DRAWOPPMETER)
 *
 *   3. Les messages superposés (PRINTLEVEL, TIMELEFTMSG, CONTINUEMSG,
 *      FLIPDISKMSG) : affichés par-dessus le décor via addmsg
 *
 *   4. Les effets visuels spéciaux de la salle de la princesse :
 *      flammes (SETUPFLAME, PSETUPFLAME), sablier (DRAWGLASS),
 *      sable qui coule (FLOW), étoiles (TWINKLE), potion (SETUPFLASK),
 *      étoile d'impact de combat (SETUPCOMIX)
 *
 *   5. Utilitaires de rendu : INITLAY (initialise les paramètres de
 *      clipping), CALCBLUE (calcule les pointeurs blueprint : définie
 *      ici bien que logiquement appartenant à FRAMEADV.S, voir GRAFIX.S)
 *
 *   6. La protection contre la copie visuelle (YELLOW, SETRECHECK0,
 *      RECHECKYEL)
 *
 * -----------------------------------------------------------------------
 * MODES DE DESSIN DES PERSONNAGES
 *
 * Les personnages peuvent être dessinés de trois façons selon le contexte :
 *
 *   DrawNormal  : mode mask+OR avec layrsave (rendu standard)
 *   DrawEored   : mode XOR (effet ombre pour le shadowman, et pour
 *                 le joueur pendant la phase de "merge" niveau 12)
 *   DrawShifted : décalé d'1 bit (effet coloré pour certains gardes,
 *                 GuardColor != 0)
 *
 * DRAWKID choisit entre Normal et Eored selon backtolife et mergetimer.
 * DRAWSHAD utilise toujours Eored.
 * DRAWGUARD choisit selon GuardColor.
 *
 * -----------------------------------------------------------------------
 * BARRES DE FORCE
 *
 * Chaque barre est composée de "bullets" dessinés dans la liste msg.
 * La barre du joueur va de gauche à droite (KidStrX/KidStrOFF).
 * La barre de l'adversaire va de droite à gauche, miroir (OppStrX/OppStrOFF,
 * OPACITY bit 7 = miroir).
 * Une balle clignote quand la force est à 1 (utilise PAGE pour le flash).
 * ----------------------------------------------------------------------- */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"
#include "movedata.h"

/* -----------------------------------------------------------------------
 * CONSTANTES
 * ----------------------------------------------------------------------- */

/* Barres de force */
#define BULLET_IMG    0x88   /* image d'une "balle" de force (bgtable2) */
#define BLANK_IMG     0x8c   /* image d'un emplacement vide */

/* Post de la salle de la princesse */
#define POST_X        31
#define POST_Y        152
#define POST_IMG      0x0c   /* image dans chtable6 */

/* Étoiles hors de la fenêtre (salle de la princesse) */
#define STAR_X        2
#define STAR_IMAGE    0x41   /* image comix */
#define STAR_TABLE    0      /* chtable1 */

/* Sablier */
#define GLASS_X       19
#define GLASS_Y       151

/* Positions Y des boîtes de message */
#define MSG_Y         90
#define LOW_MSG_Y     153
#define HI_CONT_Y     73
#define LOW_CONT_Y    168

/* Types de potions (pour SETUPFLASK) */
#define EMPTY_POT     0x00
#define REFRESH_POT   0x20   /* recharge force */
#define BOOST_POT     0x40   /* augmente force max */
#define MYST_POT      0x60   /* mystérieuse (bleue) */

/* Nombre de frames torch */
#define TORCH_LAST    17   /* dernier frame de flamme valide */
#define BUBB_LAST      8   /* dernier frame de bulle de potion */


/* -----------------------------------------------------------------------
 * ROUTINES PUBLIQUES : Jump table à $4c00
 * ----------------------------------------------------------------------- */

/*
 * UPDATEMETERS : Redessine les barres de force si redkidmeter/redoppmeter
 * sont non nuls. Appelé depuis FAST (FRAMEADV.S) en fin de frame rapide.
 */
void UPDATEMETERS(void);

/*
 * DRAWKIDMETER : Dessine la barre de force du joueur (bas-gauche).
 * Utilise KidStrength, MaxKidStr, PAGE (pour le flash à 1).
 * Ajoute les images à la liste msg via addmsg.
 */
void DRAWKIDMETER(void);

/*
 * DRAWOPPMETER : Dessine la barre de force de l'adversaire (bas-droite).
 * N'affiche pas la barre pour la souris (CharID=24), le squelette (4),
 * ni le shadowman sauf sur level 12.
 */
void DRAWOPPMETER(void);

/*
 * DRAWSWORD : Ajoute l'épée à la table d'objets (mode DrawNormal).
 * Appelé depuis addchars (TOPCTRL.S) après setupkid/setupshad.
 */
void DRAWSWORD(void);

/*
 * DRAWKID : Ajoute le joueur à la table d'objets.
 * Choisit DrawNormal, DrawEored (merge/backtolife) selon état.
 */
void DRAWKID(void);

/*
 * DRAWSHAD : Ajoute le shadowman à la table d'objets (toujours DrawEored).
 */
void DRAWSHAD(void);

/*
 * DRAWGUARD : Ajoute un garde à la table d'objets.
 * DrawNormal si GuardColor==0, DrawShifted sinon.
 */
void DRAWGUARD(void);

/*
 * SETUPFLAME : Prépare les paramètres pour dessiner une flamme de torche
 * dans le décor normal (section B d'une torche).
 *
 * In: XCO = blockxco, YCO = Ay, X = état de la torche (frame index)
 * Out: IMAGE, XCO, YCO, OPACITY, TABLE prêts pour addback ou fastlay
 * Ne fait rien si X >= torchLast+1.
 */
void SETUPFLAME(uint8_t torch_state);

/*
 * PSETUPFLAME : Version pour la salle de la princesse.
 * Utilise ptorchflame[] et chtable6. Appelle INITLAY.
 * In: XCO, YCO, X = frame #
 */
void PSETUPFLAME(uint8_t frame);

/*
 * CONTINUEMSG : Affiche "Press button to continue" via addmsg+layrsave.
 * Positionne le message en bas si KidBlockX est pair.
 */
void CONTINUEMSG(void);

/*
 * ADDCHAROBJ : Ajoute un personnage à la table d'objets.
 *
 * Convertit FCharX (280-res) en XCO/OFFSET (byte/offset) via CVTX,
 * remplit les champs objX/Y/IMG/TAB/FACE/CU/CL/CR/CD/TYP/INDX,
 * appelle SETOBJINDX pour marquer objbuf.
 *
 * In: FCharVars (X, Y, Image, Table, Face, CU/CL/CR/CD, Index)
 *     A = TypeXxx (TypeKid, TypeShad, TypeGd, TypeSword, TypeComix...)
 */
void ADDCHAROBJ(uint8_t obj_type);

/*
 * SETOBJINDX : Enregistre l'index de bloc du personnage dans objbuf.
 * In: X = index dans objX[] (déjà incrémenté), FCharIndex
 * Marque objbuf[FCharIndex] = 1 si le bloc est onscreen.
 */
void SETOBJINDX(void);

/*
 * PRINTLEVEL : Affiche le message "Level XX" en superposition.
 * Utilise msgbox + levelmsg + digit1/digit2.
 */
void PRINTLEVEL(void);

/*
 * DRAWOPPMETER (déjà déclarée ci-dessus) */

/*
 * FLIPDISKMSG : Affiche "Turn disk over" via addmsg+layrsave.
 */
void FLIPDISKMSG(void);

/*
 * TIMELEFTMSG : Affiche "XX Minutes Left" (ou "XX Seconds").
 * Bascule sur "Seconds" pendant la dernière minute.
 * Repositionne le message en bas si le joueur est en train de tomber
 * sur la rangée du milieu.
 */
void TIMELEFTMSG(void);

/*
 * SETUPFLASK : Prépare les paramètres pour dessiner un flacon animé.
 *
 * In: XCO, YCO (position du bloc), X = état du flacon
 *     (bits 7-5 = type de potion, bits 4-0 = frame courant)
 * Out: IMAGE, XCO, YCO, OFFSET, OPACITY, TABLE prêts pour addmid/lay
 *
 * Les potions boost (0x40) et mystérieuses (0x60) ont un flacon plus haut.
 */
void SETUPFLASK(uint8_t flask_state);

/*
 * SETUPCOMIX : Ajoute une étoile d'impact de combat à la table d'objets.
 *
 * Positionne l'étoile au-dessus de CharY (ou en bas si mort/accroupi).
 * Choisit la couleur (rouge pour le joueur, bleu pour les adversaires)
 * selon CharID et la parité de FCharX.
 * Sauvegarde et restaure FCharVars autour du dessin.
 */
void SETUPCOMIX(void);

/*
 * DRAWPOST : Dessine le grand pilier blanc de la salle de la princesse.
 * Ajoute directement à la liste fg (addfore).
 */
void DRAWPOST(void);

/*
 * DRAWGLASS : Dessine le sablier dans la salle de la princesse.
 * In: X = état du sablier (0-8, 0 = plein)
 * Ajoute à la liste bg (addback) via chtable6.
 */
void DRAWGLASS(uint8_t glass_state);

/*
 * INITLAY : Initialise les paramètres de clipping pour un appel à LAY.
 * Met BANK=3 (auxmem), LEFTCUT=0, RIGHTCUT=40, TOPCUT=0, BOTCUT=192.
 * Appelé avant tout appel direct à lay() ou fastlay() dans ce module.
 */
void INITLAY(void);

/*
 * TWINKLE : Fait scintiller une étoile hors de la fenêtre (mode EOR).
 * Met à jour les DEUX pages hi-res directement (bypass du système normal).
 * In: X = numéro d'étoile (0-3)
 */
void TWINKLE(uint8_t star_num);

/*
 * FLOW : Dessine le sable qui coule dans le sablier.
 * In: X = frame (0-3), Y = état du sablier (0-7, 8 = vide)
 * Appelle lay() directement avec BOTCUT ajusté selon la hauteur du sable.
 */
void FLOW(uint8_t frame, uint8_t glass_state);

/*
 * PMASK : Masque le visage et les cheveux de la princesse pour certains
 * frames (plie, pslump). Appelé après ADDCHAROBJ dans les scènes.
 */
void PMASK(void);

/*
 * YELLOW : Protection contre la copie (2ème niveau).
 * Sur la version 3.5", met simplement yellowflag=$80.
 * Sur la version 5.25", effectue une vérification de signature disque.
 * In: X = paramètre de vérification
 */
void YELLOW(uint8_t param);

/*
 * SETRECHECK0 : Initialise le pointeur locals pour RECHECKYEL.
 * Fait pointer locals vers recheck0.
 * Out: A = 0 (tombe dans INITLAY)
 */
void SETRECHECK0(void);

/*
 * RECHECKYEL : Revérifie la protection si l'échec était dû à un disque
 * absent. Appelé après cutprincess dans la boucle des cinématiques.
 * Out: A = 0xFF si vérification réussie, 0 sinon
 */
uint8_t RECHECKYEL(void);


/* -----------------------------------------------------------------------
 * DONNÉES STATIQUES
 * ----------------------------------------------------------------------- */

/* Coordonnées X et offsets pour les bullets de la barre du joueur */
extern const uint8_t KidStrX[12];
extern const uint8_t KidStrOFF[12];

/* Coordonnées X et offsets pour les bullets de la barre de l'adversaire */
extern const uint8_t OppStrX[12];
extern const uint8_t OppStrOFF[12];

/* Frames d'animation des flammes de torche (18 frames, bgtable1) */
extern const uint8_t torchflame[18];

/* Frames d'animation des flammes de la princesse (18 frames, chtable6) */
extern const uint8_t ptorchflame[18];

/* Frames d'animation des bulles de potion (9 frames) */
extern const uint8_t bubble[9];

/* Images du sablier selon état (0-8, chtable6) */
extern const uint8_t glassimg[9];

/* Images du sable qui coule (3 frames) */
extern const uint8_t flowimg[3];

/* Hauteurs de sable selon l'état du sablier (pour BOTCUT) */
extern const uint8_t sandht[8];

/* Tables de chiffres pour l'affichage des niveaux et du temps */
extern const uint8_t digit1[13];   /* dizaines (0 si < 10) */
extern const uint8_t digit2[13];   /* unités */

#endif /* GAMEBG_H */
