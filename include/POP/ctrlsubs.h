#ifndef CTRLSUBS_H
#define CTRLSUBS_H

/*
 * ctrlsubs.h
 * Traduction de CTRLSUBS.S — Sous-routines de contrôle et de mouvement
 * Source originale : Jordan Mechner, 1989
 * Module chargé à $d000 (main LC) sur Apple II.
 *
 * Ce fichier est la bibliothèque d'utilitaires la plus utilisée du projet.
 * Presque tous les autres modules l'includent via la jump table.
 */

#include <stdint.h>
#include "eq.h"
#include "gameeq.h"
#include "movedata.h"
#include "seqdata.h"

/* -----------------------------------------------------------------------
 * CONSTANTES INTERNES
 * ----------------------------------------------------------------------- */
#define MAXMAXSTR  10   /* valeur maximale absolue de la barre de force */
#define THINNER    3    /* épaisseur retirée de chaque côté pour Fthinmark */
#define ESTWIDTH   13   /* estimation de la largeur d'un personnage (pixels) */


/* -----------------------------------------------------------------------
 * LECTURE DU BLUEPRINT — RDBLOCK
 *
 * Routine centrale : lit le type d'un bloc à partir de coordonnées
 * (screen, blockx, blocky) en gérant récursivement les références
 * hors-écran via la MAP.
 *
 * In:  scrn = numéro d'écran, bx = bloc X (0-9), by = bloc Y (0-2)
 * Out: retourne l'objid du bloc (après masquage idmask)
 *      Met à jour : BlueType, BlueSpec, tempscrn, tempblockx, tempblocky
 *
 * Note C : en assembleur, A=scrn, X=bx, Y=by. En C on passe par des
 * paramètres explicites. Les variables globales tempscrn/tempblockx/
 * tempblocky restent utilisées en interne par les routines dérivées.
 * ----------------------------------------------------------------------- */
uint8_t RDBLOCK(uint8_t scrn, uint8_t bx, uint8_t by);
uint8_t RDBLOCK1(void);  /* version sans initialisation (utilise temp*) */


/* -----------------------------------------------------------------------
 * NAVIGATION DANS LA MAP — Écrans adjacents
 *
 * In: A = numéro d'écran courant
 * Out: A = numéro de l'écran adjacent (0 si inexistant)
 * ----------------------------------------------------------------------- */
uint8_t GETLEFT(uint8_t scrn);
uint8_t GETRIGHT(uint8_t scrn);
uint8_t GETUP(uint8_t scrn);
uint8_t GETDOWN(uint8_t scrn);

/* Calcule les 8 écrans voisins de VisScrn et les stocke dans
 * scrnLeft, scrnRight, scrnAbove, scrnBelow, et les 4 diagonales */
void GETSCRNS(void);


/* -----------------------------------------------------------------------
 * NAVIGATION DANS LA SEQTABLE
 * ----------------------------------------------------------------------- */

/*
 * JUMPSEQ — Repositionne CharSeq sur un point d'entrée de la seqtable.
 *
 * In: A = numéro de séquence (1-127), correspondant aux constantes de seqdata.h
 *
 * Implémentation C :
 *   CharSeq = seqtab[(seq-1)*2]      (octet bas)
 *   CharSeq+1 = seqtab[(seq-1)*2+1]  (octet haut)
 *
 * seqtab est la table d'index chargée à $3000 (seqtable.s).
 * En C : extern uint16_t seqtab[115]  (offsets dans Fdef[])
 */
void JUMPSEQ(uint8_t seq);

/* Version pour l'adversaire (met à jour OpSeq au lieu de CharSeq) */
void OPJUMPSEQ(uint8_t seq);

/*
 * GETSEQ — Lit le prochain octet de la séquence courante et avance CharSeq.
 *
 * In:  CharSeq (pointeur courant dans Fdef[])
 * Out: retourne l'octet lu ; CharSeq est incrémenté
 *
 * Note : en assembleur ce retour est dans A (registre accumulateur).
 * En C : uint8_t GETSEQ(void) — mais attention, les valeurs négatives
 * (instructions SEQ_GOTO, SEQ_ACT etc.) sont des int8_t signés.
 * Utiliser (int8_t)GETSEQ() pour les tester comme valeurs négatives.
 */
