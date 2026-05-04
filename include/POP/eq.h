#ifndef EQ_H
#define EQ_H

/*
 * eq.h
 * Traduction de EQ.S : Equates globaux du jeu Prince of Persia (Apple II)
 * Source originale : Jordan Mechner, 1989
 *
 * Ce fichier regroupe :
 *   - Les adresses mémoire Apple II (simulées ou remplacées par des buffers C)
 *   - Les tables de saut (remplacées par des pointeurs de fonctions)
 *   - Les listes d'images (image lists)
 *   - Les variables zero page (PAGE, XCO, YCO, etc.)
 *   - Les variables globales du jeu
 *   - Les constantes misc (dimensions écran, opcodes de dessin...)
 */

#include <stdint.h>

/* -----------------------------------------------------------------------
 * ADRESSES MÉMOIRE APPLE II
 * Sur Apple II, ces zones correspondent à des emplacements mémoire fixes.
 * En C, elles sont remplacées par des buffers alloués statiquement.
 * ----------------------------------------------------------------------- */

/* Main LC (Language Card) */
/* rw18      = $d000  → peelbuf1 */
/* peelbuf1  = $d000 */
/* peelbuf2  = $d800 */
/* hrtables  = $e000  → YLO, YHI lookup tables */
/* unpack    = $ea00  → routines de décompression */
/* hires     = $ee00  → jump table routines graphiques bas niveau */
/* master    = $f880  → jump table principale */

/* Aux mem */
/* grafix    = $0400  → jump table routines graphiques haut niveau */
/* tables    = $0e00  → ByteTable, OffsetTable, BlockTable... */
/* frameadv  = $1290  → jump table FRAMEADV (sure, fast, getinitobj) */
/* redbufs   = $5e00  → redraw buffers */
/* imlists   = $ac00  → image lists (bgX, bgY, bgIMG...) */
/* blueprnt  = $b700  → level blueprint (BLUETYPE, BLUESPEC, MAP, INFO...) */
/* bluecopy  = $d000  → copie du blueprint (bank 1 aux LC) */


/* -----------------------------------------------------------------------
 * REDRAW BUFFERS
 * En assembleur, ces buffers sont à l'adresse $5e00 (aux mem).
 * Chaque buffer fait 30 octets (un compteur par bloc d'écran, 10x3 blocs).
 * TOPBUF est exceptionnel : seulement 10 octets.
 * ----------------------------------------------------------------------- */
extern uint8_t halfbuf[30];
extern uint8_t redbuf[30];
extern uint8_t fredbuf[30];
extern uint8_t floorbuf[30];
extern uint8_t wipebuf[30];
extern uint8_t movebuf[30];
extern uint8_t objbuf[30];
extern uint8_t whitebuf[30];
extern uint8_t topbuf[10];  /* seulement 10 octets ! */


/* -----------------------------------------------------------------------
 * LOOKUP TABLES (hrtables = $e000)
 * YLO[y] et YHI[y] donnent l'adresse mémoire vidéo de la ligne y.
 * Utilisées par fastlay pour calculer la position d'un pixel à l'écran.
 * 192 entrées (0xc0) correspondant aux 192 lignes de l'écran Apple II.
 * ----------------------------------------------------------------------- */
#define HRTABLES_SIZE 0xc0  /* 192 */
extern uint8_t YLO[HRTABLES_SIZE];
extern uint8_t YHI[HRTABLES_SIZE];


/* -----------------------------------------------------------------------
 * JUMP TABLES → POINTEURS DE FONCTIONS
 *
 * Sur Apple II, les jump tables sont des blocs de 3 octets (JMP addr)
 * permettant d'appeler des routines par adresse fixe.
 * En C, on les remplace par des pointeurs de fonctions déclarés extern.
 * Les implémentations réelles seront dans les fichiers .c correspondants.
 * ----------------------------------------------------------------------- */

/* -- master ($f880) : fonctions système principales -- */
extern void firstboot(void);
extern void loadlevel(void);
extern void reload(void);
extern void loadstage2(void);
extern void attractmode(void);
extern void cutprincess(void);
extern void savegame(void);
extern void loadgame(void);
extern void dostartgame(void);
extern void epilog(void);
extern void loadaltset(void);

