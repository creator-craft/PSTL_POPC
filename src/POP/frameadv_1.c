//
// Created by OrianeCrouzet on 26/02/2025.
//

#include "vars.h"
#include "bgdata.h"

//#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Buffers de mise à jour (simulés)
uint8_t wipebuf[30], redbuf[30], movebuf[30], floorbuf[30], halfbuf[30], objbuf[30], fredbuf[30], topbuf[30];

// Tableaux
uint8_t BlockBot[SCREEN_HEIGHT] = {10, 20, 30}; // Exemple de coordonnées Y
uint8_t Mult10[SCREEN_HEIGHT] = {0, 10, 20}; // Index des blocs
uint8_t PREV[SCREEN_HEIGHT] = {0, 0, 0}; // Blocs précédents
uint8_t sprev[SCREEN_HEIGHT] = {0, 0, 0}; // États des blocs précédents


// *********************** Fonctions à implémenter dans d'autres fichiers ***********************

void setupflame(uint8_t spreced);
void addback();
void setback();
uint8_t calcblue(); // Fonction fictive pour récupérer l'adresse du screen
void maddfore();
void addfore();
void DrawKid();
void DrawShad();
void addmid();
// Déclaration d'un pointeur de fonction global
void (*add)();
// Déclaration de la fonction cible
extern void addmidezfast();
void DrawSword();
void DrawGuard();
void addamask();
void updatemeters();
void addwipe();
void wipe(uint8_t height);
void setupflask(uint8_t state);
void addmidezo();

// *********************** Fonctions secondaires ***********************

// Fonction de vérification d'un index donné
uint8_t mbsub(uint8_t index) {
    return redbuf[index] | floorbuf[index] | halfbuf[index] | fredbuf[index] | wipebuf[index];
}

// Vérifier si la barre de vie du joueur doit être redessinée
uint8_t metbufs3(uint8_t index, uint8_t value) {
    value |= mbsub(index);
    index++;
    return value;
}

// Vérifier si la barre de vie de l’adversaire doit être redessinée
uint8_t metbufs2(uint8_t index, uint8_t value) {
    value |= mbsub(index);
    index++;
    return value;
}

uint8_t getinitobj1() {
    uint8_t spec = BlueSpec[yindex];  // Récupère la valeur de BlueSpec

    switch (objid) {
    case GATE:  // Cas des portes
        return (spec >= GMINVAL && spec <= GMAXVAL) ? (spec * 4) : 0;
        // `spec * 4` simule un comportement basé sur `GMAXVAL`

    case LOOSE: // Sol fragile
        return 0;  // Aucun état initial spécifique

    case FLASK: // Flasque (valeur multipliée par 32)
        return spec << 5;  // `asl` * 5 = Multiplication par 32

    default: // Si ce n'est pas un gadget
        return 0;
    }
}

uint8_t getobjbldr() {
    objid = BlueType[y] & IDMASK;  // Récupération de l’ID de l’objet

    if (getinitobj1()) {  // Appelle `getinitobj1()`, si carry set en assembleur → return != 0
        state = BlueSpec[y];  // Si `getinitobj1()` ne modifie pas `state`, on prend `BlueSpec[y]`
    }

    return objid;
}

uint8_t getobjid1() {
    // **Mode éditeur**
    if (inbuilder) {
        return getobjbldr();
    }

    // **Récupération de l'état de l'objet**
    state = BlueSpec[y];

    // **Extraction de l'ID**
    objid = BlueType[y] & IDMASK;

    // **Cas particulier : Plaque de pression normale**
    if (objid == PRESSPLATE) {
        uint8_t link_index = state;
        uint8_t link_state = LINKMAP[link_index] & 0x1F;  // Bits 0-4

        if (link_state >= 2) {
            return DPRESSPLATE;  // Plaque enfoncée
        }
        return PRESSPLATE;  // Plaque relevée
    }

    // **Cas particulier : Plaque de pression supérieure**
    if (objid == UPRESSPLATE) {
        uint8_t link_index = state;
        uint8_t link_state = LINKMAP[link_index] & 0x1F;  // Bits 0-4

        if (link_state >= 2) {
            state = 0;
            return FLOOR;  // Plaque haute enfoncée = sol normal
        }
        return UPRESSPLATE;  // Plaque haute relevée
    }

    return objid;
}

uint8_t getState() {
    return BlueSpec[y];  // Renvoie directement l’état de l’objet
}

/*-------------------------------
*
*  Get objids & states of 3 rightmost blocks
*  of left-neighboring screen
*
*  Out: PREV/sprev [0-2]
*
*-------------------------------*/
void getprev() {
    if (SCRNUM == 0) {
        if (scrnLeft == 0) {
            PREV[0] = PREV[1] = PREV[2] = BLOCK_ID;
            sprev[0] = sprev[1] = sprev[2] = 0;
        }
        return;
    }

    if (scrnLeft == 0) {
        return;
    }

    calcblue();  // Calcul de l’adresse mémoire de l’écran de gauche

    // Récupération des objets et états aux positions 9, 19 et 29
    y = 9;
    PREV[0] = getobjid1();
    sprev[0] = getState();

    y = 19;
    PREV[1] = getobjid1();
    sprev[1] = getState();

    y = 29;
    PREV[2] = getobjid1();
    sprev[2] = getState();
}

