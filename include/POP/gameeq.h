#ifndef GAMEEQ_H
#define GAMEEQ_H

/*
 * gameeq.h
 * Traduction de GAMEEQ.S — Equates spécifiques au gameplay de Prince of Persia
 * Source originale : Jordan Mechner, 1989
 *
 * Ce fichier regroupe :
 *   - Les adresses des tables d'images (chtable, bgtable)
 *   - Les adresses des modules du jeu (topctrl, ctrl, auto, coll, gamebg...)
 *   - Les jump tables de chaque module → déclarées comme extern void (void)
 *   - Les structures de données mobiles (TROBs, MOBs, saved game)
 *   - Les variables globales de jeu (zero page $40-$e7)
 *   - Les données des personnages (Char, Kid, Shad, Op, FCharVars)
 *   - Les constantes de gameplay
 */

#include <stdint.h>

/* -----------------------------------------------------------------------
 * ADRESSES DES TABLES D'IMAGES
 * Sur Apple II, les tables de sprites sont chargées à des adresses fixes.
 * En C, ce sont des pointeurs vers des buffers chargés depuis les fichiers
 * binaires IMG.CHTAB1..7 et IMG.BGTAB1..2.
 *
 * Note importante (rapport Crouzet/Ayed) : en pratique, toutes les images
 * sont extraites avec l'adresse de base 0x6000 (= bgtable1/chtable1),
 * quelle que soit la table. Les adresses ci-dessous correspondent au code
 * source original mais certaines sont inexploitables directement.
 * ----------------------------------------------------------------------- */

/* Tables de personnages (character image tables) */
#define CHTABLE1_ADDR  0x6000   /* player (kid), misc shapes */
#define CHTABLE2_ADDR  0x8400   /* player (kid), misc shapes */
#define CHTABLE3_ADDR  0x0800   /* player (kid), misc shapes */
#define CHTABLE4_ADDR  0x9600   /* enemy (guard, fat guard, skeleton, vizier, shadow) */
#define CHTABLE5_ADDR  0xa800   /* player (kid), misc shapes */
#define CHTABLE6_ADDR  0x6000   /* princess scenes (chtab6a = niveaux 1-2, chtab6b = 3+) */
#define CHTABLE7_ADDR  0x9f00   /* princess scenes (princess, vizier, hourglass) */

/* Tables de background (background image tables) */
#define BGTABLE1_ADDR  0x6000   /* dungeon background */
#define BGTABLE2_ADDR  0x8400   /* palace background */

/* Pointeurs effectifs vers les données binaires chargées en mémoire */
extern uint8_t *chtable1;
extern uint8_t *chtable2;
extern uint8_t *chtable3;
extern uint8_t *chtable4;
extern uint8_t *chtable5;
extern uint8_t *chtable6;
extern uint8_t *chtable7;
extern uint8_t *bgtable1;
extern uint8_t *bgtable2;


/* -----------------------------------------------------------------------
 * OBJETS TRANSITOIRES (TROBs) ET OBJETS MOBILES (MOBs)
 * Définis à l'adresse mobtables = $b600 en aux mem.
 *
 * TROBs (Transitional Objects) : objets qui ont des parties mobiles mais
 *   restent à position fixe (portail, pics, plaque pression, torche).
 *   Stockés dans trloc/trscrn/trdirec.
 *
 * MOBs (Mobile Objects) : objets pouvant se déplacer entre screens
 *   (plancher en chute). Stockés dans mobx/moby/mobscrn/mobvel/mobtype/moblevel.
 * ----------------------------------------------------------------------- */
#define TROBSPACE  0x20   /* 32 entrées max pour les TROBs */
#define MOBSPACE   0x10   /* 16 entrées max pour les MOBs */
#define MAXSFX     0x20   /* 32 entrées max pour les effets sonores */

/* TROBs */
extern uint8_t trloc[TROBSPACE];    /* position bloc (0-29) */
extern uint8_t trscrn[TROBSPACE];   /* numéro de screen (1-24) */
extern uint8_t trdirec[TROBSPACE];  /* direction/phase de mouvement ; -1 = inactif */