/* -- hires ($ee00) : routines graphiques bas niveau (HIRES.S) -- */
extern void boot3(void);
extern void cls(void);
extern void lay(void);
extern void fastlay(void);
extern void layrsave(void);
extern void lrcls(void);
extern void fastmask(void);
extern void fastblack(void);
extern void peel(void);
extern void getwidth(void);
extern void copy2000(void);
extern void copy2000aux(void);
extern void setfastaux(void);
extern void setfastmain(void);
extern void copy2000ma(void);
extern void copy2000am(void);

/* -- unpack ($ea00) : routines de décompression -- */
extern void SngExpand(void);
extern void DblExpand(void);
extern void DeltaExpPop(void);
extern void inverty(void);
extern void DeltaExpWipe(void);
extern void purple(void);
extern void prompt(void);
extern void blackout(void);
extern void clr(void);
extern void text(void);
extern void setdhires(void);
extern void fadein(void);
extern void loadsuper(void);
extern void fadeout(void);

/* -- grafix ($0400) : routines graphiques haut niveau (GRAFIX.S) -- */
extern void gr(void);
extern void drawall(void);
extern void controller(void);
extern void saveblue(void);
extern void reloadblue(void);
extern void movemem(void);
extern void buttons(void);
extern void gtone(void);
extern void setcenter(void);
extern void dimchar(void);
extern void cvtx(void);
extern void zeropeel(void);
extern void zeropeels(void);
extern void pread(void);
extern void addpeel(void);
extern void copyscrn(void);
extern void sngpeel(void);
extern void rnd(void);
/* cls déjà déclarée */
/* lay, fastlay, layrsave, lrcls, fastmask, fastblack, peel, getwidth déjà déclarés */
/* copy2000, copy2000ma, setfastaux, setfastmain, copy2000am déjà déclarés */
extern void loadlevel_g(void);    /* loadlevel dans grafix */
extern void attractmode_g(void);  /* attractmode dans grafix */
extern void xminit(void);
extern void xmplay(void);
extern void cutprincess_g(void);
extern void xtitle(void);
extern void reload_g(void);
extern void loadstage2_g(void);
extern void getselect(void);
extern void getdesel(void);

/* Fonctions d'ajout aux listes d'images (GRAFIX.S) */
extern void addback(void);   /* ajouter une image au background */
extern void addfore(void);   /* ajouter une image au foreground */
extern void addmid(void);    /* ajouter une image au midground */
extern void addmidez(void);  /* version simplifiée addmid */
extern void addwipe(void);   /* ajouter un wipe */
extern void addmsg(void);    /* ajouter un message */
extern void addmidezo(void); /* autre variante addmid */

/* Autres fonctions grafix */
extern void savegame_g(void);
extern void loadgame_g(void);
extern void zerolsts(void);   /* zeroing des listes d'images */
extern void screendump(void);
extern void minit(void);
extern void mplay(void);
extern void savebinfo(void);
extern void reloadbinfo(void);
extern void inverty_g(void);
extern void normspeed(void);
extern void calcblue(void);   /* calcul adresse blueprint */
extern void zerored(void);    /* zeroing des redraw buffers */
extern void xplaycut(void);
extern void checkIIGS(void);
extern void fastspeed(void);
extern void musickeys(void);
extern void dostartgame_g(void);
extern void epilog_g(void);
extern void loadaltset_g(void);
extern void xmovemusic(void);
extern void whoop(void);
extern void vblank(void);
extern void vbli(void);

/* -- frameadv ($1290) : fonctions de FRAMEADV.S -- */
extern void sure(void);       /* assemble les listes d'images pour l'écran courant */
extern void fast(void);       /* version rapide de sure */
extern void getinitobj(void); /* récupère l'objet initial */

/* Pointeur de fonction générique utilisé comme "add" (setback/setfore/setmid) */
extern void (*add)(void);


/* -----------------------------------------------------------------------
 * TABLES UTILITAIRES (tables = $0e00)
 * ----------------------------------------------------------------------- */
#define BYTETABLE_SIZE   0x100
#define OFFSETTABLE_SIZE 0x100
#define BLOCKTABLE_SIZE  0x100
#define PIXELTABLE_SIZE  0x100