/*-------------------------------
*
*  Get objid & state
*
*  In: BlueType/Spec,Y
*
*  Out: A = objid
*       state = state
*
*  Preserves X & Y
*
*-------------------------------*/
uint8_t getobjid() {
    if (SCRNUM == 0) return 0;  // Écran nul (aucun blueprint)

    // Mode éditeur
    if (inbuilder) return getobjbldr();

    state = BlueSpec[y];  // Charge l’état de l’objet
    objid = BlueType[y] & IDMASK;  // Extrait l’ID de l’objet

    // **Gestion des plaques de pression**
    if (objid == PRESSPLATE) {
        uint8_t link_index = state;
        uint8_t link_state = LINKMAP[link_index] & 0x1F;  // Bits 0-4

        if (link_state >= 2) {
            return DPRESSPLATE;  // Plaque enfoncée
        }
        return PRESSPLATE;  // Plaque relevée
    }

    // **Gestion des plaques de pression supérieures**
    if (objid == UPRESSPLATE) {
        uint8_t link_index = state;
        uint8_t link_state = LINKMAP[link_index] & 0x1F;  // Bits 0-4

        if (link_state >= 2) {
            state = 0;
            return FLOOR;  // Plaque haute enfoncée = sol normal
        }
        return UPRESSPLATE;  // Plaque haute relevée
    }

    return objid;
}

/*-------------------------------
*
*  D R A W   G A T E   " B "
*
*  Lay down (STA) the gate in sections, bottom
*  to top.  The bottom piece has two blank lines that
*  erase its trail as the gate rises.
*  Topmost section has 8 separate shapes, 1-8 pixels high.
*
*-------------------------------*/
void setupdgb() {
    blockthr = Dy - 62;  // Définition de la ligne la plus haute de la section B

    state = spreced;
    if (state > GMAXVAL) {
        state = GMAXVAL;
    }

    gateposn = (state / 4) + 1;  // Équivalent de `lsr lsr` puis `adc #1`
    gatebot = Ay - gateposn;  // Positionner la base de la porte
}

void drawgatebf() {
    setupdgb();  // Configuration de la porte (position et limites)

    // **Affichage de la base de la porte**
    OPACITY = ORA_OPACITY;
    YCO = gatebot - 2;  // Décalage vers le haut pour éviter les lignes blanches

    IMAGE = GATEBOT_ORA;
    addfore();

    // **Affichage des pièces intermédiaires**
    IMAGE = GATEB1;
    YCO = gatebot - 12;  // Déplacement initial vers le haut

    while (1) {
        if (YCO >= 192) return;  // Vérification de la hauteur maximale

        YCO -= 7;  // Vérification si la grille dépasse la zone du bloc
        if (YCO < blockthr) break;

        addfore();  // Ajouter la section de la porte

        YCO -= 8;  // Déplacement vers le haut pour la prochaine pièce
    }
}

/*-------------------------------
*
*  Compare object [xsave] with object [xsave-1]
*
*  If X is IN FRONT OF X-1, or if it doesn't matter, return cc;
*  If X is BEHIND X-1, return cs (switch 'em).
*
*-------------------------------*/
uint8_t compare(uint8_t x) {
    obj1 = sortX[x];     // Objet actuel
    obj2 = sortX[x - 1]; // Objet précédent

    // **Vérification du type d’objet**
    if (objTYP[obj1] == TYPE_SHAD) {
        return 1;  // L'ombre est toujours derrière, pas d'échange nécessaire
    }

    // **Comparaison des positions Y**
    if (objY[obj1] < objY[obj2]) {
        return 1;  // `obj1` est plus haut → Derrière (`clc` en assembleur)
    } else if (objY[obj1] > objY[obj2]) {
        return 0;  // `obj1` est plus bas → Devant (`sec` en assembleur)
    }

    // **Cas où `objY[obj1] == objY[obj2]`**
    return 1;  // Pas besoin d’échanger
}

/*-------------------------------
*
*  Sort objects in sort list into back-to-front order
*  (Foremost object should be at bottom of list)
*
*-------------------------------*/
void sortlist() {
    do {
        switches = 0;  // Réinitialise le compteur de permutations
        uint8_t x = sortX[0];  // Commence par le dernier élément

        for (uint8_t i = 1; i < x; i++) {
            if (!compare(i)) {  // compare(obj[i], obj[i-1]) → renvoie 0 si besoin d'un échange
                // Échanger sortX[i] et sortX[i-1]
                uint8_t temp = sortX[i];
                sortX[i] = sortX[i - 1];
                sortX[i - 1] = temp;

                switches = 1;  // Une permutation a eu lieu
            }
        }
    } while (switches);  // Répète jusqu'à ce qu'aucune permutation ne soit faite
}

/*-------------------------------
*
*  Get objids & states of 10 blocks in row below,
*  1 block to left
*
*  In:  rowno
*  Out: BELOW/SBELOW [0-9]
*
*  Use getbelow1 to look at screens other than scrnBelow
*  (In: A = scrn #)
*
*-------------------------------*/
void getbelow() {
    if (rowno < 2) {
        // **ONSCREEN** : On regarde sous les lignes du haut et du milieu
        BELOW[0] = PREV[1];  // Bloc précédent
        SBELOW[0] = sprev[1];  // État du bloc précédent

        yindex += 10;  // Aller à la ligne suivante
        for (uint8_t i = 1; i < 10; i++) {
            BELOW[i] = getobjid();
            SBELOW[i] = getState();
            yindex++;
        }
        yindex -= 10;  // Restauration de yindex
        return;
    }

    // **OFFSCREEN** : On regarde sous la ligne du bas
    if (scrnBelow == 0) {
        // Il n'y a pas d'écran en dessous → On remplit avec des blocs noirs
        for (uint8_t i = 1; i < 10; i++) {
            BELOW[i] = BLOCK;
        }
    } else {
        calcblue();  // Calcule l'adresse mémoire de l'écran en dessous

        // Remplissage de `BELOW` en ignorant le bloc le plus à droite
        for (uint8_t i = 1; i < 9; i++) {
            yindex = i + 10;
            BELOW[i] = getobjid();
            SBELOW[i] = getState();
        }
    }

    // **Vérification de l'écran en bas à gauche**
    if (scrnBelowL == 0) {
        // Il n'y a pas d'écran en bas à gauche
        BELOW[0] = (level == LEVEL_MAX) ? SPACE : BLOCK;
    } else {
        calcblue();  // Calcule l'adresse mémoire de l'écran en bas à gauche
        yindex = 9;
        BELOW[0] = getobjid();
        SBELOW[0] = getState();
    }

    // **Restauration de l'écran actuel**
    calcblue();
}