/* MOBs */
extern uint8_t mobx[MOBSPACE];      /* coordonnée X (0-39) */
extern uint8_t moby[MOBSPACE];      /* coordonnée Y */
extern uint8_t mobscrn[MOBSPACE];   /* numéro de screen */
extern uint8_t mobvel[MOBSPACE];    /* vélocité */
extern uint8_t mobtype[MOBSPACE];   /* type : 0 = plancher en chute */
extern uint8_t moblevel[MOBSPACE];  /* niveau de bloc (0-2) */

/* Table des effets sonores actifs */
extern uint8_t soundtable[MAXSFX];

/* Compteur de TROBs actifs */
extern uint8_t trobcount;


/* -----------------------------------------------------------------------
 * DONNÉES DU JEU SAUVEGARDÉ (savedgame = $b6f0)
 * ----------------------------------------------------------------------- */
extern uint8_t  SavLevel;
extern uint8_t  SavStrength;
extern uint8_t  SavMaxed;
extern uint8_t  SavTimer[2];   /* 2 octets (minutes + secondes ?) */
extern uint8_t  SavNextMsg;


/* -----------------------------------------------------------------------
 * JUMP TABLES — TOPCTRL.S (topctrl = $2000)
 * Routines d'initialisation et boucle principale du jeu.
 * ----------------------------------------------------------------------- */