extern uint8_t ByteTable[BYTETABLE_SIZE];
extern uint8_t OffsetTable[OFFSETTABLE_SIZE];
extern uint8_t BlockTable[BLOCKTABLE_SIZE];
extern uint8_t PixelTable[PIXELTABLE_SIZE];
extern uint8_t Mult10[0x10];
extern uint8_t Mult7[0x10];
extern uint8_t Mult30[0x40];
extern uint8_t BlockEdge[20];
extern uint8_t BlockTop[5];
extern uint8_t BlockBot[5];
extern uint8_t FloorY[5];
extern uint8_t BlockAy[5];


/* -----------------------------------------------------------------------
 * LEVEL BLUEPRINT (blueprnt = $b700)
 * Structure du blueprint d'un niveau, 2304 octets au total.
 *   BLUETYPE  [24*30] : type de chaque bloc (objid)
 *   BLUESPEC  [24*30] : spec de chaque bloc (état, pattern, lien pp...)
 *   LINKLOC   [256]   : positions des portails liés à chaque plaque pression
 *   LINKMAP   [256]   : timer + bits hauts du screen# du portail
 *   MAP       [24*4]  : connexions entre screens (gauche/droite/haut/bas)
 *   INFO      [256]   : positions de départ joueur + gardes
 * ----------------------------------------------------------------------- */
#define NUM_SCREENS  24
#define BLOCKS_PER_SCREEN 30

extern uint8_t BLUETYPE[NUM_SCREENS * BLOCKS_PER_SCREEN];
extern uint8_t BLUESPEC[NUM_SCREENS * BLOCKS_PER_SCREEN];
extern uint8_t LINKLOC[256];
extern uint8_t LINKMAP[256];
extern uint8_t MAP[NUM_SCREENS * 4];
extern uint8_t INFO[256];


/* -----------------------------------------------------------------------
 * INFO BLOCK : positions de départ (sous-structure de INFO)
 * En assembleur, défini avec "dum INFO" puis des offsets.
 * ----------------------------------------------------------------------- */
/* Offsets dans INFO[] */
#define INFO_OFFSET_KID      64   /* les 64 premiers octets réservés à l'éditeur */

/* Pour y accéder directement : */
#define KidStartScrn   INFO[64]
#define KidStartBlock  INFO[65]
#define KidStartFace   INFO[66]
/* INFO[67] = réservé */
#define SwStartScrn    INFO[68]
#define SwStartBlock   INFO[69]
/* INFO[70] = réservé */
/* GdStartBlock[24] commence à INFO[71] */
extern uint8_t *GdStartBlock;  /* pointe sur INFO[71] */
extern uint8_t *GdStartFace;   /* pointe sur INFO[71+24] */
extern uint8_t *GdStartX;      /* pointe sur INFO[71+48] */
extern uint8_t *GdStartSeqL;   /* pointe sur INFO[71+72] */
extern uint8_t *GdStartProg;   /* pointe sur INFO[71+96] */
extern uint8_t *GdStartSeqH;   /* pointe sur INFO[71+120] */


/* -----------------------------------------------------------------------
 * IMAGE LISTS (imlists = $ac00)
 * Tableaux remplis par les routines addback/addfore/addmid/addwipe/addmsg
 * puis lus par DRAWBACK/DRAWFORE/DRAWMID/DRAWWIPE/DRAWMSG dans GRAFIX.S.
 *
 * Convention : le premier octet de chaque liste (ex: bgX[0]) contient
 * le NOMBRE d'éléments à afficher. Les données commencent à l'indice 1.
 * ----------------------------------------------------------------------- */
#define MAX_BACK  200
#define MAX_FORE  100
#define MAX_WIPE   20
#define MAX_PEEL   46
#define MAX_MID    46
#define MAX_OBJ    20
#define MAX_MSG    32

/* Flag d'effacement de l'écran (genCLS != 0 → effacer avant dessin) */
extern uint8_t genCLS;

/* Background plane */
extern uint8_t bgX[MAX_BACK];    /* coordonnée X (en octets, 0-39) */
extern uint8_t bgY[MAX_BACK];    /* coordonnée Y (ligne, 0-191) */
extern uint8_t bgIMG[MAX_BACK];  /* numéro d'image encodé (table+image#) */
extern uint8_t bgOP[MAX_BACK];   /* opacité / opération de dessin */

/* Foreground plane */
extern uint8_t fgX[MAX_FORE];
extern uint8_t fgY[MAX_FORE];
extern uint8_t fgIMG[MAX_FORE];
extern uint8_t fgOP[MAX_FORE];