/*-------------------------------
*
*  L O A D   O B J E C T
*
*  Load vars with object data
*
*  In:  x = object table index
*       X, OFF, Y, IMG, FACE, TYP, CU, CD, CL, CR, TAB
*
*  Out: XCO, OFFSET, YCO, IMAGE, TABLE
*       FCharFace, FCharCU-CD-CL-CR
*       A = objTYP
*
*-------------------------------*/
uint8_t loadobj(uint8_t index) {
    if (index >= MAX_OBJECTS) return 0; // Sécurité pour éviter un dépassement

    GameObject obj = objTable[index];

    FCharX = XCO = obj.x;
    OFFSET = obj.offset;
    FCharY = YCO = obj.y;
    IMAGE = obj.img;
    TABLE = obj.tab;
    FCharFace = obj.face;
    FCharCU = obj.cu;
    FCharCD = obj.cd;
    FCharCL = obj.cl;
    FCharCR = obj.cr;

    return obj.type;  // Renvoie le type d'objet
}

/*-------------------------------
*  Draw flask A (bubbles)
*-------------------------------*/
void drawflaska() {
    if (inbuilder) return;  // Pas d'affichage des bulles en mode éditeur

    XCO = blockxco;
    YCO = Ay;

    setupflask(state);  // Utilisation directe de la variable globale
    addmidezo();
}

/*-------------------------------
* Draw Gate B Front?
* (only if kid is to the left of bars)
*-------------------------------*/
void DrawGateBF() {
    if (rowno != KidBlockY) return;  // Le Kid est-il sur la même ligne ?

    if (colno - 1 != KidBlockX) return;  // Est-il dans le bloc de la porte ?

    if (scrnRight == KidScrn) return;  // Vérification d’écran

    drawgatebf();  // Dessine les barreaux de la porte
}

/*-------------------------------
*
*  Return cs if C-section is visible, cc if hidden
*
*-------------------------------*/
uint8_t checkc() {
    if (objid == SPACE || objid == PILLARTOP || objid == PANELWOF || objid >= ARCHTOP1) {
        return 1;  // C-section est visible
    }
    return 0;  // C-section est cachée
}

/*-------------------------------
* Draw slicer front
*-------------------------------*/
void drawslicerf() {
    uint8_t x = state & 0x7F;
    if (x >= SLICERRET) x = SLICERRET;

    x = slicerseq[x] - 1;
    XCO = blockxco;
    YCO = Ay;

    if (slicerfrnt[x] == 0) return;

    IMAGE = slicerfrnt[x];
    maddfore();
}

/*-------------------------------
*
*  D R A W   F R O N T
*
*-------------------------------*/
void drawfrnt() {
    if (PRECED == GATE) {
        DrawGateBF();
        return;
    }

    if (objid == SLICER) {
        drawslicerf();
        return;
    }

    if (objid == FLASK) {
        uint8_t state_masked = state & 0xE0;  // Masque %11100000
        if (state_masked == 0xA0 || state_masked < 0x40) {
            goto front_check;
        }
        IMAGE = SPECIAL_FLASK;
        goto set_position;
    }

    front_check:
        if (fronti[objid] == 0) {
            return;
        }

    IMAGE = fronti[objid];

    set_position:
        YCO = Ay + fronty[objid];
    XCO = blockxco + frontx[objid];

    if (objid >= ARCHTOP2) {
        OPACITY = 1;  // Simule `sta`
        addfore();
        return;
    }

    // Vérification des backgrounds spéciaux (dans l'éditeur)
    if (OPACITY == 2) {  // Simule la vérification `EditorDisk`
        goto normal_case;
    }

    // Gestion du background dungeon
    if (objid == POSTS) {
        goto normal_case;
    }

    if (objid == BLOCK) {
        goto handle_block;
    }

    normal_case:
        maddfore();
    return;

    handle_block:
        block_index = state;
    if (block_index >= numblox) {
        block_index = 0;
    }
    IMAGE = blockfr[block_index];

    OPACITY = 1;  // Simule `sta`
    addfore();
}

void setmid() {
    add = addmidezfast;  // Met à jour le pointeur de fonction
}

/*-------------------------------
* Draw Falling Floor
*-------------------------------*/
void DrawFF() {
    FCharFace = -1;  // Orientation normale

    FCharImage = IMAGE;  // Stocker temporairement `IMAGE` (mobframe)

    // **Section A**
    YCO = FCharY - 3;  // Ajustement en hauteur

    IMAGE = maska[FLOOR];  // Masque de la section A
    OPACITY = AND_OPACITY;
    IMAGE = USELAYRSAVE;
    addmid();

    IMAGE = loosea[FCharImage];  // Partie visible de la section A
    OPACITY = ORA_OPACITY;
    IMAGE = USELAY;
    addmid();

    // **Section D**
    IMAGE = loosed[FCharImage];  // Section D
    YCO = FCharY;  // Positionnement en Y
    OPACITY = STA_OPACITY;
    IMAGE = USELAYRSAVE;
    addmid();

    // **Section B**
    XCO = FCharX + 4;  // Ajustement horizontal

    YCO = FCharY - 4;  // Ajustement vertical

    IMAGE = LOOSEB;  // Section B
    OPACITY = ORA_OPACITY;
    IMAGE = USELAYRSAVE;
    addmid();
}