uint8_t GETSEQ(void);

/*
 * GETFRAME — Calcule l'adresse du frame dans Fdef[] à partir d'un numéro.
 *
 * In: A = numéro de frame (1-192)
 * Out: framepoint = pointeur vers les 5 octets du frame dans Fdef[]
 *
 * Formule : framepoint = Fdef + (frame-1) * 5
 */
void GETFRAME(uint8_t frame_num);

/*
 * GETFRAMEINFO — Lit les 5 champs d'un frame depuis Fdef[CharPosn].
 *
 * In:  CharPosn
 * Out: Fimage, Fsword, Fdx, Fdy, Fcheck
 *      Gère aussi la sélection du bon jeu d'animation (altset1/2)
 *      selon CharID.
 */
void GETFRAMEINFO(void);


/* -----------------------------------------------------------------------
 * CALCUL DE POSITION
 * ----------------------------------------------------------------------- */

/*
 * GETBASEX — Calcule la coordonnée X de base du personnage.
 *
 * "Base X" = position du pied avant du personnage dans le système 140px.
 * Tient compte de Fdx (déplacement du frame) et Fcheck (offset de pied).
 *
 * In:  Char data, Fcheck, Fdx
 * Out: retourne la coordonnée X de base
 */
uint8_t GETBASEX(void);

/*
 * ADDCHARX — Ajoute un déplacement à CharX en tenant compte de CharFace.
 *
 * In: A = déplacement (+ = vers l'avant, - = vers l'arrière)
 *     CharX, CharFace
 * Out: retourne la nouvelle coordonnée X
 *
 * Si CharFace = gauche (0xFF), le signe du déplacement est inversé.
 */
uint8_t ADDCHARX(int8_t dx);

/* Version pour FCharX (coordonnée 2x) */
void ADDFCHARX(int8_t dx);

/*
 * GETBLOCKX — Convertit une coordonnée X en numéro de bloc (0-9).
 *
 * In: A = coordonnée X (système 140px)
 * Out: A = numéro de bloc (0-9)
 *      OFFSET = pixel dans ce bloc (0-13)
 */
uint8_t GETBLOCKX(uint8_t x);

/*
 * GETBLOCKXP — Comme GETBLOCKX mais pour le plan central (soustrait "angle=7").
 * Utiliser pour les personnages (plan milieu), GETBLOCKX pour le foreground.
 */
uint8_t GETBLOCKXP(uint8_t x);

/*
 * GETBLOCKY — Convertit une coordonnée Y en ligne de blocs (0-2, 3=hors-écran).
 * GETBLOCKYP — version pour plan central (utilise FloorY au lieu de BlockTop).
 */
uint8_t GETBLOCKY(uint8_t y);
uint8_t GETBLOCKYP(uint8_t y);

/*
 * GETBLOCKEJ — Retourne la coordonnée X du bord gauche d'un bloc.
 * In: A = numéro de bloc (-5 à 14)
 * Out: A = coordonnée X (140px)
 */
uint8_t GETBLOCKEJ(int8_t block_num);

/*
 * GETDIST — Distance en pixels entre CharX et la fin du bloc courant.
 * Out: A = pixels (0-13) à ajouter à CharX pour atteindre le bord du bloc
 */
uint8_t GETDIST(void);

/* Variante qui part d'un X déjà calculé (résultat de GETBASEX) */
uint8_t GETDIST1(uint8_t base_x);

/*
 * GETOPDIST — Distance en pixels entre Char et Op (adversaire).
 *
 * Positive = Op est devant Char (dans la direction où Char regarde).
 * Négative = Op est derrière Char.
 * Retourne ±127 si Op est sur un écran différent.
 *
 * Ajuste la distance d'environ ESTWIDTH si les personnages se font face.
 */