/* Wipe effects */
extern uint8_t wipeX[MAX_WIPE];
extern uint8_t wipeY[MAX_WIPE];
extern uint8_t wipeH[MAX_WIPE];
extern uint8_t wipeW[MAX_WIPE];
extern uint8_t wipeCOL[MAX_WIPE];

/* Peel list (double-buffered : x2) */
extern uint8_t peelX[MAX_PEEL * 2];
extern uint8_t peelY[MAX_PEEL * 2];
extern uint8_t peelIMGL[MAX_PEEL * 2];
extern uint8_t peelIMGH[MAX_PEEL * 2];

/* Mid plane (le plus riche : 11 champs) */
extern uint8_t midX[MAX_MID];
extern uint8_t midOFF[MAX_MID];   /* offset X sub-octet (0-6) */
extern uint8_t midY[MAX_MID];
extern uint8_t midIMG[MAX_MID];
extern uint8_t midOP[MAX_MID];
extern uint8_t midTYP[MAX_MID];   /* type d'image (pour mid seulement) */
extern uint8_t midCU[MAX_MID];    /* top cutoff */
extern uint8_t midCD[MAX_MID];    /* bottom cutoff */
extern uint8_t midCL[MAX_MID];    /* left cutoff */
extern uint8_t midCR[MAX_MID];    /* right cutoff */
extern uint8_t midTAB[MAX_MID];   /* table source */

/* Object list (12 champs, similaire au mid) */
extern uint8_t objINDX[MAX_OBJ];
extern uint8_t objX[MAX_OBJ];
extern uint8_t objOFF[MAX_OBJ];
extern uint8_t objY[MAX_OBJ];
extern uint8_t objIMG[MAX_OBJ];
extern uint8_t objFACE[MAX_OBJ];  /* direction du personnage */
extern uint8_t objTYP[MAX_OBJ];
extern uint8_t objCU[MAX_OBJ];
extern uint8_t objCD[MAX_OBJ];
extern uint8_t objCL[MAX_OBJ];
extern uint8_t objCR[MAX_OBJ];
extern uint8_t objTAB[MAX_OBJ];

/* Message plane */
extern uint8_t msgX[MAX_MSG];
extern uint8_t msgOFF[MAX_MSG];
extern uint8_t msgY[MAX_MSG];
extern uint8_t msgIMG[MAX_MSG];
extern uint8_t msgOP[MAX_MSG];


/* -----------------------------------------------------------------------
 * ZERO PAGE : Paramètres graphiques ($00-$17)
 * Sur Apple II, ces variables occupent la page zéro (accès rapide).
 * En C, on les déclare comme variables globales ordinaires.
 * ----------------------------------------------------------------------- */
extern uint8_t  PAGE;      /* $00 : page graphique active (0x20 ou 0x40) */
extern uint8_t  XCO;       /* $01 : coordonnée X courante (en octets) */
extern uint8_t  YCO;       /* $02 : coordonnée Y courante (ligne) */
extern uint8_t  OFFSET;    /* $03 : offset sub-octet (0-6) */
extern uint8_t  IMAGE[2];  /* $04-$05 : adresse de l'image courante (lo/hi) */
extern uint8_t  OPACITY;   /* $06 : type d'opération de dessin */
extern uint8_t  TABLE[2];  /* $07-$08 : adresse de la table d'images */
extern uint8_t  PEELBUF[2];
extern uint8_t  PEELIMG[2];
extern uint8_t  PEELXCO;
extern uint8_t  PEELXCO_val; /* PEELXCO dans le source */
extern uint8_t  PEELXYCO;
extern uint8_t  TOPCUT;
extern uint8_t  LEFTCUT;
extern uint8_t  RIGHTCUT;
extern uint8_t  BANK;
extern uint8_t  BOTCUT;

/* Alias pratiques pour les dimensions de l'image courante */
#define height IMAGE[0]
#define width  IMAGE[1]


/* -----------------------------------------------------------------------
 * ZERO PAGE : Variables globales ($18-$3F)
 * ----------------------------------------------------------------------- */