/*-------------------------------
*
*  Draw object #x
*  (Add appropriate images to mid table)
*
*  In: x = object table index
*
*-------------------------------*/
void drawobjx(uint8_t objIndex) {
    loadobj(objIndex);  // Charge les données de l’objet

    uint8_t type = objTYP[objIndex];

    switch (type) {
    case TYPE_KID:
    case TYPE_REFLECT:
        DrawKid();
        break;
    case TYPE_SHAD:
        DrawShad();
        break;
    case TYPE_FF:
        DrawFF();
        break;
    case TYPE_SWORD:
    case TYPE_COMIX:
        DrawSword();
        break;
    case TYPE_GD:
        DrawGuard();
        break;
    default:
        return;  // Aucune action pour les autres types
    }
}

/*-------------------------------
*  Draw slicer A
*-------------------------------*/
void drawslicera() {
    uint8_t x = state & 0x7F;
    if (x >= SLICERRET) x = SLICERRET;

    x = slicerseq[x] - 1;
    XCO = blockxco;
    YCO = Ay;

    if (state & 0x80) {
        if (slicerbot2[x] != 0) {
            IMAGE = slicerbot2[x];
            OPACITY = ORA;
            add();
        }
    } else {
        if (slicerbot[x] != 0) {
            IMAGE = slicerbot[x];
            OPACITY = ORA;
            add();
        }
    }

    if (slicertop[x] == 0) return;

    IMAGE = slicertop[x];
    YCO = Ay - slicergap[x];
    OPACITY = ORA;

    add();
}

/*-------------------------------
*
*  Draw C-section of piece below & to left
*
*-------------------------------*/
void dodrawc() {
    uint8_t below_obj = BELOW[colno];

    if (below_obj == BLOCK) {
        block_index = SBELOW[colno];
        if (block_index >= numblox) {
            block_index = 0;
        }
        IMAGE = blockc[block_index];
        if (IMAGE == 0) return;  // Pas d'image pour ce bloc
    } else {
        IMAGE = piecec[below_obj];
        if (IMAGE == 0) return;  // L'objet n'a pas de section C

        if (IMAGE == PANELC0) {
            uint8_t panel_index = SBELOW[colno];
            if (panel_index >= numpans) return;  // Trop grand, on sort
            IMAGE = panelc[panel_index];
            if (IMAGE == 0) return;  // Pas d'image pour ce panneau
        }
    }

    XCO = blockxco;
    YCO = Dy;
    OPACITY = ORA;

    add();
}

/*-------------------------------
*
*  Mask B-section of piece to left
*
*-------------------------------*/
void domaskb() {
    IMAGE = maskb[PRECED];

    if (IMAGE == 0) return;  // Pas de masque, on quitte

    YCO = Dy;
    OPACITY = AND;

    add();
}

/*-------------------------------
*
*  Draw objects
*
*  Draw object/s with index # = yindex
*  (Add appropriate images to mid list)
*
*-------------------------------*/
void drawobjs() {
    // **Vérifier si la liste des objets est vide**
    if (objX == 0) {
        return;  // Aucun objet à dessiner
    }

    uint8_t sortCount = 0; // Compteur d'objets triés

    // **Parcourir la liste des objets et rechercher ceux ayant objINDX == yindex**
    for (uint8_t i = 1; i < objX; i++) {
        if (objINDX[i] == yindex) {
            sortX[sortCount++] = i;  // Ajouter l'index de l'objet à la liste triée
        }
    }

    // **Si aucun objet correspondant, sortir**
    if (sortCount == 0) {
        return;
    }

    sortX[0] = sortCount;  // Stocke le nombre d'objets trouvés en première position

    // **Trier les objets du fond vers le premier plan**
    sortlist();

    // **Dessiner les objets triés**
    for (uint8_t i = 1; i <= sortCount; i++) {
        xsave = i;                // Sauvegarde de l'index
        drawobjx(sortX[i]);       // Dessine l'objet correspondant
    }
}

/*-------------------------------
*
*  Draw B-section of piece to left
*
*-------------------------------*/
void drawb() {
    // Si `objid` est un bloc, la section B est cachée
    if (objid == BLOCK) return;

    // Gestion des cas spéciaux avec `PRECED`
    switch (PRECED) {
    case SPACE:  // Section B d’un espace
        if (spreced >= numpans + 1) return;
        IMAGE = spaceb[spreced];
        if (IMAGE == 0) return;
        YCO = Ay + spaceby[spreced];
        goto draw_standard;

    case FLOOR:  // Section B d’un sol
        if (spreced >= numpans + 1) spreced = 0;
        IMAGE = floorb[spreced];
        if (IMAGE == 0) return;
        YCO = Ay + floorby[spreced];
        goto draw_standard;

    case BLOCK:  // Section B d’un bloc
        if (spreced >= numblox) spreced = 0;
        IMAGE = blockb[spreced];
        if (IMAGE == 0) return;
        goto draw_standard;
    }

    // Récupérer l’image de la section B normale
    IMAGE = pieceb[PRECED];
    if (IMAGE == 0) goto check_stripe;  // Si pas d'image, vérifier si on doit ajouter une rayure

    if (IMAGE == PANELB0) {  // Gestion spéciale des panneaux
        if (spreced >= numpans) return;
        IMAGE = panelb[spreced];
        if (IMAGE == 0) return;
    }

    draw_standard:
        XCO = blockxco;
    OPACITY = ORA;
    add();
    return;

    check_stripe:
        if (bstripe[PRECED] == 0) return;  // Vérifier si une rayure est nécessaire
    IMAGE = bstripe[PRECED];
    YCO = Ay - 32;  // Ajustement Y de la rayure
    goto draw_standard;
}

/*-------------------------------
*
* Get loose floor index
*
* In: A = state
* Out: Y = index
*
*-------------------------------*/
uint8_t getloosey(uint8_t stat) {
    if (inbuilder) return 1;

    y = stat;

    if (stat & 0x80) {  // Si le bit de poids fort est actif
        y &= 0x7F;  // On le masque
        if (y >= FFALLING + 1) y = 1;  // Vérification du seuil
    }

    return y;
}