extern void start(void);
extern void restart(void);
extern void startresume(void);
extern void initsystem(void);
extern void docrosscut(void);
extern void goattract(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — CTRL.S (ctrl = $3a00)
 * Routines de contrôle du personnage joueur.
 * ----------------------------------------------------------------------- */
extern void PlayerCtrl(void);
extern void checkfloor(void);
extern void ShadCtrl(void);
extern void rereadblocks(void);
extern void checkpress(void);
extern void DoImpale(void);
extern void GenCtrl(void);
extern void checkimpale(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — AUTO.S (auto = $5400)
 * Routines de comportement autonome des ennemis (IA).
 * ----------------------------------------------------------------------- */
extern void AutoCtrl(void);
extern void checkstrike(void);
extern void checkstab(void);
extern void AutoPlayback(void);
extern void cutcheck(void);
extern void cutguard(void);
extern void addguard(void);
extern void cut(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — COLL.S (coll = $4500)
 * Routines de détection de collision.
 * ----------------------------------------------------------------------- */
extern void checkbarr(void);
extern void collisions(void);
extern void getfwddist(void);
extern void checkcoll(void);
extern void animchar(void);
extern void checkslice(void);
extern void checkslice2(void);
extern void checkgate(void);
extern void enemycoll(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — GAMEBG.S (gamebg = $4c00)
 * Routines d'affichage des éléments de jeu (barres de vie, personnages...).
 * ----------------------------------------------------------------------- */
extern void updatmeters(void);
extern void DrawKidMeter(void);
extern void DrawSword(void);
extern void DrawKid(void);
extern void DrawShad(void);
extern void setupflame(void);
extern void continuemsg(void);
extern void addcharobj(void);
extern void setobjindx(void);
extern void printlevel(void);
extern void DrawOppMeter(void);
extern void flipdiskmsg(void);
extern void timeleftmsg(void);
extern void DrawGuard(void);
extern void DrawGuard2(void);
extern void setupflask(void);
extern void setupcomix(void);
extern void psetupflame(void);
extern void drawpost(void);
extern void drawglass(void);
extern void initlay(void);
extern void twinkle(void);
extern void flow(void);
extern void pmask(void);
extern void yellow(void);
extern void setrecheck0(void);
extern void recheckyel(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — SPECIALK.S (specialk = $d900)
 * Routines de gestion des inputs et du timing.
 * ----------------------------------------------------------------------- */
extern void keys(void);
extern void clrjstk(void);
extern void zerosound(void);
extern void addsound(void);
extern void facejstk(void);
extern void SaveSelect(void);
extern void LoadSelect(void);
extern void SaveDesel(void);
extern void LoadDesel(void);
extern void initinput(void);
extern void demokeys(void);
extern void listtorches(void);
extern void burn(void);
extern void getminleft(void);
extern void keeptime(void);
extern void shortentime(void);
extern void cuesong(void);
extern void dloop(void);
extern void strobe(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — MOVER.S (mover = $ee00)
 * Routines d'animation des objets transitoires et mobiles.
 * ----------------------------------------------------------------------- */
extern void animtrans(void);
extern void trigspikes(void);
extern void pushpp(void);
extern void breakloose1(void);
extern void breakloose(void);
extern void animmobs(void);
extern void addmobs(void);
extern void closeexit(void);
extern void getspikes(void);
extern void shakem(void);
extern void trigslicer(void);
extern void trigtorch(void);
extern void getflameframe(void);
extern void smashmirror(void);
extern void jamspikes(void);
extern void trigflask(void);
extern void getflaskframe(void);
extern void trigsword(void);
extern void jampp(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — CTRLSUBS.S (ctrlsubs = $d000)
 * Sous-routines de contrôle / utilitaires communs.
 * ----------------------------------------------------------------------- */
extern void getframe(void);
extern void getseq(void);
extern void getbasex(void);
extern void getblockx(void);
extern void getblockxp(void);
extern void getblocky(void);
extern void getblockej(void);
extern void addcharx(void);
extern void getdist(void);
extern void getdist1(void);
extern void getabovebeh(void);
extern void rdblock(void);
extern void rdblock1(void);
extern void setupsword(void);
extern void getscrns(void);
extern void addguardobj(void);
extern void opjumpseq(void);
extern void getedges(void);
extern void indexchar(void);
extern void quickfg(void);
extern void cropchar(void);
extern void getleft(void);
extern void getright(void);
extern void getup(void);
extern void getdown(void);
extern void cmpspace(void);
extern void cmpbarr(void);
extern void addkidobj(void);
extern void addshadobj(void);
extern void addreflobj(void);
extern void LoadKid(void);
extern void LoadShad(void);
extern void SaveKid(void);
extern void SaveShad(void);
extern void setupchar(void);
extern void GetFrameInfo(void);
extern void indexblock(void);
extern void markred(void);
extern void markfred(void);
extern void markwipe(void);
extern void markmove(void);
extern void markfloor(void);
extern void unindex(void);
extern void quickfloor(void);
extern void unevenfloor(void);
extern void markhalf(void);
extern void addswordobj(void);
extern void getblocky1(void);
extern void checkledge(void);
extern void get2infront(void);
extern void checkspikes(void);
extern void rechargemeter(void);
extern void addfcharx(void);
extern void facedx(void);
extern void jumpseq(void);
extern void GetBaseBlock(void);
extern void LoadKidwOp(void);
extern void SaveKidwOp(void);
extern void getopdist(void);
extern void LoadShadwOp(void);
extern void SavShadwOp(void);
extern void boostmeter(void);
extern void getunderft(void);
extern void getinfront(void);
extern void getbehind(void);
extern void getabove(void);
extern void getaboveinf(void);
extern void cmpwall(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — SUBS.S (subs = $e000)
 * Sous-routines diverses du jeu.
 * ----------------------------------------------------------------------- */
extern void addtorches(void);
extern void doflashon(void);
extern void PageFlip(void);
extern void demo(void);
extern void showtime(void);
extern void doflashoff(void);
extern void lrclse(void);
extern void addslicers(void);
extern void pause_game(void);   /* "pause" est un mot réservé en C++ */
extern void deadenemy(void);
extern void playcut(void);
extern void addlowersound(void);
extern void RemoveObj(void);
extern void addfall(void);
extern void setinitials(void);
extern void startkid(void);
extern void startkid1(void);
extern void gravity(void);
extern void initialguards(void);
extern void mirappear(void);
extern void crumble(void);


/* -----------------------------------------------------------------------
 * JUMP TABLES — MISC.S (misc = $f900)
 * ----------------------------------------------------------------------- */
extern void VanishChar(void);
extern void movemusic(void);
extern void moveauxlc(void);
extern void firstguard(void);
extern void markmeters(void);
extern void potioneffect(void);
extern void mouserescue(void);
extern void StabChar(void);
extern void unholy(void);
extern void reflection(void);
extern void MarkKidMeter(void);
extern void MarkOppMeter(void);
extern void bonesrise(void);
extern void decstr(void);
extern void DoSaveGame(void);
extern void LoadLevelX(void);
extern void checkalert(void);
extern void dispversion(void);


/* -----------------------------------------------------------------------
 * SEQUENCE TABLE (seqtable = $2800, seqtab = $3000)
 * Contient les définitions de frames pour les animations.
 * 1200 octets = 240 frames × 5 octets chacune.
 * altset1/altset2 : jeux d'animation alternatifs.
 * swordtab : table des positions d'épée (192 octets).
 * ----------------------------------------------------------------------- */
#define FDEF_SIZE     1200
#define ALTSET1_SIZE   200
#define ALTSET2_SIZE   450
#define SWORDTAB_SIZE  192

extern uint8_t Fdef[FDEF_SIZE];
extern uint8_t altset1[ALTSET1_SIZE];
extern uint8_t altset2[ALTSET2_SIZE];
extern uint8_t swordtab[SWORDTAB_SIZE];


/* -----------------------------------------------------------------------
 * VARIABLES GLOBALES DE JEU — Zero page $40-$e7
 *
 * Sur Apple II, ces variables sont dans la zone zero page pour un accès
 * rapide (1 cycle au lieu de 3-4). En C, on les déclare globalement.
 *
 * STRUCTURE DES DONNÉES PERSONNAGE :
 * Char, Kid, Shad sont des blocs de 16 octets identiques.
 * Char est utilisé comme espace de travail temporaire.
 * Kid et Shad sont les données permanentes du joueur et de son adversaire.
 * ----------------------------------------------------------------------- */

/* Bloc de données d'un personnage — 16 octets */
typedef struct {
    uint8_t Posn;    /* frame # de la position courante */
    uint8_t X;       /* coordonnée X (système 140px) */
    uint8_t Y;       /* coordonnée Y */
    uint8_t Face;    /* direction : 0 = droite, -1 (0xFF) = gauche */
    uint8_t BlockX;  /* bloc X courant (0-9) */
    uint8_t BlockY;  /* bloc Y courant (0-2) */
    uint8_t Action;  /* code d'action (voir CharAction values) */
    uint8_t XVel;    /* vélocité X (freefall) */
    uint8_t YVel;    /* vélocité Y (freefall) */
    uint8_t SeqL;    /* pointeur séquence (octet bas) */
    uint8_t SeqH;    /* pointeur séquence (octet haut) */
    uint8_t Scrn;    /* numéro de screen courant (0 = null) */
    uint8_t Repeat;  /* flag de répétition au bord d'un gouffre */
    uint8_t ID;      /* identifiant du personnage (voir CharID values) */
    uint8_t Sword;   /* 2 = épée sortie, 0 = épée rangée */
    uint8_t Life;    /* points de vie : -1 = vivant, 0-127 = mort */
} CharData;

extern CharData Char;   /* espace de travail temporaire */
extern CharData Kid;    /* données permanentes du joueur */
extern CharData Shad;   /* données permanentes de l'adversaire */

/* Accès aux champs via les noms originaux du code source */
#define CharPosn   Char.Posn
#define CharX      Char.X
#define CharY      Char.Y
#define CharFace   Char.Face
#define CharBlockX Char.BlockX
#define CharBlockY Char.BlockY
#define CharAction Char.Action
#define CharXVel   Char.XVel
#define CharYVel   Char.YVel
#define CharSeq    Char.SeqL   /* SeqL est le premier octet des 2 */
#define CharScrn   Char.Scrn
#define CharRepeat Char.Repeat
#define CharID     Char.ID
#define CharSword  Char.Sword
#define CharLife   Char.Life

#define KidPosn    Kid.Posn
#define KidX       Kid.X
/* etc. — pattern identique pour Kid et Shad */

/* Opponent (adversaire courant dans l'espace de travail) */
/* Op est un alias vers la zone Op dans la page 2-3 ($320) */
extern CharData Op;   /* espace de travail adversaire */

/* FCharVars — données du "free character" (personnage libre) : 12 octets */
typedef struct {
    uint8_t Image;
    uint8_t X[2];    /* 2 octets */
    uint8_t Y;
    uint8_t Face;
    uint8_t Index;
    uint8_t CU, CD, CL, CR;
    uint8_t Table;
} FCharVarsData;
extern FCharVarsData FCharVars;


/* -----------------------------------------------------------------------
 * VARIABLES GLOBALES — divers ($40 suite)
 * ----------------------------------------------------------------------- */
extern uint8_t  yellowflag;
extern uint8_t  timebomb;
extern uint8_t  justblocked;
extern uint8_t  gdtimer;
extern uint8_t  framepoint[2];
extern uint8_t  Fimage;
extern uint8_t  Fdx;
extern uint8_t  Fdy;
extern uint8_t  Fcheck;
extern uint8_t  exitopen;
extern uint8_t  collX;
extern uint8_t  lightning;
extern uint8_t  lightcolor;
extern uint8_t  offguard;
extern uint8_t  blockid;
extern uint8_t  blockx;
extern uint8_t  blocky;
extern uint8_t  infrontx;
extern uint8_t  behindx;
extern uint8_t  abovey;
extern uint8_t  tempblockx, tempblocky, tempscrn, tempid;
extern uint8_t  numtrans;      /* nombre de TROBs actifs */
extern uint8_t  tempnt;
extern uint8_t  redrawflg;
extern uint8_t  xdiff[2];
extern uint8_t  ydiff[2];
extern uint8_t  xdir, ydir;
extern uint8_t  RNDseed;
extern uint8_t  invert;
extern uint8_t  PlayCount;
extern uint8_t  refract;
extern uint8_t  backtolife;
extern uint8_t  cutplan;
extern uint8_t  lastcmd;
extern uint8_t  distfallen;
extern uint8_t  cutscrn;
extern uint8_t  waitingtojump;
extern uint8_t  trigppabove;
extern uint8_t  direcpp;
extern uint8_t  blockaddr[2];
extern uint8_t  delay;
extern uint8_t  XCOORD[2];
extern uint8_t  savekidx;
extern uint8_t  mirrx;
extern uint8_t  dmirr;
extern uint8_t  barrdist, barrcode;
extern uint8_t  imwidth, imheight;
extern uint8_t  leadedge;
extern uint8_t  leftej, rightej, topej;
extern uint8_t  leftblock, rightblock, topblock, bottomblock;
extern uint8_t  CDLeftEj, CDRightEj;
extern uint8_t  endrange;
extern uint8_t  bufindex;
extern uint8_t  blockedge;
extern uint8_t  collideL, collideR;
extern uint8_t  weightless;
extern uint8_t  cutorder;
extern uint8_t  AMtimer;
extern uint8_t  begrange;
extern uint8_t  scrn;
extern uint8_t  keybufptr;
extern uint8_t  VisScrn;       /* screen visible à l'écran */
extern uint8_t  OppStrength;
extern uint8_t  jarabove;
extern uint8_t  KidStrength;
extern uint8_t  ChgKidStr;
extern uint8_t  MaxKidStr;
extern uint8_t  EnemyAlert;
extern uint8_t  ChgOppStr;
extern uint8_t  heroic;
extern uint8_t  clrF, clrB, clrU, clrD, clrbtn;
extern uint8_t  Fsword;
extern uint8_t  purpleflag;
extern uint8_t  msgtimer;
extern uint8_t  MaxOppStr;
extern uint8_t  guardprog;
extern uint8_t  ManCtrl;
extern uint8_t  mergetimer;
extern uint8_t  lastpotion;
extern uint8_t  origstrength;
extern uint8_t  jmpaddr[2];
extern uint8_t  alertguard;
extern uint8_t  createshad;
extern uint8_t  stunned;
extern uint8_t  droppedout;


/* -----------------------------------------------------------------------
 * PAGES 2-3 — Variables de jeu supplémentaires
 * ----------------------------------------------------------------------- */
extern uint8_t  milestone;
extern uint8_t  GlassState;
extern uint8_t  redrawglass;
extern uint8_t  doortop;
extern uint8_t  GuardColor;
extern uint8_t  shadowaction;
extern uint8_t  skipmessage;
extern uint8_t  savezp[32];
extern uint8_t  MSset;
extern uint8_t  rjumpflag;
extern uint8_t  redherring;

/* $300 */
extern uint8_t  MinLeft;        /* minutes restantes */
extern uint8_t  NextTimeMsg;
extern uint8_t  SecLeft;        /* secondes restantes */
extern uint8_t  BGset1, BGset2; /* jeu de background actif */
extern uint8_t  CHset;          /* jeu de sprites actif */
extern uint8_t  FrameCount[2];
extern uint8_t  SongCount;
extern uint8_t  PreRecPtr;
extern uint8_t  gotsword;       /* flag : le joueur a l'épée */
extern uint8_t  message;
extern uint8_t  SPEED;
extern uint8_t  nummob;         /* nombre de MOBs actifs */
extern uint8_t  clrSEL[5];
extern uint8_t  clrDESEL[5];
extern uint8_t  vibes;
extern uint8_t  SongCue;
extern uint8_t  musicon;
extern uint8_t  redkidmeter;
extern uint8_t  NextLevel;
extern uint8_t  scrncolor;
extern uint8_t  redoppmeter;
extern uint8_t  timerequest;

/* $320 — tableaux de collision detection */
#define CD_SIZE 0x10
extern uint8_t CDthisframe[CD_SIZE];
extern uint8_t CDlastframe[CD_SIZE];
extern uint8_t CDbelow[CD_SIZE];
extern uint8_t CDabove[CD_SIZE];
extern uint8_t SNthisframe[CD_SIZE];
extern uint8_t SNlastframe[CD_SIZE];
extern uint8_t SNbelow[CD_SIZE];
extern uint8_t SNabove[10];
extern uint8_t BlockYthis;
extern uint8_t BlockYlast;

/* Keyboard buffer */
#define KEYBUFLEN 10
extern uint8_t  keybuf[KEYBUFLEN];


/* -----------------------------------------------------------------------
 * CONSTANTES DE GAMEPLAY
 * ----------------------------------------------------------------------- */

/* Bits du frame check (Fcheck / Fcheckmark) */
#define Fcheckmark  0x40   /* %01000000 — floor check requis */
#define Fthinmark   0x20   /* %00100000 — frame "thin" pour collision */
#define Ffootmark   0x1F   /* %00011111 — position X du pied (0-31) */

/* Dimensions physiques */
#define floorheight  15    /* hauteur d'un bloc en pixels */
#define angle         7    /* angle pour certains calculs de position */
#define VertDist     11    /* distance verticale entre niveaux de blocs */

/* Modes de dessin des personnages */
#define UseFastlay   0     /* utiliser fastlay (pas d'offset, pas de clip) */
#define UseLay       1     /* utiliser lay (complet) */
#define UseLayrsave  2     /* utiliser layrsave (sauvegarde couche) */

/* CharID values — identifiants des personnages */
#define TypeKid      0
#define TypeShad     1
#define TypeGd       2     /* guard / vizier en jeu */
#define TypeSword    3
#define TypeReflect  4
#define TypeComix    5
#define TypeFF       0x80  /* free-floating character */

/* CharAction values — codes d'action */
#define ACTION_DEAD    (-1)   /* mort */
#define ACTION_STAND     0    /* debout immobile */
#define ACTION_RUN       1    /* course / saut (nécessite sol sous les pieds) */
#define ACTION_HANG      2    /* suspendu / grimpe */
#define ACTION_MIDAIR    3    /* en l'air (brièvement) */
#define ACTION_FREEFALL  4    /* chute libre */
#define ACTION_BUMP      5    /* repoussé */
#define ACTION_HANGSTR   6    /* suspendu droit */
#define ACTION_TURNING   7    /* en train de se retourner */

/* CharFace values */
#define FACE_RIGHT   0      /* regarde à droite */
#define FACE_LEFT    0xFF   /* regarde à gauche (= -1 en signé) */

/* CharSword values */
#define SWORD_SHEATHED  0   /* épée rangée */
#define SWORD_DRAWN     2   /* épée sortie */

/* CharLife values */
#define ALIVE  0xFF  /* = -1 signé : vivant */
/* 0-127 : nombre de frames depuis la mort */

/* Potion IDs (utilisés dans potioneffect) */
#define POTION_EMPTY      0
#define POTION_HEAL       1   /* +1 unité de force */
#define POTION_BOOST      2   /* force max +1 */
#define POTION_WEIGHTLESS 3
#define POTION_UPSIDEDOWN 4
#define POTION_POISON     5   /* -1 unité de force */

/* NonExistent character code */
#define NONEXISTENT_CHAR  86  /* CharFace = 86 → personnage inexistant */

#endif /* GAMEEQ_H */