extern uint8_t JSTKX;       /* joystick X brut */
extern uint8_t JSTKY;       /* joystick Y brut */
extern uint8_t BTN0;
extern uint8_t BTN1;
extern uint8_t BUTT0;
extern uint8_t BUTT1;
extern uint8_t JSTKUP;
extern uint8_t b0down;
extern uint8_t b1down;
extern uint8_t SINGSTEP;    /* mode pas-à-pas (debug) */
extern uint8_t blackflag;   /* flag pour écran noir temporaire */
extern uint8_t SCRNUM;      /* numéro du screen actuellement affiché (1-24) */
extern uint8_t BlueType[2]; /* type du bloc courant (lecture blueprint) */
extern uint8_t BlueSpec[2]; /* spec du bloc courant */
extern uint8_t CUTTIMER;
extern uint8_t PRECED;      /* objid du bloc précédent (pour contexte dessin) */
extern uint8_t spreced;     /* state du bloc précédent */
extern uint8_t PREV[3];     /* objid des 3 blocs du screen précédent (droite) */
extern uint8_t sprev[3];    /* state des 3 blocs précédents */
extern uint8_t scrnLeft;    /* numéro du screen à gauche */
extern uint8_t scrnRight;
extern uint8_t scrnAbove;
extern uint8_t scrnBelow;
extern uint8_t scrnBelowL;
extern uint8_t scrnAboveL;
extern uint8_t scrnAboveR;
extern uint8_t scrnBelowR;
extern uint8_t kbdX;
extern uint8_t kbdY;
extern uint8_t joyX;
extern uint8_t joyY;
extern uint8_t btn;
extern uint8_t butt;


/* -----------------------------------------------------------------------
 * PAGES 2-3 : Variables supplémentaires ($200-$3FF)
 * ----------------------------------------------------------------------- */
extern uint8_t inmenu;
extern uint8_t inbuilder;
extern uint8_t ineditor;
extern uint8_t soundon;
extern uint8_t jctr[2];
extern uint8_t jthres1x, jthres1y;
extern uint8_t jthres2x, jthres2y;
extern uint8_t jvert, jhoriz, jbtns;
extern uint8_t joyon;
extern uint8_t develment;
extern uint8_t keypress, keydown;
extern uint8_t IIGS;        /* flag Apple IIGS */

/* $3c0 */
extern uint8_t sortX[0x10];
extern uint8_t BELOW[0x10];   /* objid des blocs du screen en dessous */
extern uint8_t SBELOW[0x10];  /* state des blocs du screen en dessous */

/* $3f0 */
extern uint8_t bluepTRK, bluepREG;
extern uint8_t binfoTRK, binfoREG;
extern uint8_t level;        /* numéro de niveau courant (0-14) */
extern uint8_t BBundID;
extern uint8_t redherring2;
extern uint8_t pausetemp;
extern uint8_t recheck0;


/* -----------------------------------------------------------------------
 * CONSTANTES : Dimensions de l'écran
 * ----------------------------------------------------------------------- */
#define ScrnWidth   140   /* largeur en pixels (système 140px) */
#define ScrnHeight  192   /* hauteur en lignes */
#define ScrnLeft     58   /* bord gauche en coordonnée X 140px */
#define ScrnRight   (ScrnLeft + ScrnWidth - 1)   /* = 197 */
#define ScrnTop       0
#define ScrnBottom  (ScrnTop + ScrnHeight - 1)   /* = 191 */


/* -----------------------------------------------------------------------
 * CONSTANTES : Masques de bits pour BLUETYPE
 * secmask : bits 7-6 (numéro de section)
 * reqmask : bit 5   (requiert dessin spécial ?)
 * idmask  : bits 4-0 (identifiant de l'objet, 0-31)
 * ----------------------------------------------------------------------- */
#define secmask  0xC0   /* %11000000 */
#define reqmask  0x20   /* %00100000 */
#define idmask   0x1F   /* %00011111 */


/* -----------------------------------------------------------------------
 * CONSTANTES : Opcodes de dessin (valeur de OPACITY)
 * Détermine l'opération logique appliquée pixel par pixel dans fastlay.
 * ----------------------------------------------------------------------- */
#define OP_AND   0   /* and  : masque (pixels noirs = transparents) */
#define OP_ORA   1   /* ora  : or logique (superposition additive) */
#define OP_STA   2   /* sta  : store direct (opaque, pas de transparence) */
#define OP_EOR   3   /* eor  : xor logique */
#define OP_MASK  4   /* mask : masque inverse */

/* Aliases compatibles avec le code assembleur original */
#define and_op  OP_AND
#define ora     OP_ORA
#define sta_op  OP_STA
#define eor_op  OP_EOR
#define mask    OP_MASK

#endif /* EQ_H */