/*-------------------------------
*
*  Draw D-section
*
*-------------------------------*/
uint8_t drawd() {
    OPACITY = STA;

    if (objid == BLOCK) {
        block_index = (state < numblox) ? state : 0;
        IMAGE = blockd[block_index];
    } else {
        if (objid == PANELWOF) {
            OPACITY = ORA;  // Appliquer un masque
        }
        IMAGE = pieced[objid];
    }

    if (IMAGE == 0) return 0;  // Pas d’image → on sort

    XCO = blockxco;
    YCO = Dy;
    add();

    return 1;  // IMAGE a été modifiée
}

void redrawd() {
    if (!drawd()) return;  // Si `drawd()` ne modifie pas IMAGE, on sort
    addfore();
}

/*-------------------------------
*
* Get piece "A"
*
* In: state; X = objid
* Out: A = A-section image #
*
*-------------------------------*/
uint8_t getpiecea(uint8_t obj) {
    if (obj == LOOSE) {
        y = getloosey(state);
        return loosea[y];
    }
    return piecea[obj];
}

void adda1() {
    // **Affecter l’image stockée dans IMAGE**
    XCO = blockxco;  // Position X
    YCO = Ay + pieceay[objid];  // Ajustement de la position Y

    // **Définir l’opacité et ajouter l’image**
    OPACITY = ORA;
    add();
}

void adda() {
    IMAGE = getpiecea(objid);  // Récupérer l’image de l’objet actuel

    if (IMAGE == 0) {
        return;  // Rien à afficher, on quitte
    }

    adda1();  // Afficher l’image
}

/*-------------------------------
*
*  D R A W   ' A '
*
*  (1) If piece to left has intrusive B-section (e.g., panel):
*      MASK A-section
*  (2) OR A-section
*
*-------------------------------*/
void drawa() {
    switch (PRECED) {
    case ARCHTOP1:
        if (objid == PANELWOF) {
            IMAGE = ARCHPANEL;
            adda1();
            return;
        }
        break;

    case PANELWIF:
    case PANELWOF:
    case PILLARTOP:
    case BLOCK:
        addamask();
        break;
    }

    adda();
}

/*-------------------------------
*  Draw spikes A
*-------------------------------*/
void drawspikea() {
    uint8_t x = (state & 0x80) ? SPIKEEXT : state;  // Vérifie si state est négatif

    IMAGE = spikea[x];
    if (IMAGE == 0) return;  // Pas d’image, on quitte

    XCO = blockxco;
    YCO = Ay - 1;  // Décalage de -1 pixel en Y
    OPACITY = ORA;

    add();
}

void drawsworda() {
    // Sélectionner l’image en fonction de l’état**
    IMAGE = (state == 1) ? SWORDGLEAM1 : SWORDGLEAM0;

    // Positionner l'épée**
    XCO = blockxco;  // Position horizontale
    YCO = Ay;        // Position verticale

    // Définir l’opacité et ajouter l’image**
    OPACITY = STA;
    add();
}

/*-------------------------------
*
*  D R A W   M O V A B L E   ' A '
*
*-------------------------------*/
void drawma() {
    switch (objid) {
    case SPIKES:
        drawspikea();
        return;
    case SLICER:
        drawslicera();
        return;
    case FLASK:
        drawflaska();
        return;
    case SWORD:
        drawsworda();
        return;
    default:
        return;  // Aucun objet spécial
    }
}

/*-------------------------------
*
*  D R A W   F L O O R
*
*-------------------------------*/
void drawfloor() {
    if (PRECED != 0) return;  // Si le bloc à gauche n'est pas vide, on quitte

    addamask();
    adda();
    drawma();
    drawd();
}

/*-------------------------------
*
*  D R A W   H A L F
*
*  Special version of "drawfloor" for climbup
*
*-------------------------------*/
void drawhalf() {
    if (PRECED != 0) return;  // L'espace à gauche doit être vide

    switch (objid) {
    case FLOOR:
    case TORCH:
    case DPRESSPLATE:
    case EXIT:
        IMAGE = CUPIECE;
        break;

    default:
        if (BGset1 != 1) {
            drawfloor();
            return;
        }
        if (objid == POSTS) {
            IMAGE = CUPOST;
        } else if (objid == ARCHBOT) {
            drawfloor();
            return;
        } else {
            return;
        }
    }

    OPACITY = ORA;
    add();

    // Ajout du masque
    IMAGE = CUMASK;
    XCO = blockxco;
    YCO = Ay;

    if (objid == DPRESSPLATE) {
        YCO++;  // Ajustement spécial pour `dpressplate`
    }

    OPACITY = AND;
    add();
}

/*-------------------------------
*
*  S H O R T   W I P E
*
*  In: Y = buffer index
*
*-------------------------------*/
void wipesq() {
    height = whitebuf[yindex];  // Récupère la hauteur d'effacement depuis `whitebuf`
    width = 4;                  // Largeur d'effacement fixe

    XCO = blockxco;             // Définit la position X
    YCO = Dy;                   // Définit la position Y

    addwipe();                  // Applique l'effacement
}

/*-------------------------------
*
* Wipe D-section
*
*-------------------------------*/
void wiped() {
    if (objid == PILLARTOP || objid == PANELWIF || objid == PANELWOF || objid == BLOCK) {
        return;  // Ces objets ne doivent pas être effacés
    }
    wipe(3);  // Effacer une hauteur de 3 pixels
}

/*-------------------------------
*  Draw spikes B
*-------------------------------*/
void drawspikeb() {
    uint8_t x = (spreced & 0x80) ? SPIKEEXT : spreced;

    IMAGE = spikeb[x];
    if (IMAGE == 0) return;

    XCO = blockxco;
    YCO = Ay - 1;
    OPACITY = ORA;

    add();
}

