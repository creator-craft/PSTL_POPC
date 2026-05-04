#ifndef SEQDATA_H
#define SEQDATA_H

/*
 * seqdata.h
 * Traduction de SEQDATA.S : Points d'entrée et instructions de la table de séquences
 * Source originale : Jordan Mechner, 1989
 *
 * Ce fichier ne contient QUE des #define.
 *
 * CONTEXTE :
 * La seqtable (définie dans GAMEEQ.S, chargée à $2800) est un tableau de
 * 1200 octets organisé en frames de 5 octets chacune :
 *   [Fimage, Fsword, Fdx, Fdy, Fcheck]
 *
 * Les constantes ci-dessous sont des INDICES dans cette table (points d'entrée).
 * Quand le code appelle jumpseq avec l'une de ces valeurs, le pointeur CharSeq
 * est repositionné à cet endroit et l'animation repart depuis ce frame.
 *
 * Les valeurs NÉGATIVES en bas de ce fichier sont des CODES D'INSTRUCTION
 * interprétés par la routine getseq quand elle parcourt la table. Elles forment
 * un mini-langage d'animation embarqué dans la seqtable.
 */


/* -----------------------------------------------------------------------
 * POINTS D'ENTRÉE DANS LA TABLE DE SÉQUENCES
 *
 * Chaque constante est le numéro de frame de début d'une animation.
 * Les animations du joueur (1-93) et des personnages (94-114) sont séparées.
 *
 * Mouvements du joueur :
 * ----------------------------------------------------------------------- */
#define startrun        1    /* début de course */
#define stand           2    /* debout immobile */
#define standjump       3    /* saut depuis l'arrêt */
#define runjump         4    /* saut en course */
#define turn            5    /* demi-tour à l'arrêt */
#define runturn         6    /* demi-tour en course */
#define stepfall        7    /* chute d'un pas */
#define jumphangMed     8    /* saut + accrochage (hauteur moyenne) */
#define hang            9    /* suspendu à un rebord */
#define climbup        10    /* grimper sur un rebord */
#define hangdrop       11    /* lâcher prise vers le bas */
#define freefall       12    /* chute libre */
#define runstop        13    /* arrêt de course */
#define jumpup         14    /* saut vers le haut */
#define fallhang       15    /* chute → accrochage */
#define jumpbackhang   16    /* saut arrière + accrochage */
#define softland       17    /* atterrissage souple (1 étage) */
#define jumpfall       18    /* saut → chute */
#define stepfall2      19    /* chute d'un pas (variante) */
#define medland        20    /* atterrissage moyen (2 étages) */
#define rjumpfall      21    /* saut arrière → chute */
#define hardland       22    /* atterrissage dur (3+ étages) */
#define hangfall       23    /* lâcher prise → chute */
#define jumphangLong   24    /* saut + accrochage (longue distance) */
#define hangstraight   25    /* suspendu droit (sans balancement) */
#define rdiveroll      26    /* roulade plongeon droite */
#define sdiveroll      27    /* roulade plongeon courte */
#define highjump       28    /* grand saut */
/* stepfwd 1 à 14 : entrées 29 à 42 (séquentielles) */
#define stepfwd1       29    /* pas en avant #1 (stepfwd 2-14 = 30-42) */
#define turnrun        43    /* demi-tour → course */
#define testfoot       44    /* tester le sol du bout du pied */
#define bumpfall       45    /* choc → chute */
#define hardbump       46    /* choc violent */
#define bump           47    /* choc léger */
#define superhijump    48    /* super saut haut */
#define standup        49    /* se relever */
#define stoop          50    /* se baisser / ramasser */

/* Animations de mort */
#define impale         51    /* empalé sur des pics */
#define crush          52    /* écrasé par un portail */
#define deadfall       53    /* chute mortelle */
#define halve          54    /* tranché en deux */

/* Combat */
#define engarde        55    /* se mettre en garde */
#define advance        56    /* avancer en garde */
#define retreat        57    /* reculer en garde */
#define strike         58    /* frapper */
#define flee           59    /* fuir */
#define turnengarde    60    /* demi-tour en garde */
#define strikeblock    61    /* parer un coup (animation de parade) */
#define readyblock     62    /* prêt à parer */
#define landengarde    63    /* atterrir en garde (squelette) */
#define bumpengfwd     64    /* choc en avant en garde */
#define bumpengback    65    /* choc en arrière en garde */
#define blocktostrike  66    /* parer → contre-attaque */
#define strikeadv      67    /* frapper en avançant */

/* Autres mouvements */
#define climbdown      68    /* descendre d'un rebord */
#define blockedstrike  69    /* coup bloqué avec succès */
#define climbstairs    70    /* monter un escalier */
#define dropdead       71    /* tomber mort */
#define stepback       72    /* pas en arrière */
#define climbfail      73    /* tentative de grimpe ratée */
#define stabbed        74    /* reçu un coup d'épée */
#define faststrike     75    /* frappe rapide */
#define strikeret      76    /* retour de frappe */
#define alertstand     77    /* debout en alerte (gardes) */
#define drinkpotion    78    /* boire une potion */
#define crawl          79    /* ramper */
#define alertturn      80    /* demi-tour en alerte */
#define fightfall      81    /* chute en combat (joueur) */
#define efightfall     82    /* chute en combat (ennemi) */
#define efightfallfwd  83    /* chute en combat ennemi vers l'avant */
#define running        84    /* course */
#define stabkill       85    /* tuer d'un coup d'épée */
#define fastadvance    86    /* avance rapide */
#define goalertstand   87    /* aller vers alertstand */
#define arise          88    /* se lever (os qui reprennent vie) */
#define turndraw       89    /* demi-tour + sortir l'épée */
#define guardengarde   90    /* garde en position de combat */
#define pickupsword    91    /* ramasser l'épée */
#define resheathe      92    /* rengainer l'épée */
#define fastsheathe    93    /* rengainer rapidement */