int8_t GETOPDIST(void);

/*
 * FACEDX — Retourne DX signé selon la direction du personnage.
 * In: A = DX (valeur absolue), CharFace
 * Out: +DX si face à droite, -DX si face à gauche
 */
int8_t FACEDX(int8_t dx);


/* -----------------------------------------------------------------------
 * LECTURE DES BLOCS VOISINS
 * Ces routines appellent RDBLOCK avec les coordonnées du bloc demandé,
 * calculées à partir de CharBlockX/Y/Scrn et CharFace.
 * ----------------------------------------------------------------------- */

uint8_t GETUNDERFT(void);    /* bloc sous les pieds */
uint8_t GETINFRONT(void);    /* bloc devant (dans la direction de CharFace) */
uint8_t GET2INFRONT(void);   /* bloc 2 cases devant */
uint8_t GETBEHIND(void);     /* bloc derrière */
uint8_t GETABOVE(void);      /* bloc au-dessus */
uint8_t GETABOVEINF(void);   /* bloc en haut et devant */
uint8_t GETABOVEBEH(void);   /* bloc en haut et derrière */


/* -----------------------------------------------------------------------
 * INDEX / CONVERSION BLOC
 * ----------------------------------------------------------------------- */

/*
 * INDEXBLOCK — Convertit (tempblockx, tempblocky) en index linéaire (0-29).
 *
 * Out: y = index du bloc (0-29), carry clear si sur cet écran
 *      y = 0-9, carry set si sur l'écran du dessus
 *      y = 30, carry set si hors écran
 *
 * Note C : on retourne l'index via la valeur de retour,
 * et une variable globale pour le carry.
 */
uint8_t INDEXBLOCK(void);

/*
 * UNINDEX — Inverse de INDEXBLOCK.
 * In: A = index (0-29)
 * Out: A = blockx (0-9), X = blocky (0-2)
 */
void UNINDEX(uint8_t index, uint8_t *bx, uint8_t *by);

/* GETBASEBLOCK — Met à jour CharBlockX depuis CharX et les données de frame */
void GETBASEBLOCK(void);

/* INDEXCHAR — Calcule FCharIndex (index du bloc du personnage) */
void INDEXCHAR(void);


/* -----------------------------------------------------------------------
 * TESTS SUR LES BLOCS
 * ----------------------------------------------------------------------- */

/*
 * CMPSPACE — Le bloc est-il traversable (pas de sol) ?
 * Note : un block solide EST considéré comme traversable (il n'a pas de sol).
 * Out: 0 = traversable, 1 = sol présent
 */
uint8_t CMPSPACE(uint8_t objid);

/*
 * CMPBARR — Le bloc est-il une barrière verticale ?
 * Out: 0 = libre, 1 = panneau/portail, 3 = miroir/trancheuse, 4 = bloc solide
 */
uint8_t CMPBARR(uint8_t objid);

/*
 * CMPWALL — Le bloc est-il un mur (bloc solide, ou panneau face à gauche) ?
 * Out: 0 = oui c'est un mur, 1 = non
 */
uint8_t CMPWALL(uint8_t objid);

/*
 * CHECKLEDGE — Peut-on s'accrocher à ce rebord ?
 *
 * In: blockid = bloc qui doit être libre (au-dessus)
 *     A = résultat RDBLOCK du bloc qui doit être un rebord
 * Out: 1 si accrochable, 0 sinon
 */
uint8_t CHECKLEDGE(uint8_t rdblock_result);

/* CHECKSPIKES — Déclenche les pics si le personnage passe dessus */
void CHECKSPIKES(void);

/* UNEVENFLOOR — Ajuste CharY si le personnage est sur une plaque vers le bas */
void UNEVENFLOOR(void);


/* -----------------------------------------------------------------------
 * CONFIGURATION DU PERSONNAGE POUR FRAMEADV
 * ----------------------------------------------------------------------- */