/*-------------------------------
*  Draw torch B (flame)
*-------------------------------*/
void drawtorchb() {
    if (inbuilder) return;  // Pas de flammes en mode éditeur
    if (blockxco == 0) return;  // Pas de flamme pour la torche de gauche

    XCO = blockxco;
    YCO = Ay;

    setupflame(spreced);  // Positionne la flamme
    addback();  // Ajoute la flamme en arrière-plan
}

/*-------------------------------
*  D R A W  L O O S E  F L O O R  " D "
*-------------------------------*/
void drawloosed() {
    y = getloosey(state);  // Récupérer l’index correct
    IMAGE = loosed[y];

    if (IMAGE == 0) return;  // Pas d’image, on quitte

    XCO = blockxco;
    YCO = Dy;
    OPACITY = STA;

    add();
}

/*-------------------------------
*  D R A W  L O O S E  F L O O R  " B "
*-------------------------------*/
void drawlooseb() {
    y = getloosey(spreced);
    IMAGE = looseb;  // Valeur fixe pour l’image de la section B
    YCO = Ay + looseby[y];  // Ajustement vertical

    OPACITY = ORA;
    add();
}

/*-------------------------------
* Draw exit "b" (stairs)
*-------------------------------*/
void drawexitb() {
    // **Dessiner l'escalier**
    IMAGE = STAIRS;
    YCO = Ay - 12;  // Décalage en hauteur

    if (blockxco >= 36) return;  // L'escalier ne doit pas dépasser à droite

    XCO = blockxco + 1;
    OPACITY = STA_OPACITY;

    if (SCRNUM != KidStartScrn) {
        add();  // Affichage des escaliers si ce n'est pas l'écran de départ du Kid
    }

    // **Dessiner la porte de bas en haut**
    blockthr = Dy - 67;
    if (blockthr >= 192) return;  // Vérification de la hauteur maximale

    gateposn = spreced / 4;  // Position de la porte basée sur `spreced`

    doortop = Ay - 14 - gateposn;  // Calcul de la hauteur de la porte

    while (1) {
        YCO = doortop;

        // Masque de la porte
        IMAGE = DOORMASK;
        OPACITY = AND_OPACITY;
        add();

        // Affichage de la porte
        IMAGE = DOOR;
        OPACITY = ORA_OPACITY;
        add();

        // Passage à la ligne suivante
        doortop -= 4;
        if (doortop < blockthr) break;
    }

    // **Réparation du sommet de la porte**
    YCO = Ay - 64;  // Ajustement de la position en hauteur
    if (YCO >= 192) return;  // Si trop haut, ne pas afficher

    IMAGE = TOPREPAIR;
    OPACITY = STA_OPACITY;
    add();
}

/*-------------------------------
*  D R A W   G A T E   " C "
*-------------------------------*/
void drawgatec() {
    YCO = Dy;  // Positionnement en Y
    IMAGE = GATEMASK;
    OPACITY = AND_OPACITY;
    add();  // Application du masque triangulaire

    // Récupération de l’état de la porte
    state = SBELOW[colno];

    if (state > GMAXVAL) {
        state = GMAXVAL;
    }

    gateposn = state / 4;  // Équivalent de `lsr lsr`

    y = (gateposn & 0xF8) ^ 0xFF;  // Extraire les 3 bits de poids faible et les inverser
    y = y + 1 + gateposn;  // Ajustement final

    IMAGE = gate8c[y];  // Sélection de l’image
    OPACITY = ORA_OPACITY;
    add();
}

/*-------------------------------
*
*  D R A W  M O V A B L E  ' C '
*
*-------------------------------*/
void drawmc() {
    // Vérification des objets qui ne cachent pas la section C
    if (objid == SPACE || objid == PANELWOF || objid == PILLARTOP) {
        uint8_t below_obj = BELOW[colno];  // Récupération de l'objet en dessous

        if (below_obj == GATE) {
            drawgatec();  // Dessiner la section C de la porte
        }
    }
}

/*-------------------------------
*
* D R A W   M O V A B L E  ' D '
*
*-------------------------------*/
void drawmd() {
    if (objid == LOOSE) {
        drawloosed();
    }
}

void restorebot() {
    // **Restaurer l’arrière-plan de la porte**
    IMAGE = pieceb[GATE];  // Récupère l’image de la section B de la porte
    YCO = pieceby[GATE] + Ay;  // Position en Y ajustée
    XCO = blockxco;
    OPACITY = STA_OPACITY;

    add();  // Affichage

    // **Vérification de la section C**
    if (checkc()) {
        dodrawc();  // Redessiner la section C si elle est visible
    }

    drawa();  // Dessiner la section A de la porte
}

void drawgateb() {
    setupdgb();  // Configuration de la porte

    uint8_t y_pos = gatebot + 12;

    // **Si la porte est en dessous de `Ay`, il faut restaurer l'arrière-plan**
    if (y_pos >= Ay) {
        restorebot();

        YCO = gatebot - 2;  // Décalage pour éviter les lignes blanches en bas
        IMAGE = GATEBOT_ORA;
        OPACITY = ORA_OPACITY;
        addback();
    } else {
        // **Sinon, on peut directement afficher la porte**
        YCO = gatebot;
        IMAGE = GATEBOT_STA;
        OPACITY = STA_OPACITY;
        addback();
    }

    // **Affichage des pièces intermédiaires**
    OPACITY = STA_OPACITY;
    IMAGE = GATEB1;
    YCO = gatebot - 12;

    while (1) {
        if (YCO >= MAX_HEIGHT) return;

        YCO -= 7;
        if (YCO < blockthr) break;

        addback();
        YCO -= 8;
    }

    // **Ajout de la pièce finale en haut**
    uint8_t final_height = (YCO - blockthr) + 1;

    if (final_height == 0 || final_height >= 9) return;

    IMAGE = gate8b[final_height - 1];
    addback();
}