/* Animations de la princesse (P) et du vizir (V) : scènes cinématiques */
#define Pstand         94    /* princesse debout */
#define Vstand         95    /* vizir debout */
#define Vapproach      96    /* vizir approche */
#define Vstop          97    /* vizir s'arrête */
#define Palert         98    /* princesse alerte */
#define Pback          99    /* princesse recule */
#define Vexit         100    /* vizir sort */
#define Mclimb        101    /* souris grimpe */
#define Vraise        102    /* vizir lève les bras */
#define Plie          103    /* princesse s'allonge */
#define patchfall     104    /* chute patch */
#define Mscurry       105    /* souris détale */
#define Mstop         106    /* souris s'arrête */
#define Mleave        107    /* souris part */
#define Pembrace      108    /* princesse embrasse */
#define Pwaiting      109    /* princesse attend */
#define Pstroke       110    /* princesse caresse */
#define Prise         111    /* princesse se lève */
#define Pcrouch       112    /* princesse se baisse */
#define Pslump        113    /* princesse s'affaisse */
#define Mraise        114    /* souris se dresse */


/* -----------------------------------------------------------------------
 * CODES D'INSTRUCTION DE LA TABLE DE SÉQUENCES
 *
 * Quand getseq rencontre une valeur négative dans Fimage, elle l'interprète
 * comme une instruction plutôt que comme un numéro d'image.
 * Ces instructions forment un mini-langage d'animation interprété à chaque
 * frame-advance par la routine ANIMCHAR.
 *
 * En assembleur, ces valeurs négatives tiennent sur 1 octet signé (two's complement).
 * En C, on les définit comme int8_t implicitement via des constantes négatives.
 * ----------------------------------------------------------------------- */
#define SEQ_GOTO       (-1)   /* sauter à un autre point d'entrée de la seqtable */
#define SEQ_ABOUTFACE  (-2)   /* faire demi-tour (inverser CharFace) */
#define SEQ_UP         (-3)   /* monter d'un étage (CharBlockY--) */
#define SEQ_DOWN       (-4)   /* descendre d'un étage (CharBlockY++) */
#define SEQ_CHX        (-5)   /* modifier CharX de N pixels (KIDX += N avant le frame) */
#define SEQ_CHY        (-6)   /* modifier CharY de N pixels */
#define SEQ_ACT        (-7)   /* changer CharAction (code d'action suivant) */
#define SEQ_SETFALL    (-8)   /* initialiser la vélocité de chute (setfall X,Y) */
#define SEQ_IFWTLESS   (-9)   /* branchement conditionnel si weightless */
#define SEQ_DIE        (-10)  /* déclencher la mort du personnage */
#define SEQ_JARU       (-11)  /* sauter si rebord au-dessus */
#define SEQ_JARD       (-12)  /* sauter si rebord en-dessous */
#define SEQ_EFFECT     (-13)  /* déclencher un effet sonore */
#define SEQ_TAP        (-14)  /* tap (usage interne animation) */
#define SEQ_NEXTLEVEL  (-15)  /* passer au niveau suivant */

/*
 * Aliases compatibles avec le code assembleur original.
 * Le code source écrit souvent "lda #goto" ou "db goto, stand" directement.
 * Ces aliases permettent d'utiliser les mêmes noms dans auto.c / frameadv.c.
 *
 * ATTENTION : "goto" est un mot réservé en C.
 * On utilise seq_goto à la place partout, sauf dans les tableaux de données
 * où on peut écrire la valeur numérique directement.
 */
#define seq_goto       SEQ_GOTO
#define aboutface      SEQ_ABOUTFACE
#define seq_up         SEQ_UP       /* "up" peut conflictuer avec movedata.h */
#define seq_down       SEQ_DOWN
#define chx            SEQ_CHX
#define chy            SEQ_CHY
#define act            SEQ_ACT
#define setfall        SEQ_SETFALL
#define ifwtless       SEQ_IFWTLESS
#define die            SEQ_DIE
#define jaru           SEQ_JARU
#define jard           SEQ_JARD
#define effect         SEQ_EFFECT
#define tap            SEQ_TAP
#define nextlevel      SEQ_NEXTLEVEL

/*
 * Constantes de Fcheck (5e octet de chaque frame de séquence)
 * Dupliquées ici depuis SEQDATA.S pour cohérence (aussi dans gameeq.h).
 */
#define fcheckmark    0x40   /* %01000000 : floor check requis ce frame */
#define fcentermark   0x1F   /* %00011111 : position X du centre du personnage */

#endif /* SEQDATA_H */