/*
 * SETUPCHAR — Traduit les données Char en données FChar.
 *
 * Calcule FCharX (en 280px), FCharY, FCharFace, FCharImage, FCharTable
 * à partir de CharX, CharY, CharFace, Fdx, Fdy, Fimage, Fsword.
 *
 * C'est l'interface entre le système de coordonnées 140px du jeu
 * et le système 280px de l'affichage.
 */
void SETUPCHAR(void);

/* SETUPSWORD — Si l'épée du personnage est visible, l'ajoute à la table d'objets */
void SETUPSWORD(void);

/* GETEDGES — Calcule les bords de l'image du personnage (leftej, rightej, topej...)
 * et les blocs affectés (leftblock, rightblock, topblock, bottomblock) */
void GETEDGES(void);

/* CROPCHAR — Calcule les valeurs de crop (FCharCU/CL/CR/CD) selon l'environnement */
void CROPCHAR(void);

/* QUICKFLOOR — Marque pour redraw les blocs de sol que le personnage touche */
void QUICKFLOOR(void);

/* QUICKFG — Marque pour redraw les blocs de foreground que le personnage touche */
void QUICKFG(void);


/* -----------------------------------------------------------------------
 * CHARGEMENT / SAUVEGARDE DES DONNÉES DE PERSONNAGE
 *
 * Ces 8 routines copient 16 octets entre les structures Kid, Shad, Char, Op.
 * Les variantes "wOp" (with Opponent) chargent simultanément les deux personnages :
 *   LoadKidwOp  : Kid→Char  ET  Shad→Op
 *   LoadShadwOp : Shad→Char ET  Kid→Op
 * ----------------------------------------------------------------------- */
void LOADKID(void);
void SAVEKID(void);
void LOADSHAD(void);
void SAVESHAD(void);
void LOADKIDWOP(void);
void SAVEKIDWOP(void);
void LOADSHADWOP(void);
void SAVESHADWOP(void);


/* -----------------------------------------------------------------------
 * AJOUT À LA TABLE D'OBJETS
 * Appellent toutes addcharobj() avec le bon TypeXxx.
 * ----------------------------------------------------------------------- */
void ADDKIDOBJ(void);
void ADDSHADOBJ(void);
void ADDGUARDOBJ(void);
void ADDREFLOBJ(void);
void ADDSWORDOBJ(void);


/* -----------------------------------------------------------------------
 * BUFFERS DE REDRAW — routines MARK*
 *
 * Ces routines marquent des entrées dans les buffers de redraw
 * (redbuf, fredbuf, floorbuf, wipebuf, movebuf, halfbuf, topbuf)
 * pour qu'elles soient redessinées au prochain frame rapide (DoFast).
 *
 * In: A = valeur à stocker (généralement 2)
 *     Résultats de INDEXBLOCK : Y = index du bloc, carry = état
 *
 * Si carry set et Y < 10 : bloc dans topbuf (ligne du dessus)
 * Si carry set et Y >= 10 : bloc hors écran, ignoré
 * ----------------------------------------------------------------------- */
void MARKRED(uint8_t val);    /* redbuf : redessiner le bloc complet */
void MARKFRED(uint8_t val);   /* fredbuf : redessiner le foreground */
void MARKWIPE(uint8_t val);   /* wipebuf : effacer (wipe) le bloc */
void MARKMOVE(uint8_t val);   /* movebuf : bloc en mouvement */
void MARKFLOOR(uint8_t val);  /* floorbuf : sol touché par le personnage */
void MARKHALF(uint8_t val);   /* halfbuf : moitié de bloc (grimpée) */

/* ZERORED — Remet à zéro tous les buffers de redraw */
void ZERORED(void);


/* -----------------------------------------------------------------------
 * BARRES DE FORCE
 * ----------------------------------------------------------------------- */

/* RECHARGEMETER — Recharge la barre de force du joueur au maximum */
void RECHARGEMETER(void);

/* BOOSTMETER — Augmente MaxKidStr de 1 (plafonné à MAXMAXSTR) puis recharge */
void BOOSTMETER(void);

#endif /* CTRLSUBS_H */