/*-------------------------------
*
*  Draw C-section (if visible)
*
*-------------------------------*/
void drawc() {
    if (!checkc()) {  // checkc() retourne 0 si la section C est cachée
        return;       // Si la section C n'est pas visible, on quitte
    }

    dodrawc();  // Dessiner la section C de la pièce en bas à gauche
    domaskb();  // Appliquer un masque sur la section B de la pièce à gauche
}

/*-------------------------------
*
*  D R A W   M O V A B L E   ' B '
*
*-------------------------------*/
void drawmb() {
    switch (PRECED) {
    case GATE:
        drawgateb();
        return;
    case SPIKES:
        drawspikeb();
        return;
    case LOOSE:
        drawlooseb();
        return;
    case TORCH:
        drawtorchb();
        return;
    case EXIT:
        drawexitb();
        return;
    default:
        return;  // Aucun cas spécial, fin de la fonction
    }
}

/*-------------------------------
*
*  Redraw entire block
*
*-------------------------------*/
void RedBlockSure() {
    // **Dessiner chaque section du bloc**
    drawc();   // C-section (en bas à gauche)
    drawmc();  // Partie mobile de la section C

    drawb();   // B-section (à gauche)
    drawmb();  // Partie mobile de la section B

    drawd();   // D-section (dessus)
    drawmd();  // Partie mobile de la section D

    drawa();   // A-section (devant)
    drawma();  // Partie mobile de la section A

    drawfrnt(); // Dernier plan (superposition)
}

/*-------------------------------
*
* Redraw entire D-section
*
*-------------------------------*/
void RedDSure() {
    // **Dessiner les différentes sections**
    drawc();   // C-section (en bas à gauche)
    drawmc();  // Partie mobile de la section C

    drawb();   // B-section (à gauche)
    drawd();   // D-section (dessus)
    drawmd();  // Partie mobile de la section D

    drawfrnt(); // Dernier plan (superposition)
}

/*-------------------------------
*
*  Partial D-section redraw
*
*-------------------------------*/
void RedDFast() {
    // **Vérifier si la colonne actuelle doit être redessinée**
    if (topbuf[colno] == 0) {
        return;  // Si `topbuf[colno]` est 0, on ne redessine pas cette section
    }

    topbuf[colno]--;  // Décrémente la valeur (équivalent à `sbc #1`)

    // **Redessiner les éléments de la section D**
    wiped();   // Efface la zone avant de redessiner
    drawc();   // C-section (en bas à gauche)
    drawmc();  // Partie mobile de la section C
    drawb();   // B-section (à gauche)
    redrawd(); // Redessiner la section D (inclut les objets de premier plan et de fond)
    drawmd();  // Partie mobile de la section D
    drawfrnt(); // Dernier plan (superposition)
}

/*-------------------------------
*
*  Partial block redraw (as specified by redraw buffers)
*
*-------------------------------*/
void RedBlockFast() {
    // **Vérification et mise à jour de `wipebuf`**
    if (wipebuf[yindex] > 0) {
        wipebuf[yindex]--;
        wipesq();  // Efface ce bloc
    }

    // **Vérification et mise à jour de `redbuf`**
    if (redbuf[yindex] > 0) {
        redbuf[yindex]--;
        setback();
        RedBlockSure();
    }

    // **Vérification et mise à jour de `movebuf`**
    if (movebuf[yindex] > 0) {
        movebuf[yindex]--;
        setback();
        drawmc();
        drawmb();
        drawma();
    }

    // **Vérification et mise à jour de `floorbuf`**
    if (floorbuf[yindex] > 0) {
        floorbuf[yindex]--;
        setmid();
        drawfloor();
    }

    // **Vérification et mise à jour de `halfbuf`**
    if (halfbuf[yindex] > 0) {
        halfbuf[yindex]--;
        setmid();
        drawhalf();
    }

    // **Vérification et mise à jour de `objbuf`**
    if (objbuf[yindex] > 0) {
        objbuf[yindex] = 0;  // Réinitialise l’état
        drawobjs();           // Dessine tous les objets
        XCO = blockxco;       // Met à jour la position X
    }

    // **Vérification et mise à jour de `fredbuf`**
    if (fredbuf[yindex] > 0) {
        fredbuf[yindex]--;
        drawfrnt();
    }
}


// *********************** Fonctions principales ***********************

/*-------------------------------
*
*  Draw entire 10 x 3 screen from scratch
*
*-------------------------------*/
void SURE() {
    genCLS = 1; // Effacer l’écran

    setback();  // Dessiner l’arrière-plan
    getprev();  // Récupérer les 3 blocs les plus à droite de l’écran de gauche

    SCRNUM = calcblue();  // Calculer l’adresse mémoire de l’écran

    // **Dessiner les 3 lignes de 10 blocs (de gauche à droite, de haut en bas)**
    for (rowno = 2; rowno >= 0; rowno--) {
        Dy = BlockBot[rowno + 1]; // Coordonnée Y de la section D
        Ay = Dy - 3;              // Coordonnée Y de la section A

        yindex = Mult10[rowno];   // Numéro de bloc (0-29)
        PRECED = PREV[rowno];
        spreced = sprev[rowno];   // Récupérer les infos du bloc précédent

        getbelow(); // Récupérer les 10 blocs du dessous

        // **Parcourir chaque colonne de la ligne**
        for (colno = 0; colno < 10; colno++) {
            XCO = blockxco = colno * 4; // Calculer la position X

            objid = getobjid(); // Récupérer l'ID de l'objet
            RedBlockSure();     // Redessiner le bloc entier

            PRECED = objid;
            spreced = state; // Passer au bloc suivant

            yindex++; // Incrémenter le compteur de blocs
        }
    }

    // **Dessiner la ligne du bas de l’écran au-dessus (sections D uniquement)**
    rowno = 2;
    Dy = 2;
    Ay = -1;
    yindex = Mult10[rowno];
    PRECED = 0;

    // **Sauvegarder les valeurs de l’écran en dessous**
    uint8_t tempScrnBelow = scrnBelow;
    uint8_t tempScrnBelowL = scrnBelowL;

    scrnBelow = SCRNUM;
    scrnBelowL = scrnLeft; // Simuler qu'on est sur l'écran au-dessus

    getbelow();
    scrnAbove = calcblue();

    // **Dessiner les 10 blocs de gauche à droite**
    for (colno = 0; colno < 10; colno++) {
        XCO = blockxco = colno * 4;

        if (scrnAbove == 0) {
            objid = FLOOR; // Si l'écran au-dessus est nul, on dessine un sol plein
        } else {
            objid = getobjid();
        }

        RedDSure(); // Dessiner uniquement la section D

        PRECED = objid;
        spreced = state;

        yindex++; // Incrémenter le compteur de blocs
    }

    // **Restaurer les valeurs originales de l’écran en dessous**
    scrnBelowL = tempScrnBelowL;
    scrnBelow = tempScrnBelow;
} // end SURE

/*-------------------------------
*
*  Fast screen redraw
*
*  Same general structure as SURE, but redraws only those
*  blocks specified by redraw buffers.
*
*-------------------------------*/
void FAST() {
    getprev();  // Récupérer les 3 blocs les plus à droite de l’écran de gauche

    SCRNUM = calcblue();  // Calculer l’adresse mémoire de l’écran

    // **Vérifier si les barres de vie doivent être redessinées**
    redkidmeter = metbufs3(20, 0);
    redoppmeter = metbufs2(28, 0);

    // **Dessiner les personnages hors écran en premier**
    yindex = 30;
    drawobjs();

    // **Dessiner les 3 lignes de 10 blocs**
    for (rowno = 2; rowno >= 0; rowno--) {
        Dy = BlockBot[rowno + 1]; // Coordonnée Y de la section D
        Ay = Dy - 3;              // Coordonnée Y de la section A

        yindex = Mult10[rowno];   // Numéro de bloc (0-29)
        PRECED = PREV[rowno];
        spreced = sprev[rowno];   // Récupérer les infos du bloc précédent

        getbelow(); // Récupérer les 10 blocs du dessous

        // **Parcourir chaque colonne de la ligne**
        for (colno = 0; colno < 10; colno++) {
            XCO = blockxco = colno * 4; // Calculer la position X

            objid = getobjid(); // Récupérer l'ID de l'objet
            RedBlockFast();     // Redessiner le bloc seulement si nécessaire

            PRECED = objid;
            spreced = state; // Passer au bloc suivant

            yindex++; // Incrémenter le compteur de blocs
        }
    }

    // **Dessiner la ligne du bas de l’écran au-dessus (sections D uniquement)**
    setback(); // Mettre à jour l'arrière-plan

    rowno = 2;
    Dy = 2;
    Ay = -1;
    yindex = Mult10[rowno];
    PRECED = 0;

    // **Sauvegarder les valeurs de l’écran en dessous**
    uint8_t tempScrnBelow = scrnBelow;
    uint8_t tempScrnBelowL = scrnBelowL;

    scrnBelow = SCRNUM;
    scrnBelowL = scrnLeft; // Simuler qu'on est sur l'écran au-dessus

    getbelow();
    if (scrnAbove != 0) {
        scrnAbove = calcblue();

        // **Dessiner les 10 blocs de gauche à droite**
        for (colno = 0; colno < 10; colno++) {
            XCO = blockxco = colno * 4;
            objid = getobjid();
            RedDFast(); // Dessiner uniquement la section D

            PRECED = objid;
            spreced = state;

            yindex++; // Incrémenter le compteur de blocs
        }
    }

    // **Restaurer les valeurs originales de l’écran en dessous**
    scrnBelowL = tempScrnBelowL;
    scrnBelow = tempScrnBelow;

    // **Dessiner les effets spéciaux (étoiles de choc, etc.)**
    yindex = 0xFF;
    drawobjs();

    // **Si en mode éditeur, ne pas mettre à jour les barres de vie**
    if (inbuilder != 0) {
        return;
    }

    // **Mettre à jour les barres de vie**
    updatemeters();
} // end FAST


uint8_t GETINITOBJ() {
    objid = BlueType[y] & IDMASK;  // Extraction de l'ID de l'objet

    uint8_t spec = BlueSpec[y];  // Récupération de la valeur de spécification

    switch (objid) {
    case GATE:  // Cas des portes
        return (spec >= GMINVAL && spec <= GMAXVAL) ? (spec * 4) : 0;
        // `spec * 4` simule un comportement basé sur `GMAXVAL`

    case LOOSE: // Sol fragile
        return 0;  // Aucun état initial spécifique

    case FLASK: // Flasque (valeur multipliée par 32)
        return spec << 5;  // `asl` * 5 = Multiplication par 32

    default: // Si ce n'est pas un gadget
        return 0;
    }
} // end GETINITOBJ


// *********************** Point d'entrée du programme pour tester ***********************

/*
int main(int argc, char* argv[]) {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    // Création de la fenêtre SDL
    SDL_Window* window = SDL_CreateWindow(
        "Editeur de niveau",       // Titre de la fenêtre
        SDL_WINDOWPOS_CENTERED,    // Position X
        SDL_WINDOWPOS_CENTERED,    // Position Y
        800,                       // Largeur
        600,                       // Hauteur
        SDL_WINDOW_SHOWN           // Flags
    );

    if (window == NULL) {
        printf("Erreur de création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Création du renderer SDL
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Erreur de création du renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Boucle principale
    bool running = true;
    SDL_Event event;
    while (running) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir
        SDL_RenderClear(renderer);

        // Dessiner ici (à compléter avec la logique du jeu)

        // Mettre à jour l'écran
        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}*/
